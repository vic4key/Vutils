/**
 * @file   stopwatch.cpp
 * @author Vic P.
 * @brief  Implementation for Stop Watch
 */

#include "Vutils.h"

#include <numeric>

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

CScopeStopWatchA::CScopeStopWatchA(const std::string& prefix, const FnLogging fnLogging)
  : CScopeStopWatchX(), m_Prefix(prefix), m_fnLogging(fnLogging)
{
}

CScopeStopWatchA::~CScopeStopWatchA()
{
  this->Stop();

  if (m_Activated)
  {
    m_fnLogging(m_Prefix + "Total", m_Watcher.Total());
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

CScopeStopWatchW::CScopeStopWatchW(const std::wstring& prefix, const FnLogging fnLogging)
  : CScopeStopWatchX(), m_Prefix(prefix),m_fnLogging(fnLogging)
{
}

CScopeStopWatchW::~CScopeStopWatchW()
{
  this->Stop();

  if (m_Activated)
  {
    m_fnLogging(m_Prefix + L"Total", m_Watcher.Total());
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

} // namespace vu