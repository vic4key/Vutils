#pragma once

#include "Sample.h"

#include <iomanip>

DEF_SAMPLE(Misc)
{
  // std::tcout << vu::format(ts("I'm %s. I'm %d years old. (A)"), ts("Vic P"), 26) << std::endl;
  // vu::msg_box(vu::get_console_window(), ts("I'm %s. I'm %d years old."), ts("Vic P"), 26);
  // vu::msg_debug(ts("I'm %s. I'm %d years old."), ts("Vic P"), 26);

  std::tcout << vu::lower_string(ts("I Love You")) << std::endl;
  std::tcout << vu::upper_string(ts("I Love You")) << std::endl;

  std::tcout << vu::trim_string(ts("   THIS IS A TRIM STRING   ")) << std::endl;

  std::tcout << vu::get_last_error() << std::endl;

  std::vector<std::tstring> l;

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

  std::tcout << vu::contains_string(ts("Written in C++ and for C++"), ts("c++"), false) << std::endl;
  std::tcout << vu::contains_string(ts("Written in C++ and for C++"), ts("c++"), true)  << std::endl;

  std::tcout << vu::compare_string(ts("C++"), ts("c++"), false) << std::endl;
  std::tcout << vu::compare_string(ts("C++"), ts("c++"), true)  << std::endl;

  std::vector<vu::ulong> pids;
  pids.clear();

  pids = vu::name_to_pid(ts("explorer.exe"));
  for (auto& pid : pids)
  {
    std::tcout << pid << std::endl;
  }

  if (!pids.empty())
  {
    std::tcout << vu::pid_to_name(*pids.begin()) << std::endl;
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

    auto result = vu::determine_encoding_type(data.get_ptr(), data.get_size());
    auto es = result == vu::eEncodingType::ET_UNKNOWN ? L"Unknown" : LES[int(result)];
    auto el = result == vu::eEncodingType::ET_UNKNOWN ? L"Unknown" : LEL[int(result)];

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

  std::vector<vu::byte> hex_bytes = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

  auto hex_string = vu::to_hex_string(hex_bytes.data(), hex_bytes.size());
  std::tcout << hex_string << std::endl;

  hex_bytes.clear();
  vu::to_hex_bytes(hex_string, hex_bytes);
  for (const auto& e : hex_bytes)
  {
    std::tcout << std::hex << std::setfill(ts('0')) << std::setw(2) << int(e);
  }
  std::tcout << std::endl;

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

  #if defined(_MSC_VER) || defined(__BCPLUSPLUS__) // LNK
  std::tstring lnk_file_path = ts("C:\\Users\\Vic\\Desktop\\Tencent Gaming Buddy.lnk");
  auto ptr_lnk = vu::parse_shortcut_lnk(nullptr, lnk_file_path);
  if (ptr_lnk != nullptr)
  {
    std::tcout << ts("Parsing ") << lnk_file_path << std::endl;
    std::tcout << vu::tab << ptr_lnk->path << std::endl;
    std::tcout << vu::tab << ptr_lnk->directory << std::endl;
    std::tcout << vu::tab << ptr_lnk->argument << std::endl;
    std::tcout << vu::tab << ptr_lnk->description << std::endl;
  }
  #endif // LNK

  return vu::VU_OK;
}
