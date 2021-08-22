/**
 * @file   filemap.cpp
 * @author Vic P.
 * @brief  Implementation for File Mapping
 */

#include "Vutils.h"

namespace vu
{

/**
 * FileMappingX
 */

FileMappingX::FileMappingX() : LastError()
{
  m_ptr_data = nullptr;
  m_map_handle = INVALID_HANDLE_VALUE;
  m_file_handle = INVALID_HANDLE_VALUE;
}

FileMappingX::~FileMappingX()
{
  this->close();
}

bool FileMappingX::valid(HANDLE handle)
{
  return (handle != nullptr && handle != INVALID_HANDLE_VALUE);
}

void* vuapi FileMappingX::view(
  eFMDesiredAccess desired_access,
  ulong max_file_offset_low,
  ulong max_file_offset_high,
  ulong number_of_bytes_to_map
)
{
  if (!this->valid(m_map_handle))
  {
    return nullptr;
  }

  m_ptr_data = MapViewOfFile(
    m_map_handle,
    desired_access,
    max_file_offset_high,
    max_file_offset_low,
    number_of_bytes_to_map
  );

  m_last_error_code = GetLastError();

  return m_ptr_data;
}

void vuapi FileMappingX::close()
{
  if (m_ptr_data != nullptr)
  {
    UnmapViewOfFile(m_ptr_data);
    m_ptr_data = nullptr;
  }

  if (this->valid(m_map_handle))
  {
    CloseHandle(m_map_handle);
    m_map_handle = INVALID_HANDLE_VALUE;
  }

  if (this->valid(m_file_handle))
  {
    CloseHandle(m_file_handle);
    m_file_handle = INVALID_HANDLE_VALUE;
  }
}

ulong vuapi FileMappingX::get_file_size()
{
  if (!this->valid(m_file_handle))
  {
    return INVALID_FILE_SIZE;
  }

  ulong result = ::GetFileSize(m_file_handle, nullptr);

  m_last_error_code = GetLastError();

  return result;
}

/**
 * FileMappingA
 */

FileMappingA::FileMappingA() : FileMappingX()
{
}

FileMappingA::~FileMappingA()
{
}

VUResult vuapi FileMappingA::create_within_file(
  const std::string& file_path,
  ulong max_size_low,
  ulong max_size_high,
  eFSGenericFlags fg_flags,
  eFSShareFlags fs_flags,
  eFSModeFlags fm_flags,
  eFSAttributeFlags fa_flags,
  ePageProtection pp_flags
)
{
  if (file_path.empty())
  {
    return 1;
  }

  m_file_handle = CreateFileA(file_path.c_str(), fg_flags, fs_flags, NULL, fm_flags, fa_flags, 0);

  m_last_error_code = GetLastError();

  if (!this->valid(m_file_handle))
  {
    return 2;
  }

  auto ret = create_named_shared_memory("", max_size_low, max_size_high, pp_flags);
  if (ret != VU_OK)
  {
    return ret;
  }

  return VU_OK;
}

VUResult vuapi FileMappingA::create_named_shared_memory(
  const std::string& mapping_name,
  ulong max_size_low,
  ulong max_size_high,
  ePageProtection pp_flags
)
{
  if (!this->valid(m_file_handle) && mapping_name.empty())
  {
    return 1;
  }

  m_map_handle = CreateFileMappingA(
    m_file_handle,
    nullptr,
    pp_flags,
    max_size_high,
    max_size_low,
    mapping_name.empty() ? nullptr : mapping_name.c_str()
  );

  m_last_error_code = GetLastError();

  if (!this->valid(m_map_handle))
  {
    return 2;
  }

  return VU_OK;
}

VUResult vuapi FileMappingA::open(
  const std::string& mapping_name,
  eFMDesiredAccess desired_access,
  bool inherit_handle
)
{
  if (mapping_name.empty())
  {
    return 1;
  }

  m_map_handle = OpenFileMappingA(desired_access, inherit_handle, mapping_name.c_str());

  m_last_error_code = GetLastError();

  if (!this->valid(m_map_handle))
  {
    return 2;
  }

  return VU_OK;
}

/**
 * FileMappingW
 */

FileMappingW::FileMappingW() : FileMappingX()
{
}

FileMappingW::~FileMappingW()
{
}

VUResult vuapi FileMappingW::create_within_file(
  const std::wstring& file_path,
  ulong max_size_low,
  ulong max_size_high,
  eFSGenericFlags fg_flags,
  eFSShareFlags fs_flags,
  eFSModeFlags fm_flags,
  eFSAttributeFlags fa_flags,
  ePageProtection pp_flags
)
{
  if (file_path.empty())
  {
    return 1;
  }

  m_file_handle = CreateFileW(file_path.c_str(), fg_flags, fs_flags, NULL, fm_flags, fa_flags, 0);

  m_last_error_code = GetLastError();

  if (!this->valid(m_file_handle))
  {
    return 2;
  }

  auto ret = create_named_shared_memory(L"", max_size_low, max_size_high, pp_flags);
  if (ret != VU_OK)
  {
    return ret;
  }

  return VU_OK;
}

VUResult vuapi FileMappingW::create_named_shared_memory(
  const std::wstring& mapping_name,
  ulong max_size_low,
  ulong max_size_high,
  ePageProtection pp_flags
)
{
  if (!this->valid(m_file_handle) && mapping_name.empty())
  {
    return 1;
  }

  m_map_handle = CreateFileMappingW(
    m_file_handle,
    nullptr,
    pp_flags,
    max_size_high,
    max_size_low,
    mapping_name.empty() ? nullptr : mapping_name.c_str()
  );

  m_last_error_code = GetLastError();

  if (!this->valid(m_map_handle))
  {
    return 2;
  }

  return VU_OK;
}

VUResult vuapi FileMappingW::open(
  const std::wstring& mapping_name,
  eFMDesiredAccess desired_access,
  bool inherit_handle
)
{
  if (mapping_name.empty())
  {
    return 1;
  }

  m_map_handle = OpenFileMappingW(desired_access, inherit_handle, mapping_name.c_str());

  m_last_error_code = GetLastError();

  if (!this->valid(m_map_handle))
  {
    return 2;
  }

  return VU_OK;
}

} // namespace vu