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

/* MessageBoxA
00000000778F12B8 | 48 83 EC 38              | sub rsp,38                                                                 |
00000000778F12BC | 45 33 DB                 | xor r11d,r11d                                                              |
00000000778F12BF | 44 39 1D 76 0E 02 00     | cmp dword ptr ds:[77912135],r11d                                           |
00000000778F12C6 | 74 2E                    | je user32.778F12F6                                                         |
00000000778F12C8 | 65 48 8B 04 25 30 00 00  | mov rax,qword ptr gs:[30]                                                  |
00000000778F12D1 | 4C 8B 50 48              | mov r10,qword ptr ds:[rax+48]                                              |
---
000000013F0D2888 | 44 39 1C 25 35 21 91 77  | cmp dword ptr ds:[77912135],r11d                                           |
*/

DEF_SAMPLE(APIINLHook)
{
  vu::CAPIHook API[2];

  if (!VU_API_IL_OVERRIDE(API[0], user32.dll, MessageBoxA))
  {
    std::cout << "API::ATTACH -> MessageBoxA -> Failure" << std::endl;
    return 1;
  }

  std::cout << "API::ATTACH -> MessageBoxA -> Success" << std::endl;

  if (!VU_API_IL_OVERRIDE(API[1], user32.dll, MessageBoxW))
  {
    std::cout << "API::ATTACH -> MessageBoxW -> Failure" << std::endl;
    return 1;
  }

  std::cout << "API::ATTACH -> MessageBoxW -> Success" << std::endl;

  MessageBoxA(vu::GetConsoleWindow(),  "The first message.",  "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindow(), L"The first message.", L"W", MB_OK);

  if (!VU_API_IL_RESTORE(API[0], user32.dll, MessageBoxA))
  {
    std::cout << "API::DETACH -> MessageBoxA -> Failure" << std::endl;
  }

  std::cout << "API::DETACH -> MessageBoxA -> Success" << std::endl;

  if (!VU_API_IL_RESTORE(API[1], user32.dll, MessageBoxW))
  {
    std::cout << "API::DETACH -> MessageBoxW -> Failure" << std::endl;
  }

  std::cout << "API::DETACH -> MessageBoxW -> Success" << std::endl;

  MessageBoxA(vu::GetConsoleWindow(),  "The second message.",  "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindow(), L"The second message.", L"W", MB_OK);

  return vu::VU_OK;
}
