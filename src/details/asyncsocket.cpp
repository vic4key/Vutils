/**
 * @file   asyncsocket.cpp
 * @author Vic P.
 * @brief  Implementation for Asynchronous Socket
 */

#include "Vutils.h"

#include <vector>
#include <utility>

namespace vu
{

#ifdef VU_INET_ENABLED

AsyncSocket::AsyncSocket(
  const vu::Socket::address_family_t af,
  const vu::Socket::type_t type,
  const vu::Socket::protocol_t proto,
  const vu::Socket::Options* options
) : m_socket(af, type, proto, options), m_thread(INVALID_HANDLE_VALUE), LastError()
{
  this->initialze();

  for (uint i = 0; i < function::UNDEFINED; i++)
  {
    m_functions[i] = nullptr;
  }
}

AsyncSocket::~AsyncSocket()
{
}

void vuapi AsyncSocket::initialze()
{
  m_n_events = 0;

  //std::lock_guard<std::recursive_mutex> lg(m_mutex_client_list);

  memset(m_connections, int(INVALID_SOCKET), sizeof(m_connections));
  memset(m_events, int(0), sizeof(m_events));

  m_running = false;
}


Socket::side_type vuapi AsyncSocket::side() const
{
  return m_socket.side();
}

bool vuapi AsyncSocket::available() const
{
  return m_socket.available();
}

bool vuapi AsyncSocket::running() const
{
  return m_running;
}

VUResult vuapi AsyncSocket::bind(const Endpoint& endpoint)
{
  return this->bind(endpoint.m_host, endpoint.m_port);
}

VUResult vuapi AsyncSocket::bind(const std::string& address, const ushort port)
{
  auto result = m_socket.bind(address, port);
  this->set_last_error_code(m_socket.get_last_error_code());
  return result;
}

VUResult vuapi AsyncSocket::listen(const int maxcon)
{
  if (!m_socket.available())
  {
    return 1;
  }

  this->initialze();

  WSAEVENT event = WSACreateEvent();
  if (WSAEventSelect(m_socket.handle(), event, FD_ALL_EVENTS) == SOCKET_ERROR) // ACCEPT | FD_CLOSE
  {
    m_last_error_code = GetLastError();
    return 2;
  }

  //std::lock_guard<std::recursive_mutex> lg(m_mutex_client_list);

  m_connections[m_n_events] = m_socket.handle();
  m_events[m_n_events] = event;
  m_n_events++;

  auto result = m_socket.listen(maxcon);

  m_last_error_code = GetLastError();

  return result;
}

IResult vuapi AsyncSocket::stop(const Socket::shutdowns_t flags, const bool cleanup)
{
  if (!m_socket.available())
  {
    return 1;
  }

  m_running = false;

  this->disconnect_connections(flags, cleanup);

  if (m_thread != INVALID_HANDLE_VALUE)
  {
    TerminateThread(m_thread, 0); // CloseHandle(m_thread);
  }

  return VU_OK;
}

VUResult vuapi AsyncSocket::connect(const Endpoint& endpoint)
{
  if (!m_socket.available())
  {
    return 1;
  }

  this->initialze();

  WSAEVENT event = WSACreateEvent();
  if (WSAEventSelect(m_socket.handle(), event, FD_ALL_EVENTS) == SOCKET_ERROR)
  {
    m_last_error_code = GetLastError();
    return 2;
  }

  //std::lock_guard<std::recursive_mutex> lg(m_mutex_client_list);

  auto result = m_socket.connect(endpoint);
  if (result == VU_OK)
  {
    m_last_error_code = ERROR_SUCCESS;
    m_connections[m_n_events] = m_socket.handle();
    m_events[m_n_events] = event;
    m_n_events++;
  }

  this->set_last_error_code(m_socket.get_last_error_code());

  return result;
}

VUResult vuapi AsyncSocket::connect(const std::string& address, const ushort port)
{
  Endpoint endpoint(address, port);
  return this->connect(endpoint);
}

void vuapi AsyncSocket::get_connections(std::set<SOCKET>& connections)
{
  connections.clear();

  if (!m_socket.available())
  {
    return;
  }

  //std::lock_guard<std::recursive_mutex> lg(m_mutex_client_list);

  for (auto& socket : m_connections)
  {
    if (socket == INVALID_SOCKET) // ignore invalid socket handle
    {
      continue;
    }

    if (m_socket.side() == Socket::side_type::SERVER && socket == m_socket.handle()) // ignore server socket handle
    {
      continue;
    }

    connections.insert(socket);
  }
}

VUResult vuapi AsyncSocket::disconnect_connections(const Socket::shutdowns_t flags, const bool cleanup)
{
  //std::lock_guard<std::recursive_mutex> lg(m_mutex_client_list);

  std::set<SOCKET> connections;
  this->get_connections(connections);
  for (const auto& connection : connections)
  {
    Socket socket(m_socket);
    socket.attach(connection);
    socket.disconnect(flags, cleanup);
  }

  return VU_OK;
}

static DWORD WINAPI AsyncSocket_Threading(LPVOID lpParam)
{
  auto ptr = reinterpret_cast<vu::AsyncSocket*>(lpParam);
  assert(ptr != nullptr);

  ptr->run(false);

  return 0;
}

VUResult vuapi AsyncSocket::run(const bool in_worker_thread)
{
  if (in_worker_thread)
  {
    m_thread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(AsyncSocket_Threading), this, 0, nullptr);
    m_last_error_code = GetLastError();
    return m_thread != INVALID_HANDLE_VALUE ? VU_OK : 1;
  }
  else
  {
    return this->run_loop();
  }
}

VUResult vuapi AsyncSocket::run_loop()
{
  if (!m_socket.available())
  {
    return 1;
  }

  VUResult result = VU_OK;

  if (m_running)
  {
    return result;
  }

  m_running = true;

  while (m_running)
  {
    if (this->loop() != VU_OK)
    {
      break;
    }

    Sleep(1);
  }

  return VU_OK;
}

VUResult vuapi AsyncSocket::loop()
{
  VUResult result = VU_OK;

  // TODO: Vic. Recheck. FALSE, WSA_INFINITE, FALSE
  DWORD idx = WSAWaitForMultipleEvents(m_n_events, m_events, FALSE, 0, FALSE);
  idx -= WSA_WAIT_EVENT_0;

  for (DWORD i = idx; i < m_n_events; i++)
  {
    // TODO: Vic. Recheck. TRUE, 1000, FALSE
    idx = WSAWaitForMultipleEvents(1, &m_events[i], FALSE, 0, FALSE);
    if (idx == WSA_WAIT_FAILED || idx == WSA_WAIT_TIMEOUT)
    {
      continue;
    }

    idx = i;

    auto connection = m_connections[idx];
    auto event  = m_events[idx];

    WSANETWORKEVENTS events = { 0 };
    WSAEnumNetworkEvents(connection, event, &events);

    if (events.lNetworkEvents & FD_CONNECT)
    {
      result = this->do_connect(events, connection);
      if (result != VU_OK)
      {
        break;
      }
    }

    if (events.lNetworkEvents & FD_ACCEPT)
    {
      result = this->do_open(events, connection);
      if (result != VU_OK)
      {
        break;
      }
    }

    if (events.lNetworkEvents & FD_READ)
    {
      result = this->do_recv(events, connection);
      if (result != VU_OK)
      {
        break;
      }
    }

    if (events.lNetworkEvents & FD_WRITE)
    {
      result = this->do_send(events, connection);
      if (result != VU_OK)
      {
        break;
      }
    }

    if (events.lNetworkEvents & FD_CLOSE)
    {
      result = this->do_close(events, connection);
      if (result != VU_OK)
      {
        break;
      }
    }
  }

  return result;
}

IResult vuapi AsyncSocket::do_connect(WSANETWORKEVENTS& events, SOCKET& connection)
{
  if (events.iErrorCode[FD_CONNECT_BIT] != 0)
  {
    return events.iErrorCode[FD_CONNECT_BIT];
  }

  Socket socket(m_socket);
  socket.attach(connection);
  this->on_connect(socket);
  socket.detach();

  return VU_OK;
}

IResult vuapi AsyncSocket::do_open(WSANETWORKEVENTS& events, SOCKET& connection)
{
  if (events.iErrorCode[FD_ACCEPT_BIT] != 0)
  {
    return events.iErrorCode[FD_ACCEPT_BIT];
  }

  //std::lock_guard<std::recursive_mutex> lg(m_mutex_client_list);

  Socket::Handle obj = { 0 };
  int n = static_cast<int>(sizeof(obj.sai));

  obj.s = accept(connection, (struct sockaddr*)&obj.sai, &n);
  if (m_n_events > WSA_MAXIMUM_WAIT_EVENTS)
  {
    ::closesocket(obj.s);
    return WSAEMFILE; // Too many connections
  }

  WSAEVENT event = WSACreateEvent();
  WSAEventSelect(obj.s, event, FD_ALL_EVENTS); // FD_READ | FD_WRITE | FD_CLOSE
  m_events[m_n_events] = event;
  m_connections[m_n_events] = obj.s;
  m_n_events++;

  Socket socket(m_socket);
  socket.attach(obj);
  this->on_open(socket);
  socket.detach();

  return VU_OK;
}

IResult vuapi AsyncSocket::do_recv(WSANETWORKEVENTS& events, SOCKET& connection)
{
  if (events.iErrorCode[FD_READ_BIT] != 0)
  {
    return events.iErrorCode[FD_READ_BIT];
  }

  //std::lock_guard<std::recursive_mutex> lg(m_mutex_client_list);

  Socket socket(m_socket);
  socket.attach(connection);
  this->on_recv(socket);
  socket.detach();

  return VU_OK;
}

IResult vuapi AsyncSocket::do_send(WSANETWORKEVENTS& events, SOCKET& connection)
{
  if (events.iErrorCode[FD_WRITE_BIT] != 0)
  {
    return events.iErrorCode[FD_WRITE_BIT];
  }

  //std::lock_guard<std::recursive_mutex> lg(m_mutex_client_list);

  Socket socket(m_socket);
  socket.attach(connection);
  this->on_send(socket);
  socket.detach();

  return VU_OK;
}

IResult vuapi AsyncSocket::do_close(WSANETWORKEVENTS& events, SOCKET& connection)
{
  // TODO: Vic. In certain cases(e.g., user - mode drivers), it crashes.
  //  I'm not sure why, so temporarily comment out these codes.
  // 
  // if (events.iErrorCode[FD_CLOSE_BIT] != 0)
  // {
  //   return events.iErrorCode[FD_CLOSE_BIT];
  // }

  //std::lock_guard<std::recursive_mutex> lg(m_mutex_client_list);

  std::vector<std::pair<SOCKET, HANDLE>> in_used_connections;

  for (int i = 0; i < WSA_MAXIMUM_WAIT_EVENTS; i++)
  {
    if (m_connections[i] == connection)
    {
      WSACloseEvent(m_events[i]);
      m_events[i] = nullptr;
      m_connections[i] = INVALID_SOCKET;
    }

    if (m_connections[i] != INVALID_SOCKET)
    {
      in_used_connections.emplace_back(std::make_pair(m_connections[i], m_events[i]));
    }
  }

  memset(m_connections, int(INVALID_SOCKET), sizeof(m_connections));
  memset(m_events, int(0), sizeof(m_events));

  m_n_events = 0;
  for (auto& e : in_used_connections)
  {
    m_connections[m_n_events] = e.first;
    m_events[m_n_events]  = e.second;
    m_n_events++;
  }

  Socket socket(m_socket);
  socket.attach(connection);
  this->on_close(socket);
  socket.detach();

  ::closesocket(connection);

  connection = INVALID_SOCKET;

  // CompressArrays(m_Events, m_Sockets, &m_nEvents);

  return VU_OK;
}

void AsyncSocket::on(const function type, const fn_prototype_t fn)
{
  assert(!m_running && type < function::UNDEFINED);
  m_functions[type] = fn;
}

void AsyncSocket::on_connect(Socket& connection)
{
  if (auto& fn = m_functions[function::CONNECT])
  {
    fn(connection);
  }
}

void AsyncSocket::on_open(Socket& connection)
{
  if (auto& fn = m_functions[function::OPEN])
  {
    fn(connection);
  }
}

void AsyncSocket::on_send(Socket& connection)
{
  if (auto& fn = m_functions[function::SEND])
  {
    fn(connection);
  }
}

void AsyncSocket::on_recv(Socket& connection)
{
  if (auto& fn = m_functions[function::RECV])
  {
    fn(connection);
  }
}

void AsyncSocket::on_close(Socket& connection)
{
  if (auto& fn = m_functions[function::CLOSE])
  {
    fn(connection);
  }
}

IResult vuapi AsyncSocket::send(
  const SOCKET& connection,
  const char* ptr_data,
  int size,
  const Socket::flags_t flags)
{
  Socket socket(m_socket);
  socket.attach(connection);
  return socket.send(ptr_data, size, flags);
}

IResult vuapi AsyncSocket::send(
  const SOCKET& connection,
  const Buffer& data,
  const Socket::flags_t flags)
{
  Socket socket(m_socket);
  socket.attach(connection);
  return socket.send(data, flags);
}

#endif // VU_INET_ENABLED

} // namespace vu