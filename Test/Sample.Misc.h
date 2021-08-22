#pragma once

#include "Sample.h"

#include <iomanip>

DEF_SAMPLE(Misc)
{
  vu::msg_box(vu::get_console_window(), ts("I'm %s. I'm %d years old."), ts("Vic P"), 26);
  vu::msg_debug(ts("I'm %s. I'm %d years old."), ts("Vic P"), 26);

  std::tcout << vu::format(ts("I'm %s. I'm %d years old. (A)"), ts("Vic P"), 26) << std::endl;

  std::tcout << vu::lower_string(ts("I Love You")) << std::endl;
  std::tcout << vu::upper_string(ts("I Love You")) << std::endl;

  std::tcout << vu::trim_string(ts("   THIS IS A TRIM STRING   ")) << std::endl;

  std::tcout << vu::get_last_error() << std::endl;

  std::vector<std::tstring>  l;

  l.clear();
  l = vu::split_string(ts("THIS IS A SPLIT STRING"), ts(" "));
  for (auto e : l) std::tcout << e << ts("|");
  std::tcout << std::endl;

  l.clear();
  l = vu::multi_string_to_list(ts("THIS\0IS\0A\0MULTI\0STRING\0\0"));
  for (auto& e : l) std::tcout << e << ts("|");
  std::tcout << std::endl;

  std::tcout << vu::date_time_to_string(time(NULL)) << std::endl;

  std::cout << vu::to_string_A(L"THIS IS A WIDE STRING") << std::endl;
  std::wcout << vu::to_string_W("THIS IS AN ANSI STRING") << std::endl;

  std::tcout << ts("Enviroment `PATH`") << std::endl;
  std::tstring envValue = vu::get_enviroment(ts("PATH"));
  auto env = vu::split_string(envValue, ts(";"));
  for (auto e : env) {
    std::tcout << '\t' << e << std::endl;
  }

  std::tcout << vu::replace_string(ts("Written in C++ and for C++"), ts("C++"), ts("Cpp")) << std::endl;

  std::tcout << vu::starts_with(ts("Written in C++ and for C++"), ts("C++")) << std::endl;
  std::tcout << vu::starts_with(ts("Written in C++ and for C++"), ts("Written")) << std::endl;

  std::tcout << vu::ends_with(ts("Written in C++ and for C++"), ts("C++")) << std::endl;
  std::tcout << vu::ends_with(ts("Written in C++ and for C++"), ts("Written")) << std::endl;

  std::vector<vu::ulong> PIDs;
  PIDs.clear();

  PIDs = vu::name_to_pid(ts("Explorer.exe"));
  for (auto& PID : PIDs) {
    std::tcout << PID << std::endl;
  }

  if (!PIDs.empty()) {
    std::tcout << vu::pid_to_name(*PIDs.begin()) << std::endl;
  }

  // std::vector<vu::ulong> pids;
  // pids = vu::name_to_pid(ts("notepad.exe")); // 64-bit
  // pids = vu::name_to_pid(ts("JRuler.exe"));  // 32-bit
  // assert(!pids.empty());
  // vu::inject_dll(pids.back(), ts("path\\to\\32-bit-dll"), true);
  // vu::inject_dll(pids.back(), ts("path\\to\\64-bit-dll"), true);

  static std::wstring LES[] = { // List Encoding Short
    L"ANSI/UTF-8", L"UTF-8 BOM",
    L"Unicode", L"Unicode BE",
    L"Unicode BOM", L"Unicode BE BOM",
    L"UTF-32 LE BOM", L"UTF-32 BE BOM"
  };

  static std::wstring LEL[] = { // List Encoding Long
    L"ANSI/UTF-8", L"UTF-8 BOM",
    L"UTF-16 Little Endian", L"UTF-16 Big Endian",
    L"UTF-16 Little Endian BOM", L"UTF-16 Big Endian BOM",
    L"UTF-32 Little Endian BOM", L"UTF-32 Big Endian BOM"
  };

  vu::FileSystem::iterate(ts("path\\to\\example"), ts("*.txt"), [](const vu::sFSObject& fso) -> bool
  {
    auto file_path = fso.directory + fso.name;
    auto data = vu::FileSystem::quick_read_as_buffer(file_path);

    auto result = vu::determine_encoding_type(data.get_ptr_data(), data.get_size());
    auto es = result == -1 ? L"Unknown" : LES[result];
    auto el = result == -1 ? L"Unknown" : LEL[result];

    std::wcout
      << std::left
      << std::setw(15) << es
      << " | "
      << std::setw(25) << el
      << " | "
      << fso.name.c_str()
      << std::endl;

    return true;
  });

  auto type = vu::eStdByte::SI;
  auto digits = 2;
  std::tcout << vu::format_bytes(912, type, digits) << std::endl; // B/Bi
  std::tcout << vu::format_bytes(91234, type, digits) << std::endl; // KB/KiB
  std::tcout << vu::format_bytes(9123456, type, digits) << std::endl; // MB/MiB
  std::tcout << vu::format_bytes(9123456789, type, digits) << std::endl; // GB/GiB
  std::tcout << vu::format_bytes(9123456789101, type, digits) << std::endl; // TB/TiB
  std::tcout << vu::format_bytes(9123456789101213, type, digits) << std::endl; // PB/PiB
  std::tcout << vu::format_bytes(9123456789101213145, type, digits) << std::endl; // EB/EiB*/

  vu::Buffer data;
  auto result = vu::find_pattern(data, ts("11 ?? 33 ?? 44 ?? 55"));
  std::tcout << ts("Result is ") << result.first << ts(" at Offset ") << result.second << std::endl;

  std::string s = "0123456789";
  vu::Buffer  slicer(s.data(), s.size());

  std::cout << slicer(0, 0).to_string_A() << std::endl;
  std::cout << slicer(9, 9).to_string_A() << std::endl;
  std::cout << slicer(0, 9).to_string_A() << std::endl;

  std::cout << slicer(3, 7).to_string_A() << std::endl;

  std::cout << slicer(-7, -2).to_string_A() << std::endl;
  std::cout << slicer(2, -2).to_string_A() << std::endl;

  std::cout << slicer(0, -8).to_string_A() << std::endl;
  std::cout << slicer(0, -9).to_string_A() << std::endl;
  std::cout << slicer(0, -10).to_string_A() << std::endl;

  std::cout << slicer(-8, 10).to_string_A() << std::endl;
  std::cout << slicer(-9, 10).to_string_A() << std::endl;
  std::cout << slicer(-10, 10).to_string_A() << std::endl;

  std::tcout << vu::undecorate_cpp_symbol(ts("?func1@a@@AAEXH@Z")) << std::endl;

  return vu::VU_OK;
}
