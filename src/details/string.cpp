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

#ifndef CHAR_BIT
#define CHAR_BIT      8    // number of bits in a char
#define SCHAR_MIN   (-128) // minimum signed char value
#define SCHAR_MAX     127  // maximum signed char value
#define UCHAR_MAX     255  // maximum unsigned char value
#endif

eEncodingType vuapi DetermineEncodingType(const void* Data, const size_t size)
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

std::string vuapi LowerStringA(const std::string& String)
{
  std::string s(String);
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

std::wstring vuapi LowerStringW(const std::wstring& String)
{
  std::wstring s(String);
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

std::string vuapi UpperStringA(const std::string& String)
{
  std::string s(String);
  std::transform(s.begin(), s.end(), s.begin(), toupper);
  return s;
}

std::wstring vuapi UpperStringW(const std::wstring& String)
{
  std::wstring s(String);
  std::transform(s.begin(), s.end(), s.begin(), toupper);
  return s;
}

std::string vuapi ToStringA(const std::wstring& String)
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

std::wstring vuapi ToStringW(const std::string& String)
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

std::vector<std::string> vuapi SplitStringA(const std::string& String, const std::string& Seperate)
{
  std::vector<std::string> l;
  l.clear();

  if (String.empty())
  {
    return l;
  }

  std::string s(String), sep(Seperate), sub;
  ulongptr start, end;

  start = 0;
  end = s.find(sep);
  while (end != std::string::npos)
  {
    sub = s.substr(start, end - start);
    l.push_back(std::string(sub.c_str()));
    start = end + sep.length();
    end = s.find(sep, start);
  }

  sub = s.substr(start, end);
  l.push_back(std::string(sub.c_str()));

  return l;
}

std::vector<std::wstring> vuapi SplitStringW(const std::wstring& String, const std::wstring& Seperate)
{
  std::vector<std::wstring> l;
  l.clear();

  if (String.empty())
  {
    return l;
  }

  std::wstring s(String), sep(Seperate), sub;
  ulongptr start, end;

  start = 0;
  end = s.find(sep);
  while (end != std::wstring::npos)
  {
    sub = s.substr(start, end - start);
    l.push_back(std::wstring(sub.c_str()));
    start = end + sep.length();
    end = s.find(sep, start);
  }

  sub = s.substr(start, end);
  l.push_back(std::wstring(sub.c_str()));

  return l;
}

std::vector<std::string> vuapi MultiStringToListA(const char* lpcszMultiString)
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

std::vector<std::wstring> vuapi MultiStringToListW(const wchar* lpcwszMultiString)
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

std::unique_ptr<char[]> vuapi ListToMultiStringA(const std::vector<std::string>& StringList)
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

std::unique_ptr<wchar[]> vuapi ListToMultiStringW(const std::vector<std::wstring>& StringList)
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

std::string vuapi LoadResourceStringA(const UINT uID, HINSTANCE ModuleHandle, const std::string& ModuleName)
{
  char* ps = nullptr;
  std::string s;
  s.clear();

  if ((ModuleHandle == nullptr || ModuleHandle == INVALID_HANDLE_VALUE) && ModuleName.length() != 0)
  {
    ModuleHandle = GetModuleHandleA(ModuleName.c_str());
  }

  auto z = LoadStringA(ModuleHandle, uID, (LPSTR)&ps, 0);
  if (z > 0)
  {
    s.assign(ps, z);
  }

  return s;
}

std::wstring vuapi LoadResourceStringW(const UINT uID, HINSTANCE ModuleHandle, const std::wstring& ModuleName)
{
  wchar* ps = nullptr;
  std::wstring s;
  s.clear();

  if ((ModuleHandle == nullptr || ModuleHandle == INVALID_HANDLE_VALUE) && ModuleName.length() != 0)
  {
    ModuleHandle = GetModuleHandleW(ModuleName.c_str());
  }

  auto z = LoadStringW(ModuleHandle, uID, (LPWSTR)&ps, 0);
  if (z > 0)
  {
    s.assign(ps, z);
  }

  return s;
}

std::string vuapi TrimStringA(const std::string& String, const eTrimType& TrimType, const std::string& TrimChars)
{
  auto s = String;

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

std::wstring vuapi TrimStringW(const std::wstring& String, const eTrimType& TrimType, const std::wstring& TrimChars)
{
  auto s = String;

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

} // namespace vu