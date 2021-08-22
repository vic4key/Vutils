#pragma once

#include "Sample.h"

DEF_SAMPLE(StopWatch)
{
  vu::ScopeStopWatch logger(ts("StopWatch ->"), vu::ScopeStopWatch::console);

  Sleep(200);
  logger.log(ts("Test 1 : "));

  Sleep(300);
  logger.log(ts("Test 2 : "));

  Sleep(500);
  logger.log(ts("Test 3 : "));

  return vu::VU_OK;
}
