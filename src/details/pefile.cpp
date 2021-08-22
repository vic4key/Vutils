/**
 * @file   pefile.cpp
 * @author Vic P.
 * @brief  Implementation for PE File
 */

#include "Vutils.h"

#include <cassert>

namespace vu
{

template class PEFileTX<ulong32>;
template class PEFileTX<ulong64>;

typedef struct _IMAGE_BASE_RELOCATION_ENTRY
{
  USHORT offset : 12;
  USHORT type : 4;
} IMAGE_BASE_RELOCATION_ENTRY, * PIMAGE_BASE_RELOCATION_ENTRY;

#define COUNT_RELOCATION_ENTRY(ptr)\
  (ptr == nullptr ? 0 : (PIMAGE_BASE_RELOCATION(ptr)->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(IMAGE_BASE_RELOCATION_ENTRY))

template<typename T>
PEFileTX<T>::PEFileTX()
{
  m_initialized = false;

  m_ptr_base = nullptr;
  m_ptr_dos_header = nullptr;
  m_ptr_pe_header  = nullptr;
  m_section_headers.clear();
  m_import_descriptors.clear();
  m_ex_iids.clear();
  m_import_functions.clear();
  m_relocation_entries.clear();

  if (sizeof(T) == 4)
  {
    m_ordinal_flag = (T)IMAGE_ORDINAL_FLAG32;
  }
  else
  {
    m_ordinal_flag = (T)IMAGE_ORDINAL_FLAG64;
  }
}

template<typename T>
PEFileTX<T>::~PEFileTX() {};

template<typename T>
void* vuapi PEFileTX<T>::get_ptr_base()
{
  return m_ptr_base;
}

template<typename T>
TPEHeaderT<T>* vuapi PEFileTX<T>::get_ptr_pe_header()
{
  if (!m_initialized)
  {
    return nullptr;
  }

  return m_ptr_pe_header;
}

template<typename T>
const std::vector<PSectionHeader>& vuapi PEFileTX<T>::get_setion_headers(bool in_cache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (in_cache && !m_section_headers.empty())
  {
    return m_section_headers;
  }

  m_section_headers.clear();

  vu::PSectionHeader ptr_section_header = (PSectionHeader)((ulong64)m_ptr_pe_header + sizeof(vu::TNTHeaderT<T>));
  if (ptr_section_header == nullptr)
  {
    return m_section_headers;
  }

  m_section_headers.clear();
  for (int i = 0; i < m_ptr_pe_header->FileHeader.NumberOfSections; i++)
  {
    m_section_headers.push_back(ptr_section_header);
    ptr_section_header++;
  }

  return m_section_headers;
}

// IMAGE_REL_BASED_<X>
// static const char* RelocationEntryTypes[] =
// {
//   "ABSOLUTE",
//   "HIGH",
//   "LOW",
//   "HIGHLOW",
//   "HIGHADJ",
//   "MACHINE_SPECIFIC_5",
//   "RESERVED",
//   "MACHINE_SPECIFIC_7",
//   "MACHINE_SPECIFIC_8",
//   "MACHINE_SPECIFIC_9",
//   "DIR64",
// };

template<typename T>
const std::vector<sRelocationEntryT<T>> vuapi PEFileTX<T>::get_relocation_entries(bool in_cache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (in_cache && !m_relocation_entries.empty())
  {
    return m_relocation_entries;
  }

  auto idd = this->get_ptr_pe_header()->OptHeader.Relocation;

  for (DWORD size = 0; size < idd.Size; )
  {
    auto ptr = PUCHAR(reinterpret_cast<ulong64>(this->get_ptr_base()) + this->rva_to_offset(idd.VirtualAddress) + size);
    assert(ptr != nullptr);

    auto ptr_base_relocation = PIMAGE_BASE_RELOCATION(ptr);
    assert(ptr_base_relocation != nullptr);

    auto n_entries = COUNT_RELOCATION_ENTRY(ptr);
    ptr += sizeof(IMAGE_BASE_RELOCATION);

    for (DWORD idx = 0; idx < n_entries; idx++)
    {
      const auto ptr_entry = PIMAGE_BASE_RELOCATION_ENTRY(ptr + idx * sizeof(IMAGE_BASE_RELOCATION_ENTRY));
      assert(ptr_entry);

      sRelocationEntryT<T> entry = { 0 };
      entry.RVA = ptr_base_relocation->VirtualAddress + ptr_entry->offset;
      auto offset = this->rva_to_offset(entry.RVA);
      // Entry.Offset = this->RVA2Offset(Entry.RVA);
      // Entry.VA = this->GetpPEHeader()->OptHeader.ImageBase + Entry.RVA;
      entry.Value = *reinterpret_cast<T*>(reinterpret_cast<ulong64>(this->get_ptr_base()) + offset);

      m_relocation_entries.push_back(std::move(entry));
    }

    size += ptr_base_relocation->SizeOfBlock;
  }

  return m_relocation_entries;
}

template<typename T>
const std::vector<sExIID>& vuapi PEFileTX<T>::get_ex_iids(bool in_cache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (in_cache && !m_ex_iids.empty())
  {
    return m_ex_iids;
  }

  m_ex_iids.clear();

  T iid_offset = this->rva_to_offset(m_ptr_pe_header->OptHeader.Import.VirtualAddress);
  if (iid_offset == T(-1))
  {
    return m_ex_iids;
  }

  auto ptr_iid = (PImportDescriptor)((ulong64)m_ptr_base + iid_offset);
  if (ptr_iid == nullptr)
  {
    return m_ex_iids;
  }

  m_ex_iids.clear();

  for (int i = 0; ptr_iid->FirstThunk != 0; i++, ptr_iid++)
  {
    sExIID ex_iid;
    ex_iid.iid_id = i;
    ex_iid.name = (char*)((ulong64)m_ptr_base + this->rva_to_offset(ptr_iid->Name));;
    ex_iid.ptr_iid = ptr_iid;

    m_ex_iids.push_back(std::move(ex_iid));
  }

  return m_ex_iids;
}

template<typename T>
const std::vector<PImportDescriptor>& vuapi PEFileTX<T>::get_import_descriptors(bool in_cache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (in_cache && !m_import_descriptors.empty())
  {
    return m_import_descriptors;
  }

  this->get_ex_iids(in_cache);

  m_import_descriptors.clear();

  for (const auto& e: m_ex_iids)
  {
    m_import_descriptors.push_back(e.ptr_iid);
  }

  return m_import_descriptors;
}

template<typename T>
const std::vector<sImportModule> vuapi PEFileTX<T>::get_import_modules(bool in_cache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (in_cache && !m_import_modules.empty())
  {
    return m_import_modules;
  }

  m_import_modules.clear();

  this->get_ex_iids(in_cache);

  for (const auto& e: m_ex_iids)
  {
    sImportModule mi;
    mi.iid_id = e.iid_id;
    mi.name = e.name;
    // mi.NumberOfFuctions = 0;

    m_import_modules.push_back(std::move(mi));
  }

  return m_import_modules;
}

template<typename T>
const std::vector<sImportFunctionT<T>> vuapi PEFileTX<T>::get_import_functions(bool in_cache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (in_cache && !m_import_functions.empty())
  {
    return m_import_functions;
  }

  this->get_ex_iids(in_cache);

  m_import_functions.clear();

  TThunkDataT<T>* ptr_thunk_data = nullptr;
  sImportFunctionT<T> funcInfo;
  for (const auto& e: m_ex_iids)
  {
    T offset = this->rva_to_offset(e.ptr_iid->FirstThunk);
    if (offset == -1 || (ptr_thunk_data = (TThunkDataT<T>*)((ulong64)m_ptr_base + offset)) == nullptr) continue;
    do
    {
      if ((ptr_thunk_data->u1.AddressOfData & m_ordinal_flag) == m_ordinal_flag)   // Imported by ordinal
      {
        funcInfo.name = "";
        funcInfo.hint = -1;
        funcInfo.ordinal = ptr_thunk_data->u1.AddressOfData & ~m_ordinal_flag;
      }
      else   // Imported by name
      {
        offset = this->rva_to_offset(ptr_thunk_data->u1.AddressOfData);
        PImportByName p = (PImportByName)((ulong64)m_ptr_base + offset);
        if (offset != -1 && p != nullptr)
        {
          funcInfo.hint = p->Hint;
          funcInfo.ordinal = T(-1);
          funcInfo.name = (char*)p->Name;
        }
      }

      funcInfo.iid_id = e.iid_id;
      funcInfo.rva = ptr_thunk_data->u1.AddressOfData;
      m_import_functions.push_back(funcInfo);

      ptr_thunk_data++;
    }
    while (ptr_thunk_data->u1.AddressOfData != 0);
  }

  return m_import_functions;
}

template<typename T>
const sImportModule* vuapi PEFileTX<T>::find_ptr_import_module(const std::string& module_name, bool in_cache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  this->get_import_modules(in_cache);

  const sImportModule* result = nullptr;

  auto s1 = upper_string_A(module_name);

  for (const auto& e: m_import_modules)
  {
    auto s2 = upper_string_A(e.name);
    if (s1 == s2)
    {
      result = &e;
      break;
    }
  }

  return result;
}

template<typename T>
const sImportFunctionT<T>* vuapi PEFileTX<T>::find_ptr_import_function(
  const sImportFunctionT<T>& import_function,
  eImportedFunctionFindMethod method,
  bool in_cache
)
{
  if (!m_initialized)
  {
    assert(0);
  }

  const sImportFunctionT<T>* result = nullptr;

  this->get_import_functions(in_cache);

  switch (method)
  {
  case eImportedFunctionFindMethod::IFFM_HINT:
    for (const auto& e: m_import_functions)
    {
      if (e.hint == import_function.hint)
      {
        result = &e;
        break;
      }
    }
    break;

  case eImportedFunctionFindMethod::IFFM_NAME:
    for (const auto& e: m_import_functions)
    {
      if (e.name == import_function.name)
      {
        result = &e;
        break;
      }
    }
    break;

  default:
    break;
  }

  return result;
}

template<typename T>
const sImportFunctionT<T>* vuapi PEFileTX<T>::find_ptr_import_function(
  const std::string& function_name,
  bool in_cache)
{
  sImportFunctionT<T> o = {0};
  o.name = function_name;
  return this->find_ptr_import_function(o, eImportedFunctionFindMethod::IFFM_NAME);
}

template<typename T>
const sImportFunctionT<T>* vuapi PEFileTX<T>::find_ptr_import_function(
  const ushort function_hint,
  bool in_cache)
{
  sImportFunctionT<T> o = {0};
  o.hint = function_hint;
  return this->find_ptr_import_function(o, eImportedFunctionFindMethod::IFFM_HINT);
}

template<typename T>
T vuapi PEFileTX<T>::rva_to_offset(T rva, bool in_cache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (!in_cache || m_section_headers.empty())
  {
    this->get_setion_headers(false);
  }

  if (m_section_headers.empty())
  {
    return T(-1);
  }

  const auto& the_last_section = *m_section_headers.rbegin();

  std::pair<T, T> range(T(0), T(the_last_section->VirtualAddress) + T(the_last_section->Misc.VirtualSize));
  if (rva < range.first || rva > range.second)
  {
    return T(-1);
  }

  T result = rva;

  for (auto& e : m_section_headers)
  {
    if ((rva >= e->VirtualAddress) && (rva < (e->VirtualAddress + e->Misc.VirtualSize)))
    {
      result  = e->PointerToRawData;
      result += (rva - e->VirtualAddress);
      break;
    }
  }

  return result;
}

template<typename T>
T vuapi PEFileTX<T>::offset_to_rva(T offset, bool in_cache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (!in_cache || m_section_headers.empty())
  {
    this->get_setion_headers(false);
  }

  if (m_section_headers.empty())
  {
    return T(-1);
  }

  const auto& the_last_section = *m_section_headers.rbegin();

  std::pair<T, T> range(T(0), T(the_last_section->PointerToRawData) + T(the_last_section->SizeOfRawData));
  if (offset < range.first || offset > range.second)
  {
    return T(-1);
  }

  T result = offset;

  for (auto& e : m_section_headers)
  {
    if ((offset >= e->PointerToRawData) && (offset < (e->PointerToRawData + e->SizeOfRawData)))
    {
      result  = e->VirtualAddress;
      result += (offset - e->PointerToRawData);
      break;
    }
  }

  return result;
}

template class PEFileTA<ulong32>;
template class PEFileTA<ulong64>;

template<typename T>
PEFileTA<T>::PEFileTA(const std::string& pe_file_path)
{
  PEFileTX<T>::m_initialized = false;

  PEFileTX<T>::m_ptr_base = nullptr;
  PEFileTX<T>::m_ptr_dos_header = nullptr;
  PEFileTX<T>::m_ptr_pe_header = nullptr;

  m_file_path = pe_file_path;
}

template<typename T>
PEFileTA<T>::~PEFileTA()
{
  if (PEFileTX<T>::m_initialized)
  {
    m_file_map.close();
  }
}

template<typename T>
VUResult vuapi PEFileTA<T>::parse()
{
  if (m_file_path.empty())
  {
    return 1;
  }

  if (!is_file_exists_A(m_file_path))
  {
    return 2;
  }

  if (m_file_map.create_within_file(
    m_file_path, 0, 0,
    eFSGenericFlags::FG_READ,
    eFSShareFlags::FS_READ,
    eFSModeFlags::FM_OPENALWAYS,
    eFSAttributeFlags::FA_NORMAL,
    ePageProtection::PP_READ_ONLY) != vu::VU_OK)
  {
    return 3;
  }

  PEFileTX<T>::m_ptr_base = m_file_map.view(eFMDesiredAccess::DA_READ);
  if (PEFileTX<T>::m_ptr_base == nullptr)
  {
    return 4;
  }

  PEFileTX<T>::m_ptr_dos_header = (PDOSHeader)PEFileTX<T>::m_ptr_base;
  if (PEFileTX<T>::m_ptr_dos_header == nullptr)
  {
    return 5;
  }

  PEFileTX<T>::m_ptr_pe_header = (TPEHeaderT<T>*)((ulong64)PEFileTX<T>::m_ptr_base + PEFileTX<T>::m_ptr_dos_header->e_lfanew);
  if (PEFileTX<T>::m_ptr_pe_header == nullptr)
  {
    return 6;
  }

  if (sizeof(T) == sizeof(pe32))
  {
    if (PEFileTX<T>::m_ptr_pe_header->OptHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else if (sizeof(T) == sizeof(pe64))
  {
    if (PEFileTX<T>::m_ptr_pe_header->OptHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else
  {
    return 8; // The curent type data was not supported
  }

  PEFileTX<T>::m_initialized = true;

  return VU_OK;
}

template class PEFileTW<ulong32>;
template class PEFileTW<ulong64>;

template<typename T>
PEFileTW<T>::PEFileTW(const std::wstring& pe_file_path)
{
  PEFileTX<T>::m_initialized = false;

  PEFileTX<T>::m_ptr_base = nullptr;
  PEFileTX<T>::m_ptr_dos_header = nullptr;
  PEFileTX<T>::m_ptr_pe_header  = nullptr;

  m_file_path = pe_file_path;
}

template<typename T>
PEFileTW<T>::~PEFileTW()
{
  if (PEFileTX<T>::m_initialized)
  {
    m_file_map.close();
  }
}

template<typename T>
VUResult vuapi PEFileTW<T>::parse()
{
  if (m_file_path.empty())
  {
    return 1;
  }

  if (!is_file_exists_W(m_file_path))
  {
    return 2;
  }

  if (m_file_map.create_within_file(m_file_path, 0, 0,
    eFSGenericFlags::FG_READ,
    eFSShareFlags::FS_READ,
    eFSModeFlags::FM_OPENALWAYS,
    eFSAttributeFlags::FA_NORMAL,
    ePageProtection::PP_READ_ONLY) != vu::VU_OK)
  {
    return 3;
  }

  PEFileTX<T>::m_ptr_base = m_file_map.view(eFMDesiredAccess::DA_READ);
  if (PEFileTX<T>::m_ptr_base == nullptr)
  {
    return 4;
  }

  PEFileTX<T>::m_ptr_dos_header = (PDOSHeader)PEFileTX<T>::m_ptr_base;
  if (PEFileTX<T>::m_ptr_dos_header == nullptr)
  {
    return 5;
  }

  PEFileTX<T>::m_ptr_pe_header = (TPEHeaderT<T>*)((ulong64)PEFileTX<T>::m_ptr_base + PEFileTX<T>::m_ptr_dos_header->e_lfanew);
  if (PEFileTX<T>::m_ptr_pe_header == nullptr)
  {
    return 6;
  }

  if (sizeof(T) == sizeof(pe32))
  {
    if (PEFileTX<T>::m_ptr_pe_header->OptHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else if (sizeof(T) == sizeof(pe64))
  {
    if (PEFileTX<T>::m_ptr_pe_header->OptHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else
  {
    return 8; // The curent type data was not supported
  }

  PEFileTX<T>::m_initialized = true;

  return VU_OK;
}

} // namespace vu