/**
 * @file   binary.cpp
 * @author Vic P.
 * @brief  Implementation for Binary
 */

#include "Vutils.h"

namespace vu
{

CBuffer::CBuffer() : m_UsedSize(0), m_Size(0), m_pData(0) {}

CBuffer::CBuffer(const CBuffer &right) : m_UsedSize(0), m_Size(0), m_pData(0)
{
  SetpData(right.m_pData, right.m_Size);
}

CBuffer::CBuffer(const void* pData, ulong ulSize) : m_UsedSize(0), m_Size(0), m_pData(0)
{
  SetpData(pData, ulSize);
}

CBuffer::CBuffer(ulong ulSize) : m_UsedSize(0), m_Size(0), m_pData(0)
{
  AdjustSize(ulSize);
}

CBuffer::~CBuffer()
{
  if (m_pData != nullptr)
  {
    delete[] (uchar*)m_pData;
  }
}

const CBuffer& CBuffer::operator=(const CBuffer& right)
{
  if(this != &right)
  {
    SetpData(right.m_pData, right.m_Size);
  }

  return *this;
}

bool CBuffer::operator==(const CBuffer& right) const
{
  if(m_Size != right.m_Size)
  {
    return false;
  }

  return (memcmp(m_pData, right.m_pData, m_Size) == 0);
}

bool CBuffer::operator!=(const CBuffer& right) const
{
  return (!(*this == right));
}

const ulong CBuffer::GetSize() const
{
  return m_Size;
}

void CBuffer::SetUsedSize(ulong ulUsedSize)
{
  if (ulUsedSize <= m_Size)
  {
    m_UsedSize = ulUsedSize;
  }
}

const ulong CBuffer::GetUsedSize() const
{
  return m_UsedSize;
}

void* CBuffer::GetpData()
{
  return m_pData;
}

const void* CBuffer::GetpData() const
{
  return m_pData;
}

void CBuffer::SetpData(const void* pData, ulong ulSize)
{
  AdjustSize(ulSize);
  memcpy(m_pData, pData, ulSize);
  m_Size = ulSize;
  m_UsedSize = ulSize;
}

void CBuffer::AdjustSize(ulong ulSize)
{
  if(ulSize > m_Size)
  {
    if(m_pData != nullptr)
    {
      delete[] (uchar*)m_pData;
    }

    m_pData = new uchar[ulSize];
    if (m_pData != nullptr)
    {
      m_Size = ulSize;
      m_UsedSize = ulSize;
      memset(m_pData, 0, m_Size*sizeof(uchar));
    }
  }
}

bool CBuffer::SaveAsFile(const std::string& filePath)
{
  if (filePath.empty())
  {
    return false;
  }

  bool result = true; 

  CFileSystemA file(filePath, vu::FM_CREATEALWAY);
  result &= file.Write(m_pData, m_Size);
  result &= file.Close();

  return result;
}

bool CBuffer::SaveAsFile(const std::wstring& filePath)
{
  const auto s = ToStringA(filePath);
  return SaveAsFile(s);
}

} // namespace vu