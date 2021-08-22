#pragma once

#include "Sample.h"

#define SEPERATOR() std::tcout << ts("----------------------------------------") << std::endl;

DEF_SAMPLE(PEFile)
{
  #ifdef _WIN64
  #define PROCESS_NAME ts("x64dbg.exe")
  #else // _WIN32
  #define PROCESS_NAME ts("x32dbg.exe")
  #endif // _WIN64

  auto pids = vu::name_to_pid(PROCESS_NAME);
  if (pids.empty())
  {
    std::cout << "Not found the target process for PE Parsing ..." << std::endl;
    return vu::VU_OK;
  }

  auto pid = pids.back();

  vu::Process process;
  process.attach(pid);
  auto module = process.get_module_information();

  vu::PEFileT<vu::peX> pe(module.szExePath);
  vu::VUResult result = pe.parse();
  if (result != vu::VU_OK)
  {
    std::tstring s;
    if (result == 7)
    {
      s = ts(" (Used wrong type data for the current PE file format)");
    }
    if (result == 8)
    {
      s = ts(" (The curent type data was not supported)");
    }
    std::tcout << ts("PE -> Parse -> Failure") << vu::get_last_error() << s << std::endl;
    return 1;
  }

  void* ptr_base = pe.get_ptr_base();
  if (ptr_base == nullptr)
  {
    std::tcout << ts("PE -> get_ptr_base -> Failure") << vu::get_last_error() << std::endl;
    return 1;
  }

  SEPERATOR()

  auto sections = pe.get_setion_headers();
  if (sections.size() == 0)
  {
    std::tcout << ts("PE -> get_setion_headers -> Failure") << vu::get_last_error() << std::endl;
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

  auto ptr_pe_header = pe.get_ptr_pe_header();
  assert(ptr_pe_header != nullptr);

  std::cout << std::hex << pe.offset_to_rva(0x00113a92) << std::endl;
  std::cout << std::hex << pe.rva_to_offset(0x00115292) << std::endl;

  auto iids = pe.get_import_descriptors();
  assert(!iids.empty());

  for (auto iid : iids)
  {
    printf("%+20s %08X %08X %08X %08X\n",
      ((char*)ptr_base + pe.rva_to_offset(iid->Name)),
      iid->Name,
      iid->FirstThunk,
      iid->OriginalFirstThunk,
      iid->Characteristics
    );
  }

  SEPERATOR()

  auto modules = pe.get_import_modules();
  assert(!modules.empty());

  for (auto e: modules)
  {
    printf("%08X, '%s'\n", e.iid_id, e.name.c_str());
  }

  SEPERATOR()

  auto functions = pe.get_import_functions();
  assert(!functions.empty());

  for (auto& e : functions)
  {
    auto s = vu::format_A("iid_id = %08X, hint = %04X, '%s'", e.iid_id, e.hint, e.name.c_str());
    std::cout << s << std::endl;
  }

  // SEPERATOR()

  auto ptr_module = pe.find_ptr_import_module("KERNEL32.DLL");
  if (ptr_module != nullptr)
  {
    printf("%08X, '%s'\n", ptr_module->iid_id, ptr_module->name.c_str());
  }

  SEPERATOR()

  auto pfn = pe.find_ptr_import_function("GetLastError");
  if (pfn != nullptr)
  {
    printf("%08X, %04X, '%s'\n", pfn->iid_id, pfn->hint, pfn->name.c_str());
  }

  SEPERATOR()

  for (const auto& entry : pe.get_relocation_entries())
  {
    auto value = vu::peX(0);
    vu::rpm(process.handle(), LPVOID(vu::peX(module.modBaseAddr) + entry.rva), &value, sizeof(value));

    #ifdef _WIN64
    auto fmt = ts("%llX : %llX -> %llX");
    #else // _WIN32
    auto fmt = ts("%08X : %08X -> %08X");
    #endif // _WIN64

    std::tcout << vu::format(fmt, entry.rva, entry.value, value) << std::endl;
  }

  return vu::VU_OK;
}
