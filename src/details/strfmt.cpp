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

int vuapi get_format_length_vl_A(const std::string format, va_list args)
{
  int N = -1;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return N;
  }

  #ifdef _MSC_VER
  N = _vscprintf(format.c_str(), args) + 1;
  #else
  N = pfn_vscprintf(format.c_str(), args) + 1;
  #endif

  return N;
}

int vuapi get_format_length_vl_W(const std::wstring format, va_list args)
{
  int N = -1;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return N;
  }

  #ifdef _MSC_VER
  N = _vscwprintf(format.c_str(), args) + 1;
  #else
  N = pfn_vscwprintf(format.c_str(), args) + 1;
  #endif

  return N;
}

int vuapi get_format_length_A(const std::string format, ...)
{
  va_list args;
  va_start(args, format);

  auto N = get_format_length_vl_A(format, args);

  va_end(args);

  return N;
}

int vuapi get_format_length_W(const std::wstring format, ...)
{
  va_list args;
  va_start(args, format);

  auto N = get_format_length_vl_W(format, args);

  va_end(args);

  return N;
}

std::string vuapi format_vl_A(const std::string format, va_list args)
{
  std::string s;
  s.clear();

  auto N = get_format_length_vl_A(format, args);
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
  vsnprintf(p.get(), N, format.c_str(), args);
  #else
  pfn_vsnprintf(p.get(), N, format.c_str(), args);
  #endif

  s.assign(p.get());

  return s;
}

std::wstring vuapi format_vl_W(const std::wstring format, va_list args)
{
  std::wstring s;
  s.clear();

  auto N = get_format_length_vl_W(format, args);
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
  vswprintf(p.get(), format.c_str(), args);
  #else
  pfn_vswprintf(p.get(), N, format.c_str(), args);
  #endif

  s.assign(p.get());

  return s;
}

std::string vuapi format_A(const std::string format, ...)
{
  va_list args;
  va_start(args, format);

  auto s = format_vl_A(format, args);

  va_end(args);

  return s;
}

std::wstring vuapi format_W(const std::wstring format, ...)
{
  va_list args;
  va_start(args, format);

  auto s = format_vl_W(format, args);

  va_end(args);

  return s;
}

void vuapi msg_debug_A(const std::string format, ...)
{
  va_list args;
  va_start(args, format);

  auto s = format_vl_A(format, args);

  va_end(args);

  OutputDebugStringA(s.c_str());
}

void vuapi msg_debug_W(const std::wstring format, ...)
{
  va_list args;
  va_start(args, format);

  auto s = format_vl_W(format, args);

  va_end(args);

  OutputDebugStringW(s.c_str());
}

int vuapi msg_box_A(const std::string format, ...)
{
  va_list args;
  va_start(args, format);

  auto s = format_vl_A(format, args);

  va_end(args);

  return MessageBoxA(GetActiveWindow(), s.c_str(), VU_TITLE_BOXA.c_str(), MB_ICONINFORMATION);
}

int vuapi msg_box_W(const std::wstring format, ...)
{
  va_list args;
  va_start(args, format);

  auto s = format_vl_W(format, args);

  va_end(args);

  return MessageBoxW(GetActiveWindow(), s.c_str(), VU_TITLE_BOXW.c_str(), MB_ICONINFORMATION);
}

int vuapi msg_box_A(HWND hwnd, const std::string format, ...)
{
  va_list args;
  va_start(args, format);

  auto s = format_vl_A(format, args);

  va_end(args);

  return MessageBoxA(hwnd, s.c_str(), VU_TITLE_BOXA.c_str(), MB_ICONINFORMATION);
}

int vuapi msg_box_W(HWND hwnd, const std::wstring format, ...)
{
  va_list args;
  va_start(args, format);

  auto s = format_vl_W(format, args);

  va_end(args);

  return MessageBoxW(hwnd, s.c_str(), VU_TITLE_BOXW.c_str(), MB_ICONINFORMATION);
}

int vuapi msg_box_A(HWND hwnd, uint type, const std::string& caption, const std::string format, ...)
{
  va_list args;
  va_start(args, format);

  auto s = format_vl_A(format, args);

  va_end(args);

  return MessageBoxA(hwnd, s.c_str(), caption.c_str(), type);
}

int vuapi msg_box_W(HWND hwnd, uint type, const std::wstring& caption, const std::wstring format, ...)
{
  va_list args;
  va_start(args, format);

  auto s = format_vl_W(format, args);

  va_end(args);

  return MessageBoxW(hwnd, s.c_str(), caption.c_str(), type);
}

std::string vuapi get_last_error_A(ulong code)
{
  if (code == -1)
  {
    code = ::GetLastError();
  }

  char* ptr_error_message = nullptr;

  FormatMessageA(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    NULL,
    code,
    LANG_USER_DEFAULT,
    (char*)&ptr_error_message,
    0,
    NULL
  );

  std::string s(ptr_error_message);
  s = trim_string_A(s);

  return s;
}

std::wstring vuapi get_last_error_W(ulong code)
{
  if (code == -1)
  {
    code = ::GetLastError();
  }

  wchar* ptr_error_message = nullptr;

  FormatMessageW(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    NULL,
    code,
    LANG_USER_DEFAULT,
    (wchar*)&ptr_error_message,
    0,
    NULL
  );

  std::wstring s(ptr_error_message);
  s = trim_string_W(s);

  return s;
}

std::string vuapi get_format_string_for_number(std::string type_id)
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

  if (type_id == "i")
  {
    fs = "%d";
  }
  else if (type_id == "l")
  {
    fs = "%ld";
  }
  else if (type_id == "x")
  {
    fs = "lld";
  }
  else if (type_id == "j")
  {
    fs = "%u";
  }
  else if (type_id == "m")
  {
    fs = "%lu";
  }
  else if (type_id == "y")
  {
    fs = "%llu";
  }
  else if (type_id == "f")
  {
    fs = "%f";
  }
  else if (type_id == "d")
  {
    fs = "%e";
  }
  else if (type_id == "e")
  {
    fs = "%Le";
  }
  else
  {
    fs = "";
  }

  return fs;
}

std::string vuapi date_time_to_string_A(const time_t t)
{
  std::string s = format_date_time_A(t, "%H:%M:%S %d/%m/%Y");
  return s;
}

std::wstring vuapi date_time_to_string_W(const time_t t)
{
  std::wstring s = format_date_time_W(t, L"%H:%M:%S %d/%m/%Y");
  return s;
}

std::string vuapi format_date_time_A(const time_t t, const std::string format)
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

  strftime(p.get(), MAX_SIZE, format.c_str(), &lt);

  s.assign(p.get());

  return s;
}

std::wstring vuapi format_date_time_W(const time_t t, const std::wstring format)
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

  wcsftime(p.get(), 2*MAXBYTE, format.c_str(), &lt);

  s.assign(p.get());

  return s;
}

void vuapi hex_dump(const void* data, int size)
{
  const int DEFAULT_DUMP_COLUMN = 16;

  int i = 0;
  uchar buffer[DEFAULT_DUMP_COLUMN + 1], *ptr_data = (uchar*)data;

  for (int i = 0; i < size; i++)
  {
    if (i % DEFAULT_DUMP_COLUMN == 0)
    {
      if (i != 0)
      {
        printf("  %s\n", buffer);
      }

      printf("  %04x ", i);
    }

    if (i % DEFAULT_DUMP_COLUMN == 8) printf(" ");

    printf(" %02x", ptr_data[i]);

    if (ptr_data[i] < 0x20 || ptr_data[i] > 0x7E)
    {
      buffer[i % DEFAULT_DUMP_COLUMN] = '.';
    }
    else
    {
      buffer[i % DEFAULT_DUMP_COLUMN] = ptr_data[i];
    }

    buffer[(i % DEFAULT_DUMP_COLUMN) + 1] = '\0';
  }

  while (i % DEFAULT_DUMP_COLUMN != 0)
  {
    printf("   ");
    i++;
  }

  printf("  %s\n", buffer);
}

std::string vuapi format_bytes_A(long long bytes, eStdByte std, int digits)
{
  std::string result = "";

  if (bytes < 0 || digits < 0)
  {
    return result;
  }

  // Max positive number of long long is 2*63 ~ 9{18}.
  // So log(9{18}, 1000-1024) ~ 6-7 array index.
  static const char* SIUnits[]  = { "B", "KB", "MB", "GB", "TB", "PB", "EB" };
  static const char* IECUnits[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB" };
  static const auto  Units = std == eStdByte::IEC ? IECUnits : SIUnits;

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

  if (bytes > 0)
  {
    idx = (int)logn(static_cast<long double>(bytes), static_cast<long double>(std));

    std::string fmt = "%0.";
    fmt += std::to_string(digits);
    fmt += "f %s";

    result = format_A(fmt, float(bytes / powl(static_cast<long double>(std), idx)), Units[idx]);
  }

  return result;
}

std::wstring vuapi format_bytes_W(long long bytes, eStdByte std, int digits)
{
  return to_string_W(format_bytes_A(bytes, std, digits));
}

std::string vuapi to_hex_string_A(const byte* ptr, const size_t size)
{
  std::stringstream ss;

  for (size_t i = 0; i < size; i++)
  {
    ss << std::hex << int(ptr[i]);
  }

  return ss.str();
}

std::wstring vuapi to_hex_string_W(const byte* ptr, const size_t size)
{
  auto s = to_hex_string_A(ptr, size);
  return to_string_W(s);
}

/**
 * FundamentalA
 */

FundamentalA::FundamentalA()
{
  m_data.clear();
}

FundamentalA::~FundamentalA()
{
}

std::stringstream& FundamentalA::data()
{
  return m_data;
}

std::string FundamentalA::to_string() const
{
  return m_data.str();
}

bool FundamentalA::to_boolean() const
{
  return to_integer() != 0;
}

int FundamentalA::to_integer() const
{
  return atoi(m_data.str().c_str());
}

long FundamentalA::to_long() const
{
  return atol(m_data.str().c_str());
}

float FundamentalA::to_float() const
{
  return float(to_double());
}

double FundamentalA::to_double() const
{
  return atof(m_data.str().c_str());
}

FundamentalW::FundamentalW()
{
  m_data.clear();
}

FundamentalW::~FundamentalW()
{
}

std::wstringstream& FundamentalW::data()
{
  return m_data;
}

std::wstring FundamentalW::to_string() const
{
  return m_data.str();
}

bool FundamentalW::to_boolean() const
{
  return to_integer() != 0;
}

int FundamentalW::to_integer() const
{
  return atoi(to_string_A(m_data.str()).c_str());
}

long FundamentalW::to_long() const
{
  return atol(to_string_A(m_data.str()).c_str());
}

float FundamentalW::to_float() const
{
  return float(to_double());
}

double FundamentalW::to_double() const
{
  return atof(to_string_A(m_data.str()).c_str());
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu