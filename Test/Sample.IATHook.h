#pragma once

#include "Sample.h"

DEF_SAMPLE(IATHook)
{
  VU_API_IAT_OVERRIDE(Test.exe, user32.dll, MessageBoxA);
  VU_API_IAT_OVERRIDE(Test.exe, user32.dll, MessageBoxW);

  MessageBoxA(vu::GetConsoleWindow(),  "The first message.",  "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindow(), L"The first message.", L"W", MB_OK);

  VU_API_IAT_RESTORE(Test.exe, user32.dll, MessageBoxA);
  VU_API_IAT_RESTORE(Test.exe, user32.dll, MessageBoxW);

  MessageBoxA(vu::GetConsoleWindow(),  "The second message.",  "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindow(), L"The second message.", L"W", MB_OK);

  return vu::VU_OK;
}
