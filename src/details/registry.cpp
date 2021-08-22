/**
 * @file   registry.cpp
 * @author Vic P.
 * @brief  Implementation for Registry
 */

#include "Vutils.h"
#include "lazy.h"

namespace vu
{

CRegistryX::CRegistryX() : CLastError()
{
  m_hk_root_key = nullptr;
  m_hk_sub_key  = nullptr;
}

CRegistryX::~CRegistryX()
{
}

HKEY vuapi CRegistryX::get_current_key_handle()
{
  return m_hk_sub_key;
}

eRegReflection vuapi CRegistryX::query_reflection_key()
{
  BOOL bReflectedDisabled = FALSE;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return eRegReflection::RR_ERROR;
  }

  if (pfnRegQueryReflectionKey(m_hk_sub_key, &bReflectedDisabled) != ERROR_SUCCESS)
  {
    return eRegReflection::RR_ERROR;
  }

  if (bReflectedDisabled == TRUE)
  {
    return eRegReflection::RR_DISABLED;
  }
  else
  {
    return eRegReflection::RR_ENABLED;
  }
}

bool vuapi CRegistryX::set_reflection_key(eRegReflection RegReflection)
{
  bool result = false;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return result;
  }

  switch (RegReflection)
  {
  case eRegReflection::RR_DISABLE:
    result = (pfnRegDisableReflectionKey(m_hk_sub_key) == ERROR_SUCCESS);
    break;
  case eRegReflection::RR_ENABLE:
    result = (pfnRegEnableReflectionKey(m_hk_sub_key) == ERROR_SUCCESS);
    break;
  default:
    result = false;
    break;
  }

  return result;
}

bool vuapi CRegistryX::close_key()
{
  m_last_error_code = RegCloseKey(m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

CRegistryA::CRegistryA() : CRegistryX()
{
  m_sub_key.clear();
}

CRegistryA::CRegistryA(eRegRoot RegRoot) : CRegistryX()
{
  m_hk_root_key = (HKEY)RegRoot;
}

CRegistryA::CRegistryA(eRegRoot RegRoot, const std::string& SubKey)
{
  m_hk_root_key = (HKEY)RegRoot;
  m_sub_key = SubKey;
}

CRegistryA::~CRegistryA()
{
}

ulong vuapi CRegistryA::set_size_of_multi_string(const char* multi_string)
{
  ulong ulLength = 0;

  while (*multi_string)
  {
    ulong crLength = lstrlenA(multi_string) + sizeof(char);
    ulLength += crLength;
    multi_string = multi_string + crLength;
  }

  return ulLength;
}

ulong vuapi CRegistryA::get_data_size(const std::string& value_name, ulong type)
{
  ulong ulDataSize = 0;

  m_last_error_code = RegQueryValueExA(m_hk_sub_key, value_name.c_str(), NULL, &type, NULL, &ulDataSize);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return 0;
  }

  return ulDataSize;
}

bool vuapi CRegistryA::create_key()
{
  return this->create_key(m_sub_key);
}

bool vuapi CRegistryA::create_key(const std::string& SubKey)
{
  m_last_error_code = RegCreateKeyA(m_hk_root_key, SubKey.c_str(), &m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryA::key_exists()
{
  return this->key_exists(m_sub_key);
}

bool vuapi CRegistryA::key_exists(const std::string& sub_key)
{
  m_last_error_code = RegOpenKeyA(m_hk_root_key, sub_key.c_str(), &m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryA::open_key(eRegAccess reg_access)
{
  return this->open_key(m_sub_key, reg_access);
}

bool vuapi CRegistryA::open_key(const std::string& sub_key, eRegAccess reg_access)
{
  m_last_error_code = RegOpenKeyExA(m_hk_root_key, sub_key.c_str(), 0, (REGSAM)reg_access, &m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryA::delete_key()
{
  return this->delete_key(m_sub_key);
}

bool vuapi CRegistryA::delete_key(const std::string& sub_key)
{
  if (m_hk_root_key == 0)
  {
    return false;
  }

  m_last_error_code = RegDeleteKeyA(m_hk_root_key, sub_key.c_str());

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryA::delete_value(const std::string& value)
{
  if (m_hk_sub_key == 0)
  {
    return false;
  }

  m_last_error_code = RegDeleteValueA(m_hk_sub_key, value.c_str());

  return (m_last_error_code == ERROR_SUCCESS);
}

std::vector<std::string> vuapi CRegistryA::enum_keys()
{
  std::vector<std::string> l;
  l.clear();

  char s_class[MAXPATH] = { 0 };
  ulong cch_class = MAXPATH;
  ulong c_sub_keys = 0;
  ulong cb_max_sub_key_length = 0;
  ulong cb_max_class_length = 0;
  ulong c_values = 0;
  ulong cb_max_value_name_length = 0;
  ulong cb_max_value_length = 0;
  ulong cb_security_descriptor = 0;
  FILETIME last_write_time = { 0 };

  ZeroMemory(&s_class, sizeof(s_class));
  ZeroMemory(&last_write_time, sizeof(last_write_time));

  m_last_error_code = RegQueryInfoKeyA(
    m_hk_sub_key,
    (char*)&s_class,
    &cch_class,
    NULL,
    &c_sub_keys,
    &cb_max_sub_key_length,
    &cb_max_class_length,
    &c_values,
    &cb_max_value_name_length,
    &cb_max_value_length,
    &cb_security_descriptor,
    &last_write_time
  );

  if (m_last_error_code != ERROR_SUCCESS || c_sub_keys == 0) return l;

  cb_max_sub_key_length += 1;

  char * ps_sub_key_name = new char[cb_max_sub_key_length];

  for (ulong i = 0; i < c_sub_keys; i++)
  {
    ZeroMemory(ps_sub_key_name, cb_max_sub_key_length);
    m_last_error_code = RegEnumKeyA(m_hk_sub_key, i, ps_sub_key_name, cb_max_sub_key_length);
    l.push_back(ps_sub_key_name);
  }

  delete[] ps_sub_key_name;

  return l;
}

std::vector<std::string> vuapi CRegistryA::enum_values()
{
  std::vector<std::string> l;
  l.clear();

  char s_class[MAXPATH] = { 0 };
  ulong cch_class = MAXPATH;
  ulong c_sub_keys = 0;
  ulong cb_max_sub_key_length = 0;
  ulong cb_max_class_length = 0;
  ulong c_values = 0;
  ulong cb_max_value_name_length = 0;
  ulong cb_max_value_length = 0;
  ulong cb_security_descriptor = 0;
  FILETIME last_write_time = { 0 };

  ZeroMemory(&s_class, sizeof(s_class));
  ZeroMemory(&last_write_time, sizeof(last_write_time));

  m_last_error_code = RegQueryInfoKeyA(
    m_hk_sub_key,
    (char*)&s_class,
    &cch_class,
    NULL,
    &c_sub_keys,
    &cb_max_sub_key_length,
    &cb_max_class_length,
    &c_values,
    &cb_max_value_name_length,
    &cb_max_value_length,
    &cb_security_descriptor,
    &last_write_time
  );

  if (m_last_error_code != ERROR_SUCCESS || c_values == 0) return l;

  cb_max_value_name_length += 1;

  char * pvalue_name = new char[cb_max_value_name_length];
  ulong value_name_length;
  for (ulong i = 0; i < c_values; i++)
  {
    value_name_length = cb_max_value_name_length;
    ZeroMemory(pvalue_name, cb_max_value_name_length);
    m_last_error_code = RegEnumValueA(m_hk_sub_key, i, pvalue_name, &value_name_length, NULL, NULL, NULL, NULL);
    l.push_back(pvalue_name);
  }

  delete[] pvalue_name;

  return l;
}

// Write

bool vuapi CRegistryA::write_integer(const std::string& value_name, int Value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExA(m_hk_sub_key, value_name.c_str(), 0, REG_DWORD, (const uchar*)&Value, sizeof(Value));

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryA::write_bool(const std::string& value_name, bool Value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExA(m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (const uchar*)&Value, sizeof(Value));

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryA::write_float(const std::string& value_name, float Value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExA(m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (const uchar*)&Value, sizeof(Value));

  return (m_last_error_code == ERROR_SUCCESS);
}

// For REG_SZ, REG_MULTI_SZ and REG_EXPAND_SZ:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724884(v=vs.85).aspx

bool vuapi CRegistryA::write_string(const std::string& value_name, const std::string& Value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExA(
    m_hk_sub_key,
    value_name.c_str(),
    0,
    REG_SZ,
    (const uchar*)Value.c_str(),
    (ulong)Value.length() + sizeof(char)
  );

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryA::write_multi_string(const std::string& value_name, const char* ps_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExA(
    m_hk_sub_key,
    value_name.c_str(),
    0,
    REG_MULTI_SZ,
    (const uchar*)ps_value,
    this->set_size_of_multi_string(ps_value) + sizeof(char)
  );

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryA::write_multi_string(const std::string& value_name, const std::vector<std::string>& value)
{
  auto p = list_to_multi_string_A(value);
  return this->write_multi_string(value_name, p.get());
}

bool vuapi CRegistryA::write_expand_string(const std::string& value_name, const std::string& value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExA(
    m_hk_sub_key,
    value_name.c_str(),
    0,
    REG_EXPAND_SZ,
    (const uchar*)value.c_str(),
    (ulong)value.length() + sizeof(char)
  );

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryA::write_binary(const std::string& value_name, void* lpData, ulong ulSize)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExA(m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (const uchar*)lpData, ulSize);

  return (m_last_error_code == ERROR_SUCCESS);
}

// Read

int vuapi CRegistryA::read_integer(const std::string& value_name, int default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  int result = 0;
  ulong type = REG_DWORD, ret = sizeof(result);

  m_last_error_code = RegQueryValueExA(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

bool vuapi CRegistryA::read_bool(const std::string& value_name, bool default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  bool result = false;
  ulong type = REG_BINARY, ret = sizeof(result);

  m_last_error_code = RegQueryValueExA(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

float vuapi CRegistryA::read_float(const std::string& value_name, float default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  float result = 0;
  ulong type = REG_BINARY, ret = sizeof(result);

  m_last_error_code = RegQueryValueExA(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

std::string vuapi CRegistryA::read_string(const std::string& value_name, const std::string& default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  ulong type = REG_SZ, ret = this->get_data_size(value_name, type);
  if (ret == 0)
  {
    return default_value;
  }

  ret += sizeof(char);

  std::unique_ptr<char[]> p(new char [ret]);
  if (p == nullptr)
  {
    return default_value;
  }

  ZeroMemory(p.get(), ret);

  m_last_error_code = RegQueryValueExA(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::string s(p.get());

  return s;
}

std::vector<std::string> vuapi CRegistryA::read_multi_string(
  const std::string& value_name,
  std::vector<std::string> default_value
)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  ulong type = REG_MULTI_SZ, ret = this->get_data_size(value_name, type);
  if (ret == 0)
  {
    return default_value;
  }

  ret += sizeof(char);

  std::unique_ptr<char[]> p(new char [ret]);
  if (p == nullptr)
  {
    return default_value;
  }

  ZeroMemory(p.get(), ret);

  m_last_error_code = RegQueryValueExA(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::vector<std::string> l = multi_string_to_list_A(p.get());

  return l;
}

std::string vuapi CRegistryA::read_expand_string(const std::string& value_name, const std::string& default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  ulong type = REG_EXPAND_SZ, ret = this->get_data_size(value_name, type);
  if (ret == 0)
  {
    return default_value;
  }

  ret += sizeof(char);

  std::unique_ptr<char[]> p(new char [ret]);
  if (p == nullptr)
  {
    return default_value;
  }

  ZeroMemory(p.get(), ret);

  m_last_error_code = RegQueryValueExA(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::string s(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi CRegistryA::read_binary(const std::string& value_name, const void* pDefault)
{
  std::unique_ptr<uchar[]> pDef((uchar*)pDefault);

  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return pDef;
  }

  ulong type = REG_BINARY, ret = this->get_data_size(value_name, type);
  if (ret == 0)
  {
    m_last_error_code = ERROR_INCORRECT_SIZE;
    SetLastError(m_last_error_code);
    return pDef;
  }

  ret += 1;

  std::unique_ptr<uchar[]> p(new uchar [ret]);
  if (p == nullptr)
  {
    return pDef;
  }

  ZeroMemory((void*)p.get(), ret);

  m_last_error_code = RegQueryValueExA(m_hk_sub_key, value_name.c_str(), NULL, &type, p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return pDef;
  }

  return p;
}

CRegistryW::CRegistryW() : CRegistryX()
{
  m_sub_key.clear();
}

CRegistryW::CRegistryW(eRegRoot RegRoot) : CRegistryX()
{
  m_hk_root_key = (HKEY)RegRoot;
}

CRegistryW::CRegistryW(eRegRoot RegRoot, const std::wstring& sub_key) : CRegistryX()
{
  m_hk_root_key = (HKEY)RegRoot;
  m_sub_key = sub_key;
}

CRegistryW::~CRegistryW()
{
}

ulong vuapi CRegistryW::set_size_of_multi_string(const wchar* multi_string)
{
  ulong ulLength = 0;

  while (*multi_string)
  {
    ulong crLength = sizeof(wchar)*(lstrlenW(multi_string) + 1);
    ulLength += crLength;
    multi_string = multi_string + crLength / 2;
  }

  return ulLength;
}

ulong vuapi CRegistryW::get_data_size(const std::wstring& value_name, ulong type)
{
  ulong ulDataSize = 0;

  m_last_error_code = RegQueryValueExW(m_hk_sub_key, value_name.c_str(), NULL, &type, NULL, &ulDataSize);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return 0;
  }

  return ulDataSize;
}

bool vuapi CRegistryW::create_key()
{
  return this->create_key(m_sub_key);
}

bool vuapi CRegistryW::create_key(const std::wstring& sub_key)
{
  m_last_error_code = RegCreateKeyW(m_hk_root_key, sub_key.c_str(), &m_hk_root_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryW::key_exists()
{
  return this->key_exists(m_sub_key);
}

bool vuapi CRegistryW::key_exists(const std::wstring& sub_key)
{
  m_last_error_code = RegOpenKeyW(m_hk_root_key, sub_key.c_str(), &m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryW::open_key(eRegAccess reg_access)
{
  return this->open_key(m_sub_key, reg_access);
}

bool vuapi CRegistryW::open_key(const std::wstring& sub_key, eRegAccess reg_access)
{
  m_last_error_code = RegOpenKeyExW(m_hk_root_key, sub_key.c_str(), 0, (REGSAM)reg_access, &m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryW::delete_key()
{
  return this->delete_key(m_sub_key);
}

bool vuapi CRegistryW::delete_key(const std::wstring& sub_key)
{
  if (m_hk_root_key == 0)
  {
    return false;
  }

  m_last_error_code = RegDeleteKeyW(m_hk_root_key, sub_key.c_str());

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryW::delete_value(const std::wstring& value)
{
  if (m_hk_sub_key == 0)
  {
    return false;
  }

  m_last_error_code = RegDeleteValueW(m_hk_sub_key, value.c_str());

  return (m_last_error_code == ERROR_SUCCESS);
}

std::vector<std::wstring> vuapi CRegistryW::enum_keys()
{
  std::vector<std::wstring> l;
  l.clear();

  wchar s_class[MAXPATH] = { 0 };
  ulong cch_class = MAXPATH;
  ulong c_sub_keys = 0;
  ulong cb_max_sub_key_length = 0;
  ulong cb_max_class_length = 0;
  ulong c_values = 0;
  ulong cb_max_value_name_length = 0;
  ulong cb_max_value_length = 0;
  ulong cb_security_descriptor = 0;
  FILETIME last_write_time = { 0 };

  ZeroMemory(&s_class, sizeof(s_class));
  ZeroMemory(&last_write_time, sizeof(last_write_time));

  m_last_error_code = RegQueryInfoKeyW(
    m_hk_sub_key,
    (wchar*)&s_class,
    &cch_class,
    NULL,
    &c_sub_keys,
    &cb_max_sub_key_length,
    &cb_max_class_length,
    &c_values,
    &cb_max_value_name_length,
    &cb_max_value_length,
    &cb_security_descriptor,
    &last_write_time
  );

  if (m_last_error_code != ERROR_SUCCESS || c_sub_keys == 0) return l;

  cb_max_sub_key_length += 1;

  wchar * ps_sub_key_name = new wchar[cb_max_sub_key_length];

  for (ulong i = 0; i < c_sub_keys; i++)
  {
    ZeroMemory(ps_sub_key_name, sizeof(wchar)*cb_max_sub_key_length);
    m_last_error_code = RegEnumKeyW(m_hk_sub_key, i, ps_sub_key_name, sizeof(wchar)*cb_max_sub_key_length);
    l.push_back(ps_sub_key_name);
  }

  delete[] ps_sub_key_name;

  return l;
}

std::vector<std::wstring> vuapi CRegistryW::enum_values()
{
  std::vector<std::wstring> l;
  l.clear();

  wchar s_class[MAXPATH] = { 0 };
  ulong cch_class = MAXPATH;
  ulong c_sub_keys = 0;
  ulong cb_max_sub_key_length = 0;
  ulong cb_max_class_length = 0;
  ulong c_values = 0;
  ulong cb_max_value_name_length = 0;
  ulong cb_max_value_length = 0;
  ulong cb_security_descriptor = 0;
  FILETIME last_write_time = { 0 };

  ZeroMemory(&s_class, sizeof(s_class));
  ZeroMemory(&last_write_time, sizeof(last_write_time));

  m_last_error_code = RegQueryInfoKeyA(
    m_hk_sub_key,
    (char*)&s_class,
    &cch_class,
    NULL,
    &c_sub_keys,
    &cb_max_sub_key_length,
    &cb_max_class_length,
    &c_values,
    &cb_max_value_name_length,
    &cb_max_value_length,
    &cb_security_descriptor,
    &last_write_time
  );

  if (m_last_error_code != ERROR_SUCCESS || c_values == 0) return l;

  cb_max_value_name_length += 1;

  wchar* pvalue_name = new wchar[cb_max_value_name_length];
  ulong value_name_length;
  for (ulong i = 0; i < c_values; i++)
  {
    value_name_length = sizeof(wchar)*cb_max_value_name_length;
    ZeroMemory(pvalue_name, cb_max_value_name_length);
    m_last_error_code = RegEnumValueW(m_hk_sub_key, i, pvalue_name, &value_name_length, NULL, NULL, NULL, NULL);
    l.push_back(pvalue_name);
  }

  delete[] pvalue_name;

  return l;
}

// Write

bool vuapi CRegistryW::write_integer(const std::wstring& value_name, int Value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExW(m_hk_sub_key, value_name.c_str(), 0, REG_DWORD, (const uchar *)&Value, sizeof(Value));

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryW::write_bool(const std::wstring& value_name, bool Value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExW(m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (const uchar *)&Value, sizeof(Value));

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryW::write_float(const std::wstring& value_name, float Value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExW(m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (const uchar *)&Value, sizeof(Value));

  return (m_last_error_code == ERROR_SUCCESS);
}

// For REG_SZ, REG_MULTI_SZ and REG_EXPAND_SZ:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724884(v=vs.85).aspx

bool vuapi CRegistryW::write_string(const std::wstring& value_name, const std::wstring& Value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExW(
    m_hk_sub_key,
    value_name.c_str(),
    0,
    REG_SZ,
    (const uchar *)Value.c_str(),
    sizeof(wchar)*((ulong)Value.length() + 1)
  );

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryW::write_multi_string(const std::wstring& value_name, const wchar* ps_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExW(
    m_hk_sub_key,
    value_name.c_str(),
    0,
    REG_MULTI_SZ,
    (const uchar *)ps_value,
    this->set_size_of_multi_string(ps_value) + sizeof(wchar)
  );

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryW::write_multi_string(const std::wstring& value_name, const std::vector<std::wstring> value)
{
  auto p = list_to_multi_string_W(value);
  return this->write_multi_string(value_name, p.get());
}

bool vuapi CRegistryW::write_expand_string(const std::wstring& value_name, const std::wstring& value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExW(
    m_hk_sub_key,
    value_name.c_str(),
    0,
    REG_EXPAND_SZ,
    (const uchar *)value.c_str(),
    sizeof(wchar)*((ulong)value.length() + 1)
  );

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi CRegistryW::write_binary(const std::wstring& value_name, void* lpData, ulong size)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExW(m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (const uchar *)lpData, size);

  return (m_last_error_code == ERROR_SUCCESS);
}

// Read

int vuapi CRegistryW::read_integer(const std::wstring& value_name, int default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  int result = 0;
  ulong type = REG_DWORD, ret = sizeof(result);

  m_last_error_code = RegQueryValueExW(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

bool vuapi CRegistryW::read_bool(const std::wstring& value_name, bool default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  bool result = false;
  ulong type = REG_BINARY, ret = sizeof(result);

  m_last_error_code = RegQueryValueExW(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

float vuapi CRegistryW::read_float(const std::wstring& value_name, float default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  float result = 0;
  ulong type = REG_BINARY, ret = sizeof(result);

  m_last_error_code = RegQueryValueExW(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

std::wstring vuapi CRegistryW::read_string(const std::wstring& value_name, const std::wstring& default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  ulong type = REG_SZ, ret = this->get_data_size(value_name, type);
  if (ret == 0)
  {
    return default_value;
  }

  ret += sizeof(wchar);

  std::unique_ptr<wchar[]> p(new wchar [ret / sizeof(wchar)]);
  if (p == nullptr)
  {
    return default_value;
  }

  ZeroMemory(p.get(), ret);

  m_last_error_code = RegQueryValueExW(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::wstring s(p.get());

  return s;
}

std::vector<std::wstring> vuapi CRegistryW::read_multi_string(
  const std::wstring& value_name,
  std::vector<std::wstring> default_value
)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  ulong type = REG_MULTI_SZ, ret = this->get_data_size(value_name, type);
  if (ret == 0)
  {
    return default_value;
  }

  ret += sizeof(wchar);

  std::unique_ptr<wchar[]> p(new wchar [ret / sizeof(wchar)]);
  if (p == nullptr)
  {
    return default_value;
  }

  ZeroMemory(p.get(), ret);

  m_last_error_code = RegQueryValueExW(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::vector<std::wstring> l = multi_string_to_list_W(p.get());

  return l;
}

std::wstring vuapi CRegistryW::read_expand_string(const std::wstring& value_name, const std::wstring& default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  ulong type = REG_EXPAND_SZ, ret = this->get_data_size(value_name, type);
  if (ret == 0)
  {
    return default_value;
  }

  ret += sizeof(wchar);

  std::unique_ptr<wchar[]> p(new wchar [ret / 2]);
  if (p == nullptr)
  {
    return default_value;
  }

  ZeroMemory(p.get(), ret);

  m_last_error_code = RegQueryValueExW(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::wstring s(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi CRegistryW::read_binary(const std::wstring& value_name, const void* pDefault)
{
  std::unique_ptr<uchar[]> pDef((uchar*)pDefault);

  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return pDef;
  }

  ulong type = REG_BINARY, ret = this->get_data_size(value_name, type);
  if (ret == 0)
  {
    m_last_error_code = ERROR_INCORRECT_SIZE;
    SetLastError(m_last_error_code);
    return pDef;
  }

  ret += sizeof(tchar); // padding for safety

  std::unique_ptr<uchar[]> p(new uchar [ret]);
  if (p == nullptr)
  {
    return pDef;
  }

  ZeroMemory(p.get(), ret);

  m_last_error_code = RegQueryValueExW(m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return pDef;
  }

  return p;
}

} // namespace vu