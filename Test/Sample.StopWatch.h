#pragma once

#include "Sample.h"

DEF_SAMPLE(StopWatch)
{
  vu::CScopeStopWatch watcher(_T("StopWatch -> "));

  Sleep(200);
  watcher.Log(_T("Test 1"));

  Sleep(300);
  watcher.Log(_T("Test 2"));

  Sleep(500);
  watcher.Log(_T("Test 3"));

  return vu::VU_OK;
}
