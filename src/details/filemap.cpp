/**
 * @file   filemap.cpp
 * @author Vic P.
 * @brief  Implementation for File Mapping
 */

#include "Vutils.h"

namespace vu
{

/**
 * CFileMappingX
 */

CFileMappingX::CFileMappingX() : CLastError()
{
  m_pData = nullptr;
  m_MapHandle  = INVALID_HANDLE_VALUE;
  m_FileHandle = INVALID_HANDLE_VALUE;
}

CFileMappingX::~CFileMappingX()
{
  this->Close();
}

bool CFileMappingX::IsValidHandle(HANDLE Handle)
{
  return (Handle != nullptr && Handle != INVALID_HANDLE_VALUE);
}

void* vuapi CFileMappingX::View(
  ulong ulDesiredAccess,
  ulong ulMaxFileOffsetLow,
  ulong ulMaxFileOffsetHigh,
  ulong ulNumberOfBytesToMap
)
{
  if (!this->IsValidHandle(m_MapHandle))
  {
    return nullptr;
  }

  m_pData = MapViewOfFile(
    m_MapHandle,
    ulDesiredAccess,
    ulMaxFileOffsetHigh,
    ulMaxFileOffsetLow,
    ulNumberOfBytesToMap
  );

  m_LastErrorCode = GetLastError();

  return m_pData;
}

void vuapi CFileMappingX::Close()
{
  if (m_pData != nullptr)
  {
    UnmapViewOfFile(m_pData);
    m_pData = nullptr;
  }

  if (this->IsValidHandle(m_MapHandle))
  {
    CloseHandle(m_MapHandle);
    m_MapHandle = INVALID_HANDLE_VALUE;
  }

  if (this->IsValidHandle(m_FileHandle))
  {
    CloseHandle(m_FileHandle);
    m_FileHandle = INVALID_HANDLE_VALUE;
  }
}

ulong vuapi CFileMappingX::GetFileSize()
{
  if (!this->IsValidHandle(m_FileHandle))
  {
    return INVALID_FILE_SIZE;
  }

  ulong result = ::GetFileSize(m_FileHandle, nullptr);

  m_LastErrorCode = GetLastError();

  return result;
}

/**
 * CFileMappingA
 */

CFileMappingA::CFileMappingA() : CFileMappingX()
{
}

CFileMappingA::~CFileMappingA()
{
}

VUResult vuapi CFileMappingA::CreateWithinFile(
  const std::string& FileName,
  ulong ulMaxSizeLow,
  ulong ulMaxSizeHigh,
  eFSGenericFlags fgFlag,
  eFSShareFlags fsFlag,
  eFSModeFlags fmFlag,
  eFSAttributeFlags faFlag
)
{
  if (FileName.empty())
  {
    return 1;
  }

  m_FileHandle = CreateFileA(FileName.c_str(), fgFlag, fsFlag, NULL, fmFlag, faFlag, 0);

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_FileHandle))
  {
    return 2;
  }

  auto ret = CreateNamedSharedMemory("", ulMaxSizeLow, ulMaxSizeHigh);
  if (ret != VU_OK)
  {
    return ret;
  }

  return VU_OK;
}

VUResult vuapi CFileMappingA::CreateNamedSharedMemory(
  const std::string& MappingName,
  ulong ulMaxSizeLow,
  ulong ulMaxSizeHigh,
  ulong ulProtect
)
{
  if (!this->IsValidHandle(m_FileHandle) && MappingName.empty())
  {
    return 1;
  }

  m_MapHandle = CreateFileMappingA(
    m_FileHandle,
    nullptr,
    ulProtect,
    ulMaxSizeHigh,
    ulMaxSizeLow,
    MappingName.empty() ? nullptr : MappingName.c_str()
  );

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 2;
  }

  return VU_OK;
}

VUResult vuapi CFileMappingA::Open(
  const std::string& MappingName,
  ulong ulDesiredAccess,
  bool bInheritHandle
)
{
  if (MappingName.empty())
  {
    return 1;
  }

  m_MapHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, bInheritHandle, MappingName.c_str());

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 2;
  }

  return VU_OK;
}

/**
 * CFileMappingW
 */

CFileMappingW::CFileMappingW() : CFileMappingX()
{
}

CFileMappingW::~CFileMappingW()
{
}

VUResult vuapi CFileMappingW::CreateWithinFile(
  const std::wstring& FileName,
  ulong ulMaxSizeLow,
  ulong ulMaxSizeHigh,
  eFSGenericFlags fgFlag,
  eFSShareFlags fsFlag,
  eFSModeFlags fmFlag,
  eFSAttributeFlags faFlag
)
{
  if (FileName.empty())
  {
    return 1;
  }

  m_FileHandle = CreateFileW(FileName.c_str(), fgFlag, fsFlag, NULL, fmFlag, faFlag, 0);

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_FileHandle))
  {
    return 2;
  }

  auto ret = CreateNamedSharedMemory(L"", ulMaxSizeLow, ulMaxSizeHigh);
  if (ret != VU_OK)
  {
    return ret;
  }

  return VU_OK;
}

VUResult vuapi CFileMappingW::CreateNamedSharedMemory(
  const std::wstring& MappingName,
  ulong ulMaxSizeLow,
  ulong ulMaxSizeHigh,
  ulong ulProtect
)
{
  if (!this->IsValidHandle(m_FileHandle) && MappingName.empty())
  {
    return 1;
  }

  m_MapHandle = CreateFileMappingW(
    m_FileHandle,
    nullptr,
    ulProtect,
    ulMaxSizeHigh,
    ulMaxSizeLow,
    MappingName.empty() ? nullptr : MappingName.c_str()
  );

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 2;
  }

  return VU_OK;
}

VUResult vuapi CFileMappingW::Open(
  const std::wstring& MappingName,
  ulong ulDesiredAccess,
  bool bInheritHandle
)
{
  if (MappingName.empty())
  {
    return 1;
  }

  m_MapHandle = OpenFileMappingW(FILE_MAP_ALL_ACCESS, bInheritHandle, MappingName.c_str());

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 2;
  }

  return VU_OK;
}

} // namespace vu