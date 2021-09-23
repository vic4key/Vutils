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

#ifdef VU_SOCKET_ENABLED

AsyncSocket::AsyncSocket(
  const vu::Socket::address_family_t af,
  const vu::Socket::type_t type,
  const vu::Socket::protocol_t proto)
  : m_server(af, type, proto)
{
  this->initialze();

  for (uint i = 0; i < eFnType::UNDEFINED; i++)
  {
    m_functions[i] = nullptr;
  }
}

AsyncSocket::~AsyncSocket()
{
  this->close();
}

void vuapi AsyncSocket::initialze()
{
  m_n_events = 0;

  memset(m_sockets, int(INVALID_SOCKET), sizeof(m_sockets));
  memset(m_events, int(0), sizeof(m_events));

  m_running = false;
}

std::set<SOCKET> vuapi AsyncSocket::get_clients()
{
  std::set<SOCKET> result;

  if (m_server.available())
  {
    for (auto& socket : m_sockets)
    {
      if (socket != INVALID_SOCKET && socket != m_server.handle())
      {
        result.insert(socket);
      }
    }
  }

  return result;
}

bool vuapi AsyncSocket::available()
{
  return m_server.available();
}

bool vuapi AsyncSocket::running()
{
  return m_running;
}

VUResult vuapi AsyncSocket::bind(const Socket::sEndPoint& endpoint)
{
  return m_server.bind(endpoint);
}

VUResult vuapi AsyncSocket::bind(const std::string& address, const ushort port)
{
  return m_server.bind(address, port);
}

VUResult vuapi AsyncSocket::listen(const int maxcon)
{
  if (!m_server.available())
  {
    return 1;
  }

  this->initialze();

  WSAEVENT event = WSACreateEvent();
  if (WSAEventSelect(m_server.handle(), event, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
  {
    return 2;
  }

  m_sockets[m_n_events] = m_server.handle();
  m_events[m_n_events]  = event;
  m_n_events++;

  return m_server.listen(maxcon);
}

IResult vuapi AsyncSocket::close()
{
  return m_server.close();
}

VUResult vuapi AsyncSocket::stop()
{
  m_mutex.lock();
  m_running = false;
  m_mutex.unlock();
  return VU_OK;
}

VUResult vuapi AsyncSocket::run()
{
  if (!m_server.available())
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

    auto socket = m_sockets[idx];
    auto event  = m_events[idx];

    WSANETWORKEVENTS events = { 0 };
    WSAEnumNetworkEvents(socket, event, &events);

    if (events.lNetworkEvents & FD_ACCEPT)
    {
      result = this->do_open(events, socket);
      if (result != VU_OK)
      {
        break;
      }
    }

    if (events.lNetworkEvents & FD_READ)
    {
      result = this->do_recv(events, socket);
      if (result != VU_OK)
      {
        break;
      }
    }

    if (events.lNetworkEvents & FD_WRITE)
    {
      result = this->do_send(events, socket);
      if (result != VU_OK)
      {
        break;
      }
    }

    if (events.lNetworkEvents & FD_CLOSE)
    {
      result = this->do_close(events, socket);
      if (result != VU_OK)
      {
        break;
      }
    }
  }

  return result;
}

IResult vuapi AsyncSocket::do_open(WSANETWORKEVENTS& events, SOCKET& socket)
{
  if (events.iErrorCode[FD_ACCEPT_BIT] != 0)
  {
    return events.iErrorCode[FD_ACCEPT_BIT];
  }

  Socket::sSocket obj = { 0 };
  int n = static_cast<int>(sizeof(obj.sai));

  obj.s = accept(socket, (struct sockaddr*)&obj.sai, &n);
  if (m_n_events > WSA_MAXIMUM_WAIT_EVENTS)
  {
    ::closesocket(obj.s);
    return WSAEMFILE; // Too many connections
  }

  WSAEVENT event = WSACreateEvent();
  WSAEventSelect(obj.s, event, FD_READ | FD_WRITE | FD_CLOSE);
  m_events[m_n_events]  = event;
  m_sockets[m_n_events] = obj.s;
  m_n_events++;

  Socket client(m_server.af(), m_server.type(), m_server.protocol(), false);
  client.attach(obj);
  this->on_open(client);
  client.detach();

  return VU_OK;
}

IResult vuapi AsyncSocket::do_recv(WSANETWORKEVENTS& events, SOCKET& socket)
{
  if (events.iErrorCode[FD_READ_BIT] != 0)
  {
    return events.iErrorCode[FD_READ_BIT];
  }

  Socket client(m_server.af(), m_server.type(), m_server.protocol(), false);
  client.attach(socket);
  this->on_recv(client);
  client.detach();

  return VU_OK;
}

IResult vuapi AsyncSocket::do_send(WSANETWORKEVENTS& events, SOCKET& socket)
{
  if (events.iErrorCode[FD_WRITE_BIT] != 0)
  {
    return events.iErrorCode[FD_WRITE_BIT];
  }

  Socket client(m_server.af(), m_server.type(), m_server.protocol(), false);
  client.attach(socket);
  this->on_send(client);
  client.detach();

  return VU_OK;
}

IResult vuapi AsyncSocket::do_close(WSANETWORKEVENTS& events, SOCKET& socket)
{
  if (events.iErrorCode[FD_CLOSE_BIT] != 0)
  {
    return events.iErrorCode[FD_CLOSE_BIT];
  }

  std::vector<std::pair<SOCKET, HANDLE>> in_used_clients;

  for (int i = 0; i < WSA_MAXIMUM_WAIT_EVENTS; i++)
  {
    if (m_sockets[i] == socket)
    {
      WSACloseEvent(m_events[i]);
      m_events[i]  = nullptr;
      m_sockets[i] = INVALID_SOCKET;
    }

    if (m_sockets[i] != INVALID_SOCKET)
    {
      in_used_clients.emplace_back(std::make_pair(m_sockets[i], m_events[i]));
    }
  }

  memset(m_sockets, int(INVALID_SOCKET), sizeof(m_sockets));
  memset(m_events, int(0), sizeof(m_events));

  m_n_events = 0;
  for (auto& e : in_used_clients)
  {
    m_sockets[m_n_events] = e.first;
    m_events[m_n_events]  = e.second;
    m_n_events++;
  }

  Socket client(m_server.af(), m_server.type(), m_server.protocol(), false);
  client.attach(socket);
  this->on_close(client);
  client.detach();

  ::closesocket(socket);

  socket = INVALID_SOCKET;

  // CompressArrays(m_Events, m_Sockets, &m_nEvents);

  return VU_OK;
}

void AsyncSocket::on(const eFnType type, const fn_prototype_t fn)
{
  assert(!m_running && type < eFnType::UNDEFINED);
  m_functions[type] = fn;
}

void AsyncSocket::on_open(Socket& client)
{
  if (auto& fn = m_functions[eFnType::OPEN])
  {
    fn(client);
  }
}

void AsyncSocket::on_send(Socket& client)
{
  if (auto& fn = m_functions[eFnType::SEND])
  {
    fn(client);
  }
}

void AsyncSocket::on_recv(Socket& client)
{
  if (auto& fn = m_functions[eFnType::RECV])
  {
    fn(client);
  }
}

void AsyncSocket::on_close(Socket& client)
{
  if (auto& fn = m_functions[eFnType::CLOSE])
  {
    fn(client);
  }
}

IResult vuapi AsyncSocket::send(
  const SOCKET& client,
  const char* ptr_data,
  int size,
  const Socket::flags_t flags)
{
  vu::Socket socket;
  socket.attach(client);
  return socket.send(ptr_data, size, flags);
}

IResult vuapi AsyncSocket::send(
  const SOCKET& client,
  const Buffer& data,
  const Socket::flags_t flags)
{
  vu::Socket socket;
  socket.attach(client);
  return socket.send(data, flags);
}

#endif // VU_SOCKET_ENABLED

} // namespace vu