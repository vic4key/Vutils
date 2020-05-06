/**
 * @file   process.cpp
 * @author Vic P.
 * @brief  Implementation for Process
 */

#include "Vutils.h"
#include "lazy.h"

#include <cassert>
#include <cmath>
#include <thread>

#include <tlhelp32.h>

namespace vu
{

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

eWow64 vuapi IsWow64(const ulong ulPID)
{
  HANDLE hProcess = NULL;

  if (ulPID != (ulong)-1)
  {
    SetPrivilege(SE_DEBUG_NAME, true);
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
    SetPrivilege(SE_DEBUG_NAME, false);
  }
  else
  {
    hProcess = GetCurrentProcess();
  }

  auto result = IsWow64(hProcess);

  CloseHandle(hProcess);

  return result;
}

eWow64 vuapi IsWow64(const HANDLE hProcess)
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

  BOOL bWow64 = false;
  if (!pfnIsWow64Process(hProcess, &bWow64))
  {
    return WOW64_ERROR;
  }

  return (bWow64 ? WOW64_YES : WOW64_NO);
}

bool vuapi RPM(const HANDLE hProcess, const void* lpAddress, void* lpBuffer, const SIZE_T ulSize, const bool force)
{
  ulong  ulOldProtect = 0;
  SIZE_T ulRead = 0;

  if (force) VirtualProtectEx(hProcess, LPVOID(lpAddress), ulSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

  auto ret = ReadProcessMemory(hProcess, lpAddress, lpBuffer, ulSize, &ulRead);
  const auto theLastError = ret != FALSE ? ERROR_SUCCESS : GetLastError();

  if (force) VirtualProtectEx(hProcess, LPVOID(lpAddress), ulSize, ulOldProtect, &ulOldProtect);

  SetLastError(theLastError);
  return ulRead == ulSize;
}

bool vuapi RPMEX(
  const eXBit bit,
  const HANDLE Handle,
  const void* lpAddress,
  void* lpBuffer,
  const SIZE_T ulSize,
  const bool force,
  const SIZE_T nOffsets,
  ...)
{
  va_list args;
  va_start(args, nOffsets);
  std::vector<vu::ulong> offsets;
  for (size_t i = 0; i < nOffsets; i++) offsets.push_back(va_arg(args, vu::ulong));
  va_end(args);

  bool result = true;

  if (offsets.empty())
  {
    result = RPM(Handle, LPVOID(lpAddress), lpBuffer, ulSize, force);
  }
  else
  {
    auto Address = ulonglong(lpAddress);

    for (size_t i = 0; i < nOffsets; i++)
    {
      bool isoffset = i < nOffsets - 1;
      result &= RPM(
        Handle,
        LPCVOID(Address + offsets.at(i)),
        isoffset ? LPVOID(&Address) : lpBuffer,
        isoffset ? bit : ulSize,
        force
      );
      if (!result) break;
    }
  }

  return result;
}

bool vuapi WPM(
  const HANDLE hProcess,
  const void* lpAddress,
  const void* lpcBuffer,
  const SIZE_T ulSize,
  const bool force)
{
  ulong ulOldProtect = 0;
  SIZE_T ulWritten = 0;

  if (force) VirtualProtectEx(hProcess, LPVOID(lpAddress), ulSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

  auto ret = WriteProcessMemory(hProcess, LPVOID(lpAddress), lpcBuffer, ulSize, &ulWritten);
  const auto theLastError = ret != FALSE ? ERROR_SUCCESS : GetLastError();

  if (force) VirtualProtectEx(hProcess, LPVOID(lpAddress), ulSize, ulOldProtect, &ulOldProtect);

  SetLastError(theLastError);
  return ulWritten == ulSize;
}

bool vuapi WPMEX(
  const eXBit bit,
  const HANDLE Handle,
  const void* lpAddress,
  const void* lpBuffer,
  const SIZE_T ulSize,
  const bool force,
  const SIZE_T nOffsets,
  ...)
{
  va_list args;
  va_start(args, nOffsets);
  std::vector<ulong> offsets;
  for (size_t i = 0; i < nOffsets; i++) offsets.push_back(va_arg(args, ulong));
  va_end(args);

  bool result = true;

  if (offsets.empty())
  {
    result = WPM(Handle, LPVOID(lpAddress), lpBuffer, ulSize, force);
  }
  else
  {
    auto Address = ulonglong(lpAddress);

    for (size_t i = 0; i < nOffsets; i++)
    {
      bool isoffset = i < nOffsets - 1;
      if (isoffset)
      {
        result &= RPM(
          Handle, LPCVOID(Address + offsets.at(i)), LPVOID(&Address), bit, force);
      }
      else
      {
        result &= WPM(
          Handle, LPCVOID(Address + offsets.at(i)), lpBuffer, ulSize, force);
      }
      if (!result) break;
    }
  }

  return result;
}

ulong vuapi GetParentPID(ulong ulChildPID)
{
  if (InitTlHelp32() != VU_OK)
  {
    return (ulong)-1;
  }

  auto hSnapshot = pfnCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
  {
    return (ulong)-1;
  }

  ulong result = -1;

  TProcessEntry32A pe = { 0 };
  pe.dwSize = sizeof(TProcessEntry32A);

  auto nextale = pfnProcess32FirstA(hSnapshot, &pe);
  while (nextale)
  {
    if (pe.th32ProcessID == ulChildPID)
    {
      result = pe.th32ParentProcessID;
      break;
    }

    nextale = pfnProcess32NextA(hSnapshot, &pe);
  }

  CloseHandle(hSnapshot);

  return result;
}

ulong vuapi GetMainThreadID(ulong ulPID)
{
  auto hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (hSnap == INVALID_HANDLE_VALUE)
  {
    return -1;
  }

  ulong result = -1;

  ::THREADENTRY32 te = { 0 };
  te.dwSize = sizeof(THREADENTRY32);

  auto nextable = Thread32First(hSnap, &te);
  while (nextable)
  {
    if (te.th32OwnerProcessID == ulPID)
    {
      result = te.th32ThreadID;
      break;
    }

    nextable = Thread32Next(hSnap, &te);
  }

  CloseHandle(hSnap);

  return result;
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

  ulong ulPID;
  for (vu::ulong i = 0; i < nProcesses; i++)
  {
    ulPID = pProcesses.get()[i];

    s2.clear();
    s2 = LowerStringA(vu::PIDToNameA(ulPID));
    if (s1 == s2)
    {
      l.push_back(ulPID);
    }
  }

  SetLastError(ERROR_SUCCESS);

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

  ulong ulPID;
  for (vu::ulong i = 0; i < nProcesses; i++)
  {
    ulPID = pProcesses.get()[i];

    s2.clear();
    s2 = LowerStringW(vu::PIDToNameW(ulPID));
    if (s1 == s2)
    {
      l.push_back(ulPID);
    }
  }

  SetLastError(ERROR_SUCCESS);

  return l;
}

std::string vuapi PIDToNameA(ulong ulPID)
{
  std::string s;
  s.clear();

  if (InitTlHelp32() != VU_OK)
  {
    return s;
  }

  SetPrivilege(SE_DEBUG_NAME, true);
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
  SetPrivilege(SE_DEBUG_NAME, false);
  if (!hProcess)
  {
    return s;
  }

  std::unique_ptr<char[]> szProcessPath(new char [MAXPATH]);
  ZeroMemory(szProcessPath.get(), MAXPATH);

  ulong ulPathLength = MAXPATH;

  BOOL ret = pfnQueryFullProcessImageNameA(hProcess, 0, szProcessPath.get(), &ulPathLength);

  CloseHandle(hProcess);

  if (ret == FALSE)
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

  if (InitTlHelp32() != VU_OK)
  {
    return s;
  }

  SetPrivilege(SE_DEBUG_NAME, true);
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
  SetPrivilege(SE_DEBUG_NAME, false);
  if (hProcess == nullptr)
  {
    return s;
  }

  std::unique_ptr<wchar[]> wszProcessPath(new wchar [MAXPATH]);
  ZeroMemory(wszProcessPath.get(), 2*MAXBYTE);

  ulong ulPathLength = 2*MAXPATH;

  BOOL ret = pfnQueryFullProcessImageNameW(hProcess, 0, wszProcessPath.get(), &ulPathLength);

  CloseHandle(hProcess);

  if (ret == FALSE)
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

  SetPrivilege(SE_DEBUG_NAME, true);
  auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ulPID);
  SetPrivilege(SE_DEBUG_NAME, false);
  if (hProcess == nullptr)
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

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

VUResult vuapi InjectDLLA(ulong ulPID, const std::string& DLLFilePath, bool WaitLoadingDLL)
{
  if (ulPID == 0 || ulPID == -1)
  {
    return 1;
  }

  if (!IsFileExistsA(DLLFilePath))
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return 2;
  }

  auto DLLFileName = ExtractFileNameA(DLLFilePath);
  if (DLLFileName.empty())
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return 2;
  }

  if (RemoteGetModuleHandleA(ulPID, DLLFileName) != 0)
  {
    SetLastError(ERROR_ALREADY_EXISTS);
    return 3;
  }

  SetPrivilege(SE_DEBUG_NAME, true);
  std::shared_ptr<void> hp(OpenProcess(CREATE_THREAD_ACCESS, FALSE, ulPID), CloseHandle);
  SetPrivilege(SE_DEBUG_NAME, false);
  if (hp.get() == nullptr)
  {
    return 4;
  }

  /**
   * TODO: Should get the function address in the target process, not this process like this.
   * Solution: Get the function VA + process base address of the target process.
   */
  typedef HMODULE(WINAPI* PfnLoadLibraryA)(LPCSTR lpLibFileName);
  PfnLoadLibraryA pfnLoadLibraryA = VU_GET_API(kernel32.dll, LoadLibraryA);
  if (pfnLoadLibraryA == nullptr)
  {
    return 5;
  }

  auto pBlock = VirtualAllocEx(
    hp.get(),
    nullptr,
    (DLLFilePath.length() + 1) * sizeof(char), // +1 for a null-terminated ANSI string
    MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
  );
  if (pBlock == nullptr)
  {
    return 6;
  }

  if (!WPM(hp.get(), pBlock, DLLFilePath.c_str(), DLLFilePath.length() * sizeof(char)))
  {
    return 7;
  }

  auto hThread = CreateRemoteThread(
    hp.get(),
    nullptr, 0,
    (LPTHREAD_START_ROUTINE)pfnLoadLibraryA,
    pBlock,
    0, nullptr
  );
  if (hThread == nullptr)
  {
    return 8;
  }

  if (WaitLoadingDLL)
  {
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    VirtualFreeEx(hp.get(), pBlock, 0, MEM_RELEASE);
  }

  SetLastError(ERROR_SUCCESS);

  return 0;
}

VUResult vuapi InjectDLLW(ulong ulPID, const std::wstring& DLLFilePath, bool WaitLoadingDLL)
{
  if (ulPID == 0 || ulPID == -1)
  {
    return 1;
  }

  if (!IsFileExistsW(DLLFilePath))
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return 2;
  }

  auto DLLFileName = ExtractFileNameW(DLLFilePath);
  if (DLLFileName.empty())
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return 2;
  }

  if (RemoteGetModuleHandleW(ulPID, DLLFileName) != 0)
  {
    SetLastError(ERROR_ALREADY_EXISTS);
    return 3;
  }

  SetPrivilege(SE_DEBUG_NAME, true);
  std::shared_ptr<void> hp(OpenProcess(CREATE_THREAD_ACCESS, FALSE, ulPID), CloseHandle);
  SetPrivilege(SE_DEBUG_NAME, false);
  if (hp.get() == nullptr)
  {
    return 4;
  }

  /**
   * TODO: Should get the function address in the target process, not this process like this.
   * Solution: Get the function VA + process base address of the target process.
   */
  typedef HMODULE(WINAPI* PfnLoadLibraryW)(LPCWSTR lpLibFileName);
  PfnLoadLibraryW pfnLoadLibraryW = VU_GET_API(kernel32.dll, LoadLibraryW);
  if (pfnLoadLibraryW == nullptr)
  {
    return 5;
  }

  auto pBlock = VirtualAllocEx(
    hp.get(),
    nullptr,
    (DLLFilePath.length() + 1) * sizeof(wchar_t), // +1 for a null-terminated UNICODE string
    MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
  );
  if (pBlock == nullptr)
  {
    return 6;
  }

  if (!WPM(hp.get(), pBlock, DLLFilePath.c_str(), DLLFilePath.length() * sizeof(wchar_t)))
  {
    return 7;
  }

  auto hThread = CreateRemoteThread(
    hp.get(),
    nullptr, 0,
    (LPTHREAD_START_ROUTINE)pfnLoadLibraryW,
    pBlock,
    0, nullptr
  );
  if (hThread == nullptr)
  {
    return 8;
  }

  if (WaitLoadingDLL)
  {
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    VirtualFreeEx(hp.get(), pBlock, 0, MEM_RELEASE);
  }

  SetLastError(ERROR_SUCCESS);

  return 0;
}

/**
 * CProcess
 */

CProcess::CProcess()
  : m_PID(INVALID_PID_VALUE)
  , m_Handle(nullptr)
  , m_Wow64(eWow64::WOW64_ERROR)
  , m_Bit(eXBit::x86)
  , m_Name(L"")
  , m_LastSystemTimeUTC(0)
  , m_LastSystemTimePerCoreUTC(0)
{
}

CProcess::CProcess(const vu::ulong PID)
  : m_PID(PID)
  , m_Handle(nullptr)
  , m_Wow64(eWow64::WOW64_ERROR)
  , m_Bit(eXBit::x86)
  , m_Name(L"")
  , m_LastSystemTimeUTC(0)
  , m_LastSystemTimePerCoreUTC(0)
{
  Attach(m_PID);
}

CProcess::~CProcess()
{
  Close(m_Handle);
}

CProcess& CProcess::operator=(const CProcess& right)
{
  m_PID = right.m_PID;
  m_Handle = right.m_Handle;
  m_Wow64 = right.m_Wow64;
  m_Bit = right.m_Bit;
  m_Name = right.m_Name;
  return *this;
}

bool CProcess::operator==(const CProcess& right)
{
  bool result = true;
  result &= m_PID == right.m_PID;
  result &= m_Handle == right.m_Handle;
  result &= m_Wow64 == right.m_Wow64;
  result &= m_Bit == right.m_Bit;
  result &= m_Name == right.m_Name;
  return result;
}

bool CProcess::operator!=(const CProcess& right)
{
  return !(*this == right);
}

std::ostream& operator<<(std::ostream& os, const CProcess& process)
{
  assert(0);
  return os;
}

const vu::ulong CProcess::PID() const
{
  return m_PID;
}

const HANDLE CProcess::Handle() const
{
  return m_Handle;
}

const std::wstring& CProcess::Name() const
{
  return m_Name;
}

const vu::eWow64 CProcess::Wow64() const
{
  return m_Wow64;
}

const vu::eXBit CProcess::Bits() const
{
  return m_Bit;
}

bool CProcess::Is64Bits(HANDLE Handle)
{
  if (Handle == nullptr)
  {
    Handle = GetCurrentProcess();
  }

  CProcessX process;
  process.Attach(Handle);
  assert(process.Ready());

  return process.Bits() == eXBit::x64;
}

bool CProcess::Is64Bits(ulong PID)
{
  if (PID == NULL)
  {
    PID = GetCurrentProcessId();
  }

  CProcessX process;
  process.Attach(PID);
  assert(process.Ready());

  return process.Bits() == eXBit::x64;
}

bool CProcess::IsWow64(HANDLE Handle)
{
  if (Handle == nullptr)
  {
    Handle = GetCurrentProcess();
  }

  CProcessX process;
  process.Attach(Handle);
  assert(process.Ready());

  return process.Wow64() == eWow64::WOW64_YES;
}

bool CProcess::IsWow64(ulong PID)
{
  if (PID == NULL)
  {
    PID = GetCurrentProcessId();
  }

  CProcessX process;
  process.Attach(PID);
  assert(process.Ready());

  return process.Wow64() == eWow64::WOW64_YES;
}

bool CProcessX::Ready()
{
  return m_Handle != nullptr;
}

bool CProcess::Attach(const ulong PID)
{
  if (PID == INVALID_PID_VALUE)
  {
    return false;
  }

  auto Handle = Open(PID);
  if (Handle == nullptr)
  {
    return false;
  }

  return Attach(Handle);
}

bool CProcess::Attach(const HANDLE Handle)
{
  if (Handle == nullptr)
  {
    return false;
  }

  auto PID = GetProcessId(Handle);
  if (PID == INVALID_PID_VALUE)
  {
    return false;
  }

  m_PID = PID;
  m_Handle = Handle;

  SetLastError(ERROR_SUCCESS);

  Parse();

  return true;
}

HANDLE CProcess::Open(const ulong PID)
{
  if (PID == INVALID_PID_VALUE)
  {
    return nullptr;
  }

  SetPrivilege(SE_DEBUG_NAME, true);
  SetLastError(ERROR_SUCCESS);

  auto result = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
  if (result == nullptr)
  {
    result = nullptr;
  }

  m_LastErrorCode = GetLastError();

  SetPrivilege(SE_DEBUG_NAME, false);
  SetLastError(ERROR_SUCCESS);

  return result;
}

bool CProcess::Close(const HANDLE Handle)
{
  if (Handle == nullptr)
  {
    return false;
  }

  return CloseHandle(m_Handle) != FALSE;
}

void CProcess::Parse()
{
  m_Wow64 = vu::IsWow64(m_Handle);

  if (GetProcessorArchitecture() == vu::PA_X64)
  {
    m_Bit = m_Wow64 == eWow64::WOW64_YES ? eXBit::x86 : eXBit::x64;
  }
  else
  {
    m_Bit = eXBit::x86;
  }

  m_Name = PIDToNameW(m_PID);
}

bool CProcess::Read(const ulongptr Address, CBinary& Data)
{
  if (Address == 0 || Data.GetSize() == 0)
  {
    return false;
  }

  return Read(Address, Data.GetpData(), Data.GetSize());
}

bool CProcess::Read(const ulongptr Address, void* pData, const ulong ulSize)
{
  if (Address == 0 || pData == 0 || ulSize == 0)
  {
    return false;
  }

  auto result = RPM(m_Handle, LPCVOID(Address), pData, ulSize, true);

  m_LastErrorCode = GetLastError();

  return result;
}

bool CProcess::Write(const ulongptr Address, const CBinary& Data)
{
  return Write(Address, Data.GetpData(), Data.GetSize());
}

bool CProcess::Write(const ulongptr Address, const void* pData, const ulong ulSize)
{
  if (Address == 0 || pData == 0 || ulSize == 0)
  {
    return false;
  }

  auto result = WPM(m_Handle, LPCVOID(Address), pData, ulSize, true);

  m_LastErrorCode = GetLastError();

  return result;
}

double CProcess::GetCPUPercentUsage()
{
  const auto FileTimeToUTC = [](const FILETIME * FileTime) -> uint64_t
  {
    LARGE_INTEGER li;
    li.LowPart  = FileTime->dwLowDateTime;
    li.HighPart = FileTime->dwHighDateTime;
    return li.QuadPart;
  };

  FILETIME SystemTime = { 0 };
  GetSystemTimeAsFileTime(&SystemTime);
  const int64_t SystemTimeUTC = FileTimeToUTC(&SystemTime);

  const auto Time = GetTimeInformation();

  auto nCores = std::thread::hardware_concurrency();
  const int64_t SystemTimePerCoreUTC = (FileTimeToUTC(&Time.KernelTime) + FileTimeToUTC(&Time.UserTime)) / nCores;

  if (m_LastSystemTimePerCoreUTC == 0 || m_LastSystemTimeUTC == 0)
  {
    m_LastSystemTimePerCoreUTC = SystemTimePerCoreUTC;
    m_LastSystemTimeUTC = SystemTimeUTC;
    return 0.; // GetCPUPercentUsage();
  }

  const int64_t SystemTimeDeltaPerCoreUTC = SystemTimePerCoreUTC - m_LastSystemTimePerCoreUTC;
  const int64_t SystemTimeDeltaUTC = SystemTimeUTC - m_LastSystemTimeUTC;

  if (SystemTimeDeltaUTC == 0)
  {
    return 0.; // GetCPUPercentUsage();
  }

  double result = (SystemTimeDeltaPerCoreUTC * 100. + SystemTimeDeltaUTC / 2.) / SystemTimeDeltaUTC;

  m_LastSystemTimePerCoreUTC = SystemTimePerCoreUTC;
  m_LastSystemTimeUTC = SystemTimeUTC;

  return result;
};

PROCESS_CPU_COUNTERS CProcess::GetCPUInformation(const double interval)
{
  PROCESS_CPU_COUNTERS result = { 0 };

  const int DEF_PART_MS = 200; // millisecond

  const auto ToMiliseconds = [](const double second) -> double
  {
    return second * 1000.; // millisecond
  };

  result.Usage = 0.;

  const int nCount = int(std::ceil(ToMiliseconds(interval) / DEF_PART_MS));

  for (int i = 0; i < nCount; i++)
  {
    const double Usage = GetCPUPercentUsage();
    result.Usage = Usage > result.Usage ? Usage : result.Usage;
    // result.Usage = Usage;
    Sleep(DEF_PART_MS);
  }

  return result;
}

PROCESS_MEMORY_COUNTERS CProcess::GetMemoryInformation()
{
  PROCESS_MEMORY_COUNTERS result = { 0 };

  if (InitTlHelp32() != VU_OK)
  {
    return result;
  }

  pfnGetProcessMemoryInfo(m_Handle, &result, sizeof(result));

  return result;
}

PROCESS_TIME_COUNTERS CProcess::GetTimeInformation()
{
  PROCESS_TIME_COUNTERS result = { 0 };

  GetProcessTimes(
    m_Handle,
    &result.CreationTime,
    &result.ExitTime,
    &result.KernelTime,
    &result.UserTime);

  return result;
}

PROCESS_IO_COUNTERS CProcess::GetIOInformation()
{
  PROCESS_IO_COUNTERS result = { 0 };

  GetProcessIoCounters(m_Handle, &result);

  return result;
}

const CProcess::Threads& CProcess::GetThreads()
{
  m_Threads.clear();

  if (m_PID == INVALID_PID_VALUE)
  {
    return m_Threads;
  }

  auto hSnapThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (hSnapThread == INVALID_HANDLE_VALUE)
  {
    return m_Threads;
  }

  ::THREADENTRY32 te32 = { 0 };
  te32.dwSize = sizeof(THREADENTRY32);

  if (Thread32First(hSnapThread, &te32))
  {
    do
    {
      if (te32.th32OwnerProcessID == m_PID)
      {
        m_Threads.emplace_back(*reinterpret_cast<vu::THREADENTRY32*>(&te32));
      }
    } while (Thread32Next(hSnapThread, &te32));
  }

  CloseHandle(hSnapThread);

  return m_Threads;
}

#pragma push_macro("MODULEENTRY32")
#pragma push_macro("Module32First")
#pragma push_macro("Module32Next")
#undef MODULEENTRY32
#undef Module32First
#undef Module32Next

const CProcess::Modules& CProcess::GetModules()
{
  m_Modules.clear();

  if (m_PID == INVALID_PID_VALUE)
  {
    return m_Modules;
  }

  ulong   cbNeeded = 0;
  HMODULE hModules[MAX_NMODULES] = { 0 };
  pfnEnumProcessModulesEx(m_Handle, hModules, sizeof(hModules), &cbNeeded, LIST_MODULES_ALL);

  char s[MAX_PATH] = { 0 };

  for (auto& hModule : hModules)
  {
    if (hModule == nullptr)
    {
      break;
    }

    MODULEENTRY32 me32 = { 0 };
    me32.dwSize  = sizeof(me32);
    me32.hModule = hModule;
    me32.th32ProcessID = m_PID;

    ZeroMemory(s, sizeof(s));
    pfnGetModuleBaseNameA(m_Handle, hModule, s, sizeof(s));
    strcpy_s(me32.szModule, s);

    ZeroMemory(s, sizeof(s));
    pfnGetModuleFileNameExA(m_Handle, hModule, s, sizeof(s));
    strcpy_s(me32.szExePath, s);

    MODULEINFO_PTR mi = { 0 };
    pfnGetModuleInformation(m_Handle, hModule, &mi, sizeof(mi));
    me32.modBaseAddr = reinterpret_cast<BYTE*>(mi.lpBaseOfDll);
    me32.modBaseSize = static_cast<DWORD>(mi.SizeOfImage);

    m_Modules.emplace_back(*reinterpret_cast<vu::MODULEENTRY32*>(&me32));
  }

  return m_Modules;
}

#pragma pop_macro("Module32Next")
#pragma pop_macro("Module32First")
#pragma pop_macro("MODULEENTRY32")

} // namespace vu