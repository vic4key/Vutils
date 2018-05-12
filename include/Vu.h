/**
 * @file   Vu.h
 * @author Vic P.
 * @brief  Shorten Inclusion Header for Vutils
 */

#ifndef VU_H
#define VU_H

#include <Vutils.h>
#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#if defined(_DLL) // MD & MDd
#ifdef _DEBUG     // MDd
#pragma comment(lib, "Vutils-MDd.lib")
#else             // MD
#pragma comment(lib, "Vutils-MD.lib")
#endif // _DEBUG
#else             // MT & MTd
#ifdef _DEBUG     // MTd
#pragma comment(lib, "Vutils-MTd.lib")
#else             // MT
#pragma comment(lib, "Vutils-MT.lib")
#endif // _DEBUG
#endif
#endif // _DLL

#endif // VU_H