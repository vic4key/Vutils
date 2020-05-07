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

template<typename T>
CPEFileTX<T>::CPEFileTX()
{
  m_Initialized = false;

  m_pBase = nullptr;
  m_pDosHeader = nullptr;
  m_pPEHeader  = nullptr;
  m_SectionHeaders.clear();
  m_ImportDescriptors.clear();
  m_ExIDDs.clear();
  m_ImportFunctions.clear();

  if (sizeof(T) == 4)
  {
    m_OrdinalFlag = (T)IMAGE_ORDINAL_FLAG32;
  }
  else
  {
    m_OrdinalFlag = (T)IMAGE_ORDINAL_FLAG64;
  }
}

template<typename T>
CPEFileTX<T>::~CPEFileTX() {};

template<typename T>
void* vuapi CPEFileTX<T>::GetpBase()
{
  return m_pBase;
}

template<typename T>
TPEHeaderT<T>* vuapi CPEFileTX<T>::GetpPEHeader()
{
  if (!m_Initialized)
  {
    return nullptr;
  }

  return m_pPEHeader;
}

template<typename T>
const std::vector<PSectionHeader>& vuapi CPEFileTX<T>::GetSetionHeaders(bool InCache)
{
  if (!m_Initialized)
  {
    assert(0);
  }

  if (InCache && !m_SectionHeaders.empty())
  {
    return m_SectionHeaders;
  }

  m_SectionHeaders.clear();

  vu::PSectionHeader pSH = (PSectionHeader)((ulong64)m_pPEHeader + sizeof(vu::TNTHeaderT<T>));
  if (pSH == nullptr)
  {
    return m_SectionHeaders;
  }

  m_SectionHeaders.clear();
  for (int i = 0; i < m_pPEHeader->NumberOfSections; i++)
  {
    m_SectionHeaders.push_back(pSH);
    pSH++;
  }

  return m_SectionHeaders;
}

template<typename T>
const std::vector<TExIID>& vuapi CPEFileTX<T>::GetExIIDs(bool InCache)
{
  if (!m_Initialized)
  {
    assert(0);
  }

  if (InCache && !m_ExIDDs.empty())
  {
    return m_ExIDDs;
  }

  m_ExIDDs.clear();

  T ulIIDOffset = this->RVA2Offset(m_pPEHeader->Import.VirtualAddress);
  if (ulIIDOffset == T(-1))
  {
    return m_ExIDDs;
  }

  auto pIID = (PImportDescriptor)((ulong64)m_pBase + ulIIDOffset);
  if (pIID == nullptr)
  {
    return m_ExIDDs;
  }

  m_ExIDDs.clear();

  for (int i = 0; pIID->FirstThunk != 0; i++, pIID++)
  {
    TExIID ExIID;
    ExIID.IIDID = i;
    ExIID.Name = (char*)((ulong64)m_pBase + this->RVA2Offset(pIID->Name));;
    ExIID.pIID = pIID;

    m_ExIDDs.push_back(std::move(ExIID));
  }

  return m_ExIDDs;
}

template<typename T>
const std::vector<PImportDescriptor>& vuapi CPEFileTX<T>::GetImportDescriptors(bool InCache)
{
  if (!m_Initialized)
  {
    assert(0);
  }

  if (InCache && !m_ImportDescriptors.empty())
  {
    return m_ImportDescriptors;
  }

  this->GetExIIDs(InCache);

  m_ImportDescriptors.clear();

  for (const auto& e: m_ExIDDs)
  {
    m_ImportDescriptors.push_back(e.pIID);
  }

  return m_ImportDescriptors;
}

template<typename T>
const std::vector<TImportModule> vuapi CPEFileTX<T>::GetImportModules(bool InCache)
{
  if (!m_Initialized)
  {
    assert(0);
  }

  if (InCache && !m_ImportModules.empty())
  {
    return m_ImportModules;
  }

  m_ImportModules.clear();

  this->GetExIIDs(InCache);

  for (const auto& e: m_ExIDDs)
  {
    TImportModule mi;
    mi.IIDID = e.IIDID;
    mi.Name  = e.Name;
    // mi.NumberOfFuctions = 0;

    m_ImportModules.push_back(std::move(mi));
  }

  return m_ImportModules;
}

template<typename T>
const std::vector<TImportFunctionT<T>> vuapi CPEFileTX<T>::GetImportFunctions(bool InCache)
{
  if (!m_Initialized)
  {
    assert(0);
  }

  if (InCache && !m_ImportFunctions.empty())
  {
    return m_ImportFunctions;
  }

  this->GetExIIDs(InCache);

  m_ImportFunctions.clear();

  TThunkDataT<T>* pThunkData = nullptr;
  TImportFunctionT<T> funcInfo;
  for (const auto& e: m_ExIDDs)
  {
    T ulOffset = this->RVA2Offset(e.pIID->FirstThunk);
    if (ulOffset == -1 || (pThunkData = (TThunkDataT<T>*)((ulong64)m_pBase + ulOffset)) == nullptr) continue;
    do
    {
      if ((pThunkData->u1.AddressOfData & m_OrdinalFlag) == m_OrdinalFlag)   // Imported by ordinal
      {
        funcInfo.Name = "";
        funcInfo.Hint = -1;
        funcInfo.Ordinal = pThunkData->u1.AddressOfData & ~m_OrdinalFlag;
      }
      else   // Imported by name
      {
        ulOffset = this->RVA2Offset(pThunkData->u1.AddressOfData);
        PImportByName p = (PImportByName)((ulong64)m_pBase + ulOffset);
        if (ulOffset != -1 && p != nullptr)
        {
          funcInfo.Hint = p->Hint;
          funcInfo.Ordinal = T(-1);
          funcInfo.Name = (char*)p->Name;
        }
      }

      funcInfo.IIDID = e.IIDID;
      funcInfo.RVA = pThunkData->u1.AddressOfData;
      m_ImportFunctions.push_back(funcInfo);

      pThunkData++;
    }
    while (pThunkData->u1.AddressOfData != 0);
  }

  return m_ImportFunctions;
}

template<typename T>
const TImportModule* vuapi CPEFileTX<T>::FindImportModule(const std::string& ModuleName, bool InCache)
{
  if (!m_Initialized)
  {
    assert(0);
  }

  this->GetImportModules(InCache);

  const TImportModule* result = nullptr;

  auto s1 = UpperStringA(ModuleName);

  for (const auto& e: m_ImportModules)
  {
    auto s2 = UpperStringA(e.Name);
    if (s1 == s2)
    {
      result = &e;
      break;
    }
  }

  return result;
}

template<typename T>
const TImportFunctionT<T>* vuapi CPEFileTX<T>::FindImportFunction(
  const TImportFunctionT<T>& ImportFunction,
  eImportedFunctionFindMethod Method,
  bool InCache
)
{
  if (!m_Initialized)
  {
    assert(0);
  }

  const TImportFunctionT<T>* result = nullptr;

  this->GetImportFunctions(InCache);

  switch (Method)
  {
  case eImportedFunctionFindMethod::IFFM_HINT:
    for (const auto& e: m_ImportFunctions)
    {
      if (e.Hint == ImportFunction.Hint)
      {
        result = &e;
        break;
      }
    }
    break;

  case eImportedFunctionFindMethod::IFFM_NAME:
    for (const auto& e: m_ImportFunctions)
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
const TImportFunctionT<T>* vuapi CPEFileTX<T>::FindImportFunction(
  const std::string& FunctionName,
  bool InCache)
{
  TImportFunctionT<T> o = {0};
  o.Name = FunctionName;
  return this->FindImportFunction(o, eImportedFunctionFindMethod::IFFM_NAME);
}

template<typename T>
const TImportFunctionT<T>* vuapi CPEFileTX<T>::FindImportFunction(
  const ushort FunctionHint,
  bool InCache)
{
  TImportFunctionT<T> o = {0};
  o.Hint = FunctionHint;
  return this->FindImportFunction(o, eImportedFunctionFindMethod::IFFM_HINT);
}

template<typename T>
T vuapi CPEFileTX<T>::RVA2Offset(T RVA, bool InCache)
{
  if (!m_Initialized)
  {
    assert(0);
  }

  if (!InCache || m_SectionHeaders.empty())
  {
    this->GetSetionHeaders(false);
  }

  if (m_SectionHeaders.empty())
  {
    return T(-1);
  }

  const auto& theLastSection = *m_SectionHeaders.rbegin();

  std::pair<T, T> range(T(0), T(theLastSection->VirtualAddress) + T(theLastSection->Misc.VirtualSize));
  if (RVA < range.first || RVA > range.second)
  {
    return T(-1);
  }

  T result = RVA;

  for (auto& e : m_SectionHeaders)
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
T vuapi CPEFileTX<T>::Offset2RVA(T Offset, bool InCache)
{
  if (!m_Initialized)
  {
    assert(0);
  }

  if (!InCache || m_SectionHeaders.empty())
  {
    this->GetSetionHeaders(false);
  }

  if (m_SectionHeaders.empty())
  {
    return T(-1);
  }

  const auto& theLastSection = *m_SectionHeaders.rbegin();

  std::pair<T, T> range(T(0), T(theLastSection->PointerToRawData) + T(theLastSection->SizeOfRawData));
  if (Offset < range.first || Offset > range.second)
  {
    return T(-1);
  }

  T result = Offset;

  for (auto& e : m_SectionHeaders)
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
CPEFileTA<T>::CPEFileTA(const std::string& PEFilePath)
{
  CPEFileTX<T>::m_Initialized = false;

  CPEFileTX<T>::m_pBase = nullptr;
  CPEFileTX<T>::m_pDosHeader = nullptr;
  CPEFileTX<T>::m_pPEHeader  = nullptr;

  m_FilePath = PEFilePath;
}

template<typename T>
CPEFileTA<T>::~CPEFileTA()
{
  if (CPEFileTX<T>::m_Initialized)
  {
    m_FileMap.Close();
  }
}

template<typename T>
VUResult vuapi CPEFileTA<T>::Parse(const std::string& PEFilePath)
{
  if (m_FilePath.empty())
  {
    return 1;
  }

  if (!IsFileExistsA(m_FilePath))
  {
    return 2;
  }

  if (m_FileMap.CreateWithinFile(m_FilePath) != vu::VU_OK)
  {
    return 3;
  }

  CPEFileTX<T>::m_pBase = m_FileMap.View();
  if (CPEFileTX<T>::m_pBase == nullptr)
  {
    return 4;
  }

  CPEFileTX<T>::m_pDosHeader = (PDOSHeader)CPEFileTX<T>::m_pBase;
  if (CPEFileTX<T>::m_pDosHeader == nullptr)
  {
    return 5;
  }

  CPEFileTX<T>::m_pPEHeader = (TPEHeaderT<T>*)((ulong64)CPEFileTX<T>::m_pBase + CPEFileTX<T>::m_pDosHeader->e_lfanew);
  if (CPEFileTX<T>::m_pPEHeader == nullptr)
  {
    return 6;
  }

  if (sizeof(T) == sizeof(pe32))
  {
    if (CPEFileTX<T>::m_pPEHeader->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else if (sizeof(T) == sizeof(pe64))
  {
    if (CPEFileTX<T>::m_pPEHeader->Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else
  {
    return 8; // The curent type data was not supported
  }

  CPEFileTX<T>::m_Initialized = true;

  return VU_OK;
}

template class CPEFileTW<ulong32>;
template class CPEFileTW<ulong64>;

template<typename T>
CPEFileTW<T>::CPEFileTW(const std::wstring& PEFilePath)
{
  CPEFileTX<T>::m_Initialized = false;

  CPEFileTX<T>::m_pBase = nullptr;
  CPEFileTX<T>::m_pDosHeader = nullptr;
  CPEFileTX<T>::m_pPEHeader  = nullptr;

  m_FilePath = PEFilePath;
}

template<typename T>
CPEFileTW<T>::~CPEFileTW()
{
  if (CPEFileTX<T>::m_Initialized)
  {
    m_FileMap.Close();
  }
}

template<typename T>
VUResult vuapi CPEFileTW<T>::Parse()
{
  if (m_FilePath.empty())
  {
    return 1;
  }

  if (!IsFileExistsW(m_FilePath))
  {
    return 2;
  }

  if (m_FileMap.CreateWithinFile(m_FilePath) != vu::VU_OK)
  {
    return 3;
  }

  CPEFileTX<T>::m_pBase = m_FileMap.View();
  if (CPEFileTX<T>::m_pBase == nullptr)
  {
    return 4;
  }

  CPEFileTX<T>::m_pDosHeader = (PDOSHeader)CPEFileTX<T>::m_pBase;
  if (CPEFileTX<T>::m_pDosHeader == nullptr)
  {
    return 5;
  }

  CPEFileTX<T>::m_pPEHeader = (TPEHeaderT<T>*)((ulong64)CPEFileTX<T>::m_pBase + CPEFileTX<T>::m_pDosHeader->e_lfanew);
  if (CPEFileTX<T>::m_pPEHeader == nullptr)
  {
    return 6;
  }

  if (sizeof(T) == sizeof(pe32))
  {
    if (CPEFileTX<T>::m_pPEHeader->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else if (sizeof(T) == sizeof(pe64))
  {
    if (CPEFileTX<T>::m_pPEHeader->Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
      return 7; // Used wrong type data for the current PE file
    }
  }
  else
  {
    return 8; // The curent type data was not supported
  }

  CPEFileTX<T>::m_Initialized = true;

  return VU_OK;
}

} // namespace vu