/**
 * @file   inifile.cpp
 * @author Vic P.
 * @brief  Implementation for INI File
 */

#include "Vutils.h"
#include "strfmt.h"

namespace vu
{

CINIFileA::CINIFileA() : CLastError()
{
  m_section  = "";
  m_file_path = "";
}

CINIFileA::CINIFileA(const std::string& file_path) : CLastError()
{
  m_section  = "";
  m_file_path = file_path;
}

CINIFileA::~CINIFileA()
{
}

void CINIFileA::update_file_path()
{
  if (m_file_path.empty())
  {
    std::string file_path = get_current_file_path_A();
    std::string file_dir  = extract_file_directory_A(file_path, true);
    std::string file_name = extract_file_name_A(file_path, false);
    m_file_path = file_dir + file_name + ".INI";
  }
}

void CINIFileA::set_current_file_path(const std::string& file_path)
{
  m_file_path = file_path;
}

void CINIFileA::set_current_section(const std::string& section)
{
  m_section = section;
}

// Long-Read

std::vector<std::string> vuapi CINIFileA::read_section_names(ulong max_size)
{
  std::vector<std::string> l;
  l.clear();

  std::unique_ptr<char[]> p(new char [max_size]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), max_size);

  this->update_file_path();

  GetPrivateProfileSectionNamesA(p.get(), max_size, m_file_path.c_str());

  m_last_error_code = GetLastError();

  for (char * s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::string(s))->c_str());
    s += lstrlenA(s);
  }

  return l;
}

std::vector<std::string> vuapi CINIFileA::read_section(const std::string& section, ulong max_size)
{
  std::vector<std::string> l;
  l.clear();

  std::unique_ptr<char[]> p(new char[max_size]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), max_size);

  this->update_file_path();

  GetPrivateProfileSectionA(section.c_str(), p.get(), max_size, m_file_path.c_str());

  m_last_error_code = GetLastError();

  for (char* s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::string(s))->c_str());
    s += lstrlenA(s);
  }

  return l;
}

int vuapi CINIFileA::read_integer(const std::string& section, const std::string& key, int default_value)
{
  this->update_file_path();
  uint result = GetPrivateProfileIntA(section.c_str(), key.c_str(), default_value, m_file_path.c_str());
  m_last_error_code = GetLastError();
  return result;
}

bool vuapi CINIFileA::read_bool(const std::string& section, const std::string& key, bool default_value)
{
  this->update_file_path();
  bool result = GetPrivateProfileIntA(section.c_str(), key.c_str(), default_value, m_file_path.c_str()) == 1 ? true : false;
  m_last_error_code = GetLastError();
  return result;
}

float vuapi CINIFileA::read_float(const std::string& section, const std::string& key, float default_value)
{
  const std::string tmp_default = number_to_string_A(default_value);
  char sz_result[MAX_SIZE];

  ZeroMemory(sz_result, sizeof(sz_result));

  this->update_file_path();

  GetPrivateProfileStringA(section.c_str(), key.c_str(), tmp_default.c_str(), sz_result, MAX_SIZE, m_file_path.c_str());

  m_last_error_code = GetLastError();

  return (float)atof(sz_result);
}

std::string vuapi CINIFileA::read_string(
  const std::string& section,
  const std::string& key,
  const std::string& default_value
)
{
  std::string s;
  s.clear();

  std::unique_ptr<char[]> p(new char[MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), MAX_SIZE);

  this->update_file_path();

  ulong result = GetPrivateProfileStringA(
    section.c_str(),
    key.c_str(),
    default_value.c_str(),
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

std::unique_ptr<uchar[]> vuapi CINIFileA::read_struct(const std::string& section, const std::string& key, ulong size)
{
  std::unique_ptr<uchar[]> p(new uchar[size]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), size);

  this->update_file_path();

  if (GetPrivateProfileStructA(section.c_str(), key.c_str(), (void*)p.get(), size, m_file_path.c_str()) == 0)
  {
    m_last_error_code = GetLastError();
    return nullptr;
  }

  return p;
}

// Short-Read

std::vector<std::string> vuapi CINIFileA::read_current_section(ulong max_size)
{
  return this->read_section(m_section, max_size);
}

int vuapi CINIFileA::read_integer(const std::string& key, int default_value)
{
  return this->read_integer(m_section, key, default_value);
}

bool vuapi CINIFileA::read_bool(const std::string& key, bool default_value)
{
  return this->read_bool(m_section, key, default_value);
}

float vuapi CINIFileA::read_float(const std::string& key, float default_value)
{
  return this->read_float(m_section, key, default_value);
}

std::string vuapi CINIFileA::read_string(const std::string& key, const std::string& default_value)
{
  return this->read_string(m_section, key, default_value);
}

std::unique_ptr<uchar[]> vuapi CINIFileA::read_struct(const std::string& key, ulong size)
{
  return this->read_struct(m_section, key, size);
}

// Long-Write

bool vuapi CINIFileA::write_integer(const std::string& section, const std::string& key, int value)
{
  this->update_file_path();
  const std::string s = number_to_string_A(value);
  bool result = WritePrivateProfileStringA(section.c_str(), key.c_str(), s.c_str(), m_file_path.c_str()) != 0;
  m_last_error_code = GetLastError();
  return result;
}

bool vuapi CINIFileA::write_bool(const std::string& section, const std::string& key, bool value)
{
  this->update_file_path();
  const std::string s(value ? "1" : "0");
  bool result = WritePrivateProfileStringA(section.c_str(), key.c_str(), s.c_str(), m_file_path.c_str()) != 0;
  m_last_error_code = GetLastError();
  return result;
}

bool vuapi CINIFileA::write_float(const std::string& section, const std::string& key, float value)
{
  this->update_file_path();
  const std::string s = number_to_string_A(value);
  bool result = WritePrivateProfileStringA(section.c_str(), key.c_str(), s.c_str(), m_file_path.c_str()) != 0;
  m_last_error_code = GetLastError();
  return result;
}

bool vuapi CINIFileA::write_string(const std::string& section, const std::string& key, const std::string& value)
{
  this->update_file_path();
  bool result = WritePrivateProfileStringA(section.c_str(), key.c_str(), value.c_str(), m_file_path.c_str()) != 0;
  m_last_error_code = GetLastError();
  return result;
}

bool vuapi CINIFileA::write_struct(const std::string& section, const std::string& key, void* ptr_struct, ulong size)
{
  this->update_file_path();
  bool result = WritePrivateProfileStructA(section.c_str(), key.c_str(), ptr_struct, size, m_file_path.c_str()) != 0;
  m_last_error_code = GetLastError();
  return result;
}

// Short-Write

bool vuapi CINIFileA::write_integer(const std::string& key, int value)
{
  return this->write_integer(m_section, key, value);
}

bool vuapi CINIFileA::write_bool(const std::string& key, bool value)
{
  return this->write_bool(m_section, key, value);
}

bool vuapi CINIFileA::write_float(const std::string& key, float value)
{
  return this->write_float(m_section, key, value);
}

bool vuapi CINIFileA::write_string(const std::string& key, const std::string& value)
{
  return this->write_string(m_section, key, value);
}

bool vuapi CINIFileA::write_struct(const std::string& key, void* ptr_struct, ulong size)
{
  return this->write_struct(m_section, key, ptr_struct, size);
}

CINIFileW::CINIFileW() : CLastError()
{
  m_section = L"";
  m_file_path = L"";
}

CINIFileW::CINIFileW(const std::wstring& file_path) : CLastError()
{
  m_section = L"";
  m_file_path = file_path;
}

CINIFileW::~CINIFileW()
{
}

void CINIFileW::update_file_path()
{
  if (m_file_path.empty())
  {
    std::wstring file_path = get_current_file_path_W();
    std::wstring file_dir = extract_file_directory_W(file_path, true);
    std::wstring file_name = extract_file_name_W(file_path, false);
    m_file_path = file_dir + file_name + L".INI";
  }
}

void CINIFileW::set_current_file_path(const std::wstring& file_path)
{
  m_file_path = file_path;
}

void CINIFileW::set_current_section(const std::wstring& section)
{
  m_section = section;
}

// Long-Read

std::vector<std::wstring> vuapi CINIFileW::read_section_names(ulong max_size)
{
  std::vector<std::wstring> l;
  l.clear();

  std::unique_ptr<wchar[]> p(new wchar[max_size]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), 2 * max_size);

  this->update_file_path();

  GetPrivateProfileSectionNamesW(p.get(), 2 * max_size, m_file_path.c_str());

  m_last_error_code = GetLastError();

  for (wchar* s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::wstring(s))->c_str());
    s += lstrlenW(s);
  }

  return l;
}

std::vector<std::wstring> vuapi CINIFileW::read_section(const std::wstring& section, ulong max_size)
{
  std::vector<std::wstring> l;
  l.clear();

  std::unique_ptr<wchar[]> p(new wchar[max_size]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), 2 * max_size);

  this->update_file_path();

  GetPrivateProfileSectionW(section.c_str(), p.get(), 2 * max_size, m_file_path.c_str());

  m_last_error_code = GetLastError();

  for (wchar* s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::wstring(s))->c_str());
    s += lstrlenW(s);
  }

  return l;
}

int vuapi CINIFileW::read_integer(const std::wstring& section, const std::wstring& key, int default_value)
{
  this->update_file_path();
  int result = GetPrivateProfileIntW(section.c_str(), key.c_str(), default_value, m_file_path.c_str());
  m_last_error_code = GetLastError();
  return result;
}

bool vuapi CINIFileW::read_bool(const std::wstring& section, const std::wstring& key, bool default_value)
{
  this->update_file_path();
  bool result = GetPrivateProfileIntW(section.c_str(), key.c_str(), default_value, m_file_path.c_str()) == 1 ? true : false;
  m_last_error_code = GetLastError();
  return result;
}

float vuapi CINIFileW::read_float(const std::wstring& section, const std::wstring& key, float default_value)
{
  this->update_file_path();

  const std::wstring tmp_default = number_to_string_W(default_value);
  wchar sz_result[MAX_SIZE];

  ZeroMemory(sz_result, sizeof(sz_result));

  GetPrivateProfileStringW(
    section.c_str(),
    key.c_str(),
    tmp_default.c_str(),
    sz_result,
    sizeof(sz_result),
    m_file_path.c_str()
  );

  m_last_error_code = GetLastError();

  const std::string s = to_string_A(sz_result);

  return (float)atof(s.c_str());
}

std::wstring vuapi CINIFileW::read_string(
  const std::wstring& section,
  const std::wstring& key,
  const std::wstring& default_value
)
{
  std::wstring s;
  s.clear();

  std::unique_ptr<wchar[]> p(new wchar[MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), 2 * MAX_SIZE);

  this->update_file_path();

  ulong result = GetPrivateProfileStringW(
    section.c_str(),
    key.c_str(),
    default_value.c_str(),
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

std::unique_ptr<uchar[]> vuapi CINIFileW::read_struct(
  const std::wstring& section,
  const std::wstring& key,
  ulong size
)
{
  std::unique_ptr<uchar[]> p(new uchar[size]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), size);

  this->update_file_path();

  if (GetPrivateProfileStructW(section.c_str(), key.c_str(), (void*)p.get(), size, m_file_path.c_str()) == 0)
  {
    m_last_error_code = GetLastError();
    return nullptr;
  }

  return p;
}

// Short-Read

std::vector<std::wstring> vuapi CINIFileW::read_current_section(ulong max_size)
{
  return this->read_section(m_section, max_size);
}

int vuapi CINIFileW::read_integer(const std::wstring& key, int default_value)
{
  return this->read_integer(m_section, key, default_value);
}

bool vuapi CINIFileW::read_bool(const std::wstring& key, bool default_value)
{
  return this->read_bool(m_section, key, default_value);
}

float vuapi CINIFileW::read_float(const std::wstring& key, float default_value)
{
  return this->read_float(m_section, key, default_value);
}

std::wstring vuapi CINIFileW::read_string(const std::wstring& key, const std::wstring& default_value)
{
  return this->read_string(m_section, key, default_value);
}

std::unique_ptr<uchar[]> vuapi CINIFileW::read_struct(const std::wstring& key, ulong size)
{
  return this->read_struct(m_section, key, size);
}

// Long-Write

bool vuapi CINIFileW::write_integer(const std::wstring& section, const std::wstring& key, int value)
{
  this->update_file_path();
  const std::wstring s = number_to_string_W(value);
  bool result = WritePrivateProfileStringW(section.c_str(), key.c_str(), s.c_str(), m_file_path.c_str()) != 0;
  m_last_error_code = GetLastError();
  return result;
}

bool vuapi CINIFileW::write_bool(const std::wstring& section, const std::wstring& key, bool value)
{
  this->update_file_path();
  const std::wstring s(value ? L"1" : L"0");
  return (WritePrivateProfileStringW(section.c_str(), key.c_str(), s.c_str(), m_file_path.c_str()) != 0);
}

bool vuapi CINIFileW::write_float(const std::wstring& section, const std::wstring& key, float value)
{
  this->update_file_path();
  const std::wstring s = number_to_string_W(value);
  bool result = WritePrivateProfileStringW(section.c_str(), key.c_str(), s.c_str(), m_file_path.c_str()) != 0;
  m_last_error_code = GetLastError();
  return result;
}

bool vuapi CINIFileW::write_string(const std::wstring& section, const std::wstring& key, const std::wstring& value)
{
  this->update_file_path();
  bool result = WritePrivateProfileStringW(section.c_str(), key.c_str(), value.c_str(), m_file_path.c_str()) != 0;
  m_last_error_code = GetLastError();
  return result;
}

bool vuapi CINIFileW::write_struct(
  const std::wstring& section,
  const std::wstring& key,
  void* ptr_struct,
  ulong size
)
{
  this->update_file_path();
  bool result = WritePrivateProfileStructW(section.c_str(), key.c_str(), ptr_struct, size, m_file_path.c_str()) != 0;
  m_last_error_code = GetLastError();
  return result;
}

// Short-Write

bool vuapi CINIFileW::write_integer(const std::wstring& key, int value)
{
  return this->write_integer(m_section.c_str(), key.c_str(), value);
}

bool vuapi CINIFileW::write_bool(const std::wstring& key, bool value)
{
  return this->write_bool(m_section.c_str(), key.c_str(), value);
}

bool vuapi CINIFileW::write_float(const std::wstring& key, float value)
{
  return this->write_float(m_section.c_str(), key.c_str(), value);
}

bool vuapi CINIFileW::write_string(const std::wstring& key, const std::wstring& value)
{
  return this->write_string(m_section.c_str(), key.c_str(), value);
}

bool vuapi CINIFileW::write_struct(const std::wstring& key, void* ptr_struct, ulong size)
{
  return this->write_struct(m_section.c_str(), key.c_str(), ptr_struct, size);
}

} // namespace vu