/**
 * @file   filemap.cpp
 * @author Vic P.
 * @brief  Implementation for File Mapping
 */

#include "Vutils.h"

namespace vu
{

CFileMappingA::CFileMappingA()
{
  m_HasInit = false;
  m_MapFile = false;

  m_pData = nullptr;
  m_MapHandle  = INVALID_HANDLE_VALUE;
  m_FileHandle = INVALID_HANDLE_VALUE;

  m_LastErrorCode = ERROR_SUCCESS;
}

CFileMappingA::~CFileMappingA()
{
  this->Close();
}

bool CFileMappingA::IsValidHandle(HANDLE Handle)
{
  return (Handle != nullptr && Handle != INVALID_HANDLE_VALUE);
}

VUResult vuapi CFileMappingA::Init(
  bool bMapFile,
  const std::string& FileName,
  eFSGenericFlags fgFlag,
  eFSShareFlags fsFlag,
  eFSModeFlags fmFlag,
  eFSAttributeFlags faFlag
)
{
  if ((m_MapFile = bMapFile))
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
  }

  m_HasInit = true;

  return VU_OK;
}

VUResult vuapi CFileMappingA::Create(const std::string& MapName, ulong ulMaxSizeLow, ulong ulMaxSizeHigh)
{
  if (!m_MapFile)
  {
    if (MapName.empty())
    {
      return 1;
    }
  }

  if (!m_HasInit || (m_MapFile && !this->IsValidHandle(m_FileHandle)))
  {
    return 2;
  }

  m_MapHandle = CreateFileMappingA(
    m_FileHandle,
    NULL,
    PAGE_READWRITE,
    ulMaxSizeHigh,
    ulMaxSizeLow,
    (m_MapFile ? NULL : MapName.c_str())
  );

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 3;
  }

  return VU_OK;
}

VUResult vuapi CFileMappingA::Open(const std::string& MapName, bool bInheritHandle)
{
  if (!m_MapFile)
  {
    return 1;
  }

  if (MapName.empty())
  {
    return 2;
  }

  m_MapHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, bInheritHandle, MapName.c_str());

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 3;
  }

  return VU_OK;
}

void* vuapi CFileMappingA::View(
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
    FILE_MAP_ALL_ACCESS,
    ulMaxFileOffsetHigh,
    ulMaxFileOffsetLow,
    ulNumberOfBytesToMap
  );

  m_LastErrorCode = GetLastError();

  return m_pData;
}

void vuapi CFileMappingA::Close()
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

  if (m_MapFile && this->IsValidHandle(m_FileHandle))
  {
    CloseHandle(m_FileHandle);
    m_FileHandle = INVALID_HANDLE_VALUE;
  }
}

ulong vuapi CFileMappingA::GetFileSize()
{
  if (m_MapFile && !this->IsValidHandle(m_FileHandle))
  {
    return INVALID_FILE_SIZE;
  }

  ulong result = ::GetFileSize(m_FileHandle, NULL);

  m_LastErrorCode = GetLastError();

  return result;
}

CFileMappingW::CFileMappingW()
{
  m_HasInit = false;
  m_MapFile = false;

  m_pData = nullptr;
  m_MapHandle  = INVALID_HANDLE_VALUE;
  m_FileHandle = INVALID_HANDLE_VALUE;

  m_LastErrorCode = ERROR_SUCCESS;
}

CFileMappingW::~CFileMappingW()
{
  this->Close();
}

bool CFileMappingW::IsValidHandle(HANDLE Handle)
{
  return (Handle != nullptr && Handle != INVALID_HANDLE_VALUE);
}

VUResult vuapi CFileMappingW::Init(
  bool bMapFile,
  const std::wstring FileName,
  eFSGenericFlags fgFlag,
  eFSShareFlags fsFlag,
  eFSModeFlags fmFlag,
  eFSAttributeFlags faFlag
)
{
  if ((m_MapFile = bMapFile))
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
  }

  m_HasInit = true;

  return VU_OK;
}

VUResult vuapi CFileMappingW::Create(const std::wstring& MapName, ulong ulMaxSizeLow, ulong ulMaxSizeHigh)
{
  if (!m_MapFile)
  {
    if (MapName.empty())
    {
      return 1;
    }
  }

  if (!m_HasInit || (m_MapFile && !this->IsValidHandle(m_FileHandle)))
  {
    return 2;
  }

  m_MapHandle = CreateFileMappingW(
    m_FileHandle,
    NULL,
    PAGE_READWRITE,
    ulMaxSizeHigh,
    ulMaxSizeLow,
    (m_MapFile ? NULL : MapName.c_str())
  );

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 3;
  }

  return VU_OK;
}

VUResult vuapi CFileMappingW::Open(const std::wstring& MapName, bool bInheritHandle)
{
  if (!m_MapFile)
  {
    return 1;
  }

  if (MapName.empty())
  {
    return 2;
  }

  m_MapHandle = OpenFileMappingW(FILE_MAP_ALL_ACCESS, bInheritHandle, MapName.c_str());

  m_LastErrorCode = GetLastError();

  if (!this->IsValidHandle(m_MapHandle))
  {
    return 3;
  }

  return VU_OK;
}

void* vuapi CFileMappingW::View(
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
    FILE_MAP_ALL_ACCESS,
    ulMaxFileOffsetHigh,
    ulMaxFileOffsetLow,
    ulNumberOfBytesToMap
  );

  m_LastErrorCode = GetLastError();

  return m_pData;
}

void vuapi CFileMappingW::Close()
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

  if (m_MapFile && this->IsValidHandle(m_FileHandle))
  {
    CloseHandle(m_FileHandle);
    m_FileHandle = INVALID_HANDLE_VALUE;
  }
}

ulong vuapi CFileMappingW::GetFileSize()
{
  if (m_MapFile && !this->IsValidHandle(m_FileHandle))
  {
    return INVALID_FILE_SIZE;
  }

  ulong result = ::GetFileSize(m_FileHandle, NULL);

  m_LastErrorCode = GetLastError();

  return result;
}

} // namespace vu