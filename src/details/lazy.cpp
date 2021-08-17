/**
 * @file   lazy.cpp
 * @author Vic P.
 * @brief  Implementation for Lazy
 */

#include "lazy.h"
#include "defs.h"

#define ERROR_CODE(code) (ECBase + code)

#define VU_OBJ_GET_API(O, F)  pfn ## F = (Pfn ## F)O.GetProcAddress(_T( # F ));
#define VU_OBJ_GET_APIA(O, F) pfn ## F ## A = (Pfn ## F ## A)O.GetProcAddress(_T( # F ));
#define VU_OBJ_GET_APIW(O, F) pfn ## F ## W = (Pfn ## F ## W)O.GetProcAddress(_T( # F ));

namespace vu
{

/**
 * Variables
 */

bool g_Initialized_DLL_TLHELP32 = false;
bool g_Initialized_DLL_PSAPI = false;
bool g_Initialized_DLL_MISC = false;

// THHELP32

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

// PSAPI

PfnGetModuleBaseNameA pfnGetModuleBaseNameA = nullptr;
PfnGetModuleBaseNameW pfnGetModuleBaseNameW = nullptr;

PfnGetModuleFileNameExA pfnGetModuleFileNameExA = nullptr;
PfnGetModuleFileNameExW pfnGetModuleFileNameExW = nullptr;

PfnGetModuleInformation pfnGetModuleInformation = nullptr;

PfnGetProcessMemoryInfo pfnGetProcessMemoryInfo = nullptr;

PfnQueryFullProcessImageNameA pfnQueryFullProcessImageNameA = nullptr;
PfnQueryFullProcessImageNameW pfnQueryFullProcessImageNameW = nullptr;

PfnGetMappedFileNameA pfnGetMappedFileNameA = nullptr;
PfnGetMappedFileNameW pfnGetMappedFileNameW = nullptr;

// MISC

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

VUResult vuapi Initialize_DLL_LAZY()
{
  VUResult result = VU_OK;

  result |= Initialize_DLL_PSAPI();
  result |= Initialize_DLL_TLHELP32();

  return result;
}

VUResult vuapi Initialize_DLL_TLHELP32()
{
  const VUResult ECBase = 100;

  if (g_Initialized_DLL_TLHELP32)
  {
    return VU_OK;
  }

  CLibrary kernel32(_T("kernel32.dll"));
  if (!kernel32.IsAvailable())
  {
    return ERROR_CODE(__LINE__);
  }

  VU_OBJ_GET_API(kernel32, CreateToolhelp32Snapshot);
  if (pfnCreateToolhelp32Snapshot == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_OBJ_GET_APIA(kernel32, Process32First);
  if (pfnProcess32FirstA == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_OBJ_GET_APIA(kernel32, Process32Next);
  if (pfnProcess32NextA == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_OBJ_GET_API(kernel32, Process32FirstW);
  if (pfnProcess32FirstA == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_OBJ_GET_API(kernel32, Process32NextW);
  if (pfnProcess32NextW == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_OBJ_GET_APIA(kernel32, Module32First);
  if (pfnModule32FirstA == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_OBJ_GET_APIA(kernel32, Module32Next);
  if (pfnModule32NextA == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_OBJ_GET_API(kernel32, Module32FirstW);
  if (pfnModule32FirstW == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_OBJ_GET_API(kernel32, Module32NextW);
  if (pfnModule32NextW == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_OBJ_GET_API(kernel32, EnumProcessModules);
  if (pfnEnumProcessModules == nullptr)
  {
    pfnEnumProcessModules = (PfnEnumProcessModules)kernel32.GetProcAddress(_T("K32EnumProcessModules"));
    if (pfnEnumProcessModules == nullptr)
    {
      return ERROR_CODE(__LINE__);
    }
  }

  VU_OBJ_GET_API(kernel32, EnumProcessModulesEx);
  if (pfnEnumProcessModulesEx == nullptr)
  {
    pfnEnumProcessModulesEx = (PfnEnumProcessModulesEx)kernel32.GetProcAddress(_T("K32EnumProcessModulesEx"));
    if (pfnEnumProcessModulesEx == nullptr)
    {
      return ERROR_CODE(__LINE__);
    }
  }

  VU_OBJ_GET_API(kernel32, EnumProcesses);
  if (pfnEnumProcesses == nullptr)
  {
    pfnEnumProcesses = (PfnEnumProcesses)kernel32.GetProcAddress(_T("K32EnumProcesses"));
    if (pfnEnumProcesses == nullptr)
    {
      return ERROR_CODE(__LINE__);
    }
  }

  VU_OBJ_GET_API(kernel32, GetModuleBaseNameA);
  if (pfnGetModuleBaseNameA == nullptr)
  {
    pfnGetModuleBaseNameA = (PfnGetModuleBaseNameA)kernel32.GetProcAddress(_T("K32GetModuleBaseNameA"));
    if (pfnGetModuleBaseNameA == nullptr)
    {
      return ERROR_CODE(__LINE__);
    }
  }

  VU_OBJ_GET_API(kernel32, GetModuleBaseNameW);
  if (pfnGetModuleBaseNameW == nullptr)
  {
    pfnGetModuleBaseNameW = (PfnGetModuleBaseNameW)kernel32.GetProcAddress(_T("K32GetModuleBaseNameW"));
    if (pfnGetModuleBaseNameW == nullptr)
    {
      return ERROR_CODE(__LINE__);
    }
  }

  VU_OBJ_GET_API(kernel32, QueryFullProcessImageNameA);
  if (pfnQueryFullProcessImageNameA == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_OBJ_GET_API(kernel32, QueryFullProcessImageNameW)
  if (pfnQueryFullProcessImageNameW == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  g_Initialized_DLL_TLHELP32 = true;

  return VU_OK;
}

VUResult vuapi Initialize_DLL_PSAPI()
{
  const VUResult ECBase = 200;

  if (g_Initialized_DLL_PSAPI)
  {
    return VU_OK;
  }

  CLibrary kernel32(_T("kernel32.dll"));
  if (!kernel32.IsAvailable())
  {
    return ERROR_CODE(__LINE__);
  }

  CLibrary psapi(_T("psapi.dll"));
  if (!psapi.IsAvailable())
  {
    return ERROR_CODE(__LINE__);
  }

  pfnGetProcessMemoryInfo = (PfnGetProcessMemoryInfo)kernel32.GetProcAddress(_T("K32GetProcessMemoryInfo"));
  if (pfnGetProcessMemoryInfo == nullptr)
  {
    VU_OBJ_GET_API(psapi, GetProcessMemoryInfo);
    if (pfnGetProcessMemoryInfo == nullptr)
    {
      return ERROR_CODE(__LINE__);
    }
  }

  pfnGetModuleFileNameExA = (PfnGetModuleFileNameExA)kernel32.GetProcAddress(_T("K32GetModuleFileNameExA"));
  if (pfnGetModuleFileNameExA == nullptr)
  {
    VU_OBJ_GET_API(psapi, GetModuleFileNameExA);
    if (pfnGetModuleFileNameExA == nullptr)
    {
      return ERROR_CODE(__LINE__);
    }
  }

  pfnGetModuleFileNameExW = (PfnGetModuleFileNameExW)kernel32.GetProcAddress(_T("K32GetModuleFileNameExW")); 
  if (pfnGetModuleFileNameExW == nullptr)
  {
    VU_OBJ_GET_API(psapi, GetModuleFileNameExW);
    if (pfnGetModuleFileNameExW == nullptr)
    {
      return ERROR_CODE(__LINE__);
    }
  }

  pfnGetModuleInformation = (PfnGetModuleInformation)kernel32.GetProcAddress(_T("K32GetModuleInformation"));
  if (pfnGetModuleInformation == nullptr)
  {
    VU_OBJ_GET_API(psapi, GetModuleInformation)
    if (pfnGetModuleInformation == nullptr)
    {
      return ERROR_CODE(__LINE__);
    }
  }

  pfnGetMappedFileNameA = (PfnGetMappedFileNameA)kernel32.GetProcAddress(_T("K32GetMappedFileNameA"));
  if (pfnGetMappedFileNameA == nullptr)
  {
    VU_OBJ_GET_API(psapi, GetMappedFileNameA)
    if (pfnGetMappedFileNameA == nullptr)
    {
      return ERROR_CODE(__LINE__);
    }
  }

  pfnGetMappedFileNameW = (PfnGetMappedFileNameW)kernel32.GetProcAddress(_T("K32GetMappedFileNameW"));
  if (pfnGetMappedFileNameW == nullptr)
  {
    VU_OBJ_GET_API(psapi, GetMappedFileNameW)
    if (pfnGetMappedFileNameW == nullptr)
    {
      return ERROR_CODE(__LINE__);
    }
  }

  g_Initialized_DLL_PSAPI = true;

  return VU_OK;
}

VUResult vuapi Initialize_DLL_MISC()
{
  const VUResult ECBase = 300;

  if (g_Initialized_DLL_MISC)
  {
    return VU_OK;
  }

  VU_GET_API(msvcrt.dll, _vscwprintf);
  if (pfn_vscwprintf == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_GET_API(msvcrt.dll, _vscprintf);
  if (pfn_vscprintf == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_GET_API(msvcrt.dll, _vscwprintf);
  if (pfn_vscwprintf == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_GET_API(msvcrt.dll, _vsnprintf);
  if (pfn_vsnprintf == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_GET_API(advapi32.dll, CheckTokenMembership);
  if (pfnCheckTokenMembership == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_GET_API(advapi32.dll, RegQueryReflectionKey);
  if (pfnRegQueryReflectionKey == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_GET_API(advapi32.dll, RegEnableReflectionKey);
  if (pfnRegEnableReflectionKey == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  VU_GET_API(advapi32.dll, RegDisableReflectionKey);
  if (pfnRegDisableReflectionKey == nullptr)
  {
    return ERROR_CODE(__LINE__);
  }

  g_Initialized_DLL_MISC = true;

  return VU_OK;
}

} // namespace vu