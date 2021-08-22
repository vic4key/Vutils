#pragma once

#include "Sample.h"

DEF_SAMPLE(FileSystem)
{
  const std::tstring FILE_NAME = _T("Test.txt");
  {
    std::string s = "This is a test string";
    vu::FileSystem file(FILE_NAME, vu::FM_CREATEALWAY);
    file.write(s.c_str(), (vu::ulong)s.length());
  }
  {
    char D[MAXBYTE] = { 0 };
    vu::FileSystem file(FILE_NAME, vu::FM_OPENEXISTING);
    file.read(&D, sizeof(D));

    std::tcout << _T("D = ") << D << std::endl;
  }

  vu::FileSystem::iterate(_T("C:\\Intel\\Logs"), _T("*.*"), [](const vu::sFSObject& fso) -> bool
  {
    std::tcout << fso.directory << _T(" | ") << fso.name << _T(" | ") << fso.size << std::endl;
    return true;
  });

  return vu::VU_OK;
}
