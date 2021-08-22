/**
 * @file   strfmt.h
 * @author Vic P.
 * @brief  Header for String Format
 */

#pragma once

#include "Vutils.h"

namespace vu
{

int vuapi get_format_length_A(const std::string format, ...);
int vuapi get_format_length_W(const std::wstring format, ...);
std::string vuapi get_format_string_for_number(std::string type_id);

template<typename T>
std::string vuapi number_to_string_A(T v)
{
  #if defined(__MINGW32__)
  std::string s = "";
  std::string tid = std::string(typeid(v).name());
  std::string fs = get_format_string_for_number(tid);

  int z = get_format_length_A(fs, v);
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
std::wstring vuapi number_to_string_W(T v)
{
  #if defined(__MINGW32__)
  std::wstring s = L"";
  std::string tid = std::string(typeid(v).name());

  std::string fs = get_format_string_for_number(tid);

  if (fs == "%Le")   // Does not support now
  {
    return s;
  }

  std::wstring wfs = to_string_W(fs);

  int z = get_format_length_W(wfs, v);
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