#pragma once

#include "Sample.h"

DEF_SAMPLE(Process)
{
  #ifdef _WIN64
  #define PROCESS_NAME _T("x64dbg.exe")
  #else  // _WIN32
  #define PROCESS_NAME _T("x32dbg.exe")
  #endif // _WIN64

  auto PIDs = vu::name_to_pid(PROCESS_NAME);
  assert(!PIDs.empty());

  vu::CProcess process;
  process.Attach(PIDs.back());
  assert(process.Ready());

  auto cpu  = process.GetCPUInformation();
  auto mem  = process.GetMemoryInformation();
  // auto time = process.GetTimeInformation();
  // auto io   = process.GetIOInformation();

  std::tcout << _T("CPU : ") << cpu.Usage << std::endl;
  std::tcout << _T("WS  : ") << vu::format_bytes(mem.WorkingSetSize) << std::endl;

  for (const auto& thread : process.GetThreads())
  {
    static int idx = 0;
    std::tcout << ++idx << ". TID = " << thread.th32ThreadID << std::endl;
    std::tcout << _T("\tPID   = ") << thread.th32OwnerProcessID << std::endl;
    std::tcout << _T("\tUsage = ") << thread.cntUsage << std::endl;
    std::tcout << _T("\tBase Priority  = ") << thread.tpBasePri << std::endl;
    std::tcout << _T("\tDelta Priority = ") << thread.tpDeltaPri << std::endl;
    std::tcout << std::endl;
  }

  for (const auto& module : process.GetModules())
  {
    static int idx = 0;
    std::cout << ++idx << ". MID = " << LPVOID(module.hModule) << std::endl;
    std::cout << "\tBase Address = " << LPVOID(module.modBaseAddr) << std::endl;
    std::cout << "\tBase Size = " << vu::format_bytes_A(module.modBaseSize) << std::endl;
    std::cout << "\tModule = " << module.szModule << std::endl;
    std::cout << std::endl;
  }

  for (const auto& e : process.GetMemories())
  {
    static int i = 0;
    std::cout << std::dec << ++i << ". ";
    std::cout << std::hex << e.BaseAddress << " - " << vu::format_bytes_A(e.RegionSize) << std::endl;
    std::cout << std::hex << "\tProtect = " << e.Protect << std::endl;
    std::cout << std::hex << "\tState   = " << e.State << std::endl;
    std::cout << std::hex << "\tType    = " << e.Type << std::endl;
    std::cout << std::endl;
  }

  return vu::VU_OK;
}
