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
bool vuapi GetAssembleInstruction(
  const HDE::tagHDE& hde,
  const ulong offset,
  std::vector<TMemoryInstruction>& instructions
)
{
  ulong ulPosDisp = 0;
  TMemoryInstruction mi = {0};
  bool result = false, bFoundRelative = false;;

  // http://wiki.osdev.org/X86-64_Instruction_Encoding

  // if (IsFlagOn(hde.flags, HDE::F_MODRM)) {  // ModR/M exists
  //   // ModR/M
  // }
  // else if (IsFlagOn(hde.flags, HDE::F_SIB)) {   // SIB exists
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
      /*ulPosDisp += 1;   // The first opcode. Always exists a byte for opcode of each instruction.
      if (IsFlagOn(hde.flags, HDE::F_PREFIX_SEG)) { // * In 64-bit the CS, SS, DS and ES segment overrides are ignored.
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
        ulPosDisp += 1; // If it's being used the segment.
      }
      if (IsFlagOn(hde.flags, HDE::F_MODRM)) {
        ulPosDisp += 1; // The second opcode.
      }
      { // Others
        // ...
      }
      */

      auto ulImmFlags = 0;
      #ifdef _WIN64
      ulImmFlags = (HDE::C_IMM8 | HDE::F_IMM16 | HDE::F_IMM32 | HDE::F_IMM64);
      #else
      ulImmFlags = (HDE::C_IMM8 | HDE::F_IMM16 | HDE::F_IMM32);
      #endif

      ulPosDisp = hde.len - ((hde.flags & ulImmFlags) >> 2) - 4;

      mi.Position = ulPosDisp;
      mi.MAO.A32 = hde.disp.disp32;
      mi.MemoryAddressType = eMemoryAddressType::MAT_32; // [RIP/EIP1,2 + disp32]
      bFoundRelative = true;
      #endif
    }
    if (hde.modrm_rm == 4 || hde.modrm_rm == 12)    // [SIB] {SP, R12}
    {
      // ...
    }
    break;
  case 1: // MOD = b01
    if ((hde.modrm_rm >= 0 && hde.modrm_rm <= 3)||    // {AX, BX, CX, DX}
        (hde.modrm_rm >= 5 && hde.modrm_rm <= 11) ||  // {BP, SI, DI, R8, R9, R10, R11}
        (hde.modrm_rm >= 13 && hde.modrm_rm <= 15)    // {R13, R14, R15}
    ) // [R/M + D32]
    {
      // ...
    }
    if (hde.modrm_rm == 4 || hde.modrm_rm == 12)    // [SIB + D32] // {SP, R12}
    {
      // ...
    }
    break;
  case 2: // MOD = b10
    if ((hde.modrm_rm >= 0 && hde.modrm_rm <= 3)||    // {AX, BX, CX, DX}
        (hde.modrm_rm >= 5 && hde.modrm_rm <= 11) ||  // {BP, SI, DI, R8, R9, R10, R11}
        (hde.modrm_rm >= 13 && hde.modrm_rm <= 15)    // {R13, R14, R15}
    ) // [R/M + D8]
    {
      // ...
    }
    if (hde.modrm_rm == 4 || hde.modrm_rm == 12)    // [SIB + D8] // {SP, R12}
    {
      // ...
    }
    break;
  default: // MOD = b01[3] (Ignored)
    // [R/M]
    break;
  }

  if (bFoundRelative)
  {
    mi.Offset = offset;
    instructions.push_back(mi);
  }

  result = true;

  return result;
}

bool vuapi CAPIHookX::Attach(void* pProc, void* pHookProc, void** pOldProc)
{
  /*
    // x86
    EIP + 0 | FF 25 ?? ?? ?? ??       | JMP DWORD PTR DS:[XXXXXXXX] ; Jump to XXXXXXXX

    // x64
    RIP + 0 | FF 25 ?? ??? ?? ??      | JMP QWORD PTR DS:[RIP+6] ; Jump to [RIP + 6]
    RIP + 6 | ?? ?? ?? ?? ?? ?? ?? ?? | XXXXXXXXXXXXXXXX
  */

  TRedirect O2N = {0}, T2O = {0};
  ulong iTrampolineSize = 0;
  bool bFoundTrampolineSize = true;

  do
  {
    HDE::tagHDE hde = { 0 };

    HDE::Disasemble((const void *)((ulongptr)pProc + iTrampolineSize), &hde);
    if ((hde.flags & HDE::F_ERROR) == HDE::F_ERROR)
    {
      bFoundTrampolineSize = false;
      break;
    }
    else
    {
      GetAssembleInstruction(hde, iTrampolineSize, m_ListMemoryInstruction);
    }

    iTrampolineSize += hde.len;
  }
  while (iTrampolineSize < MIN_HOOK_SIZE);

  if (!bFoundTrampolineSize)
  {
    return false;
  }

  // The allocated address must be not too far with the target (Better is in image process address)
  // pOldProc is a trampoline function
  // VirtualAlloc(nullptr, iTrampolineSize + sizeof(TRedirect), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  *pOldProc = (void*)IBH::AllocateBuffer(pProc);
  if (*pOldProc == nullptr)
  {
    return false;
  }

  memcpy(*pOldProc, pProc, iTrampolineSize);

  // fix memory instruction
  if (m_ListMemoryInstruction.size() > 0)
  {
    for (auto e: m_ListMemoryInstruction)
    {
      // if (e.MemoryAddressType == eMemoryAddressType::MAT_32) {
      auto v = (ulongptr)pProc - (ulongptr)*pOldProc + (ulongptr)e.MAO.A32;
      auto p = ((ulongptr)*pOldProc + e.Offset + e.Position);
      *(ulongptr*)p = (ulongptr)v; // check again
      // }
    }
  }

  T2O.JMP = 0x25FF;
  #ifdef _WIN64
  T2O.Unknown = 0; // JMP_OPCODE_SIZE
  #else  // _WIN32
  T2O.Unknown = ((ulongptr)*pOldProc + iTrampolineSize) + sizeof(T2O.JMP) + sizeof(T2O.Unknown);
  #endif // _WIN64
  T2O.Address = ((ulongptr)pProc + iTrampolineSize);

  // Write the jump code (trampoline -> original) on the bottom of the trampoline function
  memcpy((void*)((ulongptr)*pOldProc + iTrampolineSize), (void*)&T2O, sizeof(T2O));

  ulong ulOldProtect = 0;
  VirtualProtect(pProc, iTrampolineSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

  O2N.JMP = 0x25FF;
  #ifdef _WIN64
  O2N.Unknown = 0; // JMP_OPCODE_SIZE;
  #else  // _WIN32
  O2N.Unknown = ((ulongptr)pProc + sizeof(O2N.JMP) + sizeof(O2N.Unknown));
  #endif // _WIN64
  O2N.Address = (ulongptr)pHookProc;

  // Write the jump code (original -> new) on the top of the target function
  memcpy(pProc, (void*)&O2N, sizeof(O2N));

  return true;
}

bool vuapi CAPIHookX::Detach(void* pProc, void** pOldProc)
{
  if (!m_Hooked)
  {
    return false;
  }

  // fix memory instruction
  if (m_ListMemoryInstruction.size() > 0)
  {
    for (auto e: m_ListMemoryInstruction)
    {
      // if (e.MemoryAddressType == eMemoryAddressType::MAT_32) {
      auto p = ((ulongptr)*pOldProc + e.Offset + e.Position);
      auto v = (ulongptr)e.MAO.A32;
      *(ulongptr*)p = v; // check again
      // }
    }
  }

  memcpy(pProc, *pOldProc, MIN_HOOK_SIZE);

  // VirtualFree(*pOldProc, MIN_HOOK_SIZE + sizeof(TRedirect), MEM_RELEASE);
  IBH::FreeBuffer(*pOldProc);

  *pOldProc = nullptr;

  return true;
}

bool vuapi CAPIHookA::Override(
  const std::string& ModuleName,
  const std::string& ProcName,
  void* lpHookProc,
  void** lpOldProc
)
{
  void* lpProc = CLibraryA::quick_get_proc_address(ModuleName, ProcName);
  if (lpProc == nullptr)
  {
    return false;
  }

  m_Hooked = this->Attach(lpProc, lpHookProc, lpOldProc);

  return m_Hooked;
}

bool vuapi CAPIHookA::Restore(
  const std::string& ModuleName,
  const std::string& ProcName,
  void** lpOldProc
)
{
  if (!m_Hooked)
  {
    return false;
  }

  void* lpProc = CLibraryA::quick_get_proc_address(ModuleName, ProcName);
  if (lpProc == nullptr)
  {
    return false;
  }

  return this->Detach(lpProc, lpOldProc);
}

bool vuapi CAPIHookW::Override(
  const std::wstring& ModuleName,
  const std::wstring& ProcName,
  void* lpHookProc,
  void** lpOldProc
)
{
  void* lpProc = CLibraryW::quick_get_proc_address(ModuleName, ProcName);
  if (lpProc == nullptr)
  {
    return false;
  }

  m_Hooked = this->Attach(lpProc, lpHookProc, lpOldProc);

  return m_Hooked;
}

bool vuapi CAPIHookW::Restore(
  const std::wstring& ModuleName,
  const std::wstring& ProcName,
  void** lpOldProc
)
{
  if (!m_Hooked)
  {
    return false;
  }

  void* lpProc = CLibraryW::quick_get_proc_address(ModuleName, ProcName);
  if (lpProc == nullptr)
  {
    return false;
  }

  return this->Detach(lpProc, lpOldProc);
}

} // namespace vu