/**
 * @file   wmhook.cpp
 * @author Vic P.
 * @brief  Implementation for Windows Messages Hooking
 */

#include "Vutils.h"

namespace vu
{

HHOOK CWMHookX::m_Handles[WH_MAXHOOK] = { 0 };

CWMHookX::CWMHookX() : CLastError()
{
  m_PID = INVALID_PID_VALUE;
}

CWMHookX::~CWMHookX()
{
}

VUResult CWMHookX::SetWindowsHookExX(int Type, HMODULE hModule, HOOKPROC pProc)
{
  m_LastErrorCode = ERROR_SUCCESS;

  if (m_PID == INVALID_PID_VALUE)
  {
    return 1;
  }

  if (Type < WH_MINHOOK || Type > WH_MAXHOOK)
  {
    return 2;
  }

  if (m_Handles[Type] != nullptr)
  {
    return 3;
  }

  // For desktop apps, if this parameter is zero, the hook procedure is associated
  // with all existing threads running in the same desktop as the calling thread.
  auto tid = get_main_thread_id(m_PID);
  if (tid == -1)
  {
    m_LastErrorCode = GetLastError();
    return 4;
  }

  auto ret = SetWindowsHookExW(Type, pProc, hModule, tid);
  if (ret == nullptr)
  {
    m_LastErrorCode = GetLastError();
    return 5;
  }

  m_Handles[Type] = ret;

  return VU_OK;
}

VUResult CWMHookX::Stop(int Type)
{
  m_LastErrorCode = ERROR_SUCCESS;

  if (Type < WH_MINHOOK || Type > WH_MAXHOOK)
  {
    return 1;
  }

  if (m_Handles[Type] == nullptr)
  {
    return 2;
  }

  auto ret = UnhookWindowsHookEx(m_Handles[Type]);
  if (ret == FALSE)
  {
    m_LastErrorCode = GetLastError();
    return 3;
  }

  m_Handles[Type] = nullptr;

  return VU_OK;
}

CWMHookA::CWMHookA(ulong PID, const std::string& DLLFilePath) : CWMHookX(), m_DLL(DLLFilePath)
{
  m_PID = PID;
}

CWMHookA::~CWMHookA()
{
}

VUResult CWMHookA::Start(int Type, const std::string& ProcName)
{
  return SetWindowsHookExX(Type, m_DLL.GetHandle(), (HOOKPROC)m_DLL.GetProcAddress(ProcName));
}

CWMHookW::CWMHookW(ulong PID, const std::wstring& DLLFilePath) : CWMHookX(), m_DLL(DLLFilePath)
{
  m_PID = PID;
}

CWMHookW::~CWMHookW()
{
}

VUResult CWMHookW::Start(int Type, const std::wstring& ProcName)
{
  return SetWindowsHookExX(Type, m_DLL.GetHandle(), (HOOKPROC)m_DLL.GetProcAddress(ProcName));
}

} // namespace vu