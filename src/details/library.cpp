/**
 * @file   library.cpp
 * @author Vic P.
 * @brief  Implementation for Dynamic Library
 */

#include "Vutils.h"

namespace vu
{

CLibraryA::CLibraryA(const std::string& ModuleName)
{
  m_ModuleHandle  = LoadLibraryA(ModuleName.c_str());
  m_LastErrorCode = GetLastError();
}

CLibraryA::~CLibraryA()
{
  FreeLibrary(m_ModuleHandle);
}

const HMODULE& vuapi CLibraryA::GetHandle() const
{
  return m_ModuleHandle;
}

bool vuapi CLibraryA::IsAvailable()
{
  return m_ModuleHandle != nullptr;
}

void* vuapi CLibraryA::GetProcAddress(const std::string& ProcName)
{
  if (m_ModuleHandle == nullptr || ProcName.empty())
  {
    return nullptr;
  }

  return ::GetProcAddress(m_ModuleHandle, ProcName.c_str());
}

void* vuapi CLibraryA::QuickGetProcAddress(const std::string& ModuleName, const std::string& ProcName)
{
  if (ModuleName.empty() || ProcName.empty())
  {
    return nullptr;
  }

  CLibraryA lib(ModuleName);
  if (!lib.IsAvailable())
  {
    return nullptr;
  }

  return lib.GetProcAddress(ProcName);
}

CLibraryW::CLibraryW(const std::wstring& ModuleName)
{
  m_ModuleHandle  = LoadLibraryW(ModuleName.c_str());
  m_LastErrorCode = GetLastError();
}

CLibraryW::~CLibraryW()
{
  FreeLibrary(m_ModuleHandle);
}

const HMODULE& vuapi CLibraryW::GetHandle() const
{
  return m_ModuleHandle;
}

bool vuapi CLibraryW::IsAvailable()
{
  return m_ModuleHandle != nullptr;
}

void* vuapi CLibraryW::GetProcAddress(const std::wstring& ProcName)
{
  if (m_ModuleHandle == nullptr || ProcName.empty())
  {
    return nullptr;
  }

  std::string s(ProcName.cbegin(), ProcName.cend());

  return ::GetProcAddress(m_ModuleHandle, s.c_str());
}

void* vuapi CLibraryW::QuickGetProcAddress(const std::wstring& ModuleName, const std::wstring& ProcName)
{
  if (ModuleName.empty() || ProcName.empty())
  {
    return nullptr;
  }

  CLibraryW lib(ModuleName);
  if (!lib.IsAvailable())
  {
    return nullptr;
  }

  return lib.GetProcAddress(ProcName);
}

} // namespace vu