#pragma once

#include "Sample.h"

DEF_SAMPLE(Process)
{
  #ifdef _WIN64
  #define PROCESS_NAME ts("x64dbg.exe")
  #else  // _WIN32
  #define PROCESS_NAME ts("x32dbg.exe")
  #endif // _WIN64

  auto pids = vu::name_to_pid(PROCESS_NAME);
  if (pids.empty())
  {
    std::cout << "Not found the target process for Process Testing ..." << std::endl;
    return vu::VU_OK;
  }

  auto pid = pids.back();

  vu::Process process;
  process.attach(pid);
  assert(process.ready());

  auto cpu  = process.get_cpu_information();
  auto mem  = process.get_memory_information();
  // auto time = process.get_time_information();
  // auto io   = process.get_io_information();

  std::tcout << ts("CPU : ") << cpu.Usage << std::endl;
  std::tcout << ts("WS  : ") << vu::format_bytes(mem.WorkingSetSize) << std::endl;

  for (const auto& thread : process.get_threads())
  {
    static int idx = 0;
    std::tcout << ++idx << ". TID = " << thread.th32ThreadID << std::endl;
    std::tcout << ts("\tPID   = ") << thread.th32OwnerProcessID << std::endl;
    std::tcout << ts("\tUsage = ") << thread.cntUsage << std::endl;
    std::tcout << ts("\tBase Priority  = ") << thread.tpBasePri << std::endl;
    std::tcout << ts("\tDelta Priority = ") << thread.tpDeltaPri << std::endl;
    std::tcout << std::endl;
  }

  for (const auto& module : process.get_modules())
  {
    static int idx = 0;
    std::cout << ++idx << ". MID = " << LPVOID(module.hModule) << std::endl;
    std::cout << "\tBase Address = " << LPVOID(module.modBaseAddr) << std::endl;
    std::cout << "\tBase Size = " << vu::format_bytes_A(module.modBaseSize) << std::endl;
    std::cout << "\tModule = " << module.szModule << std::endl;
    std::cout << std::endl;
  }

  for (const auto& e : process.get_memories())
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
