/**
 * @file   iathook.cpp
 * @author Vic P.
 * @brief  Implementation for IAT API Hooking
 */

#include "Vutils.h"

#include <algorithm>

namespace vu
{

/**
 * IATElement
 */

struct IATElement
{
  std::string target;
  std::string module;
  std::string function;
  ulongptr original;
  ulongptr replacement;

  IATElement() : target(""), module(""), function(""), original(0), replacement(0) {}

  IATElement(
    const std::string& t,
    const std::string& m,
    const std::string& f,
    const ulongptr o = 0,
    const ulongptr c = 0) : target(t), module(m), function(f), original(o), replacement(c) {}

  const IATElement& operator=(const IATElement& right)
  {
    target = right.target;
    module = right.module;
    function = right.function;
    original = right.original;
    replacement = right.replacement;
    return *this;
  }

  bool operator==(const IATElement& right) const
  {
    return\
      UpperStringA(target) == UpperStringA(right.target) &&
      UpperStringA(module) == UpperStringA(right.module) &&
      UpperStringA(function) == UpperStringA(right.function);
  }

  bool operator!=(const IATElement& right) const
  {
    return !(*this == right);
  }
};

/**
 * CIATHookManagerA
 */

CIATHookManagerA::CIATHookManagerA()
{
}

CIATHookManagerA::~CIATHookManagerA()
{
}

CIATHookManagerA::IATElements::iterator CIATHookManagerA::Find(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  return std::find(m_IATElements.begin(), m_IATElements.end(), IATElement(target, module, function));
}

CIATHookManagerA::IATElements::iterator CIATHookManagerA::Find(const IATElement& element)
{
  return this->Find(element.target, element.module, element.function);
}

bool CIATHookManagerA::Exist(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  return this->Find(target, module, function) != m_IATElements.end();
}

VUResult CIATHookManagerA::Override(
  const std::string& target,
  const std::string& module,
  const std::string& function,
  const ulongptr replacement,
  ulongptr** original)
{
  if (this->Exist(target, module, function))
  {
    return 1;
  }

  auto element = IATElement(target, module, function, 0, replacement);

  if (this->Do(IATAction::IAT_OVERRIDE, element) != VU_OK)
  {
    return 2;
  }

  m_IATElements.emplace_back(element);

  if (original != nullptr)
  {
    *original = reinterpret_cast<ulongptr*>(element.original);
  }

  return VU_OK;
}

VUResult CIATHookManagerA::Restore(
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

VUResult CIATHookManagerA::Do(const IATAction action, IATElement& element)
{
  if (element.target.empty() || element.module.empty() || element.function.empty())
  {
    return 1;
  }

  this->Iterate(element.target, [&](
    const std::string& m, const std::string& f, PIMAGE_THUNK_DATA& pOFT, PIMAGE_THUNK_DATA& pFT)
  {
    // MsgA("[%p] [%p] %s!%s\n", pOFT->u1.Function, pFT->u1.Function, m.c_str(), f.c_str());
  
    if (element == IATElement(element.target, m, f))
    {
      ulongptr address = 0;

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
        ulong protect = 0;
        VirtualProtect(LPVOID(&pFT->u1.Function), sizeof(ulongptr), PAGE_READWRITE, &protect);
        pFT->u1.Function = address;
        VirtualProtect(LPVOID(&pFT->u1.Function), sizeof(ulongptr), protect, &protect);
      }

      return false;
    }
  
    return true;
  });

  return VU_OK;
}

VUResult CIATHookManagerA::Iterate(
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
    reinterpret_cast<ulongptr>(pBase) + pDOSHeader->e_lfanew);
  assert(pNTHeader != nullptr);

  auto pIDD = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
  if (pIDD.VirtualAddress == 0 || pIDD.Size == 0) // did not import any module
  {
    return 2;
  }

  auto pIID = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
    reinterpret_cast<ulongptr>(pBase) + pIDD.VirtualAddress);
  assert(pIID != nullptr);

  bool breaked = false;

  for (int i = 0; pIID->FirstThunk != 0; i++, pIID++)
  {
    std::string module = reinterpret_cast<ulongptr>(pBase) + reinterpret_cast<LPCSTR>(pIID->Name);

    auto pFT = reinterpret_cast<PIMAGE_THUNK_DATA>(
      reinterpret_cast<ulongptr>(pBase) + pIID->FirstThunk);

    auto pOFT = reinterpret_cast<PIMAGE_THUNK_DATA>(
      reinterpret_cast<ulongptr>(pBase) + pIID->OriginalFirstThunk);

    for (int j = 0; pOFT->u1.AddressOfData != 0; j++, pFT++, pOFT++)
    {
      std::string function = reinterpret_cast<char*>(reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
        reinterpret_cast<ulongptr>(pBase) + pOFT->u1.AddressOfData)->Name);

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

/**
 * CIATHookManagerW
 */

CIATHookManagerW::CIATHookManagerW()
{
}

CIATHookManagerW::~CIATHookManagerW()
{
}

VUResult CIATHookManagerW::Override(
  const std::wstring& target,
  const std::wstring& module,
  const std::wstring& function,
  const ulongptr replacement,
  ulongptr** original)
{
  return CIATHookManagerA::Instance().Override(
    ToStringA(target), ToStringA(module), ToStringA(function), replacement, original);
}

VUResult CIATHookManagerW::Restore(
  const std::wstring& target,
  const std::wstring& module,
  const std::wstring& function)
{
  return CIATHookManagerA::Instance().Restore(
    ToStringA(target), ToStringA(module), ToStringA(function));
}

} // namespace vu