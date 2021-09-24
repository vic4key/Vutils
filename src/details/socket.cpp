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

Socket::Socket(
  const address_family_t af,
  const type_t type,
  const protocol_t proto,
  bool  wsa
) : LastError(), m_af(af), m_type(type), m_proto(proto), m_wsa(wsa), m_self(false)
{
  ZeroMemory(&m_wsa_data, sizeof(m_wsa_data));
  ZeroMemory(&m_sai, sizeof(m_sai));

  if (m_wsa)
  {
    if (WSAStartup(MAKEWORD(2, 2), &m_wsa_data) != 0)
    {
      m_last_error_code = GetLastError();
    }
  }

  m_socket = ::socket(m_af, m_type, m_proto);

  m_sai.sin_family = m_af;
}

Socket::~Socket()
{
  this->close();

  if (m_wsa)
  {
    WSACleanup();
  }

  m_last_error_code = GetLastError();
}

bool vuapi Socket::valid(const SOCKET& socket)
{
  return !(socket == 0 || socket == INVALID_SOCKET);
}

bool vuapi Socket::available()
{
  return this->valid(m_socket);
}

void vuapi Socket::attach(const SOCKET& socket)
{
  sSocket obj = { 0 };
  obj.s = socket;
  this->attach(obj);
}

void vuapi Socket::attach(const sSocket& socket)
{
  m_socket = socket.s;
  m_sai = socket.sai;
}

void vuapi Socket::detach()
{
  m_socket = INVALID_SOCKET;
  ZeroMemory(&m_sai, sizeof(m_sai));
}

const WSADATA& vuapi Socket::wsa() const
{
  return m_wsa_data;
}

const Socket::address_family_t vuapi Socket::af() const
{
  return m_af;
}

const Socket::type_t vuapi Socket::type() const
{
  return m_type;
}

const Socket::protocol_t vuapi Socket::protocol() const
{
  return m_proto;
}

SOCKET& vuapi Socket::handle()
{
  return m_socket;
}

const sockaddr_in vuapi Socket::get_local_sai()
{
  sockaddr_in result = { 0 };

  if (this->available())
  {
    auto size = int(sizeof(result));
    ::getsockname(m_socket, (struct sockaddr*)&result, &size);
  }

  return result;
}

const sockaddr_in vuapi Socket::get_remote_sai()
{
  sockaddr_in result = { 0 };

  if (this->available())
  {
    auto size = int(sizeof(result));
    ::getpeername(m_socket, (struct sockaddr*)&result, &size);
  }

  return result;
}

VUResult vuapi Socket::set_option(
  const int level,
  const int opt,
  const std::string& val,
  const int size)
{
  if (!this->available())
  {
    return 1;
  }

  if (val.empty())
  {
    return 2;
  }

  if (::setsockopt(m_socket, level, opt, val.c_str(), size) != 0)
  {
    m_last_error_code = GetLastError();
    return 3;
  }

  return VU_OK;
}

VUResult vuapi Socket::enable_non_blocking(bool state)
{
  if (!this->available())
  {
    return 1;
  }

  ulong non_block = state ? 1 : 0;
  if (::ioctlsocket(m_socket, FIONBIO, &non_block) == SOCKET_ERROR)
  {
    return 2;
  }

  return VU_OK;
}

VUResult vuapi Socket::bind(const sEndPoint& endpoint)
{
  return this->bind(endpoint.host, endpoint.port);
}

VUResult vuapi Socket::bind(const std::string& address, const ushort port)
{
  if (!this->available())
  {
    return 1;
  }

  std::string ip = this->is_host_name(address) ? this->get_host_address(address) : address;
  if (ip.empty())
  {
    return 2;
  }

  m_sai.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
  m_sai.sin_port = htons(port);

  if (::bind(m_socket, (const struct sockaddr*)&m_sai, sizeof(m_sai)) == SOCKET_ERROR)
  {
    m_last_error_code = GetLastError();
    return 3;
  }

  return VU_OK;
}

VUResult vuapi Socket::listen(const int maxcon)
{
  if (!this->available())
  {
    return 1;
  }

  int result = ::listen(m_socket, maxcon);

  m_last_error_code = GetLastError();

  return (result == SOCKET_ERROR ? 2 : VU_OK);
}

VUResult vuapi Socket::accept(sSocket& socket)
{
  if (!this->available())
  {
    return 1;
  }

  ZeroMemory(&socket, sizeof(socket));

  int size = sizeof(socket.sai);

  socket.s = ::accept(m_socket, (struct sockaddr*)&socket.sai, &size);

  m_last_error_code = GetLastError();

  if (!this->valid(socket.s))
  {
    return 2;
  }

  this->parse(socket);

  return VU_OK;
}

VUResult vuapi Socket::connect(const sEndPoint& endpoint)
{
  return this->connect(endpoint.host, endpoint.port);
}

VUResult vuapi Socket::connect(const std::string& address, ushort port)
{
  std::string ip;

  if (this->is_host_name(address) == true)
  {
    ip = this->get_host_address(address);
  }
  else
  {
    ip = address;
  }

  if (ip.empty())
  {
    return 1;
  }

  m_sai.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
  m_sai.sin_port = htons(port);

  if (::connect(m_socket, (const struct sockaddr*)&m_sai, sizeof(m_sai)) == SOCKET_ERROR)
  {
    m_last_error_code = GetLastError();
    this->close();
    return 2;
  }

  m_self = true;

  return VU_OK;
}

IResult vuapi Socket::send(const char* ptr_data, int size, const flags_t flags)
{
  if (!this->available())
  {
    return SOCKET_ERROR;
  }

  IResult z = ::send(m_socket, ptr_data, size, flags);
  if (z == SOCKET_ERROR)
  {
    m_last_error_code = GetLastError();
  }

  return z;
}

IResult vuapi Socket::send(const Buffer& buffer, const flags_t flags)
{
  return this->send((const char*)buffer.get_ptr(), int(buffer.get_size()), flags);
}

IResult vuapi Socket::recv(char* ptr_data, int size, const flags_t flags)
{
  if (!this->available())
  {
    return SOCKET_ERROR;
  }

  IResult z = ::recv(m_socket, ptr_data, size, flags);
  if (z == SOCKET_ERROR)
  {
    m_last_error_code = GetLastError();
  }

  return z;
}

IResult vuapi Socket::recv(Buffer& buffer, const flags_t flags)
{
  if (!this->available())
  {
    return SOCKET_ERROR;
  }

  auto z = this->recv((char*)buffer.get_ptr(), int(buffer.get_size()), flags);
  if (z != SOCKET_ERROR)
  {
    buffer.resize(z);
  }

  return z;
}

IResult vuapi Socket::recv_all(Buffer& buffer, const flags_t flags)
{
  Buffer block;

  do
  {
    block.resize(VU_DEF_BLOCK_SIZE);
    IResult z = this->recv(block, flags);
    if (z <= 0)
    {
      block.reset();
    }
    else
    {
      if (z < static_cast<int>(block.get_size()))
      {
        block.resize(z);
      }
      buffer.append(block);
      if (z < VU_DEF_BLOCK_SIZE)
      {
        block.reset();
      }
    }
  } while (!block.empty());

  return IResult(buffer.get_size());
}

IResult vuapi Socket::send_to(const Buffer& buffer, const sSocket& socket)
{
  return this->send_to((const char*)buffer.get_ptr(), int(buffer.get_size()), socket);
}

IResult vuapi Socket::send_to(const char* lpData, const int size, const sSocket& socket)
{
  if (!this->available())
  {
    return SOCKET_ERROR;
  }

  IResult z = ::sendto(
    m_socket,
    lpData,
    size,
    0,
    (const struct sockaddr*)&socket.sai,
    sizeof(socket.sai)
  );

  if (z == SOCKET_ERROR)
  {
    m_last_error_code = GetLastError();
  }

  return z;
}

IResult vuapi Socket::recv_from(Buffer& buffer, const sSocket& socket)
{
  auto z = this->recv_from((char*)buffer.get_ptr(), int(buffer.get_size()), socket);
  if (z != SOCKET_ERROR)
  {
    buffer.resize(z);
  }

  return z;
}

IResult vuapi Socket::recv_from(char* ptr_data, int size, const sSocket& socket)
{
  if (!this->available())
  {
    return SOCKET_ERROR;
  }

  int n = sizeof(socket.sai);
  IResult z = ::recvfrom(m_socket, ptr_data, size, 0, (struct sockaddr *)&socket.sai, &n);
  if (z == SOCKET_ERROR)
  {
    m_last_error_code = GetLastError();
  }
  else
  {
    this->parse(socket);
  }

  return z;
}

IResult vuapi Socket::recv_all_from(Buffer& buffer, const sSocket& socket)
{
  Buffer block;

  do
  {
    block.resize(VU_DEF_BLOCK_SIZE);
    IResult z = this->recv_from(block, socket);
    if (z <= 0)
    {
      block.reset();
    }
    else
    {
      if (z < static_cast<int>(block.get_size()))
      {
        block.resize(z);
      }
      buffer.append(block);
      if (z < VU_DEF_BLOCK_SIZE)
      {
        block.reset();
      }
    }
  } while (!block.empty());

  return IResult(buffer.get_size());
}

VUResult vuapi Socket::close()
{
  if (!this->available())
  {
    return 1;
  }

  if (m_self)
  {
    ::closesocket(m_socket);
  }

  m_socket = INVALID_SOCKET;

  return VU_OK;
}

VUResult vuapi Socket::disconnect(const shutdowns_t flags)
{
  if (!this->available())
  {
    return 1;
  }

  if (::shutdown(m_socket, flags) == SOCKET_ERROR)
  {
    m_last_error_code = GetLastError();
    return 2;
  }

  return VU_OK;
}

std::string vuapi Socket::get_host_name()
{
  std::string result = "";

  if (!this->available())
  {
    return result;
  }

  std::unique_ptr<char[]> h(new char [MAXBYTE]);
  if (h == nullptr)
  {
    return result;
  }

  ZeroMemory(h.get(), MAXBYTE);
  if (::gethostname(h.get(), MAXBYTE) == SOCKET_ERROR)
  {
    m_last_error_code = GetLastError();
    return result;
  }

  result.assign(h.get());

  return result;
}

std::string vuapi Socket::get_host_address(const std::string& name)
{
  std::string result = "";

  if (!this->available())
  {
    WSASetLastError(6);  // WSA_INVALID_HANDLE
    return result;
  }

  if (name.empty())
  {
    WSASetLastError(87); // WSA_INVALID_PARAMETER
    return result;
  }

  if (name.length() >= MAXBYTE)
  {
    WSASetLastError(87); // WSA_INVALID_PARAMETER
    return result;
  }

  hostent * h = gethostbyname(name.c_str());
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

bool vuapi Socket::is_host_name(const std::string& s)
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

bool vuapi Socket::parse(const sSocket& socket)
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