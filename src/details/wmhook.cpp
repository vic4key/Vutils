/**
 * @file   wmhook.cpp
 * @author Vic P.
 * @brief  Implementation for Windows Messages Hooking
 */

#include "Vutils.h"

namespace vu
{

HHOOK CWMHookX::m_handles[WH_MAXHOOK] = { 0 };

CWMHookX::CWMHookX() : CLastError()
{
  m_pid = INVALID_PID_VALUE;
}

CWMHookX::~CWMHookX()
{
}

VUResult CWMHookX::set_windows_hook_ex_X(int type, HMODULE hmodule, HOOKPROC pfn_hook_function)
{
  m_last_error_code = ERROR_SUCCESS;

  if (m_pid == INVALID_PID_VALUE)
  {
    return 1;
  }

  if (type < WH_MINHOOK || type > WH_MAXHOOK)
  {
    return 2;
  }

  if (m_handles[type] != nullptr)
  {
    return 3;
  }

  // For desktop apps, if this parameter is zero, the hook procedure is associated
  // with all existing threads running in the same desktop as the calling thread.
  auto tid = get_main_thread_id(m_pid);
  if (tid == -1)
  {
    m_last_error_code = GetLastError();
    return 4;
  }

  auto ret = SetWindowsHookExW(type, pfn_hook_function, hmodule, tid);
  if (ret == nullptr)
  {
    m_last_error_code = GetLastError();
    return 5;
  }

  m_handles[type] = ret;

  return VU_OK;
}

VUResult CWMHookX::uninstall(int type)
{
  m_last_error_code = ERROR_SUCCESS;

  if (type < WH_MINHOOK || type > WH_MAXHOOK)
  {
    return 1;
  }

  if (m_handles[type] == nullptr)
  {
    return 2;
  }

  auto ret = UnhookWindowsHookEx(m_handles[type]);
  if (ret == FALSE)
  {
    m_last_error_code = GetLastError();
    return 3;
  }

  m_handles[type] = nullptr;

  return VU_OK;
}

CWMHookA::CWMHookA(ulong pid, const std::string& dll_file_path) : CWMHookX(), m_library(dll_file_path)
{
  m_pid = pid;
}

CWMHookA::~CWMHookA()
{
}

VUResult CWMHookA::install(int type, const std::string& function_name)
{
  return set_windows_hook_ex_X(type, m_library.handle(), (HOOKPROC)m_library.get_proc_address(function_name));
}

CWMHookW::CWMHookW(ulong pid, const std::wstring& dll_file_path) : CWMHookX(), m_library(dll_file_path)
{
  m_pid = pid;
}

CWMHookW::~CWMHookW()
{
}

VUResult CWMHookW::install(int type, const std::wstring& function_name)
{
  return set_windows_hook_ex_X(type, m_library.handle(), (HOOKPROC)m_library.get_proc_address(function_name));
}

} // namespace vu