#pragma once

#include "Sample.h"

DEF_SAMPLE(DF)
{
  const std::tstring FILE_DIR  = ts("C:\\Intel\\Logs\\");
  const std::tstring FILE_PATH = ts("C:\\Intel\\Logs\\IntelGFX.log");

  std::tcout << (vu::is_directory_exists(FILE_DIR) ? ts("Directory is exist") : ts("Directory isn't exist")) << std::endl;
  std::tcout << (vu::is_file_exists(FILE_PATH) ? ts("File is exist") : ts("File isn't exist")) << std::endl;

  std::tcout << vu::get_file_type(FILE_PATH) << std::endl;

  std::tcout << vu::extract_file_directory(FILE_PATH) << std::endl;
  std::tcout << vu::extract_file_directory(FILE_PATH, false) << std::endl;

  std::tcout << vu::extract_file_name(FILE_PATH) << std::endl;
  std::tcout << vu::extract_file_name(FILE_PATH, false) << std::endl;

  std::tcout << vu::get_current_directory() << std::endl;
  std::tcout << vu::get_current_directory(false) << std::endl;

  std::tcout << vu::get_contain_directory() << std::endl;
  std::tcout << vu::get_contain_directory(false) << std::endl;

  std::tcout << vu::get_current_file_path() << std::endl;

  std::tcout << vu::join_path(ts("C:\\path\\to\\"), ts("\\file.exe")) << std::endl;
  std::tcout << vu::join_path(ts("C:\\path\\to\\"), ts("file.exe")) << std::endl;
  std::tcout << vu::join_path(ts("C:\\path\\to"), ts("\\file.exe")) << std::endl;
  std::tcout << vu::join_path(ts("C:\\path\\to"), ts("file.exe")) << std::endl;

  std::tcout << vu::normalize_path(ts("C:/path/to/file.exe"), vu::ePathSep::POSIX) << std::endl;
  std::tcout << vu::normalize_path(ts("C:\\path\\to\\file.exe"), vu::ePathSep::POSIX) << std::endl;
  std::tcout << vu::normalize_path(ts("C:\\path\\\\to\\file.exe"), vu::ePathSep::POSIX) << std::endl;
  std::tcout << vu::normalize_path(ts("C:\\path\\\\to\\file.exe"), vu::ePathSep::POSIX) << std::endl;
  std::tcout << vu::normalize_path(ts("C:\\\\path\\to/file.exe"), vu::ePathSep::POSIX) << std::endl;

  std::tcout << vu::normalize_path(ts("C:/path/to/file.exe")) << std::endl;
  std::tcout << vu::normalize_path(ts("C:\\path\\to\\file.exe")) << std::endl;
  std::tcout << vu::normalize_path(ts("C:\\path\\\\to\\file.exe")) << std::endl;
  std::tcout << vu::normalize_path(ts("C:\\path\\\\to\\file.exe")) << std::endl;
  std::tcout << vu::normalize_path(ts("C:\\\\path\\to/file.exe")) << std::endl;

  vu::CPath fileDir;
  fileDir.Join(ts("   C:/Users")).Join(ts("/Vic")).Join(ts("\\.vscode\\")).Finalize();

  vu::CPath fileDirTmp = std::tstring(ts("C:/Users/Vic\\.vscode\\extensions"));
  
  vu::CPath filePath(fileDir);
  filePath += vu::CPath(ts("argv.json"));
  filePath.Finalize();

  vu::CPath filePathTmp(fileDir);
  filePathTmp = filePathTmp + vu::CPath(ts("argv-tmp.json"));
  filePathTmp.Finalize();

  std::tcout << fileDir  << std::endl;
  std::tcout << fileDirTmp << std::endl;

  std::tcout << filePath << std::endl;
  std::tcout << filePathTmp << std::endl;

  std::tcout << (filePath == filePathTmp) << std::endl;
  std::tcout << (filePath != filePathTmp) << std::endl;

  std::tcout << ts("Exists (Dir)  : ") << fileDir.Exists() << std::endl;
  std::tcout << ts("Exists (File) : ") << filePath.Exists() << std::endl;

  std::tcout << ts("File Name : ") << filePath.FileName().AsString() << std::endl;
  std::tcout << ts("File Directory : ") << filePath.FileDirectory().AsString() << std::endl;

  // TODO: Vic. Uncompleted.

  // #ifdef _WIN64
  // #define EXE_NAME ts("x64dbg.exe")
  // #else  // _WIN32
  // #define EXE_NAME ts("x32dbg.exe")
  // #endif // _WIN64
  // 
  // auto PIDs = vu::name_to_pid(EXE_NAME);
  // if (PIDs.empty())
  // {
  //   return 1;
  // }
  // 
  // vu::ulong PID = *PIDs.begin();
  // 
  // std::cout << std::hex << vu::remote_get_module_handle_A(PID, "kernel32.dll") << std::endl;
  // std::tcout << vu::last_error().c_str() << std::endl;
  // 
  // std::wcout << std::hex << vu::remote_get_module_handle_W(PID, L"kernel32.dll") << std::endl;
  // std::tcout << vu::last_error().c_str() << std::endl;

  return vu::VU_OK;
}
