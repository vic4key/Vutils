/**
 * @file   crisec.cpp
 * @author Vic P.
 * @brief  Implementation for Critical Section
 */

#include "Vutils.h"

namespace vu
{

/**
 * ThreadLock
 */

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

/**
 * GlobalThreadLock
 */

std::map<int, std::unique_ptr<ThreadLock>> _GlobalThreadLock::m_list;

_GlobalThreadLock::_GlobalThreadLock(int thread_lock_id) : m_thread_lock_id(thread_lock_id)
{
  auto it = m_list.find(thread_lock_id);
  if (it == m_list.cend())
  {
    m_thread_lock_id = thread_lock_id;
    m_list[m_thread_lock_id].reset(new ThreadLock());
  }

  m_list[m_thread_lock_id]->lock();
}

_GlobalThreadLock::~_GlobalThreadLock()
{
  if (m_thread_lock_id != -1)
  {
    m_list[m_thread_lock_id]->unlock();
    //m_list.erase(m_thread_lock_id);
  }
}

/**
 * ThreadSignal
 */

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