/**
 * @file   filedir.cpp
 * @author Vic P.
 * @brief  Implementation for File/Directory
 */

#include "Vutils.h"

#include <map>
#include <cwctype>
#include <shellapi.h>
#include <comdef.h>
#include <wbemidl.h>

namespace vu
{

bool vuapi IsDirectoryExistsA(const std::string& Directory)
{
  if (GetFileAttributesA(Directory.c_str()) == INVALID_FILE_ATTRIBUTES)
  {
    return false;
  }

  return true;
}

bool vuapi IsDirectoryExistsW(const std::wstring& Directory)
{
  if (GetFileAttributesW(Directory.c_str()) == INVALID_FILE_ATTRIBUTES)
  {
    return false;
  }

  return true;
}

std::string vuapi FileTypeA(const std::string& FilePath)
{
  SHFILEINFOA SHINFO = {0};
  std::unique_ptr<char[]> szFileType(new char [MAXBYTE]);
  std::string s;
  s.clear();

  if (SHGetFileInfoA(FilePath.c_str(), 0, &SHINFO, sizeof(SHFILEINFOA), SHGFI_TYPENAME) != 0)
  {
    ZeroMemory(szFileType.get(), MAXBYTE);
    s = SHINFO.szTypeName;
  }

  return s;
}

std::wstring vuapi FileTypeW(const std::wstring& FilePath)
{
  SHFILEINFOW SHINFO = {0};
  std::unique_ptr<wchar[]> szFileType(new wchar [MAXBYTE]);
  std::wstring s;
  s.clear();

  if (SHGetFileInfoW(FilePath.c_str(), 0, &SHINFO, sizeof(SHFILEINFOA), SHGFI_TYPENAME) != 0)
  {
    ZeroMemory(szFileType.get(), 2*MAXBYTE);
    s = SHINFO.szTypeName;
  }

  return s;
}

bool vuapi IsFileExistsA(const std::string& FilePath)
{
  bool bResult = false;
  WIN32_FIND_DATAA wfd = {0};

  HANDLE hFile = FindFirstFileA(FilePath.c_str(), &wfd);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    bResult = true;
    FindClose(hFile);
  }

  return bResult;
}

bool vuapi IsFileExistsW(const std::wstring& FilePath)
{
  bool bResult = false;
  WIN32_FIND_DATAW wfd = {0};

  HANDLE hFile = FindFirstFileW(FilePath.c_str(), &wfd);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    bResult = true;
    FindClose(hFile);
  }

  return bResult;
}

std::string vuapi ExtractFileDirectoryA(const std::string& FilePath, bool Slash)
{
  std::string filePath;
  filePath.clear();

  size_t slashPos = FilePath.rfind('\\');
  if (slashPos != std::string::npos)
  {
    filePath = FilePath.substr(0, slashPos + (Slash ? 1 : 0));
  }

  return filePath;
}

std::wstring vuapi ExtractFileDirectoryW(const std::wstring& FilePath, bool Slash)
{
  std::wstring filePath;
  filePath.clear();

  size_t slashPos = FilePath.rfind(L'\\');
  if (slashPos != std::string::npos)
  {
    filePath = FilePath.substr(0, slashPos + (Slash ? 1 : 0));
  }

  return filePath;
}

std::string vuapi ExtractFileNameA(const std::string& FilePath, bool Extension)
{
  std::string fileName;
  fileName.clear();

  size_t slashPos = FilePath.rfind('\\');
  if (slashPos != std::string::npos)
  {
    fileName = FilePath.substr(slashPos + 1);
  }
  else // only file name in file path
  {
    fileName = FilePath;
  }

  if (!Extension)
  {
    size_t dotPos = fileName.rfind('.');
    if (dotPos != std::string::npos)
    {
      fileName = fileName.substr(0, dotPos);
    }
  }

  return fileName;
}

std::wstring vuapi ExtractFileNameW(const std::wstring& FilePath, bool Extension)
{
  std::wstring fileName;
  fileName.clear();

  size_t slashPos = FilePath.rfind(L'\\');
  if (slashPos != std::string::npos)
  {
    fileName = FilePath.substr(slashPos + 1);
  }
  else // only file name in file path
  {
    fileName = FilePath;
  }

  if (!Extension)
  {
    size_t dotPos = fileName.rfind(L'.');
    if (dotPos != std::string::npos)
    {
      fileName = fileName.substr(0, dotPos);
    }
  }

  return fileName;
}

std::string vuapi GetCurrentFilePathA()
{
  std::unique_ptr<char[]> p(new char [MAXPATH]);

  ZeroMemory(p.get(), MAXPATH);

  HMODULE hModule = GetModuleHandleA(NULL);
  GetModuleFileNameA(hModule, p.get(), MAXPATH);
  FreeLibrary(hModule);

  std::string s(p.get());

  return s;
}

std::wstring vuapi GetCurrentFilePathW()
{
  std::unique_ptr<wchar[]> p(new wchar [MAXPATH]);

  ZeroMemory(p.get(), 2*MAXPATH);

  HMODULE hModule = GetModuleHandleW(NULL);
  GetModuleFileNameW(hModule, p.get(), 2*MAXPATH);
  FreeLibrary(hModule);

  std::wstring s(p.get());

  return s;
}

std::string vuapi GetCurrentDirectoryA(bool Slash)
{
  std::unique_ptr<char[]> p(new char[MAXPATH]);
  ZeroMemory(p.get(), MAXPATH);

  ::GetCurrentDirectoryA(MAXPATH, p.get());
  std::string s(p.get());

  if (Slash)
  {
    if (s.back() != '\\')
    {
      s += L'\\';
    }
  }
  else
  {
    if (s.back() == '\\')
    {
      s.pop_back();
    }
  }

  return s;
}

std::wstring vuapi GetCurrentDirectoryW(bool Slash)
{
  std::unique_ptr<wchar[]> p(new wchar[MAXPATH]);
  ZeroMemory(p.get(), MAXPATH);

  ::GetCurrentDirectoryW(MAXPATH, p.get());
  std::wstring s(p.get());

  if (Slash)
  {
    if (s.back() != L'\\')
    {
      s += L'\\';
    }
  }
  else
  {
    if (s.back() == L'\\')
    {
      s.pop_back();
    }
  }

  return s;
}

std::string vuapi GetContainDirectoryA(bool Slash)
{
  return ExtractFileDirectoryA(GetCurrentFilePathA(), Slash);
}

std::wstring vuapi GetContainDirectoryW(bool Slash)
{
  return ExtractFileDirectoryW(GetCurrentFilePathW(), Slash);
}

// wchar_t ExtractDriveLetter(const std::wstring& path)
// {
//   wchar_t result = L'';
// 
//   int idx = PathGetDriveNumberW(path.c_str());
//   if (idx != -1)
//   {
//     result = static_cast<wchar_t>(idx + L'A');
//   }
// 
//   return result;
// }

#if defined(VU_WMI_ENABLED)

eDiskType GetDiskType(const wchar_t drive)
{
  if (std::iswalpha(drive) == 0)
  {
    return eDiskType::Unspecified;
  }

  static std::map<int, eDiskType> types;
  static std::map<std::wstring, int> partitions;

  // Once generate maps for Partition Names and Disk IDs and Partition Names

  if (types.empty() || partitions.empty())
  {
    CWMIProvider WMI;

    const auto fnIsNumber = [](const std::wstring& str)
    {
      return str.find_first_not_of(L"0123456789") == std::wstring::npos;
    };

    // Generate a map of Partition Names and Disk IDs

    WMI.Begin(L"ROOT\\CIMV2");
    {
      WMI.Query(L"SELECT * FROM Win32_DiskDrive", [&](IWbemClassObject& object) -> bool
      {
        VARIANT s;
        object.Get(L"DeviceID", 0, &s, 0, 0);
        std::wstring tmp = s.bstrVal;
        tmp = tmp.substr(4); // Remove `\.\` in `\.\PHYSICALDRIVE?`
        std::wstring id = tmp;
        id = id.substr(13); // Remove `PHYSICALDRIVE` in `PHYSICALDRIVE?`

        std::wstring query = L"Associators of {Win32_DiskDrive.DeviceID='\\\\.\\";
        query += tmp;
        query += L"'} where AssocClass=Win32_DiskDriveToDiskPartition";
        WMI.Query(query, [&](IWbemClassObject& object) -> bool
        {
          VARIANT s;
          object.Get(L"DeviceID", 0, &s, 0, 0);

          std::wstring query = L"Associators of {Win32_DiskPartition.DeviceID='";
          query += s.bstrVal;
          query += L"'} where AssocClass=Win32_LogicalDiskToPartition";
          WMI.Query(query, [&](IWbemClassObject& object) -> bool
          {
            if (fnIsNumber(id))
            {
              VARIANT s;
              object.Get(L"DeviceID", 0, &s, 0, 0);
              std::wstring letter = s.bstrVal;

              partitions[letter] = std::stoi(id);
            }

            return true;
          });

          return true;
        });

        return true;
      });
    }
    WMI.End();

    // Generate a map of Drive IDs and Disk Types

    WMI.Begin(L"ROOT\\Microsoft\\Windows\\Storage");
    {
      WMI.Query(L"SELECT * FROM MSFT_PhysicalDisk", [&](IWbemClassObject& object) -> bool
      {
        VARIANT deviceId;
        object.Get(L"DeviceId", 0, &deviceId, 0, 0);

        VARIANT mediaType;
        object.Get(L"MediaType", 0, &mediaType, 0, 0);

        std::wstring id = deviceId.bstrVal;
        if (fnIsNumber(id))
        {
          types[std::stoi(id)] = static_cast<eDiskType>(mediaType.uintVal);
        }

        return true;
      });
    }
    WMI.End();
  }

  // Find the Disk Type of a Partition

  eDiskType result = eDiskType::Unspecified;

  std::wstring s = L"";
  s += std::towupper(drive);
  s += L':';

  auto partition = partitions.find(s);
  if (partition != partitions.cend())
  {
    auto type = types.find(partition->second);
    if (type != types.cend())
    {
      result = type->second;
    }
  }

  return result;
}

#endif // VU_WMI_ENABLED

template <class std_string_t, typename char_t>
std_string_t JoinPathT(
  const std_string_t& Left,
  const std_string_t& Right,
  const char_t Sep
)
{
  if (Left.empty())
  {
    return Right; // "" + "/bar"
  }

  if (Left[Left.size() - 1] == Sep)
  {
    if (Right.find(Sep) == 0)
    {
      return Left + Right.substr(1); // foo/ + /bar
    }
    else
    {
      return Left + Right; // foo/ + bar
    }
  }
  else
  {
    if (Right.find(Sep) == 0)
    {
      return Left + Right; // foo + /bar
    }
    else
    {
      return Left + Sep + Right; // foo + bar
    }
  }
}

std::string vuapi JoinPathA(
  const std::string& Left,
  const std::string& Right,
  const ePathSep Separator
)
{
  const auto Sep = Separator == ePathSep::WIN ? '\\' : '/';
  return JoinPathT<std::string, char>(Left, Right, Sep);
}

std::wstring vuapi JoinPathW(
  const std::wstring& Left,
  const std::wstring& Right,
  const ePathSep Separator
)
{
  const auto Sep = Separator == ePathSep::WIN ? L'\\' : L'/';
  return JoinPathT<std::wstring, wchar_t>(Left, Right, Sep);
}

std::string NormalizePathA(const std::string& Path, const ePathSep Separator)
{
  auto result = Path;

  const std::string SepWIN = "\\";
  const std::string SepPOSIX = "/";
  const std::string Sep = Separator == ePathSep::WIN ? SepWIN : SepPOSIX;

  result = ReplaceA(result, SepWIN + SepWIN, Sep);
  result = ReplaceA(result, SepWIN, Sep);
  result = ReplaceA(result, SepPOSIX, Sep);

  return result;
}

std::wstring NormalizePathW(const std::wstring& Path, const ePathSep Separator)
{
  auto result = Path;

  const std::wstring SepWIN = L"\\";
  const std::wstring SepPOSIX = L"/";
  const std::wstring Sep = Separator == ePathSep::WIN ? SepWIN : SepPOSIX;

  result = ReplaceW(result, SepWIN + SepWIN, Sep);
  result = ReplaceW(result, SepWIN, Sep);
  result = ReplaceW(result, SepPOSIX, Sep);

  return result;
}

/**
 * CPathA
 */

CPathA::CPathA(const ePathSep Separator) : m_Path(""), m_Sep(Separator)
{
}

CPathA::CPathA(const std::string& Path, const ePathSep Separator) : m_Path(Path), m_Sep(Separator)
{
}

CPathA::CPathA(const CPathA& right)
{
  *this = right;
}

CPathA::~CPathA()
{
}

const vu::CPathA& CPathA::operator=(const CPathA& right)
{
  m_Path = right.m_Path;
  m_Sep  = right.m_Sep;
  return *this;
}

const vu::CPathA& CPathA::operator=(const std::string& right)
{
  m_Path = right;
  return *this;
}

const vu::CPathA& CPathA::operator+=(const CPathA& right)
{
  *this += right.m_Path;
  return *this;
}

const vu::CPathA& CPathA::operator+=(const std::string& right)
{
  this->Join(right);
  return *this;
}

vu::CPathA CPathA::operator+(const CPathA& right)
{
  CPathA result(*this);
  result += right.m_Path;
  return result;
}

vu::CPathA CPathA::operator+(const std::string& right)
{
  CPathA result(*this);
  result += right;
  return result;
}

bool CPathA::operator==(const CPathA& right)
{
  bool result = m_Sep == right.m_Sep;

  if (result)
  {
    if (m_Sep == ePathSep::WIN)
    {
      result &= LowerStringA(m_Path) == LowerStringA(right.m_Path);
    }
    else if (m_Sep == ePathSep::POSIX)
    {
      result &= m_Path == right.m_Path;
    }
  }

  return result;
}

bool CPathA::operator!=(const CPathA& right)
{
  return !(*this == right);
}

vu::CPathA& CPathA::Trim(const eTrimType TrimType)
{
  m_Path = TrimStringA(m_Path, TrimType);
  return *this;
}

vu::CPathA& CPathA::Normalize()
{
  m_Path = NormalizePathA(m_Path, m_Sep);
  return *this;
}

vu::CPathA& CPathA::Join(const std::string& Path)
{
  m_Path = JoinPathA(m_Path, Path, m_Sep);
  return *this;
}

vu::CPathA& CPathA::Finalize()
{
  Trim();
  Normalize();
  return *this;
}

vu::CPathA CPathA::FileName(bool Extension) const
{
  CPathA tmp(*this);
  tmp.Finalize();
  return vu::ExtractFileNameA(tmp.AsString(), Extension);
}

vu::CPathA CPathA::FileDirectory(bool Slash) const
{
  CPathA tmp(*this);
  tmp.Finalize();
  return vu::ExtractFileDirectoryA(tmp.AsString(), Slash);
}

bool CPathA::Exists() const
{
  CPathA tmp(*this);
  tmp.Finalize();
  return IsDirectoryExistsA(tmp.AsString()) || IsFileExistsA(tmp.AsString());
}

std::string CPathA::AsString() const
{
  return m_Path;
}

std::ostream& operator<<(std::ostream& os, CPathA& Path)
{
  os << Path.m_Path;
  return os;
}

/**
 * CPathW
 */

CPathW::CPathW(const ePathSep Separator) : m_Path(L""), m_Sep(Separator)
{
}

CPathW::CPathW(const std::wstring& Path, const ePathSep Separator) : m_Path(Path), m_Sep(Separator)
{
}

CPathW::CPathW(const CPathW& right)
{
  *this = right;
}

CPathW::~CPathW()
{
}

const vu::CPathW& CPathW::operator=(const CPathW& right)
{
  m_Path = right.m_Path;
  m_Sep  = right.m_Sep;
  return *this;
}

const vu::CPathW& CPathW::operator=(const std::wstring& right)
{
  m_Path = right;
  return *this;
}

const vu::CPathW& CPathW::operator+=(const CPathW& right)
{
  *this += right.m_Path;
  return *this;
}

const vu::CPathW& CPathW::operator+=(const std::wstring& right)
{
  this->Join(right);
  return *this;
}

vu::CPathW CPathW::operator+(const CPathW& right)
{
  CPathW result(*this);
  result += right.m_Path;
  return result;
}

vu::CPathW CPathW::operator+(const std::wstring& right)
{
  CPathW result(*this);
  result += right;
  return result;
}

bool CPathW::operator==(const CPathW& right)
{
  bool result = m_Sep == right.m_Sep;

  if (result)
  {
    if (m_Sep == ePathSep::WIN)
    {
      result &= LowerStringW(m_Path) == LowerStringW(right.m_Path);
    }
    else if (m_Sep == ePathSep::POSIX)
    {
      result &= m_Path == right.m_Path;
    }
  }

  return result;
}

bool CPathW::operator!=(const CPathW& right)
{
  return !(*this == right);
}

vu::CPathW& CPathW::Trim(const eTrimType TrimType)
{
  m_Path = TrimStringW(m_Path, TrimType);
  return *this;
}

vu::CPathW& CPathW::Normalize()
{
  m_Path = NormalizePathW(m_Path, m_Sep);
  return *this;
}

vu::CPathW& CPathW::Join(const std::wstring& Path)
{
  m_Path = JoinPathW(m_Path, Path, m_Sep);
  return *this;
}

vu::CPathW& CPathW::Finalize()
{
  Trim();
  Normalize();
  return *this;
}

vu::CPathW CPathW::FileName(bool Extension) const
{
  CPathW tmp(*this);
  tmp.Finalize();
  return vu::ExtractFileNameW(tmp.AsString(), Extension);
}

vu::CPathW CPathW::FileDirectory(bool Slash) const
{
  CPathW tmp(*this);
  tmp.Finalize();
  return vu::ExtractFileDirectoryW(tmp.AsString(), Slash);
}

bool CPathW::Exists() const
{
  CPathW tmp(*this);
  tmp.Finalize();
  return IsDirectoryExistsW(tmp.AsString()) || IsFileExistsW(tmp.AsString());
}

std::wstring CPathW::AsString() const
{
  return m_Path;
}

std::wostream& operator<<(std::wostream& os, CPathW& Path)
{
  os << Path.m_Path;
  return os;
}

} // namespace vu