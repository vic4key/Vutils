// dllmain.cpp : Defines the entry point for the DLL application.

#include <vu>

#ifdef __cplusplus 
#define DLL_API extern "C" __declspec(dllexport)
#else
#define DLL_API __declspec(dllexport)
#endif

DLL_API LRESULT CALLBACK fnCBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  vu::msg_debug_A("[CBT] nCode = %d, wParam = %08X, lParam = %08X", nCode, wParam, lParam);
  return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DLL_API LRESULT CALLBACK fnMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  vu::msg_debug_A("[Mouse] nCode = %d, wParam = %08X, lParam = %08X", nCode, wParam, lParam);
  return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DLL_API LRESULT CALLBACK fnKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  vu::msg_debug_A("[Keyboard] nCode = %d, wParam = %08X, lParam = %08X", nCode, wParam, lParam);
  return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  return TRUE;
}
