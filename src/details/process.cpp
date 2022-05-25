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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 6335)
#endif // _MSC_VER

processor_architecture get_processor_architecture()
{
  typedef void (WINAPI *PfnGetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);

  auto pfnGetNativeSystemInfo = (PfnGetNativeSystemInfo)Library::quick_get_proc_address(
    _T("kernel32.dll"),
    _T("GetNativeSystemInfo")
  );

  if (!pfnGetNativeSystemInfo)
  {
    return processor_architecture::PA_UNKNOWN;
  }

  _SYSTEM_INFO si = {0};
  pfnGetNativeSystemInfo(&si);
  return static_cast<processor_architecture>(si.wProcessorArchitecture);
}

wow_64 vuapi is_wow64(ulong pid)
{
  HANDLE hp = nullptr;

  if (pid == 0 || pid != INVALID_PID_VALUE)
  {
    hp = GetCurrentProcess();
  }
  else if (pid != INVALID_PID_VALUE)
  {
    set_privilege(SE_DEBUG_NAME, true);
    hp = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    set_privilege(SE_DEBUG_NAME, false);
  }

  auto result = is_wow64(hp);

  if (pid != INVALID_PID_VALUE)
  {
    CloseHandle(hp);
  }

  return result;
}

wow_64 vuapi is_wow64(HANDLE hp)
{
  if (hp == nullptr || hp == INVALID_HANDLE_VALUE)
  {
    hp = GetCurrentProcess();
  }

  typedef BOOL (WINAPI *PfnIsWow64Process)(HANDLE, PBOOL);
  auto pfnIsWow64Process = (PfnIsWow64Process)Library::quick_get_proc_address(
    _T("kernel32.dll"),
    _T("IsWow64Process")
  );
  if (pfnIsWow64Process == nullptr)
  {
    return wow_64::WOW64_ERROR;
  }

  BOOL wow64 = false;
  if (!pfnIsWow64Process(hp, &wow64))
  {
    return wow_64::WOW64_ERROR;
  }

  return (wow64 ? wow_64::WOW64_YES : wow_64::WOW64_NO);
}

bool vuapi is_64bits(HANDLE hp)
{
  if (hp == nullptr || hp == INVALID_HANDLE_VALUE)
  {
    hp = GetCurrentProcess();
  }

  ProcessX process;
  process.attach(hp);
  assert(process.ready());

  return process.bit() == arch::x64;
}

bool vuapi is_64bits(ulong pid)
{
  if (pid == 0 || pid == INVALID_PID_VALUE)
  {
    pid = GetCurrentProcessId();
  }

  ProcessX process;
  process.attach(pid);
  assert(process.ready());

  return process.bit() == arch::x64;
}

bool vuapi read_memory(
  const HANDLE hp, const void* address, void* buffer, const SIZE_T size, const bool force)
{
  ulong  protection = 0;
  if (force) VirtualProtectEx(hp, LPVOID(address), size, PAGE_EXECUTE_READWRITE, &protection);

  SIZE_T num_read_bytes = 0;
  auto ret = ReadProcessMemory(hp, address, buffer, size, &num_read_bytes);
  const auto last_error = ret != FALSE ? ERROR_SUCCESS : GetLastError();

  if (force) VirtualProtectEx(hp, LPVOID(address), size, protection, &protection);

  SetLastError(last_error);
  return num_read_bytes == size;
}

bool vuapi read_memory_ex(
  const arch bit,
  const HANDLE hp,
  const void* address,
  void* buffer,
  const SIZE_T size,
  const bool force,
  const SIZE_T n_offsets, ...)
{
  va_list args;
  va_start(args, n_offsets);
  std::vector<vu::ulong> offsets;
  for (size_t i = 0; i < n_offsets; i++) offsets.push_back(va_arg(args, vu::ulong));
  va_end(args);

  if (offsets.empty())
  {
    return read_memory(hp, address, buffer, size, force);
  }

  bool result = true;

  LPVOID ptr = LPVOID(address);
  LPVOID val = nullptr;
  SIZE_T num = 0;

  for (int i = -1; i < int(n_offsets); i++)
  {
    ptr = i == -1 ? ptr : LPVOID(ulonglong(ptr) + offsets[i]);
    val = i == n_offsets - 1 ? buffer : LPVOID(&ptr);
    num = i == n_offsets - 1 ? size : bit;

    result &= read_memory(hp, ptr, val, num, force);

    if (!result)
    {
      break;
    }
  }

  return result;
}

bool vuapi write_memory(
  const HANDLE hp, const void* address, const void* buffer, const SIZE_T size, const bool force)
{

  ulong protection = 0;
  if (force) VirtualProtectEx(hp, LPVOID(address), size, PAGE_EXECUTE_READWRITE, &protection);

  SIZE_T n_written_bytes = 0;
  auto ret = WriteProcessMemory(hp, LPVOID(address), buffer, size, &n_written_bytes);
  const auto last_error = ret != FALSE ? ERROR_SUCCESS : GetLastError();

  if (force) VirtualProtectEx(hp, LPVOID(address), size, protection, &protection);

  SetLastError(last_error);
  return n_written_bytes == size;
}

bool vuapi write_memory_ex(
  const arch bit,
  const HANDLE hp,
  const void* address,
  const void* buffer,
  const SIZE_T size,
  const bool force,
  const SIZE_T n_offsets, ...)
{
  va_list args;
  va_start(args, n_offsets);
  std::vector<ulong> offsets;
  for (size_t i = 0; i < n_offsets; i++) offsets.push_back(va_arg(args, ulong));
  va_end(args);

  if (offsets.empty())
  {
    return write_memory(hp, LPVOID(address), buffer, size, force);
  }

  bool result = true;

  LPVOID ptr = LPVOID(address);
  LPVOID val = nullptr;
  SIZE_T num = 0;

  for (int i = -1; i < int(n_offsets); i++)
  {
    ptr = i == -1 ? ptr : LPVOID(ulonglong(ptr) + offsets[i]);
    val = i == n_offsets - 1 ? LPVOID(buffer) : LPVOID(&ptr);
    num = i == n_offsets - 1 ? size : bit;

    if (i == n_offsets - 1)
    {
      result &= write_memory(hp, ptr, val, num, force);
    }
    else
    {
      result &= read_memory(hp, ptr, val, num, force);
    }

    if (!result)
    {
      break;
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

  auto h_snap = pfnCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (h_snap == INVALID_HANDLE_VALUE)
  {
    return (ulong)-1;
  }

  ulong result = -1;

  TProcessEntry32A pe = { 0 };
  pe.dwSize = sizeof(TProcessEntry32A);

  auto nextale = pfnProcess32FirstA(h_snap, &pe);
  while (nextale)
  {
    if (pe.th32ProcessID == child_pid)
    {
      result = pe.th32ParentProcessID;
      break;
    }

    nextale = pfnProcess32NextA(h_snap, &pe);
  }

  CloseHandle(h_snap);

  return result;
}

ulong vuapi get_main_thread_id(ulong pid)
{
  auto h_snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (h_snap == INVALID_HANDLE_VALUE)
  {
    return -1;
  }

  ulong result = -1;

  ::THREADENTRY32 te = { 0 };
  te.dwSize = sizeof(THREADENTRY32);

  auto nextable = Thread32First(h_snap, &te);
  while (nextable)
  {
    if (te.th32OwnerProcessID == pid)
    {
      result = te.th32ThreadID;
      break;
    }

    nextable = Thread32Next(h_snap, &te);
  }

  CloseHandle(h_snap);

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

HMODULE vuapi Remote32GetModuleHandleA(const ulong pid, const std::string& module_name)
{
  HMODULE result = NULL;

  assert(0);

  return result;
}

HMODULE vuapi Remote32GetModuleHandleW(const ulong pid, const std::wstring& module_name)
{
  auto s = to_string_A(module_name);
  return Remote32GetModuleHandleA(pid, s);
}

HMODULE vuapi Remote64GetModuleHandleA(const ulong pid, const std::string& module_name)
{
  HMODULE result = NULL;

  set_privilege(SE_DEBUG_NAME, true);
  auto hp = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
  set_privilege(SE_DEBUG_NAME, false);
  if (hp == nullptr)
  {
    return result;
  }

  HMODULE hmodules[MAX_NMODULES] = {0};
  ulong cb_needed = 0;
  pfnEnumProcessModulesEx(hp, hmodules, sizeof(hmodules), &cb_needed, LIST_MODULES_ALL);

  ulong n_modules = cb_needed / sizeof(HMODULE);
  if (n_modules == 0)
  {
    return result;
  }

  auto target_name = lower_string_A(module_name);
  target_name = trim_string_A(target_name);

  std::string it_name = "";

  char ps_module_name[MAX_PATH] = {0};
  for (ulong i = 0; i < n_modules; i++)
  {
    pfnGetModuleBaseNameA(hp, hmodules[i], ps_module_name, sizeof(module_name));
    it_name = lower_string_A(ps_module_name);
    if (it_name == target_name)
    {
      result = hmodules[i];
      break;
    }
  }

  SetLastError(ERROR_SUCCESS);

  CloseHandle(hp);

  return result;
}

HMODULE vuapi Remote64GetModuleHandleW(const ulong pid, const std::wstring& module_name)
{
  auto s = to_string_A(module_name);
  return Remote64GetModuleHandleA(pid, s);
}

HMODULE vuapi remote_get_module_handle_A(ulong pid, const std::string& module_name)
{
  HMODULE result = (HMODULE)-1;

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return (HMODULE)-1;
  }

  bool is_32bit_process = false;

  if (get_processor_architecture() == processor_architecture::PA_X64)   // 64-bit arch
  {
    if (is_wow64(pid) != wow_64::WOW64_NO)   // 32-bit process
    {
      is_32bit_process = true;
    }
    else   // 64-bit process
    {
      is_32bit_process = false;
    }
  }
  else   // 32-bit arch
  {
    is_32bit_process = true; // 32-bit process
  }

  #ifdef _WIN64 // 64-bit arch
  if (is_32bit_process)   // 32-bit process
  {
    result = Remote64GetModuleHandleA(pid, module_name); // assert(0 && "64 -> 32");
  }
  else   // 64-bit process
  {
    result = Remote64GetModuleHandleA(pid, module_name); // assert(0 && "64 -> 64");
  }
  #else // 32-bit arch
  if (is_32bit_process)   // 32-bit process
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

  bool is_32bit_process = false;

  if (get_processor_architecture() == processor_architecture::PA_X64)   // 64-bit arch
  {
    if (is_wow64(pid) != wow_64::WOW64_NO)   // 32-bit process
    {
      is_32bit_process = true;
    }
    else   // 64-bit process
    {
      is_32bit_process = false;
    }
  }
  else   // 32-bit arch
  {
    is_32bit_process = true; // 32-bit process
  }

  #ifdef _WIN64 // 64-bit arch
  if (is_32bit_process)   // 32-bit process
  {
    result = Remote64GetModuleHandleW(pid, module_name); // assert(0 && "64 -> 32");
  }
  else   // 64-bit process
  {
    result = Remote64GetModuleHandleW(pid, module_name); // assert(0 && "64 -> 64");
  }
  #else // 32-bit arch
  if (is_32bit_process)   // 32-bit process
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

  auto ptr_block = VirtualAllocEx(
    hp.get(),
    nullptr,
    (dll_file_path.length() + 1) * sizeof(char), // +1 for a null-terminated ANSI string
    MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
  );
  if (ptr_block == nullptr)
  {
    return 6;
  }

  if (!write_memory(hp.get(), ptr_block, dll_file_path.c_str(), dll_file_path.length() * sizeof(char)))
  {
    return 7;
  }

  auto hthread = CreateRemoteThread(
    hp.get(),
    nullptr, 0,
    (LPTHREAD_START_ROUTINE)pfnLoadLibraryA,
    ptr_block,
    0, nullptr
  );
  if (hthread == nullptr)
  {
    return 8;
  }

  if (wait_loading)
  {
    WaitForSingleObject(hthread, INFINITE);
    CloseHandle(hthread);
    VirtualFreeEx(hp.get(), ptr_block, 0, MEM_RELEASE);
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

  auto ptr_block = VirtualAllocEx(
    hp.get(),
    nullptr,
    (dll_file_path.length() + 1) * sizeof(wchar_t), // +1 for a null-terminated UNICODE string
    MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
  );
  if (ptr_block == nullptr)
  {
    return 6;
  }

  if (!write_memory(hp.get(), ptr_block, dll_file_path.c_str(), dll_file_path.length() * sizeof(wchar_t)))
  {
    return 7;
  }

  auto hthread = CreateRemoteThread(
    hp.get(),
    nullptr, 0,
    (LPTHREAD_START_ROUTINE)pfnLoadLibraryW,
    ptr_block,
    0, nullptr
  );
  if (hthread == nullptr)
  {
    return 8;
  }

  if (wait_loading)
  {
    WaitForSingleObject(hthread, INFINITE);
    CloseHandle(hthread);
    VirtualFreeEx(hp.get(), ptr_block, 0, MEM_RELEASE);
  }

  SetLastError(ERROR_SUCCESS);

  return 0;
}

/**
 * Process
 */

ProcessX::ProcessX()
  : m_pid(INVALID_PID_VALUE)
  , m_handle(nullptr)
  , m_wow64(wow_64::WOW64_ERROR)
  , m_bit(arch::x86)
  , m_attached(false)
  , m_last_system_time_UTC(0)
  , m_last_system_time_per_core_UTC(0)
{
}

// ProcessX::ProcessX(const vu::ulong pid)
//   : m_pid(pid)
//   , m_handle(nullptr)
//   , m_wow64(eWow64::WOW64_ERROR)
//   , m_bit(eXBit::x86)
//   , m_last_system_time_UTC(0)
//   , m_last_system_time_per_core_UTC(0)
// {
//   Attach(m_pid);
// }

ProcessX::~ProcessX()
{
  if (m_attached)
  {
    this->detach();
  }
  else
  {
    this->close(m_handle);
  }
}

ProcessX::ProcessX(ProcessX& right)
{
  *this = right;
}

ProcessX& ProcessX::operator=(ProcessX& right)
{
  m_pid = right.m_pid;
  m_handle = right.m_handle;
  m_wow64 = right.m_wow64;
  m_bit = right.m_bit;
  m_attached = right.m_attached;
  m_last_system_time_UTC = right.m_last_system_time_UTC;
  m_last_system_time_per_core_UTC = right.m_last_system_time_per_core_UTC;
  m_threads = right.m_threads;
  m_memories = right.m_memories;
  return *this;
}

bool ProcessX::operator==(ProcessX& right)
{
  bool result = true;
  result &= m_pid == right.m_pid;
  result &= m_handle == right.m_handle;
  result &= m_wow64 == right.m_wow64;
  result &= m_bit == right.m_bit;
  // result &= m_attached == right.m_attached;
  return result;
}

bool ProcessX::operator!=(ProcessX& right)
{
  return !(*this == right);
}

const vu::ulong ProcessX::pid() const
{
  return m_pid;
}

const HANDLE ProcessX::handle() const
{
  return m_handle;
}

const vu::wow_64 ProcessX::wow64() const
{
  return m_wow64;
}

const vu::arch ProcessX::bit() const
{
  return m_bit;
}

bool ProcessX::ready()
{
  return m_handle != nullptr;
}

bool ProcessX::attach(const ulong pid)
{
  if (pid == INVALID_PID_VALUE)
  {
    return false;
  }

  auto hp = this->open(pid);
  if (hp == nullptr)
  {
    return false;
  }

  return this->attach(hp);
}

bool ProcessX::attach(const HANDLE hp)
{
  if (hp == nullptr)
  {
    return false;
  }

  auto pid = GetProcessId(hp);
  if (pid == INVALID_PID_VALUE)
  {
    return false;
  }

  m_pid = pid;
  m_handle = hp;
  m_attached = true;

  SetLastError(ERROR_SUCCESS);

  this->parse();

  return true;
}

void ProcessX::detach()
{
  m_attached = false;
  m_pid = INVALID_PID_VALUE;
  m_handle = INVALID_HANDLE_VALUE;
  m_wow64 = wow_64::WOW64_ERROR;
  m_last_system_time_UTC = 0;
  m_last_system_time_per_core_UTC = 0;
  m_threads.clear();
  m_memories.clear();
}

HANDLE ProcessX::open(const ulong pid)
{
  if (pid == INVALID_PID_VALUE)
  {
    return nullptr;
  }

  set_privilege(SE_DEBUG_NAME, true);
  SetLastError(ERROR_SUCCESS);

  auto result = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (result == nullptr)
  {
    result = nullptr;
  }

  m_last_error_code = GetLastError();

  set_privilege(SE_DEBUG_NAME, false);
  SetLastError(ERROR_SUCCESS);

  return result;
}

bool ProcessX::close(const HANDLE hp)
{
  if (hp == nullptr)
  {
    return false;
  }

  return CloseHandle(m_handle) != FALSE;
}

void ProcessX::parse()
{
  m_wow64 = vu::is_wow64(m_handle);

  if (get_processor_architecture() == vu::processor_architecture::PA_X64)
  {
    m_bit = m_wow64 == wow_64::WOW64_YES ? arch::x86 : arch::x64;
  }
  else
  {
    m_bit = arch::x86;
  }
}

bool ProcessX::read_memory(const ulongptr address, Buffer& buffer, const bool force)
{
  if (address == 0 || buffer.get_size() == 0)
  {
    return false;
  }

  return this->read_memory(address, buffer.get_ptr(), buffer.get_size(), force);
}

bool ProcessX::read_memory(const ulongptr address, void* ptr_data, const size_t size, const bool force)
{
  if (address == 0 || ptr_data == 0 || size == 0)
  {
    return false;
  }

  auto result = vu::read_memory(m_handle, LPCVOID(address), ptr_data, size, force);

  m_last_error_code = GetLastError();

  return result;
}

bool ProcessX::write_memory(const ulongptr address, const Buffer& buffer, const bool force)
{
  return this->write_memory(address, buffer.get_ptr(), buffer.get_size(), force);
}

bool ProcessX::write_memory(const ulongptr address, const void* ptr_data, const size_t size, const bool force)
{
  if (address == 0 || ptr_data == 0 || size == 0)
  {
    return false;
  }

  auto result = vu::write_memory(m_handle, LPCVOID(address), ptr_data, size, force);

  m_last_error_code = GetLastError();

  return result;
}

double ProcessX::get_cpu_percent_usage()
{
  const auto fn_file_time_to_UTC = [](const FILETIME * file_time) -> uint64_t
  {
    LARGE_INTEGER li = { 0 };
    li.LowPart  = file_time->dwLowDateTime;
    li.HighPart = file_time->dwHighDateTime;
    return li.QuadPart;
  };

  FILETIME system_time = { 0 };
  GetSystemTimeAsFileTime(&system_time);
  const int64_t system_time_UTC = fn_file_time_to_UTC(&system_time);

  const auto time = this->get_time_information();

  auto n_cores = std::thread::hardware_concurrency();

  const int64_t system_time_per_core_UTC =\
    (fn_file_time_to_UTC(&time.KernelTime) + fn_file_time_to_UTC(&time.UserTime)) / n_cores;

  if (m_last_system_time_per_core_UTC == 0 || m_last_system_time_UTC == 0)
  {
    m_last_system_time_per_core_UTC = system_time_per_core_UTC;
    m_last_system_time_UTC = system_time_UTC;
    return 0.; // get_cpu_percent_usage();
  }

  const int64_t system_time_delta_per_core_UTC =\
    system_time_per_core_UTC - m_last_system_time_per_core_UTC;
  const int64_t system_time_delta_UTC = system_time_UTC - m_last_system_time_UTC;

  if (system_time_delta_UTC == 0)
  {
    return 0.; // get_cpu_percent_usage();
  }

  double result =\
    (system_time_delta_per_core_UTC * 100. + system_time_delta_UTC / 2.) / system_time_delta_UTC;

  m_last_system_time_per_core_UTC = system_time_per_core_UTC;
  m_last_system_time_UTC = system_time_UTC;

  return result;
};

PROCESS_CPU_COUNTERS ProcessX::get_cpu_information(const double interval)
{
  PROCESS_CPU_COUNTERS result = { 0 };

  const int DEF_PART_MS = 200; // millisecond

  const auto fn_to_miliseconds = [](const double second) -> double
  {
    return second * 1000.; // millisecond
  };

  result.Usage = 0.;

  const int n_count = int(std::ceil(fn_to_miliseconds(interval) / DEF_PART_MS));

  for (int i = 0; i < n_count; i++)
  {
    const double usage = this->get_cpu_percent_usage();
    result.Usage = usage > result.Usage ? usage : result.Usage;
    // result.Usage = Usage;
    Sleep(DEF_PART_MS);
  }

  return result;
}

PROCESS_MEMORY_COUNTERS ProcessX::get_memory_information()
{
  PROCESS_MEMORY_COUNTERS result = { 0 };

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return result;
  }

  pfnGetProcessMemoryInfo(m_handle, &result, sizeof(result));

  return result;
}

PROCESS_TIME_COUNTERS ProcessX::get_time_information()
{
  PROCESS_TIME_COUNTERS result = { 0 };

  GetProcessTimes(
    m_handle,
    &result.CreationTime,
    &result.ExitTime,
    &result.KernelTime,
    &result.UserTime);

  return result;
}

PROCESS_IO_COUNTERS ProcessX::get_io_information()
{
  PROCESS_IO_COUNTERS result = { 0 };

  GetProcessIoCounters(m_handle, &result);

  return result;
}

const ProcessX::threads& ProcessX::get_threads()
{
  m_threads.clear();

  if (m_pid == INVALID_PID_VALUE)
  {
    return m_threads;
  }

  auto h_snap_thread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (h_snap_thread == INVALID_HANDLE_VALUE)
  {
    return m_threads;
  }

  ::THREADENTRY32 te32 = { 0 };
  te32.dwSize = sizeof(THREADENTRY32);

  if (Thread32First(h_snap_thread, &te32))
  {
    do
    {
      if (te32.th32OwnerProcessID == m_pid)
      {
        m_threads.emplace_back(*reinterpret_cast<vu::THREADENTRY32*>(&te32));
      }
    } while (Thread32Next(h_snap_thread, &te32));
  }

  CloseHandle(h_snap_thread);

  return m_threads;
}

// https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-memory_basic_information
// https://docs.microsoft.com/en-us/windows/win32/memory/memory-protection-constants

const ProcessX::memories& ProcessX::get_memories(
  const ulong state, const ulong type, const ulong protection)
{
  m_memories.clear();

  if (m_pid == INVALID_PID_VALUE)
  {
    return m_memories;
  }

  MEMORY_BASIC_INFORMATION mbi = { 0 };

  while (VirtualQueryEx(
    m_handle, LPCVOID(ulonglong(mbi.BaseAddress) + mbi.RegionSize), &mbi, sizeof(mbi)) != FALSE)
  {
    if (mbi.BaseAddress != nullptr)
    {
      if ((mbi.State & state) && (mbi.Type & type) && (mbi.Protect & protection))
      {
        m_memories.emplace_back(mbi);
      }
    }
  }

  return m_memories;
}

bool process_scan_memory(
  std::vector<size_t>& addresses,
  ProcessX& process,
  const std::wstring& pattern,
  const std::pair<byte*, ulong>& module,
  const bool first_match_only,
  const ulong state = DEF_SM_STATE,
  const ulong type = DEF_SM_PAGE,
  const ulong protection = DEF_SM_PROTECTION)
{
  addresses.clear();

  if (!process.ready() || pattern.empty())
  {
    return false;
  }

  for (auto& mem : process.get_memories(state, type, protection))
  {
    if (module.first != nullptr && module.second != 0)
    {
      if (mem.BaseAddress < module.first || mem.BaseAddress > module.first + module.second)
      {
        continue;
      }
    }

    void*  ptr = mem.BaseAddress;
    size_t num = mem.RegionSize;

    Buffer remote_copied_mem_block;
    if (process.pid() != GetCurrentProcessId())
    {
      remote_copied_mem_block.resize(mem.RegionSize);
      bool ret = process.read_memory(ulongptr(mem.BaseAddress), remote_copied_mem_block);
      if (!ret)
      {
        continue;
      }

      ptr = remote_copied_mem_block.get_ptr();
      num = remote_copied_mem_block.get_size();
    }

    auto offsets = find_pattern_W(ptr, num, pattern, first_match_only);
    if (!offsets.empty())
    {
      for (auto& offset : offsets) offset += ulongptr(mem.BaseAddress);
      addresses.insert(addresses.end(), offsets.begin(), offsets.end());
    }

    if (first_match_only && !addresses.empty())
    {
      break;
    }
  }

  return true;
}

/**
 * ProcessA
 */

ProcessA::ProcessA() : ProcessX() , m_name("")
{
}

// ProcessA::ProcessA(const ulong pid)
//   : ProcessX(pid)
//   , m_name("")
// {
// }

ProcessA::~ProcessA()
{
}

ProcessA::ProcessA(ProcessA& right)
{
  *this = right;
}

ProcessA& ProcessA::operator=(ProcessA& right)
{
  ProcessX::operator=(right);
  m_name = right.m_name;
  return *this;
}

bool ProcessA::operator==(ProcessA& right)
{
  bool result = true;
  result &= ProcessX::operator==(right);
  result &= m_name == right.m_name;
  return result;
}

bool ProcessA::operator!=(ProcessA& right)
{
  return !(*this == right);
}

std::ostream& operator<<(std::ostream& os, ProcessA& process)
{
  assert(0);
  return os;
}

const std::string& ProcessA::name() const
{
  return m_name;
}

const std::string ProcessA::get_path()
{
  std::string result = "";

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return result;
  }

  char s[MAX_PATH] = { 0 };
  pfnGetModuleFileNameExA(m_handle, nullptr, s, ARRAYSIZE(s));
  result.assign(s);

  return result;
}

bool ProcessA::create(
  const std::string& file_path,
  const std::string& file_dir,
  const std::string& command_line,
  const uint creation_flags,
  const bool inherit_handle,
  PROCESS_INFORMATION* ptr_pi
)
{
  STARTUPINFOA si = { 0 };
  si.cb = sizeof(si);

  PROCESS_INFORMATION pi = { 0 };

  bool result = CreateProcessA(
    file_path.c_str(),
    const_cast<LPSTR>(command_line.c_str()),
    nullptr,
    nullptr,
    FALSE,
    creation_flags,
    nullptr,
    file_dir.c_str(),
    &si,
    &pi
  ) != FALSE;

  if (!result)
  {
    m_last_error_code = GetLastError();
    return false;
  }

  if (ptr_pi != nullptr)
  {
    *ptr_pi = pi;
  }

  return this->attach(pi.hProcess);
}

void ProcessA::parse()
{
  ProcessX::parse();
  m_name = pid_to_name_A(m_pid);
}

#pragma push_macro("MODULEENTRY32")
#undef MODULEENTRY32

const ProcessA::modules& ProcessA::get_modules()
{
  m_modules.clear();

  if (m_pid == INVALID_PID_VALUE)
  {
    return m_modules;
  }

  ulong   cb_needed = 0;
  HMODULE h_modules[MAX_NMODULES] = { 0 };
  pfnEnumProcessModulesEx(m_handle, h_modules, sizeof(h_modules), &cb_needed, LIST_MODULES_ALL);

  char s[MAX_PATH] = { 0 };

  for (auto& h_module : h_modules)
  {
    if (h_module == nullptr)
    {
      break;
    }

    MODULEENTRY32 me32 = { 0 };
    me32.dwSize  = sizeof(me32);
    me32.hModule = h_module;
    me32.th32ProcessID = m_pid;

    ZeroMemory(s, sizeof(s));
    pfnGetModuleBaseNameA(m_handle, h_module, s, sizeof(s));
    strcpy_s(me32.szModule, s);

    ZeroMemory(s, sizeof(s));
    pfnGetModuleFileNameExA(m_handle, h_module, s, sizeof(s));
    strcpy_s(me32.szExePath, s);

    MODULEINFO_PTR mi = { 0 };
    pfnGetModuleInformation(m_handle, h_module, &mi, sizeof(mi));
    me32.modBaseAddr = reinterpret_cast<BYTE*>(mi.lpBaseOfDll);
    me32.modBaseSize = static_cast<DWORD>(mi.SizeOfImage);

    m_modules.emplace_back(*reinterpret_cast<vu::MODULEENTRY32*>(&me32));
  }

  return m_modules;
}

const MODULEENTRY32 ProcessA::get_module_information()
{
  MODULEENTRY32 result = { 0 };

  if (m_modules.empty())
  {
    this->get_modules();
  }

  if (!m_modules.empty())
  {
    result = m_modules.front();
  }

  return result;
}

bool ProcessA::scan_memory(
  std::vector<size_t>& addresses,
  const std::string& pattern,
  const std::string& module_name,
  const bool first_match_only,
  const ulong state,
  const ulong type,
  const ulong protection)
{
  std::pair<byte*, ulong> module(nullptr, 0);

  if (!module_name.empty())
  {
    auto modules = this->get_modules();
    auto it = std::find_if(modules.begin(), modules.end(), [&](MODULEENTRY32& me)
    {
      return compare_string_A(me.szModule, module_name, true);
    });

    if (it != modules.end())
    {
      module.first = it->modBaseAddr;
      module.second = it->modBaseSize;
    }
  }

  return process_scan_memory(
    addresses, *this, to_string_W(pattern), module, first_match_only, state, type, protection);
}

#pragma pop_macro("MODULEENTRY32")

/**
 * ProcessW
 */

ProcessW::ProcessW() : ProcessX(), m_name(L"")
{
}

// ProcessW::ProcessW(const ulong pid)
//   : ProcessX(pid)
//   , m_name("")
// {
// }

ProcessW::~ProcessW()
{
}

ProcessW::ProcessW(ProcessW& right)
{
  *this = right;
}

ProcessW& ProcessW::operator=(ProcessW& right)
{
  ProcessX::operator=(right);
  m_name = right.m_name;
  return *this;
}

bool ProcessW::operator==(ProcessW& right)
{
  bool result = true;
  result &= ProcessX::operator==(right);
  result &= m_name == right.m_name;
  return result;
}

bool ProcessW::operator!=(ProcessW& right)
{
  return !(*this == right);
}

std::ostream& operator<<(std::ostream& os, ProcessW& process)
{
  assert(0);
  return os;
}

const std::wstring& ProcessW::name() const
{
  return m_name;
}

const std::wstring ProcessW::get_path()
{
  std::wstring result = L"";

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return result;
  }

  wchar s[MAX_PATH] = { 0 };
  pfnGetModuleFileNameExW(m_handle, nullptr, s, ARRAYSIZE(s));
  result.assign(s);

  return result;
}

bool ProcessW::create(
  const std::wstring& file_path,
  const std::wstring& file_dir,
  const std::wstring& command_line,
  const uint creation_flags,
  const bool inherit_handle,
  PROCESS_INFORMATION* ptr_pi
)
{
  STARTUPINFOW si = { 0 };
  si.cb = sizeof(si);

  PROCESS_INFORMATION pi = { 0 };

  bool result = CreateProcessW(
    file_path.c_str(),
    const_cast<LPWSTR>(command_line.c_str()),
    nullptr,
    nullptr,
    FALSE,
    creation_flags,
    nullptr,
    file_dir.c_str(),
    &si,
    &pi
  ) != FALSE;

  if (!result)
  {
    m_last_error_code = GetLastError();
    return false;
  }

  if (ptr_pi != nullptr)
  {
    *ptr_pi = pi;
  }

  return this->attach(pi.hProcess);
}

void ProcessW::parse()
{
  ProcessX::parse();
  m_name = pid_to_name_W(m_pid);
}

const ProcessW::modules& ProcessW::get_modules()
{
  m_modules.clear();

  if (m_pid == INVALID_PID_VALUE)
  {
    return m_modules;
  }

  ulong   cb_needed = 0;
  HMODULE h_modules[MAX_NMODULES] = { 0 };
  pfnEnumProcessModulesEx(m_handle, h_modules, sizeof(h_modules), &cb_needed, LIST_MODULES_ALL);

  wchar_t s[MAX_PATH] = { 0 };

  for (auto& h_module : h_modules)
  {
    if (h_module == nullptr)
    {
      break;
    }

    MODULEENTRY32W me32 = { 0 };
    me32.dwSize = sizeof(me32);
    me32.hModule = h_module;
    me32.th32ProcessID = m_pid;

    ZeroMemory(s, sizeof(s));
    pfnGetModuleBaseNameW(m_handle, h_module, s, sizeof(s));
    wcscpy_s(me32.szModule, s);

    ZeroMemory(s, sizeof(s));
    pfnGetModuleFileNameExW(m_handle, h_module, s, sizeof(s));
    wcscpy_s(me32.szExePath, s);

    MODULEINFO_PTR mi = { 0 };
    pfnGetModuleInformation(m_handle, h_module, &mi, sizeof(mi));
    me32.modBaseAddr = reinterpret_cast<BYTE*>(mi.lpBaseOfDll);
    me32.modBaseSize = static_cast<DWORD>(mi.SizeOfImage);

    m_modules.emplace_back(*reinterpret_cast<vu::MODULEENTRY32W*>(&me32));
  }

  return m_modules;
}

const MODULEENTRY32W ProcessW::get_module_information()
{
  MODULEENTRY32W result = { 0 };

  if (m_modules.empty())
  {
    this->get_modules();
  }

  if (!m_modules.empty())
  {
    result = m_modules.front();
  }

  return result;
}

bool ProcessW::scan_memory(
  std::vector<size_t>& addresses,
  const std::wstring& pattern,
  const std::wstring& module_name,
  const bool first_match_only,
  const ulong state,
  const ulong type,
  const ulong protection)
{
  std::pair<byte*, ulong> module(nullptr, 0);

  if (!module_name.empty())
  {
    auto modules = this->get_modules();
    auto it = std::find_if(modules.begin(), modules.end(), [&](MODULEENTRY32W& me)
    {
      return compare_string_W(me.szModule, module_name, true);
    });

    if (it != modules.end())
    {
      module.first = it->modBaseAddr;
      module.second = it->modBaseSize;
    }
  }

  return process_scan_memory(
    addresses, *this, pattern, module, first_match_only, state, type, protection);
}

/**
 * Single Process
 */

SingleProcess::SingleProcess() : m_name(L""), m_handle(nullptr)
{
}

SingleProcess::~SingleProcess()
{
  this->finalize();
}

bool SingleProcess::initialize(const std::string& name, const std::function<void(bool running)> fn)
{
  const auto s = to_string_W(name);
  return this->initialize(s, fn);
}

bool SingleProcess::initialize(const std::wstring& name, const std::function<void(bool running)> fn)
{
  if (!m_name.empty())
  {
    throw "single process initialized";
  }

  m_name = name;

  auto running = this->running();

  if (fn != nullptr)
  {
    fn(running);
  }

  return running;
}

void SingleProcess::finalize()
{
  if (m_handle != nullptr)
  {
    CloseHandle(m_handle);
    m_handle = nullptr;
    m_name = L"";
  }
}

bool SingleProcess::running()
{
  auto handle = CreateEventW(nullptr, TRUE, FALSE, m_name.c_str());
  if (GetLastError() == ERROR_ALREADY_EXISTS)
  {
    CloseHandle(handle);
    return true;
  }

  m_handle = handle;

  return false;
}

void SingleProcess::fn_default(bool running)
{
  if (running)
  {
    const std::wstring title = extract_file_name_W(get_current_file_path_W());
    const std::wstring text = L"The application is running.\nWould you like to run a new instance?";
    auto response = MessageBoxW(GetActiveWindow(), text.c_str(), title.c_str(), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
    if (response == IDNO)
    {
      ExitProcess(0);
    }
  }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu