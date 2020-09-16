#pragma once

#include "Sample.h"

using namespace vu;

#include <algorithm>

struct IATObject
{
  std::string target;
  std::string module;
  std::string function;
  ULONG_PTR original;
  ULONG_PTR replacement;

  IATObject()
    : target(""), module(""), function(""), original(0), replacement(0) {}

  IATObject(
    const std::string& t,
    const std::string& m,
    const std::string& f,
    const ULONG_PTR o = 0,
    const ULONG_PTR c = 0)
    : target(t), module(m), function(f), original(o), replacement(c) {}

  const IATObject& operator=(const IATObject& right)
  {
    target = right.target;
    module = right.module;
    function = right.function;
    original = right.original;
    replacement = right.replacement;
    return *this;
  }

  bool operator==(const IATObject& right) const
  {
    return\
      UpperStringA(target) == UpperStringA(right.target) &&
      UpperStringA(module) == UpperStringA(right.module) &&
      UpperStringA(function) == UpperStringA(right.function);
  }

  bool operator!=(const IATObject& right) const
  {
    return !(*this == right);
  }
};

class IATManager : public CSingletonT<IATManager>
{
public:
  enum IATAction
  {
    IAT_OVERRIDE,
    IAT_RESTORE,
    IAT_MARK,
  };

  typedef std::vector<IATObject> IATObjects;

  IATManager();
  virtual ~IATManager();

  VUResult Mark(
    const std::string& target,
    const std::string& module,
    const std::string& function,
    const ULONG_PTR replacement = 0);

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
  IATObjects::iterator Find(const IATObject& object);

  IATObjects::iterator Find(
    const std::string& target,
    const std::string& module,
    const std::string& function);

  bool Exist(
    const std::string& target,
    const std::string& module,
    const std::string& function);

  VUResult Do(const IATAction action, IATObject& object);

private:
  IATObjects m_IATObjects;
};

IATManager::IATManager()
{
}

IATManager::~IATManager()
{
}

IATManager::IATObjects::iterator IATManager::Find(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  return std::find(m_IATObjects.begin(), m_IATObjects.end(), IATObject(target, module, function));
}

IATManager::IATObjects::iterator IATManager::Find(const IATObject& object)
{
  return this->Find(object.target, object.module, object.function);
}

bool IATManager::Exist(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  return this->Find(target, module, function) != m_IATObjects.end();
}

VUResult IATManager::Mark(
  const std::string& target,
  const std::string& module,
  const std::string& function,
  const ULONG_PTR replacement)
{
  if (this->Exist(target, module, function))
  {
    return 1;
  }

  auto object = IATObject(target, module, function, 0, replacement);

  if (this->Do(IATAction::IAT_MARK, object) != VU_OK)
  {
    return 2;
  }

  m_IATObjects.emplace_back(object);

  return VU_OK;
}

VUResult IATManager::Override(
  const std::string& target,
  const std::string& module,
  const std::string& function,
  const ULONG_PTR replacement)
{
  if (this->Exist(target, module, function))
  {
    return 1;
  }

  auto object = IATObject(target, module, function, 0, replacement);

  if (this->Do(IATAction::IAT_OVERRIDE, object) != VU_OK)
  {
    return 2;
  }

  m_IATObjects.emplace_back(object);

  return VU_OK;
}

VUResult IATManager::Restore(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  auto it = this->Find(target, module, function);
  if (it == m_IATObjects.end())
  {
    return 1;
  }

  auto object = *it;

  if (this->Do(IATAction::IAT_RESTORE, object) != VU_OK)
  {
    return 2;
  }

  m_IATObjects.erase(it);

  return VU_OK;
}

VUResult IATManager::Do(const IATAction action, IATObject& object)
{
  if (object.target.empty() || object.module.empty() || object.function.empty())
  {
    return 1;
  }

  this->Iterate(object.target, [&](
    const std::string& m, const std::string& f, PIMAGE_THUNK_DATA& pOFT, PIMAGE_THUNK_DATA& pFT)
  {
    printf("[%p] [%p] %s!%s\n", pOFT->u1.Function, pFT->u1.Function, m.c_str(), f.c_str());
  
    if (object == IATObject(object.target, m, f))
    {
      switch (action)
      {
      case IATAction::IAT_OVERRIDE:
      case IATAction::IAT_RESTORE:
        {
          ULONG_PTR address = 0;

          if (action == IATAction::IAT_OVERRIDE)
          {
            object.original = pFT->u1.Function;
            address = object.replacement;
          }
          else if (action == IATAction::IAT_RESTORE)
          {
            object.replacement = pFT->u1.Function;
            address = object.original;
          }

          if (address != 0)
          {
            DWORD protect = 0;
            VirtualProtect(reinterpret_cast<LPVOID>(&pFT->u1.Function), sizeof(ULONG_PTR), PAGE_READWRITE, &protect);
            pFT->u1.Function = address;
            VirtualProtect(reinterpret_cast<LPVOID>(&pFT->u1.Function), sizeof(ULONG_PTR), protect, &protect);
          }
        }
        break;

      case IATAction::IAT_MARK:
        {
          object.original = pFT->u1.Function;
        }
        break;

      default:
        break;
      }

      return false;
    }
  
    return true;
  });

  return VU_OK;
}

VUResult IATManager::Iterate(
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
  IATManager::Instance().Override("msvcp110d.dll", "KERNEL32.dll", "IsDebuggerPresent", 0x160991);
  system("PAUSE");

  IATManager::Instance().Restore("msvcp110d.dll", "KERNEL32.dll", "IsDebuggerPresent");
  system("PAUSE");

  return vu::VU_OK;
}
