/**
 * @file   strfmt.cpp
 * @author Vic P.
 * @brief  Implementation for String Format
 */

#include "strfmt.h"
#include "lazy.h"

#include <math.h>

namespace vu
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 26451)
#pragma warning(disable: 26812)
#endif // _MSC_VER

const std::string  VU_TITLE_BOXA =  "Vutils";
const std::wstring VU_TITLE_BOXW = L"Vutils";

int vuapi GetFormatLengthVLA(const std::string Format, va_list args)
{
  int N = -1;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return N;
  }

  #ifdef _MSC_VER
  N = _vscprintf(Format.c_str(), args) + 1;
  #else
  N = pfn_vscprintf(Format.c_str(), args) + 1;
  #endif

  return N;
}

int vuapi GetFormatLengthVLW(const std::wstring Format, va_list args)
{
  int N = -1;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return N;
  }

  #ifdef _MSC_VER
  N = _vscwprintf(Format.c_str(), args) + 1;
  #else
  N = pfn_vscwprintf(Format.c_str(), args) + 1;
  #endif

  return N;
}

int vuapi GetFormatLengthA(const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto N = GetFormatLengthVLA(Format, args);

  va_end(args);

  return N;
}

int vuapi GetFormatLengthW(const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto N = GetFormatLengthVLW(Format, args);

  va_end(args);

  return N;
}

std::string vuapi FormatVLA(const std::string Format, va_list args)
{
  std::string s;
  s.clear();

  auto N = GetFormatLengthVLA(Format, args);
  if (N <= 0)
  {
    return s;
  }

  std::unique_ptr<char[]> p(new char [N]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), N);

  #ifdef _MSC_VER
  vsnprintf(p.get(), N, Format.c_str(), args);
  #else
  pfn_vsnprintf(p.get(), N, Format.c_str(), args);
  #endif

  s.assign(p.get());

  return s;
}

std::wstring vuapi FormatVLW(const std::wstring Format, va_list args)
{
  std::wstring s;
  s.clear();

  auto N = GetFormatLengthVLW(Format, args);
  if (N <= 0)
  {
    return s;
  }

  std::unique_ptr<wchar[]> p(new wchar [N]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), 2*N);

  #ifdef _MSC_VER
  vswprintf(p.get(), Format.c_str(), args);
  #else
  pfn_vswprintf(p.get(), N, Format.c_str(), args);
  #endif

  s.assign(p.get());

  return s;
}

std::string vuapi FormatA(const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLA(Format, args);

  va_end(args);

  return s;
}

std::wstring vuapi FormatW(const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLW(Format, args);

  va_end(args);

  return s;
}

void vuapi MsgA(const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLA(Format, args);

  va_end(args);

  OutputDebugStringA(s.c_str());
}

void vuapi MsgW(const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLW(Format, args);

  va_end(args);

  OutputDebugStringW(s.c_str());
}

int vuapi BoxA(const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLA(Format, args);

  va_end(args);

  return MessageBoxA(GetActiveWindow(), s.c_str(), VU_TITLE_BOXA.c_str(), MB_ICONINFORMATION);
}

int vuapi BoxW(const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLW(Format, args);

  va_end(args);

  return MessageBoxW(GetActiveWindow(), s.c_str(), VU_TITLE_BOXW.c_str(), MB_ICONINFORMATION);
}

int vuapi BoxA(HWND hWnd, const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLA(Format, args);

  va_end(args);

  return MessageBoxA(hWnd, s.c_str(), VU_TITLE_BOXA.c_str(), MB_ICONINFORMATION);
}

int vuapi BoxW(HWND hWnd, const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLW(Format, args);

  va_end(args);

  return MessageBoxW(hWnd, s.c_str(), VU_TITLE_BOXW.c_str(), MB_ICONINFORMATION);
}

int vuapi BoxA(HWND hWnd, uint uType, const std::string& Caption, const std::string Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLA(Format, args);

  va_end(args);

  return MessageBoxA(hWnd, s.c_str(), Caption.c_str(), uType);
}

int vuapi BoxW(HWND hWnd, uint uType, const std::wstring& Caption, const std::wstring Format, ...)
{
  va_list args;
  va_start(args, Format);

  auto s = FormatVLW(Format, args);

  va_end(args);

  return MessageBoxW(hWnd, s.c_str(), Caption.c_str(), uType);
}

std::string vuapi LastErrorA(ulong ulErrorCode)
{
  if (ulErrorCode == -1)
  {
    ulErrorCode = ::GetLastError();
  }

  char* lpszErrorMessage = nullptr;

  FormatMessageA(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    NULL,
    ulErrorCode,
    LANG_USER_DEFAULT,
    (char*)&lpszErrorMessage,
    0,
    NULL
  );

  std::string s(lpszErrorMessage);
  s = TrimStringA(s);

  return s;
}

std::wstring vuapi LastErrorW(ulong ulErrorCode)
{
  if (ulErrorCode == -1)
  {
    ulErrorCode = ::GetLastError();
  }

  wchar* lpwszErrorMessage = nullptr;

  FormatMessageW(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    NULL,
    ulErrorCode,
    LANG_USER_DEFAULT,
    (wchar*)&lpwszErrorMessage,
    0,
    NULL
  );

  std::wstring s(lpwszErrorMessage);
  s = TrimStringW(s);

  return s;
}

std::string vuapi GetFormatStringForNumber(std::string TypeID)
{
  /* MinGW
    i -> int
    l -> long
    x -> long long
    j -> unsigned
    m -> unsigned long
    y -> unsigned long long
    f -> float
    d -> double
    e -> long double
  */

  std::string fs = "";

  if (TypeID == "i")
  {
    fs = "%d";
  }
  else if (TypeID == "l")
  {
    fs = "%ld";
  }
  else if (TypeID == "x")
  {
    fs = "lld";
  }
  else if (TypeID == "j")
  {
    fs = "%u";
  }
  else if (TypeID == "m")
  {
    fs = "%lu";
  }
  else if (TypeID == "y")
  {
    fs = "%llu";
  }
  else if (TypeID == "f")
  {
    fs = "%f";
  }
  else if (TypeID == "d")
  {
    fs = "%e";
  }
  else if (TypeID == "e")
  {
    fs = "%Le";
  }
  else
  {
    fs = "";
  }

  return fs;
}

std::string vuapi DateTimeToStringA(const time_t t)
{
  std::string s = FormatDateTimeA(t, "%H:%M:%S %d/%m/%Y");
  return s;
}

std::wstring vuapi DateTimeToStringW(const time_t t)
{
  std::wstring s = FormatDateTimeW(t, L"%H:%M:%S %d/%m/%Y");
  return s;
}

std::string vuapi FormatDateTimeA(const time_t t, const std::string Format)
{
  std::string s;
  s.clear();

  std::unique_ptr<char[]> p(new char [MAXBYTE]);
  if (p == nullptr)
  {
    return s;
  }

  tm lt = {0};

  ZeroMemory(p.get(), MAX_SIZE);

  #if defined(_MSC_VER) && (_MSC_VER > 1200) // Above VC++ 6.0
  localtime_s(&lt, &t);
  #else
  memcpy((void*)&lt, localtime(&t), sizeof(tm));
  #endif

  strftime(p.get(), MAX_SIZE, Format.c_str(), &lt);

  s.assign(p.get());

  return s;
}

std::wstring vuapi FormatDateTimeW(const time_t t, const std::wstring Format)
{
  std::wstring s;
  s.clear();

  std::unique_ptr<wchar[]> p(new wchar [MAXBYTE]);
  if (p == nullptr) return s;

  tm lt = {0};

  ZeroMemory(p.get(), 2*MAXBYTE);

  #if defined(_MSC_VER) && (_MSC_VER > 1200) // Above VC++ 6.0
  localtime_s(&lt, &t);
  #else
  memcpy((void*)&lt, localtime(&t), sizeof(tm));
  #endif

  wcsftime(p.get(), 2*MAXBYTE, Format.c_str(), &lt);

  s.assign(p.get());

  return s;
}

void vuapi HexDump(const void* Data, int Size)
{
  const int DEFAULT_DUMP_COLUMN = 16;

  int i = 0;
  uchar Buffer[DEFAULT_DUMP_COLUMN + 1], *pData = (uchar*)Data;

  for (int i = 0; i < Size; i++)
  {
    if (i % DEFAULT_DUMP_COLUMN == 0)
    {
      if (i != 0)
      {
        printf("  %s\n", Buffer);
      }

      printf("  %04x ", i);
    }

    if (i % DEFAULT_DUMP_COLUMN == 8) printf(" ");

    printf(" %02x", pData[i]);

    if (pData[i] < 0x20 || pData[i] > 0x7E)
    {
      Buffer[i % DEFAULT_DUMP_COLUMN] = '.';
    }
    else
    {
      Buffer[i % DEFAULT_DUMP_COLUMN] = pData[i];
    }

    Buffer[(i % DEFAULT_DUMP_COLUMN) + 1] = '\0';
  }

  while (i % DEFAULT_DUMP_COLUMN != 0)
  {
    printf("   ");
    i++;
  }

  printf("  %s\n", Buffer);
}

std::string vuapi FormatBytesA(long long Bytes, eStdByte Std, int Digits)
{
  std::string result = "";

  if (Bytes < 0 || Digits < 0)
  {
    return result;
  }

  const auto bytes  = (long double)Bytes;
  const auto thestd = (long double)Std;

  // Max positive number of long long is 2*63 ~ 9{18}.
  // So log(9{18}, 1000-1024) ~ 6-7 array index.
  static const char* SIUnits[]  = { "B", "KB", "MB", "GB", "TB", "PB", "EB" };
  static const char* IECUnits[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB" };
  static const auto  Units = Std == eStdByte::IEC ? IECUnits : SIUnits;

  const auto log2l = [](long double v) -> long double
  {
    const long double M_LOG2E = 1.44269504088896340736;
    return logl(v) * M_LOG2E;
  };

  const auto logn = [&](long double v, long double n) -> long double
  {
    return log2l(v) / log2l(n);
  };

  int idx = 0;

  if (Bytes > 0)
  {
    idx = (int)logn(bytes, thestd);

    std::string fmt = "%0.";
    fmt += std::to_string(Digits);
    fmt += "f %s";

    result = FormatA(fmt, float(bytes / powl(thestd, idx)), Units[idx]);
  }

  return result;
}

std::wstring vuapi FormatBytesW(long long Bytes, eStdByte Std, int Digits)
{
  return ToStringW(FormatBytesA(Bytes, Std, Digits));
}

/**
 * CFundamental
 */

CFundamentalA::CFundamentalA()
{
  m_Data.clear();
}

CFundamentalA::~CFundamentalA()
{
}

std::stringstream& CFundamentalA::Data()
{
  return m_Data;
}

std::string CFundamentalA::String() const
{
  return m_Data.str();
}

bool CFundamentalA::Boolean() const
{
  return Integer() != 0;
}

int CFundamentalA::Integer() const
{
  return atoi(m_Data.str().c_str());
}

long CFundamentalA::Long() const
{
  return atol(m_Data.str().c_str());
}

float CFundamentalA::Float() const
{
  return float(Double());
}

double CFundamentalA::Double() const
{
  return atof(m_Data.str().c_str());
}

CFundamentalW::CFundamentalW()
{
  m_Data.clear();
}

CFundamentalW::~CFundamentalW()
{
}

std::wstringstream& CFundamentalW::Data()
{
  return m_Data;
}

std::wstring CFundamentalW::String() const
{
  return m_Data.str();
}

bool CFundamentalW::Boolean() const
{
  return Integer() != 0;
}

int CFundamentalW::Integer() const
{
  return atoi(ToStringA(m_Data.str()).c_str());
}

long CFundamentalW::Long() const
{
  return atol(ToStringA(m_Data.str()).c_str());
}

float CFundamentalW::Float() const
{
  return float(Double());
}

double CFundamentalW::Double() const
{
  return atof(ToStringA(m_Data.str()).c_str());
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu