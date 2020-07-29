/**
 * @file   mbuffer.cpp
 * @author Vic P.
 * @brief  Implementation for Memory Buffer
 */

#include "Vutils.h"

namespace vu
{

CBuffer::CBuffer() : m_pData(nullptr), m_Size(0)
{
  this->Create(nullptr, 0);
}

CBuffer::CBuffer(const size_t size) : m_pData(nullptr), m_Size(0)
{
  this->Create(nullptr, size);
}

CBuffer::CBuffer(const void* pData, const size_t size) : m_pData(nullptr), m_Size(0)
{
  this->Replace(pData, size);
}

CBuffer::CBuffer(const CBuffer& right) : m_pData(nullptr), m_Size(0)
{
  *this = right;
}

CBuffer::~CBuffer()
{
  this->Delete();
}

const CBuffer& CBuffer::operator=(const CBuffer& right)
{
  if (*this != right)
  {
    if (this->Create(nullptr, right.m_Size))
    {
      if (right.m_pData != nullptr)
      {
        memcpy_s(m_pData, m_Size, right.m_pData, right.m_Size);
      }
    }
  }

  return *this;
}

bool CBuffer::operator==(const CBuffer& right) const
{
  if (m_Size != right.m_Size)
  {
    return false;
  }

  return memcmp(m_pData, right.m_pData, m_Size) == 0;
}

bool CBuffer::operator!=(const CBuffer& right) const
{
  return !(*this == right);
}

byte& CBuffer::operator[](const size_t offset)
{
  if (m_pData == nullptr)
  {
    throw std::runtime_error(static_cast<const char*>("invalid pointer"));
  }

  if (m_Size == 0 || offset >= m_Size)
  {
    throw std::out_of_range(static_cast<const char*>("invalid size or offset"));
  }

  return static_cast<byte*>(m_pData)[offset];
}

CBuffer CBuffer::operator()(int begin, int end) const
{
  return this->Slice(begin, end);
}

size_t CBuffer::Find(const void* pdata, const size_t size) const
{
  size_t result = -1;

  if (m_pData == nullptr || m_Size == 0 || pdata == nullptr || size == 0 || m_Size < size)
  {
    return result;
  }

  const auto pbytes = this->GetpBytes();

  for (size_t i = 0; i <= m_Size - size; i++)
  {
    if (memcmp(reinterpret_cast<const void*>(pbytes + i), pdata, size) == 0)
    {
      result = i;
      break;
    }
  }

  return result;
}

bool CBuffer::Match(const void* pdata, const size_t size) const
{
  return this->Find(pdata, size) != -1;
}

CBuffer CBuffer::Till(const void* pdata, const size_t size) const
{
  CBuffer result;

  size_t offset = this->Find(pdata, size);
  if (offset > 0)
  {
    result.Create(m_pData, offset);
  }

  return result;
}

CBuffer CBuffer::Slice(int begin, int end) const
{
  CBuffer result;

  if (m_pData == nullptr || m_Size == 0)
  {
    return result;
  }

  if (begin < 0)
  {
    begin = static_cast<int>(m_Size) + begin;
  }

  if (end < 0)
  {
    end = static_cast<int>(m_Size) + end;
  }

  if (begin < 0 || end < 0 || begin > m_Size || end > m_Size || begin > end)
  {
    return result;
  }

  int size = end - begin;
  if (size <= 0 || size > m_Size)
  {
    return result;
  }

  result.Create(this->GetpBytes() + begin, size);

  return result;
}

byte* CBuffer::GetpBytes() const
{
  return static_cast<byte*>(m_pData);
}

void* CBuffer::GetpData() const
{
  return m_pData;
}

size_t CBuffer::GetSize() const
{
  return m_Size;
}

bool CBuffer::Create(void* ptr, const size_t size, const bool clean)
{
  if (clean || size == 0)
  {
    this->Delete();
  }

  if (size == 0)
  {
    return false;
  }

  if (clean)
  {
    m_pData = std::calloc(m_Size = size, 1);
  }
  else
  {
    m_pData = std::realloc(ptr, m_Size = size);
  }

  if (m_pData == nullptr)
  {
    throw std::bad_alloc();
  }

  if (ptr == nullptr)
  {
    memset(m_pData, 0, m_Size);
  }
  else
  {
    memcpy_s(m_pData, m_Size, ptr, size);
  }

  return true;
}

bool CBuffer::Delete()
{
  if (m_pData != nullptr)
  {
    std::free(m_pData);
  }

  m_pData = nullptr;
  m_Size  = 0;

  return true;
}

void CBuffer::Reset()
{
  this->Delete();
}

void CBuffer::Fill(const byte v)
{
  if (m_pData != nullptr && m_Size != 0)
  {
    memset(m_pData, v, m_Size);
  }
}

bool CBuffer::Resize(const size_t size)
{
  if (size == m_Size)
  {
    return true;
  }

  return this->Create(m_pData, size, false);
}

bool CBuffer::Replace(const void* pData, const size_t size)
{
  if (this->Create(nullptr, size))
  {
    if (pData != nullptr)
    {
      memcpy_s(m_pData, m_Size, pData, size);
    }
  }

  return true;
}

bool CBuffer::Replace(const CBuffer& right)
{
  return this->Replace(right.GetpData(), right.GetSize());
}

bool CBuffer::Empty() const
{
  return m_pData == nullptr || m_Size == 0;
}

bool CBuffer::Append(const void* pData, const size_t size)
{
  if (pData == nullptr || size == 0)
  {
    return false;
  }

  const size_t PrevSize = m_Size;

  this->Resize(m_Size + size);

  memcpy_s(this->GetpBytes() + PrevSize, size, pData, size);

  return true;
}

bool CBuffer::Append(const CBuffer& right)
{
  return this->Append(right.GetpData(), right.GetSize());
}

std::string CBuffer::ToStringA() const
{
  return std::string(reinterpret_cast<const char*>(m_pData), m_Size / sizeof(char));
}

std::wstring CBuffer::ToStringW() const
{
  return std::wstring(reinterpret_cast<const wchar*>(m_pData), m_Size / sizeof(wchar));
}

bool CBuffer::SaveAsFile(const std::string& filePath)
{
  if (filePath.empty())
  {
    return false;
  }

  bool result = true;

  CFileSystemA file(filePath, vu::FM_CREATEALWAY);
  result &= file.Write(m_pData, ulong(m_Size));
  result &= file.Close();

  return result;
}

bool CBuffer::SaveAsFile(const std::wstring& filePath)
{
  const auto s = vu::ToStringA(filePath);
  return this->SaveAsFile(s);
}

} // namespace vu