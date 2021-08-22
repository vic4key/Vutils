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
  std::unique_ptr<char[]> ps_file_type(new char [MAXBYTE]);
  std::string s;
  s.clear();

  if (SHGetFileInfoA(file_path.c_str(), 0, &SHINFO, sizeof(SHFILEINFOA), SHGFI_TYPENAME) != 0)
  {
    ZeroMemory(ps_file_type.get(), MAXBYTE);
    s = SHINFO.szTypeName;
  }

  return s;
}

std::wstring vuapi get_file_type_W(const std::wstring& file_path)
{
  SHFILEINFOW SHINFO = {0};
  std::unique_ptr<wchar[]> ps_file_type(new wchar [MAXBYTE]);
  std::wstring s;
  s.clear();

  if (SHGetFileInfoW(file_path.c_str(), 0, &SHINFO, sizeof(SHFILEINFOA), SHGFI_TYPENAME) != 0)
  {
    ZeroMemory(ps_file_type.get(), 2*MAXBYTE);
    s = SHINFO.szTypeName;
  }

  return s;
}

bool vuapi is_file_exists_A(const std::string& file_path)
{
  bool result = false;
  WIN32_FIND_DATAA wfd = {0};

  HANDLE hfile = FindFirstFileA(file_path.c_str(), &wfd);
  if (hfile != INVALID_HANDLE_VALUE)
  {
    result = true;
    FindClose(hfile);
  }

  return result;
}

bool vuapi is_file_exists_W(const std::wstring& file_path)
{
  bool result = false;
  WIN32_FIND_DATAW wfd = {0};

  HANDLE hfile = FindFirstFileW(file_path.c_str(), &wfd);
  if (hfile != INVALID_HANDLE_VALUE)
  {
    result = true;
    FindClose(hfile);
  }

  return result;
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

  size_t pos_slash = file_path.rfind(L'\\');
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

  HMODULE hmodule = GetModuleHandleA(NULL);
  GetModuleFileNameA(hmodule, p.get(), MAXPATH);
  FreeLibrary(hmodule);

  std::string s(p.get());

  return s;
}

std::wstring vuapi get_current_file_path_W()
{
  std::unique_ptr<wchar[]> p(new wchar [MAXPATH]);

  ZeroMemory(p.get(), 2*MAXPATH);

  HMODULE hmodule = GetModuleHandleW(NULL);
  GetModuleFileNameW(hmodule, p.get(), 2*MAXPATH);
  FreeLibrary(hmodule);

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

// wchar_t extract_drive_letter(const std::wstring& path)
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
    WMIProviderW wmi;

    const auto fnIsNumber = [](const std::wstring& str)
    {
      return str.find_first_not_of(L"0123456789") == std::wstring::npos;
    };

    // Generate a map of Partition Names and Disk IDs

    wmi.begin(L"ROOT\\CIMV2");
    {
      wmi.query(L"SELECT * FROM Win32_DiskDrive", [&](IWbemClassObject& object) -> bool
      {
        VARIANT s = { 0 };
        object.Get(L"DeviceID", 0, &s, 0, 0);
        std::wstring tmp = s.bstrVal;
        tmp = tmp.substr(4); // Remove `\.\` in `\.\PHYSICALDRIVE?`
        std::wstring id = tmp;
        id = id.substr(13); // Remove `PHYSICALDRIVE` in `PHYSICALDRIVE?`

        std::wstring query = L"Associators of {Win32_DiskDrive.DeviceID='\\\\.\\";
        query += tmp;
        query += L"'} where AssocClass=Win32_DiskDriveToDiskPartition";
        wmi.query(query, [&](IWbemClassObject& object) -> bool
        {
          VARIANT s = { 0 };
          object.Get(L"DeviceID", 0, &s, 0, 0);

          std::wstring query = L"Associators of {Win32_DiskPartition.DeviceID='";
          query += s.bstrVal;
          query += L"'} where AssocClass=Win32_LogicalDiskToPartition";
          wmi.query(query, [&](IWbemClassObject& object) -> bool
          {
            if (fnIsNumber(id))
            {
              VARIANT s = { 0 };
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
    wmi.end();

    // Generate a map of Drive IDs and Disk Types

    wmi.begin(L"ROOT\\Microsoft\\Windows\\Storage");
    {
      wmi.query(L"SELECT * FROM MSFT_PhysicalDisk", [&](IWbemClassObject& object) -> bool
      {
        VARIANT device_id = { 0 };
        object.Get(L"DeviceId", 0, &device_id, 0, 0);

        VARIANT media_type = { 0 };
        object.Get(L"MediaType", 0, &media_type, 0, 0);

        std::wstring id = device_id.bstrVal;
        if (fnIsNumber(id))
        {
          types[std::stoi(id)] = static_cast<eDiskType>(media_type.uintVal);
        }

        return true;
      });
    }
    wmi.end();
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

PathA::PathA(const ePathSep separator) : m_path(""), m_separator(separator)
{
}

PathA::PathA(const std::string& path, const ePathSep separator) : m_path(path), m_separator(separator)
{
}

PathA::PathA(const PathA& right)
{
  *this = right;
}

PathA::~PathA()
{
}

const vu::PathA& PathA::operator=(const PathA& right)
{
  m_path = right.m_path;
  m_separator  = right.m_separator;
  return *this;
}

const vu::PathA& PathA::operator=(const std::string& right)
{
  m_path = right;
  return *this;
}

const vu::PathA& PathA::operator+=(const PathA& right)
{
  *this += right.m_path;
  return *this;
}

const vu::PathA& PathA::operator+=(const std::string& right)
{
  this->join(right);
  return *this;
}

vu::PathA PathA::operator+(const PathA& right)
{
  PathA result(*this);
  result += right.m_path;
  return result;
}

vu::PathA PathA::operator+(const std::string& right)
{
  PathA result(*this);
  result += right;
  return result;
}

bool PathA::operator==(const PathA& right)
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

bool PathA::operator!=(const PathA& right)
{
  return !(*this == right);
}

vu::PathA& PathA::trim(const eTrimType type)
{
  m_path = trim_string_A(m_path, type);
  return *this;
}

vu::PathA& PathA::normalize()
{
  m_path = normalize_path_A(m_path, m_separator);
  return *this;
}

vu::PathA& PathA::join(const std::string& path)
{
  m_path = join_path_A(m_path, path, m_separator);
  return *this;
}

vu::PathA& PathA::finalize()
{
  this->trim();
  this->normalize();
  return *this;
}

vu::PathA PathA::extract_file_name(bool with_extension) const
{
  PathA tmp(*this);
  tmp.finalize();
  return vu::extract_file_name_A(tmp.as_string(), with_extension);
}

vu::PathA PathA::extract_file_directory(bool with_slash) const
{
  PathA tmp(*this);
  tmp.finalize();
  return vu::extract_file_directory_A(tmp.as_string(), with_slash);
}

bool PathA::exists() const
{
  PathA tmp(*this);
  tmp.finalize();
  return is_directory_exists_A(tmp.as_string()) || is_file_exists_A(tmp.as_string());
}

std::string PathA::as_string() const
{
  return m_path;
}

std::ostream& operator<<(std::ostream& os, PathA& Path)
{
  os << Path.m_path;
  return os;
}

/**
 * CPathW
 */

PathW::PathW(const ePathSep Separator) : m_path(L""), m_separator(Separator)
{
}

PathW::PathW(const std::wstring& path, const ePathSep separator) : m_path(path), m_separator(separator)
{
}

PathW::PathW(const PathW& right)
{
  *this = right;
}

PathW::~PathW()
{
}

const vu::PathW& PathW::operator=(const PathW& right)
{
  m_path = right.m_path;
  m_separator  = right.m_separator;
  return *this;
}

const vu::PathW& PathW::operator=(const std::wstring& right)
{
  m_path = right;
  return *this;
}

const vu::PathW& PathW::operator+=(const PathW& right)
{
  *this += right.m_path;
  return *this;
}

const vu::PathW& PathW::operator+=(const std::wstring& right)
{
  this->join(right);
  return *this;
}

vu::PathW PathW::operator+(const PathW& right)
{
  PathW result(*this);
  result += right.m_path;
  return result;
}

vu::PathW PathW::operator+(const std::wstring& right)
{
  PathW result(*this);
  result += right;
  return result;
}

bool PathW::operator==(const PathW& right)
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

bool PathW::operator!=(const PathW& right)
{
  return !(*this == right);
}

vu::PathW& PathW::trim(const eTrimType type)
{
  m_path = trim_string_W(m_path, type);
  return *this;
}

vu::PathW& PathW::normalize()
{
  m_path = normalize_path_W(m_path, m_separator);
  return *this;
}

vu::PathW& PathW::join(const std::wstring& Path)
{
  m_path = join_path_W(m_path, Path, m_separator);
  return *this;
}

vu::PathW& PathW::finalize()
{
  this->trim();
  this->normalize();
  return *this;
}

vu::PathW PathW::extract_file_name(bool with_extension) const
{
  PathW tmp(*this);
  tmp.finalize();
  return vu::extract_file_name_W(tmp.as_string(), with_extension);
}

vu::PathW PathW::extract_file_directory(bool with_slash) const
{
  PathW tmp(*this);
  tmp.finalize();
  return vu::extract_file_directory_W(tmp.as_string(), with_slash);
}

bool PathW::exists() const
{
  PathW tmp(*this);
  tmp.finalize();
  return is_directory_exists_W(tmp.as_string()) || is_file_exists_W(tmp.as_string());
}

std::wstring PathW::as_string() const
{
  return m_path;
}

std::wostream& operator<<(std::wostream& os, PathW& Path)
{
  os << Path.m_path;
  return os;
}

} // namespace vu