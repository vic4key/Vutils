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

const size_t VU_DEF_BLOCK_SIZE = KiB;

CSocket::CSocket(const AddressFamily af, const Type type, const Protocol proto) : CLastError()
{
  ZeroMemory(&m_WSAData, sizeof(m_WSAData));
  ZeroMemory(&m_SAI, sizeof(m_SAI));

  if (WSAStartup(MAKEWORD(2, 2), &m_WSAData) != 0)
  {
    m_LastErrorCode = GetLastError();
  }

  m_Socket = socket(af, type, proto);

  m_SAI.sin_family = af;
}

CSocket::~CSocket()
{
  this->Close();

  WSACleanup();

  m_LastErrorCode = GetLastError();
}

bool vuapi CSocket::Valid(const SOCKET& socket)
{
  return !(socket == 0 || socket == INVALID_SOCKET);
}

bool vuapi CSocket::Available()
{
  return this->Valid(m_Socket);
}

void vuapi CSocket::Attach(const TSocket& socket)
{
  m_Socket = socket.s;
  m_SAI = socket.sai;
}

SOCKET& vuapi CSocket::GetSocket()
{
  return m_Socket;
}

VUResult vuapi CSocket::SetOption(
  const int level,
  const int opt,
  const std::string& val,
  const int size)
{
  if (!this->Available())
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

VUResult vuapi CSocket::EnableNonBlocking(bool state)
{
  if (!this->Available())
  {
    return 1;
  }

  ulong NonBlock = state ? 1 : 0;
  if (ioctlsocket(m_Socket, FIONBIO, &NonBlock) == SOCKET_ERROR)
  {
    return 2;
  }

  return VU_OK;
}

VUResult vuapi CSocket::Bind(const TEndPoint& endpoint)
{
  return this->Bind(endpoint.Host, endpoint.Port);
}

VUResult vuapi CSocket::Bind(const std::string& address, const ushort port)
{
  if (!this->Available())
  {
    return 1;
  }

  std::string IP;

  if (this->IsHostName(address) == true)
  {
    IP = this->GetHostAddress(address);
  }
  else
  {
    IP = address;
  }

  if (IP.empty())
  {
    return 2;
  }

  m_SAI.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
  m_SAI.sin_port = htons(port);

  if (bind(m_Socket, (const struct sockaddr*)&m_SAI, sizeof(m_SAI)) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    return 3;
  }

  return VU_OK;
}

VUResult vuapi CSocket::Listen(const int maxcon)
{
  if (!this->Available())
  {
    return 1;
  }

  int result = listen(m_Socket, maxcon);

  m_LastErrorCode = GetLastError();

  return (result == SOCKET_ERROR ? 2 : VU_OK);
}

VUResult vuapi CSocket::Accept(TSocket& socket)
{
  if (!this->Available())
  {
    return 1;
  }

  ZeroMemory(&socket, sizeof(socket));

  int size = sizeof(socket.sai);

  socket.s = accept(m_Socket, (struct sockaddr*)&socket.sai, &size);

  m_LastErrorCode = GetLastError();

  if (!this->Valid(socket.s))
  {
    return 2;
  }

  this->Parse(socket);

  return VU_OK;
}

VUResult vuapi CSocket::Connect(const TEndPoint& endpoint)
{
  return this->Connect(endpoint.Host, endpoint.Port);
}

VUResult vuapi CSocket::Connect(const std::string& Address, ushort usPort)
{
  std::string IP;

  if (this->IsHostName(Address) == true)
  {
    IP = this->GetHostAddress(Address);
  }
  else
  {
    IP = Address;
  }

  if (IP.empty())
  {
    return 1;
  }

  m_SAI.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
  m_SAI.sin_port = htons(usPort);

  if (connect(m_Socket, (const struct sockaddr*)&m_SAI, sizeof(m_SAI)) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    this->Close();
    return 2;
  }

  return VU_OK;
}

IResult vuapi CSocket::Send(const char* lpData, int size, const Flags flags)
{
  if (!this->Available())
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
  if (!this->Available())
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
  if (!this->Available())
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
    buffer.Resize(VU_DEF_BLOCK_SIZE);
    IResult z = this->Recv(buffer, flags);
    if (z <= 0)
    {
      buffer.Reset();
    }
    else
    {
      if (z < static_cast<int>(buffer.GetSize()))
      {
        buffer.Resize(z);
      }
      Data.Append(buffer);
      if (z < VU_DEF_BLOCK_SIZE)
      {
        buffer.Reset();
      }
    }
  } while (!buffer.Empty());

  return IResult(Data.GetSize());
}

IResult vuapi CSocket::SendTo(const CBuffer& Data, const TSocket& socket)
{
  return this->SendTo((const char*)Data.GetpData(), int(Data.GetSize()), socket);
}

IResult vuapi CSocket::SendTo(const char* lpData, const int size, const TSocket& socket)
{
  if (!this->Available())
  {
    return SOCKET_ERROR;
  }

  IResult z = sendto(
    m_Socket,
    lpData,
    size,
    0,
    (const struct sockaddr*)&socket.sai,
    sizeof(socket.sai)
  );

  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }

  return z;
}

IResult vuapi CSocket::RecvFrom(CBuffer& Data, const TSocket& socket)
{
  auto z =  this->RecvFrom((char*)Data.GetpData(), int(Data.GetSize()), socket);
  if (z != SOCKET_ERROR)
  {
    Data.Resize(z);
  }

  return z;
}

IResult vuapi CSocket::RecvFrom(char* lpData, int size, const TSocket& socket)
{
  if (!this->Available())
  {
    return SOCKET_ERROR;
  }

  int n = sizeof(socket.sai);
  IResult z = recvfrom(m_Socket, lpData, size, 0, (struct sockaddr *)&socket.sai, &n);
  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }
  else
  {
    this->Parse(socket);
  }

  return z;
}

IResult vuapi CSocket::RecvallFrom(CBuffer& Data, const TSocket& socket)
{
  CBuffer buffer;

  do
  {
    buffer.Resize(VU_DEF_BLOCK_SIZE);
    IResult z = this->RecvFrom(buffer, socket);
    if (z <= 0)
    {
      buffer.Reset();
    }
    else
    {
      if (z < static_cast<int>(buffer.GetSize()))
      {
        buffer.Resize(z);
      }
      Data.Append(buffer);
      if (z < VU_DEF_BLOCK_SIZE)
      {
        buffer.Reset();
      }
    }
  } while (!buffer.Empty());

  return IResult(Data.GetSize());
}

VUResult vuapi CSocket::Close()
{
  if (!this->Available())
  {
    return 1;
  }

  closesocket(m_Socket);
  m_Socket = INVALID_SOCKET;

  return VU_OK;
}

VUResult vuapi CSocket::Shutdown(const Shutdowns flags)
{
  if (!this->Available())
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

std::string vuapi CSocket::GetHostName()
{
  std::string result = "";

  if (!this->Available())
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

std::string vuapi CSocket::GetHostAddress(const std::string& Name)
{
  std::string result = "";

  if (!this->Available())
  {
    WSASetLastError(6);  // WSA_INVALID_HANDLE
    return result;
  }

  if (Name.empty())
  {
    WSASetLastError(87); // WSA_INVALID_PARAMETER
    return result;
  }

  if (Name.length() >= MAXBYTE)
  {
    WSASetLastError(87); // WSA_INVALID_PARAMETER
    return result;
  }

  hostent * h = gethostbyname(Name.c_str());
  if (h == nullptr)
  {
    return result;
  }

  if (h->h_addr_list[0] == nullptr || strlen(h->h_addr_list[0]) == 0)
  {
    return result;
  }

  in_addr a = {0};
  memcpy((void*)&a, (void*)h->h_addr_list[0], sizeof(a));
  result = inet_ntoa(a);

  return result;
}

bool vuapi CSocket::IsHostName(const std::string& s)
{
  bool result = false;
  const std::string MASK = "01234567890.";

  if (s.empty())
  {
    return result;
  }

  if (s.length() >= MAXBYTE)
  {
    return result;
  }

  for (unsigned int i = 0; i < s.length(); i++)
  {
    if (strchr(MASK.c_str(), s[i]) == nullptr)
    {
      result = true;
      break;
    }
  }

  return result;
}

bool vuapi CSocket::Parse(const TSocket& socket)
{
  if (sprintf(
    (char*)socket.ip,
    "%d.%d.%d.%d\0",
    socket.sai.sin_addr.S_un.S_un_b.s_b1,
    socket.sai.sin_addr.S_un.S_un_b.s_b2,
    socket.sai.sin_addr.S_un.S_un_b.s_b3,
    socket.sai.sin_addr.S_un.S_un_b.s_b4
  ) < 0) return false;
  else return true;
}

#endif // VU_SOCKET_ENABLED

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu