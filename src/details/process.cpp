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

#include "defs.h"

namespace vu
{

eProcessorArchitecture get_processor_architecture()
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

eWow64 vuapi is_wow64(const ulong pid)
{
  HANDLE hp = NULL;

  if (pid != INVALID_PID_VALUE)
  {
    set_privilege(SE_DEBUG_NAME, true);
    hp = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    set_privilege(SE_DEBUG_NAME, false);
  }
  else
  {
    hp = GetCurrentProcess();
  }

  auto result = is_wow64(hp);

  CloseHandle(hp);

  return result;
}

eWow64 vuapi is_wow64(const HANDLE hp)
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

  BOOL wow64 = false;
  if (!pfnIsWow64Process(hp, &wow64))
  {
    return WOW64_ERROR;
  }

  return (wow64 ? WOW64_YES : WOW64_NO);
}

bool vuapi rpm(const HANDLE hp, const void* address, void* buffer, const SIZE_T size, const bool force)
{
  ulong  old_protect = 0;
  if (force) VirtualProtectEx(hp, LPVOID(address), size, PAGE_EXECUTE_READWRITE, &old_protect);

  SIZE_T num_read_bytes = 0;
  auto ret = ReadProcessMemory(hp, address, buffer, size, &num_read_bytes);
  const auto last_error = ret != FALSE ? ERROR_SUCCESS : GetLastError();

  if (force) VirtualProtectEx(hp, LPVOID(address), size, old_protect, &old_protect);

  SetLastError(last_error);
  return num_read_bytes == size;
}

bool vuapi rpm_ex(const eXBit bit, const HANDLE hp, const void* address, void* buffer, const SIZE_T size, const bool force, const SIZE_T n_offsets, ...)
{
  va_list args;
  va_start(args, n_offsets);
  std::vector<vu::ulong> offsets;
  for (size_t i = 0; i < n_offsets; i++) offsets.push_back(va_arg(args, vu::ulong));
  va_end(args);

  bool result = true;

  if (offsets.empty())
  {
    result = rpm(hp, LPVOID(address), buffer, size, force);
  }
  else
  {
    for (size_t i = 0; i < n_offsets; i++)
    {
      bool is_offset = i < n_offsets - 1;
      result &= rpm(hp, LPCVOID(ulonglong(address) + offsets.at(i)), is_offset ? LPVOID(&address) : buffer, is_offset ? bit : size, force);
      if (!result) break;
    }
  }

  return result;
}

bool vuapi wpm(const HANDLE hp, const void* address, const void* buffer, const SIZE_T size, const bool force)
{

  ulong old_protect = 0;
  if (force) VirtualProtectEx(hp, LPVOID(address), size, PAGE_EXECUTE_READWRITE, &old_protect);

  SIZE_T n_written_bytes = 0;
  auto ret = WriteProcessMemory(hp, LPVOID(address), buffer, size, &n_written_bytes);
  const auto last_error = ret != FALSE ? ERROR_SUCCESS : GetLastError();

  if (force) VirtualProtectEx(hp, LPVOID(address), size, old_protect, &old_protect);

  SetLastError(last_error);
  return n_written_bytes == size;
}

bool vuapi wpm_ex(const eXBit bit, const HANDLE hp, const void* address, const void* buffer, const SIZE_T size, const bool force, const SIZE_T n_offsets, ...)
{
  va_list args;
  va_start(args, n_offsets);
  std::vector<ulong> offsets;
  for (size_t i = 0; i < n_offsets; i++) offsets.push_back(va_arg(args, ulong));
  va_end(args);

  bool result = true;

  if (offsets.empty())
  {
    result = wpm(hp, LPVOID(address), buffer, size, force);
  }
  else
  {
    for (size_t i = 0; i < n_offsets; i++)
    {
      bool is_offset = i < n_offsets - 1;
      result &= rpm(hp, LPCVOID(ulonglong(address) + offsets.at(i)), LPVOID(&address), is_offset ? bit : size, force);
      if (!result) break;
    }
  }

  return result;
}

ulong vuapi get_parent_pid(ulong child_pid)
{
  if (Initialize_DLL_LAZY() != VU_OK)
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
    if (pe.th32ProcessID == child_pid)
    {
      result = pe.th32ParentProcessID;
      break;
    }

    nextale = pfnProcess32NextA(hSnapshot, &pe);
  }

  CloseHandle(hSnapshot);

  return result;
}

ulong vuapi get_main_thread_id(ulong pid)
{
  auto hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (hSnap == INVALID_HANDLE_VALUE)
  {
    return -1;
  }

  ulong result = -1;

  ::THREADENTRY32 te = { 0 };
  te.dwSize = sizeof(THREADENTRY32);

  while (auto nextable = Thread32First(hSnap, &te))
  {
    if (te.th32OwnerProcessID == pid)
    {
      result = te.th32ThreadID;
      break;
    }

    nextable = Thread32Next(hSnap, &te);
  }

  CloseHandle(hSnap);

  return result;
}

std::vector<ulong> vuapi name_to_pid_A(const std::string& name)
{
  std::vector<ulong> l;
  l.clear();

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return l;
  }

  std::unique_ptr<ulong[]> ptr_processes(new ulong[MAX_NPROCESSES]);
  if (ptr_processes == nullptr)
  {
    return l;
  }

  ZeroMemory(ptr_processes.get(), MAX_NPROCESSES * sizeof(ulong));

  vu::ulong cb_needed = 0;
  pfnEnumProcesses(ptr_processes.get(), MAX_NPROCESSES * sizeof(vu::ulong), &cb_needed);

  if (cb_needed <= 0)
  {
    return l;
  }

  vu::ulong n_processes = cb_needed / sizeof(ulong);

  std::string s1 = lower_string_A(name), s2;

  for (vu::ulong i = 0; i < n_processes; i++)
  {
    ulong pid = ptr_processes.get()[i];

    s2.clear();
    s2 = lower_string_A(vu::pid_to_name_A(pid));
    if (s1 == s2)
    {
      l.push_back(pid);
    }
  }

  SetLastError(ERROR_SUCCESS);

  return l;
}

std::vector<ulong> vuapi name_to_pid_W(const std::wstring& name)
{
  std::vector<ulong> l;
  l.clear();

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return l;
  }

  std::unique_ptr<ulong[]> ptr_processes(new ulong[MAX_NPROCESSES]);
  if (ptr_processes == nullptr)
  {
    return l;
  }

  ZeroMemory(ptr_processes.get(), MAX_NPROCESSES * sizeof(ulong));

  vu::ulong cb_needed = 0;
  pfnEnumProcesses(ptr_processes.get(), MAX_NPROCESSES *sizeof(vu::ulong), &cb_needed);

  if (cb_needed <= 0)
  {
    return l;
  }

  vu::ulong n_processes = cb_needed / sizeof(ulong);

  std::wstring s1 = lower_string_W(name), s2;

  for (vu::ulong i = 0; i < n_processes; i++)
  {
    ulong pid = ptr_processes.get()[i];

    s2.clear();
    s2 = lower_string_W(vu::pid_to_name_W(pid));
    if (s1 == s2)
    {
      l.push_back(pid);
    }
  }

  SetLastError(ERROR_SUCCESS);

  return l;
}

std::string vuapi pid_to_name_A(ulong pid)
{
  std::string s;
  s.clear();

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return s;
  }

  set_privilege(SE_DEBUG_NAME, true);
  HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
  set_privilege(SE_DEBUG_NAME, false);
  if (hp == nullptr)
  {
    return s;
  }

  std::unique_ptr<char[]> ptr_process_path(new char [MAXPATH]);
  ZeroMemory(ptr_process_path.get(), MAXPATH);

  ulong process_path_length = MAXPATH;

  BOOL ret = pfnQueryFullProcessImageNameA(hp, 0, ptr_process_path.get(), &process_path_length);

  CloseHandle(hp);

  if (ret == FALSE)
  {
    return s;
  }

  s.assign(ptr_process_path.get());

  s = extract_file_name_A(s);

  return s;
}

std::wstring vuapi pid_to_name_W(ulong pid)
{
  std::wstring s;
  s.clear();

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return s;
  }

  set_privilege(SE_DEBUG_NAME, true);
  HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
  set_privilege(SE_DEBUG_NAME, false);
  if (hp == nullptr)
  {
    return s;
  }

  std::unique_ptr<wchar[]> ptr_process_path(new wchar [MAXPATH]);
  ZeroMemory(ptr_process_path.get(), 2*MAXBYTE);

  ulong process_path_length = 2*MAXPATH;

  BOOL ret = pfnQueryFullProcessImageNameW(hp, 0, ptr_process_path.get(), &process_path_length);

  CloseHandle(hp);

  if (ret == FALSE)
  {
    return s;
  }

  s.assign(ptr_process_path.get());

  s = extract_file_name_W(s);

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
  auto moduleName = to_string_A(ModuleName);
  return Remote32GetModuleHandleA(ulPID, moduleName);
}

HMODULE vuapi Remote64GetModuleHandleA(const ulong ulPID, const std::string& ModuleName)
{
  HMODULE result = NULL;

  set_privilege(SE_DEBUG_NAME, true);
  auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ulPID);
  set_privilege(SE_DEBUG_NAME, false);
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

  auto targetName = lower_string_A(ModuleName);
  targetName = trim_string_A(targetName);

  std::string iterName("");

  char moduleName[MAX_PATH] = {0};
  for (ulong i = 0; i < nModules; i++)
  {
    pfnGetModuleBaseNameA(hProcess, hModules[i], moduleName, sizeof(ModuleName));
    iterName = lower_string_A(moduleName);
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
  auto moduleName = to_string_A(ModuleName);
  return Remote64GetModuleHandleA(ulPID, moduleName);
}

HMODULE vuapi remote_get_module_handle_A(ulong pid, const std::string& module_name)
{
  HMODULE result = (HMODULE)-1;

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return (HMODULE)-1;
  }

  bool bIs32Process = false;

  if (get_processor_architecture() == eProcessorArchitecture::PA_X64)   // 64-bit arch
  {
    if (is_wow64(pid))   // 32-bit process
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
    result = Remote64GetModuleHandleA(pid, module_name); // assert(0 && "64 -> 32");
  }
  else   // 64-bit process
  {
    result = Remote64GetModuleHandleA(pid, module_name); // assert(0 && "64 -> 64");
  }
  #else // 32-bit arch
  if (bIs32Process)   // 32-bit process
  {
    result = Remote32GetModuleHandleA(pid, module_name); // assert(0 && "32 -> 32");
  }
  else   // 64-bit process
  {
    result = Remote32GetModuleHandleA(pid, module_name); // assert(0 && "32 -> 64");
  }
  #endif // _WIN64

  return result;
}

HMODULE vuapi remote_get_module_handle_W(const ulong pid, const std::wstring& module_name)
{
  HMODULE result = (HMODULE)-1;

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return (HMODULE)-1;
  }

  bool bIs32Process = false;

  if (get_processor_architecture() == eProcessorArchitecture::PA_X64)   // 64-bit arch
  {
    if (is_wow64(pid))   // 32-bit process
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
    result = Remote64GetModuleHandleW(pid, module_name); // assert(0 && "64 -> 32");
  }
  else   // 64-bit process
  {
    result = Remote64GetModuleHandleW(pid, module_name); // assert(0 && "64 -> 64");
  }
  #else // 32-bit arch
  if (bIs32Process)   // 32-bit process
  {
    result = Remote32GetModuleHandleW(pid, module_name); // assert(0 && "32 -> 32");
  }
  else   // 64-bit process
  {
    result = Remote32GetModuleHandleW(pid, module_name); // assert(0 && "32 -> 64");
  }
  #endif // _WIN64

  return result;
}

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

VUResult vuapi inject_dll_A(ulong pid, const std::string& dll_file_path, bool wait_loading)
{
  if (pid == INVALID_PID_VALUE)
  {
    return 1;
  }

  if (!is_file_exists_A(dll_file_path))
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return 2;
  }

  auto dll_file_name = extract_file_name_A(dll_file_path);
  if (dll_file_name.empty())
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return 2;
  }

  if (remote_get_module_handle_A(pid, dll_file_name) != 0)
  {
    SetLastError(ERROR_ALREADY_EXISTS);
    return 3;
  }

  set_privilege(SE_DEBUG_NAME, true);
  std::shared_ptr<void> hp(OpenProcess(CREATE_THREAD_ACCESS, FALSE, pid), CloseHandle);
  set_privilege(SE_DEBUG_NAME, false);
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
    (dll_file_path.length() + 1) * sizeof(char), // +1 for a null-terminated ANSI string
    MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
  );
  if (pBlock == nullptr)
  {
    return 6;
  }

  if (!wpm(hp.get(), pBlock, dll_file_path.c_str(), dll_file_path.length() * sizeof(char)))
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

  if (wait_loading)
  {
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    VirtualFreeEx(hp.get(), pBlock, 0, MEM_RELEASE);
  }

  SetLastError(ERROR_SUCCESS);

  return 0;
}

VUResult vuapi inject_dll_W(ulong pid, const std::wstring& dll_file_path, bool wait_loading)
{
  if (pid == INVALID_PID_VALUE)
  {
    return 1;
  }

  if (!is_file_exists_W(dll_file_path))
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return 2;
  }

  auto dll_file_name = extract_file_name_W(dll_file_path);
  if (dll_file_name.empty())
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return 2;
  }

  if (remote_get_module_handle_W(pid, dll_file_name) != 0)
  {
    SetLastError(ERROR_ALREADY_EXISTS);
    return 3;
  }

  set_privilege(SE_DEBUG_NAME, true);
  std::shared_ptr<void> hp(OpenProcess(CREATE_THREAD_ACCESS, FALSE, pid), CloseHandle);
  set_privilege(SE_DEBUG_NAME, false);
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
    (dll_file_path.length() + 1) * sizeof(wchar_t), // +1 for a null-terminated UNICODE string
    MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
  );
  if (pBlock == nullptr)
  {
    return 6;
  }

  if (!wpm(hp.get(), pBlock, dll_file_path.c_str(), dll_file_path.length() * sizeof(wchar_t)))
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

  if (wait_loading)
  {
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    VirtualFreeEx(hp.get(), pBlock, 0, MEM_RELEASE);
  }

  SetLastError(ERROR_SUCCESS);

  return 0;
}

/**
 * Process
 */

CProcessX::CProcessX()
  : m_PID(INVALID_PID_VALUE)
  , m_Handle(nullptr)
  , m_Wow64(eWow64::WOW64_ERROR)
  , m_Bit(eXBit::x86)
  , m_LastSystemTimeUTC(0)
  , m_LastSystemTimePerCoreUTC(0)
{
}

// CProcessX::CProcessX(const vu::ulong PID)
//   : m_PID(PID)
//   , m_Handle(nullptr)
//   , m_Wow64(eWow64::WOW64_ERROR)
//   , m_Bit(eXBit::x86)
//   , m_LastSystemTimeUTC(0)
//   , m_LastSystemTimePerCoreUTC(0)
// {
//   Attach(m_PID);
// }

CProcessX::~CProcessX()
{
  Close(m_Handle);
}

// CProcessX::CProcessX(CProcessX& right)
// {
//   *this = right;
// }

// CProcessX& CProcessX::operator=(CProcessX& right)
// {
//   m_PID = right.m_PID;
//   m_Handle = right.m_Handle;
//   m_Wow64 = right.m_Wow64;
//   m_Bit = right.m_Bit;
//   return *this;
// }

bool CProcessX::operator==(CProcessX& right)
{
  bool result = true;
  result &= m_PID == right.m_PID;
  result &= m_Handle == right.m_Handle;
  result &= m_Wow64 == right.m_Wow64;
  result &= m_Bit == right.m_Bit;
  return result;
}

bool CProcessX::operator!=(CProcessX& right)
{
  return !(*this == right);
}

const vu::ulong CProcessX::PID() const
{
  return m_PID;
}

const HANDLE CProcessX::Handle() const
{
  return m_Handle;
}

const vu::eWow64 CProcessX::Wow64() const
{
  return m_Wow64;
}

const vu::eXBit CProcessX::Bits() const
{
  return m_Bit;
}

bool CProcessX::Is64Bits(HANDLE Handle)
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

bool CProcessX::Is64Bits(ulong PID)
{
  if (PID == 0)
  {
    PID = GetCurrentProcessId();
  }

  CProcessX process;
  process.Attach(PID);
  assert(process.Ready());

  return process.Bits() == eXBit::x64;
}

bool CProcessX::IsWow64(HANDLE Handle)
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

bool CProcessX::IsWow64(ulong PID)
{
  if (PID == 0)
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

bool CProcessX::Attach(const ulong PID)
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

bool CProcessX::Attach(const HANDLE Handle)
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

HANDLE CProcessX::Open(const ulong PID)
{
  if (PID == INVALID_PID_VALUE)
  {
    return nullptr;
  }

  set_privilege(SE_DEBUG_NAME, true);
  SetLastError(ERROR_SUCCESS);

  auto result = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
  if (result == nullptr)
  {
    result = nullptr;
  }

  m_LastErrorCode = GetLastError();

  set_privilege(SE_DEBUG_NAME, false);
  SetLastError(ERROR_SUCCESS);

  return result;
}

bool CProcessX::Close(const HANDLE Handle)
{
  if (Handle == nullptr)
  {
    return false;
  }

  return CloseHandle(m_Handle) != FALSE;
}

void CProcessX::Parse()
{
  m_Wow64 = vu::is_wow64(m_Handle);

  if (get_processor_architecture() == vu::PA_X64)
  {
    m_Bit = m_Wow64 == eWow64::WOW64_YES ? eXBit::x86 : eXBit::x64;
  }
  else
  {
    m_Bit = eXBit::x86;
  }
}

bool CProcessX::Read(const ulongptr Address, CBuffer& Data)
{
  if (Address == 0 || Data.GetSize() == 0)
  {
    return false;
  }

  return Read(Address, Data.GetpData(), Data.GetSize());
}

bool CProcessX::Read(const ulongptr Address, void* pData, const size_t Size)
{
  if (Address == 0 || pData == 0 || Size == 0)
  {
    return false;
  }

  auto result = rpm(m_Handle, LPCVOID(Address), pData, Size, true);

  m_LastErrorCode = GetLastError();

  return result;
}

bool CProcessX::Write(const ulongptr Address, const CBuffer& Data)
{
  return Write(Address, Data.GetpData(), Data.GetSize());
}

bool CProcessX::Write(const ulongptr Address, const void* pData, const size_t Size)
{
  if (Address == 0 || pData == 0 || Size == 0)
  {
    return false;
  }

  auto result = wpm(m_Handle, LPCVOID(Address), pData, Size, true);

  m_LastErrorCode = GetLastError();

  return result;
}

double CProcessX::GetCPUPercentUsage()
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

PROCESS_CPU_COUNTERS CProcessX::GetCPUInformation(const double interval)
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

PROCESS_MEMORY_COUNTERS CProcessX::GetMemoryInformation()
{
  PROCESS_MEMORY_COUNTERS result = { 0 };

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return result;
  }

  pfnGetProcessMemoryInfo(m_Handle, &result, sizeof(result));

  return result;
}

PROCESS_TIME_COUNTERS CProcessX::GetTimeInformation()
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

PROCESS_IO_COUNTERS CProcessX::GetIOInformation()
{
  PROCESS_IO_COUNTERS result = { 0 };

  GetProcessIoCounters(m_Handle, &result);

  return result;
}

const CProcessX::Threads& CProcessX::GetThreads()
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

// https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-memory_basic_information
// https://docs.microsoft.com/en-us/windows/win32/memory/memory-protection-constants

const CProcessX::Memories& CProcessX::GetMemories(
  const ulong state,
  const ulong type,
  const ulong protection)
{
  m_Memories.clear();

  if (m_PID == INVALID_PID_VALUE)
  {
    return m_Memories;
  }

  MEMORY_BASIC_INFORMATION mbi = { 0 };

  while (VirtualQueryEx(
    m_Handle, LPCVOID(ulonglong(mbi.BaseAddress) + mbi.RegionSize), &mbi, sizeof(mbi)) != FALSE)
  {
    if (mbi.BaseAddress != nullptr)
    {
      if ((mbi.State & state) && (mbi.Type & type) && (mbi.Protect & protection))
      {
        m_Memories.emplace_back(mbi);
      }
    }
  }

  return m_Memories;
}

CProcessA::CProcessA() : CProcessX() , m_Name("")
{
}

// CProcessA::CProcessA(const ulong PID)
//   : CProcessX(PID)
//   , m_Name("")
// {
// }

CProcessA::~CProcessA()
{
}

// CProcessA::CProcessA(CProcessA& right)
// {
//   *this = right;
// }

// CProcessA& CProcessA::operator=(CProcessA& right)
// {
//   CProcessX::operator=(right);
//   m_Name = right.m_Name;
//   return *this;
// }

bool CProcessA::operator==(CProcessA& right)
{
  bool result = true;
  result &= CProcessX::operator==(right);
  result &= m_Name == right.m_Name;
  return result;
}

bool CProcessA::operator!=(CProcessA& right)
{
  return !(*this == right);
}

std::ostream& operator<<(std::ostream& os, CProcessA& process)
{
  assert(0);
  return os;
}

const std::string& CProcessA::Name() const
{
  return m_Name;
}

void CProcessA::Parse()
{
  CProcessX::Parse();
  m_Name = pid_to_name_A(m_PID);
}

#pragma push_macro("MODULEENTRY32")
#undef MODULEENTRY32

const CProcessA::Modules& CProcessA::GetModules()
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

const MODULEENTRY32 CProcessA::GetModuleInformation()
{
  MODULEENTRY32 result = { 0 };

  if (m_Modules.empty())
  {
    this->GetModules();
  }

  if (!m_Modules.empty())
  {
    result = m_Modules.front();
  }

  return result;
}

#pragma pop_macro("MODULEENTRY32")

/**
 * CProcessW
 */

CProcessW::CProcessW() : CProcessX(), m_Name(L"")
{
}

// CProcessW::CProcessW(const ulong PID)
//   : CProcessX(PID)
//   , m_Name("")
// {
// }

CProcessW::~CProcessW()
{
}

// CProcessW::CProcessW(CProcessW& right)
// {
//   *this = right;
// }

// CProcessW& CProcessW::operator=(CProcessW& right)
// {
//   CProcessX::operator=(right);
//   m_Name = right.m_Name;
//   return *this;
// }

bool CProcessW::operator==(CProcessW& right)
{
  bool result = true;
  result &= CProcessX::operator==(right);
  result &= m_Name == right.m_Name;
  return result;
}

bool CProcessW::operator!=(CProcessW& right)
{
  return !(*this == right);
}

std::ostream& operator<<(std::ostream& os, CProcessW& process)
{
  assert(0);
  return os;
}

const std::wstring& CProcessW::Name() const
{
  return m_Name;
}

void CProcessW::Parse()
{
  CProcessX::Parse();
  m_Name = pid_to_name_W(m_PID);
}

const CProcessW::Modules& CProcessW::GetModules()
{
  m_Modules.clear();

  if (m_PID == INVALID_PID_VALUE)
  {
    return m_Modules;
  }

  ulong   cbNeeded = 0;
  HMODULE hModules[MAX_NMODULES] = { 0 };
  pfnEnumProcessModulesEx(m_Handle, hModules, sizeof(hModules), &cbNeeded, LIST_MODULES_ALL);

  wchar_t s[MAX_PATH] = { 0 };

  for (auto& hModule : hModules)
  {
    if (hModule == nullptr)
    {
      break;
    }

    MODULEENTRY32W me32 = { 0 };
    me32.dwSize = sizeof(me32);
    me32.hModule = hModule;
    me32.th32ProcessID = m_PID;

    ZeroMemory(s, sizeof(s));
    pfnGetModuleBaseNameW(m_Handle, hModule, s, sizeof(s));
    wcscpy_s(me32.szModule, s);

    ZeroMemory(s, sizeof(s));
    pfnGetModuleFileNameExW(m_Handle, hModule, s, sizeof(s));
    wcscpy_s(me32.szExePath, s);

    MODULEINFO_PTR mi = { 0 };
    pfnGetModuleInformation(m_Handle, hModule, &mi, sizeof(mi));
    me32.modBaseAddr = reinterpret_cast<BYTE*>(mi.lpBaseOfDll);
    me32.modBaseSize = static_cast<DWORD>(mi.SizeOfImage);

    m_Modules.emplace_back(*reinterpret_cast<vu::MODULEENTRY32W*>(&me32));
  }

  return m_Modules;
}

const MODULEENTRY32W CProcessW::GetModuleInformation()
{
  MODULEENTRY32W result = { 0 };

  if (m_Modules.empty())
  {
    this->GetModules();
  }

  if (!m_Modules.empty())
  {
    result = m_Modules.front();
  }

  return result;
}

} // namespace vu