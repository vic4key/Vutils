#pragma once

#include "Sample.h"

DEF_SAMPLE(StopWatch)
{
  vu::CScopeStopWatch logger(ts("StopWatch ->"), vu::CScopeStopWatch::console);

  Sleep(200);
  logger.log(ts("Test 1 : "));

  Sleep(300);
  logger.log(ts("Test 2 : "));

  Sleep(500);
  logger.log(ts("Test 3 : "));

  return vu::VU_OK;
}
