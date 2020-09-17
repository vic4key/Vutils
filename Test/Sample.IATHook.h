#pragma once

#include "Sample.h"

DEF_SAMPLE(IATHook)
{
  std::wstring TARGET = _T("Test.exe");

  vu::CIATHookManager::Instance().Override(TARGET, _T("user32.dll"), _T("MessageBoxA"), vu::ulongptr(HfnMessageBoxA), (vu::ulongptr**) & pfnMessageBoxA);
  vu::CIATHookManager::Instance().Override(TARGET, _T("user32.dll"), _T("MessageBoxW"), vu::ulongptr(HfnMessageBoxW), (vu::ulongptr**) & pfnMessageBoxW);

  MessageBoxA(vu::GetConsoleWindow(),  "The first message.",  "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindow(), L"The first message.", L"W", MB_OK);

  vu::CIATHookManager::Instance().Restore(TARGET, _T("user32.dll"), _T("MessageBoxA"));
  vu::CIATHookManager::Instance().Restore(TARGET, _T("user32.dll"), _T("MessageBoxW"));

  MessageBoxA(vu::GetConsoleWindow(),  "The second message.",  "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindow(), L"The second message.", L"W", MB_OK);

  return vu::VU_OK;
}
