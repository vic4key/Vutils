#pragma once

#include "Sample.h"

#include <conio.h>

DEF_SAMPLE(WMHook)
{
  // extern "C" __declspec(dllexport) LRESULT CALLBACK fnXProc(int nCode, WPARAM wParam, LPARAM lParam)
  // {
  //   return CallNextHookEx(nullptr, nCode, wParam, lParam);
  // }

  #define VU_STRINGIZE(s) _T( #s )

  #ifdef _WIN64
  #define EXE_NAME _T("x64dbg.exe")
  #define DLL_PATH _T("Test.WH.x64.dll")
  #define UND(s) VU_STRINGIZE(s)
  #else // _WIN32
  #define EXE_NAME _T("x32dbg.exe")
  #define DLL_PATH _T("Test.WH.x86.dll")
  #define UND(s) VU_STRINGIZE(_##s##@12)
  #endif // _WIN64

  auto pids = vu::NameToPID(EXE_NAME);
  assert(!pids.empty());
  auto PID = pids.back();

  vu::CWMHook wh(PID, DLL_PATH);

  std::tcout << _T("Enter to start Window Message hooking ...") << std::endl; _getch();

  assert(wh.Start(WH_CBT, UND(fnCBTProc)) == vu::VU_OK);
  assert(wh.Start(WH_MOUSE, UND(fnMouseProc)) == vu::VU_OK);
  assert(wh.Start(WH_KEYBOARD, UND(fnKeyboardProc)) == vu::VU_OK);

  std::tcout << _T("Enter to stop Window Message hooking ...") << std::endl; _getch();

  assert(wh.Stop(WH_CBT) == vu::VU_OK);
  assert(wh.Stop(WH_MOUSE) == vu::VU_OK);
  assert(wh.Stop(WH_KEYBOARD) == vu::VU_OK);

  return vu::VU_OK;
}
