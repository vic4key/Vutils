/**
 * @file   mbuffer.cpp
 * @author Vic P.
 * @brief  Implementation for Memory Buffer
 */

#include "Vutils.h"

namespace vu
{

Buffer::Buffer() : m_ptr(nullptr), m_size(0)
{
  this->create(nullptr, 0);
}

Buffer::Buffer(const size_t size) : m_ptr(nullptr), m_size(0)
{
  this->create(nullptr, size);
}

Buffer::Buffer(const void* ptr, const size_t size) : m_ptr(nullptr), m_size(0)
{
  this->replace(ptr, size);
}

Buffer::Buffer(const Buffer& right) : m_ptr(nullptr), m_size(0)
{
  *this = right;
}

Buffer::~Buffer()
{
  this->destroy();
}

const Buffer& Buffer::operator=(const Buffer& right)
{
  if (*this != right)
  {
    if (this->create(nullptr, right.m_size))
    {
      if (right.m_ptr != nullptr)
      {
        memcpy_s(m_ptr, m_size, right.m_ptr, right.m_size);
      }
    }
  }

  return *this;
}

bool Buffer::operator==(const Buffer& right) const
{
  if (m_size != right.m_size)
  {
    return false;
  }

  return memcmp(m_ptr, right.m_ptr, m_size) == 0;
}

bool Buffer::operator!=(const Buffer& right) const
{
  return !(*this == right);
}

byte& Buffer::operator[](const size_t offset)
{
  if (m_ptr == nullptr)
  {
    throw std::runtime_error(static_cast<const char*>("invalid pointer"));
  }

  if (m_size == 0 || offset >= m_size)
  {
    throw std::out_of_range(static_cast<const char*>("invalid size or offset"));
  }

  return static_cast<byte*>(m_ptr)[offset];
}

std::unique_ptr<Buffer> Buffer::operator()(int begin, int end) const
{
  return this->slice(begin, end);
}

size_t Buffer::find(const void* ptr, const size_t size) const
{
  size_t result = -1;

  if (m_ptr == nullptr || m_size == 0 || ptr == nullptr || size == 0 || m_size < size)
  {
    return result;
  }

  const auto ptr_bytes = this->bytes();

  for (size_t i = 0; i <= m_size - size; i++)
  {
    if (memcmp(reinterpret_cast<const void*>(ptr_bytes + i), ptr, size) == 0)
    {
      result = i;
      break;
    }
  }

  return result;
}

bool Buffer::match(const void* ptr, const size_t size) const
{
  return this->find(ptr, size) != -1;
}

std::unique_ptr<Buffer> Buffer::till(const void* ptr, const size_t size) const
{
  size_t offset = this->find(ptr, size);
  if (offset == 0)
  {
    return nullptr;
  }

  std::unique_ptr<Buffer> result(new Buffer);
  result->create(m_ptr, offset);
  return result;
}

std::unique_ptr<Buffer> Buffer::slice(int begin, int end) const
{
  if (m_ptr == nullptr || m_size == 0)
  {
    return nullptr;
  }

  if (begin < 0)
  {
    begin = int(m_size) + begin;
  }

  if (end < 0)
  {
    end = int(m_size) + end;
  }

  if (begin < 0 || end < 0 || begin > int(m_size) || end > int(m_size) || begin > end)
  {
    return nullptr;
  }

  int size = end - begin;

  if (size <= 0 || size > int(m_size))
  {
    return nullptr;
  }

  std::unique_ptr<Buffer> result(new Buffer);
  result->create(this->bytes() + begin, size);

  return result;
}

byte* Buffer::bytes() const
{
  return static_cast<byte*>(m_ptr);
}

void* Buffer::pointer() const
{
  return m_ptr;
}

size_t Buffer::size() const
{
  return m_size;
}

bool Buffer::create(void* ptr, const size_t size, const bool clean)
{
  if (clean || size == 0)
  {
    this->destroy();
  }

  if (size == 0)
  {
    return false;
  }

  if (clean)
  {
    m_ptr = std::calloc(m_size = size, 1);
  }
  else
  {
    m_ptr = std::realloc(ptr, m_size = size);
  }

  if (m_ptr == nullptr)
  {
    throw std::bad_alloc();
  }

  if (ptr == nullptr)
  {
    memset(m_ptr, 0, m_size);
  }
  else if (clean)
  {
    memcpy_s(m_ptr, m_size, ptr, size);
  }

  return true;
}

bool Buffer::destroy()
{
  if (m_ptr != nullptr)
  {
    std::free(m_ptr);
  }

  m_ptr = nullptr;
  m_size = 0;

  return true;
}

void Buffer::reset()
{
  this->destroy();
}

void Buffer::fill(const byte v)
{
  if (m_ptr != nullptr && m_size != 0)
  {
    memset(m_ptr, v, m_size);
  }
}

bool Buffer::resize(const size_t size)
{
  if (size == m_size)
  {
    return true;
  }

  return this->create(m_ptr, size, false);
}

bool Buffer::replace(const void* ptr, const size_t size)
{
  if (this->create(nullptr, size))
  {
    if (ptr != nullptr)
    {
      memcpy_s(m_ptr, m_size, ptr, size);
    }
  }

  return true;
}

bool Buffer::replace(const Buffer& right)
{
  return this->replace(right.pointer(), right.size());
}

bool Buffer::empty() const
{
  return m_ptr == nullptr || m_size == 0;
}

bool Buffer::append(const void* ptr, const size_t size)
{
  if (ptr == nullptr || size == 0)
  {
    return false;
  }

  const size_t prev_size = m_size;

  this->resize(m_size + size);

  memcpy_s(this->bytes() + prev_size, size, ptr, size);

  return true;
}

bool Buffer::append(const Buffer& right)
{
  return this->append(right.pointer(), right.size());
}

std::unique_ptr<std::string> Buffer::to_string_A() const
{
  std::unique_ptr<std::string> result(new std::string);
  if (m_ptr == nullptr || m_size == 0)
  {
    return result;
  }

  result.reset(new std::string(reinterpret_cast<const char*>(m_ptr), m_size / sizeof(char)));
  return result;
}

std::unique_ptr<std::wstring> Buffer::to_string_W() const
{
  std::unique_ptr<std::wstring> result(new std::wstring);
  if (m_ptr == nullptr || m_size == 0)
  {
    return result;
  }

  result.reset(new std::wstring(reinterpret_cast<const wchar*>(m_ptr), m_size / sizeof(wchar)));
  return result;
}

bool Buffer::save_to_file(const std::string& file_path)
{
  if (file_path.empty())
  {
    return false;
  }

  bool result = true;

  FileSystemA file(file_path, fs_mode::FM_CREATEALWAY);
  result &= file.write(m_ptr, ulong(m_size));
  result &= file.close();

  return result;
}

bool Buffer::save_to_file(const std::wstring& file_path)
{
  if (file_path.empty())
  {
    return false;
  }

  bool result = true;

  FileSystemW file(file_path, fs_mode::FM_CREATEALWAY);
  result &= file.write(m_ptr, ulong(m_size));
  result &= file.close();

  return result;
}

} // namespace vu