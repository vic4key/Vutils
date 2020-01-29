/**
 * @file   binary.cpp
 * @author Vic P.
 * @brief  Implementation for Binary
 */

#include "Vutils.h"

namespace vu
{

CBinary::CBinary() : m_UsedSize(0), m_Size(0), m_pData(0) {}

CBinary::CBinary(const CBinary &right) : m_UsedSize(0), m_Size(0), m_pData(0)
{
  SetpData(right.m_pData, right.m_Size);
}

CBinary::CBinary(const void* pData, ulong ulSize) : m_UsedSize(0), m_Size(0), m_pData(0)
{
  SetpData(pData, ulSize);
}

CBinary::CBinary(ulong ulSize) : m_UsedSize(0), m_Size(0), m_pData(0)
{
  AdjustSize(ulSize);
}

CBinary::~CBinary()
{
  if (m_pData != nullptr)
  {
    delete[] (uchar*)m_pData;
  }
}

const CBinary& CBinary::operator=(const CBinary& right)
{
  if(this != &right)
  {
    SetpData(right.m_pData, right.m_Size);
  }

  return *this;
}

bool CBinary::operator==(const CBinary& right) const
{
  if(m_Size != right.m_Size)
  {
    return false;
  }

  return (memcmp(m_pData, right.m_pData, m_Size) == 0);
}

bool CBinary::operator!=(const CBinary& right) const
{
  return (!(*this == right));
}

const ulong CBinary::GetSize() const
{
  return m_Size;
}

void CBinary::SetUsedSize(ulong ulUsedSize)
{
  if (ulUsedSize <= m_Size)
  {
    m_UsedSize = ulUsedSize;
  }
}

const ulong CBinary::GetUsedSize() const
{
  return m_UsedSize;
}

void* CBinary::GetpData()
{
  return m_pData;
}

const void* CBinary::GetpData() const
{
  return m_pData;
}

void CBinary::SetpData(const void* pData, ulong ulSize)
{
  AdjustSize(ulSize);
  memcpy(m_pData, pData, ulSize);
  m_Size = ulSize;
  m_UsedSize = ulSize;
}

void CBinary::AdjustSize(ulong ulSize)
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

} // namespace vu