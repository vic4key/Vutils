#pragma once

#include "Sample.h"

#include <iomanip>

DEF_SAMPLE(Misc)
{
  vu::Box(vu::GetConsoleWindow(), _T("I'm %s. I'm %d years old."), _T("Vic P"), 26);
  vu::Msg(_T("I'm %s. I'm %d years old."), _T("Vic P"), 26);

  std::tcout << vu::Fmt(_T("I'm %s. I'm %d years old. (A)"), _T("Vic P"), 26) << std::endl;

  std::tcout << vu::LowerString(_T("I Love You")) << std::endl;
  std::tcout << vu::UpperString(_T("I Love You")) << std::endl;

  std::tcout << vu::TrimString(_T("   THIS IS A TRIM STRING   ")) << std::endl;

  std::tcout << vu::LastError() << std::endl;

  std::vector<std::tstring>  l;

  l.clear();
  l = vu::SplitString(_T("THIS IS A SPLIT STRING"), _T(" "));
  for (auto e : l) std::tcout << e << _T("|");
  std::tcout << std::endl;

  l.clear();
  l = vu::MultiStringToList(_T("THIS\0IS\0A\0MULTI\0STRING\0\0"));
  for (auto& e : l) std::tcout << e << _T("|");
  std::tcout << std::endl;

  std::tcout << vu::DateTimeToString(time(NULL)) << std::endl;

  std::cout << vu::ToStringA(L"THIS IS A WIDE STRING") << std::endl;
  std::wcout << vu::ToStringW("THIS IS AN ANSI STRING") << std::endl;

  std::tcout << _T("Enviroment `PATH`") << std::endl;
  std::tstring envValue = vu::GetEnviroment(_T("PATH"));
  auto env = vu::SplitString(envValue, _T(";"));
  for (auto e : env) {
    std::tcout << '\t' << e << std::endl;
  }

  std::tcout << vu::ReplaceString(_T("Written in C++ and for C++"), _T("C++"), _T("Cpp")) << std::endl;

  std::tcout << vu::StartsWith(_T("Written in C++ and for C++"), _T("C++")) << std::endl;
  std::tcout << vu::StartsWith(_T("Written in C++ and for C++"), _T("Written")) << std::endl;

  std::tcout << vu::EndsWith(_T("Written in C++ and for C++"), _T("C++")) << std::endl;
  std::tcout << vu::EndsWith(_T("Written in C++ and for C++"), _T("Written")) << std::endl;

  std::vector<vu::ulong> PIDs;
  PIDs.clear();

  PIDs = vu::NameToPID(_T("Explorer.exe"));
  for (auto& PID : PIDs) {
    std::tcout << PID << std::endl;
  }

  if (!PIDs.empty()) {
    std::tcout << vu::PIDToName(*PIDs.begin()) << std::endl;
  }

  // std::vector<vu::ulong> pids;
  // pids = vu::NameToPID(_T("notepad.exe")); // 64-bit
  // pids = vu::NameToPID(_T("JRuler.exe"));  // 32-bit
  // assert(!pids.empty());
  // vu::InjectDLL(pids.back(), _T("path\\to\\32-bit-dll"), true);
  // vu::InjectDLL(pids.back(), _T("path\\to\\64-bit-dll"), true);

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

  vu::CFileSystem::Iterate(_T("path\\to\\example"), _T("*.txt"), [](const vu::TFSObject& FSObject) -> bool
  {
    auto filePath = FSObject.Directory + FSObject.Name;
    auto data = vu::CFileSystem::QuickReadAsBuffer(filePath);

    auto result = vu::DetermineEncodingType(data.GetpData(), data.GetSize());
    auto es = result == -1 ? L"Unknown" : LES[result];
    auto el = result == -1 ? L"Unknown" : LEL[result];

    std::wcout
      << std::left
      << std::setw(15) << es
      << " | "
      << std::setw(25) << el
      << " | "
      << FSObject.Name.c_str()
      << std::endl;

    return true;
  });

  auto type = vu::eStdByte::SI;
  auto digits = 2;
  std::tcout << vu::FormatBytes(912, type, digits) << std::endl; // B/Bi
  std::tcout << vu::FormatBytes(91234, type, digits) << std::endl; // KB/KiB
  std::tcout << vu::FormatBytes(9123456, type, digits) << std::endl; // MB/MiB
  std::tcout << vu::FormatBytes(9123456789, type, digits) << std::endl; // GB/GiB
  std::tcout << vu::FormatBytes(9123456789101, type, digits) << std::endl; // TB/TiB
  std::tcout << vu::FormatBytes(9123456789101213, type, digits) << std::endl; // PB/PiB
  std::tcout << vu::FormatBytes(9123456789101213145, type, digits) << std::endl; // EB/EiB*/

  vu::CBuffer data;
  auto result = vu::FindPattern(data, _T("11 ?? 33 ?? 44 ?? 55"));
  std::tcout << _T("Result is ") << result.first << _T(" at Offset ") << result.second << std::endl;

  std::string  s = "0123456789";
  vu::CBuffer  slicer(s.data(), s.size());

  std::cout << slicer(0, 0).ToStringA() << std::endl;
  std::cout << slicer(9, 9).ToStringA() << std::endl;
  std::cout << slicer(0, 9).ToStringA() << std::endl;

  std::cout << slicer(3, 7).ToStringA() << std::endl;

  std::cout << slicer(-7, -2).ToStringA() << std::endl;
  std::cout << slicer(2, -2).ToStringA() << std::endl;

  std::cout << slicer(0, -8).ToStringA() << std::endl;
  std::cout << slicer(0, -9).ToStringA() << std::endl;
  std::cout << slicer(0, -10).ToStringA() << std::endl;

  std::cout << slicer(-8, 10).ToStringA() << std::endl;
  std::cout << slicer(-9, 10).ToStringA() << std::endl;
  std::cout << slicer(-10, 10).ToStringA() << std::endl;

  std::tcout << vu::DecorateCppSymbol(_T("?func1@a@@AAEXH@Z")) << std::endl;

  return vu::VU_OK;
}
