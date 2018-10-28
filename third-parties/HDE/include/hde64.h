/*
 * Hacker Disassembler Engine 64
 * Copyright (c) 2008-2009, Vyacheslav Patkov.
 * All rights reserved.
 *
 * hde64.h: C/C++ header file
 *
 */

#ifndef _HDE64_H_
#define _HDE64_H_

/* stdint.h - C99 standard header
 * http://en.wikipedia.org/wiki/stdint.h
 *
 * if your compiler doesn't contain "stdint.h" header (for
 * example, Microsoft Visual C++), you can download file:
 *   http://www.azillionmonkeys.com/qed/pstdint.h
 * and change next line to:
 *   #include "pstdint.h"
 */
#include <stdint.h>

namespace HDE64
{

const uint32_t F_MODRM            = 0x00000001;
const uint32_t F_SIB              = 0x00000002;
const uint32_t F_IMM8             = 0x00000004;
const uint32_t F_IMM16            = 0x00000008;
const uint32_t F_IMM32            = 0x00000010;
const uint32_t F_IMM64            = 0x00000020;
const uint32_t F_DISP8            = 0x00000040;
const uint32_t F_DISP16           = 0x00000080;
const uint32_t F_DISP32           = 0x00000100;
const uint32_t F_RELATIVE         = 0x00000200;
const uint32_t F_ERROR            = 0x00001000;
const uint32_t F_ERROR_OPCODE     = 0x00002000;
const uint32_t F_ERROR_LENGTH     = 0x00004000;
const uint32_t F_ERROR_LOCK       = 0x00008000;
const uint32_t F_ERROR_OPERAND    = 0x00010000;
const uint32_t F_PREFIX_REPNZ     = 0x01000000;
const uint32_t F_PREFIX_REPX      = 0x02000000;
const uint32_t F_PREFIX_REP       = 0x03000000;
const uint32_t F_PREFIX_66        = 0x04000000;
const uint32_t F_PREFIX_67        = 0x08000000;
const uint32_t F_PREFIX_LOCK      = 0x10000000;
const uint32_t F_PREFIX_SEG       = 0x20000000;
const uint32_t F_PREFIX_REX       = 0x40000000;
const uint32_t F_PREFIX_ANY       = 0x7f000000;

const uint8_t PREFIX_SEGMENT_CS   = 0x2e;
const uint8_t PREFIX_SEGMENT_SS   = 0x36;
const uint8_t PREFIX_SEGMENT_DS   = 0x3e;
const uint8_t PREFIX_SEGMENT_ES   = 0x26;
const uint8_t PREFIX_SEGMENT_FS   = 0x64;
const uint8_t PREFIX_SEGMENT_GS   = 0x65;
const uint8_t PREFIX_LOCK         = 0xf0;
const uint8_t PREFIX_REPNZ        = 0xf2;
const uint8_t PREFIX_REPX         = 0xf3;
const uint8_t PREFIX_OPERAND_SIZE = 0x66;
const uint8_t PREFIX_ADDRESS_SIZE = 0x67;

#pragma pack(push,1)

typedef struct {
    uint8_t len;
    uint8_t p_rep;
    uint8_t p_lock;
    uint8_t p_seg;
    uint8_t p_66;
    uint8_t p_67;
    uint8_t rex;
    uint8_t rex_w;
    uint8_t rex_r;
    uint8_t rex_x;
    uint8_t rex_b; 
    uint8_t opcode;
    uint8_t opcode2;
    uint8_t modrm;
    uint8_t modrm_mod;
    uint8_t modrm_reg;
    uint8_t modrm_rm;
    uint8_t sib;
    uint8_t sib_scale;
    uint8_t sib_index;
    uint8_t sib_base;
    union {
        uint8_t imm8;
        uint16_t imm16;
        uint32_t imm32;
        uint64_t imm64;
    } imm;
    union {
        uint8_t disp8;
        uint16_t disp16;
        uint32_t disp32;
    } disp;
    uint32_t flags;
} hde64s, tagHDE;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

/* __cdecl */
unsigned int hde64_disasm(const void *code, hde64s *hs);

static unsigned int Disasemble(const void *code, hde64s *hs)
{
  return hde64_disasm(code, hs);
}

#ifdef __cplusplus
}
#endif

} // HDE64

#endif /* _HDE64_H_ */