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

bool vuapi FileSystemX::read(ulong offset, void* ptr_buffer, ulong size, fs_position_at flags)
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

bool vuapi FileSystemX::write(ulong offset, const void* ptr_buffer, ulong size, fs_position_at flags)
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

bool vuapi FileSystemX::seek(ulong offset, fs_position_at flags)
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

bool vuapi FileSystemX::io_control(
  ulong code, void* ptr_send_buffer, ulong send_size, void* ptr_recv_buffer, ulong recv_size)
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

std::unique_ptr<Buffer> vuapi FileSystemX::read_as_buffer()
{
  auto size = this->get_file_size();
  if (size == 0)
  {
    return nullptr;
  }

  std::unique_ptr<Buffer> buffer(new Buffer(size));

  this->read(0, buffer->pointer(), size, fs_position_at::PA_BEGIN);

  return buffer;
}

// A

FileSystemA::FileSystemA() : FileSystemX()
{
}

FileSystemA::FileSystemA(
  const std::string& file_path,
  fs_mode fm_flags,
  fs_generic fg_flags,
  fs_share fs_flags,
  fs_attribute fa_flags)
  : FileSystemX()
{
  this->initialize(file_path, fm_flags, fg_flags, fs_flags, fa_flags);
}

FileSystemA::~FileSystemA()
{
}

bool vuapi FileSystemA::initialize(
  const std::string& file_path,
  fs_mode fm_flags,
  fs_generic fg_flags,
  fs_share fs_flags,
  fs_attribute fa_flags)
{
  m_handle = CreateFileA(file_path.c_str(), fg_flags, fs_flags, NULL, fm_flags, fa_flags, NULL);
  if (!this->valid(m_handle))
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

std::unique_ptr<std::string> vuapi FileSystemA::read_as_text()
{
  auto buffer = this->read_as_buffer();
  if (buffer == nullptr)
  {
    return nullptr;
  }

  auto text_encoding = detect_text_encoding(buffer->pointer(), buffer->size());
  if (text_encoding == text_encoding::TE_UNKNOWN)
  {
    return nullptr;
  }

  std::unique_ptr<std::string> result(new std::string);

  if (text_encoding == text_encoding::TE_UTF16_LE ||
      text_encoding == text_encoding::TE_UTF16_BE ||
      text_encoding == text_encoding::TE_UTF16_LE_BOM ||
      text_encoding == text_encoding::TE_UTF16_BE_BOM)
  {
    std::wstring tmp((wchar*)(buffer->bytes() + 2), (buffer->size() - 2) / 2);
    result->assign(std::move(to_string_A(tmp, true))); // remove bom
  }
  else if (text_encoding == text_encoding::TE_UTF8)
  {
    std::string tmp((char*)buffer->bytes(), buffer->size());
    result->assign(std::move(tmp));
  }
  else if (text_encoding == text_encoding::TE_UTF8_BOM)
  {
    std::string tmp((char*)buffer->bytes() + 3, buffer->size() - 3); // remove bom
    result->assign(std::move(tmp));
  }
  else
  {
    assert(0 && "the text encoding is not yet implemented to read");
  }

  return result;
}

std::unique_ptr<std::string> vuapi FileSystemA::quick_read_as_text(const std::string& file_path)
{
  FileSystemA file(file_path, vu::fs_mode::FM_OPENEXISTING, fs_generic::FG_READ, fs_share::FS_READ);
  return file.read_as_text();
}

std::unique_ptr<Buffer> FileSystemA::quick_read_as_buffer(const std::string& file_path)
{
  if (!is_file_exists_A(file_path))
  {
    return nullptr;
  }

  FileSystemA fs(file_path, fs_mode::FM_OPENEXISTING, fs_generic::FG_READ, fs_share::FS_READ);
  return fs.read_as_buffer();
}

bool FileSystemA::iterate(
  const std::string& path,
  const std::string& pattern,
  const std::function<bool(const FSObjectA& fso)> fn_callback)
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

  FSObjectA file_object;
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

FileSystemW::FileSystemW(
  const std::wstring& file_path,
  fs_mode fm_flags,
  fs_generic fg_flags,
  fs_share fs_flags,
  fs_attribute fa_flags)
  : FileSystemX()
{
  this->initialize(file_path, fm_flags, fg_flags, fs_flags, fa_flags);
}

FileSystemW::~FileSystemW()
{
}

bool vuapi FileSystemW::initialize(
  const std::wstring& file_path,
  fs_mode fm_flags,
  fs_generic fg_flags,
  fs_share fs_flags,
  fs_attribute fa_flags)
{
  m_handle = CreateFileW(file_path.c_str(), fg_flags, fs_flags, NULL, fm_flags, fa_flags, NULL);
  if (!this->valid(m_handle))
  {
    m_last_error_code = GetLastError();
    return false;
  }

  return true;
}

std::unique_ptr<std::wstring> vuapi FileSystemW::read_as_text()
{
  auto buffer = this->read_as_buffer();
  if (buffer == nullptr)
  {
    return nullptr;
  }

  auto text_encoding = detect_text_encoding(buffer->pointer(), buffer->size());
  if (text_encoding == text_encoding::TE_UNKNOWN)
  {
    return nullptr;
  }

  std::unique_ptr<std::wstring> result(new std::wstring);

  if (text_encoding == text_encoding::TE_UTF16_LE ||
      text_encoding == text_encoding::TE_UTF16_BE ||
      text_encoding == text_encoding::TE_UTF16_LE_BOM ||
      text_encoding == text_encoding::TE_UTF16_BE_BOM)
  {
    std::wstring tmp((wchar*)(buffer->bytes() + 2), (buffer->size() - 2) / 2);
    result->assign(std::move(tmp)); // remove bom
  }
  else if (text_encoding == text_encoding::TE_UTF8)
  {
    std::string tmp((char*)buffer->bytes(), buffer->size());
    result->assign(std::move(to_string_W(tmp, true)));
  }
  else if (text_encoding == text_encoding::TE_UTF8_BOM)
  {
    std::string tmp((char*)buffer->bytes() + 3, buffer->size() - 3); // remove bom
    result->assign(std::move(to_string_W(tmp, true)));
  }
  else
  {
    assert(0 && "the text encoding is not yet implemented to read");
  }

  return result;
}

std::unique_ptr<std::wstring> vuapi FileSystemW::quick_read_as_text(const std::wstring& file_path)
{
  FileSystemW file(file_path, vu::fs_mode::FM_OPENEXISTING, fs_generic::FG_READ, fs_share::FS_READ);
  return file.read_as_text();
}

std::unique_ptr<Buffer> FileSystemW::quick_read_as_buffer(const std::wstring& file_path)
{
  if (!is_file_exists_W(file_path))
  {
    return nullptr;
  }

  FileSystemW fs(file_path, fs_mode::FM_OPENEXISTING, fs_generic::FG_READ, fs_share::FS_READ);
  return fs.read_as_buffer();
}

bool FileSystemW::iterate(
  const std::wstring& path,
  const std::wstring& pattern,
  const std::function<bool(const FSObjectW& FSObject)> fn_callback)
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

  FSObjectW file_object;
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