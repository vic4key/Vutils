#pragma once

#include "Sample.h"

DEF_SAMPLE(AOBScanning)
{
  // AOB scanning simple testing

  vu::byte data_test_find_pattern[] =
  {
    0x77,
    0x11, 0x77, 0x33, 0x77, 0x44, 0x77, 0x55, // 0x01
    0x77, 0x77,
    0x11, 0x77, 0x33, 0x77, 0x44, 0x77, 0x55, // 0x0A
    0x77, 0x77, 0x77,
    0x11, 0x77, 0x33, 0x77, 0x44, 0x77, 0x55, // 0x14
  };
  vu::Buffer data(data_test_find_pattern, sizeof(data_test_find_pattern));
  auto offsets = vu::find_pattern(data, ts("11 ?? 33 ?? 44 ?? 55"), false);
  for (auto& offset : offsets)
  {
    std::tcout << ts("Found at ") << offset << std::endl;
  }

  // AOB scanning a process testing

  auto pids = vu::name_to_pid(ts("dll_load_test.exe")); // remember to run app x86 or x64
  assert(!pids.empty());
  auto pid = pids.front();

  // pid = GetCurrentProcessId(); // or scan the current process itself

  Process process;
  process.attach(pid);
  assert(process.ready());

  #ifdef _WIN64
  auto pattern = ts("48 8B 54 24 ?? 48 8B 4C 24 ?? FF");
  #else
  auto pattern = ts("8B 44 24 ?? 8B 00");
  #endif

  std::vector<size_t> addresses;
  process.scan_memory(addresses, pattern, ts("combase.dll"));

  std::cout << "number of found addresses " << addresses.size() << std::endl;
  for (auto& e : addresses) std::cout << vu::format_A("%p", e) << std::endl;

  return vu::VU_OK;
}
