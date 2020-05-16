#pragma once

#include "Sample.h"

#include <thread>

#include <vu>

DEF_SAMPLE(ThreadPool)
{
  const auto fn = []()
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << VU_FUNC_NAME << std::endl;
  };

  vu::CStopWatch sw;

  sw.Start();
  for (int i = 0; i < 10; i++)
  {
    fn();
  }
  sw.Stop();
  std::cout << "Total Time : " << sw.Duration().second << " seconds" << std::endl;

  sw.Start();
  vu::CThreadPool pool;
  for (int i = 0; i < 10; i++)
  {
    pool.AddTask(fn);
  }
  pool.Launch();
  sw.Stop();
  std::cout << "Total Time : " << sw.Duration().second << " seconds" << std::endl;

  return vu::VU_OK;
}
