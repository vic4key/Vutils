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

CStopWatch::CStopWatch()
{
  m_reset = true;
  m_count = 0;
  m_delta = 0;
  m_delta_history.clear();
}

CStopWatch::~CStopWatch()
{
  m_delta_history.clear();
}

void CStopWatch::start(bool reset)
{
  m_count = std::clock();

  if (m_reset = reset)
  {
    m_delta_history.clear();
  }
}

const CStopWatch::TDuration CStopWatch::stop()
{
  if (m_count != 0)
  {
    m_delta = std::clock() - m_count;
    m_delta_history.push_back(m_delta);
  }

  return this->duration();
}

const CStopWatch::TDuration CStopWatch::duration()
{
  return TDuration(m_delta, m_delta / float(CLOCKS_PER_SEC));
}

const vu::CStopWatch::TDuration CStopWatch::total()
{
  const auto PrevDelta = m_delta;

  m_delta = std::accumulate(m_delta_history.cbegin(), m_delta_history.cend(), 0);
  const auto duration = this->duration();

  m_delta = PrevDelta;

  return duration;
}

/**
 * CScopeStopWatchX
 */

CScopeStopWatchX::CScopeStopWatchX() : m_activated(true)
{
  this->start(true);
}

CScopeStopWatchX::~CScopeStopWatchX()
{
}

void CScopeStopWatchX::start(bool reset)
{
  m_watcher.start(reset);
}

void CScopeStopWatchX::stop()
{
  m_watcher.stop();
}

void CScopeStopWatchX::active(bool state)
{
  m_activated = state;
}

void CScopeStopWatchX::reset()
{
  this->stop();
  this->start(false);
}

CScopeStopWatchA::CScopeStopWatchA(const std::string& prefix, const FnLogging fn_logging)
  : CScopeStopWatchX(), m_prefix(prefix + ' '), m_fn_logging(fn_logging)
{
}

CScopeStopWatchA::~CScopeStopWatchA()
{
  this->stop();

  if (m_activated)
  {
    m_fn_logging(m_prefix + "TOTAL : ", m_watcher.total());
  }
}

void CScopeStopWatchA::Log(const std::string& id)
{
  this->stop();

  if (m_activated)
  {
    m_fn_logging(m_prefix + id, m_watcher.duration());
  }

  this->start(false);
}

void CScopeStopWatchA::message(const std::string& id, const CStopWatch::TDuration& duration)
{
  vu::msg_debug_A(id + "%.3fs", duration.second);
}

void CScopeStopWatchA::console(const std::string& id, const CStopWatch::TDuration& duration)
{
  char s[KB] = { 0 };
  memset(s, 0, sizeof(s));
  sprintf_s(s, lengthof(s), "%.3fs", duration.second);
  std::cout << id << s << std::endl;
}

CScopeStopWatchW::CScopeStopWatchW(const std::wstring& prefix, const FnLogging fn_logging)
  : CScopeStopWatchX(), m_prefix(prefix + L' '),m_fn_logging(fn_logging)
{
}

CScopeStopWatchW::~CScopeStopWatchW()
{
  this->stop();

  if (m_activated)
  {
    m_fn_logging(m_prefix + L"TOTAL : ", m_watcher.total());
  }
}

void CScopeStopWatchW::log(const std::wstring& id)
{
  this->stop();

  if (m_activated)
  {
    m_fn_logging(m_prefix + id, m_watcher.duration());
  }

  this->start(false);
}

void CScopeStopWatchW::message(const std::wstring& id, const CStopWatch::TDuration& duration)
{
  vu::msg_debug_W(id + L"%.3fs", duration.second);
}

void CScopeStopWatchW::console(const std::wstring& id, const CStopWatch::TDuration& duration)
{
  wchar_t s[KB] = { 0 };
  memset(s, 0, sizeof(s));
  swprintf_s(s, lengthof(s), L"%.3fs", duration.second);
  std::wcout << id << s << std::endl;
}

} // namespace vu