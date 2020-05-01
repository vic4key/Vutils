#pragma once

#include "Sample.h"

DEF_SAMPLE(FileSystem)
{
  const std::tstring FILE_NAME = _T("Test.txt");
  {
    std::string s = "This is a test string";
    vu::CFileSystem file(FILE_NAME, vu::FM_CREATEALWAY);
    file.Write(s.c_str(), (vu::ulong)s.length());
  }
  {
    char D[MAXBYTE] = { 0 };
    vu::CFileSystem file(FILE_NAME, vu::FM_OPENEXISTING);
    file.Read(&D, sizeof(D));

    std::tcout << _T("D = ") << D << std::endl;
  }
  vu::CFileSystem::Iterate(_T("C:\\Intel\\Logs"), _T("*.*"), [](const vu::TFSObject& FSObject) -> bool
    {
      std::tcout << FSObject.Directory << _T(" | ") << FSObject.Name << _T(" | ") << FSObject.Size << std::endl;
      return true;
    });

  return vu::VU_OK;
}
