/**
 * @file   asyncsocket.cpp
 * @author Vic P.
 * @brief  Implementation for Asynchronous Socket
 */

#include "Vutils.h"

namespace vu
{

#ifdef VU_SOCKET_ENABLED

CAsyncSocket::CAsyncSocket(const vu::CSocket::AddressFamily af, const vu::CSocket::Type type, const vu::CSocket::Protocol proto) : m_server(af, type, proto)
{
  this->initialze();

  for (uint i = 0; i < eFnType::UNDEFINED; i++)
  {
    m_functions[i] = nullptr;
  }
}

CAsyncSocket::~CAsyncSocket()
{
  this->close();
}

void vuapi CAsyncSocket::initialze()
{
  m_n_events = 0;

  ZeroMemory(m_sockets, sizeof(m_sockets));
  ZeroMemory(m_events, sizeof(m_events));

  m_running = false;
}

std::set<SOCKET> vuapi CAsyncSocket::get_clients()
{
  std::set<SOCKET> result;

  if (m_server.available())
  {
    for (auto& socket : m_sockets)
    {
      if (socket != 0 && socket != INVALID_SOCKET && socket != m_server.get_socket())
      {
        result.insert(socket);
      }
    }
  }

  return result;
}

bool vuapi CAsyncSocket::available()
{
  return m_server.available();
}

bool vuapi CAsyncSocket::running()
{
  return m_running;
}

VUResult vuapi CAsyncSocket::bind(const CSocket::TEndPoint& endpoint)
{
  return m_server.bind(endpoint);
}

VUResult vuapi CAsyncSocket::bind(const std::string& address, const ushort port)
{
  return m_server.bind(address, port);
}

VUResult vuapi CAsyncSocket::listen(const int maxcon)
{
  if (!m_server.available())
  {
    return 1;
  }

  this->initialze();

  WSAEVENT event = WSACreateEvent();
  if (WSAEventSelect(m_server.get_socket(), event, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
  {
    return 2;
  }

  m_sockets[m_n_events] = m_server.get_socket();
  m_events[m_n_events]  = event;
  m_n_events++;

  return m_server.listen(maxcon);
}

IResult vuapi CAsyncSocket::close()
{
  return m_server.close();
}

VUResult vuapi CAsyncSocket::stop()
{
  m_mutex.lock();
  m_running = false;
  m_mutex.unlock();
  return VU_OK;
}

VUResult vuapi CAsyncSocket::run()
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

VUResult vuapi CAsyncSocket::loop()
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

    auto& socket = m_sockets[idx];
    auto& event  = m_events[idx];

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

IResult vuapi CAsyncSocket::do_open(WSANETWORKEVENTS& events, SOCKET& socket)
{
  if (events.iErrorCode[FD_ACCEPT_BIT] != 0)
  {
    return events.iErrorCode[FD_ACCEPT_BIT];
  }

  CSocket::TSocket obj = { 0 };
  int n = static_cast<int>(sizeof(obj.sai));

  obj.s = accept(socket, (struct sockaddr*)&obj.sai, &n);
  if (m_n_events > WSA_MAXIMUM_WAIT_EVENTS)
  {
    closesocket(obj.s);
    return WSAEMFILE; // Too many connections
  }

  WSAEVENT event = WSACreateEvent();
  WSAEventSelect(obj.s, event, FD_READ | FD_WRITE | FD_CLOSE);
  m_events[m_n_events]  = event;
  m_sockets[m_n_events] = obj.s;
  m_n_events++;

  CSocket client(m_server.get_af(), m_server.get_type(), m_server.get_protocol(), false);
  client.attach(obj);
  this->on_open(client);
  client.detach();

  return VU_OK;
}

IResult vuapi CAsyncSocket::do_recv(WSANETWORKEVENTS& events, SOCKET& socket)
{
  if (events.iErrorCode[FD_READ_BIT] != 0)
  {
    return events.iErrorCode[FD_READ_BIT];
  }

  CSocket client(m_server.get_af(), m_server.get_type(), m_server.get_protocol(), false);
  client.attach(socket);
  this->on_recv(client);
  client.detach();

  return VU_OK;
}

IResult vuapi CAsyncSocket::do_send(WSANETWORKEVENTS& events, SOCKET& socket)
{
  if (events.iErrorCode[FD_WRITE_BIT] != 0)
  {
    return events.iErrorCode[FD_WRITE_BIT];
  }

  CSocket client(m_server.get_af(), m_server.get_type(), m_server.get_protocol(), false);
  client.attach(socket);
  this->on_send(client);
  client.detach();

  return VU_OK;
}

IResult vuapi CAsyncSocket::do_close(WSANETWORKEVENTS& events, SOCKET& socket)
{
  if (events.iErrorCode[FD_CLOSE_BIT] != 0)
  {
    return events.iErrorCode[FD_CLOSE_BIT];
  }

  CSocket client(m_server.get_af(), m_server.get_type(), m_server.get_protocol(), false);
  client.attach(socket);
  this->on_close(client);
  client.detach();

  closesocket(socket);
  socket = 0;
  // CompressArrays(m_Events, m_Sockets, &m_nEvents);

  return VU_OK;
}

void CAsyncSocket::on(const eFnType type, const FnPrototype fn)
{
  assert(!m_running && type < eFnType::UNDEFINED);
  m_functions[type] = fn;
}

void CAsyncSocket::on_open(CSocket& client)
{
  if (auto& fn = m_functions[eFnType::OPEN])
  {
    fn(client);
  }
}

void CAsyncSocket::on_send(CSocket& client)
{
  if (auto& fn = m_functions[eFnType::SEND])
  {
    fn(client);
  }
}

void CAsyncSocket::on_recv(CSocket& client)
{
  if (auto& fn = m_functions[eFnType::RECV])
  {
    fn(client);
  }
}

void CAsyncSocket::on_close(CSocket& client)
{
  if (auto& fn = m_functions[eFnType::CLOSE])
  {
    fn(client);
  }
}

#endif // VU_SOCKET_ENABLED

} // namespace vu