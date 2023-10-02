#pragma once

#include "Sample.h"

DEF_SAMPLE(INIFile)
{
  vu::INIFile ini(vu::get_current_file_path() + ts(".ini"));

  struct struct_t
  {
    char a;
    int b;
    float c;
  };

  struct_t object =
  {
    'X',
    702,
    7.02f
  };

  if (auto section = ini.section(ts("section")))
  {
    section->write(ts("int"), -32);
    section->write(ts("uint"), 32);
    section->write(ts("int64"), -64);
    section->write(ts("uint64"), 64);
    section->write(ts("bool"), true);
    section->write(ts("float"), 7.02F);
    section->write(ts("double"), 16.09);
    section->write(ts("string"), ts("string"));
    section->write(ts("object"), &object, sizeof(object));
  }

  std::cout << std::endl;

  std::vector<std::tstring> section_names;
  if (ini.read_section_names(section_names))
  {
    for (auto& section_name : section_names)
    {
      std::tcout << section_name << std::endl;
    }
  }

  std::cout << std::endl;

  if (auto section = ini.section(ts("section")))
  {
    std::tcout << section->read(ts("int")).to_int() << std::endl;
    std::tcout << section->read(ts("uint")).to_int() << std::endl;
    std::tcout << section->read(ts("int64")).to_int64() << std::endl;
    std::tcout << section->read(ts("uint64")).to_uint64() << std::endl;
    std::tcout << section->read(ts("bool")).to_bool() << std::endl;
    std::tcout << section->read(ts("float")).to_float() << std::endl;
    std::tcout << section->read(ts("double")).to_double() << std::endl;
    std::tcout << section->read(ts("string")).to_string() << std::endl;
    auto pbytes = section->read(ts("object")).to_bytes();
    auto object = *(struct_t*)pbytes.get();
    std::tcout << object.a << ts("|") << object.b << ts("|") << object.c << std::endl;
  }

  std::cout << std::endl;

  return vu::VU_OK;
}
