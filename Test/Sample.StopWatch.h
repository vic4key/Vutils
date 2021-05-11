#pragma once

#include "Sample.h"

DEF_SAMPLE(StopWatch)
{
  vu::CScopeStopWatch logger(_T("StopWatch ->"), vu::CScopeStopWatch::Console);

  Sleep(200);
  logger.Log(_T("Test 1 : "));

  Sleep(300);
  logger.Log(_T("Test 2 : "));

  Sleep(500);
  logger.Log(_T("Test 3 : "));

  return vu::VU_OK;
}
