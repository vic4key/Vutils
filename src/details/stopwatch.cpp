/**
 * @file   stopwatch.cpp
 * @author Vic P.
 * @brief  Implementation for Stop Watch
 */

#include "Vutils.h"

#include <cstdio>
#include <cwchar>
#include <numeric>
#include <iostream>

namespace vu
{

StopWatch::StopWatch()
{
  m_reset = true;
  m_count = 0;
  m_delta = 0;
  m_delta_history.clear();
}

StopWatch::~StopWatch()
{
  m_delta_history.clear();
}

void StopWatch::start(bool reset)
{
  m_count = std::clock();

  if (m_reset = reset)
  {
    m_delta_history.clear();
  }
}

const StopWatch::TDuration StopWatch::stop()
{
  if (m_count != 0)
  {
    m_delta = std::clock() - m_count;
    m_delta_history.push_back(m_delta);
  }

  return this->duration();
}

const StopWatch::TDuration StopWatch::duration()
{
  return TDuration(m_delta, m_delta / float(CLOCKS_PER_SEC));
}

const vu::StopWatch::TDuration StopWatch::total()
{
  const auto PrevDelta = m_delta;

  m_delta = std::accumulate(m_delta_history.cbegin(), m_delta_history.cend(), 0);
  const auto duration = this->duration();

  m_delta = PrevDelta;

  return duration;
}

/**
 * ScopeStopWatchX
 */

ScopeStopWatchX::ScopeStopWatchX() : m_activated(true)
{
  this->start(true);
}

ScopeStopWatchX::~ScopeStopWatchX()
{
}

void ScopeStopWatchX::start(bool reset)
{
  m_watcher.start(reset);
}

void ScopeStopWatchX::stop()
{
  m_watcher.stop();
}

void ScopeStopWatchX::active(bool state)
{
  m_activated = state;
}

void ScopeStopWatchX::reset()
{
  this->stop();
  this->start(false);
}

ScopeStopWatchA::ScopeStopWatchA(const std::string& prefix, const FnLogging fn_logging)
  : ScopeStopWatchX(), m_prefix(prefix + ' '), m_fn_logging(fn_logging)
{
}

ScopeStopWatchA::~ScopeStopWatchA()
{
  this->stop();

  if (m_activated)
  {
    m_fn_logging(m_prefix + "TOTAL : ", m_watcher.total());
  }
}

void ScopeStopWatchA::log(const std::string& id)
{
  this->stop();

  if (m_activated)
  {
    m_fn_logging(m_prefix + id, m_watcher.duration());
  }

  this->start(false);
}

void ScopeStopWatchA::message(const std::string& id, const StopWatch::TDuration& duration)
{
  vu::msg_debug_A(id + "%.3fs", duration.second);
}

void ScopeStopWatchA::console(const std::string& id, const StopWatch::TDuration& duration)
{
  char s[KB] = { 0 };
  memset(s, 0, sizeof(s));
  sprintf_s(s, lengthof(s), "%.3fs", duration.second);
  std::cout << id << s << std::endl;
}

ScopeStopWatchW::ScopeStopWatchW(const std::wstring& prefix, const FnLogging fn_logging)
  : ScopeStopWatchX(), m_prefix(prefix + L' '),m_fn_logging(fn_logging)
{
}

ScopeStopWatchW::~ScopeStopWatchW()
{
  this->stop();

  if (m_activated)
  {
    m_fn_logging(m_prefix + L"TOTAL : ", m_watcher.total());
  }
}

void ScopeStopWatchW::log(const std::wstring& id)
{
  this->stop();

  if (m_activated)
  {
    m_fn_logging(m_prefix + id, m_watcher.duration());
  }

  this->start(false);
}

void ScopeStopWatchW::message(const std::wstring& id, const StopWatch::TDuration& duration)
{
  vu::msg_debug_W(id + L"%.3fs", duration.second);
}

void ScopeStopWatchW::console(const std::wstring& id, const StopWatch::TDuration& duration)
{
  wchar_t s[KB] = { 0 };
  memset(s, 0, sizeof(s));
  swprintf_s(s, lengthof(s), L"%.3fs", duration.second);
  std::wcout << id << s << std::endl;
}

} // namespace vu