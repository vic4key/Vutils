/**
 * @file   filesys.cpp
 * @author Vic P.
 * @brief  Implementation for File System
 */

#include "Vutils.h"

#include <cassert>

namespace vu
{

CFileSystemX::CFileSystemX() : CLastError()
{
  m_read_size  = 0;
  m_wrote_size = 0;
  m_handle = nullptr;
}

CFileSystemX::~CFileSystemX()
{
  this->close();
}

bool vuapi CFileSystemX::valid(HANDLE handle)
{
  if (handle == nullptr || handle == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  return true;
}

bool vuapi CFileSystemX::ready()
{
  return this->valid(m_handle);
}

bool vuapi CFileSystemX::read(ulong offset, void* ptr_buffer, ulong size, eMoveMethodFlags flags)
{
  if (!this->seek(offset, flags))
  {
    return false;
  }

  BOOL result = ReadFile(m_handle, ptr_buffer, size, (LPDWORD)&m_read_size, NULL);
  if (!result && size != m_read_size)
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileSystemX::read(void* ptr_buffer, ulong size)
{
  BOOL result = ReadFile(m_handle, ptr_buffer, size, (LPDWORD)&m_read_size, NULL);
  if (!result && size != m_read_size)
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileSystemX::write(ulong offset, const void* ptr_buffer, ulong size, eMoveMethodFlags flags)
{
  if (!this->seek(offset, flags)) return false;

  BOOL result = WriteFile(m_handle, ptr_buffer, size, (LPDWORD)&m_wrote_size, NULL);
  if (!result && size != m_wrote_size)
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileSystemX::write(const void* ptr_buffer, ulong size)
{
  BOOL result = WriteFile(m_handle, ptr_buffer, size, (LPDWORD)&m_wrote_size, NULL);
  if (!result && size != m_wrote_size)
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileSystemX::seek(ulong offset, eMoveMethodFlags flags)
{
  if (!this->valid(m_handle))
  {
    return false;
  }

  ulong result = SetFilePointer(m_handle, offset, NULL, flags);

  m_last_error_code = GetLastError();

  return (result != INVALID_SET_FILE_POINTER);
}

ulong vuapi CFileSystemX::get_file_size()
{
  if (!this->valid(m_handle))
  {
    return 0;
  }

  ulong result = ::GetFileSize(m_handle, NULL);

  m_last_error_code = GetLastError();

  return result;
}

bool vuapi CFileSystemX::io_control(ulong code, void* ptr_send_buffer, ulong send_size, void* ptr_recv_buffer, ulong recv_size)
{
  ulong return_length = 0;

  bool result = DeviceIoControl(
    m_handle,
    code,
    ptr_send_buffer,
    send_size,
    ptr_recv_buffer,
    recv_size,
    &return_length,
    NULL
  ) != FALSE;

  m_last_error_code = GetLastError();

  return result;
}

bool vuapi CFileSystemX::close()
{
  if (!this->valid(m_handle))
  {
    return false;
  }

  if (!CloseHandle(m_handle))
  {
    return false;
  }

  m_handle = INVALID_HANDLE_VALUE;

  return true;
}

const CBuffer vuapi CFileSystemX::read_as_buffer()
{
  CBuffer buffer(0);

  auto size = this->get_file_size();
  if (size == 0)
  {
    return buffer;
  }

  buffer.resize(size);

  this->read(0, buffer.get_ptr_data(), size, eMoveMethodFlags::MM_BEGIN);

  return buffer;
}

// A

CFileSystemA::CFileSystemA() : CFileSystemX()
{
}

CFileSystemA::CFileSystemA(const std::string& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags, eFSShareFlags fs_flags, eFSAttributeFlags fa_flags) : CFileSystemX()
{
  this->initialize(file_path, fm_flags, fg_flags, fs_flags, fa_flags);
}

CFileSystemA::~CFileSystemA()
{
}

bool vuapi CFileSystemA::initialize(const std::string& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags, eFSShareFlags fs_flags, eFSAttributeFlags fa_flags)
{
  m_handle = CreateFileA(file_path.c_str(), fg_flags, fs_flags, NULL, fm_flags, fa_flags, NULL);
  if (!this->valid(m_handle))
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

const std::string vuapi CFileSystemA::read_as_string(bool remove_bom)
{
  std::string result("");

  auto buffer = this->read_as_buffer();
  auto p = (char*)buffer.get_ptr_data();

  auto encoding = determine_encoding_type(buffer.get_ptr_data(), buffer.get_size());
  if (encoding == eEncodingType::ET_UNKNOWN)
  {
    assert(0);
    return result;
  }

  if (remove_bom && encoding == eEncodingType::ET_UTF8_BOM)
  {
    p += 3; /* remove BOM */
  }

  result.assign(p);

  return result;
}

const std::string vuapi CFileSystemA::quick_read_as_string(const std::string& file_path, bool remove_bom)
{
  CFileSystemA file(file_path, vu::eFSModeFlags::FM_OPENEXISTING);
  auto result = file.read_as_string(remove_bom);
  return result;
}

CBuffer CFileSystemA::quick_read_as_buffer(const std::string& file_path)
{
  if (!is_file_exists_A(file_path))
  {
    return CBuffer();
  }

  CFileSystemA fs(file_path, eFSModeFlags::FM_OPENEXISTING);
  return fs.read_as_buffer();
}

bool CFileSystemA::iterate(const std::string& path, const std::string& pattern, const std::function<bool(const TFSObjectA& fso)> fn_callback)
{
  auto the_path = vu::trim_string_A(path);
  if (the_path.empty())
  {
    return false;
  }

  if (the_path.back() != '\\' && the_path.back() != '/')
  {
    the_path += "\\";
  }

  auto the_path_slash = the_path + pattern;

  WIN32_FIND_DATAA wfd = { 0 };

  auto hFind = FindFirstFileA(the_path_slash.c_str(), &wfd);
  if (INVALID_HANDLE_VALUE == hFind)
  {
    return false;
  }

  TFSObjectA file_object;
  file_object.directory = the_path;

  LARGE_INTEGER file_size = { 0 };

  do
  {
    if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      file_size.LowPart  = 0;
      file_size.HighPart = 0;
    }
    else
    {
      file_size.LowPart  = wfd.nFileSizeLow;
      file_size.HighPart = wfd.nFileSizeHigh;
    }

    file_object.size = file_size.QuadPart;
    file_object.attributes = wfd.dwFileAttributes;
    file_object.name = wfd.cFileName;

    if (!fn_callback(file_object))
    {
      break;
    }
  } while (FindNextFileA(hFind, &wfd) != FALSE);

  FindClose(hFind);

  return true;
}

// W

CFileSystemW::CFileSystemW() : CFileSystemX()
{
}

CFileSystemW::CFileSystemW(const std::wstring& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags, eFSShareFlags fs_flags, eFSAttributeFlags fa_flags) : CFileSystemX()
{
  this->initialize(file_path, fm_flags, fg_flags, fs_flags, fa_flags);
}

CFileSystemW::~CFileSystemW()
{
}

bool vuapi CFileSystemW::initialize(const std::wstring& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags, eFSShareFlags fs_flags, eFSAttributeFlags fa_flags)
{
  m_handle = CreateFileW(file_path.c_str(), fg_flags, fs_flags, NULL, fm_flags, fa_flags, NULL);
  if (!this->valid(m_handle))
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

const std::wstring vuapi CFileSystemW::read_as_string(bool remove_bom)
{
  std::wstring result(L"");

  auto buffer = this->read_as_buffer();
  auto p = (wchar*)buffer.get_ptr_data();

  auto encoding = determine_encoding_type(buffer.get_ptr_data(), buffer.get_size());
  if (encoding == eEncodingType::ET_UNKNOWN)
  {
    assert(0);
    return result;
  }

  if (remove_bom && (encoding == eEncodingType::ET_UTF16_LE_BOM || encoding == eEncodingType::ET_UTF16_BE_BOM))
  {
    p = (wchar*)((char*)buffer.get_ptr_data() + 2); /* remove BOM */
  }

  result.assign(p);

  return result;
}

const std::wstring vuapi CFileSystemW::quick_read_as_string(const std::wstring& FilePath, bool removeBOM)
{
  CFileSystemW file(FilePath, vu::eFSModeFlags::FM_OPENEXISTING);
  auto result = file.read_as_string(removeBOM);
  return result;
}

CBuffer CFileSystemW::quick_read_as_buffer(const std::wstring& FilePath)
{
  if (!is_file_exists_W(FilePath))
  {
    return CBuffer();
  }

  CFileSystemW fs(FilePath, eFSModeFlags::FM_OPENEXISTING);
  return fs.read_as_buffer();
}

bool CFileSystemW::iterate(const std::wstring& path, const std::wstring& pattern, const std::function<bool(const TFSObjectW& FSObject)> fn_callback)
{
  auto the_path = vu::trim_string_W(path);
  if (the_path.empty())
  {
    return false;
  }

  if (the_path.back() != L'\\' && the_path.back() != L'/')
  {
    the_path += L"\\";
  }

  auto the_path_pattern = the_path + pattern;

  WIN32_FIND_DATAW wfd = { 0 };

  auto h_find = FindFirstFileW(the_path_pattern.c_str(), &wfd);
  if (INVALID_HANDLE_VALUE == h_find)
  {
    return false;
  }

  TFSObjectW file_object;
  file_object.directory = the_path;

  LARGE_INTEGER file_size = { 0 };

  do
  {
    if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      file_size.LowPart  = 0;
      file_size.HighPart = 0;
    }
    else
    {
      file_size.LowPart  = wfd.nFileSizeLow;
      file_size.HighPart = wfd.nFileSizeHigh;
    }

    file_object.size = file_size.QuadPart;
    file_object.attributes = wfd.dwFileAttributes;
    file_object.name = wfd.cFileName;
    fn_callback(file_object);
  }
  while (FindNextFileW(h_find, &wfd) != FALSE);

  FindClose(h_find);

  return true;
}

} // namespace vu