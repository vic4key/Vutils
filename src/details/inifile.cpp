/**
 * @file   inifile.cpp
 * @author Vic P.
 * @brief  Implementation for INI File
 */

#include "Vutils.h"
#include "strfmt.h"

namespace vu
{

INIFileA::INIFileA() : LastError()
{
  m_file_path = "";
}

INIFileA::INIFileA(const std::string& file_path) : LastError()
{
  m_file_path = file_path;
}

INIFileA::~INIFileA()
{
}

std::unique_ptr<INIFileA::Section> INIFileA::section(const std::string& name)
{
  return std::unique_ptr<Section>(new Section(*this, name));
}

bool INIFileA::read_section_names(std::vector<std::string>& section_names, const ulong max_chars)
{
  section_names.clear();

  Buffer p(max_chars);

  DWORD actual_chars = GetPrivateProfileSectionNamesA(
    LPSTR(p.pointer()), max_chars, m_file_path.c_str());

  // MSDN said:
  // If the buffer is not large enough to contain all the section names associated with the
  // specified initialization file, the return value is equal to the size specified by nSize minus two.
  if (actual_chars == max_chars - 2)
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return false;
  }

  m_last_error_code = GetLastError();

  for (char* s = LPSTR(p.pointer()); *s; s++)
  {
    std::string section_name = s;
    section_names.push_back(std::move(section_name));
    s += lstrlenA(s);
  }

  return true;
}

std::string vuapi INIFileA::read_string(
  const std::string& section, const std::string& key, const std::string& def)
{
  std::string s;
  s.clear();

  std::unique_ptr<char[]> p(new char[MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), MAX_SIZE);

  ulong result = GetPrivateProfileStringA(
    section.c_str(),
    key.c_str(),
    def.c_str(),
    p.get(),
    MAX_SIZE,
    m_file_path.c_str()
  );

  if (result == 0)
  {
    m_last_error_code = GetLastError();
    return s;
  }

  s.assign(p.get());

  return s;
}

bool vuapi INIFileA::write_string(
  const std::string& section, const std::string& key, const std::string& val)
{
  bool result = WritePrivateProfileStringA(
    section.c_str(), key.c_str(), val.c_str(), m_file_path.c_str()) != 0;
  m_last_error_code = GetLastError();
  return result;
}

vu::VariantA INIFileA::read(const std::string& section, const std::string& key)
{
  VariantA result;
  result << this->read_string(section, key, "");
  return result;
}

bool INIFileA::write(const std::string& section, const std::string& key, const VariantA& var)
{
  return this->write_string(section, key, var.to_string());
}

/**
 * INIFileW
 */

INIFileW::INIFileW() : LastError()
{
  m_file_path = L"";
}

INIFileW::INIFileW(const std::wstring& file_path) : LastError()
{
  m_file_path = file_path;
}

INIFileW::~INIFileW()
{
}

std::unique_ptr<INIFileW::Section> INIFileW::section(const std::wstring& name)
{
  return std::unique_ptr<Section>(new Section(*this, name));
}

bool INIFileW::read_section_names(std::vector<std::wstring>& section_names, const ulong max_chars)
{
  section_names.clear();

  Buffer p(max_chars * 2);

  DWORD actual_chars = GetPrivateProfileSectionNamesW(
    LPWSTR(p.pointer()), max_chars, m_file_path.c_str());

  // MSDN said:
  // If the buffer is not large enough to contain all the section names associated with the
  // specified initialization file, the return value is equal to the size specified by nSize minus two.
  if (actual_chars == max_chars - 2)
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return false;
  }

  m_last_error_code = GetLastError();

  for (wchar* s = LPWSTR(p.pointer()); *s; s++)
  {
    std::wstring section_name = s;
    section_names.push_back(std::move(section_name));
    s += lstrlenW(s);
  }

  return true;
}

std::wstring INIFileW::read_string(
  const std::wstring& section, const std::wstring& key, const std::wstring& def)
{
  std::wstring s;
  s.clear();

  std::unique_ptr<wchar[]> p(new wchar[MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), 2 * MAX_SIZE);

  ulong result = GetPrivateProfileStringW(
    section.c_str(),
    key.c_str(),
    def.c_str(),
    p.get(),
    2 * MAX_SIZE,
    m_file_path.c_str()
  );
  if (result == 0)
  {
    m_last_error_code = GetLastError();
    return s;
  }

  s.assign(p.get());

  return s;
}

bool INIFileW::write_string(
  const std::wstring& section, const std::wstring& key, const std::wstring& val)
{
  bool result = WritePrivateProfileStringW(
    section.c_str(), key.c_str(), val.c_str(), m_file_path.c_str()) != 0;
  m_last_error_code = GetLastError();
  return result;
}

vu::VariantW INIFileW::read(const std::wstring& section, const std::wstring& key)
{
  VariantW result;
  result << this->read_string(section, key, L"");
  return result;
}

bool INIFileW::write(const std::wstring& section, const std::wstring& key, const VariantW& var)
{
  return this->write_string(section, key, var.to_string());
}

} // namespace vu