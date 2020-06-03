#pragma once

#include "Sample.h"

DEF_SAMPLE(DF)
{
  const std::tstring FILE_DIR  = _T("C:\\Intel\\Logs\\");
  const std::tstring FILE_PATH = _T("C:\\Intel\\Logs\\IntelGFX.log");

  std::tcout << (vu::IsDirectoryExists(FILE_DIR) ? _T("Directory is exist") : _T("Directory isn't exist")) << std::endl;
  std::tcout << (vu::IsFileExists(FILE_PATH) ? _T("File is exist") : _T("File isn't exist")) << std::endl;

  std::tcout << vu::FileType(FILE_PATH) << std::endl;

  std::tcout << vu::ExtractFileDirectory(FILE_PATH) << std::endl;
  std::tcout << vu::ExtractFileDirectory(FILE_PATH, false) << std::endl;

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

  std::tcout << vu::NormalizePath(_T("C:/path/to/file.exe"), vu::ePathSep::POSIX) << std::endl;
  std::tcout << vu::NormalizePath(_T("C:\\path\\to\\file.exe"), vu::ePathSep::POSIX) << std::endl;
  std::tcout << vu::NormalizePath(_T("C:\\path\\\\to\\file.exe"), vu::ePathSep::POSIX) << std::endl;
  std::tcout << vu::NormalizePath(_T("C:\\path\\\\to\\file.exe"), vu::ePathSep::POSIX) << std::endl;
  std::tcout << vu::NormalizePath(_T("C:\\\\path\\to/file.exe"), vu::ePathSep::POSIX) << std::endl;

  std::tcout << vu::NormalizePath(_T("C:/path/to/file.exe")) << std::endl;
  std::tcout << vu::NormalizePath(_T("C:\\path\\to\\file.exe")) << std::endl;
  std::tcout << vu::NormalizePath(_T("C:\\path\\\\to\\file.exe")) << std::endl;
  std::tcout << vu::NormalizePath(_T("C:\\path\\\\to\\file.exe")) << std::endl;
  std::tcout << vu::NormalizePath(_T("C:\\\\path\\to/file.exe")) << std::endl;

  vu::CPath fileDir;
  fileDir.Join(_T("   C:/Users")).Join(_T("/Vic")).Join(_T("\\.vscode\\")).Finalize();

  vu::CPath fileDirTmp = std::tstring(_T("C:/Users/Vic\\.vscode\\extensions"));
  
  vu::CPath filePath(fileDir);
  filePath += vu::CPath(_T("argv.json"));
  filePath.Finalize();

  vu::CPath filePathTmp(fileDir);
  filePathTmp = filePathTmp + vu::CPath(_T("argv-tmp.json"));
  filePathTmp.Finalize();

  std::tcout << fileDir  << std::endl;
  std::tcout << fileDirTmp << std::endl;

  std::tcout << filePath << std::endl;
  std::tcout << filePathTmp << std::endl;

  std::tcout << (filePath == filePathTmp) << std::endl;
  std::tcout << (filePath != filePathTmp) << std::endl;

  std::tcout << _T("Exists (Dir)  : ") << fileDir.Exists() << std::endl;
  std::tcout << _T("Exists (File) : ") << filePath.Exists() << std::endl;

  std::tcout << _T("File Name : ") << filePath.FileName().AsString() << std::endl;
  std::tcout << _T("File Directory : ") << filePath.FileDirectory().AsString() << std::endl;

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
