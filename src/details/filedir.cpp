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

static const std::wstring FILE_NAME_FORBIDDEN_CHARS = L"\\/:?\"<>|";

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

bool vuapi is_file_name_valid_A(const std::string& file_name)
{
  auto file_name_W = to_string_W(file_name);
  return is_file_name_valid_W(file_name_W);
}

bool vuapi is_file_name_valid_W(const std::wstring& file_name)
{
  for (auto c : FILE_NAME_FORBIDDEN_CHARS)
  {
    if (file_name.find(c) != std::wstring::npos)
    {
      return false;
    }
  }

  return true;
}

std::string vuapi correct_file_name_A(const std::string& file_name, const char replacement_char)
{
  auto file_name_W = to_string_W(file_name);
  file_name_W = correct_file_name_W(file_name_W, wchar_t(replacement_char));
  return to_string_A(file_name_W);
}

std::wstring vuapi correct_file_name_W(const std::wstring& file_name, const wchar_t replacement_char)
{
  std::wstring tmp = file_name;
  std::replace_if(tmp.begin(), tmp.end(), [&](wchar_t c) -> bool
  {
    return std::wstring::npos != FILE_NAME_FORBIDDEN_CHARS.find(c);
  }, replacement_char);

  return tmp;
}

bool vuapi read_file_binary_A(const std::string& file_path, std::vector<byte>& data)
{
  auto file_path_W = to_string_W(file_path);
  return read_file_binary_W(file_path_W, data);
}

bool vuapi read_file_binary_W(const std::wstring& file_path, std::vector<byte>& data)
{
  data.clear();

  FILE* file = nullptr;
  _wfopen_s(&file, file_path.c_str(), L"rb");
  if (file == nullptr)
  {
    return false;
  }

  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (data.empty() || data.size() > file_size)
  {
    data.resize(file_size);
  }

  std::fill(data.begin(), data.end(), 0);

  size_t read_bytes = fread(&data[0], 1, data.size(), file);

  fclose(file);

  return read_bytes == data.size();
}

std::unique_ptr<std::vector<vu::byte>> vuapi read_file_binary_A(const std::string& file_path)
{
  auto file_path_W = to_string_W(file_path);
  return read_file_binary_W(file_path_W);
}

std::unique_ptr<std::vector<vu::byte>> vuapi read_file_binary_W(const std::wstring& file_path)
{
  std::vector<vu::byte> data;
  read_file_binary_W(file_path, data);
  if (data.empty())
  {
    nullptr;
  }

  std::unique_ptr<std::vector<vu::byte>> ptr(new std::vector<vu::byte>);
  ptr->swap(data);
  return ptr;
}

bool vuapi write_file_binary_A(const std::string& file_path, const byte* data, const size_t size)
{
  auto file_path_W = to_string_W(file_path);
  return write_file_binary_W(file_path_W, data, size);
}

bool vuapi write_file_binary_W(const std::wstring& file_path, const std::vector<byte>& data)
{
  if (data.empty())
  {
    return false;
  }

  return write_file_binary_W(file_path , &data[0], data.size());
}

bool vuapi write_file_binary_A(const std::string& file_path, const std::vector<byte>& data)
{
  auto file_path_W = to_string_W(file_path);
  return write_file_binary_W(file_path_W, data);
}

bool vuapi write_file_binary_W(const std::wstring& file_path, const byte* data, const size_t size)
{
  if (data == nullptr || size == 0)
  {
    return false;
  }

  FILE* file = nullptr;
  _wfopen_s(&file, file_path.c_str(), L"wb");
  if (file == nullptr)
  {
    return false;
  }

  fseek(file, 0, SEEK_END);

  size_t written_bytes = fwrite(data, 1, size, file);

  fclose(file);

  return written_bytes == size;
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

std::string vuapi extract_file_extension_A(const std::string& file_path)
{
  std::string result;

  size_t pos_dot = file_path.find_last_of('.');
  if (pos_dot != std::string::npos)
  {
    result = file_path.substr(pos_dot);
  }

  return result;
}

std::wstring vuapi extract_file_extension_W(const std::wstring& file_path)
{
  std::wstring result;

  size_t pos_dot = file_path.find_last_of('.');
  if (pos_dot != std::wstring::npos)
  {
    result = file_path.substr(pos_dot);
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

disk_type get_disk_type_A(const char drive)
{
  return get_disk_type_W(wchar_t(drive));
}

disk_type get_disk_type_W(const wchar_t drive)
{
  if (std::iswalpha(drive) == 0)
  {
    return disk_type::Unspecified;
  }

  static std::map<int, disk_type> types;
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
          types[std::stoi(id)] = static_cast<disk_type>(media_type.uintVal);
        }

        return true;
      });
    }
    wmi.end();
  }

  // Find the Disk Type of a Partition

  disk_type result = disk_type::Unspecified;

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

std::string vuapi join_path_A(
  const std::string& left, const std::string& right, const path_separator separator)
{
  const auto Sep = separator == path_separator::WIN ? '\\' : '/';
  return join_path_T<std::string, char>(left, right, Sep);
}

std::wstring vuapi join_path_W(
  const std::wstring& left, const std::wstring& right, const path_separator separator)
{
  const auto sep = separator == path_separator::WIN ? L'\\' : L'/';
  return join_path_T<std::wstring, wchar_t>(left, right, sep);
}

std::string normalize_path_A(const std::string& path, const path_separator separator)
{
  auto result = path;

  const std::string SEP_WIN = "\\";
  const std::string SEP_POSIX = "/";
  const std::string sep = separator == path_separator::WIN ? SEP_WIN : SEP_POSIX;

  result = replace_string_A(result, SEP_WIN + SEP_WIN, sep);
  result = replace_string_A(result, SEP_WIN, sep);
  result = replace_string_A(result, SEP_POSIX, sep);

  return result;
}

std::wstring normalize_path_W(const std::wstring& path, const path_separator separator)
{
  auto result = path;

  const std::wstring SEP_WIN = L"\\";
  const std::wstring SEP_POSIX = L"/";
  const std::wstring sep = separator == path_separator::WIN ? SEP_WIN : SEP_POSIX;

  result = replace_string_W(result, SEP_WIN + SEP_WIN, sep);
  result = replace_string_W(result, SEP_WIN, sep);
  result = replace_string_W(result, SEP_POSIX, sep);

  return result;
}

/**
 * PathA
 */

PathA::PathA(const path_separator separator) : m_path(""), m_separator(separator)
{
}

PathA::PathA(const std::string& path, const path_separator separator) : m_path(path), m_separator(separator)
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
    if (m_separator == path_separator::WIN)
    {
      result &= lower_string_A(m_path) == lower_string_A(right.m_path);
    }
    else if (m_separator == path_separator::POSIX)
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

vu::PathA& PathA::trim(const trim_type type)
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

vu::PathA PathA::extract_name(bool with_extension) const
{
  PathA tmp(*this);
  tmp.finalize();
  return vu::extract_file_name_A(tmp.as_string(), with_extension);
}

vu::PathA PathA::extract_directory(bool with_slash) const
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
 * PathW
 */

PathW::PathW(const path_separator Separator) : m_path(L""), m_separator(Separator)
{
}

PathW::PathW(const std::wstring& path, const path_separator separator) : m_path(path), m_separator(separator)
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
    if (m_separator == path_separator::WIN)
    {
      result &= lower_string_W(m_path) == lower_string_W(right.m_path);
    }
    else if (m_separator == path_separator::POSIX)
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

vu::PathW& PathW::trim(const trim_type type)
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

vu::PathW PathW::extract_name(bool with_extension) const
{
  PathW tmp(*this);
  tmp.finalize();
  return vu::extract_file_name_W(tmp.as_string(), with_extension);
}

vu::PathW PathW::extract_directory(bool with_slash) const
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

// @refer to https://blog.aaronballman.com/2011/08/how-to-check-access-rights/

bool check_path_permissions_W(const std::wstring& path, ulong generic_access_rights)
{
  bool  result = false;

  DWORD length = 0;
  const SECURITY_INFORMATION req_info =\
    OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;
  BOOL ret = GetFileSecurityW(path.c_str(), req_info, nullptr, 0, &length);
  if (ret == FALSE && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
  {
    auto ptr_security_descriptor = PSECURITY_DESCRIPTOR(::malloc(length));
    if (GetFileSecurityW(path.c_str(), req_info, ptr_security_descriptor, length, &length))
    {
      HANDLE token = nullptr;
      DWORD  desired_access =\
        TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_DUPLICATE | STANDARD_RIGHTS_READ;
      if (OpenProcessToken(::GetCurrentProcess(), desired_access, &token))
      {
        HANDLE impersonated_token = NULL;
        if (::DuplicateToken(token, SecurityImpersonation, &impersonated_token))
        {
          BOOL access_status = FALSE;
          PRIVILEGE_SET privileges = { 0 };
          DWORD grantedAccess = 0, privilegesLength = sizeof(privileges);
          GENERIC_MAPPING mapping = { 0xFFFFFFFF };
          mapping.GenericAll = FILE_ALL_ACCESS;
          mapping.GenericRead = FILE_GENERIC_READ;
          mapping.GenericWrite = FILE_GENERIC_WRITE;
          mapping.GenericExecute = FILE_GENERIC_EXECUTE;
          ::MapGenericMask(&generic_access_rights, &mapping);
          if (::AccessCheck(
            ptr_security_descriptor,
            impersonated_token,
            generic_access_rights,
            &mapping,
            &privileges,
            &privilegesLength,
            &grantedAccess,
            &access_status))
          {
            result = access_status != FALSE;
          }
          CloseHandle(impersonated_token);
        }
        CloseHandle(token);
      }
    }
    free(ptr_security_descriptor);
  }

  return result;
}

bool check_path_permissions_A(const std::string& path, ulong generic_access_rights)
{
  const auto s = to_string_W(path);
  return check_path_permissions_W(s, generic_access_rights);
}

} // namespace vu