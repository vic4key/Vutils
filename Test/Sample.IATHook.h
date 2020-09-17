#pragma once

#include "Sample.h"

DEF_SAMPLE(IATHook)
{
  vu::CIATHookManager::Instance().Override("msvcp110d.dll", "KERNEL32.dll", "IsDebuggerPresent", 0x160991);
  system("PAUSE");

  vu::CIATHookManager::Instance().Restore("msvcp110d.dll", "KERNEL32.dll", "IsDebuggerPresent");
  system("PAUSE");

  vu::CIATHookManager::Instance().Override("msvcp110d.dll", "KERNEL32.dll", "IsDebuggerPresent", 0);
  system("PAUSE");

  return vu::VU_OK;
}
