/**
 * @file   socket.cpp
 * @author Vic P.
 * @brief  Implementation for Socket
 */

#include "Vutils.h"

#ifdef VU_SOCKET_ENABLED
#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#pragma comment(lib, "ws2_32.lib")
#endif
#endif // VU_SOCKET_ENABLED

namespace vu
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif // _MSC_VER

#ifdef VU_SOCKET_ENABLED

const std::string VU_LOCALHOST = "127.0.0.1";

CSocket::CSocket(const AddressFamily af, const Type type, const Protocol proto) : CLastError()
{
  ZeroMemory(&m_WSAData, sizeof(m_WSAData));
  ZeroMemory(&m_Server, sizeof(m_Server));
  ZeroMemory(&m_Client, sizeof(m_Client));

  if (WSAStartup(MAKEWORD(2, 2), &m_WSAData) != 0)
  {
    m_LastErrorCode = GetLastError();
  }

  m_Socket = socket(af, type, proto);

  m_Server.sin_family = af;
}

CSocket::~CSocket()
{
  WSACleanup();

  m_LastErrorCode = GetLastError();
}

bool vuapi CSocket::IsValid(const SOCKET& socket)
{
  return !(socket == 0 || socket == INVALID_SOCKET);
}

const SOCKET& vuapi CSocket::GetSocket() const
{
  return m_Socket;
}

VUResult vuapi CSocket::SetOption(
  const int level,
  const int opt,
  const std::string& val,
  const int size)
{
  if (!this->IsValid(m_Socket))
  {
    return 1;
  }

  if (val.empty())
  {
    return 2;
  }

  if (setsockopt(m_Socket, level, opt, val.c_str(), size) != 0)
  {
    m_LastErrorCode = GetLastError();
    return 3;
  }

  return VU_OK;
}

VUResult vuapi CSocket::Bind(const sServer& server)
{
  return this->Bind(server.Host, server.Port);
}

VUResult vuapi CSocket::Bind(const std::string& address, const ushort port)
{
  if (!this->IsValid(m_Socket))
  {
    return 1;
  }

  std::string IP;

  if (this->IsHostName(address) == true)
  {
    IP = this->GetHostByName(address);
  }
  else
  {
    IP = address;
  }

  if (IP.empty())
  {
    return 2;
  }

  m_Server.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
  m_Server.sin_port = htons(port);

  if (bind(m_Socket, (const struct sockaddr*)&m_Server, sizeof(m_Server)) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    return 3;
  }

  return VU_OK;
}

VUResult vuapi CSocket::Listen(const int maxcon)
{
  if (!this->IsValid(m_Socket))
  {
    return 1;
  }

  int result = listen(m_Socket, maxcon);

  m_LastErrorCode = GetLastError();

  return (result == SOCKET_ERROR ? 2 : VU_OK);
}

VUResult vuapi CSocket::Accept(sInfomation& info)
{
  if (!this->IsValid(m_Socket))
  {
    return 1;
  }

  ZeroMemory(&info, sizeof(info));

  int size = sizeof(info.sai);

  info.s = accept(m_Socket, (struct sockaddr*)&info.sai, &size);

  m_LastErrorCode = GetLastError();

  if (!this->IsValid(info.s))
  {
    return 2;
  }

  this->BytesToIP(info);

  return VU_OK;
}

VUResult vuapi CSocket::Connect(const sServer& AccessPoint)
{
  return this->Connect(AccessPoint.Host, AccessPoint.Port);
}

VUResult vuapi CSocket::Connect(const std::string& Address, ushort usPort)
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

IResult vuapi CSocket::Send(const char* lpData, int size, const Flags flags)
{
  if (!this->IsValid(m_Socket))
  {
    return SOCKET_ERROR;
  }

  IResult z = send(m_Socket, lpData, size, flags);
  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }

  return z;
}

IResult vuapi CSocket::Send(const CBuffer& data, const Flags flags)
{
  return this->Send((const char*)data.GetpData(), int(data.GetSize()), flags);
}

IResult vuapi CSocket::Recv(char* lpData, int size, const Flags flags)
{
  if (!this->IsValid(m_Socket))
  {
    return SOCKET_ERROR;
  }

  IResult z = recv(m_Socket, lpData, size, flags);
  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }

  return z;
}

IResult vuapi CSocket::Recv(CBuffer& Data, const Flags flags)
{
  if (!this->IsValid(m_Socket))
  {
    return SOCKET_ERROR;
  }

  auto z = this->Recv((char*)Data.GetpData(), int(Data.GetSize()), flags);
  if (z != SOCKET_ERROR)
  {
    Data.Resize(z);
  }

  return z;
}

IResult vuapi CSocket::Recvall(CBuffer& Data, const Flags flags)
{
  CBuffer buffer;

  do
  {
    buffer.Resize(KiB);
    IResult z = this->Recv(buffer, flags);
    if (z <= 0)
    {
      buffer.Reset();
    }
    else
    {
      if (z != static_cast<int>(buffer.GetSize()))
      {
        buffer.Resize(z);
      }
      Data.Append(buffer);
    }
  } while (!buffer.Empty());

  return IResult(Data.GetSize());
}

IResult vuapi CSocket::SendTo(const CBuffer& Data, const sInfomation& info)
{
  return this->SendTo((const char*)Data.GetpData(), int(Data.GetSize()), info);
}

IResult vuapi CSocket::SendTo(const char* lpData, const int size, const sInfomation& info)
{
  if (!this->IsValid(m_Socket))
  {
    return SOCKET_ERROR;
  }

  IResult z = sendto(
    m_Socket,
    lpData,
    size,
    0,
    (const struct sockaddr*)&info.sai,
    sizeof(info.sai)
  );

  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }

  return z;
}

IResult vuapi CSocket::RecvFrom(CBuffer& Data, const sInfomation& info)
{
  auto z =  this->RecvFrom((char*)Data.GetpData(), int(Data.GetSize()), info);
  if (z != SOCKET_ERROR)
  {
    Data.Resize(z);
  }

  return z;
}

IResult vuapi CSocket::RecvFrom(char* lpData, int size, const sInfomation& info)
{
  if (!this->IsValid(m_Socket))
  {
    return SOCKET_ERROR;
  }

  int n = sizeof(info.sai);
  IResult z = recvfrom(m_Socket, lpData, size, 0, (struct sockaddr *)&info.sai, &n);
  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }
  else
  {
    this->BytesToIP(info);
  }

  return z;
}

IResult vuapi CSocket::RecvallFrom(CBuffer& Data, const sInfomation& info)
{
  CBuffer buffer;

  do
  {
    buffer.Resize(KiB);
    IResult z = this->RecvFrom(buffer, info);
    if (z <= 0)
    {
      buffer.Reset();
    }
    else
    {
      if (z != static_cast<int>(buffer.GetSize()))
      {
        buffer.Resize(z);
      }
      Data.Append(buffer);
    }
  } while (!buffer.Empty());

  return IResult(Data.GetSize());
}

bool vuapi CSocket::Close()
{
  if (!this->IsValid(m_Socket))
  {
    return false;
  }

  closesocket(m_Socket);
  m_Socket = INVALID_SOCKET;

  return true;
}

VUResult vuapi CSocket::Shutdown(const Shutdowns flags)
{
  if (!this->IsValid(m_Socket))
  {
    return 1;
  }

  if (shutdown(m_Socket, flags) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    return 2;
  }

  return VU_OK;
}

std::string vuapi CSocket::GetLocalHostName()
{
  std::string result = "";

  if (!this->IsValid(m_Socket))
  {
    return result;
  }

  std::unique_ptr<char[]> h(new char [MAXBYTE]);
  if (h == nullptr)
  {
    return result;
  }

  ZeroMemory(h.get(), MAXBYTE);
  if (gethostname(h.get(), MAXBYTE) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    return result;
  }

  result.assign(h.get());

  return result;
}

std::string vuapi CSocket::GetHostByName(const std::string& Name)
{
  std::string r;
  r.clear();

  if (!this->IsValid(m_Socket))
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

bool vuapi CSocket::BytesToIP(const sInfomation& info)
{
  if (sprintf(
    (char*)info.ip,
    "%d.%d.%d.%d\0",
    info.sai.sin_addr.S_un.S_un_b.s_b1,
    info.sai.sin_addr.S_un.S_un_b.s_b2,
    info.sai.sin_addr.S_un.S_un_b.s_b3,
    info.sai.sin_addr.S_un.S_un_b.s_b4
  ) < 0) return false;
  else return true;
}

#endif // VU_SOCKET_ENABLED

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu