/**
 * @file   spechrs.inl
 * @author Vic P.
 * @brief  Inline for Special Characters
 */

static const char* tab  = "\t";        // Tab
static const char* cr   = "\r";        // Carriage Return
static const char* lf   = "\n";        // Line Feed
static const char* crlf = "\r\n";      // Carriage Return & Line Feed

static const wchar_t* wtab  = L"\t";   // Tab
static const wchar_t* wcr   = L"\r";   // Carriage Return
static const wchar_t* wlf   = L"\n";   // Line Feed
static const wchar_t* wcrlf = L"\r\n"; // Carriage Return & Line Feed

#ifdef _UNICODE

#define ttab  wtab
#define tcr   wcr
#define tlf   wlf
#define tcrlf wcrlf

#else // !_UNICODE

#define ttab  tab
#define tcr   cr
#define tlf   lf
#define tcrlf crlf

#ifndef _T
#define _T(x) x
#endif // _T

#endif // _UNICODE