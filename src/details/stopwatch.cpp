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
  m_Reset = true;
  m_Count = 0;
  m_Delta = 0;
  m_DeltaHistory.clear();
}

CStopWatch::~CStopWatch()
{
  m_DeltaHistory.clear();
}

void CStopWatch::Start(bool reset)
{
  m_Count = std::clock();

  if (m_Reset = reset)
  {
    m_DeltaHistory.clear();
  }
}

const CStopWatch::TDuration CStopWatch::Stop()
{
  if (m_Count != 0)
  {
    m_Delta = std::clock() - m_Count;
    m_DeltaHistory.push_back(m_Delta);
  }

  return this->Duration();
}

const CStopWatch::TDuration CStopWatch::Duration()
{
  return TDuration(m_Delta, m_Delta / float(CLOCKS_PER_SEC));
}

const vu::CStopWatch::TDuration CStopWatch::Total()
{
  const auto PrevDelta = m_Delta;

  m_Delta = std::accumulate(m_DeltaHistory.cbegin(), m_DeltaHistory.cend(), 0);
  const auto duration = this->Duration();

  m_Delta = PrevDelta;

  return duration;
}

/**
 * CScopeStopWatchX
 */

CScopeStopWatchX::CScopeStopWatchX() : m_Activated(true)
{
  this->Start(true);
}

CScopeStopWatchX::~CScopeStopWatchX()
{
}

void CScopeStopWatchX::Start(bool reset)
{
  m_Watcher.Start(reset);
}

void CScopeStopWatchX::Stop()
{
  m_Watcher.Stop();
}

void CScopeStopWatchX::Active(bool state)
{
  m_Activated = state;
}

void CScopeStopWatchX::Reset()
{
  this->Stop();
  this->Start(false);
}

CScopeStopWatchA::CScopeStopWatchA(const std::string& prefix, const FnLogging fnLogging)
  : CScopeStopWatchX(), m_Prefix(prefix + ' '), m_fnLogging(fnLogging)
{
}

CScopeStopWatchA::~CScopeStopWatchA()
{
  this->Stop();

  if (m_Activated)
  {
    m_fnLogging(m_Prefix + "TOTAL : ", m_Watcher.Total());
  }
}

void CScopeStopWatchA::Log(const std::string& id)
{
  this->Stop();

  if (m_Activated)
  {
    m_fnLogging(m_Prefix + id, m_Watcher.Duration());
  }

  this->Start(false);
}

void CScopeStopWatchA::Message(const std::string& id, const CStopWatch::TDuration& duration)
{
  vu::msg_A(id + "%.3fs", duration.second);
}

void CScopeStopWatchA::Console(const std::string& id, const CStopWatch::TDuration& duration)
{
  char s[KB] = { 0 };
  memset(s, 0, sizeof(s));
  sprintf_s(s, lengthof(s), "%.3fs", duration.second);
  std::cout << id << s << std::endl;
}

CScopeStopWatchW::CScopeStopWatchW(const std::wstring& prefix, const FnLogging fnLogging)
  : CScopeStopWatchX(), m_Prefix(prefix + L' '),m_fnLogging(fnLogging)
{
}

CScopeStopWatchW::~CScopeStopWatchW()
{
  this->Stop();

  if (m_Activated)
  {
    m_fnLogging(m_Prefix + L"TOTAL : ", m_Watcher.Total());
  }
}

void CScopeStopWatchW::Log(const std::wstring& id)
{
  this->Stop();

  if (m_Activated)
  {
    m_fnLogging(m_Prefix + id, m_Watcher.Duration());
  }

  this->Start(false);
}

void CScopeStopWatchW::Message(const std::wstring& id, const CStopWatch::TDuration& duration)
{
  vu::msg_W(id + L"%.3fs", duration.second);
}

void CScopeStopWatchW::Console(const std::wstring& id, const CStopWatch::TDuration& duration)
{
  wchar_t s[KB] = { 0 };
  memset(s, 0, sizeof(s));
  swprintf_s(s, lengthof(s), L"%.3fs", duration.second);
  std::wcout << id << s << std::endl;
}

} // namespace vu