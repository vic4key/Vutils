#pragma once

#include "Sample.h"

DEF_SAMPLE(DF)
{
  const std::tstring FILE_DIR = _T("C:\\Intel\\Logs\\");
  const std::tstring FILE_PATH = _T("C:\\Intel\\Logs\\IntelGFX.log");

  std::tcout << (vu::IsDirectoryExists(FILE_DIR) ? _T("Directory is exist") : _T("Directory isn't exist")) << std::endl;
  std::tcout << (vu::IsFileExists(FILE_PATH) ? _T("File is exist") : _T("File isn't exist")) << std::endl;

  std::tcout << vu::FileType(FILE_PATH) << std::endl;

  std::tcout << vu::ExtractFilePath(FILE_PATH) << std::endl;
  std::tcout << vu::ExtractFilePath(FILE_PATH, false) << std::endl;

  std::tcout << vu::ExtractFileName(FILE_PATH) << std::endl;
  std::tcout << vu::ExtractFileName(FILE_PATH, false) << std::endl;

  std::tcout << vu::GetCurDirectory() << std::endl;
  std::tcout << vu::GetCurDirectory(false) << std::endl;

  std::tcout << vu::GetContainDirectory() << std::endl;
  std::tcout << vu::GetContainDirectory(false) << std::endl;

  std::tcout << vu::GetCurrentFilePath() << std::endl;

  std::tcout << vu::JoinPath(_T("C:\\path\\to\\"), _T("\\file.exe")) << std::endl;
  std::tcout << vu::JoinPath(_T("C:\\path\\to\\"), _T("file.exe")) << std::endl;
  std::tcout << vu::JoinPath(_T("C:\\path\\to"), _T("\\file.exe")) << std::endl;
  std::tcout << vu::JoinPath(_T("C:\\path\\to"), _T("file.exe")) << std::endl;

  std::tcout << vu::Replace(_T("Written in C++ and for C++"), _T("C++"), _T("Cpp")) << std::endl;

  std::tcout << vu::StartsWith(_T("Written in C++ and for C++"), _T("C++")) << std::endl;
  std::tcout << vu::StartsWith(_T("Written in C++ and for C++"), _T("Written")) << std::endl;

  // TODO: Vic. Uncompleted.

  // #ifdef _WIN64
  // #define EXE_NAME _T("x64dbg.exe")
  // #else  // _WIN32
  // #define EXE_NAME _T("x32dbg.exe")
  // #endif // _WIN64
  // 
  // auto PIDs = vu::NameToPID(EXE_NAME);
  // if (PIDs.empty())
  // {
  //   return 1;
  // }
  // 
  // vu::ulong PID = *PIDs.begin();
  // 
  // std::cout << std::hex << vu::RemoteGetModuleHandleA(PID, "kernel32.dll") << std::endl;
  // std::tcout << vu::LastError().c_str() << std::endl;
  // 
  // std::wcout << std::hex << vu::RemoteGetModuleHandleW(PID, L"kernel32.dll") << std::endl;
  // std::tcout << vu::LastError().c_str() << std::endl;

  return vu::VU_OK;
}
