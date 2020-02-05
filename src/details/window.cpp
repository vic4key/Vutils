/**
 * @file   window.cpp
 * @author Vic P.
 * @brief  Implementation for Window
 */

#include "Vutils.h"

namespace vu
{

HWND vuapi GetConsoleWindow()
{
  typedef HWND (WINAPI *PfnGetConsoleWindow)();

  HWND hwConsole = NULL;

  PfnGetConsoleWindow pfnGetConsoleWindow = (PfnGetConsoleWindow)CLibrary::QuickGetProcAddress(
    _T("kernel32.dll"),
    _T("GetConsoleWindow")
  );

  if (pfnGetConsoleWindow)
  {
    hwConsole = pfnGetConsoleWindow();
  }

  return hwConsole;
}

HWND vuapi FindTopWindow(ulong ulPID)
{
  typedef std::pair<ulong, HWND> PairPIDHWND;
  PairPIDHWND params(ulPID, nullptr);

  auto ret = EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL
  {
    auto& params = *(PairPIDHWND*)lParam;

    DWORD thePID = 0;

    if (GetWindowThreadProcessId(hWnd, &thePID) && thePID == params.first)
    {
      params.second = hWnd;
      return FALSE;
    }

    return TRUE;
  }, (LPARAM)&params);

  return params.second;
}

} // namespace vu