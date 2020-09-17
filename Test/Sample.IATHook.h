#pragma once

#include "Sample.h"

DEF_SAMPLE(IATHook)
{
#ifdef _DEBUG
  std::string DLL = "msvcp110d.dll";
#else
  std::string DLL = "msvcp110.dll";
#endif // !_DEBUG

  DLL = "Test.exe";

  vu::CIATHookManager::Instance().Override(DLL, "user32.dll", "MessageBoxA", vu::ulongptr(HfnMessageBoxA), (vu::ulongptr**)&pfnMessageBoxA);
  vu::CIATHookManager::Instance().Override(DLL, "user32.dll", "MessageBoxW", vu::ulongptr(HfnMessageBoxW), (vu::ulongptr**)&pfnMessageBoxW);
  MessageBoxA(vu::GetConsoleWindow(),  "The first message.",  "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindow(), L"The first message.", L"W", MB_OK);

  vu::CIATHookManager::Instance().Restore(DLL, "user32.dll", "MessageBoxA");
  vu::CIATHookManager::Instance().Restore(DLL, "user32.dll", "MessageBoxW");
  MessageBoxA(vu::GetConsoleWindow(),  "The second message.",  "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindow(), L"The second message.", L"W", MB_OK);

  return vu::VU_OK;
}
