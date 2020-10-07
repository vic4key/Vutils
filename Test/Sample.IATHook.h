#pragma once

#include "Sample.h"

typedef int (WINAPI* PfnMessageBoxA)(HWND   hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT   uType);
typedef int (WINAPI* PfnMessageBoxW)(HWND   hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT   uType);

PfnMessageBoxA pfnMessageBoxA = nullptr;
PfnMessageBoxW pfnMessageBoxW = nullptr;

int WINAPI HfnMessageBoxA(HWND   hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT   uType)
{
  lpText = "MessageBoxA -> Hooked";
  return pfnMessageBoxA(hWnd, lpText, lpCaption, uType);
}

int WINAPI HfnMessageBoxW(HWND   hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT   uType)
{
  lpText = L"MessageBoxW -> Hooked";
  return pfnMessageBoxW(hWnd, lpText, lpCaption, uType);
}

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
