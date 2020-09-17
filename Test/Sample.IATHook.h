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

  vu::CIATHookManager::Instance().Override(DLL, "user32.dll", "MessageBoxA", DWORD_PTR(HfnMessageBoxA), (DWORD_PTR**)&pfnMessageBoxA);
  system("PAUSE");

  MessageBoxA(vu::GetConsoleWindow(), "The first message.", "A", MB_OK);

  vu::CIATHookManager::Instance().Restore(DLL, "user32.dll", "MessageBoxA");
  system("PAUSE");

  MessageBoxA(vu::GetConsoleWindow(), "The second message.", "A", MB_OK);

  vu::CIATHookManager::Instance().Override(DLL, "user32.dll", "MessageBoxA");
  system("PAUSE");

  // MessageBoxA(vu::GetConsoleWindow(), "The third message.", "A", MB_OK);

  return vu::VU_OK;
}
