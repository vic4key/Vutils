/**
 * @file   apihookiat.cpp
 * @author Vic P.
 * @brief  Implementation for IAT API Hooking
 */

#include "Vutils.h"

#include <algorithm>

namespace vu
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 6011)
#pragma warning(disable: 4312)
#endif // _MSC_VER

/**
 * IATElement
 */

struct IATElement
{
  std::string target;
  std::string module;
  std::string function;
  const void* original;
  const void* replacement;

  IATElement() : target(""), module(""), function(""), original(nullptr), replacement(nullptr) {}

  IATElement(
    const std::string& t,
    const std::string& m,
    const std::string& f,
    const void* o = 0,
    const void* r = 0) : target(t), module(m), function(f), original(o), replacement(r) {}

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
      upper_string_A(target) == upper_string_A(right.target) &&
      upper_string_A(module) == upper_string_A(right.module) &&
      upper_string_A(function) == upper_string_A(right.function);
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

CIATHookManagerA::IATElements::iterator CIATHookManagerA::find(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  return std::find(m_iat_elements.begin(), m_iat_elements.end(), IATElement(target, module, function));
}

CIATHookManagerA::IATElements::iterator CIATHookManagerA::find(const IATElement& element)
{
  return this->find(element.target, element.module, element.function);
}

bool CIATHookManagerA::exist(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  return this->find(target, module, function) != m_iat_elements.end();
}

VUResult CIATHookManagerA::install(
  const std::string& target,
  const std::string& module,
  const std::string& function,
  const void* replacement,
  const void** original)
{
  if (this->exist(target, module, function))
  {
    return 1;
  }

  auto element = IATElement(target, module, function, nullptr, replacement);

  if (this->perform(IATAction::IAT_INSTALL, element) != VU_OK)
  {
    return 2;
  }

  if (original != nullptr)
  {
    *original = element.original;
  }

  m_iat_elements.emplace_back(element);

  return VU_OK;
}

VUResult CIATHookManagerA::uninstall(
  const std::string& target,
  const std::string& module,
  const std::string& function,
  const void** replacement)
{
  auto it = this->find(target, module, function);
  if (it == m_iat_elements.end())
  {
    return 1;
  }

  auto element = *it;

  if (this->perform(IATAction::IAT_UNINSTALL, element) != VU_OK)
  {
    return 2;
  }

  if (replacement != nullptr)
  {
    *replacement = element.replacement;
  }

  m_iat_elements.erase(it);

  return VU_OK;
}

VUResult CIATHookManagerA::perform(const IATAction action, IATElement& element)
{
  if (element.target.empty() || element.module.empty() || element.function.empty())
  {
    return 1;
  }

  this->iterate(element.target, [&](
    const std::string& m, const std::string& f, PIMAGE_THUNK_DATA& pOFT, PIMAGE_THUNK_DATA& pFT)
  {
    // MsgA("[%p] [%p] %s!%s\n", pOFT->u1.Function, pFT->u1.Function, m.c_str(), f.c_str());
  
    if (element == IATElement(element.target, m, f))
    {
      const void* address = 0;

      if (action == IATAction::IAT_INSTALL)
      {
        element.original = reinterpret_cast<const void*>(pFT->u1.Function);
        address = element.replacement;
      }
      else if (action == IATAction::IAT_UNINSTALL)
      {
        element.replacement = reinterpret_cast<const void*>(pFT->u1.Function);
        address = element.original;
      }

      if (address != 0)
      {
        ulong protect = 0;
        VirtualProtect(LPVOID(&pFT->u1.Function), sizeof(ulongptr), PAGE_READWRITE, &protect);
        pFT->u1.Function = reinterpret_cast<ulongptr>(address);
        VirtualProtect(LPVOID(&pFT->u1.Function), sizeof(ulongptr), protect, &protect);
      }

      return false;
    }
  
    return true;
  });

  return VU_OK;
}

VUResult CIATHookManagerA::iterate(
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

    auto ptr_iat = reinterpret_cast<PIMAGE_THUNK_DATA>(
      reinterpret_cast<ulongptr>(pBase) + pIID->FirstThunk); // IAT - Import Address Table

    auto ptr_int = reinterpret_cast<PIMAGE_THUNK_DATA>(
      reinterpret_cast<ulongptr>(pBase) + pIID->OriginalFirstThunk); // INT - Import Name Table

    for (; ptr_int->u1.AddressOfData != 0; ptr_iat++, ptr_int++)
    {
      std::string function = reinterpret_cast<char*>(reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
        reinterpret_cast<ulongptr>(pBase) + ptr_int->u1.AddressOfData)->Name);

      breaked = !fn(module, function, ptr_int, ptr_iat);
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

VUResult CIATHookManagerW::install(
  const std::wstring& target,
  const std::wstring& module,
  const std::wstring& function,
  const void* replacement,
  const void** original)
{
  return CIATHookManagerA::Instance().install(
    to_string_A(target), to_string_A(module), to_string_A(function), replacement, original);
}

VUResult CIATHookManagerW::uninstall(
  const std::wstring& target,
  const std::wstring& module,
  const std::wstring& function,
  const void** replacement)
{
  return CIATHookManagerA::Instance().uninstall(
    to_string_A(target), to_string_A(module), to_string_A(function), replacement);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu