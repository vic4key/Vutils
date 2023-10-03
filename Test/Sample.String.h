#pragma once

#include "Sample.h"

DEF_SAMPLE(String)
{
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

  std::tcout << ts("Environment `PATH`") << std::endl;
  std::tstring var_list = vu::get_env(ts("PATH"));
  auto env = vu::split_string(var_list, ts(";"));
  for (auto e : env)
  {
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

  struct
  {
    vu::text_encoding encoding;
    std::string file_name;
  } list_text_encodings[] =
  {
    { vu::text_encoding::TE_UTF8, "ansi.txt" },
    { vu::text_encoding::TE_UTF8_BOM, "utf-8-bom.txt" },
    { vu::text_encoding::TE_UTF16_LE, "utf-16-le.txt" },
    { vu::text_encoding::TE_UTF16_BE, "utf-16-be.txt" },
    { vu::text_encoding::TE_UTF16_LE_BOM, "utf-16-le-bom.txt" },
    { vu::text_encoding::TE_UTF16_BE_BOM, "utf-16-be-bom.txt" },
    //{ vu::text_encoding::TE_UTF32_LE_BOM, "utf-32-le-bom.txt" },
    //{ vu::text_encoding::TE_UTF32_BE_BOM, "utf-32-be-bom.txt" },
  };

  for (auto& e : list_text_encodings)
  {
    vu::PathA path = std::string("Text.Encoding");
    path.join(e.file_name).finalize();
    auto file_path = path.as_string();
    std::cout << file_path << std::endl;

    auto data = vu::read_file_binary_A(file_path);
    auto text_encoding = vu::detect_text_encoding(data->data(), data->size());
    assert(text_encoding == e.encoding);

    {
      auto temp = vu::FileSystemA::quick_read_as_text(file_path);
      auto text = *temp; // vu::to_string_W(*temp.get(), true);
      assert(!text.empty());
      std::cout << std::tab << text << std::endl;
    }

    {
      auto temp = vu::FileSystemW::quick_read_as_text(vu::to_string_W(file_path));
      auto text = vu::to_string_A(*temp, true);
      assert(!text.empty());
      std::cout << std::tab << text << std::endl;
    }
  }

  std::vector<std::tstring> strings = { ts("ape"), ts("apple"), ts("peach"), ts("puppy") };
  auto closest_string = vu::find_closest_string(ts("aple"), strings);
  assert(closest_string == ts("apple"));

  return vu::VU_OK;
}
