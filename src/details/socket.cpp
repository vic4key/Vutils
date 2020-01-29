/**
 * @file   misc.cpp
 * @author Vic P.
 * @brief  Implementation for Miscellaneous
 */

#include "Vutils.h"

#ifdef VU_SOCKET_ENABLED
#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#pragma comment(lib, "ws2_32.lib")
#endif
#endif // VU_SOCKET_ENABLED

namespace vu
{

#ifdef VU_SOCKET_ENABLED

const std::string VU_LOCALHOST = "127.0.0.1";

CSocket::CSocket()
{
  m_Socket = INVALID_SOCKET;

  memset((void*)&m_WSAData, 0, sizeof(m_WSAData));
  memset((void*)&m_Server, 0, sizeof(m_Server));
  memset((void*)&m_Client, 0, sizeof(m_Client));

  m_LastErrorCode = ERROR_SUCCESS;
}

CSocket::CSocket(eSocketAF socketAF, eSocketType SocketType)
{
  m_Socket = INVALID_SOCKET;

  memset((void*)&m_WSAData, 0, sizeof(m_WSAData));
  memset((void*)&m_Server, 0, sizeof(m_Server));
  memset((void*)&m_Client, 0, sizeof(m_Client));

  m_LastErrorCode = ERROR_SUCCESS;

  this->Socket(socketAF, SocketType);
}

CSocket::~CSocket() {}

bool vuapi CSocket::IsSocketValid(SOCKET socket)
{
  if (!socket || socket == INVALID_SOCKET)
  {
    return false;
  }

  return true;
}

VUResult vuapi CSocket::Socket(eSocketAF SocketAF, eSocketType SocketType, eSocketProtocol SocketProtocol)
{
  if (WSAStartup(MAKEWORD(2, 2), &m_WSAData) != 0)
  {
    m_LastErrorCode = GetLastError();
    return 1;
  }

  m_Socket = socket(SocketAF, SocketType, SocketProtocol);
  if (!this->IsSocketValid(m_Socket))
  {
    return 2;
  }

  m_Server.sin_family = SocketAF;

  return VU_OK;
}

VUResult vuapi CSocket::Bind(const TAccessPoint& AccessPoint)
{
  return this->Bind(AccessPoint.Host, AccessPoint.Port);
}

VUResult vuapi CSocket::Bind(const std::string& Address, unsigned short usPort)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  std::string IP;

  if (this->IsHostName(Address) == true)
  {
    IP = this->GetHostByName(Address);
  }
  else
  {
    IP = Address;
  }

  if (IP.empty())
  {
    return 2;
  }

  m_Server.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
  m_Server.sin_port = htons(usPort);

  if (bind(m_Socket, (const struct sockaddr*)&m_Server, sizeof(m_Server)) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    return 3;
  }

  return VU_OK;
}

VUResult vuapi CSocket::Listen(int iMaxConnection)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  int result = listen(m_Socket, iMaxConnection);

  m_LastErrorCode = GetLastError();

  return (result == SOCKET_ERROR ? 2 : VU_OK);
}

VUResult vuapi CSocket::Accept(TSocketInfomation& SocketInformation)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  memset((void*)&SocketInformation, 0, sizeof(SocketInformation));

  int size = sizeof(SocketInformation.sai);

  SocketInformation.s = accept(m_Socket, (struct sockaddr*)&SocketInformation.sai, &size);

  m_LastErrorCode = GetLastError();

  if (!this->IsSocketValid(SocketInformation.s))
  {
    return 2;
  }

  this->BytesToIP(SocketInformation);

  return VU_OK;
}

VUResult vuapi CSocket::Connect(const TAccessPoint& AccessPoint)
{
  return this->Connect(AccessPoint.Host, AccessPoint.Port);
}

VUResult vuapi CSocket::Connect(const std::string& Address, unsigned short usPort)
{
  std::string IP;

  if (this->IsHostName(Address) == true)
  {
    IP = this->GetHostByName(Address);
  }
  else
  {
    IP = Address;
  }

  if (IP.empty())
  {
    return 1;
  }

  m_Server.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
  m_Server.sin_port = htons(usPort);

  if (connect(m_Socket, (const struct sockaddr*)&m_Server, sizeof(m_Server)) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    this->Close();
    return 2;
  }

  return VU_OK;
}

IResult vuapi CSocket::Send(const char* lpData, int iLength, eSocketMessage SocketMessage)
{
  return this->Send(m_Socket, lpData, iLength, SocketMessage);
}

IResult vuapi CSocket::Send(const CBinary& Data, eSocketMessage SocketMessage)
{
  return this->Send(m_Socket, (const char*)Data.GetpData(), Data.GetUsedSize(), SocketMessage);
}

IResult vuapi CSocket::Send(SOCKET socket, const char* lpData, int iLength, eSocketMessage SocketMessage)
{
  if (!this->IsSocketValid(socket))
  {
    return SOCKET_ERROR;
  }

  IResult z = send(socket, lpData, iLength, SocketMessage);
  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }

  return z;
}

IResult vuapi CSocket::Recv(char* lpData, int iLength, eSocketMessage SocketMessage)
{
  return this->Recv(m_Socket, lpData, iLength, SocketMessage);
}

IResult vuapi CSocket::Recv(CBinary& Data, eSocketMessage SocketMessage)
{
  void* p = Data.GetpData();
  auto z = this->Recv(m_Socket, (char*)p, Data.GetSize(), SocketMessage);
  if (z != SOCKET_ERROR)
  {
    Data.SetUsedSize(z);
  }

  return z;
}

IResult vuapi CSocket::Recv(SOCKET socket, char* lpData, int iLength, eSocketMessage SocketMessage)
{
  if (!this->IsSocketValid(socket))
  {
    return SOCKET_ERROR;
  }

  IResult z = recv(socket, lpData, iLength, SocketMessage);
  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }

  return z;
}

IResult vuapi CSocket::SendTo(const CBinary& Data, TSocketInfomation& SocketInformation)
{
  return this->SendTo((const char*)Data.GetpData(), Data.GetUsedSize(), SocketInformation);
}

IResult vuapi CSocket::SendTo(const char* lpData, int iLength, TSocketInfomation& SocketInformation)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return SOCKET_ERROR;
  }

  IResult z = sendto(
    m_Socket,
    lpData,
    iLength,
    0,
    (const struct sockaddr*)&SocketInformation.sai,
    sizeof(SocketInformation.sai)
  );

  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }

  return z;
}

IResult vuapi CSocket::RecvFrom(CBinary& Data, TSocketInfomation& SocketInformation)
{
  void* p = Data.GetpData();
  return this->RecvFrom((char*)p, Data.GetUsedSize(), SocketInformation);
}

IResult vuapi CSocket::RecvFrom(char* lpData, int iLength, TSocketInfomation& SocketInformation)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return SOCKET_ERROR;
  }

  int n = sizeof(SocketInformation.sai);
  IResult z = recvfrom(m_Socket, lpData, iLength, 0, (struct sockaddr *)&SocketInformation.sai, &n);
  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }
  else
  {
    this->BytesToIP(SocketInformation);
  }

  return z;
}

bool vuapi CSocket::Close(SOCKET socket)
{
  if (socket != 0)
  {
    if (!this->IsSocketValid(socket)) return false;
    closesocket(socket);
  }
  else if (this->IsSocketValid(m_Socket))
  {
    closesocket(m_Socket);
    m_Socket = 0;
  }

  WSACleanup();

  m_LastErrorCode = GetLastError();

  return true;
}

SOCKET vuapi CSocket::GetSocket()
{
  return m_Socket;
}

VUResult vuapi CSocket::GetOption(int iLevel, int iOptName, char* pOptVal, int* lpiLength)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  if (getsockopt(m_Socket, iLevel, iOptName, pOptVal, lpiLength) != 0)
  {
    m_LastErrorCode = GetLastError();
    return 2;
  }

  return VU_OK;
}

VUResult vuapi CSocket::SetOption(int iLevel, int iOptName, const std::string& OptVal, int iLength)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  if (OptVal.empty())
  {
    return 2;
  }

  if (setsockopt(m_Socket, iLevel, iOptName, OptVal.c_str(), iLength) != 0)
  {
    m_LastErrorCode = GetLastError();
    return 3;
  }

  return VU_OK;
}

VUResult vuapi CSocket::Shutdown(eShutdownFlag ShutdownFlag)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  if (shutdown(m_Socket, (int)ShutdownFlag) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    return 2;
  }

  return VU_OK;
}

std::string vuapi CSocket::GetLocalHostName()
{
  std::string r;
  r.clear();

  std::unique_ptr<char[]> h(new char [MAXBYTE]);
  if (h == nullptr)
  {
    return r;
  }

  if (!this->IsSocketValid(m_Socket))
  {
    return r;
  }

  memset(h.get(), 0, MAXBYTE);
  if (gethostname(h.get(), MAXBYTE) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    return r;
  }

  r.assign(h.get());

  return r;
}

std::string vuapi CSocket::GetHostByName(const std::string& Name)
{
  std::string r;
  r.clear();

  if (!this->IsSocketValid(m_Socket))
  {
    WSASetLastError(6);  // WSA_INVALID_HANDLE
    return r;
  }

  if (Name.empty())
  {
    WSASetLastError(87); // WSA_INVALID_PARAMETER
    r = VU_LOCALHOST;
    return r;
  }

  if (Name.length() >= MAXBYTE)
  {
    WSASetLastError(87); // WSA_INVALID_PARAMETER
    return r;
  }

  hostent * h = gethostbyname(Name.c_str());
  if (h == nullptr)
  {
    return r;
  }

  if (h->h_addr_list[0] == nullptr || strlen(h->h_addr_list[0]) == 0)
  {
    return r;
  }

  in_addr a = {0};
  memcpy((void*)&a, (void*)h->h_addr_list[0], sizeof(a));
  r = inet_ntoa(a);

  return r;
}

bool vuapi CSocket::IsHostName(const std::string& s)
{
  bool r = false;
  const std::string MASK = "01234567890.";

  if (s.empty())
  {
    return r;
  }

  if (s.length() >= MAXBYTE)
  {
    return r;
  }

  for (unsigned int i = 0; i < s.length(); i++)
  {
    if (strchr(MASK.c_str(), s[i]) == nullptr)
    {
      r = true;
      break;
    }
  }

  return r;
}

bool vuapi CSocket::BytesToIP(const TSocketInfomation& SocketInformation)
{
  if (sprintf(
    (char*)SocketInformation.ip,
    "%d.%d.%d.%d\0",
    SocketInformation.sai.sin_addr.S_un.S_un_b.s_b1,
    SocketInformation.sai.sin_addr.S_un.S_un_b.s_b2,
    SocketInformation.sai.sin_addr.S_un.S_un_b.s_b3,
    SocketInformation.sai.sin_addr.S_un.S_un_b.s_b4
  ) < 0) return false;
  else return true;
}

#endif // VU_SOCKET_ENABLED

} // namespace vu