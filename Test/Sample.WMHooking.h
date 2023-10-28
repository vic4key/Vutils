#pragma once

#include "Sample.h"

#include <conio.h>

DEF_SAMPLE(WMHooking)
{
  // @refer to source codes of exported callback functions (fnXProc) in `Test/Test.WH.DLL/dllmain.cpp`

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

  vu::WMHooking wh(pid, DLL_PATH);

  std::tcout << ts("Enter to start Window Message hooking ...") << std::endl; _getch();

  wh.install(WH_CBT, UND(fnCBTProc));
  wh.install(WH_MOUSE, UND(fnMouseProc));
  wh.install(WH_KEYBOARD, UND(fnKeyboardProc));

  std::tcout << ts("Enter to stop Window Message hooking ...") << std::endl; _getch();

  wh.uninstall(WH_CBT);
  wh.uninstall(WH_MOUSE);
  wh.uninstall(WH_KEYBOARD);

  return vu::VU_OK;
}
