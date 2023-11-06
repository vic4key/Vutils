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
 * IATHookingA::Entry
 */

IATHookingA::Entry::Entry()
  : target(""), module(""), function("")
  , original(nullptr), replacement(nullptr)
{
}

IATHookingA::Entry::Entry(
  const std::string& t, const std::string& m, const std::string& f,
  void* o, void* r)
  : target(t), module(m), function(f)
  , original(o), replacement(r)
{
}

IATHookingA::Entry::Entry(const Entry& right)
{
  *this = right;
}

const IATHookingA::Entry& IATHookingA::Entry::operator=(const Entry& right)
{
  if (this != &right)
  {
    target      = right.target;
    module      = right.module;
    function    = right.function;
    original    = right.original;
    replacement = right.replacement;
  }

  return *this;
}

bool IATHookingA::Entry::operator==(const Entry& right) const
{
  if (this == &right)
  {
    return true;
  }

  return\
    upper_string_A(target)   == upper_string_A(right.target) &&
    upper_string_A(module)   == upper_string_A(right.module) &&
    upper_string_A(function) == upper_string_A(right.function);
}

bool IATHookingA::Entry::operator!=(const Entry& right) const
{
  return !(*this == right);
}

/**
 * IATHookingA
 */

IATHookingA::IATHookingA()
{
}

IATHookingA::~IATHookingA()
{
}

IATHookingA::EntryList::iterator IATHookingA::find(
  const std::string& target,
  const std::string& module,
  const std::string& function)
{
  if (module.empty() || function.empty())
  {
    return m_iat_entry_list.end();
  }

  return std::find(m_iat_entry_list.begin(), m_iat_entry_list.end(), Entry(target, module, function));
}

IATHookingA::EntryList::iterator IATHookingA::find(const Entry& entry)
{
  return this->find(entry.target, entry.module, entry.function);
}

bool IATHookingA::exist(
  const std::string& target,
  const std::string& module,
  const std::string& function,
  Entry* ptr_entry)
{
  auto it = this->find(target, module, function);
  if (it == m_iat_entry_list.end())
  {
    return false;
  }

  if (ptr_entry != nullptr)
  {
    *ptr_entry = *it;
  }

  return true;
}

VUResult IATHookingA::install(
  const std::string& target,
  const std::string& module,
  const std::string& function,
  void* replacement,
  void** original)
{
  if (this->exist(target, module, function))
  {
    return 1;
  }

  Entry entry(target, module, function, nullptr, replacement);

  if (this->perform(iat_action::IAT_INSTALL, entry) != VU_OK)
  {
    return 2;
  }

  if (original != nullptr)
  {
    *original = entry.original;
  }

  m_iat_entry_list.emplace_back(entry);

  return VU_OK;
}

VUResult IATHookingA::uninstall(
  const std::string& target,
  const std::string& module,
  const std::string& function,
  void** replacement)
{
  auto it = this->find(target, module, function);
  if (it == m_iat_entry_list.end())
  {
    return 1;
  }

  auto& entry = *it;

  if (this->perform(iat_action::IAT_UNINSTALL, entry) != VU_OK)
  {
    return 2;
  }

  if (replacement != nullptr)
  {
    *replacement = entry.replacement;
  }

  m_iat_entry_list.erase(it);

  return VU_OK;
}

VUResult IATHookingA::perform(const iat_action action, Entry& entry)
{
  if (entry.target.empty() || entry.module.empty() || entry.function.empty())
  {
    return 1;
  }

  this->iterate(entry.target, [&](
    const std::string& m, const std::string& f, PIMAGE_THUNK_DATA& ptr_iat, PIMAGE_THUNK_DATA& ptr_int)
  {
    if (entry == Entry(entry.target, m, f))
    {
      const void* address = 0;

      if (action == iat_action::IAT_INSTALL)
      {
        entry.original = reinterpret_cast<void*>(ptr_iat->u1.Function);
        address = entry.replacement;
      }
      else if (action == iat_action::IAT_UNINSTALL)
      {
        entry.replacement = reinterpret_cast<void*>(ptr_iat->u1.Function);
        address = entry.original;
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
  void* replacement,
  void** original)
{
  return IATHookingA::instance().install(
    to_string_A(target), to_string_A(module), to_string_A(function), replacement, original);
}

VUResult IATHookingW::uninstall(
  const std::wstring& target,
  const std::wstring& module,
  const std::wstring& function,
  void** replacement)
{
  return IATHookingA::instance().uninstall(
    to_string_A(target), to_string_A(module), to_string_A(function), replacement);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu