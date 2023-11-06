#pragma once

#include "Sample.h"

typedef int (WINAPI* PfnMessageBoxA)(HWND hWnd, LPCSTR lpText,  LPCSTR lpCaption,  UINT uType);
typedef int (WINAPI* PfnMessageBoxW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);

PfnMessageBoxA pfnMessageBoxA = nullptr;
PfnMessageBoxW pfnMessageBoxW = nullptr;

int WINAPI HfnMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
  lpText = "MessageBoxA -> Hooked";
  return pfnMessageBoxA(hWnd, lpText, lpCaption, uType);
}

int WINAPI HfnMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
  lpText = L"MessageBoxW -> Hooked";
  return pfnMessageBoxW(hWnd, lpText, lpCaption, uType);
}

DEF_SAMPLE(IATHooking)
{
  vu::Process process;
  process.attach(GetCurrentProcess());
  auto process_name = process.name();

  vu::IATHooking::instance().install(process_name, ts("user32.dll"), ts("MessageBoxA"), HfnMessageBoxA, (void**)&pfnMessageBoxA);
  vu::IATHooking::instance().install(process_name, ts("user32.dll"), ts("MessageBoxW"), HfnMessageBoxW, (void**)&pfnMessageBoxW);

  MessageBoxA(vu::get_console_window(), "The first message.", "A", MB_OK);
  MessageBoxW(vu::get_console_window(), L"The first message.", L"W", MB_OK);

  vu::IATHooking::instance().uninstall(process_name, ts("user32.dll"), ts("MessageBoxA"), (void**)&pfnMessageBoxA);
  vu::IATHooking::instance().uninstall(process_name, ts("user32.dll"), ts("MessageBoxW"), (void**)&pfnMessageBoxW);

  MessageBoxA(vu::get_console_window(),  "The second message.",  "A", MB_OK);
  MessageBoxW(vu::get_console_window(), L"The second message.", L"W", MB_OK);

  return vu::VU_OK;
}
