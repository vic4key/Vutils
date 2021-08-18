/**
 * @file   string.cpp
 * @author Vic P.
 * @brief  Implementation for String
 */

#include "Vutils.h"

#include <csignal>
#include <algorithm>

namespace vu
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 6031)
#pragma warning(disable: 26451)
#pragma warning(disable: 26812)
#endif // _MSC_VER

#ifndef CHAR_BIT
#define CHAR_BIT      8    // number of bits in a char
#define SCHAR_MIN   (-128) // minimum signed char value
#define SCHAR_MAX     127  // maximum signed char value
#define UCHAR_MAX     255  // maximum unsigned char value
#endif

eEncodingType vuapi determine_encoding_type(const void* Data, const size_t size)
{
  if (Data == nullptr || size == 0) return eEncodingType::ET_UNKNOWN;

  auto p = (uchar*)Data;

  if (size == 1)
  {
    /* UTF-8 */
    if (p[0] >= SCHAR_MIN && p[0] <= SCHAR_MAX) return eEncodingType::ET_UTF8_BOM;
  }

  signal(SIGSEGV, [](int signal)
  {
    throw signal;
  });

  try
  {
    /* UTF-8 BOM */
    if (p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF) return eEncodingType::ET_UTF8_BOM;

    /* UTF-32 LE BOM */
    if (p[0] == 0xFF && p[1] == 0xFE && p[2] == 0x00 && p[3] == 0x00) return eEncodingType::ET_UTF32_LE_BOM;

    /* UTF-32 BE BOM */
    if (p[0] == 0x00 && p[1] == 0x00 && p[2] == 0xFE && p[3] == 0xFF) return eEncodingType::ET_UTF32_BE_BOM;

    /* UTF-16 LE BOM */
    if (p[0] == 0xFF && p[1] == 0xFE) return eEncodingType::ET_UTF16_LE_BOM;

    /* UTF-16 BE BOM */
    if (p[0] == 0xFE && p[1] == 0xFF) return eEncodingType::ET_UTF16_BE_BOM;

    /* UTF-16 LE */
    if ((p[0] >= SCHAR_MIN && p[0] <= SCHAR_MAX) && p[1] == 0x00) return eEncodingType::ET_UTF16_LE;

    /* UTF-16 BE */
    if ((p[1] >= SCHAR_MIN && p[1] <= SCHAR_MAX) && p[0] == 0x00) return eEncodingType::ET_UTF16_BE;

    /* UTF-8 */
    if ((p[0] >= SCHAR_MIN && p[0] <= SCHAR_MAX) && (p[1] >= SCHAR_MIN && p[1] <= SCHAR_MAX)) return eEncodingType::ET_UTF8;
  }
  catch (int)
  {
    return eEncodingType::ET_UNKNOWN;
  }

  return eEncodingType::ET_UNKNOWN;
}

/* ------------------------------------------------ String Working ------------------------------------------------- */

std::string vuapi lower_string_A(const std::string& String)
{
  std::string s(String);
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

std::wstring vuapi lower_string_W(const std::wstring& String)
{
  std::wstring s(String);
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

std::string vuapi upper_string_A(const std::string& String)
{
  std::string s(String);
  std::transform(s.begin(), s.end(), s.begin(), toupper);
  return s;
}

std::wstring vuapi upper_string_W(const std::wstring& String)
{
  std::wstring s(String);
  std::transform(s.begin(), s.end(), s.begin(), toupper);
  return s;
}

std::string vuapi to_string_A(const std::wstring& String)
{
  std::string s;
  s.clear();

  if (String.empty())
  {
    return s;
  }

  int N = (int)String.length() + 1;

  std::unique_ptr<char[]> p(new char [N]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), N);

  WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, String.c_str(), -1, p.get(), N, NULL, NULL);

  s.assign(p.get());

  return s;
}

std::wstring vuapi to_string_W(const std::string& String)
{
  std::wstring s;
  s.clear();

  if (String.empty())
  {
    return s;
  }

  int N = (int)String.length() + 1;

  std::unique_ptr<wchar[]> p(new wchar [N]);
  if (p == nullptr)
  {
    s.clear();
  }

  ZeroMemory(p.get(), 2*N);

  MultiByteToWideChar(CP_ACP, 0, String.c_str(), -1, p.get(), 2*N);

  s.assign(p.get());

  return s;
}

template <class std_string_t>
std::vector<std_string_t> SplitStringT(
  const std_string_t& String,
  const std_string_t& Seperate,
  bool  remempty
)
{
  std::vector<std_string_t> l;
  l.clear();

  if (String.empty())
  {
    return l;
  }

  std_string_t s(String), sep(Seperate), sub;
  ulongptr start, end;

  start = 0;
  end = s.find(sep);
  while (end != std_string_t::npos)
  {
    sub = s.substr(start, end - start);
    if (!sub.empty() || !remempty) l.push_back(std_string_t(sub.c_str()));
    start = end + sep.length();
    end = s.find(sep, start);
  }

  sub = s.substr(start, end);
  if (!sub.empty() || !remempty) l.push_back(std_string_t(sub.c_str()));

  return l;
}

std::vector<std::string> vuapi split_string_A(
  const std::string& String,
  const std::string& Seperate,
  bool  remempty 
)
{
  return SplitStringT<std::string>(String, Seperate, remempty);
}

std::vector<std::wstring> vuapi split_string_W(
  const std::wstring& String,
  const std::wstring& Seperate,
  bool  remempty
)
{
  return SplitStringT<std::wstring>(String, Seperate, remempty);
}

std::vector<std::string> vuapi multi_string_to_list_A(const char* lpcszMultiString)
{
  std::vector<std::string> l;
  l.clear();

  while (*lpcszMultiString)
  {
    l.push_back(std::string(lpcszMultiString));
    lpcszMultiString += (lstrlenA(lpcszMultiString) + 1);
  }

  return l;
}

std::vector<std::wstring> vuapi multi_string_to_list_W(const wchar* lpcwszMultiString)
{
  std::vector<std::wstring> l;
  l.clear();

  while (*lpcwszMultiString)
  {
    l.push_back(std::wstring(lpcwszMultiString));
    lpcwszMultiString += (lstrlenW(lpcwszMultiString) + 1);
  }

  return l;
}

std::unique_ptr<char[]> vuapi list_to_multi_string_A(const std::vector<std::string>& StringList)
{
  size_t ulLength = 0;
  for (std::string e : StringList)
  {
    ulLength += (e.length() + 1);
  }

  ulLength += 1; // End of multi-string.

  std::unique_ptr<char[]> p(new char [ulLength]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), ulLength);

  size_t iCrLength = 0;
  char * pP = p.get();
  for (std::string e : StringList)
  {
    iCrLength = e.length() + 1;
    lstrcpynA(pP, e.c_str(), (int)iCrLength);
    pP += iCrLength;
  }

  return p;
}

std::unique_ptr<wchar[]> vuapi list_to_multi_string_W(const std::vector<std::wstring>& StringList)
{
  size_t ulLength = 0;
  for (std::wstring e : StringList)
  {
    ulLength += (e.length() + 1);
  }

  ulLength += 1; // End of multi-string.

  std::unique_ptr<wchar[]> p(new wchar [ulLength]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), 2*ulLength);

  size_t iCrLength = 0;
  wchar * pP = p.get();
  for (std::wstring e : StringList)
  {
    iCrLength = e.length() + 1;
    lstrcpynW(pP, e.c_str(), (int)iCrLength);
    pP += iCrLength;
  }

  return p;
}

std::string vuapi load_rs_string_A(const UINT uID, const std::string& ModuleName)
{
  std::string result = "";

  HINSTANCE hInstance = ::GetModuleHandleA(ModuleName.empty() ? nullptr : ModuleName.c_str());
  if (hInstance != nullptr)
  {
    char* ps = nullptr;
    int z = ::LoadStringA(hInstance, uID, (LPSTR)&ps, 0);
    if (z > 0 && ps != nullptr)
    {
      result.assign(ps, z);
    }
  }

  return result;
}

std::wstring vuapi load_rs_string_W(const UINT uID, const std::wstring& ModuleName)
{
  std::wstring result = L"";

  HINSTANCE hInstance = ::GetModuleHandleW(ModuleName.empty() ? nullptr : ModuleName.c_str());
  if (hInstance != nullptr)
  {
    wchar* ps = nullptr;
    int z = ::LoadStringW(hInstance, uID, (LPWSTR)&ps, 0);
    if (z > 0 && ps != nullptr)
    {
      result.assign(ps, z);
    }
  }

  return result;
}

template <class std_string_t>
std_string_t TrimStringT(
  const std_string_t& String,
  const eTrimType& TrimType,
  const std_string_t& TrimChars
)
{
  std_string_t s = String;

  switch (TrimType)
  {
  case eTrimType::TS_LEFT:
    s.erase(0, s.find_first_not_of(TrimChars));
    break;

  case eTrimType::TS_RIGHT:
    s.erase(s.find_last_not_of(TrimChars) + 1);
    break;

  case eTrimType::TS_BOTH:
    s.erase(0, s.find_first_not_of(TrimChars));
    s.erase(s.find_last_not_of(TrimChars) + 1);
    break;

  default:
    break;
  }

  return s;
}

std::string vuapi trim_string_A(const std::string& String, const eTrimType& TrimType, const std::string& TrimChars)
{
  return TrimStringT<std::string>(String, TrimType, TrimChars);
}

std::wstring vuapi trim_string_W(const std::wstring& String, const eTrimType& TrimType, const std::wstring& TrimChars)
{
  return TrimStringT<std::wstring>(String, TrimType, TrimChars);
}

template <class std_string_t>
std_string_t ReplaceT(const std_string_t& Text, const std_string_t& From, const std_string_t& To)
{
  std_string_t result = Text;

  size_t start = 0;
  while ((start = result.find(From, start)) != std_string_t::npos)
  {
    result.erase(start, From.length());
    result.insert(start, To);
    start += To.length();
  }

  return result;
}

std::string vuapi replace_string_A(const std::string& Text, const std::string& From, const std::string& To)
{
  return ReplaceT<std::string>(Text, From, To);
}

std::wstring vuapi replace_string_W(const std::wstring& Text, const std::wstring& From, const std::wstring& To)
{
  return ReplaceT<std::wstring>(Text, From, To);
}

bool vuapi starts_with_A(const std::string& Text, const std::string& With)
{
  return Text.length() >= With.length() && memcmp(Text.c_str(), With.c_str(), With.length()) == 0;
}

bool vuapi starts_with_W(const std::wstring& Text, const std::wstring& With)
{
  return Text.length() >= With.length() && memcmp(Text.c_str(), With.c_str(), 2*With.length()) == 0;
}

template <class std_string_t>
static bool EndsWithT(const std_string_t& Text, const std_string_t& With)
{
  return Text.length() >= With.length() &&
    std::equal(Text.cend() - With.length(), Text.cend(), With.cbegin());
}

bool vuapi ends_with_A(const std::string& Text, const std::string& With)
{
  return EndsWithT<std::string>(Text, With);
}

bool vuapi ends_with_W(const std::wstring& Text, const std::wstring& With)
{
  return EndsWithT<std::wstring>(Text, With);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu