/**
 * @file   asyncsocket.cpp
 * @author Vic P.
 * @brief  Implementation for Asynchronous Socket
 */

#include "Vutils.h"

namespace vu
{

CAsyncSocket::CAsyncSocket(
  const vu::CSocket::AddressFamily af,
  const vu::CSocket::Type type,
  const vu::CSocket::Protocol proto) : m_Server(af, type, proto)
{
  this->Initialze();

  for (uint i = 0; i < eFnType::UNDEFINED; i++)
  {
    m_FNs[i] = nullptr;
  }
}

CAsyncSocket::~CAsyncSocket()
{
  this->Close();
}

void vuapi CAsyncSocket::Initialze()
{
  m_nEvents = 0;

  ZeroMemory(m_Sockets, sizeof(m_Sockets));
  ZeroMemory(m_Events, sizeof(m_Events));

  m_Running = false;
}

std::set<SOCKET> vuapi CAsyncSocket::GetClients()
{
  std::set<SOCKET> result;

  if (m_Server.Available())
  {
    for (auto& socket : m_Sockets)
    {
      if (socket != 0 && socket != INVALID_SOCKET && socket != m_Server.GetSocket())
      {
        result.insert(socket);
      }
    }
  }

  return result;
}

bool vuapi CAsyncSocket::Available()
{
  return m_Server.Available();
}

bool vuapi CAsyncSocket::Running()
{
  return m_Running;
}

VUResult vuapi CAsyncSocket::Bind(const CSocket::TEndPoint& endpoint)
{
  return m_Server.Bind(endpoint);
}

VUResult vuapi CAsyncSocket::Bind(const std::string& address, const ushort port)
{
  return m_Server.Bind(address, port);
}

VUResult vuapi CAsyncSocket::Listen(const int maxcon)
{
  if (!m_Server.Available())
  {
    return 1;
  }

  this->Initialze();

  WSAEVENT Event = WSACreateEvent();
  if (WSAEventSelect(m_Server.GetSocket(), Event, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
  {
    return 2;
  }

  m_Sockets[m_nEvents] = m_Server.GetSocket();
  m_Events[m_nEvents]  = Event;
  m_nEvents++;

  return m_Server.Listen(maxcon);
}

IResult vuapi CAsyncSocket::Close()
{
  return m_Server.Close();
}

VUResult vuapi CAsyncSocket::Stop()
{
  m_Mutex.lock();
  m_Running = false;
  m_Mutex.unlock();
  return VU_OK;
}

VUResult vuapi CAsyncSocket::Run()
{
  if (!m_Server.Available())
  {
    return 1;
  }

  VUResult result = VU_OK;

  if (m_Running)
  {
    return result;
  }

  m_Running = true;

  while (m_Running)
  {
    if (this->Loop() != VU_OK)
    {
      break;
    }
  }

  return VU_OK;
}

VUResult vuapi CAsyncSocket::Loop()
{
  VUResult result = VU_OK;

  // TODO: Vic. Recheck. FALSE, WSA_INFINITE, FALSE
  DWORD idx = WSAWaitForMultipleEvents(m_nEvents, m_Events, FALSE, 0, FALSE);
  idx -= WSA_WAIT_EVENT_0;

  for (DWORD i = idx; i < m_nEvents; i++)
  {
    // TODO: Vic. Recheck. TRUE, 1000, FALSE
    idx = WSAWaitForMultipleEvents(1, &m_Events[i], FALSE, 0, FALSE);
    if (idx == WSA_WAIT_FAILED || idx == WSA_WAIT_TIMEOUT)
    {
      continue;
    }

    idx = i;

    auto& Socket = m_Sockets[idx];
    auto& Event  = m_Events[idx];

    WSANETWORKEVENTS Events = { 0 };
    WSAEnumNetworkEvents(Socket, Event, &Events);

    if (Events.lNetworkEvents & FD_ACCEPT)
    {
      result = this->DoOpen(Events, Socket);
      if (result != VU_OK)
      {
        break;
      }
    }

    if (Events.lNetworkEvents & FD_READ)
    {
      result = this->DoRecv(Events, Socket);
      if (result != VU_OK)
      {
        break;
      }
    }

    if (Events.lNetworkEvents & FD_WRITE)
    {
      result = this->DoSend(Events, Socket);
      if (result != VU_OK)
      {
        break;
      }
    }

    if (Events.lNetworkEvents & FD_CLOSE)
    {
      result = this->DoClose(Events, Socket);
      if (result != VU_OK)
      {
        break;
      }
    }
  }

  return result;
}

IResult vuapi CAsyncSocket::DoOpen(WSANETWORKEVENTS& Events, SOCKET& Socket)
{
  if (Events.iErrorCode[FD_ACCEPT_BIT] != 0)
  {
    return Events.iErrorCode[FD_ACCEPT_BIT];
  }

  CSocket::TSocket obj = { 0 };
  int n = static_cast<int>(sizeof(obj.sai));

  obj.s = accept(Socket, (struct sockaddr*)&obj.sai, &n);
  if (m_nEvents > WSA_MAXIMUM_WAIT_EVENTS)
  {
    closesocket(obj.s);
    return WSAEMFILE; // Too many connections
  }

  WSAEVENT Event = WSACreateEvent();
  WSAEventSelect(obj.s, Event, FD_READ | FD_WRITE | FD_CLOSE);
  m_Events[m_nEvents]  = Event;
  m_Sockets[m_nEvents] = obj.s;
  m_nEvents++;

  CSocket client(m_Server.GetAF(), m_Server.GetType(), m_Server.GetProtocol(), false);
  client.Attach(obj);
  this->OnOpen(client);
  client.Detach();

  return VU_OK;
}

IResult vuapi CAsyncSocket::DoRecv(WSANETWORKEVENTS& Events, SOCKET& Socket)
{
  if (Events.iErrorCode[FD_READ_BIT] != 0)
  {
    return Events.iErrorCode[FD_READ_BIT];
  }

  CSocket client(m_Server.GetAF(), m_Server.GetType(), m_Server.GetProtocol(), false);
  client.Attach(Socket);
  this->OnRecv(client);
  client.Detach();

  return VU_OK;
}

IResult vuapi CAsyncSocket::DoSend(WSANETWORKEVENTS& Events, SOCKET& Socket)
{
  if (Events.iErrorCode[FD_WRITE_BIT] != 0)
  {
    return Events.iErrorCode[FD_WRITE_BIT];
  }

  CSocket client(m_Server.GetAF(), m_Server.GetType(), m_Server.GetProtocol(), false);
  client.Attach(Socket);
  this->OnSend(client);
  client.Detach();

  return VU_OK;
}

IResult vuapi CAsyncSocket::DoClose(WSANETWORKEVENTS& Events, SOCKET& Socket)
{
  if (Events.iErrorCode[FD_CLOSE_BIT] != 0)
  {
    return Events.iErrorCode[FD_CLOSE_BIT];
  }

  CSocket client(m_Server.GetAF(), m_Server.GetType(), m_Server.GetProtocol(), false);
  client.Attach(Socket);
  this->OnClose(client);
  client.Detach();

  closesocket(Socket);
  Socket = 0;
  // CompressArrays(m_Events, m_Sockets, &m_nEvents);

  return VU_OK;
}

void CAsyncSocket::On(const eFnType type, const FnPrototype fn)
{
  assert(!m_Running && type < eFnType::UNDEFINED);
  m_FNs[type] = fn;
}

void CAsyncSocket::OnOpen(CSocket& client)
{
  if (auto& fn = m_FNs[eFnType::OPEN])
  {
    fn(client);
  }
}

void CAsyncSocket::OnSend(CSocket& client)
{
  if (auto& fn = m_FNs[eFnType::SEND])
  {
    fn(client);
  }
}

void CAsyncSocket::OnRecv(CSocket& client)
{
  if (auto& fn = m_FNs[eFnType::RECV])
  {
    fn(client);
  }
}

void CAsyncSocket::OnClose(CSocket& client)
{
  if (auto& fn = m_FNs[eFnType::CLOSE])
  {
    fn(client);
  }
}

} // namespace vu