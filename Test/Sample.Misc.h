#pragma once

#include "Sample.h"

#include <iomanip>

DEF_SAMPLE(Misc)
{
  // std::tcout << vu::format(ts("I'm %s. I'm %d years old. (A)"), ts("Vic P"), 26) << std::endl;
  // vu::msg_box(vu::get_console_window(), ts("I'm %s. I'm %d years old."), ts("Vic P"), 26);
  // vu::msg_debug(ts("I'm %s. I'm %d years old."), ts("Vic P"), 26);

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

  vu::FileSystem::iterate(ts(".\\Text.Encoding"), ts("*.txt"), [](const vu::FSObject& fso) -> bool
  {
    auto file_path = fso.directory + fso.name;
    std::tcout << file_path << std::endl;
    return true;
  });

  auto type = vu::data_unit::SI;
  auto digits = 2;
  std::tcout << vu::format_bytes(912, type, digits) << std::endl; // B/Bi
  std::tcout << vu::format_bytes(91234, type, digits) << std::endl; // KB/KiB
  std::tcout << vu::format_bytes(9123456, type, digits) << std::endl; // MB/MiB
  std::tcout << vu::format_bytes(9123456789, type, digits) << std::endl; // GB/GiB
  std::tcout << vu::format_bytes(9123456789101, type, digits) << std::endl; // TB/TiB
  std::tcout << vu::format_bytes(9123456789101213, type, digits) << std::endl; // PB/PiB
  std::tcout << vu::format_bytes(9123456789101213145, type, digits) << std::endl; // EB/EiB*/

  std::vector<vu::byte> hex_bytes;

  /* 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF */
  hex_bytes.clear();
  hex_bytes.push_back(0x00); hex_bytes.push_back(0x11); hex_bytes.push_back(0x22);
  hex_bytes.push_back(0x33); hex_bytes.push_back(0x44); hex_bytes.push_back(0x55);
  hex_bytes.push_back(0x66); hex_bytes.push_back(0x77); hex_bytes.push_back(0x88);
  hex_bytes.push_back(0x99); hex_bytes.push_back(0xAA); hex_bytes.push_back(0xBB);
  hex_bytes.push_back(0xCC); hex_bytes.push_back(0xDD); hex_bytes.push_back(0xEE);
  hex_bytes.push_back(0xFF);

  auto hex_string = vu::to_hex_string(hex_bytes.data(), hex_bytes.size());
  std::tcout << hex_string << std::endl;

  hex_bytes.clear();
  vu::to_hex_bytes(hex_string, hex_bytes);
  for (const auto& e : hex_bytes)
  {
    std::tcout << std::hex << std::setfill(ts('0')) << std::setw(2) << int(e);
  }
  std::tcout << std::endl;

  std::tstring url_encoded;
  vu::url_encode(ts("vic.onl/+1 2-3%4"), url_encoded);
  std::tcout << "URL Encoded : " << url_encoded << std::endl;

  std::tstring url_decoded; // `vic.onl/+1 2-3%4`
  vu::url_decode(ts("vic.onl%2f%2b1%202-3%254"), url_decoded);
  std::tcout << "URL Decoded : " << url_decoded << std::endl;

  std::string s = "0123456789";
  vu::Buffer  slicer(s.data(), s.size());

  //std::cout << slicer(0, 0)->to_string_A()->c_str() << std::endl;
  //std::cout << slicer(9, 9)->to_string_A()->c_str() << std::endl;
  std::cout << slicer(0, 9)->to_string_A()->c_str() << std::endl;

  std::cout << slicer(3, 7)->to_string_A()->c_str() << std::endl;

  std::cout << slicer(-7, -2)->to_string_A()->c_str() << std::endl;
  std::cout << slicer(2, -2)->to_string_A()->c_str() << std::endl;

  std::cout << slicer(0, -8)->to_string_A()->c_str() << std::endl;
  std::cout << slicer(0, -9)->to_string_A()->c_str() << std::endl;
  //std::cout << slicer(0, -10)->to_string_A()->c_str() << std::endl;

  std::cout << slicer(-8, 10)->to_string_A()->c_str() << std::endl;
  std::cout << slicer(-9, 10)->to_string_A()->c_str() << std::endl;
  std::cout << slicer(-10, 10)->to_string_A()->c_str() << std::endl;

  std::tcout << vu::undecorate_cpp_symbol(ts("?func1@a@@AAEXH@Z")) << std::endl;

  #if defined(_MSC_VER) || defined(__BCPLUSPLUS__) // LNK

  #define HOTKEYF_CONTROL 0x02
  #define HOTKEYF_ALT     0x04

  std::tstring lnk_file_path = ts("C:\\Users\\Vic\\Desktop\\notepad.lnk");

  vu::LNK lnk;
  lnk.path = ts("C:\\Windows\\System32\\notepad.exe");
  lnk.argument = ts("test.txt");
  lnk.directory = ts("C:\\Users\\Vic\\Desktop");
  lnk.description = ts("shortcut for notepad.exe");
  lnk.hotkey = MAKEWORD('A', HOTKEYF_CONTROL + HOTKEYF_ALT);
  lnk.window = SW_NORMAL;
  lnk.icon = std::pair<std::tstring, int>(ts("C:\\Windows\\System32\\notepad.exe"), 0);
  vu::create_shortcut_lnk(lnk_file_path, lnk);

  auto ptr_lnk = vu::parse_shortcut_lnk(nullptr, lnk_file_path);
  if (ptr_lnk != nullptr)
  {
    std::tcout << ts("Parsing ") << lnk_file_path << std::endl;
    std::tcout << vu::tab << ptr_lnk->path << std::endl;
    std::tcout << vu::tab << ptr_lnk->argument << std::endl;
    std::tcout << vu::tab << ptr_lnk->directory << std::endl;
    std::tcout << vu::tab << ptr_lnk->description << std::endl;
    std::tcout << vu::tab << ptr_lnk->hotkey << std::endl;
    std::tcout << vu::tab << ptr_lnk->window << std::endl;
    std::tcout << vu::tab << ptr_lnk->icon.first << std::endl;
    std::tcout << vu::tab << ptr_lnk->icon.second << std::endl;
  }

  #endif // LNK

  #ifdef VU_HAS_PRINT
  vu::print(1, 2.3F, 3.4, ts('C'), ts("this is a string"));
  #endif

  #ifdef VU_HAS_STD_OP
  {
    #define container std::vector // std::list // std::set // etc
    container<int> l_int = { 1, 2, 3, 4, 5 };
    container<char> l_chr = { '1', '2', '3', '4', '5' };
    container<float> l_flt = { 1.F, 2.F, 3.F, 4.F, 5.F };
    container<double> l_dbl = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    container<std::string> l_str = { "1", "2", "3", "4", "5" };
    using namespace std::op;
    std::tcout << ( 1   <in> l_int) << std::endl;
    std::tcout << ('1'  <in> l_chr) << std::endl;
    std::tcout << ( 1.F <in> l_flt) << std::endl;
    std::tcout << ( 1.0 <in> l_dbl) << std::endl;
    std::tcout << ("1"  <in> l_str) << std::endl;
  }
  #endif

  // Debouncer
  // - Call the lambda function 5 times, and 0.1 second per each call
  // - But just once call is really executed after 1 second passed from the last call
  for (int i = 1; i <= 5; i++)
  {
    vu::Debouncer::instance().debounce(__LINE__, 1000, []() -> void
    {
      std::tcout << ts("This message printed once by Debouncer after 1 second passed") << std::endl;
    });

    std::tcout << ts("Called #") << i << std::endl; Sleep(100);
  }

  MessageBoxA(nullptr, "This message box just keep for the program running...", "", MB_OK);

  return vu::VU_OK;
}
