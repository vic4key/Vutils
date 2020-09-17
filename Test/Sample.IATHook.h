#pragma once

#include "Sample.h"

DEF_SAMPLE(IATHook)
{
#ifdef _DEBUG
  std::string DLL = "msvcp110d.dll";
#else
  std::string DLL = "msvcp110.dll";
#endif // !_DEBUG

  vu::CIATHookManager::Instance().Override(DLL, "KERNEL32.dll", "IsDebuggerPresent", 0x160991);
  system("PAUSE");

  vu::CIATHookManager::Instance().Restore(DLL, "KERNEL32.dll", "IsDebuggerPresent");
  system("PAUSE");

  vu::CIATHookManager::Instance().Override(DLL, "KERNEL32.dll", "IsDebuggerPresent");
  system("PAUSE");

  return vu::VU_OK;
}
