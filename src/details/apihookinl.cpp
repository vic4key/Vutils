/**
 * @file   apihookil.cpp
 * @author Vic P.
 * @brief  Implementation for Inline API Hooking
 */

#include "Vutils.h"
#include "defs.h"

// Internal Buffer Handler
// Copyright (c) 2009-2017 Tsuda Kageyu
#include VU_3RD_INCL(MH/include/buffer.h)

// Hacker Disassembler Engine 32/64 C
// Copyright (c) 2008-2009, Vyacheslav Patkov
// Modified the HDE source code to make it compatible with modern C++ and usable both 32-bit & 64-bit version together

#include VU_3RD_INCL(HDE/include/hde32.h)
#include VU_3RD_INCL(HDE/include/table32.h)
#include VU_3RD_INCL(HDE/include/hde64.h)
#include VU_3RD_INCL(HDE/include/table64.h)

#if defined(_M_X64) || defined(__x86_64__)
namespace HDE = HDE64;
#else
namespace HDE = HDE32;
#endif

namespace vu
{

/**
 * To handle the memory instruction.
 * @param[in]  hde          The HDE struct of the current instruction.
 * @param[in]  offset       The offset of current instruction. From the head of the current function.
 * @param[out] instructions List instructions.
 * @return  True if current instruction is a memory struction, False if it is not.
 */
bool vuapi get_assemble_instruction(
  const HDE::tagHDE& hde,
  const ulong offset,
  std::vector<MemoryInstruction>& instructions
)
{
  ulong pos_disp = 0;
  MemoryInstruction mi = {0};
  bool result = false, found_relative = false;;

  // http://wiki.osdev.org/X86-64_Instruction_Encoding

  // if (is_flag_on(hde.flags, HDE::F_MODRM)) // ModR/M exists
  // {
  //   // ModR/M
  // }
  // else if (is_flag_on(hde.flags, HDE::F_SIB)) // SIB exists
  // {
  //   // SIB
  // }

  // Now, only ModR/M.
  if (!is_flag_on(hde.flags, HDE::F_MODRM))
  {
    return result;
  }

  if (hde.modrm_mod == 3)   // [R/M]
  {
    return result;
  }

  switch (hde.modrm_mod)
  {
  case 0: // MOD = b00
    if ((hde.modrm_rm >= 0 && hde.modrm_rm <= 3) || // {AX, BX, CX, DX}
        (hde.modrm_rm >= 6 && hde.modrm_rm <= 11)   // {SI, DI, R8, R9, R10, R11}
       )   // [R/M]
    {
      // ...
    }
    if (hde.modrm_rm == 5 || hde.modrm_rm == 13)    // [RIP/EIP + D32] {BP, R13} *
    {
      #ifdef _WIN64
      /* pos_disp += 1;   // The first opcode. Always exists a byte for opcode of each instruction.
      if (is_flag_on(hde.flags, HDE::F_PREFIX_SEG)) // * In 64-bit the CS, SS, DS and ES segment overrides are ignored.
      {
        / * Prefix group 2 (take 1 byte)
          0x2E: CS segment override
          0x36: SS segment override
          0x3E: DS segment override
          0x26: ES segment override
          0x64: FS segment override
          0x65: GS segment override
          0x2E: Branch not taken
          0x3E: Branch taken
        * /
         pos_disp += 1; // If it's being used the segment.
      }
      if (is_flag_on(hde.flags, HDE::F_MODRM))
      {
         pos_disp += 1; // The second opcode.
      }
      { // Others
        // ...
      }
      */

      auto imm_flags = 0;
      #ifdef _WIN64
      imm_flags = (HDE::C_IMM8 | HDE::F_IMM16 | HDE::F_IMM32 | HDE::F_IMM64);
      #else
      imm_flags = (HDE::C_IMM8 | HDE::F_IMM16 | HDE::F_IMM32);
      #endif

      pos_disp = hde.len - ((hde.flags & imm_flags) >> 2) - 4;

      mi.position = pos_disp;
      mi.mao.A32 = hde.disp.disp32;
      mi.mat = memory_address_type::MAT_32; // [RIP/EIP1,2 + disp32]
      found_relative = true;
      #endif
    }
    if (hde.modrm_rm == 4 || hde.modrm_rm == 12)      // [SIB] {SP, R12}
    {
      // ...
    }
    break;
  case 1: // MOD = b01
    if ((hde.modrm_rm >= 0  && hde.modrm_rm <= 3)||   // {AX, BX, CX, DX}
        (hde.modrm_rm >= 5  && hde.modrm_rm <= 11) || // {BP, SI, DI, R8, R9, R10, R11}
        (hde.modrm_rm >= 13 && hde.modrm_rm <= 15)    // {R13, R14, R15}
    ) // [R/M + D32]
    {
      // ...
    }
    if (hde.modrm_rm == 4 || hde.modrm_rm == 12)      // [SIB + D32] // {SP, R12}
    {
      // ...
    }
    break;
  case 2: // MOD = b10
    if ((hde.modrm_rm >= 0  && hde.modrm_rm <= 3)||    // {AX, BX, CX, DX}
        (hde.modrm_rm >= 5  && hde.modrm_rm <= 11) ||  // {BP, SI, DI, R8, R9, R10, R11}
        (hde.modrm_rm >= 13 && hde.modrm_rm <= 15)     // {R13, R14, R15}
    ) // [R/M + D8]
    {
      // ...
    }
    if (hde.modrm_rm == 4 || hde.modrm_rm == 12)       // [SIB + D8] // {SP, R12}
    {
      // ...
    }
    break;
  default: // MOD = b01[3] (Ignored)
    // [R/M]
    break;
  }

  if (found_relative)
  {
    mi.offset = offset;
    instructions.push_back(mi);
  }

  result = true;

  return result;
}

INLHookingX::INLHookingX(const INLHookingX& right)
{
  *this = right;
}

const INLHookingX& INLHookingX::operator=(const INLHookingX& right)
{
  m_hooked = right.m_hooked;
  m_memory_instructions = right.m_memory_instructions;
  return *this;
}

bool vuapi INLHookingX::attach(void* ptr_function, void* ptr_hook_function, void** pptr_old_function)
{
  /*
    // x86
    EIP + 0 | FF 25 ?? ?? ?? ??       | JMP DWORD PTR DS:[XXXXXXXX] ; Jump to XXXXXXXX

    // x64
    RIP + 0 | FF 25 ?? ??? ?? ??      | JMP QWORD PTR DS:[RIP+6] ; Jump to [RIP + 6]
    RIP + 6 | ?? ?? ?? ?? ?? ?? ?? ?? | XXXXXXXXXXXXXXXX
  */

  RedirectInstruction O2N = {0}, T2O = {0};

  ulong trampoline_size = 0;
  bool found_trampoline_size = true;

  do
  {
    HDE::tagHDE hde = { 0 };

    HDE::Disasemble((const void*)(ulongptr(ptr_function) + trampoline_size), &hde);
    if ((hde.flags & HDE::F_ERROR) == HDE::F_ERROR)
    {
      found_trampoline_size = false;
      break;
    }
    else
    {
      get_assemble_instruction(hde, trampoline_size, m_memory_instructions);
    }

    trampoline_size += hde.len;
  }
  while (trampoline_size < MIN_HOOK_SIZE);

  if (!found_trampoline_size)
  {
    return false;
  }

  // The allocated address must be not too far with the target (Better is in image process address)
  // pOldProc is a trampoline function
  // VirtualAlloc(nullptr, iTrampolineSize + sizeof(TRedirect), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  *pptr_old_function = (void*)IBH::AllocateBuffer(ptr_function);
  if (*pptr_old_function == nullptr)
  {
    return false;
  }

  memcpy(*pptr_old_function, ptr_function, trampoline_size);

  // fix memory instruction
  for (const auto& e: m_memory_instructions)
  {
    // if (e.MemoryAddressType == eMemoryAddressType::MAT_32) {
    auto v = ulongptr(ptr_function) - ulongptr(*pptr_old_function) + ulongptr(e.mao.A32);
    auto p = ulongptr(*pptr_old_function) + e.offset + e.position;
    *(ulongptr*)p = ulongptr(v); // check again
    // }
  }

  T2O.jmp = 0x25FF;
  #ifdef _WIN64
  T2O.unknown = 0; // JMP_OPCODE_SIZE
  #else  // _WIN32
  T2O.unknown = ((ulongptr)*pptr_old_function + trampoline_size) + sizeof(T2O.jmp) + sizeof(T2O.unknown);
  #endif // _WIN64
  T2O.address = ulongptr(ptr_function) + trampoline_size;

  // Write the jump code (trampoline -> original) on the bottom of the trampoline function
  memcpy((void*)(ulongptr(*pptr_old_function) + trampoline_size), (void*)&T2O, sizeof(T2O));

  ulong old_protection = 0;
  VirtualProtect(ptr_function, trampoline_size, PAGE_EXECUTE_READWRITE, &old_protection);

  O2N.jmp = 0x25FF;
  #ifdef _WIN64
  O2N.unknown = 0; // JMP_OPCODE_SIZE;
  #else  // _WIN32
  O2N.unknown = ((ulongptr)ptr_function + sizeof(O2N.jmp) + sizeof(O2N.unknown));
  #endif // _WIN64
  O2N.address = ulongptr(ptr_hook_function);

  // Write the jump code (original -> new) on the top of the target function
  memcpy(ptr_function, (void*)&O2N, sizeof(O2N));

  return m_hooked = true;
}

bool vuapi INLHookingX::detach(void* ptr_function, void** pptr_old_function)
{
  if (!m_hooked)
  {
    return false;
  }

  // fix memory instruction
  for (const auto& e: m_memory_instructions)
  {
    // if (e.MemoryAddressType == eMemoryAddressType::MAT_32) {
    auto p = ulongptr(*pptr_old_function) + e.offset + e.position;
    auto v = ulongptr(e.mao.A32);
    *(ulongptr*)p = v; // check again
    // }
  }

  memcpy(ptr_function, *pptr_old_function, MIN_HOOK_SIZE);

  // VirtualFree(*pOldProc, MIN_HOOK_SIZE + sizeof(TRedirect), MEM_RELEASE);
  IBH::FreeBuffer(*pptr_old_function);

  *pptr_old_function = nullptr;

  return true;
}

bool vuapi INLHookingA::install(
  const std::string& module_name,
  const std::string& function_name,
  void* ptr_hook_function,
  void** pptr_old_function
)
{
  void* ptr_function = LibraryA::quick_get_proc_address(module_name, function_name);
  if (ptr_function == nullptr)
  {
    return false;
  }

  return this->attach(ptr_function, ptr_hook_function, pptr_old_function);
}

bool vuapi INLHookingA::uninstall(
  const std::string& module_name,
  const std::string& function_name,
  void** pptr_old_function
)
{
  if (!m_hooked)
  {
    return false;
  }

  void* ptr_function = LibraryA::quick_get_proc_address(module_name, function_name);
  if (ptr_function == nullptr)
  {
    return false;
  }

  return this->detach(ptr_function, pptr_old_function);
}

bool vuapi INLHookingW::install(
  const std::wstring& module_name,
  const std::wstring& function_name,
  void* ptr_hook_function,
  void** pptr_old_function
)
{
  void* ptr_function = LibraryW::quick_get_proc_address(module_name, function_name);
  if (ptr_function == nullptr)
  {
    return false;
  }

  m_hooked = this->attach(ptr_function, ptr_hook_function, pptr_old_function);

  return m_hooked;
}

bool vuapi INLHookingW::uninstall(
  const std::wstring& module_name,
  const std::wstring& function_name,
  void** pptr_old_function
)
{
  if (!m_hooked)
  {
    return false;
  }

  void* ptr_function = LibraryW::quick_get_proc_address(module_name, function_name);
  if (ptr_function == nullptr)
  {
    return false;
  }

  return this->detach(ptr_function, pptr_old_function);
}

} // namespace vu