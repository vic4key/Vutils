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
  if (!pids.empty())
  {
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
  }

  // Testing read/write multi-level pointers
  // C:\Program Files\Cheat Engine 7.4\Tutorial-x86_64.exe
  // Eg. [[[[["Tutorial-x86_64.exe" + 0x325B00] + 0x10] + 0x18] + 0x0] + 0x18]
  // https://wiki.cheatengine.org/index.php?title=Tutorials:Cheat_Engine_Tutorial_Guide_x64#Step_8:_Multilevel_pointers
  // https://wiki.cheatengine.org/index.php?title=File:Tutorials.CETutorialx64.step08.02.png

  #ifdef _WIN64
  pids = vu::name_to_pid(ts("Tutorial-x86_64.exe"));
  if (!pids.empty())
  {
    auto pid = pids.front();

    Process process;
    process.attach(pid);
    assert(process.ready());

    auto exe = process.get_module_information();
    auto address = LPCVOID(vu::ulongptr(exe.modBaseAddr) + 0x325B00);

    int value = 0;

    vu::read_memory_ex(
      arch::x64, process.handle(), address, &value, sizeof(value), false, 4, 0x10, 0x18, 0x0, 0x18);
    std::cout << value << std::endl;

    value += 1;

    vu::write_memory_ex(
      arch::x64, process.handle(), address, &value, sizeof(value), false, 4, 0x10, 0x18, 0x0, 0x18);
    std::cout << value << std::endl;
  }
  #endif

  return vu::VU_OK;
}
