/**
 * @file   string.cpp
 * @author Vic P.
 * @brief  Implementation for String
 */

#include "Vutils.h"
#include "defs.h"

#include <csignal>
#include <algorithm>

#include VU_3RD_INCL(TE/include/text_encoding_detect.h)

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

// text_encoding vuapi _detect_text_encoding(const void* data, const size_t size)
// {
//   if (data == nullptr || size == 0) return text_encoding::TE_UNKNOWN;
// 
//   auto p = (uchar*)data;
// 
//   if (size == 1)
//   {
//     /* UTF-8 */
//     if (p[0] >= SCHAR_MIN && p[0] <= SCHAR_MAX) return text_encoding::TE_UTF8_BOM;
//   }
// 
//   signal(SIGSEGV, [](int signal)
//   {
//     throw signal;
//   });
// 
//   try
//   {
//     /* UTF-8 BOM */
//     if (p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF)
//     {
//       return text_encoding::TE_UTF8_BOM;
//     }
// 
//     /* UTF-32 LE BOM */
//     if (p[0] == 0xFF && p[1] == 0xFE && p[2] == 0x00 && p[3] == 0x00)
//     {
//       return text_encoding::TE_UTF32_LE_BOM;
//     }
// 
//     /* UTF-32 BE BOM */
//     if (p[0] == 0x00 && p[1] == 0x00 && p[2] == 0xFE && p[3] == 0xFF)
//     {
//       return text_encoding::TE_UTF32_BE_BOM;
//     }
// 
//     /* UTF-16 LE BOM */
//     if (p[0] == 0xFF && p[1] == 0xFE)
//     {
//       return text_encoding::TE_UTF16_LE_BOM;
//     }
// 
//     /* UTF-16 BE BOM */
//     if (p[0] == 0xFE && p[1] == 0xFF)
//     {
//       return text_encoding::TE_UTF16_BE_BOM;
//     }
// 
//     /* Without BOM */
//     if (p[0] >= SCHAR_MIN && p[0] <= SCHAR_MAX)
//     {
//       // /* UTF-16 LE */
//       // ...
//       // {
//       //   return text_encoding::TE_UTF16_LE;
//       // }
// 
//       // /* UTF-16 BE */
//       // ...
//       // {
//       //   return text_encoding::TE_UTF16_BE;
//       // }
// 
//       /* UTF-8 */
//       if (p[1] >= SCHAR_MIN && p[1] <= SCHAR_MAX)
//       {
//         return text_encoding::TE_UTF8;
//       }
//     }
//   }
//   catch (int)
//   {
//     return text_encoding::TE_UNKNOWN;
//   }
// 
//   return text_encoding::TE_UNKNOWN;
// }

text_encoding vuapi detect_text_encoding(const void* data, const size_t size)
{
  using namespace AutoIt::Common;

  static std::unordered_map<TextEncodingDetect::Encoding, vu::text_encoding> mapping_text_encodings;
  if (mapping_text_encodings.empty())
  {
    mapping_text_encodings[TextEncodingDetect::Encoding::None]  = vu::text_encoding::TE_UNKNOWN;
    mapping_text_encodings[TextEncodingDetect::Encoding::ANSI]  = vu::text_encoding::TE_UTF8;
    mapping_text_encodings[TextEncodingDetect::Encoding::ASCII] = vu::text_encoding::TE_UTF8;
    mapping_text_encodings[TextEncodingDetect::Encoding::UTF8_BOM] = vu::text_encoding::TE_UTF8_BOM;
    mapping_text_encodings[TextEncodingDetect::Encoding::UTF16_LE_NOBOM] = vu::text_encoding::TE_UTF16_LE;
    mapping_text_encodings[TextEncodingDetect::Encoding::UTF16_BE_NOBOM] = vu::text_encoding::TE_UTF16_BE;
    mapping_text_encodings[TextEncodingDetect::Encoding::UTF16_LE_BOM] = vu::text_encoding::TE_UTF16_LE_BOM;
    mapping_text_encodings[TextEncodingDetect::Encoding::UTF16_BE_BOM] = vu::text_encoding::TE_UTF16_BE_BOM;
    //mapping_text_encodings[{ TextEncodingDetect::Encoding::None] = vu::text_encoding::TE_UTF32_LE_BOM;
    //mapping_text_encodings[{ TextEncodingDetect::Encoding::None] = vu::text_encoding::TE_UTF32_BE_BOM;
  }

  TextEncodingDetect ted;
  auto autoit_encoding = ted.DetectEncoding(reinterpret_cast<const unsigned char*>(data), size);
  return mapping_text_encodings[autoit_encoding];
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

std::string vuapi to_string_A(const std::wstring& string, const bool utf8)
{
  std::string s;
  s.clear();

  if (string.empty())
  {
    return s;
  }

  int N = (int)string.length();

  if (utf8)
  {
    N = WideCharToMultiByte(CP_UTF8, 0, string.c_str(), int(string.length()), NULL, 0, NULL, NULL);
  }

  N += 1;

  std::unique_ptr<char[]> p(new char [N]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), N);

  WideCharToMultiByte(utf8 ? CP_UTF8 : CP_ACP, WC_COMPOSITECHECK, string.c_str(), -1, p.get(), N, NULL, NULL);

  s.assign(p.get());

  return s;
}

std::wstring vuapi to_string_W(const std::string& string, const bool utf8)
{
  std::wstring s;
  s.clear();

  if (string.empty())
  {
    return s;
  }

  int N = (int)string.length();

  if (utf8)
  {
    N = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), int(string.length()), NULL, 0);
  }

  N += 1;

  std::unique_ptr<wchar[]> p(new wchar [N]);
  if (p == nullptr)
  {
    s.clear();
  }

  ZeroMemory(p.get(), 2*N);

  MultiByteToWideChar(utf8 ? CP_UTF8 : CP_ACP, 0, string.c_str(), -1, p.get(), 2*N);

  s.assign(p.get());

  return s;
}

template <class std_string_t>
std::vector<std_string_t> split_string_T(
  const std_string_t& string, const std_string_t& separator, bool remove_empty)
{
  std::vector<std_string_t> l;
  l.clear();

  if (string.empty())
  {
    return l;
  }

  std_string_t s(string), sep(separator), sub;
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
  const std::string& string, const std::string& separator, bool remove_empty)
{
  return split_string_T<std::string>(string, separator, remove_empty);
}

std::vector<std::wstring> vuapi split_string_W(
  const std::wstring& string, const std::wstring& separator, bool remove_empty)
{
  return split_string_T<std::wstring>(string, separator, remove_empty);
}

template <class std_string_t>
std_string_t vuapi join_string_T(const std::vector<std_string_t> parts, const std_string_t& separator)
{
  if (parts.empty())
  {
    return std_string_t();
  }

  if (parts.size() == 1)
  {
    return parts[0];
  }

  std_string_t result;

  size_t num = parts.size();

  for (size_t i = 0; i < num; i++)
  {
    result += parts[i];

    if (i < num - 1)
    {
      result += separator;
    }
  }

  return result;
}

std::string vuapi join_string_A(const std::vector<std::string> parts, const std::string& separator)
{
  return join_string_T<std::string>(parts, separator);
}

std::wstring vuapi join_string_W(const std::vector<std::wstring> parts, const std::wstring& separator)
{
  return join_string_T<std::wstring>(parts, separator);
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
  const std_string_t& std_string,
  const trim_type& trim,
  const std_string_t& trim_chars
)
{
  std_string_t s = std_string;

  switch (trim)
  {
  case trim_type::TS_LEFT:
    s.erase(0, s.find_first_not_of(trim_chars));
    break;

  case trim_type::TS_RIGHT:
    s.erase(s.find_last_not_of(trim_chars) + 1);
    break;

  case trim_type::TS_BOTH:
    s.erase(0, s.find_first_not_of(trim_chars));
    s.erase(s.find_last_not_of(trim_chars) + 1);
    break;

  default:
    break;
  }

  return s;
}

std::string vuapi trim_string_A(
  const std::string& string, const trim_type& type, const std::string& chars)
{
  return TrimStringT<std::string>(string, type, chars);
}

std::wstring vuapi trim_string_W(
  const std::wstring& string, const trim_type& type, const std::wstring& chars)
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

template <class std_string_t>
static bool starts_with_T(const std_string_t& text, const std_string_t& with)
{
  return\
    text.length() >= with.length() &&
    memcmp(text.c_str(), with.c_str(), sizeof(typename std_string_t::value_type) * with.length()) == 0;
}

bool vuapi starts_with_A(const std::string& text, const std::string& with, bool ignore_case)
{
  if (ignore_case)
  {
    const auto v1 = lower_string_A(text);
    const auto v2 = lower_string_A(with);
    return starts_with_T<std::string>(v1, v2);
  }

  return starts_with_T<std::string>(text, with);
}

bool vuapi starts_with_W(const std::wstring& text, const std::wstring& with, bool ignore_case)
{
  if (ignore_case)
  {
    const auto v1 = lower_string_W(text);
    const auto v2 = lower_string_W(with);
    return starts_with_T<std::wstring>(v1, v2);
  }

  return starts_with_T<std::wstring>(text, with);
}

template <class std_string_t>
static bool ends_with_T(const std_string_t& text, const std_string_t& with)
{
  return text.length() >= with.length() &&
    std::equal(text.cend() - with.length(), text.cend(), with.cbegin());
}

bool vuapi ends_with_A(const std::string& text, const std::string& with, bool ignore_case)
{
  if (ignore_case)
  {
    const auto v1 = lower_string_A(text);
    const auto v2 = lower_string_A(with);
    return ends_with_T<std::string>(v1, v2);
  }

  return ends_with_T<std::string>(text, with);
}

bool vuapi ends_with_W(const std::wstring& text, const std::wstring& with, bool ignore_case)
{
  if (ignore_case)
  {
    const auto v1 = lower_string_W(text);
    const auto v2 = lower_string_W(with);
    return ends_with_T<std::wstring>(v1, v2);
  }

  return ends_with_T<std::wstring>(text, with);
}

bool vuapi contains_string_A(const std::string& text, const std::string& test, bool ignore_case)
{
  if (ignore_case)
  {
    const auto v1 = lower_string_A(text);
    const auto v2 = lower_string_A(test);
    return v1.find(v2) != std::string::npos;
  }

  return text.find(test) != std::string::npos;
}

bool vuapi contains_string_W(const std::wstring& text, const std::wstring& test, bool ignore_case)
{
  if (ignore_case)
  {
    const auto v1 = lower_string_W(text);
    const auto v2 = lower_string_W(test);
    return v1.find(v2) != std::string::npos;
  }

  return text.find(test) != std::wstring::npos;
}

bool vuapi compare_string_A(const std::string& vl, const std::string& vr, bool ignore_case)
{
  if (ignore_case)
  {
    const auto v1 = lower_string_A(vl);
    const auto v2 = lower_string_A(vr);
    return v1 == v2;
  }

  return vl == vr;
}

bool vuapi compare_string_W(const std::wstring& vl, const std::wstring& vr, bool ignore_case)
{
  if (ignore_case)
  {
    const auto v1 = lower_string_W(vl);
    const auto v2 = lower_string_W(vr);
    return v1 == v2;
  }

  return vl == vr;
}

std::string vuapi regex_replace_string_A(
  const std::string& text,
  const std::regex& pattern,
  const std::string& replacement,
  std::regex_constants::match_flag_type flags)
{
  std::string result = text;

  while (std::regex_search(result, pattern, flags))
  {
    result = std::regex_replace(result, pattern, replacement, flags);
  }

  return result;
}

std::wstring vuapi regex_replace_string_W(
  const std::wstring& text,
  const std::wregex& pattern,
  const std::wstring& replacement,
  std::regex_constants::match_flag_type flags)
{
  std::wstring result = text;

  while (std::regex_search(result, pattern, flags))
  {
    result = std::regex_replace(result, pattern, replacement, flags);
  }

  return result;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu