// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "wh.h"

#include <vu>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
    {
      vu::MsgA("WH::DLL => LOADED");
    }
    break;

  case DLL_PROCESS_DETACH:
    {
      vu::MsgA("WH::DLL => UNLOADED");
    }
    break;
  }

  return TRUE;
}
