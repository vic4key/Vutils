/**
 * @file   defs.h
 * @author Vic P.
 * @brief  Header for Definitions
 */

#pragma once

 /**
  * Header Inclusion Macros for Third Party Libraries
  */

#define VU_STRINGIZE_EX(s) #s
#define VU_STRINGIZE(s) VU_STRINGIZE_EX(s)
#define VU_3RD_INCL(s)  VU_STRINGIZE(../3rdparty/s)

#ifndef _T
#ifdef _UNICODE
#define _T(x) L ## x
#else  // !_UNICODE
#define _T(x) x
#endif // _UNICODE
#endif // _T