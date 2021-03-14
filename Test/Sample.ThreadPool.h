#pragma once

#include "Sample.h"

DEF_SAMPLE(ThreadPool)
{
  const auto fn = []()
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << VU_FUNC_NAME << std::endl;
  };

  vu::CScopeStopWatch logger(_T("ThreadPool => "), vu::ConsoleLogging);

  // Single-threading

  logger.Reset();

  for (int i = 0; i < 10; i++)
  {
    fn();
  }

  logger.Log(_T("Taken : "));

  // Default Multi-threading

  logger.Reset();

  vu::CThreadPool pool;
  for (int i = 0; i < 10; i++)
  {
    pool.AddTask(fn);
  }
  pool.Launch();

  logger.Log(_T("Taken : "));

  // STL Multi-threading

  class CSampleTask : public vu::CSTLThreadT<std::vector<int>>
  {
  public:
    CSampleTask(std::vector<int>& items) : CSTLThreadT(items)
    {
      m_Results.resize(this->Iterations());
    };

    int Result()
    {
      return std::accumulate(m_Results.cbegin(), m_Results.cend(), 0);
    }

    virtual const vu::eReturn Task(int& item, int iteration, int threadid)
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << VU_FUNC_NAME << std::endl;

      m_Results[iteration] += item;

      return vu::eReturn::Ok;
    }

  private:
    std::vector<int> m_Results;
  };

  logger.Reset();

  std::vector<int> items(10);
  std::iota(items.begin(), items.end(), 0);

  CSampleTask task(items);
  task.Launch();
  std::cout << "Result is " << task.Result() << std::endl;

  logger.Log(_T("Taken : "));

  return vu::VU_OK;
}
