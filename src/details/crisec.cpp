/**
 * @file   crisec.cpp
 * @author Vic P.
 * @brief  Implementation for Critical Section
 */

#include "Vutils.h"

namespace vu
{

CCriticalSection::CCriticalSection()
{
  memset(&m_cs, 0, sizeof(m_cs));
}

CCriticalSection::~CCriticalSection()
{
}

void vuapi CCriticalSection::initialize()
{
  InitializeCriticalSection(&m_cs);
}

void vuapi CCriticalSection::enter()
{
  EnterCriticalSection(&m_cs);
}

void vuapi CCriticalSection::leave()
{
  LeaveCriticalSection(&m_cs);
}

void vuapi CCriticalSection::destroy()
{
  DeleteCriticalSection(&m_cs);
}

} // namespace vu