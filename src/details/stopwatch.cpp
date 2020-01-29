/**
 * @file   stopwatch.cpp
 * @author Vic P.
 * @brief  Implementation for Stop Watch
 */

#include "Vutils.h"

namespace vu
{

CStopWatch::CStopWatch()
{
  m_Reset = true;
  m_Count = 0;
  m_Delta = 0;
  m_Duration = 0;
  m_DeltaHistory.clear();
}

CStopWatch::~CStopWatch()
{
  m_DeltaHistory.clear();
}

void CStopWatch::Start(bool reset)
{
  m_Reset = reset;

  if (m_Reset)
  {
    m_Duration = 0;
    m_DeltaHistory.clear();
  }

  m_Count = std::clock();
}

const CStopWatch::TDuration CStopWatch::Stop()
{
  m_Delta = std::clock() - m_Count;

  if (!m_Reset) m_DeltaHistory.push_back(m_Delta);

  return this->Duration();
}

const CStopWatch::TDuration CStopWatch::Duration()
{
  if (!m_Reset)
  {
    auto N = m_DeltaHistory.size();
    for (decltype(N) i = 0; N >= 2, i < N - 1; i++) m_Delta += m_DeltaHistory.at(i);
  }

  m_Duration = (float(m_Delta) / float(CLOCKS_PER_SEC));

  TDuration duration(m_Delta, m_Duration);

  return duration;
}

} // namespace vu