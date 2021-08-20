#pragma once

#include "Sample.h"

#include <conio.h>

DEF_SAMPLE(WMHook)
{
  // extern "C" __declspec(dllexport) LRESULT CALLBACK fnXProc(int nCode, WPARAM wParam, LPARAM lParam)
  // {
  //   return CallNextHookEx(nullptr, nCode, wParam, lParam);
  // }

  #define VU_STRINGIZE(s) ts( #s )

  #ifdef _WIN64
  #define EXE_NAME ts("x64dbg.exe")
  #define DLL_PATH ts("Test.WH.x64.dll")
  #define UND(s) VU_STRINGIZE(s)
  #else // _WIN32
  #define EXE_NAME ts("x32dbg.exe")
  #define DLL_PATH ts("Test.WH.x86.dll")
  #define UND(s) VU_STRINGIZE(_) VU_STRINGIZE(s) VU_STRINGIZE(@12)
  #endif // _WIN64

  auto pids = vu::name_to_pid(EXE_NAME);
  if (pids.empty())
  {
    std::cout << "Not found the target process for Window Hooking ..." << std::endl;
    return vu::VU_OK;
  }

  auto pid = pids.back();

  vu::CWMHook wh(pid, DLL_PATH);

  std::tcout << ts("Enter to start Window Message hooking ...") << std::endl; _getch();

  assert(wh.Start(WH_CBT, UND(fnCBTProc)) == vu::VU_OK);
  assert(wh.Start(WH_MOUSE, UND(fnMouseProc)) == vu::VU_OK);
  assert(wh.Start(WH_KEYBOARD, UND(fnKeyboardProc)) == vu::VU_OK);

  std::tcout << ts("Enter to stop Window Message hooking ...") << std::endl; _getch();

  assert(wh.Stop(WH_CBT) == vu::VU_OK);
  assert(wh.Stop(WH_MOUSE) == vu::VU_OK);
  assert(wh.Stop(WH_KEYBOARD) == vu::VU_OK);

  return vu::VU_OK;
}
