#pragma once

#include "Sample.h"

DEF_SAMPLE(StopWatch)
{
  vu::CScopeStopWatch logger(ts("StopWatch ->"), vu::CScopeStopWatch::Console);

  Sleep(200);
  logger.Log(ts("Test 1 : "));

  Sleep(300);
  logger.Log(ts("Test 2 : "));

  Sleep(500);
  logger.Log(ts("Test 3 : "));

  return vu::VU_OK;
}
