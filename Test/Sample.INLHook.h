#pragma once

#include "Sample.h"

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

DEF_SAMPLE(INLHook)
{
  vu::CAPIHook API[2];

  VU_API_INL_OVERRIDE(API[0], user32.dll, MessageBoxA);
  VU_API_INL_OVERRIDE(API[1], user32.dll, MessageBoxW);

  MessageBoxA(vu::get_console_window(),  "The first message.",  "A", MB_OK);
  MessageBoxW(vu::get_console_window(), L"The first message.", L"W", MB_OK);

  VU_API_INL_RESTORE(API[0], user32.dll, MessageBoxA);
  VU_API_INL_RESTORE(API[1], user32.dll, MessageBoxW);

  MessageBoxA(vu::get_console_window(), "The second message.", "A", MB_OK);
  MessageBoxW(vu::get_console_window(), L"The second message.", L"W", MB_OK);

  return vu::VU_OK;
}
