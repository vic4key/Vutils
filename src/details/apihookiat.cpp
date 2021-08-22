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

struct sIATElement
{
  std::string target;
  std::string module;
  std::string function;
  const void* original;
  const void* replacement;

  sIATElement() : target(""), module(""), function(""), original(nullptr), replacement(nullptr) {}

  sIATElement(
    const std::string& t,
    const std::string& m,
    const std::string& f,
    const void* o = 0,
    const void* r = 0) : target(t), module(m), function(f), original(o), replacement(r) {}

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
      upper_string_A(target) == upper_string_A(right.target) &&
      upper_string_A(module) == upper_string_A(right.module) &&
      upper_string_A(function) == upper_string_A(right.function);
  }

  bool operator!=(const sIATElement& right) const
  {
    return !(*this == right);
  }
};

/**
 * IATHookingA
 */

IATHookingA::IATHookingA()
{
}

IATHookingA::~IATHookingA()
{
}

IATHookingA::IATElements::iterator IATHookingA::find(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  return std::find(m_iat_elements.begin(), m_iat_elements.end(), sIATElement(target, module, function));
}

IATHookingA::IATElements::iterator IATHookingA::find(const sIATElement& element)
{
  return this->find(element.target, element.module, element.function);
}

bool IATHookingA::exist(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  return this->find(target, module, function) != m_iat_elements.end();
}

VUResult IATHookingA::install(
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

  auto element = sIATElement(target, module, function, nullptr, replacement);

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

VUResult IATHookingA::uninstall(
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

VUResult IATHookingA::perform(const IATAction action, sIATElement& element)
{
  if (element.target.empty() || element.module.empty() || element.function.empty())
  {
    return 1;
  }

  this->iterate(element.target, [&](
    const std::string& m, const std::string& f, PIMAGE_THUNK_DATA& ptr_iat, PIMAGE_THUNK_DATA& ptr_int)
  {
    // msg_debug_A("[%p] [%p] %s!%s\n", ptr_iat->u1.Function, ptr_int->u1.Function, m.c_str(), f.c_str());
  
    if (element == sIATElement(element.target, m, f))
    {
      const void* address = 0;

      if (action == IATAction::IAT_INSTALL)
      {
        element.original = reinterpret_cast<const void*>(ptr_iat->u1.Function);
        address = element.replacement;
      }
      else if (action == IATAction::IAT_UNINSTALL)
      {
        element.replacement = reinterpret_cast<const void*>(ptr_iat->u1.Function);
        address = element.original;
      }

      if (address != 0)
      {
        ulong protect = 0;
        VirtualProtect(LPVOID(&ptr_iat->u1.Function), sizeof(ulongptr), PAGE_READWRITE, &protect);
        ptr_iat->u1.Function = reinterpret_cast<ulongptr>(address);
        VirtualProtect(LPVOID(&ptr_iat->u1.Function), sizeof(ulongptr), protect, &protect);
      }

      return false;
    }
  
    return true;
  });

  return VU_OK;
}

VUResult IATHookingA::iterate(
  const std::string& module,
  std::function<bool(
    const std::string& module,
    const std::string& function,
    PIMAGE_THUNK_DATA& ptr_iat,
    PIMAGE_THUNK_DATA& ptr_int)> fn)
{
  if (module.empty())
  {
    return 1;
  }

  auto ptr_base = GetModuleHandleA(module.c_str());
  if (ptr_base == nullptr)
  {
    return 2;
  }

  auto ptr_dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(ptr_base);
  assert(ptr_dos_header != nullptr);

  auto ptr_nt_header = reinterpret_cast<PIMAGE_NT_HEADERS>(
    reinterpret_cast<ulongptr>(ptr_base) + ptr_dos_header->e_lfanew);
  assert(ptr_nt_header != nullptr);

  auto ptr_idd = ptr_nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
  if (ptr_idd.VirtualAddress == 0 || ptr_idd.Size == 0) // did not import any module
  {
    return 2;
  }

  auto ptr_iid = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
    reinterpret_cast<ulongptr>(ptr_base) + ptr_idd.VirtualAddress);
  assert(ptr_iid != nullptr);

  bool breaked = false;

  for (int i = 0; ptr_iid->FirstThunk != 0; i++, ptr_iid++)
  {
    std::string module = reinterpret_cast<ulongptr>(ptr_base) + reinterpret_cast<LPCSTR>(ptr_iid->Name);

    auto ptr_iat = reinterpret_cast<PIMAGE_THUNK_DATA>(
      reinterpret_cast<ulongptr>(ptr_base) + ptr_iid->FirstThunk); // IAT - Import Address Table

    auto ptr_int = reinterpret_cast<PIMAGE_THUNK_DATA>(
      reinterpret_cast<ulongptr>(ptr_base) + ptr_iid->OriginalFirstThunk); // INT - Import Name Table

    for (; ptr_int->u1.AddressOfData != 0; ptr_iat++, ptr_int++)
    {
      std::string function = reinterpret_cast<char*>(reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
        reinterpret_cast<ulongptr>(ptr_base) + ptr_int->u1.AddressOfData)->Name);

      breaked = !fn(module, function, ptr_iat, ptr_int);
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
 * IATHookingW
 */

IATHookingW::IATHookingW()
{
}

IATHookingW::~IATHookingW()
{
}

VUResult IATHookingW::install(
  const std::wstring& target,
  const std::wstring& module,
  const std::wstring& function,
  const void* replacement,
  const void** original)
{
  return IATHookingA::instance().install(
    to_string_A(target), to_string_A(module), to_string_A(function), replacement, original);
}

VUResult IATHookingW::uninstall(
  const std::wstring& target,
  const std::wstring& module,
  const std::wstring& function,
  const void** replacement)
{
  return IATHookingA::instance().uninstall(
    to_string_A(target), to_string_A(module), to_string_A(function), replacement);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu