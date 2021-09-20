#include "pch.h"
#include "WH.h"

#include <vu>

LRESULT CALLBACK fnCBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  vu::msg_debug_A("[CBT] nCode = %d, wParam = %08X, lParam = %08X", nCode, wParam, lParam);
  return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK fnMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  vu::msg_debug_A("[Mouse] nCode = %d, wParam = %08X, lParam = %08X", nCode, wParam, lParam);
  return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK fnKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  vu::msg_debug_A("[Keyboard] nCode = %d, wParam = %08X, lParam = %08X", nCode, wParam, lParam);
  return CallNextHookEx(nullptr, nCode, wParam, lParam);
}