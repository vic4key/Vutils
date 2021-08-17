/**
 * @file   lazy.h
 * @author Vic P.
 * @brief  Header for Lazy
 */

#pragma once

#include "Vutils.h"

/**
 * To declare a new built-in function
 * 1. Defines its function type
 * 2. Defines its extern variable in the header file
 * 3. Defines its variable in the implementation file
 * 4. Gets and assigns its address in the initialize function
 */

#include <TlHelp32.h>

namespace vu
{

#ifndef PSAPI_VERSION
#define LIST_MODULES_32BIT    0x01
#define LIST_MODULES_64BIT    0x02
#define LIST_MODULES_ALL      0x03
#define LIST_MODULES_DEFAULT  0x04
#endif

typedef struct _PROCESSENTRY32A
{
  DWORD   dwSize;
  DWORD   cntUsage;
  DWORD   th32ProcessID;          // this process
  ULONG   th32DefaultHeapID;
  DWORD   th32ModuleID;           // associated exe
  DWORD   cntThreads;
  DWORD   th32ParentProcessID;    // this process's parent process
  LONG    pcPriClassBase;         // Base priority of process's threads
  DWORD   dwFlags;
  CHAR    szExeFile[MAX_PATH];    // Path
} TProcessEntry32A, *PProcessEntry32A;

typedef struct _PROCESSENTRY32W
{
  DWORD   dwSize;
  DWORD   cntUsage;
  DWORD   th32ProcessID;          // this process
  ULONG   th32DefaultHeapID;
  DWORD   th32ModuleID;           // associated exe
  DWORD   cntThreads;
  DWORD   th32ParentProcessID;    // this process's parent process
  LONG    pcPriClassBase;         // Base priority of process's threads
  DWORD   dwFlags;
  WCHAR   szExeFile[MAX_PATH];    // Path
} TProcessEntry32W, *PProcessEntry32W;

#define MAX_MODULE_NAME32 255

typedef struct _MODULEENTRY32A
{
  DWORD   dwSize;
  DWORD   th32ModuleID;       // This module
  DWORD   th32ProcessID;      // owning process
  DWORD   GlblcntUsage;       // Global usage count on the module
  DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
  BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
  DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
  HMODULE hModule;            // The hModule of this module in th32ProcessID's context
  char    szModule[MAX_MODULE_NAME32 + 1];
  char    szExePath[MAX_PATH];
} TModuleEntry32A, *PModuleEntry32A;

typedef struct _MODULEENTRY32W
{
  DWORD   dwSize;
  DWORD   th32ModuleID;       // This module
  DWORD   th32ProcessID;      // owning process
  DWORD   GlblcntUsage;       // Global usage count on the module
  DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
  BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
  DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
  HMODULE hModule;            // The hModule of this module in th32ProcessID's context
  WCHAR   szModule[MAX_MODULE_NAME32 + 1];
  WCHAR   szExePath[MAX_PATH];
} TModuleEntry32W, *PModuleEntry32W;

/**
 * Types
 */

// TLHELP32

typedef HANDLE (WINAPI *PfnCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);

typedef BOOL (WINAPI *PfnProcess32FirstA)(HANDLE hSnapshot, PProcessEntry32A lppe);
typedef BOOL (WINAPI *PfnProcess32NextA)(HANDLE hSnapshot, PProcessEntry32A lppe);
typedef BOOL (WINAPI *PfnProcess32FirstW)(HANDLE hSnapshot, PProcessEntry32W lppe);
typedef BOOL (WINAPI *PfnProcess32NextW)(HANDLE hSnapshot, PProcessEntry32W lppe);

typedef BOOL (WINAPI *PfnModule32FirstA)(HANDLE hSnapshot, PModuleEntry32A lpme);
typedef BOOL (WINAPI *PfnModule32NextA)(HANDLE hSnapshot, PModuleEntry32A lpme);
typedef BOOL (WINAPI *PfnModule32FirstW)(HANDLE hSnapshot, PModuleEntry32W lpme);
typedef BOOL (WINAPI *PfnModule32NextW)(HANDLE hSnapshot, PModuleEntry32W lpme);

typedef BOOL (WINAPI *PfnEnumProcessModules)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded);
typedef BOOL (WINAPI *PfnEnumProcessModulesEx)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded, DWORD dwFilterFlag);

typedef BOOL (WINAPI *PfnEnumProcesses)(DWORD *pProcessIds, DWORD cb, DWORD *pBytesReturned);

// PSAPI

typedef DWORD (WINAPI *PfnGetModuleBaseNameA)(HANDLE hProcess, HMODULE hModule, LPSTR lpBaseName, DWORD nSize);
typedef DWORD (WINAPI *PfnGetModuleBaseNameW)(HANDLE hProcess, HMODULE hModule, LPWSTR lpBaseName, DWORD nSize);

typedef DWORD (WINAPI* PfnGetModuleFileNameExA)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize);
typedef DWORD (WINAPI* PfnGetModuleFileNameExW)(HANDLE hProcess, HMODULE hModule, LPWSTR lpFilename, DWORD nSize);

typedef BOOL  (WINAPI *PfnGetModuleInformation)(HANDLE hProcess, HMODULE hModule, LPMODULEINFO_PTR lpmodinfo, DWORD cb);

typedef BOOL (WINAPI *PfnGetProcessMemoryInfo)(HANDLE Process, PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb);

typedef BOOL (WINAPI *PfnQueryFullProcessImageNameA)(HANDLE hProcess, DWORD  dwFlags, LPSTR lpExeName, PDWORD lpdwSize);
typedef BOOL (WINAPI *PfnQueryFullProcessImageNameW)(HANDLE hProcess, DWORD  dwFlags, LPWSTR lpExeName, PDWORD lpdwSize);

typedef DWORD (WINAPI * PfnGetMappedFileNameA)(HANDLE hProcess, LPVOID lpv, LPSTR  lpFilename, DWORD nSize);
typedef DWORD (WINAPI * PfnGetMappedFileNameW)(HANDLE hProcess, LPVOID lpv, LPWSTR lpFilename, DWORD nSize);

// MISC

typedef int(__cdecl* Pfn_vscprintf)(const char* format, va_list argptr);
typedef int(__cdecl* Pfn_vscwprintf)(const wchar_t* format, va_list argptr);
typedef int(__cdecl* Pfn_vsnprintf)(char* s, size_t n, const char* format, va_list arg);
typedef int(__cdecl* Pfn_vswprintf)(wchar_t* s, size_t len, const wchar_t* format, va_list arg);
typedef BOOL(WINAPI* PfnCheckTokenMembership)(HANDLE TokenHandle, PSID SidToCheck, PBOOL IsMember);
typedef LONG(WINAPI* PfnRegQueryReflectionKey)(HKEY hBase, BOOL* bIsReflectionDisabled);
typedef LONG(WINAPI* PfnRegEnableReflectionKey)(HKEY hBase);
typedef LONG(WINAPI* PfnRegDisableReflectionKey)(HKEY hBase);

/**
 * Variables
 */

#define DECLARE_LAZY(prefix, suffix)\
	/* TLHELP32 */\
	\
	prefix PfnCreateToolhelp32Snapshot pfnCreateToolhelp32Snapshot suffix \
	\
	prefix PfnProcess32FirstA pfnProcess32FirstA suffix \
	prefix PfnProcess32NextA pfnProcess32NextA   suffix \
	prefix PfnProcess32FirstW pfnProcess32FirstW suffix \
	prefix PfnProcess32NextW pfnProcess32NextW   suffix \
	\
	prefix PfnModule32FirstA pfnModule32FirstA suffix \
	prefix PfnModule32NextA pfnModule32NextA   suffix \
	prefix PfnModule32FirstW pfnModule32FirstW suffix \
	prefix PfnModule32NextW pfnModule32NextW   suffix \
	\
	prefix PfnEnumProcessModules pfnEnumProcessModules suffix \
	prefix PfnEnumProcessModulesEx pfnEnumProcessModulesEx suffix \
	\
	prefix PfnEnumProcesses pfnEnumProcesses suffix \
	\
	/* PSAPI */\
	\
	prefix PfnGetModuleBaseNameA pfnGetModuleBaseNameA suffix \
	prefix PfnGetModuleBaseNameW pfnGetModuleBaseNameW suffix \
	\
	prefix PfnGetModuleFileNameExA pfnGetModuleFileNameExA suffix \
	prefix PfnGetModuleFileNameExW pfnGetModuleFileNameExW suffix \
	\
	prefix PfnGetModuleInformation pfnGetModuleInformation suffix \
	\
	prefix PfnGetProcessMemoryInfo pfnGetProcessMemoryInfo suffix \
	\
	prefix PfnQueryFullProcessImageNameA pfnQueryFullProcessImageNameA suffix \
	prefix PfnQueryFullProcessImageNameW pfnQueryFullProcessImageNameW suffix \
	\
	prefix PfnGetMappedFileNameA pfnGetMappedFileNameA suffix \
	prefix PfnGetMappedFileNameW pfnGetMappedFileNameW suffix \
	\
	/* MISC */\
	\
	prefix Pfn_vsnprintf pfn_vsnprintf suffix \
	prefix Pfn_vswprintf pfn_vswprintf suffix \
	prefix Pfn_vscprintf pfn_vscprintf suffix \
	prefix Pfn_vscwprintf pfn_vscwprintf suffix \
	\
	prefix PfnCheckTokenMembership pfnCheckTokenMembership suffix \
	prefix PfnRegQueryReflectionKey pfnRegQueryReflectionKey suffix \
	prefix PfnRegEnableReflectionKey pfnRegEnableReflectionKey suffix \
	prefix PfnRegDisableReflectionKey pfnRegDisableReflectionKey suffix \
	\
	/* END */

DECLARE_LAZY(extern,;)

extern bool g_Initialized_DLL_TLHELP32;
extern bool g_Initialized_DLL_PSAPI;
extern bool g_Initialized_DLL_MISC;

/**
 * Functions
 */

/**
 * Initialize dynamic functions.
 * return VU_OK if succeeds otherwise return the following error codes :
 *	- 1xx - TLHELP32
 *	- 2xx - PSAPI
 *	- 3xx - MISC
 */

VUResult vuapi Initialize_DLL_LAZY();
VUResult vuapi Initialize_DLL_MISC();
VUResult vuapi Initialize_DLL_PSAPI();
VUResult vuapi Initialize_DLL_TLHELP32();

} // namespace vu