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

typedef std::pair<ulong, HWND> PairPIDHWND;

BOOL CALLBACK fnFindTopWindowCallback(HWND hWnd, LPARAM lParam)
{
  auto& params = *(PairPIDHWND*)lParam;

  DWORD thePID = 0;

  if (GetWindowThreadProcessId(hWnd, &thePID) && thePID == params.first)
  {
    params.second = hWnd;
    return FALSE;
  }

  return TRUE;
}

HWND vuapi FindTopWindow(ulong ulPID)
{
  PairPIDHWND params(ulPID, nullptr);

  auto ret = EnumWindows(fnFindTopWindowCallback, (LPARAM)&params);

  return params.second;
}

} // namespace vu