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

ThreadSignal::ThreadSignal(bool waiting)
{
  m_event = CreateEventA(nullptr, TRUE, waiting, nullptr);
}

ThreadSignal::ThreadSignal(const ThreadSignal& right)
{
  m_event = CreateEventA(nullptr, TRUE, right.get(), nullptr);
}

ThreadSignal::~ThreadSignal()
{
  CloseHandle(m_event);
}

const ThreadSignal& ThreadSignal::operator=(const ThreadSignal& right)
{
  this->set(right.get());
  return *this;
}

const ThreadSignal& ThreadSignal::operator=(bool waiting)
{
  this->set(waiting);
  return *this;
}

ThreadSignal::operator bool() const
{
  return this->get();
}

bool ThreadSignal::operator==(ThreadSignal& right)
{
  return right.get() == this->get();
}

bool ThreadSignal::operator!=(ThreadSignal& right)
{
  return !this->operator==(right);
}

bool ThreadSignal::operator==(bool waiting)
{
  return this->get() == waiting;
}

bool ThreadSignal::operator!=(bool waiting)
{
  return !this->operator==(waiting);
}

void ThreadSignal::set(bool state)
{
  if (state)
  {
    SetEvent(m_event);
  }
  else
  {
    ResetEvent(m_event);
  }
}

void ThreadSignal::notify()
{
  this->set(true);
}

bool ThreadSignal::get() const
{
  return this->wait(0);
}

bool ThreadSignal::wait(ulong time_out) const
{
  return WaitForSingleObject(m_event, time_out) == WAIT_OBJECT_0;
}

} // namespace vu