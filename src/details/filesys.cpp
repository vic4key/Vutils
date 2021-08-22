/**
 * @file   filesys.cpp
 * @author Vic P.
 * @brief  Implementation for File System
 */

#include "Vutils.h"

#include <cassert>

namespace vu
{

FileSystemX::FileSystemX() : LastError()
{
  m_read_size  = 0;
  m_wrote_size = 0;
  m_handle = nullptr;
}

FileSystemX::~FileSystemX()
{
  this->close();
}

bool vuapi FileSystemX::valid(HANDLE handle)
{
  if (handle == nullptr || handle == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  return true;
}

bool vuapi FileSystemX::ready()
{
  return this->valid(m_handle);
}

bool vuapi FileSystemX::read(ulong offset, void* ptr_buffer, ulong size, eMoveMethodFlags flags)
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

bool vuapi FileSystemX::read(void* ptr_buffer, ulong size)
{
  BOOL result = ReadFile(m_handle, ptr_buffer, size, (LPDWORD)&m_read_size, NULL);
  if (!result && size != m_read_size)
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

bool vuapi FileSystemX::write(ulong offset, const void* ptr_buffer, ulong size, eMoveMethodFlags flags)
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

bool vuapi FileSystemX::write(const void* ptr_buffer, ulong size)
{
  BOOL result = WriteFile(m_handle, ptr_buffer, size, (LPDWORD)&m_wrote_size, NULL);
  if (!result && size != m_wrote_size)
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

bool vuapi FileSystemX::seek(ulong offset, eMoveMethodFlags flags)
{
  if (!this->valid(m_handle))
  {
    return false;
  }

  ulong result = SetFilePointer(m_handle, offset, NULL, flags);

  m_last_error_code = GetLastError();

  return (result != INVALID_SET_FILE_POINTER);
}

ulong vuapi FileSystemX::get_file_size()
{
  if (!this->valid(m_handle))
  {
    return 0;
  }

  ulong result = ::GetFileSize(m_handle, NULL);

  m_last_error_code = GetLastError();

  return result;
}

bool vuapi FileSystemX::io_control(ulong code, void* ptr_send_buffer, ulong send_size, void* ptr_recv_buffer, ulong recv_size)
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

bool vuapi FileSystemX::close()
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

const Buffer vuapi FileSystemX::read_as_buffer()
{
  Buffer buffer(0);

  auto size = this->get_file_size();
  if (size == 0)
  {
    return buffer;
  }

  buffer.resize(size);

  this->read(0, buffer.get_ptr(), size, eMoveMethodFlags::MM_BEGIN);

  return buffer;
}

// A

FileSystemA::FileSystemA() : FileSystemX()
{
}

FileSystemA::FileSystemA(const std::string& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags, eFSShareFlags fs_flags, eFSAttributeFlags fa_flags) : FileSystemX()
{
  this->initialize(file_path, fm_flags, fg_flags, fs_flags, fa_flags);
}

FileSystemA::~FileSystemA()
{
}

bool vuapi FileSystemA::initialize(const std::string& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags, eFSShareFlags fs_flags, eFSAttributeFlags fa_flags)
{
  m_handle = CreateFileA(file_path.c_str(), fg_flags, fs_flags, NULL, fm_flags, fa_flags, NULL);
  if (!this->valid(m_handle))
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

const std::string vuapi FileSystemA::read_as_string(bool remove_bom)
{
  std::string result("");

  auto buffer = this->read_as_buffer();
  auto p = (char*)buffer.get_ptr();

  auto encoding = determine_encoding_type(buffer.get_ptr(), buffer.get_size());
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

const std::string vuapi FileSystemA::quick_read_as_string(const std::string& file_path, bool remove_bom)
{
  FileSystemA file(file_path, vu::eFSModeFlags::FM_OPENEXISTING, eFSGenericFlags::FG_READ, eFSShareFlags::FS_READ);
  auto result = file.read_as_string(remove_bom);
  return result;
}

Buffer FileSystemA::quick_read_as_buffer(const std::string& file_path)
{
  if (!is_file_exists_A(file_path))
  {
    return Buffer();
  }

  FileSystemA fs(file_path, eFSModeFlags::FM_OPENEXISTING, eFSGenericFlags::FG_READ, eFSShareFlags::FS_READ);
  return fs.read_as_buffer();
}

bool FileSystemA::iterate(const std::string& path, const std::string& pattern, const std::function<bool(const sFSObjectA& fso)> fn_callback)
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

  sFSObjectA file_object;
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

FileSystemW::FileSystemW() : FileSystemX()
{
}

FileSystemW::FileSystemW(const std::wstring& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags, eFSShareFlags fs_flags, eFSAttributeFlags fa_flags) : FileSystemX()
{
  this->initialize(file_path, fm_flags, fg_flags, fs_flags, fa_flags);
}

FileSystemW::~FileSystemW()
{
}

bool vuapi FileSystemW::initialize(const std::wstring& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags, eFSShareFlags fs_flags, eFSAttributeFlags fa_flags)
{
  m_handle = CreateFileW(file_path.c_str(), fg_flags, fs_flags, NULL, fm_flags, fa_flags, NULL);
  if (!this->valid(m_handle))
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

const std::wstring vuapi FileSystemW::read_as_string(bool remove_bom)
{
  std::wstring result(L"");

  auto buffer = this->read_as_buffer();
  auto p = (wchar*)buffer.get_ptr();

  auto encoding = determine_encoding_type(buffer.get_ptr(), buffer.get_size());
  if (encoding == eEncodingType::ET_UNKNOWN)
  {
    assert(0);
    return result;
  }

  if (remove_bom && (encoding == eEncodingType::ET_UTF16_LE_BOM || encoding == eEncodingType::ET_UTF16_BE_BOM))
  {
    p = (wchar*)((char*)buffer.get_ptr() + 2); /* remove BOM */
  }

  result.assign(p);

  return result;
}

const std::wstring vuapi FileSystemW::quick_read_as_string(const std::wstring& file_path, bool remove_bom)
{
  FileSystemW file(file_path, vu::eFSModeFlags::FM_OPENEXISTING, eFSGenericFlags::FG_READ, eFSShareFlags::FS_READ);
  auto result = file.read_as_string(remove_bom);
  return result;
}

Buffer FileSystemW::quick_read_as_buffer(const std::wstring& file_path)
{
  if (!is_file_exists_W(file_path))
  {
    return Buffer();
  }

  FileSystemW fs(file_path, eFSModeFlags::FM_OPENEXISTING, eFSGenericFlags::FG_READ, eFSShareFlags::FS_READ);
  return fs.read_as_buffer();
}

bool FileSystemW::iterate(const std::wstring& path, const std::wstring& pattern, const std::function<bool(const sFSObjectW& FSObject)> fn_callback)
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

  auto hfind = FindFirstFileW(the_path_pattern.c_str(), &wfd);
  if (INVALID_HANDLE_VALUE == hfind)
  {
    return false;
  }

  sFSObjectW file_object;
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
  while (FindNextFileW(hfind, &wfd) != FALSE);

  FindClose(hfind);

  return true;
}

} // namespace vu