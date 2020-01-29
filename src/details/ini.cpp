/**
 * @file   ini.cpp
 * @author Vic P.
 * @brief  Implementation for INI File
 */

#include "Vutils.h"
#include "strfmt.h"

namespace vu
{

CINIFileA::CINIFileA(const std::string& FilePath)
{
  m_FilePath = FilePath;
}

CINIFileA::CINIFileA(const std::string& FilePath, const std::string& Section)
{
  m_FilePath = FilePath;
  m_Section  = Section;
}

void CINIFileA::ValidFilePath()
{
  if (m_FilePath.empty())
  {
    std::string filePath = GetCurrentFilePathA();
    std::string fileDir  = ExtractFilePathA(filePath, true);
    std::string fileName = ExtractFileNameA(filePath, false);
    m_FilePath = fileDir + fileName + ".INI";
  }
}

void CINIFileA::SetCurrentFilePath(const std::string& FilePath)
{
  m_FilePath = FilePath;
}

void CINIFileA::SetCurrentSection(const std::string& Section)
{
  m_Section = Section;
}

// Long-Read

std::vector<std::string> vuapi CINIFileA::ReadSectionNames(ulong ulMaxSize)
{
  std::vector<std::string> l;
  l.clear();

  std::unique_ptr<char[]> p(new char [ulMaxSize]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), ulMaxSize);

  this->ValidFilePath();

  GetPrivateProfileSectionNamesA(p.get(), ulMaxSize, m_FilePath.c_str());

  m_LastErrorCode = GetLastError();

  for (char * s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::string(s))->c_str());
    s += lstrlenA(s);
  }

  return l;
}

std::vector<std::string> vuapi CINIFileA::ReadSection(const std::string& Section, ulong ulMaxSize)
{
  std::vector<std::string> l;
  l.clear();

  std::unique_ptr<char[]> p(new char [ulMaxSize]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), ulMaxSize);

  this->ValidFilePath();

  GetPrivateProfileSectionA(Section.c_str(), p.get(), ulMaxSize, m_FilePath.c_str());

  m_LastErrorCode = GetLastError();

  for (char * s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::string(s))->c_str());
    s += lstrlenA(s);
  }

  return l;
}

int vuapi CINIFileA::ReadInteger(const std::string& Section, const std::string& Key, int Default)
{
  this->ValidFilePath();
  uint result = GetPrivateProfileIntA(Section.c_str(), Key.c_str(), Default, m_FilePath.c_str());
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileA::ReadBool(const std::string& Section, const std::string& Key, bool Default)
{
  this->ValidFilePath();
  bool result = (GetPrivateProfileIntA(Section.c_str(), Key.c_str(), Default, m_FilePath.c_str()) == 1 ? true : false);
  m_LastErrorCode = GetLastError();
  return result;
}

float vuapi CINIFileA::ReadFloat(const std::string& Section, const std::string& Key, float Default)
{
  const std::string sDefault = NumberToStringA(Default);
  char lpszResult[MAX_SIZE];

  ZeroMemory(lpszResult, sizeof(lpszResult));

  this->ValidFilePath();

  GetPrivateProfileStringA(Section.c_str(), Key.c_str(), sDefault.c_str(), lpszResult, MAX_SIZE, m_FilePath.c_str());

  m_LastErrorCode = GetLastError();

  return (float)atof(lpszResult);
}

std::string vuapi CINIFileA::ReadString(
  const std::string& Section,
  const std::string& Key,
  const std::string& Default
)
{
  std::string s;
  s.clear();

  std::unique_ptr<char[]> p(new char [MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), MAX_SIZE);

  this->ValidFilePath();

  ulong result = GetPrivateProfileStringA(
    Section.c_str(),
    Key.c_str(),
    Default.c_str(),
    p.get(),
    MAX_SIZE,
    m_FilePath.c_str()
  );

  if (result == 0)
  {
    m_LastErrorCode = GetLastError();
    return s;
  }

  s.assign(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi CINIFileA::ReadStruct(const std::string& Section, const std::string& Key, ulong ulSize)
{
  std::unique_ptr<uchar[]> p(new uchar [ulSize]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), ulSize);

  this->ValidFilePath();

  if (GetPrivateProfileStructA(Section.c_str(), Key.c_str(), (void*)p.get(), ulSize, m_FilePath.c_str()) == 0)
  {
    m_LastErrorCode = GetLastError();
    return nullptr;
  }

  return p;
}

// Short-Read

std::vector<std::string> vuapi CINIFileA::ReadSection(ulong ulMaxSize)
{
  return this->ReadSection(m_Section, ulMaxSize);
}

int vuapi CINIFileA::ReadInteger(const std::string& Key, int Default)
{
  return this->ReadInteger(m_Section, Key, Default);
}

bool vuapi CINIFileA::ReadBool(const std::string& Key, bool Default)
{
  return this->ReadBool(m_Section, Key, Default);
}

float vuapi CINIFileA::ReadFloat(const std::string& Key, float Default)
{
  return this->ReadFloat(m_Section, Key, Default);
}

std::string vuapi CINIFileA::ReadString(const std::string& Key, const std::string& Default)
{
  return this->ReadString(m_Section, Key, Default);
}

std::unique_ptr<uchar[]> vuapi CINIFileA::ReadStruct(const std::string& Key, ulong ulSize)
{
  return this->ReadStruct(m_Section, Key, ulSize);
}

// Long-Write

bool vuapi CINIFileA::WriteInteger(const std::string& Section, const std::string& Key, int Value)
{
  this->ValidFilePath();
  const std::string s = NumberToStringA(Value);
  bool result = (WritePrivateProfileStringA(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileA::WriteBool(const std::string& Section, const std::string& Key, bool Value)
{
  this->ValidFilePath();
  const std::string s(Value ? "1" : "0");
  bool result = (WritePrivateProfileStringA(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileA::WriteFloat(const std::string& Section, const std::string& Key, float Value)
{
  this->ValidFilePath();
  const std::string s = NumberToStringA(Value);
  bool result = (WritePrivateProfileStringA(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileA::WriteString(const std::string& Section, const std::string& Key, const std::string& Value)
{
  this->ValidFilePath();
  bool result = (WritePrivateProfileStringA(Section.c_str(), Key.c_str(), Value.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileA::WriteStruct(const std::string& Section, const std::string& Key, void* pStruct, ulong ulSize)
{
  this->ValidFilePath();
  bool result = (WritePrivateProfileStructA(Section.c_str(), Key.c_str(), pStruct, ulSize, m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

// Short-Write

bool vuapi CINIFileA::WriteInteger(const std::string& Key, int Value)
{
  return this->WriteInteger(m_Section, Key, Value);
}

bool vuapi CINIFileA::WriteBool(const std::string& Key, bool Value)
{
  return this->WriteBool(m_Section, Key, Value);
}

bool vuapi CINIFileA::WriteFloat(const std::string& Key, float Value)
{
  return this->WriteFloat(m_Section, Key, Value);
}

bool vuapi CINIFileA::WriteString(const std::string& Key, const std::string& Value)
{
  return this->WriteString(m_Section, Key, Value);
}

bool vuapi CINIFileA::WriteStruct(const std::string& Key, void* pStruct, ulong ulSize)
{
  return this->WriteStruct(m_Section, Key, pStruct, ulSize);
}

CINIFileW::CINIFileW(const std::wstring& FilePath)
{
  m_FilePath = FilePath;

  m_LastErrorCode = ERROR_SUCCESS;
}

CINIFileW::CINIFileW(const std::wstring& FilePath, const std::wstring& Section)
{
  m_FilePath = FilePath;
  m_Section  = Section;

  m_LastErrorCode = ERROR_SUCCESS;
}

void CINIFileW::ValidFilePath()
{
  if (m_FilePath.empty())
  {
    std::wstring filePath = GetCurrentFilePathW();
    std::wstring fileDir = ExtractFilePathW(filePath, true);
    std::wstring fileName = ExtractFileNameW(filePath, false);
    m_FilePath = fileDir + fileName + L".INI";
  }
}

void CINIFileW::SetCurrentFilePath(const std::wstring& FilePath)
{
  m_FilePath = FilePath;
}

void CINIFileW::SetCurrentSection(const std::wstring& Section)
{
  m_Section = Section;
}

// Long-Read

std::vector<std::wstring> vuapi CINIFileW::ReadSectionNames(ulong ulMaxSize)
{
  std::vector<std::wstring> l;
  l.clear();

  std::unique_ptr<wchar[]> p(new wchar [ulMaxSize]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), 2*ulMaxSize);

  this->ValidFilePath();

  GetPrivateProfileSectionNamesW(p.get(), 2*ulMaxSize, m_FilePath.c_str());

  m_LastErrorCode = GetLastError();

  for (wchar * s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::wstring(s))->c_str());
    s += lstrlenW(s);
  }

  return l;
}

std::vector<std::wstring> vuapi CINIFileW::ReadSection(const std::wstring& Section, ulong ulMaxSize)
{
  std::vector<std::wstring> l;
  l.clear();

  std::unique_ptr<wchar[]> p(new wchar [ulMaxSize]);
  if (p == nullptr)
  {
    return l;
  }

  ZeroMemory(p.get(), 2*ulMaxSize);

  this->ValidFilePath();

  GetPrivateProfileSectionW(Section.c_str(), p.get(), 2*ulMaxSize, m_FilePath.c_str());

  m_LastErrorCode = GetLastError();

  for (wchar * s = p.get(); *s; s++)
  {
    l.push_back(s); // l.push_back((new std::wstring(s))->c_str());
    s += lstrlenW(s);
  }

  return l;
}

int vuapi CINIFileW::ReadInteger(const std::wstring& Section, const std::wstring& Key, int Default)
{
  this->ValidFilePath();
  int result = GetPrivateProfileIntW(Section.c_str(), Key.c_str(), Default, m_FilePath.c_str());
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileW::ReadBool(const std::wstring& Section, const std::wstring& Key, bool Default)
{
  this->ValidFilePath();
  bool result = (GetPrivateProfileIntW(Section.c_str(), Key.c_str(), Default, m_FilePath.c_str()) == 1 ? true : false);
  m_LastErrorCode = GetLastError();
  return result;
}

float vuapi CINIFileW::ReadFloat(const std::wstring& Section, const std::wstring& Key, float Default)
{
  this->ValidFilePath();

  const std::wstring sDefault = NumberToStringW(Default);
  wchar lpwszResult[MAX_SIZE];

  ZeroMemory(lpwszResult, sizeof(lpwszResult));

  GetPrivateProfileStringW(
    Section.c_str(),
    Key.c_str(),
    sDefault.c_str(),
    lpwszResult,
    sizeof(lpwszResult),
    m_FilePath.c_str()
  );

  m_LastErrorCode = GetLastError();

  const std::string s = ToStringA(lpwszResult);

  return (float)atof(s.c_str());
}

std::wstring vuapi CINIFileW::ReadString(
  const std::wstring& Section,
  const std::wstring& Key,
  const std::wstring& Default
)
{
  std::wstring s;
  s.clear();

  std::unique_ptr<wchar[]> p(new wchar [MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), 2*MAX_SIZE);

  this->ValidFilePath();

  ulong result = GetPrivateProfileStringW(
    Section.c_str(),
    Key.c_str(),
    Default.c_str(),
    p.get(),
    2 * MAX_SIZE,
    m_FilePath.c_str()
  );
  if (result == 0)
  {
    m_LastErrorCode = GetLastError();
    return s;
  }

  s.assign(p.get());

  return s;
}

std::unique_ptr<uchar[]> vuapi CINIFileW::ReadStruct(
  const std::wstring& Section,
  const std::wstring& Key,
  ulong ulSize
)
{
  std::unique_ptr<uchar[]> p(new uchar [ulSize]);
  if (p == nullptr)
  {
    return nullptr;
  }

  ZeroMemory(p.get(), ulSize);

  this->ValidFilePath();

  if (GetPrivateProfileStructW(Section.c_str(), Key.c_str(), (void*)p.get(), ulSize, m_FilePath.c_str()) == 0)
  {
    m_LastErrorCode = GetLastError();
    return nullptr;
  }

  return p;
}

// Short-Read

std::vector<std::wstring> vuapi CINIFileW::ReadSection(ulong ulMaxSize)
{
  return this->ReadSection(m_Section, ulMaxSize);
}

int vuapi CINIFileW::ReadInteger(const std::wstring& Key, int Default)
{
  return this->ReadInteger(m_Section, Key, Default);
}

bool vuapi CINIFileW::ReadBool(const std::wstring& Key, bool Default)
{
  return this->ReadBool(m_Section, Key, Default);
}

float vuapi CINIFileW::ReadFloat(const std::wstring& Key, float Default)
{
  return this->ReadFloat(m_Section, Key, Default);
}

std::wstring vuapi CINIFileW::ReadString(const std::wstring& Key, const std::wstring& Default)
{
  return this->ReadString(m_Section, Key, Default);
}

std::unique_ptr<uchar[]> vuapi CINIFileW::ReadStruct(const std::wstring& Key, ulong ulSize)
{
  return this->ReadStruct(m_Section, Key, ulSize);
}

// Long-Write

bool vuapi CINIFileW::WriteInteger(const std::wstring& Section, const std::wstring& Key, int Value)
{
  this->ValidFilePath();
  const std::wstring s = NumberToStringW(Value);
  bool result = (WritePrivateProfileStringW(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileW::WriteBool(const std::wstring& Section, const std::wstring& Key, bool Value)
{
  this->ValidFilePath();
  const std::wstring s(Value ? L"1" : L"0");
  return (WritePrivateProfileStringW(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
}

bool vuapi CINIFileW::WriteFloat(const std::wstring& Section, const std::wstring& Key, float Value)
{
  this->ValidFilePath();
  const std::wstring s = NumberToStringW(Value);
  bool result = (WritePrivateProfileStringW(Section.c_str(), Key.c_str(), s.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileW::WriteString(const std::wstring& Section, const std::wstring& Key, const std::wstring& Value)
{
  this->ValidFilePath();
  bool result = (WritePrivateProfileStringW(Section.c_str(), Key.c_str(), Value.c_str(), m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

bool vuapi CINIFileW::WriteStruct(
  const std::wstring& Section,
  const std::wstring& Key,
  void* pStruct,
  ulong ulSize
)
{
  this->ValidFilePath();
  bool result = (WritePrivateProfileStructW(Section.c_str(), Key.c_str(), pStruct, ulSize, m_FilePath.c_str()) != 0);
  m_LastErrorCode = GetLastError();
  return result;
}

// Short-Write

bool vuapi CINIFileW::WriteInteger(const std::wstring& Key, int Value)
{
  return this->WriteInteger(m_Section.c_str(), Key.c_str(), Value);
}

bool vuapi CINIFileW::WriteBool(const std::wstring& Key, bool Value)
{
  return this->WriteBool(m_Section.c_str(), Key.c_str(), Value);
}

bool vuapi CINIFileW::WriteFloat(const std::wstring& Key, float Value)
{
  return this->WriteFloat(m_Section.c_str(), Key.c_str(), Value);
}

bool vuapi CINIFileW::WriteString(const std::wstring& Key, const std::wstring& Value)
{
  return this->WriteString(m_Section.c_str(), Key.c_str(), Value);
}

bool vuapi CINIFileW::WriteStruct(const std::wstring& Key, void* pStruct, ulong ulSize)
{
  return this->WriteStruct(m_Section.c_str(), Key.c_str(), pStruct, ulSize);
}

} // namespace vu