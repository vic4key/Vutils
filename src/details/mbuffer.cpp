/**
 * @file   mbuffer.cpp
 * @author Vic P.
 * @brief  Implementation for Memory Buffer
 */

#include "Vutils.h"

namespace vu
{

CBuffer::CBuffer()
{
  this->Create(nullptr, 0);
}

CBuffer::CBuffer(const size_t size)
{
  this->Create(nullptr, size);
}

CBuffer::CBuffer(const void* pData, const size_t size)
{
  this->Replace(pData, size);
}

CBuffer::CBuffer(const CBuffer& right)
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

byte& CBuffer::operator[](const size_t offset) const
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

  m_pData = std::realloc(ptr, m_Size = size);
  if (m_pData == nullptr)
  {
    throw std::bad_alloc();
  }

  if (ptr == nullptr)
  {
    memset(m_pData, 0, m_Size);
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
  const auto s = ToStringA(filePath);
  return this->SaveAsFile(s);
}

} // namespace vu