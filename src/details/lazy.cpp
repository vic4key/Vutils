/**
 * @file   lazy.cpp
 * @author Vic P.
 * @brief  Implementation for Lazy
 */

#include "lazy.h"

#define ErrorCode(code) (ECBase + code)

#define VU_OBJ_GET_API(O, F) pfn ## F = (Pfn ## F)O.GetProcAddress(_T( # F ));
#define VU_OBJ_GET_APIA(O, F) pfn ## F ## A = (Pfn ## F ## A)O.GetProcAddress(_T( # F ));
#define VU_OBJ_GET_APIW(O, F) pfn ## F ## W = (Pfn ## F ## W)O.GetProcAddress(_T( # F ));

namespace vu
{

bool g_HasToolHelp32  = false;
bool g_HasMiscRoutine = false;

/**
 * Variables
 */

PfnCreateToolhelp32Snapshot pfnCreateToolhelp32Snapshot = nullptr;

PfnProcess32FirstA pfnProcess32FirstA = nullptr;
PfnProcess32NextA pfnProcess32NextA   = nullptr;
PfnProcess32FirstW pfnProcess32FirstW = nullptr;
PfnProcess32NextW pfnProcess32NextW   = nullptr;

PfnModule32FirstA pfnModule32FirstA = nullptr;
PfnModule32NextA pfnModule32NextA   = nullptr;
PfnModule32FirstW pfnModule32FirstW = nullptr;
PfnModule32NextW pfnModule32NextW   = nullptr;

PfnEnumProcessModules pfnEnumProcessModules = nullptr;
PfnEnumProcessModulesEx pfnEnumProcessModulesEx = nullptr;

PfnEnumProcesses pfnEnumProcesses = nullptr;

PfnGetModuleBaseNameA pfnGetModuleBaseNameA = nullptr;
PfnGetModuleBaseNameW pfnGetModuleBaseNameW = nullptr;

PfnQueryFullProcessImageNameA pfnQueryFullProcessImageNameA = nullptr;
PfnQueryFullProcessImageNameW pfnQueryFullProcessImageNameW = nullptr;

/**
 * Variables
 */

Pfn_vsnprintf pfn_vsnprintf = nullptr;
Pfn_vswprintf pfn_vswprintf = nullptr;
Pfn_vscprintf pfn_vscprintf = nullptr;
Pfn_vscwprintf pfn_vscwprintf = nullptr;

PfnCheckTokenMembership pfnCheckTokenMembership = nullptr;
PfnRegQueryReflectionKey pfnRegQueryReflectionKey = nullptr;
PfnRegEnableReflectionKey pfnRegEnableReflectionKey = nullptr;
PfnRegDisableReflectionKey pfnRegDisableReflectionKey = nullptr;

/**
 * Functions
 */

VUResult vuapi InitTlHelp32()
{
  const VUResult ECBase = 100;

  if (g_HasToolHelp32)
  {
    return VU_OK;
  }

  CLibrary kernel32(_T("kernel32.dll"));
  if (!kernel32.IsAvailable())
  {
    return ErrorCode(1);
  }

  VU_OBJ_GET_API(kernel32, CreateToolhelp32Snapshot);
  if (pfnCreateToolhelp32Snapshot == nullptr)
  {
    return ErrorCode(2);
  }

  VU_OBJ_GET_APIA(kernel32, Process32First);
  if (pfnProcess32FirstA == nullptr)
  {
    return ErrorCode(3);
  }

  VU_OBJ_GET_APIA(kernel32, Process32Next);
  if (pfnProcess32NextA == nullptr)
  {
    return ErrorCode(4);
  }

  VU_OBJ_GET_API(kernel32, Process32FirstW);
  if (pfnProcess32FirstA == nullptr)
  {
    return ErrorCode(5);
  }

  VU_OBJ_GET_API(kernel32, Process32NextW);
  if (pfnProcess32NextW == nullptr)
  {
    return ErrorCode(6);
  }

  VU_OBJ_GET_APIA(kernel32, Module32First);
  if (pfnModule32FirstA == nullptr)
  {
    return ErrorCode(7);
  }

  VU_OBJ_GET_APIA(kernel32, Module32Next);
  if (pfnModule32NextA == nullptr)
  {
    return ErrorCode(8);
  }

  VU_OBJ_GET_API(kernel32, Module32FirstW);
  if (pfnModule32FirstW == nullptr)
  {
    return ErrorCode(9);
  }

  VU_OBJ_GET_API(kernel32, Module32NextW);
  if (pfnModule32NextW == nullptr)
  {
    return ErrorCode(10);
  }

  VU_OBJ_GET_API(kernel32, EnumProcessModules);
  if (pfnEnumProcessModules == nullptr)
  {
    pfnEnumProcessModules = (PfnEnumProcessModules)kernel32.GetProcAddress(_T("K32EnumProcessModules"));
    if (pfnEnumProcessModules == nullptr)
    {
      return ErrorCode(11);
    }
  }

  VU_OBJ_GET_API(kernel32, EnumProcessModulesEx);
  if (pfnEnumProcessModulesEx == nullptr)
  {
    pfnEnumProcessModulesEx = (PfnEnumProcessModulesEx)kernel32.GetProcAddress(_T("K32EnumProcessModulesEx"));
    if (pfnEnumProcessModulesEx == nullptr)
    {
      return ErrorCode(12);
    }
  }

  VU_OBJ_GET_API(kernel32, EnumProcesses);
  if (pfnEnumProcesses == nullptr)
  {
    pfnEnumProcesses = (PfnEnumProcesses)kernel32.GetProcAddress(_T("K32EnumProcesses"));
    if (pfnEnumProcesses == nullptr)
    {
      return ErrorCode(13);
    }
  }

  VU_OBJ_GET_API(kernel32, GetModuleBaseNameA);
  if (pfnGetModuleBaseNameA == nullptr)
  {
    pfnGetModuleBaseNameA = (PfnGetModuleBaseNameA)kernel32.GetProcAddress(_T("K32GetModuleBaseNameA"));
    if (pfnGetModuleBaseNameA == nullptr)
    {
      return ErrorCode(14);
    }
  }

  VU_OBJ_GET_API(kernel32, GetModuleBaseNameW);
  if (pfnGetModuleBaseNameW == nullptr)
  {
    pfnGetModuleBaseNameW = (PfnGetModuleBaseNameW)kernel32.GetProcAddress(_T("K32GetModuleBaseNameW"));
    if (pfnGetModuleBaseNameW == nullptr)
    {
      return ErrorCode(15);
    }
  }

  VU_OBJ_GET_API(kernel32, QueryFullProcessImageNameA);
  if (pfnQueryFullProcessImageNameA == nullptr)
  {
    return ErrorCode(16);
  }

  VU_OBJ_GET_API(kernel32, QueryFullProcessImageNameW)
  if (pfnQueryFullProcessImageNameW == nullptr)
  {
    return ErrorCode(17);
  }

  g_HasToolHelp32 = true;

  return VU_OK;
}

VUResult vuapi InitMiscRoutine()
{
  const VUResult ECBase = 200;

  if (g_HasMiscRoutine)
  {
    return VU_OK;
  }

  VU_GET_API(msvcrt.dll, _vscwprintf);
  if (pfn_vscwprintf == nullptr)
  {
    return ErrorCode(1);
  }

  VU_GET_API(msvcrt.dll, _vscprintf);
  if (pfn_vscprintf == nullptr)
  {
    return ErrorCode(2);
  }

  VU_GET_API(msvcrt.dll, _vscwprintf);
  if (pfn_vscwprintf == nullptr)
  {
    return ErrorCode(3);
  }

  VU_GET_API(msvcrt.dll, _vsnprintf);
  if (pfn_vsnprintf == nullptr)
  {
    return ErrorCode(4);
  }

  VU_GET_API(advapi32.dll, CheckTokenMembership);
  if (pfnCheckTokenMembership == nullptr)
  {
    return ErrorCode(10);
  }

  VU_GET_API(advapi32.dll, RegQueryReflectionKey);
  if (pfnRegQueryReflectionKey == nullptr)
  {
    return ErrorCode(11);
  }

  VU_GET_API(advapi32.dll, RegEnableReflectionKey);
  if (pfnRegEnableReflectionKey == nullptr)
  {
    return ErrorCode(12);
  }

  VU_GET_API(advapi32.dll, RegDisableReflectionKey);
  if (pfnRegDisableReflectionKey == nullptr)
  {
    return ErrorCode(13);
  }

  g_HasMiscRoutine = true;

  return VU_OK;
}

} // namespace vu