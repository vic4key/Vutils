/**
 * @file   mbuffer.cpp
 * @author Vic P.
 * @brief  Implementation for Memory Buffer
 */

#include "Vutils.h"

namespace vu
{

Buffer::Buffer() : m_ptr_data(nullptr), m_size(0)
{
  this->create(nullptr, 0);
}

Buffer::Buffer(const size_t size) : m_ptr_data(nullptr), m_size(0)
{
  this->create(nullptr, size);
}

Buffer::Buffer(const void* ptr, const size_t size) : m_ptr_data(nullptr), m_size(0)
{
  this->replace(ptr, size);
}

Buffer::Buffer(const Buffer& right) : m_ptr_data(nullptr), m_size(0)
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
      if (right.m_ptr_data != nullptr)
      {
        memcpy_s(m_ptr_data, m_size, right.m_ptr_data, right.m_size);
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

  return memcmp(m_ptr_data, right.m_ptr_data, m_size) == 0;
}

bool Buffer::operator!=(const Buffer& right) const
{
  return !(*this == right);
}

byte& Buffer::operator[](const size_t offset)
{
  if (m_ptr_data == nullptr)
  {
    throw std::runtime_error(static_cast<const char*>("invalid pointer"));
  }

  if (m_size == 0 || offset >= m_size)
  {
    throw std::out_of_range(static_cast<const char*>("invalid size or offset"));
  }

  return static_cast<byte*>(m_ptr_data)[offset];
}

Buffer Buffer::operator()(int begin, int end) const
{
  return this->slice(begin, end);
}

size_t Buffer::find(const void* ptr, const size_t size) const
{
  size_t result = -1;

  if (m_ptr_data == nullptr || m_size == 0 || ptr == nullptr || size == 0 || m_size < size)
  {
    return result;
  }

  const auto pbytes = this->get_ptr_bytes();

  for (size_t i = 0; i <= m_size - size; i++)
  {
    if (memcmp(reinterpret_cast<const void*>(pbytes + i), ptr, size) == 0)
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

Buffer Buffer::till(const void* ptr, const size_t size) const
{
  Buffer result;

  size_t offset = this->find(ptr, size);
  if (offset > 0)
  {
    result.create(m_ptr_data, offset);
  }

  return result;
}

Buffer Buffer::slice(int begin, int end) const
{
  Buffer result;

  if (m_ptr_data == nullptr || m_size == 0)
  {
    return result;
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
    return result;
  }

  int size = end - begin;

  if (size <= 0 || size > int(m_size))
  {
    return result;
  }

  result.create(this->get_ptr_bytes() + begin, size);

  return result;
}

byte* Buffer::get_ptr_bytes() const
{
  return static_cast<byte*>(m_ptr_data);
}

void* Buffer::get_ptr_data() const
{
  return m_ptr_data;
}

size_t Buffer::get_size() const
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
    m_ptr_data = std::calloc(m_size = size, 1);
  }
  else
  {
    m_ptr_data = std::realloc(ptr, m_size = size);
  }

  if (m_ptr_data == nullptr)
  {
    throw std::bad_alloc();
  }

  if (ptr == nullptr)
  {
    memset(m_ptr_data, 0, m_size);
  }
  else if (clean)
  {
    memcpy_s(m_ptr_data, m_size, ptr, size);
  }

  return true;
}

bool Buffer::destroy()
{
  if (m_ptr_data != nullptr)
  {
    std::free(m_ptr_data);
  }

  m_ptr_data = nullptr;
  m_size  = 0;

  return true;
}

void Buffer::reset()
{
  this->destroy();
}

void Buffer::fill(const byte v)
{
  if (m_ptr_data != nullptr && m_size != 0)
  {
    memset(m_ptr_data, v, m_size);
  }
}

bool Buffer::resize(const size_t size)
{
  if (size == m_size)
  {
    return true;
  }

  return this->create(m_ptr_data, size, false);
}

bool Buffer::replace(const void* pData, const size_t size)
{
  if (this->create(nullptr, size))
  {
    if (pData != nullptr)
    {
      memcpy_s(m_ptr_data, m_size, pData, size);
    }
  }

  return true;
}

bool Buffer::replace(const Buffer& right)
{
  return this->replace(right.get_ptr_data(), right.get_size());
}

bool Buffer::empty() const
{
  return m_ptr_data == nullptr || m_size == 0;
}

bool Buffer::append(const void* pData, const size_t size)
{
  if (pData == nullptr || size == 0)
  {
    return false;
  }

  const size_t PrevSize = m_size;

  this->resize(m_size + size);

  memcpy_s(this->get_ptr_bytes() + PrevSize, size, pData, size);

  return true;
}

bool Buffer::append(const Buffer& right)
{
  return this->append(right.get_ptr_data(), right.get_size());
}

std::string Buffer::to_string_A() const
{
  return std::string(reinterpret_cast<const char*>(m_ptr_data), m_size / sizeof(char));
}

std::wstring Buffer::to_string_W() const
{
  return std::wstring(reinterpret_cast<const wchar*>(m_ptr_data), m_size / sizeof(wchar));
}

bool Buffer::save_to_file(const std::string& filePath)
{
  if (filePath.empty())
  {
    return false;
  }

  bool result = true;

  FileSystemA file(filePath, vu::FM_CREATEALWAY);
  result &= file.write(m_ptr_data, ulong(m_size));
  result &= file.close();

  return result;
}

bool Buffer::save_to_file(const std::wstring& filePath)
{
  const auto s = vu::to_string_A(filePath);
  return this->save_to_file(s);
}

} // namespace vu