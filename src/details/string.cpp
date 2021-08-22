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

eEncodingType vuapi determine_encoding_type(const void* data, const size_t size)
{
  if (data == nullptr || size == 0) return eEncodingType::ET_UNKNOWN;

  auto p = (uchar*)data;

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
    if (p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF)
    {
      return eEncodingType::ET_UTF8_BOM;
    }

    /* UTF-32 LE BOM */
    if (p[0] == 0xFF && p[1] == 0xFE && p[2] == 0x00 && p[3] == 0x00)
    {
      return eEncodingType::ET_UTF32_LE_BOM;
    }

    /* UTF-32 BE BOM */
    if (p[0] == 0x00 && p[1] == 0x00 && p[2] == 0xFE && p[3] == 0xFF)
    {
      return eEncodingType::ET_UTF32_BE_BOM;
    }

    /* UTF-16 LE BOM */
    if (p[0] == 0xFF && p[1] == 0xFE)
    {
      return eEncodingType::ET_UTF16_LE_BOM;
    }

    /* UTF-16 BE BOM */
    if (p[0] == 0xFE && p[1] == 0xFF)
    {
      return eEncodingType::ET_UTF16_BE_BOM;
    }

    /* UTF-16 LE */
    if ((p[0] >= SCHAR_MIN && p[0] <= SCHAR_MAX) && p[1] == 0x00)
    {
      return eEncodingType::ET_UTF16_LE;
    }

    /* UTF-16 BE */
    if ((p[1] >= SCHAR_MIN && p[1] <= SCHAR_MAX) && p[0] == 0x00)
    {
      return eEncodingType::ET_UTF16_BE;
    }

    /* UTF-8 */
    if ((p[0] >= SCHAR_MIN && p[0] <= SCHAR_MAX) && (p[1] >= SCHAR_MIN && p[1] <= SCHAR_MAX))
    {
      return eEncodingType::ET_UTF8;
    }
  }
  catch (int)
  {
    return eEncodingType::ET_UNKNOWN;
  }

  return eEncodingType::ET_UNKNOWN;
}

/* ------------------------------------------------ String Working ------------------------------------------------- */

std::string vuapi lower_string_A(const std::string& string)
{
  std::string s(string);
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

std::wstring vuapi lower_string_W(const std::wstring& string)
{
  std::wstring s(string);
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

std::string vuapi upper_string_A(const std::string& string)
{
  std::string s(string);
  std::transform(s.begin(), s.end(), s.begin(), toupper);
  return s;
}

std::wstring vuapi upper_string_W(const std::wstring& string)
{
  std::wstring s(string);
  std::transform(s.begin(), s.end(), s.begin(), toupper);
  return s;
}

std::string vuapi to_string_A(const std::wstring& string)
{
  std::string s;
  s.clear();

  if (string.empty())
  {
    return s;
  }

  int N = (int)string.length() + 1;

  std::unique_ptr<char[]> p(new char [N]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), N);

  WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, string.c_str(), -1, p.get(), N, NULL, NULL);

  s.assign(p.get());

  return s;
}

std::wstring vuapi to_string_W(const std::string& string)
{
  std::wstring s;
  s.clear();

  if (string.empty())
  {
    return s;
  }

  int N = (int)string.length() + 1;

  std::unique_ptr<wchar[]> p(new wchar [N]);
  if (p == nullptr)
  {
    s.clear();
  }

  ZeroMemory(p.get(), 2*N);

  MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, p.get(), 2*N);

  s.assign(p.get());

  return s;
}

template <class std_string_t>
std::vector<std_string_t> split_string_T(
  const std_string_t& string, const std_string_t& seperate, bool remove_empty)
{
  std::vector<std_string_t> l;
  l.clear();

  if (string.empty())
  {
    return l;
  }

  std_string_t s(string), sep(seperate), sub;
  ulongptr start, end;

  start = 0;
  end = s.find(sep);
  while (end != std_string_t::npos)
  {
    sub = s.substr(start, end - start);
    if (!sub.empty() || !remove_empty) l.push_back(std_string_t(sub.c_str()));
    start = end + sep.length();
    end = s.find(sep, start);
  }

  sub = s.substr(start, end);
  if (!sub.empty() || !remove_empty) l.push_back(std_string_t(sub.c_str()));

  return l;
}

std::vector<std::string> vuapi split_string_A(
  const std::string& string, const std::string& seperate, bool remove_empty)
{
  return split_string_T<std::string>(string, seperate, remove_empty);
}

std::vector<std::wstring> vuapi split_string_W(
  const std::wstring& string, const std::wstring& seperate, bool remove_empty)
{
  return split_string_T<std::wstring>(string, seperate, remove_empty);
}

std::vector<std::string> vuapi multi_string_to_list_A(const char* ps_multi_string)
{
  std::vector<std::string> l;
  l.clear();

  while (*ps_multi_string)
  {
    l.push_back(std::string(ps_multi_string));
    ps_multi_string += (lstrlenA(ps_multi_string) + 1);
  }

  return l;
}

std::vector<std::wstring> vuapi multi_string_to_list_W(const wchar* ps_multi_string)
{
  std::vector<std::wstring> l;
  l.clear();

  while (*ps_multi_string)
  {
    l.push_back(std::wstring(ps_multi_string));
    ps_multi_string += (lstrlenW(ps_multi_string) + 1);
  }

  return l;
}

std::unique_ptr<char[]> vuapi list_to_multi_string_A(const std::vector<std::string>& strings)
{
  size_t length = 0;
  for (std::string e : strings)
  {
    length += (e.length() + 1);
  }

  length += 1; // End of multi-string.

  std::unique_ptr<char[]> p(new char [length]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), length);

  size_t z = 0;
  char * ptr_str = p.get();
  for (std::string e : strings)
  {
    z = e.length() + 1;
    lstrcpynA(ptr_str, e.c_str(), (int)z);
    ptr_str += z;
  }

  return p;
}

std::unique_ptr<wchar[]> vuapi list_to_multi_string_W(const std::vector<std::wstring>& strings)
{
  size_t length = 0;
  for (std::wstring e : strings)
  {
    length += (e.length() + 1);
  }

  length += 1; // End of multi-string.

  std::unique_ptr<wchar[]> p(new wchar [length]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), 2*length);

  size_t z = 0;
  wchar * ptr_str = p.get();
  for (std::wstring e : strings)
  {
    z = e.length() + 1;
    lstrcpynW(ptr_str, e.c_str(), (int)z);
    ptr_str += z;
  }

  return p;
}

std::string vuapi load_rs_string_A(const uint id, const std::string& module_name)
{
  std::string result = "";

  HINSTANCE hInstance = ::GetModuleHandleA(module_name.empty() ? nullptr : module_name.c_str());
  if (hInstance != nullptr)
  {
    char* ps = nullptr;
    int z = ::LoadStringA(hInstance, id, (LPSTR)&ps, 0);
    if (z > 0 && ps != nullptr)
    {
      result.assign(ps, z);
    }
  }

  return result;
}

std::wstring vuapi load_rs_string_W(const uint id, const std::wstring& module_name)
{
  std::wstring result = L"";

  HINSTANCE hInstance = ::GetModuleHandleW(module_name.empty() ? nullptr : module_name.c_str());
  if (hInstance != nullptr)
  {
    wchar* ps = nullptr;
    int z = ::LoadStringW(hInstance, id, (LPWSTR)&ps, 0);
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

std::string vuapi trim_string_A(
  const std::string& string, const eTrimType& type, const std::string& chars)
{
  return TrimStringT<std::string>(string, type, chars);
}

std::wstring vuapi trim_string_W(
  const std::wstring& string, const eTrimType& type, const std::wstring& chars)
{
  return TrimStringT<std::wstring>(string, type, chars);
}

template <class std_string_t>
std_string_t replace_string_T(
  const std_string_t& string, const std_string_t& from, const std_string_t& to)
{
  std_string_t result = string;

  size_t start = 0;
  while ((start = result.find(from, start)) != std_string_t::npos)
  {
    result.erase(start, from.length());
    result.insert(start, to);
    start += to.length();
  }

  return result;
}

std::string vuapi replace_string_A(
  const std::string& text, const std::string& from, const std::string& to)
{
  return replace_string_T<std::string>(text, from, to);
}

std::wstring vuapi replace_string_W(
  const std::wstring& text, const std::wstring& from, const std::wstring& to)
{
  return replace_string_T<std::wstring>(text, from, to);
}

bool vuapi starts_with_A(const std::string& text, const std::string& with)
{
  return text.length() >= with.length() && memcmp(text.c_str(), with.c_str(), with.length()) == 0;
}

bool vuapi starts_with_W(const std::wstring& text, const std::wstring& with)
{
  return text.length() >= with.length() && memcmp(text.c_str(), with.c_str(), 2*with.length()) == 0;
}

template <class std_string_t>
static bool ends_with_T(const std_string_t& text, const std_string_t& with)
{
  return text.length() >= with.length() &&
    std::equal(text.cend() - with.length(), text.cend(), with.cbegin());
}

bool vuapi ends_with_A(const std::string& text, const std::string& with)
{
  return ends_with_T<std::string>(text, with);
}

bool vuapi ends_with_W(const std::wstring& text, const std::wstring& with)
{
  return ends_with_T<std::wstring>(text, with);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu