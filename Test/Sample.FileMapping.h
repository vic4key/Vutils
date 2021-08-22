#pragma once

#include "Sample.h"

DEF_SAMPLE(FileMapping)
{
  vu::FileMapping fm;

  void* p = nullptr;

  // if (fm.create_named_shared_memory(ts("Global\\Sample"), KB) != vu::VU_OK)
  // {
  //   std::tcout << ts("Create -> Failed") << vu::get_last_error(fm.get_last_error_code()) << std::endl;
  // }

  if (fm.create_within_file(ts("Test.txt")) != vu::VU_OK)
  {
    std::tcout << ts("Create -> Failed ") << vu::get_last_error(fm.get_last_error_code()) << std::endl;
  }

  p = fm.view();
  if (p == nullptr)
  {
    std::tcout << ts("View -> Failed ") << vu::get_last_error(fm.get_last_error_code()) << std::endl;
  }

  std::tcout << std::hex << ts("File Mapping at ") << p << std::endl;

  if (p != nullptr)
  {
    if (fm.get_file_size() != -1)
    {
      vu::hex_dump(p, fm.get_file_size());
    }
    else
    {
      std::tcout << ts("Waiting for a communication then enter ...") << std::endl; _getch();
      std::cout << (char*)p << std::endl;

      *(vu::ulong*)p = 0x48474645; // EFGH
      std::tcout << ts("Wrote data to file mapping object") << std::endl; _getch();
    }
  }

  return vu::VU_OK;
}
