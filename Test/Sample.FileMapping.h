#pragma once

#include "Sample.h"

DEF_SAMPLE(FileMapping)
{
  vu::CFileMapping fm;

  void* p = nullptr;

  // if (fm.CreateNamedSharedMemory(_T("Global\\Sample"), KB) != vu::VU_OK)
  // {
  //   std::tcout << _T("Create -> Failed") << vu::last_error(fm.GetLastErrorCode()) << std::endl;
  // }

  if (fm.CreateWithinFile(_T("Test.txt")) != vu::VU_OK)
  {
    std::tcout << _T("Create -> Failed ") << vu::last_error(fm.GetLastErrorCode()) << std::endl;
  }

  p = fm.View();
  if (p == nullptr)
  {
    std::tcout << _T("View -> Failed ") << vu::last_error(fm.GetLastErrorCode()) << std::endl;
  }

  std::tcout << std::hex << _T("File Mapping at ") << p << std::endl;

  if (p != nullptr)
  {
    if (fm.GetFileSize() != -1)
    {
      vu::hex_dump(p, fm.GetFileSize());
    }
    else
    {
      std::tcout << _T("Waiting for a communication then enter ...") << std::endl; _getch();
      std::cout << (char*)p << std::endl;

      *(vu::ulong*)p = 0x48474645; // EFGH
      std::tcout << _T("Wrote data to file mapping object") << std::endl; _getch();
    }
  }

  return vu::VU_OK;
}
