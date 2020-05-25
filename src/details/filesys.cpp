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
  m_ReadSize = 0;
  m_WroteSize = 0;
  m_FileHandle = nullptr;
}

CFileSystemX::~CFileSystemX()
{
  this->Close();
}

bool vuapi CFileSystemX::IsFileHandleValid(HANDLE fileHandle)
{
  if (!fileHandle || fileHandle == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  return true;
}

bool vuapi CFileSystemX::IsReady()
{
  return this->IsFileHandleValid(m_FileHandle);
}

bool vuapi CFileSystemX::Read(
  ulong ulOffset,
  void* Buffer,
  ulong ulSize,
  eMoveMethodFlags mmFlag
)
{
  if (!this->Seek(ulOffset, mmFlag)) return false;

  BOOL result = ReadFile(m_FileHandle, Buffer, ulSize, (LPDWORD)&m_ReadSize, NULL);
  if (!result && ulSize != m_ReadSize)
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileSystemX::Read(void* Buffer, ulong ulSize)
{
  BOOL result = ReadFile(m_FileHandle, Buffer, ulSize, (LPDWORD)&m_ReadSize, NULL);
  if (!result && ulSize != m_ReadSize)
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileSystemX::Write(
  ulong ulOffset,
  const void* cBuffer,
  ulong ulSize,
  eMoveMethodFlags mmFlag
)
{
  if (!this->Seek(ulOffset, mmFlag)) return false;

  BOOL result = WriteFile(m_FileHandle, cBuffer, ulSize, (LPDWORD)&m_WroteSize, NULL);
  if (!result && ulSize != m_WroteSize)
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileSystemX::Write(const void* cBuffer, ulong ulSize)
{
  BOOL result = WriteFile(m_FileHandle, cBuffer, ulSize, (LPDWORD)&m_WroteSize, NULL);
  if (!result && ulSize != m_WroteSize)
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

bool vuapi CFileSystemX::Seek(ulong ulOffset, eMoveMethodFlags mmFlag)
{
  if (!this->IsFileHandleValid(m_FileHandle))
  {
    return false;
  }

  ulong result = SetFilePointer(m_FileHandle, ulOffset, NULL, mmFlag);

  m_LastErrorCode = GetLastError();

  return (result != INVALID_SET_FILE_POINTER);
}

ulong vuapi CFileSystemX::GetFileSize()
{
  if (!this->IsFileHandleValid(m_FileHandle))
  {
    return 0;
  }

  ulong result = ::GetFileSize(m_FileHandle, NULL);

  m_LastErrorCode = GetLastError();

  return result;
}

bool vuapi CFileSystemX::IOControl(
  ulong ulControlCode,
  void* lpSendBuffer,
  ulong ulSendSize,
  void* lpReveiceBuffer,
  ulong ulReveiceSize
)
{
  ulong ulReturnedLength = 0;

  bool result = (DeviceIoControl(
    m_FileHandle,
    ulControlCode,
    lpSendBuffer,
    ulSendSize,
    lpReveiceBuffer,
    ulReveiceSize,
    &ulReturnedLength,
    NULL
  ) != 0);

  m_LastErrorCode = GetLastError();

  return result;
}

bool vuapi CFileSystemX::Close()
{
  if (!this->IsFileHandleValid(m_FileHandle))
  {
    return false;
  }

  if (!CloseHandle(m_FileHandle))
  {
    return false;
  }

  m_FileHandle = INVALID_HANDLE_VALUE;

  return true;
}

const CBuffer vuapi CFileSystemX::ReadAsBuffer()
{
  CBuffer pContent(0);

  auto size = this->GetFileSize();
  if (size == 0) return pContent;

  #ifdef _WIN64
  size += 8;
  #else // _WIN32
  size += 4;
  #endif

  pContent.AdjustSize(size);

  this->Read(0, pContent.GetpData(), size, eMoveMethodFlags::MM_BEGIN);

  return pContent;
}

// A

CFileSystemA::CFileSystemA() : CFileSystemX()
{
}

CFileSystemA::CFileSystemA(
  const std::string& FilePath,
  eFSModeFlags fmFlag,
  eFSGenericFlags fgFlag,
  eFSShareFlags fsFlag,
  eFSAttributeFlags faFlag
) : CFileSystemX()
{
  this->Init(FilePath, fmFlag, fgFlag, fsFlag, faFlag);
}

CFileSystemA::~CFileSystemA()
{
}

bool vuapi CFileSystemA::Init(
  const std::string& FilePath,
  eFSModeFlags fmFlag,
  eFSGenericFlags fgFlag,
  eFSShareFlags fsFlag,
  eFSAttributeFlags faFlag
)
{
  m_FileHandle = CreateFileA(FilePath.c_str(), fgFlag, fsFlag, NULL, fmFlag, faFlag, NULL);
  if (!this->IsFileHandleValid(m_FileHandle))
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

const std::string vuapi CFileSystemA::ReadFileAsString(bool removeBOM)
{
  std::string result("");

  auto pContent = this->ReadAsBuffer();
  auto p = (char*)pContent.GetpData();

  auto encoding = DetermineEncodingType(pContent.GetpData(), pContent.GetSize());
  if (encoding == eEncodingType::ET_UNKNOWN)
  {
    assert(0);
    return result;
  }

  if (removeBOM && encoding == eEncodingType::ET_UTF8_BOM)
  {
    p += 3; /* remove BOM */
  }

  result.assign(p);

  return result;
}

const std::string vuapi CFileSystemA::QuickReadAsString(const std::string& FilePath, bool removeBOM)
{
  CFileSystemA file(FilePath, vu::eFSModeFlags::FM_OPENEXISTING);
  auto result = file.ReadFileAsString(removeBOM);
  return result;
}

CBuffer CFileSystemA::QuickReadAsBuffer(const std::string& FilePath)
{
  if (!IsFileExistsA(FilePath))
  {
    return CBuffer();
  }

  CFileSystemA fs(FilePath, eFSModeFlags::FM_OPENEXISTING);
  return fs.ReadAsBuffer();
}

bool CFileSystemA::Iterate(
  const std::string& Path,
  const std::string& Pattern,
  const std::function<bool(const TFSObjectA& FSObject)> fnCallback)
{
  auto thePathSlash = vu::TrimStringA(Path);
  if (thePathSlash.empty())
  {
    return false;
  }

  if (thePathSlash.back() != '\\' && thePathSlash.back() != '/')
  {
    thePathSlash += "\\";
  }

  auto thePathPattern = thePathSlash + Pattern;

  WIN32_FIND_DATAA theWDF = { 0 };

  auto hFind = FindFirstFileA(thePathPattern.c_str(), &theWDF);
  if (INVALID_HANDLE_VALUE == hFind)
  {
    return false;
  }

  TFSObjectA theFileObject;
  theFileObject.Directory = thePathSlash;

  LARGE_INTEGER theFileSize = { 0 };

  do
  {
    if (theWDF.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      theFileSize.LowPart  = 0;
      theFileSize.HighPart = 0;
    }
    else
    {
      theFileSize.LowPart  = theWDF.nFileSizeLow;
      theFileSize.HighPart = theWDF.nFileSizeHigh;
    }
    theFileObject.Size = theFileSize.QuadPart;
    theFileObject.Attributes = theWDF.dwFileAttributes;
    theFileObject.Name = theWDF.cFileName;
    if (!fnCallback(theFileObject))
    {
      break;
    }
  } while (FindNextFileA(hFind, &theWDF) != FALSE);

  FindClose(hFind);

  return true;
}

// W

CFileSystemW::CFileSystemW() : CFileSystemX()
{
}

CFileSystemW::CFileSystemW(
  const std::wstring& FilePath,
  eFSModeFlags fmFlag,
  eFSGenericFlags fgFlag,
  eFSShareFlags fsFlag,
  eFSAttributeFlags faFlag
) : CFileSystemX()
{
  this->Init(FilePath, fmFlag, fgFlag, fsFlag, faFlag);
}

CFileSystemW::~CFileSystemW()
{
}

bool vuapi CFileSystemW::Init(
  const std::wstring& FilePath,
  eFSModeFlags fmFlag,
  eFSGenericFlags fgFlag,
  eFSShareFlags fsFlag,
  eFSAttributeFlags faFlag
)
{
  m_FileHandle = CreateFileW(FilePath.c_str(), fgFlag, fsFlag, NULL, fmFlag, faFlag, NULL);
  if (!this->IsFileHandleValid(m_FileHandle))
  {
    m_LastErrorCode = GetLastError();
    return false;
  }

  return true;
}

const std::wstring vuapi CFileSystemW::ReadAsString(bool removeBOM)
{
  std::wstring result(L"");

  auto pContent = this->ReadAsBuffer();
  auto p = (wchar*)pContent.GetpData();

  auto encoding = DetermineEncodingType(pContent.GetpData(), pContent.GetSize());
  if (encoding == eEncodingType::ET_UNKNOWN)
  {
    assert(0);
    return result;
  }

  if (removeBOM && (encoding == eEncodingType::ET_UTF16_LE_BOM || encoding == eEncodingType::ET_UTF16_BE_BOM))
  {
    p = (wchar*)((char*)pContent.GetpData() + 2); /* remove BOM */
  }

  result.assign(p);

  return result;
}

const std::wstring vuapi CFileSystemW::QuickReadAsString(const std::wstring& FilePath, bool removeBOM)
{
  CFileSystemW file(FilePath, vu::eFSModeFlags::FM_OPENEXISTING);
  auto result = file.ReadAsString(removeBOM);
  return result;
}

CBuffer CFileSystemW::QuickReadAsBuffer(const std::wstring& FilePath)
{
  if (!IsFileExistsW(FilePath))
  {
    return CBuffer();
  }

  CFileSystemW fs(FilePath, eFSModeFlags::FM_OPENEXISTING);
  return fs.ReadAsBuffer();
}

bool CFileSystemW::Iterate(
  const std::wstring& Path,
  const std::wstring& Pattern,
  const std::function<bool(const TFSObjectW& FSObject)> fnCallback)
{
  auto thePathSlash = vu::TrimStringW(Path);
  if (thePathSlash.empty())
  {
    return false;
  }

  if (thePathSlash.back() != L'\\' && thePathSlash.back() != L'/')
  {
    thePathSlash += L"\\";
  }

  auto thePathPattern = thePathSlash + Pattern;

  WIN32_FIND_DATAW theWDF = { 0 };

  auto hFind = FindFirstFileW(thePathPattern.c_str(), &theWDF);
  if (INVALID_HANDLE_VALUE == hFind)
  {
    return false;
  }

  TFSObjectW theFileObject;
  theFileObject.Directory = thePathSlash;

  LARGE_INTEGER theFileSize = { 0 };

  do
  {
    if (theWDF.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      theFileSize.LowPart  = 0;
      theFileSize.HighPart = 0;
    }
    else
    {
      theFileSize.LowPart  = theWDF.nFileSizeLow;
      theFileSize.HighPart = theWDF.nFileSizeHigh;
    }
    theFileObject.Size = theFileSize.QuadPart;
    theFileObject.Attributes = theWDF.dwFileAttributes;
    theFileObject.Name = theWDF.cFileName;
    fnCallback(theFileObject);
  } while (FindNextFileW(hFind, &theWDF) != FALSE);

  FindClose(hFind);

  return true;
}

} // namespace vu