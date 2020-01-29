/**
 * @file   strfmt.h
 * @author Vic P.
 * @brief  Header for String Format
 */

#pragma once

#include "Vutils.h"

namespace vu
{

int vuapi GetFormatLengthA(const std::string Format, ...);
int vuapi GetFormatLengthW(const std::wstring Format, ...);
std::string vuapi GetFormatStringForNumber(std::string TypeID);

template<typename T>
std::string vuapi NumberToStringA(T v)
{
  #if defined(__MINGW32__)
  std::string s = "";
  std::string tid = std::string(typeid(v).name());
  std::string fs = GetFormatStringForNumber(tid);

  int z = GetFormatLengthA(fs, v);
  if (z <= 0)
  {
    return s;
  }

  std::unique_ptr<char[]> p(new char [z]);
  memset(p.get(), 0, z*sizeof(char));
  sprintf(p.get(), fs.c_str(), v);
  s.assign(p.get());

  return s;
  #else // defined(_MSC_VER) - defined(__BCPLUSPLUS__)
  return std::to_string(v);
  #endif
}

template<typename T>
std::wstring vuapi NumberToStringW(T v)
{
  #if defined(__MINGW32__)
  std::wstring s = L"";
  std::string tid = std::string(typeid(v).name());

  std::string fs = GetFormatStringForNumber(tid);

  if (fs == "%Le")   // Does not support now
  {
    return s;
  }

  std::wstring wfs = ToStringW(fs);

  int z = GetFormatLengthW(wfs, v);
  if (z <= 0)
  {
    return s;
  }

  std::unique_ptr<wchar[]> p(new wchar [z]);
  memset(p.get(), 0, z*sizeof(wchar));
  _snwprintf(p.get(), z*sizeof(wchar), wfs.c_str(), v);
  s.assign(p.get());

  return s;
  #else // defined(_MSC_VER) - defined(__BCPLUSPLUS__)
  return std::to_wstring(v);
  #endif
}

} // namespace vu