/**
 * @file   library.cpp
 * @author Vic P.
 * @brief  Implementation for Dynamic Library
 */

#include "Vutils.h"

namespace vu
{

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

} // namespace vu