/**
 * @file   pefile.cpp
 * @author Vic P.
 * @brief  Implementation for PE File
 */

#include "Vutils.h"

#include <cassert>

namespace vu
{

template class CPEFileTX<ulong32>;
template class CPEFileTX<ulong64>;

typedef struct _IMAGE_BASE_RELOCATION_ENTRY
{
  USHORT Offset : 12;
  USHORT Type : 4;
} IMAGE_BASE_RELOCATION_ENTRY, * PIMAGE_BASE_RELOCATION_ENTRY;

#define COUNT_RELOCATION_ENTRY(ptr)\
  (ptr == nullptr ? 0 : (PIMAGE_BASE_RELOCATION(ptr)->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(IMAGE_BASE_RELOCATION_ENTRY))

template<typename T>
CPEFileTX<T>::CPEFileTX()
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
CPEFileTX<T>::~CPEFileTX() {};

template<typename T>
void* vuapi CPEFileTX<T>::get_ptr_base()
{
  return m_ptr_base;
}

template<typename T>
TPEHeaderT<T>* vuapi CPEFileTX<T>::get_ptr_pe_header()
{
  if (!m_initialized)
  {
    return nullptr;
  }

  return m_ptr_pe_header;
}

template<typename T>
const std::vector<PSectionHeader>& vuapi CPEFileTX<T>::get_setion_headers(bool InCache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (InCache && !m_section_headers.empty())
  {
    return m_section_headers;
  }

  m_section_headers.clear();

  vu::PSectionHeader pSH = (PSectionHeader)((ulong64)m_ptr_pe_header + sizeof(vu::TNTHeaderT<T>));
  if (pSH == nullptr)
  {
    return m_section_headers;
  }

  m_section_headers.clear();
  for (int i = 0; i < m_ptr_pe_header->FileHeader.NumberOfSections; i++)
  {
    m_section_headers.push_back(pSH);
    pSH++;
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
const std::vector<TRelocationEntryT<T>> vuapi CPEFileTX<T>::get_relocation_entries(bool InCache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (InCache && !m_relocation_entries.empty())
  {
    return m_relocation_entries;
  }

  auto IDD = this->get_ptr_pe_header()->OptHeader.Relocation;

  for (DWORD Size = 0; Size < IDD.Size; )
  {
    auto ptr = PUCHAR(reinterpret_cast<ulong64>(this->get_ptr_base()) + this->rva_to_offset(IDD.VirtualAddress) + Size);
    assert(ptr != nullptr);

    auto pIBR = PIMAGE_BASE_RELOCATION(ptr);
    assert(pIBR != nullptr);

    auto nEntries = COUNT_RELOCATION_ENTRY(ptr);
    ptr += sizeof(IMAGE_BASE_RELOCATION);

    for (DWORD idx = 0; idx < nEntries; idx++)
    {
      const auto pEntry = PIMAGE_BASE_RELOCATION_ENTRY(ptr + idx * sizeof(IMAGE_BASE_RELOCATION_ENTRY));
      assert(pEntry);

      TRelocationEntryT<T> Entry = { 0 };
      Entry.RVA = pIBR->VirtualAddress + pEntry->Offset;
      auto Offset = this->rva_to_offset(Entry.RVA);
      // Entry.Offset = this->RVA2Offset(Entry.RVA);
      // Entry.VA = this->GetpPEHeader()->OptHeader.ImageBase + Entry.RVA;
      Entry.Value = *reinterpret_cast<T*>(reinterpret_cast<ulong64>(this->get_ptr_base()) + Offset);

      m_relocation_entries.push_back(std::move(Entry));
    }

    Size += pIBR->SizeOfBlock;
  }

  return m_relocation_entries;
}

template<typename T>
const std::vector<TExIID>& vuapi CPEFileTX<T>::get_ex_iids(bool InCache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (InCache && !m_ex_iids.empty())
  {
    return m_ex_iids;
  }

  m_ex_iids.clear();

  T ulIIDOffset = this->rva_to_offset(m_ptr_pe_header->OptHeader.Import.VirtualAddress);
  if (ulIIDOffset == T(-1))
  {
    return m_ex_iids;
  }

  auto pIID = (PImportDescriptor)((ulong64)m_ptr_base + ulIIDOffset);
  if (pIID == nullptr)
  {
    return m_ex_iids;
  }

  m_ex_iids.clear();

  for (int i = 0; pIID->FirstThunk != 0; i++, pIID++)
  {
    TExIID ExIID;
    ExIID.IIDID = i;
    ExIID.Name = (char*)((ulong64)m_ptr_base + this->rva_to_offset(pIID->Name));;
    ExIID.pIID = pIID;

    m_ex_iids.push_back(std::move(ExIID));
  }

  return m_ex_iids;
}

template<typename T>
const std::vector<PImportDescriptor>& vuapi CPEFileTX<T>::get_import_descriptors(bool InCache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (InCache && !m_import_descriptors.empty())
  {
    return m_import_descriptors;
  }

  this->get_ex_iids(InCache);

  m_import_descriptors.clear();

  for (const auto& e: m_ex_iids)
  {
    m_import_descriptors.push_back(e.pIID);
  }

  return m_import_descriptors;
}

template<typename T>
const std::vector<TImportModule> vuapi CPEFileTX<T>::get_import_modules(bool InCache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (InCache && !m_import_modules.empty())
  {
    return m_import_modules;
  }

  m_import_modules.clear();

  this->get_ex_iids(InCache);

  for (const auto& e: m_ex_iids)
  {
    TImportModule mi;
    mi.IIDID = e.IIDID;
    mi.Name  = e.Name;
    // mi.NumberOfFuctions = 0;

    m_import_modules.push_back(std::move(mi));
  }

  return m_import_modules;
}

template<typename T>
const std::vector<TImportFunctionT<T>> vuapi CPEFileTX<T>::get_import_functions(bool InCache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (InCache && !m_import_functions.empty())
  {
    return m_import_functions;
  }

  this->get_ex_iids(InCache);

  m_import_functions.clear();

  TThunkDataT<T>* pThunkData = nullptr;
  TImportFunctionT<T> funcInfo;
  for (const auto& e: m_ex_iids)
  {
    T ulOffset = this->rva_to_offset(e.pIID->FirstThunk);
    if (ulOffset == -1 || (pThunkData = (TThunkDataT<T>*)((ulong64)m_ptr_base + ulOffset)) == nullptr) continue;
    do
    {
      if ((pThunkData->u1.AddressOfData & m_ordinal_flag) == m_ordinal_flag)   // Imported by ordinal
      {
        funcInfo.Name = "";
        funcInfo.Hint = -1;
        funcInfo.Ordinal = pThunkData->u1.AddressOfData & ~m_ordinal_flag;
      }
      else   // Imported by name
      {
        ulOffset = this->rva_to_offset(pThunkData->u1.AddressOfData);
        PImportByName p = (PImportByName)((ulong64)m_ptr_base + ulOffset);
        if (ulOffset != -1 && p != nullptr)
        {
          funcInfo.Hint = p->Hint;
          funcInfo.Ordinal = T(-1);
          funcInfo.Name = (char*)p->Name;
        }
      }

      funcInfo.IIDID = e.IIDID;
      funcInfo.RVA = pThunkData->u1.AddressOfData;
      m_import_functions.push_back(funcInfo);

      pThunkData++;
    }
    while (pThunkData->u1.AddressOfData != 0);
  }

  return m_import_functions;
}

template<typename T>
const TImportModule* vuapi CPEFileTX<T>::find_ptr_import_module(const std::string& ModuleName, bool InCache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  this->get_import_modules(InCache);

  const TImportModule* result = nullptr;

  auto s1 = upper_string_A(ModuleName);

  for (const auto& e: m_import_modules)
  {
    auto s2 = upper_string_A(e.Name);
    if (s1 == s2)
    {
      result = &e;
      break;
    }
  }

  return result;
}

template<typename T>
const TImportFunctionT<T>* vuapi CPEFileTX<T>::find_ptr_import_function(
  const TImportFunctionT<T>& ImportFunction,
  eImportedFunctionFindMethod Method,
  bool InCache
)
{
  if (!m_initialized)
  {
    assert(0);
  }

  const TImportFunctionT<T>* result = nullptr;

  this->get_import_functions(InCache);

  switch (Method)
  {
  case eImportedFunctionFindMethod::IFFM_HINT:
    for (const auto& e: m_import_functions)
    {
      if (e.Hint == ImportFunction.Hint)
      {
        result = &e;
        break;
      }
    }
    break;

  case eImportedFunctionFindMethod::IFFM_NAME:
    for (const auto& e: m_import_functions)
    {
      if (e.Name == ImportFunction.Name)
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
const TImportFunctionT<T>* vuapi CPEFileTX<T>::find_ptr_import_function(
  const std::string& FunctionName,
  bool InCache)
{
  TImportFunctionT<T> o = {0};
  o.Name = FunctionName;
  return this->find_ptr_import_function(o, eImportedFunctionFindMethod::IFFM_NAME);
}

template<typename T>
const TImportFunctionT<T>* vuapi CPEFileTX<T>::find_ptr_import_function(
  const ushort FunctionHint,
  bool InCache)
{
  TImportFunctionT<T> o = {0};
  o.Hint = FunctionHint;
  return this->find_ptr_import_function(o, eImportedFunctionFindMethod::IFFM_HINT);
}

template<typename T>
T vuapi CPEFileTX<T>::rva_to_offset(T RVA, bool InCache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (!InCache || m_section_headers.empty())
  {
    this->get_setion_headers(false);
  }

  if (m_section_headers.empty())
  {
    return T(-1);
  }

  const auto& theLastSection = *m_section_headers.rbegin();

  std::pair<T, T> range(T(0), T(theLastSection->VirtualAddress) + T(theLastSection->Misc.VirtualSize));
  if (RVA < range.first || RVA > range.second)
  {
    return T(-1);
  }

  T result = RVA;

  for (auto& e : m_section_headers)
  {
    if ((RVA >= e->VirtualAddress) && (RVA < (e->VirtualAddress + e->Misc.VirtualSize)))
    {
      result  = e->PointerToRawData;
      result += (RVA - e->VirtualAddress);
      break;
    }
  }

  return result;
}

template<typename T>
T vuapi CPEFileTX<T>::offset_to_rva(T Offset, bool InCache)
{
  if (!m_initialized)
  {
    assert(0);
  }

  if (!InCache || m_section_headers.empty())
  {
    this->get_setion_headers(false);
  }

  if (m_section_headers.empty())
  {
    return T(-1);
  }

  const auto& theLastSection = *m_section_headers.rbegin();

  std::pair<T, T> range(T(0), T(theLastSection->PointerToRawData) + T(theLastSection->SizeOfRawData));
  if (Offset < range.first || Offset > range.second)
  {
    return T(-1);
  }

  T result = Offset;

  for (auto& e : m_section_headers)
  {
    if ((Offset >= e->PointerToRawData) && (Offset < (e->PointerToRawData + e->SizeOfRawData)))
    {
      result  = e->VirtualAddress;
      result += (Offset - e->PointerToRawData);
      break;
    }
  }

  return result;
}

template class CPEFileTA<ulong32>;
template class CPEFileTA<ulong64>;

template<typename T>
CPEFileTA<T>::CPEFileTA(const std::string& pe_file_path)
{
  CPEFileTX<T>::m_initialized = false;

  CPEFileTX<T>::m_ptr_base = nullptr;
  CPEFileTX<T>::m_ptr_dos_header = nullptr;
  CPEFileTX<T>::m_ptr_pe_header = nullptr;

  m_file_path = pe_file_path;
}

template<typename T>
CPEFileTA<T>::~CPEFileTA()
{
  if (CPEFileTX<T>::m_initialized)
  {
    m_file_map.close();
  }
}

template<typename T>
VUResult vuapi CPEFileTA<T>::parse()
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

  CPEFileTX<T>::m_ptr_base = m_file_map.view(eFMDesiredAccess::DA_READ);
  if (CPEFileTX<T>::m_ptr_base == nullptr)
  {
    return 4;
  }

  CPEFileTX<T>::m_ptr_dos_header = (PDOSHeader)CPEFileTX<T>::m_ptr_base;
  if (CPEFileTX<T>::m_ptr_dos_header == nullptr)
  {
    return 5;
  }

  CPEFileTX<T>::m_ptr_pe_header = (TPEHeaderT<T>*)((ulong64)CPEFileTX<T>::m_ptr_base + CPEFileTX<T>::m_ptr_dos_header->e_lfanew);
  if (CPEFileTX<T>::m_ptr_pe_header == nullptr)
  {
    return 6;
  }

  if (sizeof(T) == sizeof(pe32))
  {
    if (CPEFileTX<T>::m_ptr_pe_header->OptHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else if (sizeof(T) == sizeof(pe64))
  {
    if (CPEFileTX<T>::m_ptr_pe_header->OptHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else
  {
    return 8; // The curent type data was not supported
  }

  CPEFileTX<T>::m_initialized = true;

  return VU_OK;
}

template class CPEFileTW<ulong32>;
template class CPEFileTW<ulong64>;

template<typename T>
CPEFileTW<T>::CPEFileTW(const std::wstring& pe_file_path)
{
  CPEFileTX<T>::m_initialized = false;

  CPEFileTX<T>::m_ptr_base = nullptr;
  CPEFileTX<T>::m_ptr_dos_header = nullptr;
  CPEFileTX<T>::m_ptr_pe_header  = nullptr;

  m_file_path = pe_file_path;
}

template<typename T>
CPEFileTW<T>::~CPEFileTW()
{
  if (CPEFileTX<T>::m_initialized)
  {
    m_file_map.close();
  }
}

template<typename T>
VUResult vuapi CPEFileTW<T>::parse()
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

  CPEFileTX<T>::m_ptr_base = m_file_map.view(eFMDesiredAccess::DA_READ);
  if (CPEFileTX<T>::m_ptr_base == nullptr)
  {
    return 4;
  }

  CPEFileTX<T>::m_ptr_dos_header = (PDOSHeader)CPEFileTX<T>::m_ptr_base;
  if (CPEFileTX<T>::m_ptr_dos_header == nullptr)
  {
    return 5;
  }

  CPEFileTX<T>::m_ptr_pe_header = (TPEHeaderT<T>*)((ulong64)CPEFileTX<T>::m_ptr_base + CPEFileTX<T>::m_ptr_dos_header->e_lfanew);
  if (CPEFileTX<T>::m_ptr_pe_header == nullptr)
  {
    return 6;
  }

  if (sizeof(T) == sizeof(pe32))
  {
    if (CPEFileTX<T>::m_ptr_pe_header->OptHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else if (sizeof(T) == sizeof(pe64))
  {
    if (CPEFileTX<T>::m_ptr_pe_header->OptHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else
  {
    return 8; // The curent type data was not supported
  }

  CPEFileTX<T>::m_initialized = true;

  return VU_OK;
}

} // namespace vu