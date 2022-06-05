/**
 * @file   debouncer.cpp
 * @author Vic P.
 * @brief  Implementation for Debouncer
 */

#include "Vutils.h"

namespace vu
{

ulongptr Debouncer::m_using_timer_id;
std::unordered_map<ulongptr, Debouncer::Timer*> Debouncer::m_timers;

bool Debouncer::exists(ulongptr id)
{
  auto it = m_timers.find(id);
  return it != m_timers.cend();
}

void Debouncer::remove(ulongptr id)
{
  auto it = m_timers.find(id);
  if (it == m_timers.cend())
  {
    return;
  }

  if (it->second != nullptr)
  {
    if (it->second->m_handle != 0)
    {
      ::KillTimer(0, it->second->m_handle);
    }

    delete it->second;
  }

  m_timers.erase(it);
}

void Debouncer::cleanup()
{
  for (auto& e : m_timers)
  {
    if (e.second != nullptr)
    {
      if (e.second->m_handle != 0)
      {
        ::KillTimer(0, e.second->m_handle);
      }

      delete e.second;
    }
  }

  m_timers.clear();
}

void Debouncer::debounce(ulongptr id, ulong elapse, std::function<void()> fn)
{
  m_using_timer_id = id;
  Timer* ptr_timer = nullptr;

  auto it = m_timers.find(m_using_timer_id);
  if (it != m_timers.cend())
  {
    ptr_timer = it->second;
  }

  if (ptr_timer == nullptr)
  {
    m_timers[m_using_timer_id] = new Timer();
    ptr_timer = m_timers[m_using_timer_id];
  }

  if (ptr_timer == nullptr)
  {
    return;
  }

  if (ptr_timer->m_handle != 0)
  {
    ::KillTimer(0, ptr_timer->m_handle);
  }

  ptr_timer->m_function = fn;
  ptr_timer->m_handle = ::SetTimer(0, m_using_timer_id, elapse, fn_timer_proc);
}

VOID CALLBACK Debouncer::fn_timer_proc(HWND hWnd, UINT message, UINT_PTR idTimer, DWORD dwTime)
{
  auto it = m_timers.find(m_using_timer_id);
  if (it == m_timers.cend())
  {
    return;
  }

  if (it->second == nullptr)
  {
    return;
  }

  if (it->second->m_function != nullptr)
  {
    it->second->m_function();
  }

  ::KillTimer(0, it->second->m_handle);
}

} // namespace vu