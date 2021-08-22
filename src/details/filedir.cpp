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

bool vuapi is_directory_exists_A(const std::string& directory)
{
  if (GetFileAttributesA(directory.c_str()) == INVALID_FILE_ATTRIBUTES)
  {
    return false;
  }

  return true;
}

bool vuapi is_directory_exists_W(const std::wstring& directory)
{
  if (GetFileAttributesW(directory.c_str()) == INVALID_FILE_ATTRIBUTES)
  {
    return false;
  }

  return true;
}

std::string vuapi get_file_type_A(const std::string& file_path)
{
  SHFILEINFOA SHINFO = {0};
  std::unique_ptr<char[]> szFileType(new char [MAXBYTE]);
  std::string s;
  s.clear();

  if (SHGetFileInfoA(file_path.c_str(), 0, &SHINFO, sizeof(SHFILEINFOA), SHGFI_TYPENAME) != 0)
  {
    ZeroMemory(szFileType.get(), MAXBYTE);
    s = SHINFO.szTypeName;
  }

  return s;
}

std::wstring vuapi get_file_type_W(const std::wstring& file_path)
{
  SHFILEINFOW SHINFO = {0};
  std::unique_ptr<wchar[]> szFileType(new wchar [MAXBYTE]);
  std::wstring s;
  s.clear();

  if (SHGetFileInfoW(file_path.c_str(), 0, &SHINFO, sizeof(SHFILEINFOA), SHGFI_TYPENAME) != 0)
  {
    ZeroMemory(szFileType.get(), 2*MAXBYTE);
    s = SHINFO.szTypeName;
  }

  return s;
}

bool vuapi is_file_exists_A(const std::string& file_path)
{
  bool bResult = false;
  WIN32_FIND_DATAA wfd = {0};

  HANDLE hFile = FindFirstFileA(file_path.c_str(), &wfd);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    bResult = true;
    FindClose(hFile);
  }

  return bResult;
}

bool vuapi is_file_exists_W(const std::wstring& file_path)
{
  bool bResult = false;
  WIN32_FIND_DATAW wfd = {0};

  HANDLE hFile = FindFirstFileW(file_path.c_str(), &wfd);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    bResult = true;
    FindClose(hFile);
  }

  return bResult;
}

std::string vuapi extract_file_directory_A(const std::string& file_path, bool last_slash)
{
  std::string result;
  result.clear();

  size_t pos_slash = file_path.rfind('\\');
  if (pos_slash != std::string::npos)
  {
    result = file_path.substr(0, pos_slash + (last_slash ? 1 : 0));
  }

  return result;
}

std::wstring vuapi extract_file_directory_W(const std::wstring& file_path, bool last_slash)
{
  std::wstring result;
  result.clear();

  size_t pos_slash = file_path.rfind(L'\\');
  if (pos_slash != std::string::npos)
  {
    result = file_path.substr(0, pos_slash + (last_slash ? 1 : 0));
  }

  return result;
}

std::string vuapi extract_file_name_A(const std::string& file_path, bool extension)
{
  std::string result;
  result.clear();

  size_t pos_slash = file_path.rfind('\\');
  if (pos_slash != std::string::npos)
  {
    result = file_path.substr(pos_slash + 1);
  }
  else // only file name in file path
  {
    result = file_path;
  }

  if (!extension)
  {
    size_t pos_dot = result.rfind('.');
    if (pos_dot != std::string::npos)
    {
      result = result.substr(0, pos_dot);
    }
  }

  return result;
}

std::wstring vuapi extract_file_name_W(const std::wstring& file_path, bool extension)
{
  std::wstring result;
  result.clear();

  size_t slashPos = file_path.rfind(L'\\');
  if (slashPos != std::string::npos)
  {
    result = file_path.substr(slashPos + 1);
  }
  else // only file name in file path
  {
    result = file_path;
  }

  if (!extension)
  {
    size_t pos_dot = result.rfind(L'.');
    if (pos_dot != std::string::npos)
    {
      result = result.substr(0, pos_dot);
    }
  }

  return result;
}

std::string vuapi get_current_file_path_A()
{
  std::unique_ptr<char[]> p(new char [MAXPATH]);

  ZeroMemory(p.get(), MAXPATH);

  HMODULE hModule = GetModuleHandleA(NULL);
  GetModuleFileNameA(hModule, p.get(), MAXPATH);
  FreeLibrary(hModule);

  std::string s(p.get());

  return s;
}

std::wstring vuapi get_current_file_path_W()
{
  std::unique_ptr<wchar[]> p(new wchar [MAXPATH]);

  ZeroMemory(p.get(), 2*MAXPATH);

  HMODULE hModule = GetModuleHandleW(NULL);
  GetModuleFileNameW(hModule, p.get(), 2*MAXPATH);
  FreeLibrary(hModule);

  std::wstring s(p.get());

  return s;
}

std::string vuapi get_current_directory_A(bool last_slash)
{
  std::unique_ptr<char[]> p(new char[MAXPATH]);
  ZeroMemory(p.get(), MAXPATH);

  ::GetCurrentDirectoryA(MAXPATH, p.get());
  std::string s(p.get());

  if (last_slash)
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

std::wstring vuapi get_current_directory_W(bool last_slash)
{
  std::unique_ptr<wchar[]> p(new wchar[MAXPATH]);
  ZeroMemory(p.get(), MAXPATH);

  ::GetCurrentDirectoryW(MAXPATH, p.get());
  std::wstring s(p.get());

  if (last_slash)
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

std::string vuapi get_contain_directory_A(bool last_slash)
{
  return extract_file_directory_A(get_current_file_path_A(), last_slash);
}

std::wstring vuapi get_contain_directory_W(bool last_slash)
{
  return extract_file_directory_W(get_current_file_path_W(), last_slash);
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

eDiskType get_disk_type_A(const char drive)
{
  return get_disk_type_W(wchar_t(drive));
}

eDiskType get_disk_type_W(const wchar_t drive)
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
    CWMIProviderW WMI;

    const auto fnIsNumber = [](const std::wstring& str)
    {
      return str.find_first_not_of(L"0123456789") == std::wstring::npos;
    };

    // Generate a map of Partition Names and Disk IDs

    WMI.begin(L"ROOT\\CIMV2");
    {
      WMI.query(L"SELECT * FROM Win32_DiskDrive", [&](IWbemClassObject& object) -> bool
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
        WMI.query(query, [&](IWbemClassObject& object) -> bool
        {
          VARIANT s;
          object.Get(L"DeviceID", 0, &s, 0, 0);

          std::wstring query = L"Associators of {Win32_DiskPartition.DeviceID='";
          query += s.bstrVal;
          query += L"'} where AssocClass=Win32_LogicalDiskToPartition";
          WMI.query(query, [&](IWbemClassObject& object) -> bool
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
    WMI.end();

    // Generate a map of Drive IDs and Disk Types

    WMI.begin(L"ROOT\\Microsoft\\Windows\\Storage");
    {
      WMI.query(L"SELECT * FROM MSFT_PhysicalDisk", [&](IWbemClassObject& object) -> bool
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
    WMI.end();
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
std_string_t join_path_T(const std_string_t& left, const std_string_t& right, const char_t separator)
{
  if (left.empty())
  {
    return right; // "" + "/bar"
  }

  if (left[left.size() - 1] == separator)
  {
    if (right.find(separator) == 0)
    {
      return left + right.substr(1); // foo/ + /bar
    }
    else
    {
      return left + right; // foo/ + bar
    }
  }
  else
  {
    if (right.find(separator) == 0)
    {
      return left + right; // foo + /bar
    }
    else
    {
      return left + separator + right; // foo + bar
    }
  }
}

std::string vuapi join_path_A(const std::string& left, const std::string& right, const ePathSep separator)
{
  const auto Sep = separator == ePathSep::WIN ? '\\' : '/';
  return join_path_T<std::string, char>(left, right, Sep);
}

std::wstring vuapi join_path_W(const std::wstring& left, const std::wstring& right, const ePathSep separator)
{
  const auto sep = separator == ePathSep::WIN ? L'\\' : L'/';
  return join_path_T<std::wstring, wchar_t>(left, right, sep);
}

std::string normalize_path_A(const std::string& path, const ePathSep separator)
{
  auto result = path;

  const std::string SEP_WIN = "\\";
  const std::string SEP_POSIX = "/";
  const std::string sep = separator == ePathSep::WIN ? SEP_WIN : SEP_POSIX;

  result = replace_string_A(result, SEP_WIN + SEP_WIN, sep);
  result = replace_string_A(result, SEP_WIN, sep);
  result = replace_string_A(result, SEP_POSIX, sep);

  return result;
}

std::wstring normalize_path_W(const std::wstring& path, const ePathSep separator)
{
  auto result = path;

  const std::wstring SEP_WIN = L"\\";
  const std::wstring SEP_POSIX = L"/";
  const std::wstring sep = separator == ePathSep::WIN ? SEP_WIN : SEP_POSIX;

  result = replace_string_W(result, SEP_WIN + SEP_WIN, sep);
  result = replace_string_W(result, SEP_WIN, sep);
  result = replace_string_W(result, SEP_POSIX, sep);

  return result;
}

/**
 * CPathA
 */

CPathA::CPathA(const ePathSep Separator) : m_path(""), m_separator(Separator)
{
}

CPathA::CPathA(const std::string& Path, const ePathSep Separator) : m_path(Path), m_separator(Separator)
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
  m_path = right.m_path;
  m_separator  = right.m_separator;
  return *this;
}

const vu::CPathA& CPathA::operator=(const std::string& right)
{
  m_path = right;
  return *this;
}

const vu::CPathA& CPathA::operator+=(const CPathA& right)
{
  *this += right.m_path;
  return *this;
}

const vu::CPathA& CPathA::operator+=(const std::string& right)
{
  this->join(right);
  return *this;
}

vu::CPathA CPathA::operator+(const CPathA& right)
{
  CPathA result(*this);
  result += right.m_path;
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
  bool result = m_separator == right.m_separator;

  if (result)
  {
    if (m_separator == ePathSep::WIN)
    {
      result &= lower_string_A(m_path) == lower_string_A(right.m_path);
    }
    else if (m_separator == ePathSep::POSIX)
    {
      result &= m_path == right.m_path;
    }
  }

  return result;
}

bool CPathA::operator!=(const CPathA& right)
{
  return !(*this == right);
}

vu::CPathA& CPathA::trim(const eTrimType TrimType)
{
  m_path = trim_string_A(m_path, TrimType);
  return *this;
}

vu::CPathA& CPathA::normalize()
{
  m_path = normalize_path_A(m_path, m_separator);
  return *this;
}

vu::CPathA& CPathA::join(const std::string& Path)
{
  m_path = join_path_A(m_path, Path, m_separator);
  return *this;
}

vu::CPathA& CPathA::finalize()
{
  this->trim();
  this->normalize();
  return *this;
}

vu::CPathA CPathA::extract_file_name(bool Extension) const
{
  CPathA tmp(*this);
  tmp.finalize();
  return vu::extract_file_name_A(tmp.as_string(), Extension);
}

vu::CPathA CPathA::extract_file_directory(bool Slash) const
{
  CPathA tmp(*this);
  tmp.finalize();
  return vu::extract_file_directory_A(tmp.as_string(), Slash);
}

bool CPathA::exists() const
{
  CPathA tmp(*this);
  tmp.finalize();
  return is_directory_exists_A(tmp.as_string()) || is_file_exists_A(tmp.as_string());
}

std::string CPathA::as_string() const
{
  return m_path;
}

std::ostream& operator<<(std::ostream& os, CPathA& Path)
{
  os << Path.m_path;
  return os;
}

/**
 * CPathW
 */

CPathW::CPathW(const ePathSep Separator) : m_path(L""), m_separator(Separator)
{
}

CPathW::CPathW(const std::wstring& Path, const ePathSep Separator) : m_path(Path), m_separator(Separator)
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
  m_path = right.m_path;
  m_separator  = right.m_separator;
  return *this;
}

const vu::CPathW& CPathW::operator=(const std::wstring& right)
{
  m_path = right;
  return *this;
}

const vu::CPathW& CPathW::operator+=(const CPathW& right)
{
  *this += right.m_path;
  return *this;
}

const vu::CPathW& CPathW::operator+=(const std::wstring& right)
{
  this->join(right);
  return *this;
}

vu::CPathW CPathW::operator+(const CPathW& right)
{
  CPathW result(*this);
  result += right.m_path;
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
  bool result = m_separator == right.m_separator;

  if (result)
  {
    if (m_separator == ePathSep::WIN)
    {
      result &= lower_string_W(m_path) == lower_string_W(right.m_path);
    }
    else if (m_separator == ePathSep::POSIX)
    {
      result &= m_path == right.m_path;
    }
  }

  return result;
}

bool CPathW::operator!=(const CPathW& right)
{
  return !(*this == right);
}

vu::CPathW& CPathW::trim(const eTrimType TrimType)
{
  m_path = trim_string_W(m_path, TrimType);
  return *this;
}

vu::CPathW& CPathW::normalize()
{
  m_path = normalize_path_W(m_path, m_separator);
  return *this;
}

vu::CPathW& CPathW::join(const std::wstring& Path)
{
  m_path = join_path_W(m_path, Path, m_separator);
  return *this;
}

vu::CPathW& CPathW::finalize()
{
  this->trim();
  this->normalize();
  return *this;
}

vu::CPathW CPathW::extract_file_name(bool Extension) const
{
  CPathW tmp(*this);
  tmp.finalize();
  return vu::extract_file_name_W(tmp.as_string(), Extension);
}

vu::CPathW CPathW::extract_file_directory(bool Slash) const
{
  CPathW tmp(*this);
  tmp.finalize();
  return vu::extract_file_directory_W(tmp.as_string(), Slash);
}

bool CPathW::exists() const
{
  CPathW tmp(*this);
  tmp.finalize();
  return is_directory_exists_W(tmp.as_string()) || is_file_exists_W(tmp.as_string());
}

std::wstring CPathW::as_string() const
{
  return m_path;
}

std::wostream& operator<<(std::wostream& os, CPathW& Path)
{
  os << Path.m_path;
  return os;
}

} // namespace vu