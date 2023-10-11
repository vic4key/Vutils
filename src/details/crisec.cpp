/**
 * @file   crisec.cpp
 * @author Vic P.
 * @brief  Implementation for Critical Section
 */

#include "Vutils.h"

namespace vu
{

ThreadLock::ThreadLock()
{
  memset(&m_cs, 0, sizeof(m_cs));
  InitializeCriticalSection(&m_cs);
}

ThreadLock::~ThreadLock()
{
  DeleteCriticalSection(&m_cs);
}

void vuapi ThreadLock::lock()
{
  EnterCriticalSection(&m_cs);
}

void vuapi ThreadLock::unlock()
{
  LeaveCriticalSection(&m_cs);
}

} // namespace vu