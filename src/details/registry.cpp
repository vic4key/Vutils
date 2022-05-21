/**
 * @file   registry.cpp
 * @author Vic P.
 * @brief  Implementation for Registry
 */

#include "Vutils.h"
#include "lazy.h"

namespace vu
{

RegistryX::RegistryX() : LastError()
{
  m_hk_root_key = nullptr;
  m_hk_sub_key  = nullptr;
}

RegistryX::~RegistryX()
{
}

HKEY vuapi RegistryX::get_current_key_handle()
{
  return m_hk_sub_key;
}

registry_reflection vuapi RegistryX::query_reflection_key()
{
  BOOL reflected_disabled = FALSE;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return registry_reflection::RR_ERROR;
  }

  if (pfnRegQueryReflectionKey(m_hk_sub_key, &reflected_disabled) != ERROR_SUCCESS)
  {
    return registry_reflection::RR_ERROR;
  }

  if (reflected_disabled == TRUE)
  {
    return registry_reflection::RR_DISABLED;
  }
  else
  {
    return registry_reflection::RR_ENABLED;
  }
}

bool vuapi RegistryX::set_reflection_key(registry_reflection reg_reflection)
{
  bool result = false;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return result;
  }

  switch (reg_reflection)
  {
  case registry_reflection::RR_DISABLE:
    result = (pfnRegDisableReflectionKey(m_hk_sub_key) == ERROR_SUCCESS);
    break;
  case registry_reflection::RR_ENABLE:
    result = (pfnRegEnableReflectionKey(m_hk_sub_key) == ERROR_SUCCESS);
    break;
  default:
    result = false;
    break;
  }

  return result;
}

bool vuapi RegistryX::close_key()
{
  m_last_error_code = RegCloseKey(m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

RegistryA::RegistryA() : RegistryX()
{
  m_sub_key.clear();
}

RegistryA::RegistryA(registry_key reg_root) : RegistryX()
{
  m_hk_root_key = (HKEY)reg_root;
}

RegistryA::RegistryA(registry_key RegRoot, const std::string& sub_key)
{
  m_hk_root_key = (HKEY)RegRoot;
  m_sub_key = sub_key;
}

RegistryA::~RegistryA()
{
}

ulong vuapi RegistryA::set_size_of_multi_string(const char* multi_string)
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

ulong vuapi RegistryA::get_data_size(const std::string& value_name, ulong type)
{
  ulong ulDataSize = 0;

  m_last_error_code = RegQueryValueExA(
    m_hk_sub_key, value_name.c_str(), NULL, &type, NULL, &ulDataSize);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return 0;
  }

  return ulDataSize;
}

bool vuapi RegistryA::create_key()
{
  return this->create_key(m_sub_key);
}

bool vuapi RegistryA::create_key(const std::string& SubKey)
{
  m_last_error_code = RegCreateKeyA(m_hk_root_key, SubKey.c_str(), &m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryA::key_exists()
{
  return this->key_exists(m_sub_key);
}

bool vuapi RegistryA::key_exists(const std::string& sub_key)
{
  m_last_error_code = RegOpenKeyA(m_hk_root_key, sub_key.c_str(), &m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryA::open_key(registry_access reg_access)
{
  return this->open_key(m_sub_key, reg_access);
}

bool vuapi RegistryA::open_key(const std::string& sub_key, registry_access reg_access)
{
  m_last_error_code = RegOpenKeyExA(
    m_hk_root_key, sub_key.c_str(), 0, (REGSAM)reg_access, &m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryA::delete_key()
{
  return this->delete_key(m_sub_key);
}

bool vuapi RegistryA::delete_key(const std::string& sub_key)
{
  if (m_hk_root_key == 0)
  {
    return false;
  }

  m_last_error_code = RegDeleteKeyA(m_hk_root_key, sub_key.c_str());

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryA::delete_value(const std::string& value)
{
  if (m_hk_sub_key == 0)
  {
    return false;
  }

  m_last_error_code = RegDeleteValueA(m_hk_sub_key, value.c_str());

  return (m_last_error_code == ERROR_SUCCESS);
}

std::vector<std::string> vuapi RegistryA::enum_keys()
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

std::vector<std::string> vuapi RegistryA::enum_values()
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
    m_last_error_code = RegEnumValueA(
      m_hk_sub_key, i, pvalue_name, &value_name_length, NULL, NULL, NULL, NULL);
    l.push_back(pvalue_name);
  }

  delete[] pvalue_name;

  return l;
}

// Write

bool vuapi RegistryA::write_integer(const std::string& value_name, int value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExA(
    m_hk_sub_key, value_name.c_str(), 0, REG_DWORD, (const uchar*)&value, sizeof(value));

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryA::write_bool(const std::string& value_name, bool value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExA(
    m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (const uchar*)&value, sizeof(value));

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryA::write_float(const std::string& value_name, float value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExA(
    m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (const uchar*)&value, sizeof(value));

  return (m_last_error_code == ERROR_SUCCESS);
}

// For REG_SZ, REG_MULTI_SZ and REG_EXPAND_SZ:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724884(v=vs.85).aspx

bool vuapi RegistryA::write_string(const std::string& value_name, const std::string& Value)
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

bool vuapi RegistryA::write_multi_string(const std::string& value_name, const char* ps_value)
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

bool vuapi RegistryA::write_multi_string(
  const std::string& value_name, const std::vector<std::string>& value)
{
  auto p = list_to_multi_string_A(value);
  return this->write_multi_string(value_name, p.get());
}

bool vuapi RegistryA::write_expand_string(const std::string& value_name, const std::string& value)
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

bool vuapi RegistryA::write_binary(const std::string& value_name, void* ptr_data, ulong size)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExA(
    m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (const uchar*)ptr_data, size);

  return (m_last_error_code == ERROR_SUCCESS);
}

// Read

int vuapi RegistryA::read_integer(const std::string& value_name, int default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  int result = 0;
  ulong type = REG_DWORD, ret = sizeof(result);

  m_last_error_code = RegQueryValueExA(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

bool vuapi RegistryA::read_bool(const std::string& value_name, bool default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  bool result = false;
  ulong type = REG_BINARY, ret = sizeof(result);

  m_last_error_code = RegQueryValueExA(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

float vuapi RegistryA::read_float(const std::string& value_name, float default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  float result = 0;
  ulong type = REG_BINARY, ret = sizeof(result);

  m_last_error_code = RegQueryValueExA(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

std::string vuapi RegistryA::read_string(
  const std::string& value_name, const std::string& default_value)
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

  m_last_error_code = RegQueryValueExA(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::string s(p.get());

  return s;
}

std::vector<std::string> vuapi RegistryA::read_multi_string(
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

  m_last_error_code = RegQueryValueExA(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::vector<std::string> l = multi_string_to_list_A(p.get());

  return l;
}

std::string vuapi RegistryA::read_expand_string(
  const std::string& value_name, const std::string& default_value)
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

  m_last_error_code = RegQueryValueExA(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::string s(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi RegistryA::read_binary(
  const std::string& value_name, const void* pDefault)
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

RegistryW::RegistryW() : RegistryX()
{
  m_sub_key.clear();
}

RegistryW::RegistryW(registry_key RegRoot) : RegistryX()
{
  m_hk_root_key = (HKEY)RegRoot;
}

RegistryW::RegistryW(registry_key RegRoot, const std::wstring& sub_key) : RegistryX()
{
  m_hk_root_key = (HKEY)RegRoot;
  m_sub_key = sub_key;
}

RegistryW::~RegistryW()
{
}

ulong vuapi RegistryW::set_size_of_multi_string(const wchar* multi_string)
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

ulong vuapi RegistryW::get_data_size(const std::wstring& value_name, ulong type)
{
  ulong data_size = 0;

  m_last_error_code = RegQueryValueExW(
    m_hk_sub_key, value_name.c_str(), NULL, &type, NULL, &data_size);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return 0;
  }

  return data_size;
}

bool vuapi RegistryW::create_key()
{
  return this->create_key(m_sub_key);
}

bool vuapi RegistryW::create_key(const std::wstring& sub_key)
{
  m_last_error_code = RegCreateKeyW(m_hk_root_key, sub_key.c_str(), &m_hk_root_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryW::key_exists()
{
  return this->key_exists(m_sub_key);
}

bool vuapi RegistryW::key_exists(const std::wstring& sub_key)
{
  m_last_error_code = RegOpenKeyW(m_hk_root_key, sub_key.c_str(), &m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryW::open_key(registry_access reg_access)
{
  return this->open_key(m_sub_key, reg_access);
}

bool vuapi RegistryW::open_key(const std::wstring& sub_key, registry_access reg_access)
{
  m_last_error_code = RegOpenKeyExW(
    m_hk_root_key, sub_key.c_str(), 0, (REGSAM)reg_access, &m_hk_sub_key);
  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryW::delete_key()
{
  return this->delete_key(m_sub_key);
}

bool vuapi RegistryW::delete_key(const std::wstring& sub_key)
{
  if (m_hk_root_key == 0)
  {
    return false;
  }

  m_last_error_code = RegDeleteKeyW(m_hk_root_key, sub_key.c_str());

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryW::delete_value(const std::wstring& value)
{
  if (m_hk_sub_key == 0)
  {
    return false;
  }

  m_last_error_code = RegDeleteValueW(m_hk_sub_key, value.c_str());

  return (m_last_error_code == ERROR_SUCCESS);
}

std::vector<std::wstring> vuapi RegistryW::enum_keys()
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
    m_last_error_code = RegEnumKeyW(
      m_hk_sub_key, i, ps_sub_key_name, sizeof(wchar)*cb_max_sub_key_length);
    l.push_back(ps_sub_key_name);
  }

  delete[] ps_sub_key_name;

  return l;
}

std::vector<std::wstring> vuapi RegistryW::enum_values()
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
    m_last_error_code = RegEnumValueW(
      m_hk_sub_key, i, pvalue_name, &value_name_length, NULL, NULL, NULL, NULL);
    l.push_back(pvalue_name);
  }

  delete[] pvalue_name;

  return l;
}

// Write

bool vuapi RegistryW::write_integer(const std::wstring& value_name, int value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExW(
    m_hk_sub_key, value_name.c_str(), 0, REG_DWORD, (const uchar *)&value, sizeof(value));

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryW::write_bool(const std::wstring& value_name, bool value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExW(
    m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (const uchar *)&value, sizeof(value));

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryW::write_float(const std::wstring& value_name, float Value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExW(m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (
    const uchar *)&Value, sizeof(Value));

  return (m_last_error_code == ERROR_SUCCESS);
}

// For REG_SZ, REG_MULTI_SZ and REG_EXPAND_SZ:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724884(v=vs.85).aspx

bool vuapi RegistryW::write_string(const std::wstring& value_name, const std::wstring& value)
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
    (const uchar *)value.c_str(),
    sizeof(wchar)*((ulong)value.length() + 1)
  );

  return (m_last_error_code == ERROR_SUCCESS);
}

bool vuapi RegistryW::write_multi_string(const std::wstring& value_name, const wchar* ps_value)
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

bool vuapi RegistryW::write_multi_string(
  const std::wstring& value_name, const std::vector<std::wstring> value)
{
  auto p = list_to_multi_string_W(value);
  return this->write_multi_string(value_name, p.get());
}

bool vuapi RegistryW::write_expand_string(const std::wstring& value_name, const std::wstring& value)
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

bool vuapi RegistryW::write_binary(const std::wstring& value_name, void* ptr_data, ulong size)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return false;
  }

  m_last_error_code = RegSetValueExW(
    m_hk_sub_key, value_name.c_str(), 0, REG_BINARY, (const uchar *)ptr_data, size);

  return (m_last_error_code == ERROR_SUCCESS);
}

// Read

int vuapi RegistryW::read_integer(const std::wstring& value_name, int default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  int result = 0;
  ulong type = REG_DWORD, ret = sizeof(result);

  m_last_error_code = RegQueryValueExW(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

bool vuapi RegistryW::read_bool(const std::wstring& value_name, bool default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  bool result = false;
  ulong type = REG_BINARY, ret = sizeof(result);

  m_last_error_code = RegQueryValueExW(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

float vuapi RegistryW::read_float(const std::wstring& value_name, float default_value)
{
  if (m_hk_sub_key == 0)
  {
    m_last_error_code = ERROR_INVALID_HANDLE;
    SetLastError(m_last_error_code);
    return default_value;
  }

  float result = 0;
  ulong type = REG_BINARY, ret = sizeof(result);

  m_last_error_code = RegQueryValueExW(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)&result, &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  return result;
}

std::wstring vuapi RegistryW::read_string(
  const std::wstring& value_name, const std::wstring& default_value)
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

  m_last_error_code = RegQueryValueExW(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::wstring s(p.get());

  return s;
}

std::vector<std::wstring> vuapi RegistryW::read_multi_string(
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

  m_last_error_code = RegQueryValueExW(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::vector<std::wstring> l = multi_string_to_list_W(p.get());

  return l;
}

std::wstring vuapi RegistryW::read_expand_string(
  const std::wstring& value_name, const std::wstring& default_value)
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

  m_last_error_code = RegQueryValueExW(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return default_value;
  }

  std::wstring s(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi RegistryW::read_binary(
  const std::wstring& value_name, const void* default_value)
{
  std::unique_ptr<uchar[]> pDef((uchar*)default_value);

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

  m_last_error_code = RegQueryValueExW(
    m_hk_sub_key, value_name.c_str(), NULL, &type, (uchar*)p.get(), &ret);
  if (m_last_error_code != ERROR_SUCCESS)
  {
    return pDef;
  }

  return p;
}

} // namespace vu