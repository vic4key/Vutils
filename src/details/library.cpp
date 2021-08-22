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

LibraryA::LibraryA(const std::string& module_name) : LastError()
{
  m_module_handle  = LoadLibraryA(module_name.c_str());
  m_last_error_code = GetLastError();
}

LibraryA::~LibraryA()
{
  FreeLibrary(m_module_handle);
}

const HMODULE& vuapi LibraryA::handle() const
{
  return m_module_handle;
}

bool vuapi LibraryA::available()
{
  return m_module_handle != nullptr;
}

void* vuapi LibraryA::get_proc_address(const std::string& function_name)
{
  if (m_module_handle == nullptr || function_name.empty())
  {
    return nullptr;
  }

  return (void*)::GetProcAddress(m_module_handle, function_name.c_str());
}

void* vuapi LibraryA::quick_get_proc_address(const std::string& module_name, const std::string& function_name)
{
  if (module_name.empty() || function_name.empty())
  {
    return nullptr;
  }

  LibraryA lib(module_name);
  if (!lib.available())
  {
    return nullptr;
  }

  return lib.get_proc_address(function_name);
}

LibraryW::LibraryW(const std::wstring& module_name) : LastError()
{
  m_module_handle  = LoadLibraryW(module_name.c_str());
  m_last_error_code = GetLastError();
}

LibraryW::~LibraryW()
{
  FreeLibrary(m_module_handle);
}

const HMODULE& vuapi LibraryW::handle() const
{
  return m_module_handle;
}

bool vuapi LibraryW::available()
{
  return m_module_handle != nullptr;
}

void* vuapi LibraryW::get_proc_address(const std::wstring& function_name)
{
  if (m_module_handle == nullptr || function_name.empty())
  {
    return nullptr;
  }

  auto s = to_string_A(function_name);

  return (void*)::GetProcAddress(m_module_handle, s.c_str());
}

void* vuapi LibraryW::quick_get_proc_address(const std::wstring& module_name, const std::wstring& function_name)
{
  if (module_name.empty() || function_name.empty())
  {
    return nullptr;
  }

  LibraryW lib(module_name);
  if (!lib.available())
  {
    return nullptr;
  }

  return lib.get_proc_address(function_name);
}

/**
 * Misc
 */

// @refer to https://docs.microsoft.com/en-us/windows/win32/memory/obtaining-a-file-name-from-a-file-handle

std::wstring get_file_name_from_handle_X(HANDLE hf)
{
  std::wstring result = L"";

  if (hf == INVALID_HANDLE_VALUE)
  {
    return result;
  }

  if (Initialize_DLL_LAZY() != VU_OK)
  {
    return result;
  }

  BOOL  bSuccess = FALSE;

  const int BUFSIZE = 512;
  wchar_t pszFilename[MAX_PATH + 1];

  // Get the file size.
  DWORD dwFileSizeHi = 0;
  DWORD dwFileSizeLo = GetFileSize(hf, &dwFileSizeHi);
  if (dwFileSizeLo == 0 && dwFileSizeHi == 0)
  {
    return result;
  }

  // Create a file mapping object.
  if (auto hFileMap = CreateFileMappingW(hf, NULL, PAGE_READONLY, 0, 1, NULL))
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

std::string vuapi get_file_name_from_handle_A(HANDLE hf)
{
  return to_string_A(get_file_name_from_handle_W(hf));
}

std::wstring vuapi get_file_name_from_handle_W(HANDLE hf)
{
  return get_file_name_from_handle_X(hf);
}

} // namespace vu