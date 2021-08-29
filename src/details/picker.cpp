/**
 * @file   picker.cpp
 * @author Vic P.
 * @brief  Implementation for Picker
 */

#include "Vutils.h"

#if defined(_M_X64) || defined(__x86_64__)
#pragma pack(push, 8)
#endif

#include <shlobj.h>

#include <commdlg.h>
#ifdef _MSC_VER
#pragma comment(lib, "comdlg32.lib")
#endif // _MSC_VER

namespace vu
{

// PickerX

PickerX::PickerX(HWND hwnd) : m_hwnd(hwnd)
{
}

PickerX::~PickerX()
{
}

bool PickerX::choose_rgb_color(COLORREF& color, const ulong flags)
{
  static COLORREF custom_colors[16] = { 0 }; // default black color

  CHOOSECOLOR cc = { 0 };
  cc.lStructSize = sizeof(cc);
  cc.hwndOwner = m_hwnd;
  cc.lpCustColors = custom_colors;
  cc.rgbResult = color;
  cc.Flags = flags;

  bool result = ChooseColor(&cc) == IDOK;
  if (result)
  {
    color = cc.rgbResult;
  }

  return result;
}

#define IOPicker_Initialize_OFN(ofn, hwnd, buf, dir)\
  ofn.lStructSize = sizeof(ofn);\
  ofn.hwndOwner = hwnd;\
  ofn.hInstance = nullptr;\
  ofn.lpstrFile = buf;\
  ofn.nMaxFile = ARRAYSIZE(buf);\
  ofn.lpstrFilter = filters;\
  ofn.nFilterIndex = 1;\
  ofn.lpstrFileTitle = nullptr;\
  ofn.nMaxFileTitle = 0;\
  ofn.lpstrInitialDir = dir.empty() ? nullptr : dir.c_str();

// PickerA

PickerA::PickerA(HWND hwnd) : PickerX(hwnd)
{
}

PickerA::~PickerA()
{
}

bool PickerA::choose_log_font(LOGFONTA& font, const ulong flags)
{
  CHOOSEFONTA cf = { 0 };
  cf.lStructSize = sizeof(cf);
  cf.hwndOwner = m_hwnd;
  cf.lpLogFont = &font;
  cf.rgbColors = 0x000000; // default black color
  cf.Flags = flags;

  return ChooseFontA(&cf) == IDOK;
}

bool PickerA::choose_file(
  const action_t action,
  std::string& file_path,
  const std::string& file_dir,
  const char* filters,
  const ulong flags)
{
  char sz[KB] = { 0 };
  if (!file_path.empty())
  {
    strcpy_s(sz, file_path.c_str());
  }

  OPENFILENAMEA ofn = { 0 };
  IOPicker_Initialize_OFN(ofn, m_hwnd, sz, file_dir);
  ofn.Flags = flags | (action == action_t::save ? OFN_OVERWRITEPROMPT : 0);

  BOOL ret = action == action_t::open ? GetOpenFileNameA(&ofn) : GetSaveFileNameA(&ofn);
  bool result = ret == IDOK;
  if (result)
  {
    file_path.clear();
    file_path.assign(sz);
  }

  return result;
}

bool PickerA::choose_files(
  std::vector<std::string>& file_names,
  std::string& file_dir,
  const char* filters,
  const ulong flags)
{
  file_names.clear();

  char sz[KB] = { 0 };
  OPENFILENAMEA ofn = { 0 };
  IOPicker_Initialize_OFN(ofn, m_hwnd, sz, file_dir);
  ofn.Flags = flags | OFN_ALLOWMULTISELECT;

  bool result = GetOpenFileNameA(&ofn) == IDOK;

  file_dir.clear();

  if (result)
  {
    auto lst = multi_string_to_list_A(sz);
    assert(!lst.empty());
    file_dir = lst[0];
    file_names.assign(lst.cbegin() + 1, lst.cend());
  }

  return result;
}

static int CALLBACK fncb_BrowseDirectoryA(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  if (uMsg == BFFM_INITIALIZED)
  {
    SendMessageA(hwnd, BFFM_SETSELECTION, TRUE, lpData);
  }

  return 0;
}

bool PickerA::choose_directory(std::string& directory, const ulong flags)
{
  OleInitialize(NULL);

  char sz[MAX_PATH];

  BROWSEINFOA bi = { 0 };
  bi.ulFlags = flags;
  bi.lParam = LPARAM(directory.empty() ? nullptr : directory.c_str());
  bi.lpfn = fncb_BrowseDirectoryA;

  LPITEMIDLIST pIDL = SHBrowseForFolderA(&bi);

  directory.clear();

  if (pIDL != nullptr)
  {
    SHGetPathFromIDListA(pIDL, sz);

    IMalloc* ptr_imalloc = nullptr;
    if (SUCCEEDED(SHGetMalloc(&ptr_imalloc)))
    {
      ptr_imalloc->Free(pIDL);
      ptr_imalloc->Release();
    }

    directory.assign(sz);
  }

  OleUninitialize();

  return !directory.empty();
}

// PickerW

PickerW::PickerW(HWND hwnd) : PickerX(hwnd)
{
}

PickerW::~PickerW()
{
}

bool PickerW::choose_log_font(LOGFONTW& font, const ulong flags)
{
  CHOOSEFONTW cf = { 0 };
  cf.lStructSize = sizeof(cf);
  cf.hwndOwner = m_hwnd;
  cf.lpLogFont = &font;
  cf.rgbColors = 0x000000; // default black color
  cf.Flags = flags;

  return ChooseFontW(&cf) == IDOK;
}

bool PickerW::choose_file(
  const action_t action,
  std::wstring& file_path,
  const std::wstring& file_dir,
  const wchar* filters,
  const ulong flags)
{
  wchar sz[KB] = { 0 };
  if (!file_path.empty())
  {
    wcscpy_s(sz, file_path.c_str());
  }

  OPENFILENAMEW ofn = { 0 };
  IOPicker_Initialize_OFN(ofn, m_hwnd, sz, file_dir);
  ofn.Flags = flags | (action == action_t::save ? OFN_OVERWRITEPROMPT : 0);

  BOOL ret = action == action_t::open ? GetOpenFileNameW(&ofn) : GetSaveFileNameW(&ofn);
  bool result = ret == IDOK;
  if (result)
  {
    file_path.clear();
    file_path.assign(sz);
  }

  return result;
}

bool PickerW::choose_files(
  std::vector<std::wstring>& file_names,
  std::wstring& file_dir,
  const wchar* filters,
  const ulong flags)
{
  file_names.clear();

  wchar sz[KB] = { 0 };
  OPENFILENAMEW ofn = { 0 };
  IOPicker_Initialize_OFN(ofn, m_hwnd, sz, file_dir);
  ofn.Flags = flags | OFN_ALLOWMULTISELECT;

  bool result = GetOpenFileNameW(&ofn) == IDOK;

  file_dir.clear();

  if (result)
  {
    auto lst = multi_string_to_list_W(sz);
    assert(!lst.empty());
    file_dir = lst[0];
    file_names.assign(lst.cbegin() + 1, lst.cend());
  }

  return result;
}

static int CALLBACK fncb_BrowseDirectoryW(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  if (uMsg == BFFM_INITIALIZED)
  {
    SendMessageW(hwnd, BFFM_SETSELECTION, TRUE, lpData);
  }

  return 0;
}

bool PickerW::choose_directory(std::wstring& directory, const ulong flags)
{
  OleInitialize(NULL);

  wchar sz[MAX_PATH];

  BROWSEINFOW bi = { 0 };
  bi.ulFlags = flags;
  bi.lParam = LPARAM(directory.empty() ? nullptr : directory.c_str());
  bi.lpfn = fncb_BrowseDirectoryW;

  LPITEMIDLIST pIDL = SHBrowseForFolderW(&bi);

  directory.clear();

  if (pIDL != nullptr)
  {
    SHGetPathFromIDListW(pIDL, sz);

    IMalloc* ptr_imalloc = nullptr;
    if (SUCCEEDED(SHGetMalloc(&ptr_imalloc)))
    {
      ptr_imalloc->Free(pIDL);
      ptr_imalloc->Release();
    }

    directory.assign(sz);
  }

  OleUninitialize();

  return !directory.empty();
}

} // namespace vu

#if defined(_M_X64) || defined(__x86_64__)
#pragma pack(pop)
#endif