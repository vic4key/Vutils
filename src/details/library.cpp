/**
 * @file   library.cpp
 * @author Vic P.
 * @brief  Implementation for Dynamic Library
 */

#include "Vutils.h"
#include "lazy.h"

#include <strsafe.h>

namespace vu
{

CLibraryA::CLibraryA(const std::string& ModuleName) : CLastError()
{
  m_ModuleHandle  = LoadLibraryA(ModuleName.c_str());
  m_LastErrorCode = GetLastError();
}

CLibraryA::~CLibraryA()
{
  FreeLibrary(m_ModuleHandle);
}

const HMODULE& vuapi CLibraryA::GetHandle() const
{
  return m_ModuleHandle;
}

bool vuapi CLibraryA::IsAvailable()
{
  return m_ModuleHandle != nullptr;
}

void* vuapi CLibraryA::GetProcAddress(const std::string& ProcName)
{
  if (m_ModuleHandle == nullptr || ProcName.empty())
  {
    return nullptr;
  }

  return (void*)::GetProcAddress(m_ModuleHandle, ProcName.c_str());
}

void* vuapi CLibraryA::QuickGetProcAddress(const std::string& ModuleName, const std::string& ProcName)
{
  if (ModuleName.empty() || ProcName.empty())
  {
    return nullptr;
  }

  CLibraryA lib(ModuleName);
  if (!lib.IsAvailable())
  {
    return nullptr;
  }

  return lib.GetProcAddress(ProcName);
}

CLibraryW::CLibraryW(const std::wstring& ModuleName) : CLastError()
{
  m_ModuleHandle  = LoadLibraryW(ModuleName.c_str());
  m_LastErrorCode = GetLastError();
}

CLibraryW::~CLibraryW()
{
  FreeLibrary(m_ModuleHandle);
}

const HMODULE& vuapi CLibraryW::GetHandle() const
{
  return m_ModuleHandle;
}

bool vuapi CLibraryW::IsAvailable()
{
  return m_ModuleHandle != nullptr;
}

void* vuapi CLibraryW::GetProcAddress(const std::wstring& ProcName)
{
  if (m_ModuleHandle == nullptr || ProcName.empty())
  {
    return nullptr;
  }

  auto s = ToStringA(ProcName);

  return (void*)::GetProcAddress(m_ModuleHandle, s.c_str());
}

void* vuapi CLibraryW::QuickGetProcAddress(const std::wstring& ModuleName, const std::wstring& ProcName)
{
  if (ModuleName.empty() || ProcName.empty())
  {
    return nullptr;
  }

  CLibraryW lib(ModuleName);
  if (!lib.IsAvailable())
  {
    return nullptr;
  }

  return lib.GetProcAddress(ProcName);
}

/**
 * Misc
 */

// @refer to https://docs.microsoft.com/en-us/windows/win32/memory/obtaining-a-file-name-from-a-file-handle

std::wstring GetFileNameFromHandleX(HANDLE hFile)
{
  std::wstring result = L"";

  if (hFile == INVALID_HANDLE_VALUE)
  {
    return result;
  }

  if (InitTlHelp32() != VU_OK)
  {
    return result;
  }

  BOOL  bSuccess = FALSE;

  const int BUFSIZE = 512;
  wchar_t pszFilename[MAX_PATH + 1];

  // Get the file size.
  DWORD dwFileSizeHi = 0;
  DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);
  if (dwFileSizeLo == 0 && dwFileSizeHi == 0)
  {
    return result;
  }

  // Create a file mapping object.
  if (auto hFileMap = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 1, NULL))
  {
    // Create a file mapping to get the file name.
    if (void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1))
    {
      if (pfnGetMappedFileNameW(GetCurrentProcess(), pMem, pszFilename, MAX_PATH))
      {
        // Translate path with device name to drive letters.
        wchar_t szTemp[BUFSIZE] = { 0 };
        if (GetLogicalDriveStringsW(BUFSIZE - 1, szTemp))
        {
          wchar_t szName[MAX_PATH];
          wchar_t szDrive[3] = L" :";
          BOOL bFound = FALSE;
          wchar_t* p = szTemp;

          do
          {
            // Copy the drive letter to the template string
            *szDrive = *p;

            // Look up each device name
            if (QueryDosDeviceW(szDrive, szName, MAX_PATH))
            {
              size_t uNameLen = wcslen(szName);
              if (uNameLen < MAX_PATH)
              {
                bFound = _wcsnicmp(pszFilename, szName, uNameLen) == 0 && *(pszFilename + uNameLen) == L'\\';
                if (bFound)
                {
                  // Reconstruct pszFilename using szTempFile
                  // Replace device path with DOS path
                  wchar_t szTempFile[MAX_PATH];
                  StringCchPrintfW(szTempFile, MAX_PATH, L"%s%s", szDrive, pszFilename + uNameLen);
                  StringCchCopyNW(pszFilename, MAX_PATH + 1, szTempFile, wcslen(szTempFile));
                }
              }
            }

            // Go to the next NULL character.
            while (*p++);
          } while (!bFound && *p); // end of string
        }
      }

      bSuccess = TRUE;
      UnmapViewOfFile(pMem);
    }

    CloseHandle(hFileMap);
  }

  if (bSuccess)
  {
    result = pszFilename;
  }

  return result;
}

std::string vuapi GetFileNameFromHandleA(HANDLE hFile)
{
  return ToStringA(GetFileNameFromHandleW(hFile));
}

std::wstring vuapi GetFileNameFromHandleW(HANDLE hFile)
{
  return GetFileNameFromHandleX(hFile);
}

} // namespace vu