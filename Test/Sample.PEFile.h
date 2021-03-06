#pragma once

#include "Sample.h"

#define SEPERATOR() std::tcout << _T("----------------------------------------") << std::endl;

DEF_SAMPLE(PEFile)
{
#ifdef _WIN64
  #define PROCESS_NAME _T("x64dbg.exe")
#else // _WIN32
  #define PROCESS_NAME _T("x32dbg.exe")
#endif // _WIN64

  auto PIDs = vu::NameToPID(PROCESS_NAME);
  assert(!PIDs.empty());

  vu::CProcess process;
  process.Attach(PIDs.back());
  auto module = process.GetModuleInformation();

  vu::CPEFileT<vu::peX> pe(module.szExePath);
  vu::VUResult result = pe.Parse();
  if (result != vu::VU_OK)
  {
    std::tstring s;
    if (result == 7)
    {
      s = _T(" (Used wrong type data for the current PE file format)");
    }
    if (result == 8)
    {
      s = _T(" (The curent type data was not supported)");
    }
    std::tcout << _T("PE -> Parse -> Failure") << vu::LastError() << s << std::endl;
    return 1;
  }

  void* pBase = pe.GetpBase();
  if (pBase == nullptr)
  {
    std::tcout << _T("PE -> GetpBase -> Failure") << vu::LastError() << std::endl;
    return 1;
  }

  SEPERATOR()

  auto sections = pe.GetSetionHeaders();
  if (sections.size() == 0)
  {
    std::tcout << _T("PE -> GetSetionHeaderList -> Failure") << vu::LastError() << std::endl;
    return 1;
  }

  for (auto section: sections)
  {
    printf(
      "%+10s %08X %08X %08X %08X\n",
      section->Name,
      section->PointerToRawData,
      section->SizeOfRawData,
      section->VirtualAddress,
      section->Misc.VirtualSize
    );
  }

  SEPERATOR()

  auto pPEHeader = pe.GetpPEHeader();
  assert(pPEHeader != nullptr);

  std::cout << std::hex << pe.Offset2RVA(0x00113a92) << std::endl;
  std::cout << std::hex << pe.RVA2Offset(0x00115292) << std::endl;

  auto IIDs = pe.GetImportDescriptors();
  assert(!IIDs.empty());

  for (auto IID: IIDs)
  {
    printf("%+20s %08X %08X %08X %08X\n",
      ((char*)pBase + pe.RVA2Offset(IID->Name)),
      IID->Name,
      IID->FirstThunk,
      IID->OriginalFirstThunk,
      IID->Characteristics
    );
  }

  SEPERATOR()

  auto modules = pe.GetImportModules();
  assert(!modules.empty());

  for (auto e: modules)
  {
    printf("%08X, '%s'\n", e.IIDID, e.Name.c_str());
  }

  SEPERATOR()

  auto functions = pe.GetImportFunctions();
  assert(!functions.empty());

  // for (auto e : functions)
  // {
  //   auto s = vu::FormatA("IIDID = %08X, Hint = %04X, '%s'", e.IIDID, e.Hint, e.Name.c_str());
  //   std::cout << s << std::endl;
  // }

  // SEPERATOR()

  auto pModule = pe.FindImportModule("KERNEL32.DLL");
  if (pModule != nullptr)
  {
    printf("%08X, '%s'\n", pModule->IIDID, pModule->Name.c_str());
  }

  SEPERATOR()

  auto pfn = pe.FindImportFunction("GetLastError");
  if (pfn != nullptr)
  {
    printf("%08X, %04X, '%s'\n", pfn->IIDID, pfn->Hint, pfn->Name.c_str());
  }

  SEPERATOR()

  for (const auto& Entry : pe.GetRelocationEntries())
  {
    auto Value = vu::peX(0);
    vu::RPM(process.Handle(), LPVOID(vu::peX(module.modBaseAddr) + Entry.RVA), &Value, sizeof(Value));

    #ifdef _WIN64
    auto fmt = _T("%llX : %llX -> %llX");
    #else // _WIN32
    auto fmt = _T("%08X : %08X -> %08X");
    #endif // _WIN64

    std::tcout << vu::Fmt(fmt, Entry.RVA, Entry.Value, Value) << std::endl;
  }

  return vu::VU_OK;
}
