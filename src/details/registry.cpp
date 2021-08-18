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
  m_HKRootKey = nullptr;
  m_HKSubKey  = nullptr;
}

CRegistryX::~CRegistryX()
{
}

HKEY vuapi CRegistryX::GetCurrentKeyHandle()
{
  return m_HKSubKey;
}

eRegReflection vuapi CRegistryX::QueryReflectionKey()
{
  BOOL bReflectedDisabled = FALSE;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return eRegReflection::RR_ERROR;
  }

  if (pfnRegQueryReflectionKey(m_HKSubKey, &bReflectedDisabled) != ERROR_SUCCESS)
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

bool vuapi CRegistryX::SetReflectionKey(eRegReflection RegReflection)
{
  bool result = false;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return result;
  }

  switch (RegReflection)
  {
  case eRegReflection::RR_DISABLE:
    result = (pfnRegDisableReflectionKey(m_HKSubKey) == ERROR_SUCCESS);
    break;
  case eRegReflection::RR_ENABLE:
    result = (pfnRegEnableReflectionKey(m_HKSubKey) == ERROR_SUCCESS);
    break;
  default:
    result = false;
    break;
  }

  return result;
}

bool vuapi CRegistryX::CloseKey()
{
  m_LastErrorCode = RegCloseKey(m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

CRegistryA::CRegistryA() : CRegistryX()
{
  m_SubKey.clear();
}

CRegistryA::CRegistryA(eRegRoot RegRoot) : CRegistryX()
{
  m_HKRootKey = (HKEY)RegRoot;
}

CRegistryA::CRegistryA(eRegRoot RegRoot, const std::string& SubKey)
{
  m_HKRootKey = (HKEY)RegRoot;
  m_SubKey = SubKey;
}

CRegistryA::~CRegistryA()
{
}

ulong vuapi CRegistryA::GetSizeOfMultiString(const char* lpcszMultiString)
{
  ulong ulLength = 0;

  while (*lpcszMultiString)
  {
    ulong crLength = lstrlenA(lpcszMultiString) + sizeof(char);
    ulLength += crLength;
    lpcszMultiString = lpcszMultiString + crLength;
  }

  return ulLength;
}

ulong vuapi CRegistryA::GetDataSize(const std::string& ValueName, ulong ulType)
{
  ulong ulDataSize = 0;

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, NULL, &ulDataSize);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return 0;
  }

  return ulDataSize;
}

bool vuapi CRegistryA::CreateKey()
{
  return this->CreateKey(m_SubKey);
}

bool vuapi CRegistryA::CreateKey(const std::string& SubKey)
{
  m_LastErrorCode = RegCreateKeyA(m_HKRootKey, SubKey.c_str(), &m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::KeyExists()
{
  return this->KeyExists(m_SubKey);
}

bool vuapi CRegistryA::KeyExists(const std::string& SubKey)
{
  m_LastErrorCode = RegOpenKeyA(m_HKRootKey, SubKey.c_str(), &m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::OpenKey(eRegAccess RegAccess)
{
  return this->OpenKey(m_SubKey, RegAccess);
}

bool vuapi CRegistryA::OpenKey(const std::string& SubKey, eRegAccess RegAccess)
{
  m_LastErrorCode = RegOpenKeyExA(m_HKRootKey, SubKey.c_str(), 0, (REGSAM)RegAccess, &m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::DeleteKey()
{
  return this->DeleteKey(m_SubKey);
}

bool vuapi CRegistryA::DeleteKey(const std::string& SubKey)
{
  if (m_HKRootKey == 0)
  {
    return false;
  }

  m_LastErrorCode = RegDeleteKeyA(m_HKRootKey, SubKey.c_str());

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::DeleteValue(const std::string& Value)
{
  if (m_HKSubKey == 0)
  {
    return false;
  }

  m_LastErrorCode = RegDeleteValueA(m_HKSubKey, Value.c_str());

  return (m_LastErrorCode == ERROR_SUCCESS);
}

std::vector<std::string> vuapi CRegistryA::EnumKeys()
{
  std::vector<std::string> l;
  l.clear();

  char Class[MAXPATH] = { 0 };
  ulong cchClass = MAXPATH;
  ulong cSubKeys = 0;
  ulong cbMaxSubKeyLen = 0;
  ulong cbMaxClassLen = 0;
  ulong cValues = 0;
  ulong cbMaxValueNameLen = 0;
  ulong cbMaxValueLen = 0;
  ulong cbSecurityDescriptor = 0;
  FILETIME ftLastWriteTime = { 0 };

  ZeroMemory(&Class, sizeof(Class));
  ZeroMemory(&ftLastWriteTime, sizeof(ftLastWriteTime));

  m_LastErrorCode = RegQueryInfoKeyA(
    m_HKSubKey,
    (char*)&Class,
    &cchClass,
    NULL,
    &cSubKeys,
    &cbMaxSubKeyLen,
    &cbMaxClassLen,
    &cValues,
    &cbMaxValueNameLen,
    &cbMaxValueLen,
    &cbSecurityDescriptor,
    &ftLastWriteTime
  );

  if (m_LastErrorCode != ERROR_SUCCESS || cSubKeys == 0) return l;

  cbMaxSubKeyLen += 1;

  char * pSubKeyName = new char[cbMaxSubKeyLen];

  for (ulong i = 0; i < cSubKeys; i++)
  {
    ZeroMemory(pSubKeyName, cbMaxSubKeyLen);
    m_LastErrorCode = RegEnumKeyA(m_HKSubKey, i, pSubKeyName, cbMaxSubKeyLen);
    l.push_back(pSubKeyName);
  }

  delete[] pSubKeyName;

  return l;
}

std::vector<std::string> vuapi CRegistryA::EnumValues()
{
  std::vector<std::string> l;
  l.clear();

  char Class[MAXPATH] = { 0 };
  ulong cchClass = MAXPATH;
  ulong cSubKeys = 0;
  ulong cbMaxSubKeyLen = 0;
  ulong cbMaxClassLen = 0;
  ulong cValues = 0;
  ulong cbMaxValueNameLen = 0;
  ulong cbMaxValueLen = 0;
  ulong cbSecurityDescriptor = 0;
  FILETIME ftLastWriteTime = { 0 };

  ZeroMemory(&Class, sizeof(Class));
  ZeroMemory(&ftLastWriteTime, sizeof(ftLastWriteTime));

  m_LastErrorCode = RegQueryInfoKeyA(
    m_HKSubKey,
    (char*)&Class,
    &cchClass,
    NULL,
    &cSubKeys,
    &cbMaxSubKeyLen,
    &cbMaxClassLen,
    &cValues,
    &cbMaxValueNameLen,
    &cbMaxValueLen,
    &cbSecurityDescriptor,
    &ftLastWriteTime
  );

  if (m_LastErrorCode != ERROR_SUCCESS || cValues == 0) return l;

  cbMaxValueNameLen += 1;

  char * pValueName = new char[cbMaxValueNameLen];
  ulong ulValueNameLength;
  for (ulong i = 0; i < cValues; i++)
  {
    ulValueNameLength = cbMaxValueNameLen;
    ZeroMemory(pValueName, cbMaxValueNameLen);
    m_LastErrorCode = RegEnumValueA(m_HKSubKey, i, pValueName, &ulValueNameLength, NULL, NULL, NULL, NULL);
    l.push_back(pValueName);
  }

  delete[] pValueName;

  return l;
}

// Write

bool vuapi CRegistryA::WriteInteger(const std::string& ValueName, int Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(m_HKSubKey, ValueName.c_str(), 0, REG_DWORD, (const uchar*)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::WriteBool(const std::string& ValueName, bool Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar*)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::WriteFloat(const std::string& ValueName, float Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar*)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

// For REG_SZ, REG_MULTI_SZ and REG_EXPAND_SZ:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724884(v=vs.85).aspx

bool vuapi CRegistryA::WriteString(const std::string& ValueName, const std::string& Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_SZ,
    (const uchar*)Value.c_str(),
    (ulong)Value.length() + sizeof(char)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::WriteMultiString(const std::string& ValueName, const char* lpValue)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_MULTI_SZ,
    (const uchar*)lpValue,
    this->GetSizeOfMultiString(lpValue) + sizeof(char)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::WriteMultiString(const std::string& ValueName, const std::vector<std::string>& Value)
{
  auto p = list_to_multi_string_A(Value);
  return this->WriteMultiString(ValueName, p.get());
}

bool vuapi CRegistryA::WriteExpandString(const std::string& ValueName, const std::string& Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_EXPAND_SZ,
    (const uchar*)Value.c_str(),
    (ulong)Value.length() + sizeof(char)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryA::WriteBinary(const std::string& ValueName, void* lpData, ulong ulSize)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExA(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar*)lpData, ulSize);

  return (m_LastErrorCode == ERROR_SUCCESS);
}

// Read

int vuapi CRegistryA::ReadInteger(const std::string& ValueName, int Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  int ret = 0;
  ulong ulType = REG_DWORD, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

bool vuapi CRegistryA::ReadBool(const std::string& ValueName, bool Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  bool ret = false;
  ulong ulType = REG_BINARY, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

float vuapi CRegistryA::ReadFloat(const std::string& ValueName, float Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  float ret = 0;
  ulong ulType = REG_BINARY, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

std::string vuapi CRegistryA::ReadString(const std::string& ValueName, const std::string& Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(char);

  std::unique_ptr<char[]> p(new char [ulReturn]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::string s(p.get());

  return s;
}

std::vector<std::string> vuapi CRegistryA::ReadMultiString(
  const std::string& ValueName,
  std::vector<std::string> Default
)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_MULTI_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(char);

  std::unique_ptr<char[]> p(new char [ulReturn]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::vector<std::string> l = multi_string_to_list_A(p.get());

  return l;
}

std::string vuapi CRegistryA::ReadExpandString(const std::string& ValueName, const std::string& Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_EXPAND_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(char);

  std::unique_ptr<char[]> p(new char [ulReturn]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::string s(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi CRegistryA::ReadBinary(const std::string& ValueName, const void* pDefault)
{
  std::unique_ptr<uchar[]> pDef((uchar*)pDefault);

  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return pDef;
  }

  ulong ulType = REG_BINARY, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    m_LastErrorCode = ERROR_INCORRECT_SIZE;
    SetLastError(m_LastErrorCode);
    return pDef;
  }

  ulReturn += 1;

  std::unique_ptr<uchar[]> p(new uchar [ulReturn]);
  if (p == nullptr)
  {
    return pDef;
  }

  ZeroMemory((void*)p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExA(m_HKSubKey, ValueName.c_str(), NULL, &ulType, p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return pDef;
  }

  return p;
}

CRegistryW::CRegistryW() : CRegistryX()
{
  m_SubKey.clear();
}

CRegistryW::CRegistryW(eRegRoot RegRoot) : CRegistryX()
{
  m_HKRootKey = (HKEY)RegRoot;
}

CRegistryW::CRegistryW(eRegRoot RegRoot, const std::wstring& SubKey) : CRegistryX()
{
  m_HKRootKey = (HKEY)RegRoot;
  m_SubKey = SubKey;
}

CRegistryW::~CRegistryW()
{
}

ulong vuapi CRegistryW::GetSizeOfMultiString(const wchar* lpcwszMultiString)
{
  ulong ulLength = 0;

  while (*lpcwszMultiString)
  {
    ulong crLength = sizeof(wchar)*(lstrlenW(lpcwszMultiString) + 1);
    ulLength += crLength;
    lpcwszMultiString = lpcwszMultiString + crLength / 2;
  }

  return ulLength;
}

ulong vuapi CRegistryW::GetDataSize(const std::wstring& ValueName, ulong ulType)
{
  ulong ulDataSize = 0;

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, NULL, &ulDataSize);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return 0;
  }

  return ulDataSize;
}

bool vuapi CRegistryW::CreateKey()
{
  return this->CreateKey(m_SubKey);
}

bool vuapi CRegistryW::CreateKey(const std::wstring& SubKey)
{
  m_LastErrorCode = RegCreateKeyW(m_HKRootKey, SubKey.c_str(), &m_HKRootKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::KeyExists()
{
  return this->KeyExists(m_SubKey);
}

bool vuapi CRegistryW::KeyExists(const std::wstring& SubKey)
{
  m_LastErrorCode = RegOpenKeyW(m_HKRootKey, SubKey.c_str(), &m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::OpenKey(eRegAccess RegAccess)
{
  return this->OpenKey(m_SubKey, RegAccess);
}

bool vuapi CRegistryW::OpenKey(const std::wstring& SubKey, eRegAccess RegAccess)
{
  m_LastErrorCode = RegOpenKeyExW(m_HKRootKey, SubKey.c_str(), 0, (REGSAM)RegAccess, &m_HKSubKey);
  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::DeleteKey()
{
  return this->DeleteKey(m_SubKey);
}

bool vuapi CRegistryW::DeleteKey(const std::wstring& SubKey)
{
  if (m_HKRootKey == 0)
  {
    return false;
  }

  m_LastErrorCode = RegDeleteKeyW(m_HKRootKey, SubKey.c_str());

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::DeleteValue(const std::wstring& Value)
{
  if (m_HKSubKey == 0)
  {
    return false;
  }

  m_LastErrorCode = RegDeleteValueW(m_HKSubKey, Value.c_str());

  return (m_LastErrorCode == ERROR_SUCCESS);
}

std::vector<std::wstring> vuapi CRegistryW::EnumKeys()
{
  std::vector<std::wstring> l;
  l.clear();

  wchar Class[MAXPATH] = { 0 };
  ulong cchClass = MAXPATH;
  ulong cSubKeys = 0;
  ulong cbMaxSubKeyLen = 0;
  ulong cbMaxClassLen = 0;
  ulong cValues = 0;
  ulong cbMaxValueNameLen = 0;
  ulong cbMaxValueLen = 0;
  ulong cbSecurityDescriptor = 0;
  FILETIME ftLastWriteTime = { 0 };

  ZeroMemory(&Class, sizeof(Class));
  ZeroMemory(&ftLastWriteTime, sizeof(ftLastWriteTime));

  m_LastErrorCode = RegQueryInfoKeyW(
    m_HKSubKey,
    (wchar*)&Class,
    &cchClass,
    NULL,
    &cSubKeys,
    &cbMaxSubKeyLen,
    &cbMaxClassLen,
    &cValues,
    &cbMaxValueNameLen,
    &cbMaxValueLen,
    &cbSecurityDescriptor,
    &ftLastWriteTime
  );

  if (m_LastErrorCode != ERROR_SUCCESS || cSubKeys == 0) return l;

  cbMaxSubKeyLen += 1;

  wchar * pSubKeyName = new wchar[cbMaxSubKeyLen];

  for (ulong i = 0; i < cSubKeys; i++)
  {
    ZeroMemory(pSubKeyName, sizeof(wchar)*cbMaxSubKeyLen);
    m_LastErrorCode = RegEnumKeyW(m_HKSubKey, i, pSubKeyName, sizeof(wchar)*cbMaxSubKeyLen);
    l.push_back(pSubKeyName);
  }

  delete[] pSubKeyName;

  return l;
}

std::vector<std::wstring> vuapi CRegistryW::EnumValues()
{
  std::vector<std::wstring> l;
  l.clear();

  wchar Class[MAXPATH] = { 0 };
  ulong cchClass = MAXPATH;
  ulong cSubKeys = 0;
  ulong cbMaxSubKeyLen = 0;
  ulong cbMaxClassLen = 0;
  ulong cValues = 0;
  ulong cbMaxValueNameLen = 0;
  ulong cbMaxValueLen = 0;
  ulong cbSecurityDescriptor = 0;
  FILETIME ftLastWriteTime = { 0 };

  ZeroMemory(&Class, sizeof(Class));
  ZeroMemory(&ftLastWriteTime, sizeof(ftLastWriteTime));

  m_LastErrorCode = RegQueryInfoKeyA(
    m_HKSubKey,
    (char*)&Class,
    &cchClass,
    NULL,
    &cSubKeys,
    &cbMaxSubKeyLen,
    &cbMaxClassLen,
    &cValues,
    &cbMaxValueNameLen,
    &cbMaxValueLen,
    &cbSecurityDescriptor,
    &ftLastWriteTime
  );

  if (m_LastErrorCode != ERROR_SUCCESS || cValues == 0) return l;

  cbMaxValueNameLen += 1;

  wchar * pValueName = new wchar[cbMaxValueNameLen];
  ulong ulValueNameLength;
  for (ulong i = 0; i < cValues; i++)
  {
    ulValueNameLength = sizeof(wchar)*cbMaxValueNameLen;
    ZeroMemory(pValueName, cbMaxValueNameLen);
    m_LastErrorCode = RegEnumValueW(m_HKSubKey, i, pValueName, &ulValueNameLength, NULL, NULL, NULL, NULL);
    l.push_back(pValueName);
  }

  delete[] pValueName;

  return l;
}

// Write

bool vuapi CRegistryW::WriteInteger(const std::wstring& ValueName, int Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(m_HKSubKey, ValueName.c_str(), 0, REG_DWORD, (const uchar *)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::WriteBool(const std::wstring& ValueName, bool Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar *)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::WriteFloat(const std::wstring& ValueName, float Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar *)&Value, sizeof(Value));

  return (m_LastErrorCode == ERROR_SUCCESS);
}

// For REG_SZ, REG_MULTI_SZ and REG_EXPAND_SZ:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724884(v=vs.85).aspx

bool vuapi CRegistryW::WriteString(const std::wstring& ValueName, const std::wstring& Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_SZ,
    (const uchar *)Value.c_str(),
    sizeof(wchar)*((ulong)Value.length() + 1)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::WriteMultiString(const std::wstring& ValueName, const wchar * lpValue)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_MULTI_SZ,
    (const uchar *)lpValue,
    this->GetSizeOfMultiString(lpValue) + sizeof(wchar)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::WriteMultiString(const std::wstring& ValueName, const std::vector<std::wstring> Value)
{
  auto p = list_to_multi_string_W(Value);
  return this->WriteMultiString(ValueName, p.get());
}

bool vuapi CRegistryW::WriteExpandString(const std::wstring& ValueName, const std::wstring& Value)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(
    m_HKSubKey,
    ValueName.c_str(),
    0,
    REG_EXPAND_SZ,
    (const uchar *)Value.c_str(),
    sizeof(wchar)*((ulong)Value.length() + 1)
  );

  return (m_LastErrorCode == ERROR_SUCCESS);
}

bool vuapi CRegistryW::WriteBinary(const std::wstring& ValueName, void* lpData, ulong ulSize)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return false;
  }

  m_LastErrorCode = RegSetValueExW(m_HKSubKey, ValueName.c_str(), 0, REG_BINARY, (const uchar *)lpData, ulSize);

  return (m_LastErrorCode == ERROR_SUCCESS);
}

// Read

int vuapi CRegistryW::ReadInteger(const std::wstring& ValueName, int Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  int ret = 0;
  ulong ulType = REG_DWORD, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

bool vuapi CRegistryW::ReadBool(const std::wstring& ValueName, bool Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  bool ret = false;
  ulong ulType = REG_BINARY, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

float vuapi CRegistryW::ReadFloat(const std::wstring& ValueName, float Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  float ret = 0;
  ulong ulType = REG_BINARY, ulReturn = sizeof(ret);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)&ret, &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  return ret;
}

std::wstring vuapi CRegistryW::ReadString(const std::wstring& ValueName, const std::wstring& Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(wchar);

  std::unique_ptr<wchar[]> p(new wchar [ulReturn / sizeof(wchar)]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::wstring s(p.get());

  return s;
}

std::vector<std::wstring> vuapi CRegistryW::ReadMultiString(
  const std::wstring& ValueName,
  std::vector<std::wstring> Default
)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_MULTI_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(wchar);

  std::unique_ptr<wchar[]> p(new wchar [ulReturn / sizeof(wchar)]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::vector<std::wstring> l = multi_string_to_list_W(p.get());

  return l;
}

std::wstring vuapi CRegistryW::ReadExpandString(const std::wstring& ValueName, const std::wstring& Default)
{
  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return Default;
  }

  ulong ulType = REG_EXPAND_SZ, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    return Default;
  }

  ulReturn += sizeof(wchar);

  std::unique_ptr<wchar[]> p(new wchar [ulReturn / 2]);
  if (p == nullptr)
  {
    return Default;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return Default;
  }

  std::wstring s(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi CRegistryW::ReadBinary(const std::wstring& ValueName, const void* pDefault)
{
  std::unique_ptr<uchar[]> pDef((uchar*)pDefault);

  if (m_HKSubKey == 0)
  {
    m_LastErrorCode = ERROR_INVALID_HANDLE;
    SetLastError(m_LastErrorCode);
    return pDef;
  }

  ulong ulType = REG_BINARY, ulReturn = this->GetDataSize(ValueName, ulType);
  if (ulReturn == 0)
  {
    m_LastErrorCode = ERROR_INCORRECT_SIZE;
    SetLastError(m_LastErrorCode);
    return pDef;
  }

  ulReturn += sizeof(tchar); // padding for safety

  std::unique_ptr<uchar[]> p(new uchar [ulReturn]);
  if (p == nullptr)
  {
    return pDef;
  }

  ZeroMemory(p.get(), ulReturn);

  m_LastErrorCode = RegQueryValueExW(m_HKSubKey, ValueName.c_str(), NULL, &ulType, (uchar*)p.get(), &ulReturn);
  if (m_LastErrorCode != ERROR_SUCCESS)
  {
    return pDef;
  }

  return p;
}

} // namespace vu