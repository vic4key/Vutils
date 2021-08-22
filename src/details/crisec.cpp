/**
 * @file   crisec.cpp
 * @author Vic P.
 * @brief  Implementation for Critical Section
 */

#include "Vutils.h"

namespace vu
{

CriticalSection::CriticalSection()
{
  memset(&m_cs, 0, sizeof(m_cs));
}

CriticalSection::~CriticalSection()
{
}

void vuapi CriticalSection::initialize()
{
  InitializeCriticalSection(&m_cs);
}

void vuapi CriticalSection::enter()
{
  EnterCriticalSection(&m_cs);
}

void vuapi CriticalSection::leave()
{
  LeaveCriticalSection(&m_cs);
}

void vuapi CriticalSection::destroy()
{
  DeleteCriticalSection(&m_cs);
}

} // namespace vu