#pragma once

#include "Sample.h"

using namespace vu;

#include <algorithm>

struct sIATElement
{
  std::string target;
  std::string module;
  std::string function;
  ULONG_PTR original;
  ULONG_PTR replacement;

  sIATElement()
    : target(""), module(""), function(""), original(0), replacement(0) {}

  sIATElement(
    const std::string& t,
    const std::string& m,
    const std::string& f,
    const ULONG_PTR o = 0,
    const ULONG_PTR c = 0)
    : target(t), module(m), function(f), original(o), replacement(c) {}

  const sIATElement& operator=(const sIATElement& right)
  {
    target = right.target;
    module = right.module;
    function = right.function;
    original = right.original;
    replacement = right.replacement;
    return *this;
  }

  bool operator==(const sIATElement& right) const
  {
    return\
      UpperStringA(target) == UpperStringA(right.target) &&
      UpperStringA(module) == UpperStringA(right.module) &&
      UpperStringA(function) == UpperStringA(right.function);
  }

  bool operator!=(const sIATElement& right) const
  {
    return !(*this == right);
  }
};

class CIATHookManager : public CSingletonT<CIATHookManager>
{
public:
  enum IATAction
  {
    IAT_OVERRIDE,
    IAT_RESTORE,
  };

  typedef std::vector<sIATElement> IATElements;

  CIATHookManager();
  virtual ~CIATHookManager();

  VUResult Override(
    const std::string& target,
    const std::string& module,
    const std::string& function,
    const ULONG_PTR replacement
  );

  VUResult Restore(
    const std::string& target,
    const std::string& module,
    const std::string& function
  );

  /**
   * Iterate all imported-functions in a module.
   * @param[out] module   The imported-module name.
   * @param[out] function The imported-function name.
   * @param[out,in] pOFT  The Original First Thunk that point to INT <Import Name Table>.
   * @param[out,in] pFT   The First Thunk that point to IAT <Import Address Table>.
   * @return true to continue or false to exit iteration.
   */
  VUResult Iterate(const std::string& module, std::function<bool(
    const std::string& module,
    const std::string& function,
    PIMAGE_THUNK_DATA& pOFT,
    PIMAGE_THUNK_DATA& pFT)> fn);

private:
  IATElements::iterator Find(const sIATElement& element);

  IATElements::iterator Find(
    const std::string& target,
    const std::string& module,
    const std::string& function);

  bool Exist(
    const std::string& target,
    const std::string& module,
    const std::string& function);

  VUResult Do(const IATAction action, sIATElement& element);

private:
  IATElements m_IATElements;
};

CIATHookManager::CIATHookManager()
{
}

CIATHookManager::~CIATHookManager()
{
}

CIATHookManager::IATElements::iterator CIATHookManager::Find(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  return std::find(m_IATElements.begin(), m_IATElements.end(), sIATElement(target, module, function));
}

CIATHookManager::IATElements::iterator CIATHookManager::Find(const sIATElement& element)
{
  return this->Find(element.target, element.module, element.function);
}

bool CIATHookManager::Exist(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  return this->Find(target, module, function) != m_IATElements.end();
}

VUResult CIATHookManager::Override(
  const std::string& target,
  const std::string& module,
  const std::string& function,
  const ULONG_PTR replacement)
{
  if (this->Exist(target, module, function))
  {
    return 1;
  }

  auto element = sIATElement(target, module, function, 0, replacement);

  if (this->Do(IATAction::IAT_OVERRIDE, element) != VU_OK)
  {
    return 2;
  }

  m_IATElements.emplace_back(element);

  return VU_OK;
}

VUResult CIATHookManager::Restore(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  auto it = this->Find(target, module, function);
  if (it == m_IATElements.end())
  {
    return 1;
  }

  auto element = *it;

  if (this->Do(IATAction::IAT_RESTORE, element) != VU_OK)
  {
    return 2;
  }

  m_IATElements.erase(it);

  return VU_OK;
}

VUResult CIATHookManager::Do(const IATAction action, sIATElement& element)
{
  if (element.target.empty() || element.module.empty() || element.function.empty())
  {
    return 1;
  }

  this->Iterate(element.target, [&](
    const std::string& m, const std::string& f, PIMAGE_THUNK_DATA& pOFT, PIMAGE_THUNK_DATA& pFT)
  {
    printf("[%p] [%p] %s!%s\n", pOFT->u1.Function, pFT->u1.Function, m.c_str(), f.c_str());
  
    if (element == sIATElement(element.target, m, f))
    {
      ULONG_PTR address = 0;

      if (action == IATAction::IAT_OVERRIDE)
      {
        element.original = pFT->u1.Function;
        address = element.replacement;
      }
      else if (action == IATAction::IAT_RESTORE)
      {
        element.replacement = pFT->u1.Function;
        address = element.original;
      }

      if (address != 0)
      {
        DWORD protect = 0;
        VirtualProtect(reinterpret_cast<LPVOID>(&pFT->u1.Function), sizeof(ULONG_PTR), PAGE_READWRITE, &protect);
        pFT->u1.Function = address;
        VirtualProtect(reinterpret_cast<LPVOID>(&pFT->u1.Function), sizeof(ULONG_PTR), protect, &protect);
      }

      return false;
    }
  
    return true;
  });

  return VU_OK;
}

VUResult CIATHookManager::Iterate(
  const std::string& module,
  std::function<bool(
    const std::string& module,
    const std::string& function,
    PIMAGE_THUNK_DATA& pOFT,
    PIMAGE_THUNK_DATA& pFT)> fn)
{
  if (module.empty())
  {
    return 1;
  }

  auto pBase = GetModuleHandleA(module.c_str());
  if (pBase == nullptr)
  {
    return 2;
  }

  auto pDOSHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(pBase);
  assert(pDOSHeader != nullptr);

  auto pNTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
    reinterpret_cast<ULONG_PTR>(pBase) + pDOSHeader->e_lfanew);
  assert(pNTHeader != nullptr);

  auto pIDD = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
  if (pIDD.VirtualAddress == 0 || pIDD.Size == 0) // did not import any module
  {
    return 2;
  }

  auto pIID = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
    reinterpret_cast<ULONG_PTR>(pBase) + pIDD.VirtualAddress);
  assert(pIID != nullptr);

  bool breaked = false;

  for (int i = 0; pIID->FirstThunk != 0; i++, pIID++)
  {
    std::string module = reinterpret_cast<ULONG_PTR>(pBase) + reinterpret_cast<LPCSTR>(pIID->Name);

    auto pFT = reinterpret_cast<PIMAGE_THUNK_DATA>(
      reinterpret_cast<ULONG_PTR>(pBase) + pIID->FirstThunk);

    auto pOFT = reinterpret_cast<PIMAGE_THUNK_DATA>(
      reinterpret_cast<ULONG_PTR>(pBase) + pIID->OriginalFirstThunk);

    for (int j = 0; pOFT->u1.AddressOfData != 0; j++, pFT++, pOFT++)
    {
      std::string function = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
        reinterpret_cast<ULONG_PTR>(pBase) + pOFT->u1.AddressOfData)->Name;

      breaked = !fn(module, function, pOFT, pFT);
      if (breaked)
      {
        break;
      }
    }

    if (breaked)
    {
      break;
    }
  }

  return VU_OK;
}

DEF_SAMPLE(IATHook)
{
  CIATHookManager::Instance().Override("msvcp110d.dll", "KERNEL32.dll", "IsDebuggerPresent", 0x160991);
  system("PAUSE");

  CIATHookManager::Instance().Restore("msvcp110d.dll", "KERNEL32.dll", "IsDebuggerPresent");
  system("PAUSE");

  CIATHookManager::Instance().Override("msvcp110d.dll", "KERNEL32.dll", "IsDebuggerPresent", 0);
  system("PAUSE");

  return vu::VU_OK;
}
