/**
 * @file   socket.cpp
 * @author Vic P.
 * @brief  Implementation for Socket
 */

#include "Vutils.h"

#ifdef VU_INET_ENABLED
#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#pragma comment(lib, "ws2_32.lib")
#endif
#endif // VU_INET_ENABLED

namespace vu
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif // _MSC_VER

#ifdef VU_INET_ENABLED

/**
 * Endpoint
 */

Endpoint::Endpoint(const std::string& endpoint)
{
  auto l = split_string_A(endpoint, ":");
  if (l.size() != 2)
  {
    throw "invalid endpoint";
  }
  else
  {
    m_host = l[0];
    m_port = ushort(std::stoul(l[1]));
  }
}

Endpoint::Endpoint(const std::wstring& endpoint)
{
  auto temp = to_string_A(endpoint);
  auto l = split_string_A(temp, ":");
  if (l.size() != 2)
  {
    throw "invalid endpoint";
  }
  else
  {
    m_host = l[0];
    m_port = ushort(std::stoul(l[1]));
  }
}

Endpoint::Endpoint(const std::wstring& host, const ushort port) : m_port(port)
{
  m_host = to_string_A(host);
}

Endpoint::Endpoint(const std::string& host, const ushort port) : m_host(host), m_port(port)
{
}

bool Endpoint::operator==(const Endpoint& right)
{
  return m_host == right.m_host && m_port == right.m_port;
}

bool Endpoint::operator!=(const Endpoint& right)
{
  return !(*this == right);
}

const vu::Endpoint& Endpoint::operator=(const Endpoint& right)
{
  m_host = right.m_host;
  m_port = right.m_port;
  return *this;
}

const vu::Endpoint& Endpoint::operator=(const std::string& right)
{
  Endpoint endpoint(right);
  *this = endpoint;
  return *this;
}

const vu::Endpoint& Endpoint::operator=(const std::wstring& right)
{
  Endpoint endpoint(right);
  *this = endpoint;
  return *this;
}

/**
 * Socket
 */

Socket::Socket(
  const address_family_t af,
  const type_t type,
  const protocol_t proto,
  const Options* options
) : LastError(), m_af(af), m_type(type), m_proto(proto)
  , m_attached(false), m_side(Socket::side_type::UNDEFINED)
{
  ZeroMemory(&m_wsa_data, sizeof(m_wsa_data));
  if (WSAStartup(MAKEWORD(2, 2), &m_wsa_data) == INVALID_SOCKET)
  {
    m_last_error_code = GetLastError();
    assert("start wsa failed.");
  }

  if (options != nullptr)
  {
    m_options = *options;
  }

  ZeroMemory(&m_sai, sizeof(m_sai));
  m_sai.sin_family = m_af;

  m_socket = ::socket(m_af, m_type, m_proto);
  if (m_socket == INVALID_SOCKET)
  {
    m_last_error_code = GetLastError();
    assert("open socket failed.");
  }
}

Socket::Socket(const Socket& right)
{
  *this = right;
}

Socket::~Socket()
{
  if (m_attached)
  {
    return; // ignore if the connection is attached from outside
  }

  if (this->available())
  {
    this->close();
  }

  if (WSACleanup() == INVALID_SOCKET)
  {
    assert("clean wsa failed.");
    m_last_error_code = GetLastError();
  }
}

bool Socket::operator==(const Socket& right)
{
  return m_socket == m_socket;
}

bool Socket::operator!=(const Socket& right)
{
  return !(*this == right);
}

const vu::Socket& Socket::operator=(const Socket& right)
{
  if (this == &right)
  {
    return *this;
  }

  m_type = right.m_type;
  m_af = right.m_af;
  m_proto = right.m_proto;
  m_sai = right.m_sai;
  m_socket = right.m_socket;
  m_options = right.m_options;
  m_attached = right.m_attached;

  return *this;
}

Socket::side_type vuapi Socket::side() const
{
  return m_side;
}

bool vuapi Socket::valid(const SOCKET& socket) const
{
  return !(socket == 0 || socket == INVALID_SOCKET);
}

bool vuapi Socket::available() const
{
  return this->valid(m_socket);
}

void vuapi Socket::attach(const SOCKET& socket)
{
  Handle obj = { 0 };
  obj.s = socket;
  this->attach(obj);
}

void vuapi Socket::attach(const Handle& socket)
{
  m_socket = socket.s;
  m_sai = socket.sai;
  m_attached = true;
}

void vuapi Socket::detach()
{
  m_socket = INVALID_SOCKET;
  ZeroMemory(&m_sai, sizeof(m_sai));
}

Socket::Options& Socket::options()
{
  return m_options;
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

const SOCKET& vuapi Socket::handle() const
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
    m_last_error_code = GetLastError();
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
    m_last_error_code = GetLastError();
  }

  return result;
}

VUResult vuapi Socket::set_option(
  const int level,
  const int option,
  const void* value,
  const int size)
{
  if (!this->available())
  {
    return 1;
  }

  if (::setsockopt(m_socket, level, option, static_cast<const char*>(value), size) == INVALID_SOCKET)
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
    m_last_error_code = GetLastError();
    return 2;
  }

  return VU_OK;
}

VUResult vuapi Socket::bind(const Endpoint& endpoint)
{
  return this->bind(endpoint.m_host, endpoint.m_port);
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

  m_side = side_type::SERVER;

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

VUResult vuapi Socket::accept(Handle& socket)
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

VUResult vuapi Socket::connect(const Endpoint& endpoint)
{
  std::string ip;

  if (this->is_host_name(endpoint.m_host) == true)
  {
    ip = this->get_host_address(endpoint.m_host);
  }
  else
  {
    ip = endpoint.m_host;
  }

  if (ip.empty())
  {
    return 1;
  }

  m_sai.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
  m_sai.sin_port = htons(endpoint.m_port);

  VUResult result = VU_OK;

  if (::connect(m_socket, (const struct sockaddr*)&m_sai, sizeof(m_sai)) == SOCKET_ERROR)
  {
    result = GetLastError() == WSAEWOULDBLOCK ? VU_OK : 2; // ignore 'A non-blocking socket operation could not be completed immediately.'
  }

  m_side = side_type::CLIENT;

  return result;
}

IResult vuapi Socket::send(const char* ptr_data, int size, const flags_t flags)
{
  if (!this->available())
  {
    return SOCKET_ERROR;
  }

  int  sent_bytes = 0;
  auto sent_ptr_data = ptr_data;

  do
  {
    IResult z = ::send(m_socket, sent_ptr_data, size - sent_bytes, 0);
    if (z == SOCKET_ERROR)
    {
      m_last_error_code = GetLastError();
      return SOCKET_ERROR;
    }

    sent_bytes += z;
    sent_ptr_data += z;
  } while (sent_bytes < size);

  return sent_bytes;
}

IResult vuapi Socket::send(const Buffer& buffer, const flags_t flags)
{
  return this->send((const char*)buffer.pointer(), int(buffer.size()), flags);
}

IResult vuapi Socket::recv(char* ptr_data, int size, const flags_t flags)
{
  if (!this->available())
  {
    return SOCKET_ERROR;
  }

  fd_set fds_read = { 0 };
  FD_ZERO(&fds_read);
  FD_SET(m_socket, &fds_read);

  timeval timeout = { 0 };
  timeout.tv_usec = 0;
  timeout.tv_sec = m_options.timeout.recv;

  int status = ::select(0, &fds_read, nullptr, nullptr, &timeout);
  if (status == SOCKET_ERROR)
  {
    m_last_error_code = GetLastError();
    return SOCKET_ERROR;
  }
  else if (status == 0)
  {
    return VU_OK;
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

  auto z = this->recv((char*)buffer.pointer(), int(buffer.size()), flags);
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
    block.resize(VU_DEFAULT_SEND_RECV_BLOCK_SIZE);
    IResult z = this->recv(block, flags);
    if (z <= 0) // error or completed
    {
      if (z == SOCKET_ERROR)
      {
        m_last_error_code = GetLastError();
        return SOCKET_ERROR;
      }

      block.reset();
    }
    else // in-progress
    {
      if (z < static_cast<int>(block.size()))
      {
        block.resize(z);
      }
      buffer.append(block);
    }
  } while (!block.empty());

  return IResult(buffer.size());
}

IResult vuapi Socket::send_to(const Buffer& buffer, const Handle& socket)
{
  return this->send_to((const char*)buffer.pointer(), int(buffer.size()), socket);
}

IResult vuapi Socket::send_to(const char* ptr_data, const int size, const Handle& socket)
{
  if (!this->available())
  {
    return SOCKET_ERROR;
  }

  int  sent_bytes = 0;
  auto sent_ptr_data = ptr_data;

  do
  {
    IResult z = ::sendto(
      m_socket,
      sent_ptr_data,
      size - sent_bytes,
      0,
      (const struct sockaddr*)&socket.sai,
      sizeof(socket.sai)
    );
    if (z == SOCKET_ERROR)
    {
      m_last_error_code = GetLastError();
      return SOCKET_ERROR;
    }

    sent_bytes += z;
    sent_ptr_data += z;
  } while (sent_bytes < size);

  return sent_bytes;
}

IResult vuapi Socket::recv_from(Buffer& buffer, const Handle& socket)
{
  auto z = this->recv_from((char*)buffer.pointer(), int(buffer.size()), socket);
  if (z != SOCKET_ERROR)
  {
    buffer.resize(z);
  }

  return z;
}

IResult vuapi Socket::recv_from(char* ptr_data, int size, const Handle& socket)
{
  if (!this->available())
  {
    return SOCKET_ERROR;
  }

  int n = sizeof(socket.sai);
  IResult z = ::recvfrom(m_socket, ptr_data, size, 0, (struct sockaddr*)&socket.sai, &n);
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

IResult vuapi Socket::recv_all_from(Buffer& buffer, const Handle& socket)
{
  Buffer block;

  do
  {
    block.resize(VU_DEFAULT_SEND_RECV_BLOCK_SIZE);
    IResult z = this->recv_from(block, socket);
    if (z <= 0) // error or completed
    {
      if (z == SOCKET_ERROR)
      {
        m_last_error_code = GetLastError();
        return SOCKET_ERROR;
      }

      block.reset();
    }
    else // in-progress
    {
      if (z < static_cast<int>(block.size())) // last recv
      {
        block.resize(z);
      }
      buffer.append(block);
    }
  } while (!block.empty());

  return IResult(buffer.size());
}

VUResult vuapi Socket::close(const shutdowns_t flags, const bool cleanup)
{
  if (!this->available())
  {
    return 1;
  }

  if (cleanup) // clean-up all remaining data in the socket (does not need with the SD_BOTH flag)
  {
    vu::Buffer temp;
    this->recv_all(temp);
  }

  if (::shutdown(m_socket, flags) == SOCKET_ERROR)
  {
    // m_last_error_code = GetLastError();
    // return 2;
  }

  if (::closesocket(m_socket) == SOCKET_ERROR)
  {
    m_last_error_code = GetLastError();
    return 3;
  }

  m_socket = INVALID_SOCKET;

  return VU_OK;
}

VUResult vuapi Socket::disconnect(const shutdowns_t flags, const bool cleanup)
{
  if (!this->available())
  {
    return 1;
  }

  if (this->close(flags, cleanup) != VU_OK)
  {
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

std::string vuapi Socket::get_host_address(const std::string& name) const
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

bool vuapi Socket::is_host_name(const std::string& s) const
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

bool vuapi Socket::parse(const Handle& socket) const
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

#endif // VU_INET_ENABLED

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu