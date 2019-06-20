/**
 * @file   Vutils.cpp
 * @author Vic P.
 * @brief  Implementation for Vutils
 */

#include "Vutils.h"

#include <Windows.h>
#include <Winsvc.h>
#include <TlHelp32.h>
#include <cstdio>
#include <cassert>
#include <algorithm>

#ifdef VU_GUID_ENABLED
#include <rpc.h>
#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#pragma comment(lib, "rpcrt4.lib")
#endif
#endif // VU_GUID_ENABLED

#ifdef VU_SOCKET_ENABLED
#ifndef _WINSOCKAPI_
#include <WinSock2.h>
#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#pragma comment(lib, "ws2_32.lib")
#endif
#endif
#endif // VU_SOCKET_ENABLED

// Internal Buffer Handler
// Copyright (c) 2009-2017 Tsuda Kageyu
#include VU_3RD_INCL(MH/include/buffer.h)

#ifdef _MSC_VER
#pragma warning(disable: 4661) /* : no suitable definition provided for explicit template instantiation request. */
#endif

namespace vu
{

/* The common constants */

const std::string  VU_TITLE_BOXA =  "Vutils";
const std::wstring VU_TITLE_BOXW = L"Vutils";
const size_t MAX_SIZE = MAXBYTE;

/* Additional definations */

#ifndef PSAPI_VERSION
#define LIST_MODULES_32BIT    0x01
#define LIST_MODULES_64BIT    0x02
#define LIST_MODULES_ALL      0x03
#define LIST_MODULES_DEFAULT  0x04
#endif

// Error code based on range
#define ErrorCode(code) (ECBase + code)

#define VU_OGET_API(O, F) pfn ## F = (Pfn ## F)O.GetProcAddress(_T( # F ));
#define VU_OGET_APIA(O, F) pfn ## F ## A = (Pfn ## F ## A)O.GetProcAddress(_T( # F ));
#define VU_OGET_APIW(O, F) pfn ## F ## W = (Pfn ## F ## W)O.GetProcAddress(_T( # F ));

/* --------------------------------------------- Initialize ToolHelp32 --------------------------------------------- */

bool g_HasToolHelp32 = false;

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

typedef DWORD (WINAPI *PfnGetModuleBaseNameA)(HANDLE hProcess, HMODULE hModule, LPSTR lpBaseName, DWORD nSize);
typedef DWORD (WINAPI *PfnGetModuleBaseNameW)(HANDLE hProcess, HMODULE hModule, LPWSTR lpBaseName, DWORD nSize);

typedef BOOL (WINAPI *PfnQueryFullProcessImageNameA)(HANDLE hProcess, DWORD  dwFlags, LPSTR lpExeName, PDWORD lpdwSize);
typedef BOOL (WINAPI *PfnQueryFullProcessImageNameW)(HANDLE hProcess, DWORD  dwFlags, LPWSTR lpExeName, PDWORD lpdwSize);

PfnCreateToolhelp32Snapshot pfnCreateToolhelp32Snapshot = nullptr;

PfnProcess32FirstA pfnProcess32FirstA = nullptr;
PfnProcess32NextA pfnProcess32NextA   = nullptr;
PfnProcess32FirstW pfnProcess32FirstW = nullptr;
PfnProcess32NextW pfnProcess32NextW   = nullptr;

PfnModule32FirstA pfnModule32FirstA = nullptr;
PfnModule32NextA pfnModule32NextA   = nullptr;
PfnModule32FirstW pfnModule32FirstW = nullptr;
PfnModule32NextW pfnModule32NextW   = nullptr;

PfnEnumProcessModules pfnEnumProcessModules     = nullptr;
PfnEnumProcessModulesEx pfnEnumProcessModulesEx = nullptr;

PfnEnumProcesses pfnEnumProcesses = nullptr;

PfnGetModuleBaseNameA pfnGetModuleBaseNameA = nullptr;
PfnGetModuleBaseNameW pfnGetModuleBaseNameW = nullptr;

PfnQueryFullProcessImageNameA pfnQueryFullProcessImageNameA = nullptr;
PfnQueryFullProcessImageNameW pfnQueryFullProcessImageNameW = nullptr;

/**
 * Initialize Tool Help 32 functions.
 * return VU_OK if the function succeeds. Otherwise the error code 1xx.
 */
VUResult vuapi InitTlHelp32()
{
  const VUResult ECBase = 100;

  if (g_HasToolHelp32)
  {
    return VU_OK;
  }

  CLibrary kernel32(_T("kernel32.dll"));
  if (!kernel32.IsLibraryAvailable())
  {
    return ErrorCode(1);
  }

  VU_OGET_API(kernel32, CreateToolhelp32Snapshot);
  if (pfnCreateToolhelp32Snapshot == nullptr)
  {
    return ErrorCode(2);
  }

  VU_OGET_APIA(kernel32, Process32First);
  if (pfnProcess32FirstA == nullptr)
  {
    return ErrorCode(3);
  }

  VU_OGET_APIA(kernel32, Process32Next);
  if (pfnProcess32NextA == nullptr)
  {
    return ErrorCode(4);
  }

  VU_OGET_API(kernel32, Process32FirstW);
  if (pfnProcess32FirstA == nullptr)
  {
    return ErrorCode(5);
  }

  VU_OGET_API(kernel32, Process32NextW);
  if (pfnProcess32NextW == nullptr)
  {
    return ErrorCode(6);
  }

  VU_OGET_APIA(kernel32, Module32First);
  if (pfnModule32FirstA == nullptr)
  {
    return ErrorCode(7);
  }

  VU_OGET_APIA(kernel32, Module32Next);
  if (pfnModule32NextA == nullptr)
  {
    return ErrorCode(8);
  }

  VU_OGET_API(kernel32, Module32FirstW);
  if (pfnModule32FirstW == nullptr)
  {
    return ErrorCode(9);
  }

  VU_OGET_API(kernel32, Module32NextW);
  if (pfnModule32NextW == nullptr)
  {
    return ErrorCode(10);
  }

  VU_OGET_API(kernel32, EnumProcessModules);
  if (pfnEnumProcessModules == nullptr)
  {
    pfnEnumProcessModules = (PfnEnumProcessModules)kernel32.GetProcAddress(_T("K32EnumProcessModules"));
    if (pfnEnumProcessModules == nullptr)
    {
      return ErrorCode(11);
    }
  }

  VU_OGET_API(kernel32, EnumProcessModulesEx);
  if (pfnEnumProcessModulesEx == nullptr)
  {
    pfnEnumProcessModulesEx = (PfnEnumProcessModulesEx)kernel32.GetProcAddress(_T("K32EnumProcessModulesEx"));
    if (pfnEnumProcessModulesEx == nullptr)
    {
      return ErrorCode(12);
    }
  }

  VU_OGET_API(kernel32, EnumProcesses);
  if (pfnEnumProcesses == nullptr)
  {
    pfnEnumProcesses = (PfnEnumProcesses)kernel32.GetProcAddress(_T("K32EnumProcesses"));
    if (pfnEnumProcesses == nullptr)
    {
      return ErrorCode(13);
    }
  }

  VU_OGET_API(kernel32, GetModuleBaseNameA);
  if (pfnGetModuleBaseNameA == nullptr)
  {
    pfnGetModuleBaseNameA = (PfnGetModuleBaseNameA)kernel32.GetProcAddress(_T("K32GetModuleBaseNameA"));
    if (pfnGetModuleBaseNameA == nullptr)
    {
      return ErrorCode(14);
    }
  }

  VU_OGET_API(kernel32, GetModuleBaseNameW);
  if (pfnGetModuleBaseNameW == nullptr)
  {
    pfnGetModuleBaseNameW = (PfnGetModuleBaseNameW)kernel32.GetProcAddress(_T("K32GetModuleBaseNameW"));
    if (pfnGetModuleBaseNameW == nullptr)
    {
      return ErrorCode(15);
    }
  }

  VU_OGET_API(kernel32, QueryFullProcessImageNameA);
  if (pfnQueryFullProcessImageNameA == nullptr)
  {
    return ErrorCode(16);
  }

  VU_OGET_API(kernel32, QueryFullProcessImageNameW)
  if (pfnQueryFullProcessImageNameW == nullptr)
  {
    return ErrorCode(17);
  }

  g_HasToolHelp32 = true;

  return VU_OK;
}

/* ------------------------------------------- Initialize Misc Routines -------------------------------------------- */

bool g_HasMiscRoutine = false;

typedef int (__cdecl *Pfn_vscprintf)(const char* format, va_list argptr);
typedef int (__cdecl *Pfn_vscwprintf)(const wchar_t* format, va_list argptr);
typedef int (__cdecl *Pfn_vsnprintf)(char* s, size_t n, const char* format, va_list arg);
typedef int (__cdecl *Pfn_vswprintf)(wchar_t* s, size_t len, const wchar_t* format, va_list arg);
typedef BOOL (WINAPI *PfnCheckTokenMembership)(HANDLE TokenHandle, PSID SidToCheck, PBOOL IsMember);
typedef LONG (WINAPI *PfnRegQueryReflectionKey)(HKEY hBase, BOOL* bIsReflectionDisabled);
typedef LONG (WINAPI *PfnRegEnableReflectionKey)(HKEY hBase);
typedef LONG (WINAPI *PfnRegDisableReflectionKey)(HKEY hBase);

Pfn_vsnprintf pfn_vsnprintf = nullptr;
Pfn_vswprintf pfn_vswprintf = nullptr;
Pfn_vscprintf pfn_vscprintf   = nullptr;
Pfn_vscwprintf pfn_vscwprintf = nullptr;
PfnCheckTokenMembership pfnCheckTokenMembership = nullptr;
PfnRegQueryReflectionKey pfnRegQueryReflectionKey = nullptr;
PfnRegEnableReflectionKey pfnRegEnableReflectionKey = nullptr;
PfnRegDisableReflectionKey pfnRegDisableReflectionKey = nullptr;

/**
* Initialize miscellaneous functions.
* return VU_OK if the function succeeds. Otherwise the error code 2xx.
*/
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

/***************************************************** FUNCTIONS *****************************************************/

/* ------------------------------------------------- Misc Working -------------------------------------------------- */

bool vuapi IsAdministrator()
{
  BOOL IsMember = FALSE;
  SID_IDENTIFIER_AUTHORITY IA = SECURITY_NT_AUTHORITY;
  PSID SID;

  if (InitMiscRoutine() != VU_OK)
  {
    return FALSE;
  }

  if (AllocateAndInitializeSid(&IA, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &SID))
  {
    if (!pfnCheckTokenMembership(NULL, SID, &IsMember))
    {
      IsMember = FALSE;
    }

    FreeSid(SID);
  }

  return (IsMember != FALSE);
}

std::string vuapi GetEnviromentA(const std::string EnvName)
{
  std::string s;
  s.clear();

  std::unique_ptr<char[]> p(new char [MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), MAX_SIZE);

  ulong result = GetEnvironmentVariableA(EnvName.c_str(), p.get(), MAX_SIZE);
  if (result == ERROR_ENVVAR_NOT_FOUND || result == 0)
  {
    return s;
  }
  else if (result > MAX_SIZE)
  {
    result += 1;
    p.reset(new char[result]);
    if (p == nullptr)
    {
      return s;
    }

    ZeroMemory(p.get(), result);

    result = GetEnvironmentVariableA(EnvName.c_str(), p.get(), result);
    if (result == 0)
    {
      return s;
    }
  }

  s.assign(p.get());

  return s;
}

std::wstring vuapi GetEnviromentW(const std::wstring EnvName)
{
  std::wstring s;
  s.clear();

  std::unique_ptr<wchar[]> p(new wchar [MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), 2*MAX_SIZE);

  ulong result = GetEnvironmentVariableW(EnvName.c_str(), p.get(), 2*MAX_SIZE);
  if (result == ERROR_ENVVAR_NOT_FOUND || result == 0)
  {
    return s;
  }
  else if (result > MAX_SIZE)
  {
    result += 1;
    p.reset(new wchar[result]);
    if (p == nullptr)
    {
      return s;
    }

    ZeroMemory(p.get(), 2*result);

    result = GetEnvironmentVariableW(EnvName.c_str(), p.get(), 2*result);
    if (result == 0)
    {
      return s;
    }
  }

  s.assign(p.get());

  return s;
}

/* ------------------------------------------------- Math Working -------------------------------------------------- */

bool vuapi IsFlagOn(ulongptr ulFlags, ulongptr ulFlag)
{
  return ((ulFlags & ulFlag) == ulFlag);
}

intptr vuapi GCD(ulongptr count, ...) // Greatest Common Divisor -> BCNN
{
  va_list args;
  std::vector<intptr> array;
  intptr result = 0;

  array.clear();

  va_start(args, count);

  for (ulongptr i = 0; i < count; i++)
  {
    array.push_back(va_arg(args, intptr));
  }

  va_end(args);

  intptr min = *std::min_element(array.begin(), array.end());

  intptr pro = 1;
  for (auto i: array)
  {
    pro *= i;
  }

  if (pro == 0)
  {
    return 0;
  }

  ulongptr j;
  for (intptr i = min; i <= pro; i++)
  {
    for (j = 0; j < count; j++)
    {
      if (i % array[j] != 0)
      {
        break;
      }
    }
    if (j == count)
    {
      result = i;
      break;
    }
  }

  return result;
}

intptr vuapi LCM(ulongptr count, ...) // Least Common Multiples -> UCLN = | a1 * a2 * ... * an | / GCD(a1, a2, ..., an)
{
  va_list args;
  std::vector<intptr> array;
  intptr result = 0;

  array.clear();

  va_start(args, count);

  for (ulongptr i = 0; i < count; i++)
  {
    array.push_back(va_arg(args, intptr));
  }

  va_end(args);

  intptr min = *std::min_element(array.begin(), array.end());

  ulongptr j;
  for (intptr i = min - 1; i > 1; i--)
  {
    for (j = 0; j < count; j++)
    {
      if (array[j] % i != 0)
      {
        break;
      }
    }
    if (j == count)
    {
      result = i;
      break;
    }
  }

  return result;
}

void vuapi HexDump(const void* Data, int Size)
{
  const int DEFAULT_DUMP_COLUMN = 16;

  int i = 0;
  uchar Buffer[DEFAULT_DUMP_COLUMN + 1], *pData = (uchar*)Data;

  for (int i = 0; i < Size; i++)
  {
    if (i % DEFAULT_DUMP_COLUMN == 0)
    {
      if (i != 0)
      {
        printf("  %s\n", Buffer);
      }

      printf("  %04x ", i);
    }

    if (i % DEFAULT_DUMP_COLUMN == 8) printf(" ");

    printf(" %02x", pData[i]);

    if (pData[i] < 0x20 || pData[i] > 0x7E)
    {
      Buffer[i % DEFAULT_DUMP_COLUMN] = '.';
    }
    else
    {
      Buffer[i % DEFAULT_DUMP_COLUMN] = pData[i];
    }

    Buffer[(i % DEFAULT_DUMP_COLUMN) + 1] = '\0';
  }

  while (i % DEFAULT_DUMP_COLUMN != 0)
  {
    printf("   ");
    i++;
  }

  printf("  %s\n", Buffer);
}

/* --- Group : String Formatting --- */

int vuapi GetFormatLengthVLA(const std::string Format, va_list args)
{
  int N = -1;

  if (InitMiscRoutine() != VU_OK)
  {
    return N;
  }

  #ifdef _MSC_VER
  N = _vscprintf(Format.c_str(), args) + 1;
  #else
  N = pfn_vscprintf(Format.c_str(), args) + 1;
  #endif

  return N;
}

int vuapi GetFormatLengthVLW(const std::wstring Format, va_list args)
{
  int N = -1;

  if (InitMiscRoutine() != VU_OK)
  {
    return N;
  }

  #ifdef _MSC_VER
  N = _vscwprintf(Format.c_str(), args) + 1;
  #else
  N = pfn_vscwprintf(Format.c_str(), args) + 1;
  #endif

  return N;
}

int vuapi GetFormatLengthA(const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto N = GetFormatLengthVLA(Format, args);

  va_end(args);

  return N;
}

int vuapi GetFormatLengthW(const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto N = GetFormatLengthVLW(Format, args);

  va_end(args);

  return N;
}

std::string vuapi FormatVLA(const std::string Format, va_list args)
{
  std::string s;
  s.clear();

  auto N = GetFormatLengthVLA(Format, args);
  if (N <= 0)
  {
    return s;
  }

  std::unique_ptr<char[]> p(new char [N]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), N);

  #ifdef _MSC_VER
  vsnprintf(p.get(), N, Format.c_str(), args);
  #else
  pfn_vsnprintf(p.get(), N, Format.c_str(), args);
  #endif

  s.assign(p.get());

  return s;
}

std::wstring vuapi FormatVLW(const std::wstring Format, va_list args)
{
  std::wstring s;
  s.clear();

  auto N = GetFormatLengthVLW(Format, args);
  if (N <= 0)
  {
    return s;
  }

  std::unique_ptr<wchar[]> p(new wchar [N]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), 2*N);

  #ifdef _MSC_VER
  vswprintf(p.get(), Format.c_str(), args);
  #else
  pfn_vswprintf(p.get(), N, Format.c_str(), args);
  #endif

  s.assign(p.get());

  return s;
}

std::string vuapi FormatA(const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLA(Format, args);

  va_end(args);

  return s;
}

std::wstring vuapi FormatW(const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLW(Format, args);

  va_end(args);

  return s;
}

void vuapi MsgA(const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLA(Format, args);

  va_end(args);

  OutputDebugStringA(s.c_str());
}

void vuapi MsgW(const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLW(Format, args);

  va_end(args);

  OutputDebugStringW(s.c_str());
}

int vuapi BoxA(const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLA(Format, args);

  va_end(args);

  return MessageBoxA(GetActiveWindow(), s.c_str(), VU_TITLE_BOXA.c_str(), MB_ICONINFORMATION);
}

int vuapi BoxW(const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLW(Format, args);

  va_end(args);

  return MessageBoxW(GetActiveWindow(), s.c_str(), VU_TITLE_BOXW.c_str(), MB_ICONINFORMATION);
}

int vuapi BoxA(HWND hWnd, const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLA(Format, args);

  va_end(args);

  return MessageBoxA(hWnd, s.c_str(), VU_TITLE_BOXA.c_str(), MB_ICONINFORMATION);
}

int vuapi BoxW(HWND hWnd, const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLW(Format, args);

  va_end(args);

  return MessageBoxW(hWnd, s.c_str(), VU_TITLE_BOXW.c_str(), MB_ICONINFORMATION);
}

int vuapi BoxA(HWND hWnd, uint uType, const std::string& Caption, const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLA(Format, args);

  va_end(args);

  return MessageBoxA(hWnd, s.c_str(), Caption.c_str(), uType);
}

int vuapi BoxW(HWND hWnd, uint uType, const std::wstring& Caption, const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLW(Format, args);

  va_end(args);

  return MessageBoxW(hWnd, s.c_str(), Caption.c_str(), uType);
}

std::string vuapi LastErrorA(ulong ulErrorCode)
{
  if (ulErrorCode == -1)
  {
    ulErrorCode = ::GetLastError();
  }

  char* lpszErrorMessage = nullptr;

  FormatMessageA(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    NULL,
    ulErrorCode,
    LANG_USER_DEFAULT,
    (char*)&lpszErrorMessage,
    0,
    NULL
  );

  std::string s(lpszErrorMessage);
  if (s.length() != 0)
  {
    s.erase(s.length() - 1);
  }

  return s;
}

std::wstring vuapi LastErrorW(ulong ulErrorCode)
{
  if (ulErrorCode == -1)
  {
    ulErrorCode = ::GetLastError();
  }

  wchar* lpwszErrorMessage = nullptr;

  FormatMessageW(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    NULL,
    ulErrorCode,
    LANG_USER_DEFAULT,
    (wchar*)&lpwszErrorMessage,
    0,
    NULL
  );

  std::wstring s(lpwszErrorMessage);
  if (s.length() != 0)
  {
    s.erase(s.length() - 1);
  }

  return s;
}

std::string vuapi GetFormatStringForNumber(std::string TypeID)
{
  /* MinGW
    i -> int
    l -> long
    x -> long long
    j -> unsigned
    m -> unsigned long
    y -> unsigned long long
    f -> float
    d -> double
    e -> long double
  */

  std::string fs = "";

  if (TypeID == "i")
  {
    fs = "%d";
  }
  else if (TypeID == "l")
  {
    fs = "%ld";
  }
  else if (TypeID == "x")
  {
    fs = "lld";
  }
  else if (TypeID == "j")
  {
    fs = "%u";
  }
  else if (TypeID == "m")
  {
    fs = "%lu";
  }
  else if (TypeID == "y")
  {
    fs = "%llu";
  }
  else if (TypeID == "f")
  {
    fs = "%f";
  }
  else if (TypeID == "d")
  {
    fs = "%e";
  }
  else if (TypeID == "e")
  {
    fs = "%Le";
  }
  else
  {
    fs = "";
  }

  return fs;
}

template<typename T>
std::string vuapi NumberToStringA(T v)
{
  #if defined(__MINGW32__)
  std::string s = "";
  std::string tid = std::string(typeid(v).name());
  std::string fs = GetFormatStringForNumber(tid);

  int z = GetFormatLengthA(fs, v);
  if (z <= 0)
  {
    return s;
  }

  std::unique_ptr<char[]> p(new char [z]);
  memset(p.get(), 0, z*sizeof(char));
  sprintf(p.get(), fs.c_str(), v);
  s.assign(p.get());

  return s;
  #else // defined(_MSC_VER) - defined(__BCPLUSPLUS__)
  return std::to_string(v);
  #endif
}

template<typename T>
std::wstring vuapi NumberToStringW(T v)
{
  #if defined(__MINGW32__)
  std::wstring s = L"";
  std::string tid = std::string(typeid(v).name());

  std::string fs = GetFormatStringForNumber(tid);

  if (fs == "%Le")   // Does not support now
  {
    return s;
  }

  std::wstring wfs = ToStringW(fs);

  int z = GetFormatLengthW(wfs, v);
  if (z <= 0)
  {
    return s;
  }

  std::unique_ptr<wchar[]> p(new wchar [z]);
  memset(p.get(), 0, z*sizeof(wchar));
  _snwprintf(p.get(), z*sizeof(wchar), wfs.c_str(), v);
  s.assign(p.get());

  return s;
  #else // defined(_MSC_VER) - defined(__BCPLUSPLUS__)
  return std::to_wstring(v);
  #endif
}

std::string vuapi DateTimeToStringA(const time_t t)
{
  std::string s = FormatDateTimeA(t, "%H:%M:%S %d/%m/%Y");
  return s;
}

std::wstring vuapi DateTimeToStringW(const time_t t)
{
  std::wstring s = FormatDateTimeW(t, L"%H:%M:%S %d/%m/%Y");
  return s;
}

std::string vuapi FormatDateTimeA(const time_t t, const std::string Format)
{
  std::string s;
  s.clear();

  std::unique_ptr<char[]> p(new char [MAXBYTE]);
  if (p == nullptr)
  {
    return s;
  }

  tm lt = {0};

  ZeroMemory(p.get(), MAX_SIZE);

  #if defined(_MSC_VER) && (_MSC_VER > 1200) // Above VC++ 6.0
  localtime_s(&lt, &t);
  #else
  memcpy((void*)&lt, localtime(&t), sizeof(tm));
  #endif

  strftime(p.get(), MAX_SIZE, Format.c_str(), &lt);

  s.assign(p.get());

  return s;
}

std::wstring vuapi FormatDateTimeW(const time_t t, const std::wstring Format)
{
  std::wstring s;
  s.clear();

  std::unique_ptr<wchar[]> p(new wchar [MAXBYTE]);
  if (p == nullptr) return s;

  tm lt = {0};

  ZeroMemory(p.get(), 2*MAXBYTE);

  #if defined(_MSC_VER) && (_MSC_VER > 1200) // Above VC++ 6.0
  localtime_s(&lt, &t);
  #else
  memcpy((void*)&lt, localtime(&t), sizeof(tm));
  #endif

  wcsftime(p.get(), 2*MAXBYTE, Format.c_str(), &lt);

  s.assign(p.get());

  return s;
}

#ifndef CHAR_BIT
#define CHAR_BIT      8         // number of bits in a char
#define SCHAR_MIN   (-128)      // minimum signed char value
#define SCHAR_MAX     127       // maximum signed char value
#define UCHAR_MAX     0xff      // maximum unsigned char value
#endif

eEncodingType vuapi DetermineEncodingType(const void* Data, const size_t size)
{
  if (Data == nullptr || size == 0) return eEncodingType::ET_UTF8; /* Default fo empty file */

  auto p = (uchar*)Data;

  if (size > 0)
  {
    if (size == 1)   /* UTF-8 */
    {
      if (p[0] >= SCHAR_MIN && p[0] <= SCHAR_MAX) return eEncodingType::ET_UTF8_BOM;
    }
    else
    {
      {
        /* UTF-8 BOM */
        if ((size >= 3) && (p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF)) return eEncodingType::ET_UTF8_BOM;
      }
      {
        /* UTF16 */
        if ((size >= 4) &&
           ((p[0] >= SCHAR_MIN && p[0] <= SCHAR_MAX) && (p[1] == 0x00)) &&
           ((p[2] >= SCHAR_MIN && p[0] <= SCHAR_MAX) && (p[3] == 0x00)) &&
           (true)
        ) return eEncodingType::ET_UTF16_LE;
      }
      {
        /* UTF16 BOM */
        if (p[0] == 0xFF && p[1] == 0xFE) return eEncodingType::ET_UTF16_LE_BOM;
        if (p[0] == 0xFE && p[1] == 0xFF) return eEncodingType::ET_UTF16_BE_BOM;
      }
      {
        /* UTF-8 */
        if ((p[0] >= SCHAR_MIN && p[0] <= SCHAR_MAX) && (p[1] >= SCHAR_MIN && p[1] <= SCHAR_MAX))
        {
          return eEncodingType::ET_UTF8;
        }
      }
    }
  }

  return eEncodingType::ET_UNKNOWN;
}

/* ------------------------------------------------ String Working ------------------------------------------------- */

std::string vuapi LowerStringA(const std::string& String)
{
  std::string s(String);
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

std::wstring vuapi LowerStringW(const std::wstring& String)
{
  std::wstring s(String);
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

std::string vuapi UpperStringA(const std::string& String)
{
  std::string s(String);
  std::transform(s.begin(), s.end(), s.begin(), toupper);
  return s;
}

std::wstring vuapi UpperStringW(const std::wstring& String)
{
  std::wstring s(String);
  std::transform(s.begin(), s.end(), s.begin(), toupper);
  return s;
}

std::string vuapi ToStringA(const std::wstring& String)
{
  std::string s;
  s.clear();

  if (String.empty())
  {
    return s;
  }

  int N = (int)String.length() + 1;

  std::unique_ptr<char[]> p(new char [N]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), N);

  WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, String.c_str(), -1, p.get(), N, NULL, NULL);

  s.assign(p.get());

  return s;
}

std::wstring vuapi ToStringW(const std::string& String)
{
  std::wstring s;
  s.clear();

  if (String.empty())
  {
    return s;
  }

  int N = (int)String.length() + 1;

  std::unique_ptr<wchar[]> p(new wchar [N]);
  if (p == nullptr)
  {
    s.clear();
  }

  ZeroMemory(p.get(), 2*N);

  MultiByteToWideChar(CP_ACP, 0, String.c_str(), -1, p.get(), 2*N);

  s.assign(p.get());

  return s;
}

std::vector<std::string> vuapi SplitStringA(const std::string& String, const std::string& Seperate)
{
  std::vector<std::string> l;
  l.clear();

  if (String.empty())
  {
    return l;
  }

  std::string s(String), sep(Seperate), sub;
  ulongptr start, end;

  start = 0;
  end = s.find(sep);
  while (end != std::string::npos)
  {
    sub = s.substr(start, end - start);
    l.push_back(std::string(sub.c_str()));
    start = end + sep.length();
    end = s.find(sep, start);
  }

  sub = s.substr(start, end);
  l.push_back(std::string(sub.c_str()));

  return l;
}

std::vector<std::wstring> vuapi SplitStringW(const std::wstring& String, const std::wstring& Seperate)
{
  std::vector<std::wstring> l;
  l.clear();

  if (String.empty())
  {
    return l;
  }

  std::wstring s(String), sep(Seperate), sub;
  ulongptr start, end;

  start = 0;
  end = s.find(sep);
  while (end != std::wstring::npos)
  {
    sub = s.substr(start, end - start);
    l.push_back(std::wstring(sub.c_str()));
    start = end + sep.length();
    end = s.find(sep, start);
  }

  sub = s.substr(start, end);
  l.push_back(std::wstring(sub.c_str()));

  return l;
}

std::vector<std::string> vuapi MultiStringToListA(const char* lpcszMultiString)
{
  std::vector<std::string> l;
  l.clear();

  while (*lpcszMultiString)
  {
    l.push_back(std::string(lpcszMultiString));
    lpcszMultiString += (lstrlenA(lpcszMultiString) + 1);
  }

  return l;
}

std::vector<std::wstring> vuapi MultiStringToListW(const wchar* lpcwszMultiString)
{
  std::vector<std::wstring> l;
  l.clear();

  while (*lpcwszMultiString)
  {
    l.push_back(std::wstring(lpcwszMultiString));
    lpcwszMultiString += (lstrlenW(lpcwszMultiString) + 1);
  }

  return l;
}

std::unique_ptr<char[]> vuapi ListToMultiStringA(const std::vector<std::string>& StringList)
{
  size_t ulLength = 0;
  for (std::string e : StringList)
  {
    ulLength += (e.length() + 1);
  }

  ulLength += 1; // End of multi-string.

  std::unique_ptr<char[]> p(new char [ulLength]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), ulLength);

  size_t iCrLength = 0;
  char * pP = p.get();
  for (std::string e : StringList)
  {
    iCrLength = e.length() + 1;
    lstrcpynA(pP, e.c_str(), (int)iCrLength);
    pP += iCrLength;
  }

  return p;
}

std::unique_ptr<wchar[]> vuapi ListToMultiStringW(const std::vector<std::wstring>& StringList)
{
  size_t ulLength = 0;
  for (std::wstring e : StringList)
  {
    ulLength += (e.length() + 1);
  }

  ulLength += 1; // End of multi-string.

  std::unique_ptr<wchar[]> p(new wchar [ulLength]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), 2*ulLength);

  size_t iCrLength = 0;
  wchar * pP = p.get();
  for (std::wstring e : StringList)
  {
    iCrLength = e.length() + 1;
    lstrcpynW(pP, e.c_str(), (int)iCrLength);
    pP += iCrLength;
  }

  return p;
}

std::string vuapi LoadResourceStringA(const UINT uID, HINSTANCE ModuleHandle, const std::string& ModuleName)
{
  char* ps = nullptr;
  std::string s;
  s.clear();

  if ((ModuleHandle == nullptr || ModuleHandle == INVALID_HANDLE_VALUE) && ModuleName.length() != 0)
  {
    ModuleHandle = GetModuleHandleA(ModuleName.c_str());
  }

  auto z = LoadStringA(ModuleHandle, uID, (LPSTR)&ps, 0);
  if (z > 0)
  {
    s.assign(ps, z);
  }

  return s;
}

std::wstring vuapi LoadResourceStringW(const UINT uID, HINSTANCE ModuleHandle, const std::wstring& ModuleName)
{
  wchar* ps = nullptr;
  std::wstring s;
  s.clear();

  if ((ModuleHandle == nullptr || ModuleHandle == INVALID_HANDLE_VALUE) && ModuleName.length() != 0)
  {
    ModuleHandle = GetModuleHandleW(ModuleName.c_str());
  }

  auto z = LoadStringW(ModuleHandle, uID, (LPWSTR)&ps, 0);
  if (z > 0)
  {
    s.assign(ps, z);
  }

  return s;
}

std::string vuapi TrimStringA(const std::string& String, const eTrimType& TrimType, const std::string& TrimChars)
{
  auto s = String;

  switch (TrimType)
  {
  case eTrimType::TS_LEFT:
    s.erase(0, s.find_first_not_of(TrimChars));
    break;
  case eTrimType::TS_RIGHT:
    s.erase(s.find_last_not_of(TrimChars) + 1);
    break;
  case eTrimType::TS_BOTH:
    s.erase(0, s.find_first_not_of(TrimChars));
    s.erase(s.find_last_not_of(TrimChars) + 1);
    break;
  default:
    break;
  }

  return s;
}

std::wstring vuapi TrimStringW(const std::wstring& String, const eTrimType& TrimType, const std::wstring& TrimChars)
{
  auto s = String;

  switch (TrimType)
  {
  case eTrimType::TS_LEFT:
    s.erase(0, s.find_first_not_of(TrimChars));
    break;
  case eTrimType::TS_RIGHT:
    s.erase(s.find_last_not_of(TrimChars) + 1);
    break;
  case eTrimType::TS_BOTH:
    s.erase(0, s.find_first_not_of(TrimChars));
    s.erase(s.find_last_not_of(TrimChars) + 1);
    break;
  default:
    break;
  }

  return s;
}

/* ------------------------------------------------ Process Working ------------------------------------------------ */

HWND vuapi GetConsoleWindowHandle()
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

eProcessorArchitecture GetProcessorArchitecture()
{
  typedef void (WINAPI *PfnGetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);

  PfnGetNativeSystemInfo pfnGetNativeSystemInfo = (PfnGetNativeSystemInfo)CLibrary::QuickGetProcAddress(
    _T("kernel32.dll"),
    _T("GetNativeSystemInfo")
  );

  if (!pfnGetNativeSystemInfo)
  {
    return PA_UNKNOWN;
  }

  _SYSTEM_INFO si = {0};
  pfnGetNativeSystemInfo(&si);
  return static_cast<eProcessorArchitecture>(si.wProcessorArchitecture);
}

eWow64 vuapi IsWow64(ulong ulPID)
{
  typedef BOOL (WINAPI *PfnIsWow64Process)(HANDLE, PBOOL);
  PfnIsWow64Process pfnIsWow64Process = (PfnIsWow64Process)CLibrary::QuickGetProcAddress(
    _T("kernel32.dll"),
    _T("IsWow64Process")
  );
  if (pfnIsWow64Process == nullptr)
  {
    return WOW64_ERROR;
  }

  HANDLE hProcess = NULL;

  if (ulPID != (ulong)-1)
  {
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
  }
  else
  {
    hProcess = GetCurrentProcess();
  }

  BOOL bWow64 = false;
  if (!pfnIsWow64Process(hProcess, &bWow64))
  {
    return WOW64_ERROR;
  }

  CloseHandle(hProcess);

  return (bWow64 ? WOW64_YES : WOW64_NO);
}

bool vuapi RPM(HANDLE hProcess, void* lpAddress, void* lpBuffer, SIZE_T ulSize)
{
  ulong ulOldProtect = 0;
  SIZE_T ulRead = 0;

  VirtualProtectEx(hProcess, lpAddress, ulSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

  ReadProcessMemory(hProcess, (const void*)lpAddress, lpBuffer, ulSize, &ulRead);

  VirtualProtectEx(hProcess, lpAddress, ulSize, ulOldProtect, &ulOldProtect);

  if (ulRead != ulSize)
  {
    return false;
  }

  return true;
}

bool vuapi RPM(ulong ulPID, void* lpAddress, void* lpBuffer, SIZE_T ulSize)
{
  ulong ulOldProtect = 0;
  SIZE_T ulRead = 0;

  if (!ulPID)
  {
    return false;
  }

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
  if (!hProcess)
  {
    return false;
  }

  RPM(hProcess, lpAddress, lpBuffer, ulSize);

  CloseHandle(hProcess);

  if (ulRead != ulSize)
  {
    return false;
  }

  return true;
}

bool vuapi WPM(HANDLE hProcess, void* lpAddress, const void* lpcBuffer, SIZE_T ulSize)
{
  ulong ulOldProtect = 0;
  SIZE_T ulWritten = 0;

  VirtualProtectEx(hProcess, lpAddress, ulSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

  WriteProcessMemory(hProcess, lpAddress, lpcBuffer, ulSize, &ulWritten);

  VirtualProtectEx(hProcess, lpAddress, ulSize, ulOldProtect, &ulOldProtect);

  if (ulWritten != ulSize)
  {
    return false;
  }

  return true;
}

bool vuapi WPM(ulong ulPID, void* lpAddress, const void* lpcBuffer, SIZE_T ulSize)
{
  ulong ulOldProtect = 0;
  SIZE_T ulWritten = 0;

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
  if (!hProcess)
  {
    return false;
  }

  WPM(hProcess, lpAddress, lpcBuffer, ulSize);

  CloseHandle(hProcess);

  if (ulWritten != ulSize)
  {
    return false;
  }

  return true;
}

ulong vuapi GetParentPID(ulong ulChildPID)
{
  if (InitTlHelp32() != VU_OK)
  {
    return (ulong)-1;
  }

  TProcessEntry32A pe;
  HANDLE hSnapshot;
  BOOL bNext;

  hSnapshot = pfnCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
  {
    return (ulong)-1;
  }

  pe.dwSize = sizeof(TProcessEntry32A);

  bNext = pfnProcess32FirstA(hSnapshot, &pe);

  while (bNext)
  {
    if (pe.th32ProcessID == ulChildPID)
    {
      return pe.th32ParentProcessID;
    }
    bNext = pfnProcess32NextA(hSnapshot, &pe);
    pe.dwSize = sizeof(TProcessEntry32A);
  }

  return (ulong)-1;
}

std::vector<ulong> vuapi NameToPIDA(const std::string& ProcessName, ulong ulMaxProcessNumber)
{
  std::vector<ulong> l;
  l.clear();

  if (InitTlHelp32() != VU_OK)
  {
    return l;
  }

  std::unique_ptr<ulong[]> pProcesses(new ulong [ulMaxProcessNumber]);
  if (pProcesses == nullptr)
  {
    return l;
  }

  ZeroMemory(pProcesses.get(), ulMaxProcessNumber*sizeof(ulong));

  vu::ulong cbNeeded = 0;
  pfnEnumProcesses(pProcesses.get(), ulMaxProcessNumber*sizeof(vu::ulong), &cbNeeded);

  if (cbNeeded <= 0)
  {
    return l;
  }

  vu::ulong nProcesses = cbNeeded / sizeof(ulong);

  std::string s1 = LowerStringA(ProcessName), s2;

  ulong PID;
  for (vu::ulong i = 0; i < nProcesses; i++)
  {
    PID = pProcesses.get()[i];

    s2.clear();
    s2 = LowerStringA(vu::PIDToNameA(PID));
    if (s1 == s2)
    {
      l.push_back(PID);
    }
  }

  return l;
}

std::vector<ulong> vuapi NameToPIDW(const std::wstring& ProcessName, ulong ulMaxProcessNumber)
{
  std::vector<ulong> l;
  l.clear();

  if (InitTlHelp32() != VU_OK)
  {
    return l;
  }

  std::unique_ptr<ulong[]> pProcesses(new ulong [ulMaxProcessNumber]);
  if (pProcesses == nullptr)
  {
    return l;
  }

  ZeroMemory(pProcesses.get(), ulMaxProcessNumber*sizeof(ulong));

  vu::ulong cbNeeded = 0;
  pfnEnumProcesses(pProcesses.get(), ulMaxProcessNumber*sizeof(vu::ulong), &cbNeeded);

  if (cbNeeded <= 0)
  {
    return l;
  }

  vu::ulong nProcesses = cbNeeded / sizeof(ulong);

  std::wstring s1 = LowerStringW(ProcessName), s2;

  ulong PID;
  for (vu::ulong i = 0; i < nProcesses; i++)
  {
    PID = pProcesses.get()[i];

    s2.clear();
    s2 = LowerStringW(vu::PIDToNameW(PID));
    if (s1 == s2)
    {
      l.push_back(PID);
    }
  }

  return l;
}

std::string vuapi PIDToNameA(ulong ulPID)
{
  std::string s;
  s.clear();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
  if (!hProcess)
  {
    return s;
  }

  std::unique_ptr<char[]> szProcessPath(new char [MAXPATH]);
  ZeroMemory(szProcessPath.get(), MAXPATH);

  ulong ulPathLength = MAXPATH;

  BOOL ret = pfnQueryFullProcessImageNameA(hProcess, 0, szProcessPath.get(), &ulPathLength);

  CloseHandle(hProcess);

  if (ret == 0)
  {
    return s;
  }

  s.assign(szProcessPath.get());

  s = ExtractFileNameA(s);

  return s;
}

std::wstring vuapi PIDToNameW(ulong ulPID)
{
  std::wstring s;
  s.clear();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
  if (!hProcess)
  {
    return s;
  }

  std::unique_ptr<wchar[]> wszProcessPath(new wchar [MAXPATH]);
  ZeroMemory(wszProcessPath.get(), 2*MAXBYTE);

  ulong ulPathLength = 2*MAXPATH;

  BOOL ret = pfnQueryFullProcessImageNameW(hProcess, 0, wszProcessPath.get(), &ulPathLength);

  CloseHandle(hProcess);

  if (ret == 0)
  {
    return s;
  }

  s.assign(wszProcessPath.get());

  s = ExtractFileNameW(s);

  return s;
}

HMODULE vuapi Remote32GetModuleHandleA(const ulong ulPID, const std::string& ModuleName)
{
  HMODULE result = NULL;

  assert(0);

  return result;
}

HMODULE vuapi Remote32GetModuleHandleW(const ulong ulPID, const std::wstring& ModuleName)
{
  auto moduleName = ToStringA(ModuleName);
  return Remote32GetModuleHandleA(ulPID, moduleName);
}

HMODULE vuapi Remote64GetModuleHandleA(const ulong ulPID, const std::string& ModuleName)
{
  HMODULE result = NULL;

  auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ulPID);
  if (hProcess == 0 || hProcess == INVALID_HANDLE_VALUE)
  {
    return result;
  }

  HMODULE hModules[MAX_NMODULES] = {0};
  ulong cbNeeded = 0;
  pfnEnumProcessModulesEx(hProcess, hModules, sizeof(hModules), &cbNeeded, LIST_MODULES_ALL);

  ulong nModules = cbNeeded / sizeof(HMODULE);
  if (nModules == 0)
  {
    return result;
  }

  auto targetName = LowerStringA(ModuleName);
  targetName = TrimStringA(targetName);

  std::string iterName("");

  char moduleName[MAX_PATH] = {0};
  for (ulong i = 0; i < nModules; i++)
  {
    pfnGetModuleBaseNameA(hProcess, hModules[i], moduleName, sizeof(ModuleName));
    iterName = LowerStringA(moduleName);
    if (iterName == targetName)
    {
      result = hModules[i];
      break;
    }
  }

  SetLastError(ERROR_SUCCESS);

  CloseHandle(hProcess);

  return result;
}

HMODULE vuapi Remote64GetModuleHandleW(const ulong ulPID, const std::wstring& ModuleName)
{
  auto moduleName = ToStringA(ModuleName);
  return Remote64GetModuleHandleA(ulPID, moduleName);
}

HMODULE vuapi RemoteGetModuleHandleA(ulong ulPID, const std::string& ModuleName)
{
  HMODULE result = (HMODULE)-1;

  if (InitTlHelp32() != VU_OK)
  {
    return (HMODULE)-1;
  }

  bool bIs32Process = false;

  if (GetProcessorArchitecture() == eProcessorArchitecture::PA_X64)   // 64-bit arch
  {
    if (IsWow64(ulPID))   // 32-bit process
    {
      bIs32Process = true;
    }
    else   // 64-bit process
    {
      bIs32Process = false;
    }
  }
  else   // 32-bit arch
  {
    bIs32Process = true; // 32-bit process
  }

  #ifdef _WIN64 // 64-bit arch
  if (bIs32Process)   // 32-bit process
  {
    result = Remote64GetModuleHandleA(ulPID, ModuleName); // assert(0 && "64 -> 32");
  }
  else   // 64-bit process
  {
    result = Remote64GetModuleHandleA(ulPID, ModuleName); // assert(0 && "64 -> 64");
  }
  #else // 32-bit arch
  if (bIs32Process)   // 32-bit process
  {
    result = Remote32GetModuleHandleA(ulPID, ModuleName); // assert(0 && "32 -> 32");
  }
  else   // 64-bit process
  {
    result = Remote32GetModuleHandleA(ulPID, ModuleName); // assert(0 && "32 -> 64");
  }
  #endif // _WIN64

  return result;
}

HMODULE vuapi RemoteGetModuleHandleW(const ulong ulPID, const std::wstring& ModuleName)
{
  HMODULE result = (HMODULE)-1;

  if (InitTlHelp32() != VU_OK)
  {
    return (HMODULE)-1;
  }

  bool bIs32Process = false;

  if (GetProcessorArchitecture() == eProcessorArchitecture::PA_X64)   // 64-bit arch
  {
    if (IsWow64(ulPID))   // 32-bit process
    {
      bIs32Process = true;
    }
    else   // 64-bit process
    {
      bIs32Process = false;
    }
  }
  else   // 32-bit arch
  {
    bIs32Process = true; // 32-bit process
  }

  #ifdef _WIN64 // 64-bit arch
  if (bIs32Process)   // 32-bit process
  {
    result = Remote64GetModuleHandleW(ulPID, ModuleName); // assert(0 && "64 -> 32");
  }
  else   // 64-bit process
  {
    result = Remote64GetModuleHandleW(ulPID, ModuleName); // assert(0 && "64 -> 64");
  }
  #else // 32-bit arch
  if (bIs32Process)   // 32-bit process
  {
    result = Remote32GetModuleHandleW(ulPID, ModuleName); // assert(0 && "32 -> 32");
  }
  else   // 64-bit process
  {
    result = Remote32GetModuleHandleW(ulPID, ModuleName); // assert(0 && "32 -> 64");
  }
  #endif // _WIN64

  return result;
}

/* -------------------------------------------- File/Directory Working --------------------------------------------- */

bool vuapi IsDirectoryExistsA(const std::string& Directory)
{
  if (GetFileAttributesA(Directory.c_str()) == INVALID_FILE_ATTRIBUTES)
  {
    return false;
  }

  return true;
}

bool vuapi IsDirectoryExistsW(const std::wstring& Directory)
{
  if (GetFileAttributesW(Directory.c_str()) == INVALID_FILE_ATTRIBUTES)
  {
    return false;
  }

  return true;
}

std::string vuapi FileTypeA(const std::string& FilePath)
{
  SHFILEINFOA SHINFO = {0};
  std::unique_ptr<char[]> szFileType(new char [MAXBYTE]);
  std::string s;
  s.clear();

  if (SHGetFileInfoA(FilePath.c_str(), 0, &SHINFO, sizeof(SHFILEINFOA), SHGFI_TYPENAME) != 0)
  {
    ZeroMemory(szFileType.get(), MAXBYTE);
    s = SHINFO.szTypeName;
  }

  return s;
}

std::wstring vuapi FileTypeW(const std::wstring& FilePath)
{
  SHFILEINFOW SHINFO = {0};
  std::unique_ptr<wchar[]> szFileType(new wchar [MAXBYTE]);
  std::wstring s;
  s.clear();

  if (SHGetFileInfoW(FilePath.c_str(), 0, &SHINFO, sizeof(SHFILEINFOA), SHGFI_TYPENAME) != 0)
  {
    ZeroMemory(szFileType.get(), 2*MAXBYTE);
    s = SHINFO.szTypeName;
  }

  return s;
}

bool vuapi IsFileExistsA(const std::string& FilePath)
{
  bool bResult = false;
  WIN32_FIND_DATAA wfd = {0};

  HANDLE hFile = FindFirstFileA(FilePath.c_str(), &wfd);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    bResult = true;
    FindClose(hFile);
  }

  return bResult;
}

bool vuapi IsFileExistsW(const std::wstring& FilePath)
{
  bool bResult = false;
  WIN32_FIND_DATAW wfd = {0};

  HANDLE hFile = FindFirstFileW(FilePath.c_str(), &wfd);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    bResult = true;
    FindClose(hFile);
  }

  return bResult;
}

std::string vuapi ExtractFilePathA(const std::string& FilePath, bool bIncludeSlash)
{
  std::string filePath;
  filePath.clear();

  size_t slashPos = FilePath.rfind('\\');
  if (slashPos != std::string::npos)
  {
    filePath = FilePath.substr(0, slashPos + (bIncludeSlash ? 1 : 0));
  }

  return filePath;
}

std::wstring vuapi ExtractFilePathW(const std::wstring& FilePath, bool bIncludeSlash)
{
  std::wstring filePath;
  filePath.clear();

  size_t slashPos = FilePath.rfind(L'\\');
  if (slashPos != std::string::npos)
  {
    filePath = FilePath.substr(0, slashPos + (bIncludeSlash ? 1 : 0));
  }

  return filePath;
}

std::string vuapi ExtractFileNameA(const std::string& FilePath, bool bIncludeExtension)
{
  std::string fileName;
  fileName.clear();

  size_t slashPos = FilePath.rfind('\\');
  if (slashPos != std::string::npos)
  {
    fileName = FilePath.substr(slashPos + 1);
  }

  if (!bIncludeExtension)
  {
    size_t dotPos = fileName.rfind('.');
    if (dotPos != std::string::npos)
    {
      fileName = fileName.substr(0, dotPos);
    }
  }

  return fileName;
}

std::wstring vuapi ExtractFileNameW(const std::wstring& FilePath, bool bIncludeExtension)
{
  std::wstring fileName;
  fileName.clear();

  size_t slashPos = FilePath.rfind(L'\\');
  if (slashPos != std::string::npos)
  {
    fileName = FilePath.substr(slashPos + 1);
  }

  if (!bIncludeExtension)
  {
    size_t dotPos = fileName.rfind(L'.');
    if (dotPos != std::string::npos)
    {
      fileName = fileName.substr(0, dotPos);
    }
  }

  return fileName;
}

std::string vuapi GetCurrentFilePathA()
{
  std::unique_ptr<char[]> p(new char [MAXPATH]);

  ZeroMemory(p.get(), MAXPATH);

  HMODULE hModule = GetModuleHandleA(NULL);
  GetModuleFileNameA(hModule, p.get(), MAXPATH);
  FreeLibrary(hModule);

  std::string s(p.get());

  return s;
}

std::wstring vuapi GetCurrentFilePathW()
{
  std::unique_ptr<wchar[]> p(new wchar [MAXPATH]);

  ZeroMemory(p.get(), 2*MAXPATH);

  HMODULE hModule = GetModuleHandleW(NULL);
  GetModuleFileNameW(hModule, p.get(), 2*MAXPATH);
  FreeLibrary(hModule);

  std::wstring s(p.get());

  return s;
}

std::string vuapi GetCurrentDirectoryA(bool bIncludeSlash)
{
  return ExtractFilePathA(GetCurrentFilePathA(), bIncludeSlash);
}

std::wstring vuapi GetCurrentDirectoryW(bool bIncludeSlash)
{
  return ExtractFilePathW(GetCurrentFilePathW(), bIncludeSlash);
}

/****************************************************** CLASSES ******************************************************/

/* ---------------------------------------------------- GUID ----------------------------------------------------- */

#ifdef VU_GUID_ENABLED

#define NONE

#ifdef __MINGW32__
typedef unsigned char __RPC_FAR * RPC_CSTR;
#if defined(RPC_USE_NATIVE_WCHAR) && defined(_NATIVE_WCHAR_T_DEFINED)
typedef wchar_t __RPC_FAR * RPC_WSTR;
typedef const wchar_t * RPC_CWSTR;
#else
typedef unsigned short __RPC_FAR * RPC_WSTR;
typedef const unsigned short * RPC_CWSTR;
#endif
#endif

const sGUID& sGUID::operator=(const sGUID &right) const
{
  memcpy((void*)this, (const void*)&right, sizeof(*this));
  return *this;
}

bool sGUID::operator == (const sGUID &right) const
{
  return (memcmp(this, &right, sizeof(*this)) == 0);
}

bool sGUID::operator != (const sGUID &right) const
{
  return !(*this == right);
}

#define CAST_GUID(_object, _const) reinterpret_cast<_const ::UUID *> ( _object )

CGUIDX::CGUIDX(bool create)
{
  ZeroMemory(&m_GUID, sizeof(m_GUID));
  if (create)
  {
    Create();
  }
}

CGUIDX::~CGUIDX() {}

bool CGUIDX::Create()
{
  return ((m_Status = UuidCreate(CAST_GUID(&m_GUID, NONE))) == RPC_S_OK);
}

const sGUID& CGUIDX::GUID() const
{
  return m_GUID;
}

void CGUIDX::GUID(const sGUID& guid)
{
  m_GUID = guid;
}

const CGUIDX& CGUIDX::operator = (const CGUIDX &right)
{
  m_GUID = right.m_GUID;
  return *this;
}

bool CGUIDX::operator == (const CGUIDX &right) const
{
  return m_GUID == right.m_GUID;
}

bool CGUIDX::operator != (const CGUIDX &right) const
{
  return m_GUID != right.m_GUID;
}

void CGUIDA::Parse(const std::string& guid)
{
  m_GUID = CGUIDA::ToGUID(guid);
}

std::string CGUIDA::AsString() const
{
  return CGUIDA::ToString(m_GUID);
}

const std::string CGUIDA::ToString(const sGUID& guid)
{
  std::string result("");

  RPC_CSTR rpcUUID;
  #if defined(__MINGW32__)
  auto pUUID = (::UUID*)(&guid);
  if (UuidToStringA(pUUID, &rpcUUID) != RPC_S_OK)
  #else
  if (UuidToStringA(CAST_GUID(&guid, const), &rpcUUID) != RPC_S_OK)
  #endif
  {
    return result;
  }

  result = LPCSTR(rpcUUID);

  RpcStringFreeA(&rpcUUID);

  return result;
}

const sGUID CGUIDA::ToGUID(const std::string& guid)
{
  sGUID result = {0};

  auto rpcUUID = RPC_CSTR(guid.c_str());
  UuidFromStringA(rpcUUID, CAST_GUID(&result, NONE));

  return result;
}

void CGUIDW::Parse(const std::wstring& guid)
{
  m_GUID = CGUIDW::ToGUID(guid);
}

std::wstring CGUIDW::AsString() const
{
  return CGUIDW::ToString(m_GUID);
}

const std::wstring CGUIDW::ToString(const sGUID& guid)
{
  std::wstring result(L"");

  RPC_WSTR rpcUUID;
  #if defined(__MINGW32__)
  auto pUUID = (::UUID*)(&guid);
  if (UuidToStringW(pUUID, &rpcUUID) != RPC_S_OK)
  #else
  if (UuidToStringW(CAST_GUID(&guid, const), &rpcUUID) != RPC_S_OK)
  #endif
  {
    return result;
  }

  result = LPCWSTR(rpcUUID);

  RpcStringFreeW(&rpcUUID);

  return result;
}

const sGUID CGUIDW::ToGUID(const std::wstring& guid)
{
  sGUID result = {0};

  auto rpcUUID = RPC_WSTR(guid.c_str());
  UuidFromStringW(rpcUUID, CAST_GUID(&result, NONE));

  return result;
}

#endif // VU_GUID_ENABLED

/* ---------------------------------------------------- Binary ----------------------------------------------------- */

CBinary::CBinary() : m_UsedSize(0), m_Size(0), m_pData(0) {}

CBinary::CBinary(const CBinary &right) : m_UsedSize(0), m_Size(0), m_pData(0)
{
  SetpData(right.m_pData, right.m_Size);
}

CBinary::CBinary(const void* pData, ulong ulSize) : m_UsedSize(0), m_Size(0), m_pData(0)
{
  SetpData(pData, ulSize);
}

CBinary::CBinary(ulong ulSize) : m_UsedSize(0), m_Size(0), m_pData(0)
{
  AdjustSize(ulSize);
}

CBinary::~CBinary()
{
  if (m_pData != nullptr)
  {
    delete[] (uchar*)m_pData;
  }
}

const CBinary& CBinary::operator=(const CBinary& right)
{
  if(this != &right)
  {
    SetpData(right.m_pData, right.m_Size);
  }

  return *this;
}

bool CBinary::operator==(const CBinary& right) const
{
  if(m_Size != right.m_Size)
  {
    return false;
  }

  return (memcmp(m_pData, right.m_pData, m_Size) == 0);
}

bool CBinary::operator!=(const CBinary& right) const
{
  return (!(*this == right));
}

const ulong CBinary::GetSize() const
{
  return m_Size;
}

void CBinary::SetUsedSize(ulong ulUsedSize)
{
  if (ulUsedSize <= m_Size)
  {
    m_UsedSize = ulUsedSize;
  }
}

const ulong CBinary::GetUsedSize() const
{
  return m_UsedSize;
}

void* CBinary::GetpData()
{
  return m_pData;
}

const void* CBinary::GetpData() const
{
  return m_pData;
}

void CBinary::SetpData(const void* pData, ulong ulSize)
{
  AdjustSize(ulSize);
  memcpy(m_pData, pData, ulSize);
  m_Size = ulSize;
  m_UsedSize = ulSize;
}

void CBinary::AdjustSize(ulong ulSize)
{
  if(ulSize > m_Size)
  {
    if(m_pData != nullptr)
    {
      delete[] (uchar*)m_pData;
    }

    m_pData = new uchar[ulSize];
    if (m_pData != nullptr)
    {
      m_Size = ulSize;
      m_UsedSize = ulSize;
      memset(m_pData, 0, m_Size*sizeof(uchar));
    }
  }
}

/* ---------------------------------------------------- Library ---------------------------------------------------- */

CLibraryA::CLibraryA()
{
  m_ModuleName.clear();
  m_ProcName.clear();

  m_LastErrorCode = ERROR_SUCCESS;
}

CLibraryA::CLibraryA(const std::string& ModuleName)
{
  m_ModuleName = ModuleName;
  m_ProcName.clear();

  m_LastErrorCode = ERROR_SUCCESS;
}

CLibraryA::CLibraryA(const std::string& ModuleName, const std::string& ProcName)
{
  m_ModuleName = ModuleName;
  m_ProcName   = ProcName;

  m_LastErrorCode = ERROR_SUCCESS;
}

CLibraryA::~CLibraryA() {}

bool vuapi CLibraryA::IsLibraryAvailable()
{
  if (!m_ModuleName.empty())
  {
    HMODULE hmod = LoadLibraryA(m_ModuleName.c_str());

    m_LastErrorCode = GetLastError();

    if (hmod != NULL)
    {
      FreeLibrary(hmod);
      return true;
    }

    return false;
  }
  else
  {
    return false;
  }
}

void* vuapi CLibraryA::GetProcAddress()
{
  if (!m_ModuleName.empty() && !m_ProcName.empty())
  {
    return this->GetProcAddress(m_ModuleName, m_ProcName);
  }

  return nullptr;
}

void* vuapi CLibraryA::GetProcAddress(const std::string& ProcName)
{
  if (!m_ModuleName.empty() && !ProcName.empty())
  {
    return this->GetProcAddress(m_ModuleName, ProcName);
  }

  return nullptr;
}

void* vuapi CLibraryA::GetProcAddress(const std::string& ModuleName, const std::string& ProcName)
{
  HMODULE hmod = nullptr;

  if (!ModuleName.empty()) hmod = LoadLibraryA(ModuleName.c_str());

  void* p = nullptr;
  if (hmod && !ProcName.empty())
  {
    p = (void*)::GetProcAddress(hmod, ProcName.c_str());
    m_LastErrorCode = GetLastError();
  }

  FreeLibrary(hmod);

  return p;
}

void* vuapi CLibraryA::QuickGetProcAddress(const std::string& ModuleName, const std::string& ProcName)
{
  HMODULE hmod = nullptr;

  if (!ModuleName.empty()) hmod = LoadLibraryA(ModuleName.c_str());

  void* p = nullptr;
  if (hmod && !ProcName.empty())
  {
    p = (void*)::GetProcAddress(hmod, ProcName.c_str());
  }

  FreeLibrary(hmod);

  return p;
}

CLibraryW::CLibraryW()
{
  m_ModuleName.clear();
  m_ProcName.clear();

  m_LastErrorCode = ERROR_SUCCESS;
}

CLibraryW::CLibraryW(const std::wstring& ModuleName)
{
  m_ModuleName = ModuleName;
  m_ProcName.clear();

  m_LastErrorCode = GetLastError();
}

CLibraryW::CLibraryW(const std::wstring& ModuleName, const std::wstring& ProcName)
{
  m_ModuleName = ModuleName;
  m_ProcName   = ProcName;

  m_LastErrorCode = ERROR_SUCCESS;
}

CLibraryW::~CLibraryW() {}

bool vuapi CLibraryW::IsLibraryAvailable()
{
  if (!m_ModuleName.empty())
  {
    HMODULE hmod = LoadLibraryW(m_ModuleName.c_str());

    m_LastErrorCode = GetLastError();

    if (hmod != nullptr)
    {
      FreeLibrary(hmod);
      return true;
    }

    return false;
  }
  else
  {
    return false;
  }
}

void* vuapi CLibraryW::GetProcAddress()
{
  if (!m_ModuleName.empty() && !m_ProcName.empty())
  {
    return this->GetProcAddress(m_ModuleName, m_ProcName);
  }

  return nullptr;
}

void* vuapi CLibraryW::GetProcAddress(const std::wstring& ProcName)
{
  if (!m_ModuleName.empty() && !ProcName.empty())
  {
    return this->GetProcAddress(m_ModuleName, ProcName);
  }

  return nullptr;
}

void* vuapi CLibraryW::GetProcAddress(const std::wstring& ModuleName, const std::wstring& ProcName)
{
  HMODULE hmod = nullptr;

  if (!ModuleName.empty()) hmod = LoadLibraryW(ModuleName.c_str());

  void* p = nullptr;
  if (hmod && !ProcName.empty())
  {
    std::string s = ToStringA(ProcName);
    p = (void*)::GetProcAddress(hmod, s.c_str());
    m_LastErrorCode = GetLastError();
  }

  FreeLibrary(hmod);

  return p;
}

void* vuapi CLibraryW::QuickGetProcAddress(const std::wstring& ModuleName, const std::wstring& ProcName)
{
  HMODULE hmod = nullptr;

  if (!ModuleName.empty()) hmod = LoadLibraryW(ModuleName.c_str());

  void* p = nullptr;
  if (hmod && !ProcName.empty())
  {
    std::string s = ToStringA(ProcName);
    p = (void*)::GetProcAddress(hmod, s.c_str());
  }

  FreeLibrary(hmod);

  return p;
}

/* --- Group : Socket --- */

#ifdef VU_SOCKET_ENABLED

CSocket::CSocket()
{
  m_Socket = INVALID_SOCKET;

  memset((void*)&m_WSAData, 0, sizeof(m_WSAData));
  memset((void*)&m_Server, 0, sizeof(m_Server));
  memset((void*)&m_Client, 0, sizeof(m_Client));

  m_LastErrorCode = ERROR_SUCCESS;
}

CSocket::CSocket(eSocketAF socketAF, eSocketType SocketType)
{
  m_Socket = INVALID_SOCKET;

  memset((void*)&m_WSAData, 0, sizeof(m_WSAData));
  memset((void*)&m_Server, 0, sizeof(m_Server));
  memset((void*)&m_Client, 0, sizeof(m_Client));

  m_LastErrorCode = ERROR_SUCCESS;

  this->Socket(socketAF, SocketType);
}

CSocket::~CSocket() {}

bool vuapi CSocket::IsSocketValid(SOCKET socket)
{
  if (!socket || socket == INVALID_SOCKET)
  {
    return false;
  }

  return true;
}

VUResult vuapi CSocket::Socket(eSocketAF SocketAF, eSocketType SocketType, eSocketProtocol SocketProtocol)
{
  if (WSAStartup(MAKEWORD(2, 2), &m_WSAData) != 0)
  {
    m_LastErrorCode = GetLastError();
    return 1;
  }

  m_Socket = socket(SocketAF, SocketType, SocketProtocol);
  if (!this->IsSocketValid(m_Socket))
  {
    return 2;
  }

  m_Server.sin_family = SocketAF;

  return VU_OK;
}

VUResult vuapi CSocket::Bind(const TAccessPoint& AccessPoint)
{
  return this->Bind(AccessPoint.Host, AccessPoint.Port);
}

VUResult vuapi CSocket::Bind(const std::string& Address, unsigned short usPort)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  std::string IP;

  if (this->IsHostName(Address) == true)
  {
    IP = this->GetHostByName(Address);
  }
  else
  {
    IP = Address;
  }

  if (IP.empty())
  {
    return 2;
  }

  m_Server.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
  m_Server.sin_port = htons(usPort);

  if (bind(m_Socket, (const struct sockaddr*)&m_Server, sizeof(m_Server)) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    return 3;
  }

  return VU_OK;
}

VUResult vuapi CSocket::Listen(int iMaxConnection)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  int result = listen(m_Socket, iMaxConnection);

  m_LastErrorCode = GetLastError();

  return (result == SOCKET_ERROR ? 2 : VU_OK);
}

VUResult vuapi CSocket::Accept(TSocketInfomation& SocketInformation)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  memset((void*)&SocketInformation, 0, sizeof(SocketInformation));

  int size = sizeof(SocketInformation.sai);

  SocketInformation.s = accept(m_Socket, (struct sockaddr*)&SocketInformation.sai, &size);

  m_LastErrorCode = GetLastError();

  if (!this->IsSocketValid(SocketInformation.s))
  {
    return 2;
  }

  this->BytesToIP(SocketInformation);

  return VU_OK;
}

VUResult vuapi CSocket::Connect(const TAccessPoint& AccessPoint)
{
  return this->Connect(AccessPoint.Host, AccessPoint.Port);
}

VUResult vuapi CSocket::Connect(const std::string& Address, unsigned short usPort)
{
  std::string IP;

  if (this->IsHostName(Address) == true)
  {
    IP = this->GetHostByName(Address);
  }
  else
  {
    IP = Address;
  }

  if (IP.empty())
  {
    return 1;
  }

  m_Server.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
  m_Server.sin_port = htons(usPort);

  if (connect(m_Socket, (const struct sockaddr*)&m_Server, sizeof(m_Server)) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    this->Close();
    return 2;
  }

  return VU_OK;
}

IResult vuapi CSocket::Send(const char* lpData, int iLength, eSocketMessage SocketMessage)
{
  return this->Send(m_Socket, lpData, iLength, SocketMessage);
}

IResult vuapi CSocket::Send(const CBinary& Data, eSocketMessage SocketMessage)
{
  return this->Send(m_Socket, (const char*)Data.GetpData(), Data.GetUsedSize(), SocketMessage);
}

IResult vuapi CSocket::Send(SOCKET socket, const char* lpData, int iLength, eSocketMessage SocketMessage)
{
  if (!this->IsSocketValid(socket))
  {
    return SOCKET_ERROR;
  }

  IResult z = send(socket, lpData, iLength, SocketMessage);
  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }

  return z;
}

IResult vuapi CSocket::Recv(char* lpData, int iLength, eSocketMessage SocketMessage)
{
  return this->Recv(m_Socket, lpData, iLength, SocketMessage);
}

IResult vuapi CSocket::Recv(CBinary& Data, eSocketMessage SocketMessage)
{
  void* p = Data.GetpData();
  auto z = this->Recv(m_Socket, (char*)p, Data.GetSize(), SocketMessage);
  if (z != SOCKET_ERROR)
  {
    Data.SetUsedSize(z);
  }

  return z;
}

IResult vuapi CSocket::Recv(SOCKET socket, char* lpData, int iLength, eSocketMessage SocketMessage)
{
  if (!this->IsSocketValid(socket))
  {
    return SOCKET_ERROR;
  }

  IResult z = recv(socket, lpData, iLength, SocketMessage);
  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }

  return z;
}

IResult vuapi CSocket::SendTo(const CBinary& Data, TSocketInfomation& SocketInformation)
{
  return this->SendTo((const char*)Data.GetpData(), Data.GetUsedSize(), SocketInformation);
}

IResult vuapi CSocket::SendTo(const char* lpData, int iLength, TSocketInfomation& SocketInformation)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return SOCKET_ERROR;
  }

  IResult z = sendto(
    m_Socket,
    lpData,
    iLength,
    0,
    (const struct sockaddr*)&SocketInformation.sai,
    sizeof(SocketInformation.sai)
  );

  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }

  return z;
}

IResult vuapi CSocket::RecvFrom(CBinary& Data, TSocketInfomation& SocketInformation)
{
  void* p = Data.GetpData();
  return this->RecvFrom((char*)p, Data.GetUsedSize(), SocketInformation);
}

IResult vuapi CSocket::RecvFrom(char* lpData, int iLength, TSocketInfomation& SocketInformation)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return SOCKET_ERROR;
  }

  int n = sizeof(SocketInformation.sai);
  IResult z = recvfrom(m_Socket, lpData, iLength, 0, (struct sockaddr *)&SocketInformation.sai, &n);
  if (z == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
  }
  else
  {
    this->BytesToIP(SocketInformation);
  }

  return z;
}

bool vuapi CSocket::Close(SOCKET socket)
{
  if (socket != 0)
  {
    if (!this->IsSocketValid(socket)) return false;
    closesocket(socket);
  }
  else if (this->IsSocketValid(m_Socket))
  {
    closesocket(m_Socket);
    m_Socket = 0;
  }

  WSACleanup();

  m_LastErrorCode = GetLastError();

  return true;
}

SOCKET vuapi CSocket::GetSocket()
{
  return m_Socket;
}

VUResult vuapi CSocket::GetOption(int iLevel, int iOptName, char* pOptVal, int* lpiLength)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  if (getsockopt(m_Socket, iLevel, iOptName, pOptVal, lpiLength) != 0)
  {
    m_LastErrorCode = GetLastError();
    return 2;
  }

  return VU_OK;
}

VUResult vuapi CSocket::SetOption(int iLevel, int iOptName, const std::string& OptVal, int iLength)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  if (OptVal.empty())
  {
    return 2;
  }

  if (setsockopt(m_Socket, iLevel, iOptName, OptVal.c_str(), iLength) != 0)
  {
    m_LastErrorCode = GetLastError();
    return 3;
  }

  return VU_OK;
}

VUResult vuapi CSocket::Shutdown(eShutdownFlag ShutdownFlag)
{
  if (!this->IsSocketValid(m_Socket))
  {
    return 1;
  }

  if (shutdown(m_Socket, (int)ShutdownFlag) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    return 2;
  }

  return VU_OK;
}

std::string vuapi CSocket::GetLocalHostName()
{
  std::string r;
  r.clear();

  std::unique_ptr<char[]> h(new char [MAXBYTE]);
  if (h == nullptr)
  {
    return r;
  }

  if (!this->IsSocketValid(m_Socket))
  {
    return r;
  }

  memset(h.get(), 0, MAXBYTE);
  if (gethostname(h.get(), MAXBYTE) == SOCKET_ERROR)
  {
    m_LastErrorCode = GetLastError();
    return r;
  }

  r.assign(h.get());

  return r;
}

std::string vuapi CSocket::GetHostByName(const std::string& Name)
{
  std::string r;
  r.clear();

  if (!this->IsSocketValid(m_Socket))
  {
    WSASetLastError(6);  // WSA_INVALID_HANDLE
    return r;
  }

  if (Name.empty())
  {
    WSASetLastError(87); // WSA_INVALID_PARAMETER
    r = VU_LOCALHOST;
    return r;
  }

  if (Name.length() >= MAXBYTE)
  {
    WSASetLastError(87); // WSA_INVALID_PARAMETER
    return r;
  }

  hostent * h = gethostbyname(Name.c_str());
  if (h == nullptr)
  {
    return r;
  }

  if (h->h_addr_list[0] == nullptr || strlen(h->h_addr_list[0]) == 0)
  {
    return r;
  }

  in_addr a = {0};
  memcpy((void*)&a, (void*)h->h_addr_list[0], sizeof(a));
  r = inet_ntoa(a);

  return r;
}

bool vuapi CSocket::IsHostName(const std::string& s)
{
  bool r = false;
  const std::string MASK = "01234567890.";

  if (s.empty())
  {
    return r;
  }

  if (s.length() >= MAXBYTE)
  {
    return r;
  }

  for (unsigned int i = 0; i < s.length(); i++)
  {
    if (strchr(MASK.c_str(), s[i]) == nullptr)
    {
      r = true;
      break;
    }
  }

  return r;
}

bool vuapi CSocket::BytesToIP(const TSocketInfomation& SocketInformation)
{
  if (sprintf(
    (char*)SocketInformation.ip,
    "%d.%d.%d.%d\0",
    SocketInformation.sai.sin_addr.S_un.S_un_b.s_b1,
    SocketInformation.sai.sin_addr.S_un.S_un_b.s_b2,
    SocketInformation.sai.sin_addr.S_un.S_un_b.s_b3,
    SocketInformation.sai.sin_addr.S_un.S_un_b.s_b4
  ) < 0) return false;
  else return true;
}

#endif // VU_SOCKET_ENABLED

/* --- Group : API Hooking --- */

ulongptr vuapi CDynHookX::JumpLength(ulongptr ulSrcAddress, ulongptr ulDestAddress, ulongptr ulInstSize)
{
  return (ulDestAddress - ulSrcAddress - ulInstSize);
}

bool vuapi CDynHookX::HandleMemoryInstruction(const HDE::tagHDE& hde, const ulong offset)
{
  ulong ulPosDisp = 0;
  TMemoryInstruction mi = {0};
  bool result = false, bFoundRelative = false;;

  // http://wiki.osdev.org/X86-64_Instruction_Encoding

  /*if (IsFlagOn(hde.flags, HDE::F_MODRM)) {  // ModR/M exists
    // ModR/M
  }
  else if (IsFlagOn(hde.flags, HDE::F_SIB)) {   // SIB exists
    // SIB
  }*/

  // Now, only ModR/M.
  if (!IsFlagOn(hde.flags, HDE::F_MODRM))
  {
    return result;
  }

  if (hde.modrm_mod == 3)   // [R/M]
  {
    return result;
  }

  switch (hde.modrm_mod)
  {
  case 0: // MOD = b00
    if ((hde.modrm_rm >= 0 && hde.modrm_rm <= 3) || // {AX, BX, CX, DX}
        (hde.modrm_rm >= 6 && hde.modrm_rm <= 11)     // {SI, DI, R8, R9, R10, R11}
       )   // [R/M]
    {
      // ...
    }
    if (hde.modrm_rm == 5 || hde.modrm_rm == 13)    // [RIP/EIP + D32] {BP, R13} *
    {
      #ifdef _WIN64
      /*ulPosDisp += 1;   // The first opcode. Always exists a byte for opcode of each instruction.
      if (IsFlagOn(hde.flags, HDE::F_PREFIX_SEG)) { // * In 64-bit the CS, SS, DS and ES segment overrides are ignored.
        / * Prefix group 2 (take 1 byte)
          0x2E: CS segment override
          0x36: SS segment override
          0x3E: DS segment override
          0x26: ES segment override
          0x64: FS segment override
          0x65: GS segment override
          0x2E: Branch not taken
          0x3E: Branch taken
        * /
        ulPosDisp += 1; // If it's being used the segment.
      }
      if (IsFlagOn(hde.flags, HDE::F_MODRM)) {
        ulPosDisp += 1; // The second opcode.
      }
      { // Others
        // ...
      }
      */

      auto ulImmFlags = 0;
      #ifdef _WIN64
      ulImmFlags = (HDE::C_IMM8 | HDE::F_IMM16 | HDE::F_IMM32 | HDE::F_IMM64);
      #else
      ulImmFlags = (HDE::C_IMM8 | HDE::F_IMM16 | HDE::F_IMM32);
      #endif

      ulPosDisp = hde.len - ((hde.flags & ulImmFlags) >> 2) - 4;

      mi.Position = ulPosDisp;
      mi.MAO.A32 = hde.disp.disp32;
      mi.MemoryAddressType = eMemoryAddressType::MAT_32; // [RIP/EIP1,2 + disp32]
      bFoundRelative = true;
      #endif
    }
    if (hde.modrm_rm == 4 || hde.modrm_rm == 12)    // [SIB] {SP, R12}
    {
      // ...
    }
    break;
  case 1: // MOD = b01
    if ((hde.modrm_rm >= 0 && hde.modrm_rm <= 3)||    // {AX, BX, CX, DX}
        (hde.modrm_rm >= 5 && hde.modrm_rm <= 11) ||  // {BP, SI, DI, R8, R9, R10, R11}
        (hde.modrm_rm >= 13 && hde.modrm_rm <= 15)    // {R13, R14, R15}
    ) // [R/M + D32]
    {
      // ...
    }
    if (hde.modrm_rm == 4 || hde.modrm_rm == 12)    // [SIB + D32] // {SP, R12}
    {
      // ...
    }
    break;
  case 2: // MOD = b10
    if ((hde.modrm_rm >= 0 && hde.modrm_rm <= 3)||    // {AX, BX, CX, DX}
        (hde.modrm_rm >= 5 && hde.modrm_rm <= 11) ||  // {BP, SI, DI, R8, R9, R10, R11}
        (hde.modrm_rm >= 13 && hde.modrm_rm <= 15)    // {R13, R14, R15}
    ) // [R/M + D8]
    {
      // ...
    }
    if (hde.modrm_rm == 4 || hde.modrm_rm == 12)    // [SIB + D8] // {SP, R12}
    {
      // ...
    }
    break;
  default: // MOD = b01[3] (Ignored)
    // [R/M]
    break;
  }

  if (bFoundRelative)
  {
    mi.Offset = offset;
    m_ListMemoryInstruction.push_back(mi);
  }

  result = true;

  return result;
}

bool vuapi CDynHookX::Attach(void* pProc, void* pHookProc, void** pOldProc)
{
  /*
    // x86
    EIP + 0 | FF 25 ?? ?? ?? ??       | JMP DWORD PTR DS:[XXXXXXXX] ; Jump to XXXXXXXX

    // x64
    RIP + 0 | FF 25 ?? ??? ?? ??      | JMP QWORD PTR DS:[RIP+6] ; Jump to [RIP + 6]
    RIP + 6 | ?? ?? ?? ?? ?? ?? ?? ?? | XXXXXXXXXXXXXXXX
  */

  TRedirect O2N = {0}, T2O = {0};
  ulong iTrampolineSize = 0;
  bool bFoundTrampolineSize = true;

  do
  {
    HDE::tagHDE hde = { 0 };

    HDE::Disasemble((const void *)((ulongptr)pProc + iTrampolineSize), &hde);
    if ((hde.flags & HDE::F_ERROR) == HDE::F_ERROR)
    {
      bFoundTrampolineSize = false;
      break;
    }
    else
    {
      this->HandleMemoryInstruction(hde, iTrampolineSize);
    }

    iTrampolineSize += hde.len;
  }
  while (iTrampolineSize < MIN_HOOK_SIZE);

  if (!bFoundTrampolineSize)
  {
    return false;
  }

  // The allocated address must be not too far with the target (Better is in image process address)
  // pOldProc is a trampoline function
  // VirtualAlloc(nullptr, iTrampolineSize + sizeof(TRedirect), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  *pOldProc = (void*)IBH::AllocateBuffer(pProc);
  if (*pOldProc == nullptr)
  {
    return false;
  }

  memcpy(*pOldProc, pProc, iTrampolineSize);

  // fix memory instruction
  if (m_ListMemoryInstruction.size() > 0)
  {
    for (auto e: m_ListMemoryInstruction)
    {
      // if (e.MemoryAddressType == eMemoryAddressType::MAT_32) {
      auto v = (ulongptr)pProc - (ulongptr)*pOldProc + (ulongptr)e.MAO.A32;
      auto p = ((ulongptr)*pOldProc + e.Offset + e.Position);
      *(ulongptr*)p = (ulongptr)v; // check again
      // }
    }
  }

  T2O.JMP = 0x25FF;
  #ifdef _WIN64
  T2O.Unknown = 0; // JMP_OPCODE_SIZE
  #else  // _WIN32
  T2O.Unknown = ((ulongptr)*pOldProc + iTrampolineSize) + sizeof(T2O.JMP) + sizeof(T2O.Unknown);
  #endif // _WIN64
  T2O.Address = ((ulongptr)pProc + iTrampolineSize);

  // Write the jump code (trampoline -> original) on the bottom of the trampoline function
  memcpy((void*)((ulongptr)*pOldProc + iTrampolineSize), (void*)&T2O, sizeof(T2O));

  ulong ulOldProtect = 0;
  VirtualProtect(pProc, iTrampolineSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

  O2N.JMP = 0x25FF;
  #ifdef _WIN64
  O2N.Unknown = 0; // JMP_OPCODE_SIZE;
  #else  // _WIN32
  O2N.Unknown = ((ulongptr)pProc + sizeof(O2N.JMP) + sizeof(O2N.Unknown));
  #endif // _WIN64
  O2N.Address = (ulongptr)pHookProc;

  // Write the jump code (original -> new) on the top of the target function
  memcpy(pProc, (void*)&O2N, sizeof(O2N));

  return true;
}

bool vuapi CDynHookX::Detach(void* pProc, void** pOldProc)
{
  if (!m_Hooked)
  {
    return false;
  }

  // fix memory instruction
  if (m_ListMemoryInstruction.size() > 0)
  {
    for (auto e: m_ListMemoryInstruction)
    {
      // if (e.MemoryAddressType == eMemoryAddressType::MAT_32) {
      auto p = ((ulongptr)*pOldProc + e.Offset + e.Position);
      auto v = (ulongptr)e.MAO.A32;
      *(ulongptr*)p = v; // check again
      // }
    }
  }

  memcpy(pProc, *pOldProc, MIN_HOOK_SIZE);

  // VirtualFree(*pOldProc, MIN_HOOK_SIZE + sizeof(TRedirect), MEM_RELEASE);
  IBH::FreeBuffer(*pOldProc);

  *pOldProc = nullptr;

  return true;
}

bool vuapi CDynHookA::APIAttach(
  const std::string& ModuleName,
  const std::string& ProcName,
  void* lpHookProc,
  void** lpOldProc
)
{
  void* lpProc = CLibraryA::QuickGetProcAddress(ModuleName, ProcName);
  if (lpProc == nullptr)
  {
    return false;
  }

  m_Hooked = this->Attach(lpProc, lpHookProc, lpOldProc);

  return m_Hooked;
}

bool vuapi CDynHookA::APIDetach(const std::string& ModuleName, const std::string& ProcName, void** lpOldProc
                                )
{
  if (!m_Hooked)
  {
    return false;
  }

  void* lpProc = CLibraryA::QuickGetProcAddress(ModuleName, ProcName);
  if (lpProc == nullptr)
  {
    return false;
  }

  return this->Detach(lpProc, lpOldProc);
}

bool vuapi CDynHookW::APIAttach(
  const std::wstring& ModuleName,
  const std::wstring& ProcName,
  void* lpHookProc,
  void** lpOldProc
)
{
  void* lpProc = CLibraryW::QuickGetProcAddress(ModuleName, ProcName);
  if (lpProc == nullptr)
  {
    return false;
  }

  m_Hooked = this->Attach(lpProc, lpHookProc, lpOldProc);

  return m_Hooked;
}

bool vuapi CDynHookW::APIDetach(const std::wstring& ModuleName, const std::wstring& ProcName, void** lpOldProc)
{
  if (!m_Hooked)
  {
    return false;
  }

  void* lpProc = CLibraryW::QuickGetProcAddress(ModuleName, ProcName);
  if (lpProc == nullptr)
  {
    return false;
  }

  return this->Detach(lpProc, lpOldProc);
}

/* --- Group : File Mapping --- */

CFileMappingA::CFileMappingA()
{
  m_HasInit = false;
  m_MapFile = false;

  m_pData = nullptr;
  m_MapHandle  = INVALID_HANDLE_VALUE;
  m_FileHandle = INVALID_HANDLE_VALUE;

  m_LastErrorCode = ERROR_SUCCESS;
}

CFileMappingA::~CFileMappingA()
{
  this->Close();
}

bool CFileMappingA::IsValidHandle(HANDLE Handle)
{
  return (Handle != nullptr && Handle != INVALID_HANDLE_VALUE);
}

VUResult vuapi CFileMappingA::Init(
  bool bMapFile,
  const std::string& FileName,
  eFileGenericFlags fgFlag,
  eFileShareFlags fsFlag,
  eFileModeFlags fmFlag,
  eFileAttributeFlags faFlag
)
{
  if ((m_MapFile = bMapFile))
  {
    if (FileName.empty())
    {
      return 1;
    }

    m_FileHandle = CreateFileA(FileName.c_str(), fgFlag, fsFlag, NULL, fmFlag, faFlag, 0);

    m_LastErrorCode = GetLastError();

    if (!this->IsValidHandle(m_FileHandle))
    {
      return 2;
    }
  }

  m_HasInit = true;

  return VU_OK;
}

VUResult vuapi CFileMappingA::Create(const std::string& MapName, ulong ulMaxSizeLow, ulong ulMaxSizeHigh)
{
  if (!m_MapFile)
  {
    if (MapName.empty())
    {
      return 1;
    }
  }

  if (!m_HasInit || (m_MapFile && !this->IsValidHandle(m_FileHandle)))
  {
    return 2;
  }

  m_MapHandle = CreateFileMappingA(
    m_FileHandle,
    NULL,
    PAGE_READWRITE,
    ulMaxSizeHigh,
    ulMaxSizeLow,
    (m_MapFile ? NULL : MapName.c_str())
  );

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 3;
  }

  return VU_OK;
}

VUResult vuapi CFileMappingA::Open(const std::string& MapName, bool bInheritHandle)
{
  if (!m_MapFile)
  {
    return 1;
  }

  if (MapName.empty())
  {
    return 2;
  }

  m_MapHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, bInheritHandle, MapName.c_str());

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 3;
  }

  return VU_OK;
}

void* vuapi CFileMappingA::View(
  ulong ulMaxFileOffsetLow,
  ulong ulMaxFileOffsetHigh,
  ulong ulNumberOfBytesToMap
)
{
  if (!this->IsValidHandle(m_MapHandle))
  {
    return nullptr;
  }

  m_pData = MapViewOfFile(
    m_MapHandle,
    FILE_MAP_ALL_ACCESS,
    ulMaxFileOffsetHigh,
    ulMaxFileOffsetLow,
    ulNumberOfBytesToMap
  );

  m_LastErrorCode = GetLastError();

  return m_pData;
}

void vuapi CFileMappingA::Close()
{
  if (m_pData != nullptr)
  {
    UnmapViewOfFile(m_pData);
    m_pData = nullptr;
  }

  if (this->IsValidHandle(m_MapHandle))
  {
    CloseHandle(m_MapHandle);
    m_MapHandle = INVALID_HANDLE_VALUE;
  }

  if (m_MapFile && this->IsValidHandle(m_FileHandle))
  {
    CloseHandle(m_FileHandle);
    m_FileHandle = INVALID_HANDLE_VALUE;
  }
}

ulong vuapi CFileMappingA::GetFileSize()
{
  if (m_MapFile && !this->IsValidHandle(m_FileHandle))
  {
    return INVALID_FILE_SIZE;
  }

  ulong result = ::GetFileSize(m_FileHandle, NULL);

  m_LastErrorCode = GetLastError();

  return result;
}

CFileMappingW::CFileMappingW()
{
  m_HasInit = false;
  m_MapFile = false;

  m_pData = nullptr;
  m_MapHandle  = INVALID_HANDLE_VALUE;
  m_FileHandle = INVALID_HANDLE_VALUE;

  m_LastErrorCode = ERROR_SUCCESS;
}

CFileMappingW::~CFileMappingW()
{
  this->Close();
}

bool CFileMappingW::IsValidHandle(HANDLE Handle)
{
  return (Handle != nullptr && Handle != INVALID_HANDLE_VALUE);
}

VUResult vuapi CFileMappingW::Init(
  bool bMapFile,
  const std::wstring FileName,
  eFileGenericFlags fgFlag,
  eFileShareFlags fsFlag,
  eFileModeFlags fmFlag,
  eFileAttributeFlags faFlag
)
{
  if ((m_MapFile = bMapFile))
  {
    if (FileName.empty())
    {
      return 1;
    }

    m_FileHandle = CreateFileW(FileName.c_str(), fgFlag, fsFlag, NULL, fmFlag, faFlag, 0);

    m_LastErrorCode = GetLastError();

    if (!this->IsValidHandle(m_FileHandle))
    {
      return 2;
    }
  }

  m_HasInit = true;

  return VU_OK;
}

VUResult vuapi CFileMappingW::Create(const std::wstring& MapName, ulong ulMaxSizeLow, ulong ulMaxSizeHigh)
{
  if (!m_MapFile)
  {
    if (MapName.empty())
    {
      return 1;
    }
  }

  if (!m_HasInit || (m_MapFile && !this->IsValidHandle(m_FileHandle)))
  {
    return 2;
  }

  m_MapHandle = CreateFileMappingW(
    m_FileHandle,
    NULL,
    PAGE_READWRITE,
    ulMaxSizeHigh,
    ulMaxSizeLow,
    (m_MapFile ? NULL : MapName.c_str())
  );

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 3;
  }

  return VU_OK;
}

VUResult vuapi CFileMappingW::Open(const std::wstring& MapName, bool bInheritHandle)
{
  if (!m_MapFile)
  {
    return 1;
  }

  if (MapName.empty())
  {
    return 2;
  }

  m_MapHandle = OpenFileMappingW(FILE_MAP_ALL_ACCESS, bInheritHandle, MapName.c_str());

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 3;
  }

  return VU_OK;
}

void* vuapi CFileMappingW::View(
  ulong ulMaxFileOffsetLow,
  ulong ulMaxFileOffsetHigh,
  ulong ulNumberOfBytesToMap
)
{
  if (!this->IsValidHandle(m_MapHandle))
  {
    return nullptr;
  }

  m_pData = MapViewOfFile(
    m_MapHandle,
    FILE_MAP_ALL_ACCESS,
    ulMaxFileOffsetHigh,
    ulMaxFileOffsetLow,
    ulNumberOfBytesToMap
  );

  m_LastErrorCode = GetLastError();

  return m_pData;
}

void vuapi CFileMappingW::Close()
{
  if (m_pData != nullptr)
  {
    UnmapViewOfFile(m_pData);
    m_pData = nullptr;
  }

  if (this->IsValidHandle(m_MapHandle))
  {
    CloseHandle(m_MapHandle);
    m_MapHandle = INVALID_HANDLE_VALUE;
  }

  if (m_MapFile && this->IsValidHandle(m_FileHandle))
  {
    CloseHandle(m_FileHandle);
    m_FileHandle = INVALID_HANDLE_VALUE;
  }
}

ulong vuapi CFileMappingW::GetFileSize()
{
  if (m_MapFile && !this->IsValidHandle(m_FileHandle))
  {
    return INVALID_FILE_SIZE;
  }

  ulong result = ::GetFileSize(m_FileHandle, NULL);

  m_LastErrorCode = GetLastError();

  return result;
}

/* --- Group : INI File --- */

CINIFileA::CINIFileA(const std::string& FilePath)
{
  m_FilePath = FilePath;
}

CINIFileA::CINIFileA(const std::string& FilePath, const std::string& Section)
{
  m_FilePath = FilePath;
  m_Section  = Section;
}

void CINIFileA::ValidFilePath()
{
  if (m_FilePath.empty())
  {
    std::string filePath = GetCurrentFilePathA();
    std::string fileDir  = ExtractFilePathA(filePath, true);
    std::string fileName = ExtractFileNameA(filePath, false);
    m_FilePath = fileDir + fileName + ".INI";
  }
}

void CINIFileA::SetCurrentFilePath(const std::string& FilePath)
{
  m_FilePath = FilePath;
}

void CINIFileA::SetCurrentSection(const std::string& Section)
{
  m_Section = Section;
}

// Long-Read

std::vector<std::string> vuapi CINIFileA::ReadSectionNames(ulong ulMaxSize)
{
  std::vector<std::string> l;
  l.clear();

  std::unique_ptr<char[]> p(new char [ulMaxSize]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), ulMaxSize);

  this->ValidFilePath();

  GetPrivateProfileSectionNamesA(p.get(), ulMaxSize, m_FilePath.c_str());

  m_LastErrorCode = GetLastError();

  for (char * s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::string(s))->c_str());
    s += lstrlenA(s);
  }

  return l;
}

std::vector<std::string> vuapi CINIFileA::ReadSection(const std::string& Section, ulong ulMaxSize)
{
  std::vector<std::string> l;
  l.clear();

  std::unique_ptr<char[]> p(new char [ulMaxSize]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), ulMaxSize);

  this->ValidFilePath();

  GetPrivateProfileSectionA(Section.c_str(), p.get(), ulMaxSize, m_FilePath.c_str());

  m_LastErrorCode = GetLastError();

  for (char * s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::string(s))->c_str());
    s += lstrlenA(s);
  }

  return l;
}

int vuapi CINIFileA::ReadInteger(const std::string& Section, const std::string& Key, int Default)
{
  this->ValidFilePath();
  uint result = GetPrivateProfileIntA(Section.c_str(), Key.c_str(), Default, m_FilePath.c_str());
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileA::ReadBool(const std::string& Section, const std::string& Key, bool Default)
{
  this->ValidFilePath();
  bool result = (GetPrivateProfileIntA(Section.c_str(), Key.c_str(), Default, m_FilePath.c_str()) == 1 ? true : false);
  m_LastErrorCode = GetLastError();
  return result;
}

float vuapi CINIFileA::ReadFloat(const std::string& Section, const std::string& Key, float Default)
{
  const std::string sDefault = NumberToStringA(Default);
  char lpszResult[MAX_SIZE];

  ZeroMemory(lpszResult, sizeof(lpszResult));

  this->ValidFilePath();

  GetPrivateProfileStringA(Section.c_str(), Key.c_str(), sDefault.c_str(), lpszResult, MAX_SIZE, m_FilePath.c_str());

  m_LastErrorCode = GetLastError();

  return (float)atof(lpszResult);
}

std::string vuapi CINIFileA::ReadString(
  const std::string& Section,
  const std::string& Key,
  const std::string& Default
)
{
  std::string s;
  s.clear();

  std::unique_ptr<char[]> p(new char [MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), MAX_SIZE);

  this->ValidFilePath();

  ulong result = GetPrivateProfileStringA(
    Section.c_str(),
    Key.c_str(),
    Default.c_str(),
    p.get(),
    MAX_SIZE,
    m_FilePath.c_str()
  );

  if (result == 0)
  {
    m_LastErrorCode = GetLastError();
    return s;
  }

  s.assign(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi CINIFileA::ReadStruct(const std::string& Section, const std::string& Key, ulong ulSize)
{
  std::unique_ptr<uchar[]> p(new uchar [ulSize]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), ulSize);

  this->ValidFilePath();

  if (GetPrivateProfileStructA(Section.c_str(), Key.c_str(), (void*)p.get(), ulSize, m_FilePath.c_str()) == 0)
  {
    m_LastErrorCode = GetLastError();
    return nullptr;
  }

  return p;
}

// Short-Read

std::vector<std::string> vuapi CINIFileA::ReadSection(ulong ulMaxSize)
{
  return this->ReadSection(m_Section, ulMaxSize);
}

int vuapi CINIFileA::ReadInteger(const std::string& Key, int Default)
{
  return this->ReadInteger(m_Section, Key, Default);
}

bool vuapi CINIFileA::ReadBool(const std::string& Key, bool Default)
{
  return this->ReadBool(m_Section, Key, Default);
}

float vuapi CINIFileA::ReadFloat(const std::string& Key, float Default)
{
  return this->ReadFloat(m_Section, Key, Default);
}

std::string vuapi CINIFileA::ReadString(const std::string& Key, const std::string& Default)
{
  return this->ReadString(m_Section, Key, Default);
}

std::unique_ptr<uchar[]> vuapi CINIFileA::ReadStruct(const std::string& Key, ulong ulSize)
{
  return this->ReadStruct(m_Section, Key, ulSize);
}

// Long-Write

bool vuapi CINIFileA::WriteInteger(const std::string& Section, const std::string& Key, int Value)
{
  this->ValidFilePath();
  const std::string s = NumberToStringA(Value);
  bool result = (WritePrivateProfileStringA(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileA::WriteBool(const std::string& Section, const std::string& Key, bool Value)
{
  this->ValidFilePath();
  const std::string s(Value ? "1" : "0");
  bool result = (WritePrivateProfileStringA(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileA::WriteFloat(const std::string& Section, const std::string& Key, float Value)
{
  this->ValidFilePath();
  const std::string s = NumberToStringA(Value);
  bool result = (WritePrivateProfileStringA(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileA::WriteString(const std::string& Section, const std::string& Key, const std::string& Value)
{
  this->ValidFilePath();
  bool result = (WritePrivateProfileStringA(Section.c_str(), Key.c_str(), Value.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileA::WriteStruct(const std::string& Section, const std::string& Key, void* pStruct, ulong ulSize)
{
  this->ValidFilePath();
  bool result = (WritePrivateProfileStructA(Section.c_str(), Key.c_str(), pStruct, ulSize, m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

// Short-Write

bool vuapi CINIFileA::WriteInteger(const std::string& Key, int Value)
{
  return this->WriteInteger(m_Section, Key, Value);
}

bool vuapi CINIFileA::WriteBool(const std::string& Key, bool Value)
{
  return this->WriteBool(m_Section, Key, Value);
}

bool vuapi CINIFileA::WriteFloat(const std::string& Key, float Value)
{
  return this->WriteFloat(m_Section, Key, Value);
}

bool vuapi CINIFileA::WriteString(const std::string& Key, const std::string& Value)
{
  return this->WriteString(m_Section, Key, Value);
}

bool vuapi CINIFileA::WriteStruct(const std::string& Key, void* pStruct, ulong ulSize)
{
  return this->WriteStruct(m_Section, Key, pStruct, ulSize);
}

CINIFileW::CINIFileW(const std::wstring& FilePath)
{
  m_FilePath = FilePath;

  m_LastErrorCode = ERROR_SUCCESS;
}

CINIFileW::CINIFileW(const std::wstring& FilePath, const std::wstring& Section)
{
  m_FilePath = FilePath;
  m_Section  = Section;

  m_LastErrorCode = ERROR_SUCCESS;
}

void CINIFileW::ValidFilePath()
{
  if (m_FilePath.empty())
  {
    std::wstring filePath = GetCurrentFilePathW();
    std::wstring fileDir = ExtractFilePathW(filePath, true);
    std::wstring fileName = ExtractFileNameW(filePath, false);
    m_FilePath = fileDir + fileName + L".INI";
  }
}

void CINIFileW::SetCurrentFilePath(const std::wstring& FilePath)
{
  m_FilePath = FilePath;
}

void CINIFileW::SetCurrentSection(const std::wstring& Section)
{
  m_Section = Section;
}

// Long-Read

std::vector<std::wstring> vuapi CINIFileW::ReadSectionNames(ulong ulMaxSize)
{
  std::vector<std::wstring> l;
  l.clear();

  std::unique_ptr<wchar[]> p(new wchar [ulMaxSize]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), 2*ulMaxSize);

  this->ValidFilePath();

  GetPrivateProfileSectionNamesW(p.get(), 2*ulMaxSize, m_FilePath.c_str());

  m_LastErrorCode = GetLastError();

  for (wchar * s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::wstring(s))->c_str());
    s += lstrlenW(s);
  }

  return l;
}

std::vector<std::wstring> vuapi CINIFileW::ReadSection(const std::wstring& Section, ulong ulMaxSize)
{
  std::vector<std::wstring> l;
  l.clear();

  std::unique_ptr<wchar[]> p(new wchar [ulMaxSize]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), 2*ulMaxSize);

  this->ValidFilePath();

  GetPrivateProfileSectionW(Section.c_str(), p.get(), 2*ulMaxSize, m_FilePath.c_str());

  m_LastErrorCode = GetLastError();

  for (wchar * s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::wstring(s))->c_str());
    s += lstrlenW(s);
  }

  return l;
}

int vuapi CINIFileW::ReadInteger(const std::wstring& Section, const std::wstring& Key, int Default)
{
  this->ValidFilePath();
  int result = GetPrivateProfileIntW(Section.c_str(), Key.c_str(), Default, m_FilePath.c_str());
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileW::ReadBool(const std::wstring& Section, const std::wstring& Key, bool Default)
{
  this->ValidFilePath();
  bool result = (GetPrivateProfileIntW(Section.c_str(), Key.c_str(), Default, m_FilePath.c_str()) == 1 ? true : false);
  m_LastErrorCode = GetLastError();
  return result;
}

float vuapi CINIFileW::ReadFloat(const std::wstring& Section, const std::wstring& Key, float Default)
{
  this->ValidFilePath();

  const std::wstring sDefault = NumberToStringW(Default);
  wchar lpwszResult[MAX_SIZE];

  ZeroMemory(lpwszResult, sizeof(lpwszResult));

  GetPrivateProfileStringW(
    Section.c_str(),
    Key.c_str(),
    sDefault.c_str(),
    lpwszResult,
    sizeof(lpwszResult),
    m_FilePath.c_str()
  );

  m_LastErrorCode = GetLastError();

  const std::string s = ToStringA(lpwszResult);

  return (float)atof(s.c_str());
}

std::wstring vuapi CINIFileW::ReadString(
  const std::wstring& Section,
  const std::wstring& Key,
  const std::wstring& Default
)
{
  std::wstring s;
  s.clear();

  std::unique_ptr<wchar[]> p(new wchar [MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), 2*MAX_SIZE);

  this->ValidFilePath();

  ulong result = GetPrivateProfileStringW(
    Section.c_str(),
    Key.c_str(),
    Default.c_str(),
    p.get(),
    2 * MAX_SIZE,
    m_FilePath.c_str()
  );
  if (result == 0)
  {
    m_LastErrorCode = GetLastError();
    return s;
  }

  s.assign(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi CINIFileW::ReadStruct(
  const std::wstring& Section,
  const std::wstring& Key,
  ulong ulSize
)
{
  std::unique_ptr<uchar[]> p(new uchar [ulSize]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), ulSize);

  this->ValidFilePath();

  if (GetPrivateProfileStructW(Section.c_str(), Key.c_str(), (void*)p.get(), ulSize, m_FilePath.c_str()) == 0)
  {
    m_LastErrorCode = GetLastError();
    return nullptr;
  }

  return p;
}

// Short-Read

std::vector<std::wstring> vuapi CINIFileW::ReadSection(ulong ulMaxSize)
{
  return this->ReadSection(m_Section, ulMaxSize);
}

int vuapi CINIFileW::ReadInteger(const std::wstring& Key, int Default)
{
  return this->ReadInteger(m_Section, Key, Default);
}

bool vuapi CINIFileW::ReadBool(const std::wstring& Key, bool Default)
{
  return this->ReadBool(m_Section, Key, Default);
}

float vuapi CINIFileW::ReadFloat(const std::wstring& Key, float Default)
{
  return this->ReadFloat(m_Section, Key, Default);
}

std::wstring vuapi CINIFileW::ReadString(const std::wstring& Key, const std::wstring& Default)
{
  return this->ReadString(m_Section, Key, Default);
}

std::unique_ptr<uchar[]> vuapi CINIFileW::ReadStruct(const std::wstring& Key, ulong ulSize)
{
  return this->ReadStruct(m_Section, Key, ulSize);
}

// Long-Write

bool vuapi CINIFileW::WriteInteger(const std::wstring& Section, const std::wstring& Key, int Value)
{
  this->ValidFilePath();
  const std::wstring s = NumberToStringW(Value);
  bool result = (WritePrivateProfileStringW(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileW::WriteBool(const std::wstring& Section, const std::wstring& Key, bool Value)
{
  this->ValidFilePath();
  const std::wstring s(Value ? L"1" : L"0");
  return (WritePrivateProfileStringW(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
}

bool vuapi CINIFileW::WriteFloat(const std::wstring& Section, const std::wstring& Key, float Value)
{
  this->ValidFilePath();
  const std::wstring s = NumberToStringW(Value);
  bool result = (WritePrivateProfileStringW(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileW::WriteString(const std::wstring& Section, const std::wstring& Key, const std::wstring& Value)
{
  this->ValidFilePath();
  bool result = (WritePrivateProfileStringW(Section.c_str(), Key.c_str(), Value.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileW::WriteStruct(
  const std::wstring& Section,
  const std::wstring& Key,
  void* pStruct,
  ulong ulSize
)
{
  this->ValidFilePath();
  bool result = (WritePrivateProfileStructW(Section.c_str(), Key.c_str(), pStruct, ulSize, m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

// Short-Write

bool vuapi CINIFileW::WriteInteger(const std::wstring& Key, int Value)
{
  return this->WriteInteger(m_Section.c_str(), Key.c_str(), Value);
}

bool vuapi CINIFileW::WriteBool(const std::wstring& Key, bool Value)
{
  return this->WriteBool(m_Section.c_str(), Key.c_str(), Value);
}

bool vuapi CINIFileW::WriteFloat(const std::wstring& Key, float Value)
{
  return this->WriteFloat(m_Section.c_str(), Key.c_str(), Value);
}

bool vuapi CINIFileW::WriteString(const std::wstring& Key, const std::wstring& Value)
{
  return this->WriteString(m_Section.c_str(), Key.c_str(), Value);
}

bool vuapi CINIFileW::WriteStruct(const std::wstring& Key, void* pStruct, ulong ulSize)
{
  return this->WriteStruct(m_Section.c_str(), Key.c_str(), pStruct, ulSize);
}

/* --- Group : Registry --- */

HKEY vuapi CRegistryX::GetCurrentKeyHandle()
{
  return m_HKSubKey;
}

eRegReflection vuapi CRegistryX::QueryReflectionKey()
{
  BOOL bReflectedDisabled = FALSE;

  if (InitMiscRoutine() != VU_OK)
  {
    return eRegReflection::RR_ERROR;
  }

  if (pfnRegQueryReflectionKey(m_HKSubKey, &bReflectedDisabled) != ERROR_SUCCESS)
  {
    return eRegReflection::RR_ERROR;
  }

  if (bReflectedDisabled == TRUE)
  {
    return eRegReflection::RR_DISABLED;
  }
  else
  {
    return eRegReflection::RR_ENABLED;
  }
}

bool vuapi CRegistryX::SetReflectionKey(eRegReflection RegReflection)
{
  bool result = false;

  if (InitMiscRoutine() != VU_OK)
  {
    return result;
  }

  switch (RegReflection)
  {
  case eRegReflection::RR_DISABLE:
    result = (pfnRegDisableReflectionKey(m_HKSubKey) == ERROR_SUCCESS);
    break;
  case eRegReflection::RR_ENABLE:
    result = (pfnRegEnableReflectionKey(m_HKSubKey) == ERROR_SUCCESS);
    break;
  default:
    result = false;
    break;
  }

  return result;
}

bool vuapi CRegistryX::CloseKey()
{
  m_LastErrorCode = RegCloseKey(m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

CRegistryA::CRegistryA()
{
  m_HKRootKey = NULL;
  m_HKSubKey = NULL;
  m_SubKey.clear();

  m_LastErrorCode = ERROR_SUCCESS;
}

CRegistryA::CRegistryA(eRegRoot RegRoot)
{
  m_HKRootKey = (HKEY)RegRoot;
  m_HKSubKey = NULL;

  m_LastErrorCode = ERROR_SUCCESS;
}

CRegistryA::CRegistryA(eRegRoot RegRoot, const std::string& SubKey)
{
  m_HKRootKey = (HKEY)RegRoot;
  m_HKSubKey = NULL;
  m_SubKey = SubKey;

  m_LastErrorCode = ERROR_SUCCESS;
}

ulong vuapi CRegistryA::GetSizeOfMultiString(const char* lpcszMultiString)
{
  ulong ulLength = 0;

  while (*lpcszMultiString)
  {
    ulong crLength = lstrlenA(lpcszMultiString) + sizeof(char);
    ulLength += crLength;
    lpcszMultiString = lpcszMultiString + crLength;
  }

  return ulLength;
}

ulong vuapi CRegistryA::GetDataSize(const std::string& ValueName, ulong ulType)
{
  ulong ulDataSize = 0;

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, NULL, &ulDataSize);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return 0;
  }

  return ulDataSize;
}

bool vuapi CRegistryA::CreateKey()
{
  return this->CreateKey(m_SubKey);
}

bool vuapi CRegistryA::CreateKey(const std::string& SubKey)
{
  m_LastErrorCode = RegCreateKeyA(m_HKRootKey, SubKey.c_str(), &m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::KeyExists()
{
  return this->KeyExists(m_SubKey);
}

bool vuapi CRegistryA::KeyExists(const std::string& SubKey)
{
  m_LastErrorCode = RegOpenKeyA(m_HKRootKey, SubKey.c_str(), &m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::OpenKey(eRegAccess RegAccess)
{
  return this->OpenKey(m_SubKey, RegAccess);
}

bool vuapi CRegistryA::OpenKey(const std::string& SubKey, eRegAccess RegAccess)
{
  m_LastErrorCode = RegOpenKeyExA(m_HKRootKey, SubKey.c_str(), 0, (REGSAM)RegAccess, &m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::DeleteKey()
{
  return this->DeleteKey(m_SubKey);
}

bool vuapi CRegistryA::DeleteKey(const std::string& SubKey)
{
  if (m_HKRootKey == 0)
  {
    return false;
  }

  m_LastErrorCode = RegDeleteKeyA(m_HKRootKey, SubKey.c_str());

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::DeleteValue(const std::string& Value)
{
  if (m_HKSubKey == 0)
  {
    return false;
  }

  m_LastErrorCode = RegDeleteValueA(m_HKSubKey, Value.c_str());

  return (m_LastErrorCode == ERROR_SUCCESS);
}

std::vector<std::string> vuapi CRegistryA::EnumKeys()
{
  std::vector<std::string> l;
  l.clear();

  char Class[MAXPATH] = { 0 };
  ulong cchClass = MAXPATH;
  ulong cSubKeys = 0;
  ulong cbMaxSubKeyLen = 0;
  ulong cbMaxClassLen = 0;
  ulong cValues = 0;
  ulong cbMaxValueNameLen = 0;
  ulong cbMaxValueLen = 0;
  ulong cbSecurityDescriptor = 0;
  FILETIME ftLastWriteTime = { 0 };

  ZeroMemory(&Class, sizeof(Class));
  ZeroMemory(&ftLastWriteTime, sizeof(ftLastWriteTime));

  m_LastErrorCode = RegQueryInfoKeyA(
    m_HKSubKey,
    (char*)&Class,
    &cchClass,
    NULL,
    &cSubKeys,
    &cbMaxSubKeyLen,
    &cbMaxClassLen,
    &cValues,
    &cbMaxValueNameLen,
    &cbMaxValueLen,
    &cbSecurityDescriptor,
    &ftLastWriteTime
  );

  if (m_LastErrorCode != ERROR_SUCCESS || cSubKeys == 0) return l;

  cbMaxSubKeyLen += 1;

  char * pSubKeyName = new char[cbMaxSubKeyLen];

  for (ulong i = 0; i < cSubKeys; i++)
  {
    ZeroMemory(pSubKeyName, cbMaxSubKeyLen);
    m_LastErrorCode = RegEnumKeyA(m_HKSubKey, i, pSubKeyName, cbMaxSubKeyLen);
    l.push_back(pSubKeyName);
  }

  delete[] pSubKeyName;

  return l;
}

std::vector<std::string> vuapi CRegistryA::EnumValues()
{
  std::vector<std::string> l;
  l.clear();

  char Class[MAXPATH] = { 0 };
  ulong cchClass = MAXPATH;
  ulong cSubKeys = 0;
  ulong cbMaxSubKeyLen = 0;
  ulong cbMaxClassLen = 0;
  ulong cValues = 0;
  ulong cbMaxValueNameLen = 0;
  ulong cbMaxValueLen = 0;
  ulong cbSecurityDescriptor = 0;
  FILETIME ftLastWriteTime = { 0 };

  ZeroMemory(&Class, sizeof(Class));
  ZeroMemory(&ftLastWriteTime, sizeof(ftLastWriteTime));

  m_LastErrorCode = RegQueryInfoKeyA(
    m_HKSubKey,
    (char*)&Class,
    &cchClass,
    NULL,
    &cSubKeys,
    &cbMaxSubKeyLen,
    &cbMaxClassLen,
    &cValues,
    &cbMaxValueNameLen,
    &cbMaxValueLen,
    &cbSecurityDescriptor,
    &ftLastWriteTime
  );

  if (m_LastErrorCode != ERROR_SUCCESS || cValues == 0) return l;

  cbMaxValueNameLen += 1;

  char * pValueName = new char[cbMaxValueNameLen];
  ulong ulValueNameLength;
  for (ulong i = 0; i < cValues; i++)
  {
    ulValueNameLength = cbMaxValueNameLen;
    ZeroMemory(pValueName, cbMaxValueNameLen);
    m_LastErrorCode = RegEnumValueA(m_HKSubKey, i, pValueName, &ulValueNameLength, NULL, NULL, NULL, NULL);
    l.push_back(pValueName);
  }

  delete[] pValueName;

  return l;
}

// Write

bool vuapi CRegistryA::WriteInteger(const std::string& ValueName, int Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(m_HKSubKey, ValueName.c_str(), 0, REG_DWORD, (const uchar*)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::WriteBool(const std::string& ValueName, bool Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar*)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::WriteFloat(const std::string& ValueName, float Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar*)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

// For REG_SZ, REG_MULTI_SZ and REG_EXPAND_SZ:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724884(v=vs.85).aspx

bool vuapi CRegistryA::WriteString(const std::string& ValueName, const std::string& Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_SZ,
    (const uchar*)Value.c_str(),
    (ulong)Value.length() + sizeof(char)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::WriteMultiString(const std::string& ValueName, const char* lpValue)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_MULTI_SZ,
    (const uchar*)lpValue,
    this->GetSizeOfMultiString(lpValue) + sizeof(char)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::WriteMultiString(const std::string& ValueName, const std::vector<std::string>& Value)
{
  auto p = ListToMultiStringA(Value);
  return this->WriteMultiString(ValueName, p.get());
}

bool vuapi CRegistryA::WriteExpandString(const std::string& ValueName, const std::string& Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_EXPAND_SZ,
    (const uchar*)Value.c_str(),
    (ulong)Value.length() + sizeof(char)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::WriteBinary(const std::string& ValueName, void* lpData, ulong ulSize)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar*)lpData, ulSize);

  return (m_LastErrorCode == ERROR_SUCCESS);
}

// Read

int vuapi CRegistryA::ReadInteger(const std::string& ValueName, int Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  int ret = 0;
  ulong ulType = REG_DWORD, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

bool vuapi CRegistryA::ReadBool(const std::string& ValueName, bool Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  bool ret = false;
  ulong ulType = REG_BINARY, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

float vuapi CRegistryA::ReadFloat(const std::string& ValueName, float Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  float ret = 0;
  ulong ulType = REG_BINARY, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

std::string vuapi CRegistryA::ReadString(const std::string& ValueName, const std::string& Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(char);

  std::unique_ptr<char[]> p(new char [ulReturn]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::string s(p.get());

  return s;
}

std::vector<std::string> vuapi CRegistryA::ReadMultiString(
  const std::string& ValueName,
  std::vector<std::string> Default
)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_MULTI_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(char);

  std::unique_ptr<char[]> p(new char [ulReturn]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::vector<std::string> l = MultiStringToListA(p.get());

  return l;
}

std::string vuapi CRegistryA::ReadExpandString(const std::string& ValueName, const std::string& Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_EXPAND_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(char);

  std::unique_ptr<char[]> p(new char [ulReturn]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::string s(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi CRegistryA::ReadBinary(const std::string& ValueName, const void* pDefault)
{
  std::unique_ptr<uchar[]> pDef((uchar*)pDefault);

  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return pDef;
  }

  ulong ulType = REG_BINARY, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    m_LastErrorCode = ERROR_INCORRECT_SIZE;
    SetLastError(m_LastErrorCode);
    return pDef;
  }

  ulReturn += 1;

  std::unique_ptr<uchar[]> p(new uchar [ulReturn]);
  if (p == nullptr)
  {
    return pDef;
  }

  ZeroMemory((void*)p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return pDef;
  }

  return p;
}

CRegistryW::CRegistryW()
{
  m_HKRootKey = NULL;
  m_HKRootKey = NULL;
  m_SubKey.clear();

  m_LastErrorCode = ERROR_SUCCESS;
}

CRegistryW::CRegistryW(eRegRoot RegRoot)
{
  m_HKRootKey = (HKEY)RegRoot;
  m_HKSubKey = NULL;

  m_LastErrorCode = ERROR_SUCCESS;
}

CRegistryW::CRegistryW(eRegRoot RegRoot, const std::wstring& SubKey)
{
  m_HKRootKey = (HKEY)RegRoot;
  m_HKSubKey = NULL;
  m_SubKey = SubKey;

  m_LastErrorCode = ERROR_SUCCESS;
}

ulong vuapi CRegistryW::GetSizeOfMultiString(const wchar* lpcwszMultiString)
{
  ulong ulLength = 0;

  while (*lpcwszMultiString)
  {
    ulong crLength = sizeof(wchar)*(lstrlenW(lpcwszMultiString) + 1);
    ulLength += crLength;
    lpcwszMultiString = lpcwszMultiString + crLength / 2;
  }

  return ulLength;
}

ulong vuapi CRegistryW::GetDataSize(const std::wstring& ValueName, ulong ulType)
{
  ulong ulDataSize = 0;

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, NULL, &ulDataSize);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return 0;
  }

  return ulDataSize;
}

bool vuapi CRegistryW::CreateKey()
{
  return this->CreateKey(m_SubKey);
}

bool vuapi CRegistryW::CreateKey(const std::wstring& SubKey)
{
  m_LastErrorCode = RegCreateKeyW(m_HKRootKey, SubKey.c_str(), &m_HKRootKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::KeyExists()
{
  return this->KeyExists(m_SubKey);
}

bool vuapi CRegistryW::KeyExists(const std::wstring& SubKey)
{
  m_LastErrorCode = RegOpenKeyW(m_HKRootKey, SubKey.c_str(), &m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::OpenKey(eRegAccess RegAccess)
{
  return this->OpenKey(m_SubKey, RegAccess);
}

bool vuapi CRegistryW::OpenKey(const std::wstring& SubKey, eRegAccess RegAccess)
{
  m_LastErrorCode = RegOpenKeyExW(m_HKRootKey, SubKey.c_str(), 0, (REGSAM)RegAccess, &m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::DeleteKey()
{
  return this->DeleteKey(m_SubKey);
}

bool vuapi CRegistryW::DeleteKey(const std::wstring& SubKey)
{
  if (m_HKRootKey == 0)
  {
    return false;
  }

  m_LastErrorCode = RegDeleteKeyW(m_HKRootKey, SubKey.c_str());

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::DeleteValue(const std::wstring& Value)
{
  if (m_HKSubKey == 0)
  {
    return false;
  }

  m_LastErrorCode = RegDeleteValueW(m_HKSubKey, Value.c_str());

  return (m_LastErrorCode == ERROR_SUCCESS);
}

std::vector<std::wstring> vuapi CRegistryW::EnumKeys()
{
  std::vector<std::wstring> l;
  l.clear();

  wchar Class[MAXPATH] = { 0 };
  ulong cchClass = MAXPATH;
  ulong cSubKeys = 0;
  ulong cbMaxSubKeyLen = 0;
  ulong cbMaxClassLen = 0;
  ulong cValues = 0;
  ulong cbMaxValueNameLen = 0;
  ulong cbMaxValueLen = 0;
  ulong cbSecurityDescriptor = 0;
  FILETIME ftLastWriteTime = { 0 };

  ZeroMemory(&Class, sizeof(Class));
  ZeroMemory(&ftLastWriteTime, sizeof(ftLastWriteTime));

  m_LastErrorCode = RegQueryInfoKeyW(
    m_HKSubKey,
    (wchar*)&Class,
    &cchClass,
    NULL,
    &cSubKeys,
    &cbMaxSubKeyLen,
    &cbMaxClassLen,
    &cValues,
    &cbMaxValueNameLen,
    &cbMaxValueLen,
    &cbSecurityDescriptor,
    &ftLastWriteTime
  );

  if (m_LastErrorCode != ERROR_SUCCESS || cSubKeys == 0) return l;

  cbMaxSubKeyLen += 1;

  wchar * pSubKeyName = new wchar[cbMaxSubKeyLen];

  for (ulong i = 0; i < cSubKeys; i++)
  {
    ZeroMemory(pSubKeyName, sizeof(wchar)*cbMaxSubKeyLen);
    m_LastErrorCode = RegEnumKeyW(m_HKSubKey, i, pSubKeyName, sizeof(wchar)*cbMaxSubKeyLen);
    l.push_back(pSubKeyName);
  }

  delete[] pSubKeyName;

  return l;
}

std::vector<std::wstring> vuapi CRegistryW::EnumValues()
{
  std::vector<std::wstring> l;
  l.clear();

  wchar Class[MAXPATH] = { 0 };
  ulong cchClass = MAXPATH;
  ulong cSubKeys = 0;
  ulong cbMaxSubKeyLen = 0;
  ulong cbMaxClassLen = 0;
  ulong cValues = 0;
  ulong cbMaxValueNameLen = 0;
  ulong cbMaxValueLen = 0;
  ulong cbSecurityDescriptor = 0;
  FILETIME ftLastWriteTime = { 0 };

  ZeroMemory(&Class, sizeof(Class));
  ZeroMemory(&ftLastWriteTime, sizeof(ftLastWriteTime));

  m_LastErrorCode = RegQueryInfoKeyA(
    m_HKSubKey,
    (char*)&Class,
    &cchClass,
    NULL,
    &cSubKeys,
    &cbMaxSubKeyLen,
    &cbMaxClassLen,
    &cValues,
    &cbMaxValueNameLen,
    &cbMaxValueLen,
    &cbSecurityDescriptor,
    &ftLastWriteTime
  );

  if (m_LastErrorCode != ERROR_SUCCESS || cValues == 0) return l;

  cbMaxValueNameLen += 1;

  wchar * pValueName = new wchar[cbMaxValueNameLen];
  ulong ulValueNameLength;
  for (ulong i = 0; i < cValues; i++)
  {
    ulValueNameLength = sizeof(wchar)*cbMaxValueNameLen;
    ZeroMemory(pValueName, cbMaxValueNameLen);
    m_LastErrorCode = RegEnumValueW(m_HKSubKey, i, pValueName, &ulValueNameLength, NULL, NULL, NULL, NULL);
    l.push_back(pValueName);
  }

  delete[] pValueName;

  return l;
}

// Write

bool vuapi CRegistryW::WriteInteger(const std::wstring& ValueName, int Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(m_HKSubKey, ValueName.c_str(), 0, REG_DWORD, (const uchar *)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::WriteBool(const std::wstring& ValueName, bool Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar *)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::WriteFloat(const std::wstring& ValueName, float Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar *)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

// For REG_SZ, REG_MULTI_SZ and REG_EXPAND_SZ:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724884(v=vs.85).aspx

bool vuapi CRegistryW::WriteString(const std::wstring& ValueName, const std::wstring& Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_SZ,
    (const uchar *)Value.c_str(),
    sizeof(wchar)*((ulong)Value.length() + 1)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::WriteMultiString(const std::wstring& ValueName, const wchar * lpValue)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_MULTI_SZ,
    (const uchar *)lpValue,
    this->GetSizeOfMultiString(lpValue) + sizeof(wchar)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::WriteMultiString(const std::wstring& ValueName, const std::vector<std::wstring> Value)
{
  auto p = ListToMultiStringW(Value);
  return this->WriteMultiString(ValueName, p.get());
}

bool vuapi CRegistryW::WriteExpandString(const std::wstring& ValueName, const std::wstring& Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_EXPAND_SZ,
    (const uchar *)Value.c_str(),
    sizeof(wchar)*((ulong)Value.length() + 1)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::WriteBinary(const std::wstring& ValueName, void* lpData, ulong ulSize)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar *)lpData, ulSize);

  return (m_LastErrorCode == ERROR_SUCCESS);
}

// Read

int vuapi CRegistryW::ReadInteger(const std::wstring& ValueName, int Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  int ret = 0;
  ulong ulType = REG_DWORD, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

bool vuapi CRegistryW::ReadBool(const std::wstring& ValueName, bool Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  bool ret = false;
  ulong ulType = REG_BINARY, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

float vuapi CRegistryW::ReadFloat(const std::wstring& ValueName, float Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  float ret = 0;
  ulong ulType = REG_BINARY, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

std::wstring vuapi CRegistryW::ReadString(const std::wstring& ValueName, const std::wstring& Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(wchar);

  std::unique_ptr<wchar[]> p(new wchar [ulReturn / sizeof(wchar)]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::wstring s(p.get());

  return s;
}

std::vector<std::wstring> vuapi CRegistryW::ReadMultiString(
  const std::wstring& ValueName,
  std::vector<std::wstring> Default
)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_MULTI_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(wchar);

  std::unique_ptr<wchar[]> p(new wchar [ulReturn / sizeof(wchar)]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::vector<std::wstring> l = MultiStringToListW(p.get());

  return l;
}

std::wstring vuapi CRegistryW::ReadExpandString(const std::wstring& ValueName, const std::wstring& Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_EXPAND_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(wchar);

  std::unique_ptr<wchar[]> p(new wchar [ulReturn / 2]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::wstring s(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi CRegistryW::ReadBinary(const std::wstring& ValueName, const void* pDefault)
{
  std::unique_ptr<uchar[]> pDef((uchar*)pDefault);

  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return pDef;
  }

  ulong ulType = REG_BINARY, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    m_LastErrorCode = ERROR_INCORRECT_SIZE;
    SetLastError(m_LastErrorCode);
    return pDef;
  }

  ulReturn += sizeof(tchar); // padding for safety

  std::unique_ptr<uchar[]> p(new uchar [ulReturn]);
  if (p == nullptr)
  {
    return pDef;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return pDef;
  }

  return p;
}

/* --- Group : Critical Section --- */

void vuapi CCriticalSection::Init()
{
  InitializeCriticalSection(&m_CriticalSection);
}

void vuapi CCriticalSection::Enter()
{
  EnterCriticalSection(&m_CriticalSection);
}

void vuapi CCriticalSection::Leave()
{
  LeaveCriticalSection(&m_CriticalSection);
}

void vuapi CCriticalSection::Destroy()
{
  DeleteCriticalSection(&m_CriticalSection);
}

TCriticalSection& vuapi CCriticalSection::GetCurrentSection()
{
  return m_CriticalSection;
}

/* --- Group : Stop Watch --- */

CStopWatch::CStopWatch()
{
  m_Reset = true;
  m_Count = 0;
  m_Delta = 0;
  m_Duration = 0;
  m_DeltaHistory.clear();
}

CStopWatch::~CStopWatch()
{
  m_DeltaHistory.clear();
}

void CStopWatch::Start(bool reset)
{
  m_Reset = reset;

  if (m_Reset)
  {
    m_Duration = 0;
    m_DeltaHistory.clear();
  }

  m_Count = std::clock();
}

const CStopWatch::TDuration CStopWatch::Stop()
{
  m_Delta = std::clock() - m_Count;

  if (!m_Reset) m_DeltaHistory.push_back(m_Delta);

  return this->Duration();
}

const CStopWatch::TDuration CStopWatch::Duration()
{
  if (!m_Reset)
  {
    auto N = m_DeltaHistory.size();
    for (decltype(N) i = 0; N >= 2, i < N - 1; i++) m_Delta += m_DeltaHistory.at(i);
  }

  m_Duration = (float(m_Delta) / float(CLOCKS_PER_SEC));

  TDuration duration(m_Delta, m_Duration);

  return duration;
}

/* --- Group : File Working --- */

CFileX::~CFileX()
{
  this->Close();
}

bool vuapi CFileX::IsFileHandleValid(HANDLE fileHandle)
{
  if (!fileHandle || fileHandle == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  return true;
}

bool vuapi CFileX::IsReady()
{
  return this->IsFileHandleValid(m_FileHandle);
}

bool vuapi CFileX::Read(
  ulong ulOffset,
  void* Buffer,
  ulong ulSize,
  eMoveMethodFlags mmFlag
)
{
  if (!this->Seek(ulOffset, mmFlag)) return false;

  BOOL result = ReadFile(m_FileHandle, Buffer, ulSize, (LPDWORD)&m_ReadSize, NULL);
  if (!result && ulSize != m_ReadSize)
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileX::Read(void* Buffer, ulong ulSize)
{
  BOOL result = ReadFile(m_FileHandle, Buffer, ulSize, (LPDWORD)&m_ReadSize, NULL);
  if (!result && ulSize != m_ReadSize)
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileX::Write(
  ulong ulOffset,
  const void* cBuffer,
  ulong ulSize,
  eMoveMethodFlags mmFlag
)
{
  if (!this->Seek(ulOffset, mmFlag)) return false;

  BOOL result = WriteFile(m_FileHandle, cBuffer, ulSize, (LPDWORD)&m_WroteSize, NULL);
  if (!result && ulSize != m_WroteSize)
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileX::Write(const void* cBuffer, ulong ulSize)
{
  BOOL result = WriteFile(m_FileHandle, cBuffer, ulSize, (LPDWORD)&m_WroteSize, NULL);
  if (!result && ulSize != m_WroteSize)
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileX::Seek(ulong ulOffset, eMoveMethodFlags mmFlag)
{
  if (!this->IsFileHandleValid(m_FileHandle))
  {
    return false;
  }

  ulong result = SetFilePointer(m_FileHandle, ulOffset, NULL, mmFlag);

  m_LastErrorCode = GetLastError();

  return (result != INVALID_SET_FILE_POINTER);
}

ulong vuapi CFileX::GetFileSize()
{
  if (!this->IsFileHandleValid(m_FileHandle))
  {
    return 0;
  }

  ulong result = ::GetFileSize(m_FileHandle, NULL);

  m_LastErrorCode = GetLastError();

  return result;
}

bool vuapi CFileX::IOControl(
  ulong ulControlCode,
  void* lpSendBuffer,
  ulong ulSendSize,
  void* lpReveiceBuffer,
  ulong ulReveiceSize
)
{
  ulong ulReturnedLength = 0;

  bool result = (DeviceIoControl(
    m_FileHandle,
    ulControlCode,
    lpSendBuffer,
    ulSendSize,
    lpReveiceBuffer,
    ulReveiceSize,
    &ulReturnedLength,
    NULL
  ) != 0);

  m_LastErrorCode = GetLastError();

  return result;
}

bool vuapi CFileX::Close()
{
  if (!this->IsFileHandleValid(m_FileHandle))
  {
    return false;
  }

  if (!CloseHandle(m_FileHandle))
  {
    return false;
  }

  m_FileHandle = INVALID_HANDLE_VALUE;

  return true;
}

const CBinary vuapi CFileX::ReadContent()
{
  CBinary pContent(0);

  auto size = this->GetFileSize();
  if (size == 0) return pContent;

  #ifdef _WIN64
  size += 8;
  #else // _WIN32
  size += 4;
  #endif

  pContent.AdjustSize(size);

  this->Read(0, pContent.GetpData(), size, eMoveMethodFlags::MM_BEGIN);

  return pContent;
}

// A

CFileA::CFileA(
  const std::string& FilePath,
  eFileModeFlags fmFlag,
  eFileGenericFlags fgFlag,
  eFileShareFlags fsFlag,
  eFileAttributeFlags faFlag
)
{
  this->Init(FilePath, fmFlag, fgFlag, fsFlag, faFlag);
}

bool vuapi CFileA::Init(
  const std::string& FilePath,
  eFileModeFlags fmFlag,
  eFileGenericFlags fgFlag,
  eFileShareFlags fsFlag,
  eFileAttributeFlags faFlag
)
{
  m_FileHandle = CreateFileA(FilePath.c_str(), fgFlag, fsFlag, NULL, fmFlag, faFlag, NULL);
  if (!this->IsFileHandleValid(m_FileHandle))
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

const std::string vuapi CFileA::ReadFileAsString(bool forceBOM)
{
  std::string result("");

  auto pContent = this->ReadContent();
  auto p = (char*)pContent.GetpData();

  auto encodingType = DetermineEncodingType(pContent.GetpData(), pContent.GetSize());
  if (encodingType == eEncodingType::ET_UTF8 || encodingType == eEncodingType::ET_UTF8_BOM)
  {
    if (forceBOM && encodingType == eEncodingType::ET_UTF8_BOM)
    {
      p += 3; /* remove BOM */
    }
  }
  else   /* Invalid encoding type */
  {
    assert(0);
    return result;
  }

  result.assign(p);

  return result;
}

const std::string vuapi CFileA::QuickReadFileAsString(const std::string& FilePath, bool forceBOM)
{
  CFileA file(FilePath, vu::eFileModeFlags::FM_OPENEXISTING);
  auto result = file.ReadFileAsString(forceBOM);
  file.Close();
  return result;
}

// W

CFileW::CFileW(
  const std::wstring& FilePath,
  eFileModeFlags fmFlag,
  eFileGenericFlags fgFlag,
  eFileShareFlags fsFlag,
  eFileAttributeFlags faFlag
)
{
  this->Init(FilePath, fmFlag, fgFlag, fsFlag, faFlag);
}

bool vuapi CFileW::Init(
  const std::wstring& FilePath,
  eFileModeFlags fmFlag,
  eFileGenericFlags fgFlag,
  eFileShareFlags fsFlag,
  eFileAttributeFlags faFlag
)
{
  m_FileHandle = CreateFileW(FilePath.c_str(), fgFlag, fsFlag, NULL, fmFlag, faFlag, NULL);
  if (!this->IsFileHandleValid(m_FileHandle))
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

const std::wstring vuapi CFileW::ReadFileAsString(bool forceBOM)
{
  std::wstring result(L"");

  auto pContent = this->ReadContent();
  auto p = (wchar*)pContent.GetpData();

  auto encodingType = DetermineEncodingType(pContent.GetpData(), pContent.GetSize());
  if (encodingType == eEncodingType::ET_UTF16_LE || encodingType == eEncodingType::ET_UTF16_LE_BOM)
  {
    if (forceBOM && encodingType == eEncodingType::ET_UTF16_LE_BOM)
    {
      p = (wchar*)((char*)pContent.GetpData() + 2); /* remove BOM */
    }
  }
  else /* Invalid encoding type */
  {
    assert(0);
    return result;
  }

  result.assign(p);

  return result;
}

const std::wstring vuapi CFileW::QuickReadFileAsString( const std::wstring& FilePath, bool forceBOM)
{
  CFileW file(FilePath, vu::eFileModeFlags::FM_OPENEXISTING);
  auto result = file.ReadFileAsString(forceBOM);
  file.Close();
  return result;
}

/* --- Group : Service Working --- */

bool vuapi CServiceX::Init(eSCAccessType SCAccessType)
{
  m_SCMHandle = OpenSCManager(
    NULL, // Local computer
    NULL, // ServicesActive database
    (ulong)SCAccessType
  );

  m_LastErrorCode = GetLastError();

  if (!m_SCMHandle)
  {
    m_Initialized = false;
    return false;
  }

  m_Initialized = true;

  return true;
}

bool vuapi CServiceX::Destroy()
{
  if (!m_Initialized)
  {
    return false;
  }

  BOOL result = CloseServiceHandle(m_SCMHandle);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return false;
  }

  return true;
}

bool vuapi CServiceX::Start()
{
  if (!m_ServiceHandle)
  {
    return false;
  }

  BOOL result = ::StartServiceW(m_ServiceHandle, 0, NULL);

  m_LastErrorCode = GetLastError();

  return (result != FALSE);
}

bool vuapi CServiceX::Stop()
{
  return this->Control(SC_STOP);
}

bool vuapi CServiceX::Control(eServiceControl ServiceControl)
{
  if (!m_ServiceHandle)
  {
    return false;
  }

  BOOL result = ::ControlService(m_ServiceHandle, (ulong)ServiceControl, (LPSERVICE_STATUS)&m_Status);

  m_LastErrorCode = GetLastError();

  return (result != FALSE);
}

bool vuapi CServiceX::Close()
{
  if (!m_ServiceHandle)
  {
    return false;
  }

  BOOL result = DeleteService(m_ServiceHandle);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return false;
  }

  result = CloseServiceHandle(m_ServiceHandle);

  m_LastErrorCode = GetLastError();

  return (result != FALSE);
}

bool vuapi CServiceX::QueryStatus(TServiceStatus& ServiceStatus)
{
  if (!m_ServiceHandle)
  {
    return false;
  }

  BOOL result = ::QueryServiceStatus(m_ServiceHandle, &ServiceStatus);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return false;
  }

  return true;
}

eServiceType vuapi CServiceX::GetType()
{
  if (!m_ServiceHandle)
  {
    return eServiceType::ST_UNKNOWN;
  }

  if (this->QueryStatus(m_Status) == FALSE)
  {
    return eServiceType::ST_UNKNOWN;
  }

  return (vu::eServiceType)m_Status.dwServiceType;
}

eServiceState vuapi CServiceX::GetState()
{
  if (!m_ServiceHandle)
  {
    return eServiceState::SS_UNKNOWN;
  }

  if (this->QueryStatus(m_Status) == FALSE)
  {
    return eServiceState::SS_UNKNOWN;
  }

  return (vu::eServiceState)m_Status.dwCurrentState;
}

// A

CServiceA::CServiceA()
{
  m_ServiceName.clear();
  m_DisplayName.clear();
  m_ServiceFilePath.clear();

  m_SCMHandle     = 0;
  m_ServiceHandle = 0;

  m_LastErrorCode = ERROR_SUCCESS;
}

CServiceA::~CServiceA()
{
  m_SCMHandle     = 0;
  m_ServiceHandle = 0;
}

bool vuapi CServiceA::Create (
  const std::string ServiceFilePath,
  eServiceAccessType ServiceAccessType,
  eServiceType ServiceType,
  eServiceStartType ServiceStartType,
  eServiceErrorControlType ServiceErrorControlType
)
{
  // Is a full file path?
  if (ServiceFilePath.find('\\') == std::string::npos)
  {
    m_ServiceFilePath = GetCurrentDirectoryA(true);
    m_ServiceFilePath.append(ServiceFilePath);
  }
  else
  {
    m_ServiceFilePath.assign(ServiceFilePath);
  }

  if (!IsFileExistsA(m_ServiceFilePath))
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return false;
  }

  m_ServiceFileName = ExtractFileNameA(m_ServiceFilePath);

  m_ServiceName = ExtractFileNameA(m_ServiceFilePath, false);

  time_t t = time(NULL);
  std::string crTime = FormatDateTimeA(t, "%H:%M:%S");
  std::string crDateTime = FormatDateTimeA(t, "%H:%M:%S %d/%m/%Y");

  // Name
  m_Name.clear();
  m_Name.assign(m_ServiceName);
  m_Name.append(" - ");
  m_Name.append(crTime);

  // Display Name
  m_DisplayName.clear();
  m_DisplayName.assign(m_ServiceName);
  m_DisplayName.append(" - ");
  m_DisplayName.append(crDateTime);

  m_ServiceHandle = ::CreateServiceA(
    m_SCMHandle,
    m_Name.c_str(),
    m_DisplayName.c_str(),
    (ulong)ServiceAccessType,
    (ulong)ServiceType,
    (ulong)ServiceStartType,
    (ulong)ServiceErrorControlType,
    m_ServiceFilePath.c_str(),
    NULL, NULL, NULL, NULL, NULL
  );

  m_LastErrorCode = GetLastError();

  if (!m_ServiceHandle)
  {
    CloseServiceHandle(m_SCMHandle);
    return false;
  }

  return true;
}

bool vuapi CServiceA::Open(const std::string& ServiceName, eServiceAccessType ServiceAccessType)
{
  if (!m_SCMHandle)
  {
    return false;
  }

  m_ServiceName = ServiceName;

  m_ServiceHandle = ::OpenServiceA(m_SCMHandle, m_ServiceName.c_str(), (ulong)ServiceAccessType);

  m_LastErrorCode = GetLastError();

  if (!m_ServiceHandle)
  {
    return false;
  }

  return true;
}

std::string vuapi CServiceA::GetName(const std::string& AnotherServiceDisplayName)
{
  if (AnotherServiceDisplayName.empty())
  {
    return m_Name;
  }

  std::string s;
  s.clear();
  ulong nSize = MAX_SIZE;

  std::unique_ptr<char[]> p(new char [MAX_SIZE]);
  ZeroMemory(p.get(), MAX_SIZE);

  BOOL result = GetServiceKeyNameA(m_SCMHandle, AnotherServiceDisplayName.c_str(), p.get(), &nSize);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return s;
  }

  s.assign(p.get());

  return s;
}

std::string vuapi CServiceA::GetDisplayName(const std::string& AnotherServiceName)
{
  if (AnotherServiceName.empty())
  {
    return m_DisplayName;
  }

  std::string s;
  s.clear();
  ulong nSize = MAX_SIZE;

  std::unique_ptr<char[]> p(new char [MAX_SIZE]);
  ZeroMemory(p.get(), MAX_SIZE);

  BOOL result = GetServiceDisplayNameA(m_SCMHandle, AnotherServiceName.c_str(), p.get(), &nSize);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return s;
  }

  s.assign(p.get());

  return s;
}

// W

CServiceW::CServiceW()
{
  m_ServiceName.clear();
  m_DisplayName.clear();
  m_ServiceFilePath.clear();

  m_SCMHandle     = 0;
  m_ServiceHandle = 0;

  m_LastErrorCode = ERROR_SUCCESS;
}

CServiceW::~CServiceW()
{
  m_SCMHandle     = 0;
  m_ServiceHandle = 0;
}

bool vuapi CServiceW::Create(
  const std::wstring& ServiceFilePath,
  eServiceAccessType ServiceAccessType,
  eServiceType ServiceType,
  eServiceStartType ServiceStartType,
  eServiceErrorControlType ServiceErrorControlType
)
{
  // Is a full file path?
  if (ServiceFilePath.find(L'\\') == std::wstring::npos)
  {
    m_ServiceFilePath = GetCurrentDirectoryW(true);
    m_ServiceFilePath.append(ServiceFilePath);
  }
  else
  {
    m_ServiceFilePath.assign(ServiceFilePath);
  }

  if (!IsFileExistsW(m_ServiceFilePath))
  {
    return false;
  }

  m_ServiceFileName = ExtractFileNameW(m_ServiceFilePath);

  m_ServiceName = ExtractFileNameW(m_ServiceFilePath, false);

  time_t t = time(NULL);
  std::wstring crTime = FormatDateTimeW(t, L"%H:%M:%S");
  std::wstring crDateTime = FormatDateTimeW(t, L"%H:%M:%S %d/%m/%Y");

  // Name
  m_Name.clear();
  m_Name.assign(m_ServiceName);
  m_Name.append(L" - ");
  m_Name.append(crTime);

  // Display Name
  m_DisplayName.clear();
  m_DisplayName.assign(m_ServiceName);
  m_DisplayName.append(L" - ");
  m_DisplayName.append(crDateTime);

  m_ServiceHandle = ::CreateServiceW(
    m_SCMHandle,
    m_Name.c_str(),
    m_DisplayName.c_str(),
    (ulong)ServiceAccessType,
    (ulong)ServiceType,
    (ulong)ServiceStartType,
    (ulong)ServiceErrorControlType,
    m_ServiceFilePath.c_str(),
    NULL, NULL, NULL, NULL, NULL
  );

  m_LastErrorCode = GetLastError();

  if (!m_ServiceHandle)
  {
    return false;
  }

  return true;
}

bool vuapi CServiceW::Open(const std::wstring& ServiceName, eServiceAccessType ServiceAccessType)
{
  if (!m_SCMHandle)
  {
    return false;
  }

  m_ServiceName = ServiceName;

  m_ServiceHandle = ::OpenServiceW(m_SCMHandle, m_ServiceName.c_str(), (ulong)ServiceAccessType);

  m_LastErrorCode = GetLastError();

  if (!m_ServiceHandle)
  {
    return false;
  }

  return true;
}

std::wstring vuapi CServiceW::GetName(const std::wstring& AnotherServiceDisplayName)
{
  if (AnotherServiceDisplayName.empty())
  {
    return m_Name;
  }

  std::wstring s;
  s.clear();
  ulong nSize = 2*MAX_SIZE;

  std::unique_ptr<wchar[]> p(new wchar [MAX_SIZE]);
  ZeroMemory(p.get(), 2*MAX_SIZE);

  BOOL result = GetServiceKeyNameW(m_SCMHandle, AnotherServiceDisplayName.c_str(), p.get(), &nSize);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return s;
  }

  s.assign(p.get());

  return s;
}

std::wstring vuapi CServiceW::GetDisplayName(const std::wstring& AnotherServiceName)
{
  if (AnotherServiceName.empty())
  {
    return m_DisplayName;
  }

  std::wstring s;
  s.clear();
  ulong nSize = 2*MAX_SIZE;

  std::unique_ptr<wchar[]> p(new wchar [MAX_SIZE]);
  ZeroMemory(p.get(), 2*MAX_SIZE);

  BOOL result = GetServiceDisplayNameW(m_SCMHandle, AnotherServiceName.c_str(), p.get(), &nSize);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return s;
  }

  s.assign(p.get());

  return s;
}

/* --- Group : PE File --- */

template class CPEFileTX<ulong32>;
template class CPEFileTX<ulong64>;

template<typename T>
CPEFileTX<T>::CPEFileTX()
{
  m_Initialized = false;

  m_pBase = nullptr;
  m_pDosHeader = nullptr;
  m_pPEHeader  = nullptr;
  m_SectionHeaderList.clear();
  m_ImportDescriptorList.clear();
  m_ExIDDList.clear();
  m_FunctionInfoList.clear();

  if (sizeof(T) == 4)
  {
    m_OrdinalFlag = (T)IMAGE_ORDINAL_FLAG32;
  }
  else
  {
    m_OrdinalFlag = (T)IMAGE_ORDINAL_FLAG64;
  }
}

template<typename T>
CPEFileTX<T>::~CPEFileTX() {};

template<typename T>
void* vuapi CPEFileTX<T>::GetpBase()
{
  return m_pBase;
}

template<typename T>
TPEHeaderT<T>* vuapi CPEFileTX<T>::GetpPEHeader()
{
  if (!m_Initialized)
  {
    return nullptr;
  }

  return m_pPEHeader;
}

template<typename T>
std::vector<PSectionHeader>& vuapi CPEFileTX<T>::GetSetionHeaderList(bool Reget)
{
  m_SectionHeaderList.clear();

  if (!m_Initialized)
  {
    return m_SectionHeaderList;
  }

  if (!Reget && (m_SectionHeaderList.size() != 0))
  {
    return m_SectionHeaderList;
  }

  vu::PSectionHeader pSH = (PSectionHeader)((ulong64)m_pPEHeader + sizeof(vu::TNtHeaderT<T>));
  if (pSH == nullptr)
  {
    return m_SectionHeaderList;
  }

  m_SectionHeaderList.clear();
  for (int i = 0; i < m_pPEHeader->NumberOfSections; i++)
  {
    m_SectionHeaderList.push_back(pSH);
    pSH++;
  }

  return m_SectionHeaderList;
}

template<typename T>
std::vector<TExIID>& vuapi CPEFileTX<T>::GetExIIDList()
{
  m_ExIDDList.clear();

  if (!m_Initialized)
  {
    return m_ExIDDList;
  }

  T ulIIDOffset = this->RVA2Offset(m_pPEHeader->Import.VirtualAddress);
  if (ulIIDOffset == T(-1))
  {
    return m_ExIDDList;
  }

  PImportDescriptor pIID = (PImportDescriptor)((ulong64)m_pBase + ulIIDOffset);
  if (pIID == nullptr)
  {
    return m_ExIDDList;
  }

  m_ExIDDList.clear();
  for (int i = 0; pIID->FirstThunk != 0; i++, pIID++)
  {
    std::string DllName = (char*)((ulong64)m_pBase + this->RVA2Offset(pIID->Name));

    TExIID ExIID;
    ExIID.IIDID = i;
    ExIID.Name = DllName;
    ExIID.pIID = pIID;

    m_ExIDDList.push_back(ExIID);
  }

  return m_ExIDDList;
}

template<typename T>
std::vector<PImportDescriptor>& vuapi CPEFileTX<T>::GetImportDescriptorList(bool Reget)
{
  m_ImportDescriptorList.clear();

  if (!m_Initialized)
  {
    return m_ImportDescriptorList;
  }

  if (Reget || (m_ExIDDList.size() == 0))
  {
    this->GetExIIDList();
  }

  if (m_ExIDDList.size() == 0)
  {
    return m_ImportDescriptorList;
  }

  m_ImportDescriptorList.clear();
  for (const auto& e: m_ExIDDList)
  {
    m_ImportDescriptorList.push_back(e.pIID);
  }

  return m_ImportDescriptorList;
}

template<typename T>
std::vector<TDLLInfo> vuapi CPEFileTX<T>::GetDLLInfoList(bool Reget)
{
  std::vector<TDLLInfo> DLLInfoList;
  DLLInfoList.clear();

  if (!m_Initialized)
  {
    return DLLInfoList;
  }

  if (Reget || (m_ExIDDList.size() == 0))
  {
    this->GetExIIDList();
  }

  if (m_ExIDDList.size() == 0)
  {
    return DLLInfoList;
  }

  for (const auto& e: m_ExIDDList)
  {
    TDLLInfo DLLInfo;
    DLLInfo.IIDID = e.IIDID;
    DLLInfo.Name  = e.Name;
    // DLLInfo.NumberOfFuctions = 0;

    DLLInfoList.push_back(DLLInfo);
  }

  return DLLInfoList;
}

template<typename T>
std::vector<TFunctionInfoT<T>> vuapi CPEFileTX<T>::GetFunctionInfoList(bool Reget)
{
  if (!m_Initialized)
  {
    return m_FunctionInfoList;
  }

  if (Reget || (m_ExIDDList.size() == 0))
  {
    this->GetExIIDList();
  }

  if (m_ExIDDList.size() == 0)
  {
    return m_FunctionInfoList;
  }

  m_FunctionInfoList.clear();
  TThunkDataT<T>* pThunkData = nullptr;
  TFunctionInfoT<T> funcInfo;
  for (const auto& e: m_ExIDDList)
  {
    T ulOffset = this->RVA2Offset(e.pIID->FirstThunk);
    if (ulOffset == -1 || (pThunkData = (TThunkDataT<T>*)((ulong64)m_pBase + ulOffset)) == nullptr) continue;
    do
    {
      if ((pThunkData->u1.AddressOfData & m_OrdinalFlag) == m_OrdinalFlag)   // Imported by ordinal
      {
        funcInfo.Name = "";
        funcInfo.Hint = -1;
        funcInfo.Ordinal = pThunkData->u1.AddressOfData & ~m_OrdinalFlag;
      }
      else   // Imported by name
      {
        ulOffset = this->RVA2Offset(pThunkData->u1.AddressOfData);
        PImportByName p = (PImportByName)((ulong64)m_pBase + ulOffset);
        if (ulOffset != -1 && p != nullptr)
        {
          funcInfo.Hint = p->Hint;
          funcInfo.Ordinal = T(-1);
          funcInfo.Name = (char*)p->Name;
        }
      }

      funcInfo.IIDID = e.IIDID;
      funcInfo.RVA = pThunkData->u1.AddressOfData;
      m_FunctionInfoList.push_back(funcInfo);

      pThunkData++;
    }
    while (pThunkData->u1.AddressOfData != 0);
  }

  return m_FunctionInfoList;
}

template<typename T>
TDLLInfo vuapi CPEFileTX<T>::FindImportedDLL(const std::string& DLLName)
{
  TDLLInfo o = {0};

  if (m_ExIDDList.size() == 0)
  {
    this->GetDLLInfoList();
  }

  if (m_ExIDDList.size() == 0)
  {
    return o;
  }

  auto s1 = UpperStringA(DLLName);

  for (const auto& e: m_ExIDDList)
  {
    auto s2 = UpperStringA(e.Name);
    if (s1 == s2)
    {
      o.IIDID = e.IIDID;
      o.Name = e.Name;
      break;
    }
  }

  return o;
}

template<typename T>
TFunctionInfoT<T> vuapi CPEFileTX<T>::FindImportedFunction(
  const TFunctionInfoT<T>& FunctionInfo,
  eImportedFunctionFindMethod Method
)
{
  TFunctionInfoT<T> o = {0};

  if (m_FunctionInfoList.size() == 0)
  {
    this->GetFunctionInfoList();
  }

  if (m_FunctionInfoList.size() == 0)
  {
    return o;
  }

  switch (Method)
  {
  case eImportedFunctionFindMethod::IFFM_HINT:
    for (const auto& e: m_FunctionInfoList)
    {
      if (e.Hint == FunctionInfo.Hint)
      {
        o = e;
        break;
      }
    }
    break;
  case eImportedFunctionFindMethod::IFFM_NAME:
    for (const auto& e: m_FunctionInfoList)
    {
      if (e.Name == FunctionInfo.Name)
      {
        o = e;
        break;
      }
    }
    break;
  default:
    break;
  }

  return o;
}

template<typename T>
TFunctionInfoT<T> vuapi CPEFileTX<T>::FindImportedFunction(const std::string& FunctionName)
{
  TFunctionInfoT<T> o = {0};
  o.Name = FunctionName;
  return this->FindImportedFunction(o, eImportedFunctionFindMethod::IFFM_NAME);
}

template<typename T>
TFunctionInfoT<T> vuapi CPEFileTX<T>::FindImportedFunction(const ushort FunctionHint)
{
  TFunctionInfoT<T> o = {0};
  o.Hint = FunctionHint;
  return this->FindImportedFunction(o, eImportedFunctionFindMethod::IFFM_HINT);
}

template<typename T>
T vuapi CPEFileTX<T>::RVA2Offset(T RVA)
{
  if (!m_Initialized)
  {
    return T(-1);
  }

  if (m_SectionHeaderList.size() == 0)
  {
    this->GetSetionHeaderList();
  }

  if (m_SectionHeaderList.size() == 0)
  {
    return T(-1);
  }

  const auto& theLastSection = *m_SectionHeaderList.rbegin();

  std::pair<T, T> range(T(0), T(theLastSection->VirtualAddress) + T(theLastSection->Misc.VirtualSize));
  if (RVA < range.first || RVA > range.second)
  {
    return T(-1);
  }

  T result = RVA;

  for (auto& e : m_SectionHeaderList)
  {
    if ((RVA >= e->VirtualAddress) && (RVA < (e->VirtualAddress + e->Misc.VirtualSize)))
    {
      result  = e->PointerToRawData;
      result += (RVA - e->VirtualAddress);
      break;
    }
  }

  return result;
}

template<typename T>
T vuapi CPEFileTX<T>::Offset2RVA(T Offset)
{
  if (!m_Initialized)
  {
    return (-1);
  }

  if (m_SectionHeaderList.size() == 0)
  {
    this->GetSetionHeaderList();
  }

  if (m_SectionHeaderList.size() == 0)
  {
    return T(-1);
  }

  const auto& theLastSection = *m_SectionHeaderList.rbegin();

  std::pair<T, T> range(T(0), T(theLastSection->PointerToRawData) + T(theLastSection->SizeOfRawData));
  if (Offset < range.first || Offset > range.second)
  {
    return T(-1);
  }

  T result = Offset;

  for (auto& e : m_SectionHeaderList)
  {
    if ((Offset >= e->PointerToRawData) && (Offset < (e->PointerToRawData + e->SizeOfRawData)))
    {
      result  = e->VirtualAddress;
      result += (Offset - e->PointerToRawData);
      break;
    }
  }

  return result;
}

template class CPEFileTA<ulong32>;
template class CPEFileTA<ulong64>;

template<typename T>
CPEFileTA<T>::CPEFileTA(const std::string& PEFilePath)
{
  CPEFileTX<T>::m_Initialized = false;

  CPEFileTX<T>::m_pBase = nullptr;
  CPEFileTX<T>::m_pDosHeader = nullptr;
  CPEFileTX<T>::m_pPEHeader  = nullptr;

  m_FilePath = PEFilePath;
}

template<typename T>
CPEFileTA<T>::~CPEFileTA()
{
  if (CPEFileTX<T>::m_Initialized)
  {
    m_FileMap.Close();
  }
}

template<typename T>
VUResult vuapi CPEFileTA<T>::Parse(const std::string& PEFilePath)
{
  if (PEFilePath.length() != 0)
  {
    m_FilePath = PEFilePath;
  }

  if (m_FilePath.length() == 0)
  {
    return 1;
  }

  if (!IsFileExistsA(m_FilePath))
  {
    return 2;
  }

  if (m_FileMap.Init(true, m_FilePath) != vu::VU_OK)
  {
    return 3;
  }

  if (m_FileMap.Create("", m_FileMap.GetFileSize()) != vu::VU_OK)
  {
    return 4;
  }

  CPEFileTX<T>::m_pBase = m_FileMap.View();
  if (CPEFileTX<T>::m_pBase == nullptr)
  {
    return 5;
  }

  CPEFileTX<T>::m_pDosHeader = (PDosHeader)CPEFileTX<T>::m_pBase;
  if (CPEFileTX<T>::m_pDosHeader == nullptr)
  {
    return 6;
  }

  CPEFileTX<T>::m_pPEHeader = (TPEHeaderT<T>*)((ulong64)CPEFileTX<T>::m_pBase + CPEFileTX<T>::m_pDosHeader->e_lfanew);
  if (CPEFileTX<T>::m_pPEHeader == nullptr)
  {
    return 7;
  }

  if (sizeof(T) == sizeof(pe32))
  {
    if (CPEFileTX<T>::m_pPEHeader->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
      return 8; // Used wrong type data for the current PE file
    }
  }
  else if (sizeof(T) == sizeof(pe64))
  {
    if (CPEFileTX<T>::m_pPEHeader->Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
      return 8; // Used wrong type data for the current PE file
    }
  }
  else
  {
    return 9; // The curent type data was not supported
  }

  CPEFileTX<T>::m_Initialized = true;

  return VU_OK;
}

template class CPEFileTW<ulong32>;
template class CPEFileTW<ulong64>;

template<typename T>
CPEFileTW<T>::CPEFileTW(const std::wstring& PEFilePath)
{
  CPEFileTX<T>::m_Initialized = false;

  CPEFileTX<T>::m_pBase = nullptr;
  CPEFileTX<T>::m_pDosHeader = nullptr;
  CPEFileTX<T>::m_pPEHeader  = nullptr;

  m_FilePath = PEFilePath;
}

template<typename T>
CPEFileTW<T>::~CPEFileTW()
{
  if (CPEFileTX<T>::m_Initialized)
  {
    m_FileMap.Close();
  }
}

template<typename T>
VUResult vuapi CPEFileTW<T>::Parse(const std::wstring& PEFilePath)
{
  if (PEFilePath.length() != 0)
  {
    m_FilePath = PEFilePath;
  }

  if (m_FilePath.length() == 0)
  {
    return 1;
  }

  if (!IsFileExistsW(m_FilePath))
  {
    return 2;
  }

  if (m_FileMap.Init(true, m_FilePath) != vu::VU_OK)
  {
    return 3;
  }

  if (m_FileMap.Create(L"", m_FileMap.GetFileSize()) != vu::VU_OK)
  {
    return 4;
  }

  CPEFileTX<T>::m_pBase = m_FileMap.View();
  if (CPEFileTX<T>::m_pBase == nullptr)
  {
    return 5;
  }

  CPEFileTX<T>::m_pDosHeader = (PDosHeader)CPEFileTX<T>::m_pBase;
  if (CPEFileTX<T>::m_pDosHeader == nullptr)
  {
    return 6;
  }

  CPEFileTX<T>::m_pPEHeader = (TPEHeaderT<T>*)((ulong64)CPEFileTX<T>::m_pBase + CPEFileTX<T>::m_pDosHeader->e_lfanew);
  if (CPEFileTX<T>::m_pPEHeader == nullptr)
  {
    return 7;
  }

  if (sizeof(T) == sizeof(pe32))
  {
    if (CPEFileTX<T>::m_pPEHeader->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
      return 8; // Used wrong type data for the current PE file
    }
  }
  else if (sizeof(T) == sizeof(pe64))
  {
    if (CPEFileTX<T>::m_pPEHeader->Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
      return 8; // Used wrong type data for the current PE file
    }
  }
  else
  {
    return 9; // The curent type data was not supported
  }

  CPEFileTX<T>::m_Initialized = true;

  return VU_OK;
}

} // namespace vu