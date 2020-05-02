#pragma once

#include "Sample.h"

DEF_SAMPLE(Process)
{
  #ifdef _WIN64
  #define PROCESS_NAME _T("x64dbg.exe")
  #else  // _WIN32
  #define PROCESS_NAME _T("x32dbg.exe")
  #endif // _WIN64

  auto PIDs = vu::NameToPID(PROCESS_NAME);
  assert(!PIDs.empty());

  vu::CProcess process(PIDs.back());
  assert(process.Ready());

  auto cpu  = process.GetCPUInformation();
  auto mem  = process.GetMemoryInformation();
  auto time = process.GetTimeInformation();
  auto io   = process.GetIOInformation();

  std::cout << "CPU : " << cpu.Usage << std::endl;
  std::cout << "WS  : " << vu::FormatBytesA(mem.WorkingSetSize) << std::endl;
 
  return vu::VU_OK;
}
