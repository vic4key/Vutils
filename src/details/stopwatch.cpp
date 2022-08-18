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
  this->start(true);
}

void ScopeStopWatchX::lap()
{
  this->stop();
  this->start(false);
}

/**
 * ScopeStopWatchA
 */

ScopeStopWatchA::ScopeStopWatchA(
  const std::string& prefix, const std::string& suffix, const FnLogging fn_logging)
  : ScopeStopWatchX(), m_prefix(prefix + ' '), m_suffix(suffix), m_fn_logging(fn_logging)
{
}

ScopeStopWatchA::~ScopeStopWatchA()
{
  this->stop();

  if (m_activated)
  {
    m_fn_logging(m_prefix + "TOTAL" + m_suffix, m_watcher.total());
  }
}

extern std::string vuapi format_vl_A(const std::string format, va_list args);

void ScopeStopWatchA::log(const std::string format, ...)
{
  this->stop();

  if (m_activated)
  {
    va_list args;
    va_start(args, format);
    auto text = format_vl_A(format, args);
    va_end(args);

    m_fn_logging(m_prefix + text + m_suffix, m_watcher.duration());
  }

  this->start(false);
}

void ScopeStopWatchA::message(const std::string& text, const StopWatch::TDuration& duration)
{
  vu::msg_debug_A(text + "%.3f", duration.second);
}

void ScopeStopWatchA::console(const std::string& text, const StopWatch::TDuration& duration)
{
  char time[KB] = { 0 };
  memset(time, 0, sizeof(time));
  sprintf_s(time, lengthof(time), "%.3f", duration.second);
  std::cout << text << time << std::endl;
}

/**
 * ScopeStopWatchW
 */

ScopeStopWatchW::ScopeStopWatchW(
  const std::wstring& prefix, const std::wstring& suffix, const FnLogging fn_logging)
  : ScopeStopWatchX(), m_prefix(prefix + L' '), m_suffix(suffix), m_fn_logging(fn_logging)
{
}

ScopeStopWatchW::~ScopeStopWatchW()
{
  this->stop();

  if (m_activated)
  {
    m_fn_logging(m_prefix + L"TOTAL" + m_suffix, m_watcher.total());
  }
}

extern std::wstring vuapi format_vl_W(const std::wstring format, va_list args);

void ScopeStopWatchW::log(const std::wstring format, ...)
{
  this->stop();

  if (m_activated)
  {
    va_list args;
    va_start(args, format);
    auto text = format_vl_W(format, args);
    va_end(args);

    m_fn_logging(m_prefix + text + m_suffix, m_watcher.duration());
  }

  this->start(false);
}

void ScopeStopWatchW::message(const std::wstring& text, const StopWatch::TDuration& duration)
{
  vu::msg_debug_W(text + L"%.3f", duration.second);
}

void ScopeStopWatchW::console(const std::wstring& text, const StopWatch::TDuration& duration)
{
  wchar_t time[KB] = { 0 };
  memset(time, 0, sizeof(time));
  swprintf_s(time, lengthof(time), L"%.3f", duration.second);
  std::wcout << text << time << std::endl;
}

} // namespace vu