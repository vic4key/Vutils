/**
 * @file   window.cpp
 * @author Vic P.
 * @brief  Implementation for Window
 */

#include "Vutils.h"

#include <cassert>
#include <cstdlib>

#include "defs.h"

namespace vu
{

HWND vuapi get_console_window()
{
  typedef HWND (WINAPI *PfnGetConsoleWindow)();

  HWND hwnd_console = nullptr;

  PfnGetConsoleWindow pfnGetConsoleWindow =\
    (PfnGetConsoleWindow)Library::quick_get_proc_address(_T("kernel32.dll"), _T("GetConsoleWindow"));

  if (pfnGetConsoleWindow)
  {
    hwnd_console = pfnGetConsoleWindow();
  }

  return hwnd_console;
}

typedef std::pair<ulong, HWND> PairPIDHWND;

BOOL CALLBACK fnFindTopWindowCallback(HWND hwnd, LPARAM lp)
{
  auto& params = *(PairPIDHWND*)lp;

  DWORD pid = INVALID_PID_VALUE;

  if (GetWindowThreadProcessId(hwnd, &pid) && pid == params.first)
  {
    params.second = hwnd;
    return FALSE;
  }

  return TRUE;
}

HWND vuapi find_top_window(ulong pid)
{
  PairPIDHWND params(pid, nullptr);

  auto ret = EnumWindows(fnFindTopWindowCallback, (LPARAM)&params);

  return params.second;
}

HWND vuapi find_top_window(HWND hwnd)
{
  DWORD pid = INVALID_PID_VALUE;
  GetWindowThreadProcessId(hwnd, &pid);
  if (pid == INVALID_PID_VALUE)
  {
    return nullptr;
  }

  return find_top_window(pid);
}

HWND vuapi find_main_window(ulong pid)
{
  HWND hwnd = find_top_window(pid);
  return find_main_window(hwnd);
}

HWND vuapi find_main_window(HWND hwnd)
{
  if (!IsWindow(hwnd))
  {
    return nullptr;
  }

  auto hwnd_parent = GetParent(hwnd);
  if (hwnd_parent == nullptr)
  {
    return hwnd;
  }

  return find_main_window(hwnd_parent);
}

static BOOL CALLBACK MonitorEnumProc_A(HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{
  auto& monitors = *reinterpret_cast<Monitors_A*>(pData);

  MONITORINFOEXA mi;
  memset(&mi, 0, sizeof(mi));
  mi.cbSize = sizeof(mi);

  if (GetMonitorInfoA(hMonitor, &mi) != FALSE)
  {
    monitors.push_back(mi);
  }

  return TRUE;
}

bool vuapi get_monitors_A(Monitors_A& monitors)
{
  monitors.clear();
  return EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc_A, LPARAM(&monitors)) != FALSE;
}

static BOOL CALLBACK MonitorEnumProc_W(HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{
  auto& monitors = *reinterpret_cast<Monitors_W*>(pData);

  MONITORINFOEXW mi;
  memset(&mi, 0, sizeof(mi));
  mi.cbSize = sizeof(mi);

  if (GetMonitorInfoW(hMonitor, &mi) != FALSE)
  {
    monitors.push_back(mi);
  }

  return TRUE;
}

bool vuapi get_monitors_W(Monitors_W& monitors)
{
  monitors.clear();
  return EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc_W, LPARAM(&monitors)) != FALSE;
}

FontA vuapi get_font_A(HWND hwnd)
{
  FontA result;

  if (IsWindow(hwnd))
  {
    HDC hdc = GetDC(hwnd);
    {
      HFONT hf = (HFONT)SendMessageA(hwnd, WM_GETFONT, 0, 0);
      if (hf != nullptr)
      {
        LOGFONTA lf = { 0 };
        GetObjectA(hf, sizeof(lf), &lf);
        result.name  = lf.lfFaceName;
        result.size = conv_font_height_to_size(lf.lfHeight);
        result.italic = lf.lfItalic != FALSE;
        result.under_line = lf.lfUnderline != FALSE;
        result.strike_out = lf.lfStrikeOut != FALSE;
        result.weight = lf.lfWeight;
        result.char_set = lf.lfCharSet;
        result.orientation = lf.lfOrientation;
      }
    }
    ReleaseDC(hwnd, hdc);
  }

  return result;
}

FontW vuapi get_font_W(HWND hwnd)
{
  FontW result;

  if (IsWindow(hwnd))
  {
    HDC hdc = GetDC(hwnd);
    {
      HFONT hf = (HFONT)SendMessageW(hwnd, WM_GETFONT, 0, 0);
      if (hf != nullptr)
      {
        LOGFONTW lf = { 0 };
        GetObjectW(hf, sizeof(lf), &lf);
        result.name = lf.lfFaceName;
        result.size = conv_font_height_to_size(lf.lfHeight);
        result.italic = lf.lfItalic != FALSE;
        result.under_line = lf.lfUnderline != FALSE;
        result.strike_out = lf.lfStrikeOut != FALSE;
        result.weight = lf.lfWeight;
        result.char_set = lf.lfCharSet;
        result.orientation = lf.lfOrientation;
      }
    }
    ReleaseDC(hwnd, hdc);
  }

  return result;
}

bool vuapi is_window_full_screen(HWND hwnd)
{
  WINDOWPLACEMENT wp = { 0 };
  wp.length = sizeof(WINDOWPLACEMENT);
  ::GetWindowPlacement(hwnd, &wp);
  return wp.showCmd == SW_SHOWMAXIMIZED;
}

LONG vuapi conv_font_height_to_size(LONG height, HWND hwnd)
{
  auto hdc = GetWindowDC(hwnd);
  LONG result = MulDiv(-height, 72, GetDeviceCaps(hdc, LOGPIXELSY));
  ReleaseDC(hwnd, hdc);
  return result;
}

LONG vuapi conv_font_size_to_height(LONG size, HWND hwnd)
{
  auto hdc = GetWindowDC(hwnd);
  LONG result = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
  ReleaseDC(hwnd, hdc);
  return result;
}

/**
 * WDT
 */

#define PTR_ALIGN(p, t)\
  {\
    typedef decltype(p) T;\
    p = T((t(p) + 3) & ~3);\
  }

void* fn_copy_memory(void** ppdst, void* psrc, size_t nsrc)
{
  auto pdst = (char*)*ppdst;
  memcpy(pdst, psrc, nsrc);
  pdst += nsrc;
  return (void*)pdst;
};

#define PTR_COPY_NEXT_EX(d, s, n)\
  {\
    typedef decltype(d) T;\
    d = (T)fn_copy_memory((void**)&(d), (void*)(s), (n));\
  }

#define PTR_COPY_NEXT(d, s)\
  PTR_COPY_NEXT_EX(d, &s, sizeof(s));

/**
 * WDTControl
 */

// https://docs.microsoft.com/en-us/windows/win32/dlgbox/dialog-box-overviews
// https://docs.microsoft.com/en-us/windows/win32/dlgbox/using-dialog-boxes#creating-a-template-in-memory
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-dlgtemplate
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-dlgitemtemplate

const WCHAR NullChar = L'\0';

WDTControl::WDTControl(
  const std::wstring& caption,
  const WDTControl::control_class_name type,
  const WORD  id,
  const short x,
  const short y,
  const short cx,
  const short cy,
  const DWORD style,
  const DWORD exstyle
) : m_caption(caption), m_w_type(type), m_w_class(-1), m_w_data(0)
{
  m_shape.x = x;
  m_shape.y = y;
  m_shape.cx = cx;
  m_shape.cy = cy;
  m_shape.id = id;
  m_shape.style = style;
  m_shape.dwExtendedStyle = exstyle;
}

WDTControl::~WDTControl()
{
}

void WDTControl::serialize(void** pptr)
{
  auto ptr = *pptr;

  PTR_ALIGN(ptr, DWORD_PTR);
  PTR_COPY_NEXT(ptr, m_shape);
  PTR_COPY_NEXT(ptr, m_w_class);
  PTR_COPY_NEXT(ptr, m_w_type);
  PTR_COPY_NEXT_EX(ptr, m_caption.c_str(), sizeof(wchar) * m_caption.length());
  PTR_COPY_NEXT(ptr, NullChar);
  PTR_COPY_NEXT(ptr, m_w_data);

  *pptr = ptr;
}

/**
 * WDTDialog
 */

WDTDialog::WDTDialog(
  const std::wstring& caption,
  const DWORD style,
  const DWORD exstyle,
  const short x,
  const short y,
  const short cx,
  const short cy,
  HWND hwParent
) : m_caption(caption), m_w_class(0), m_w_menu(0), m_hwnd_parent(hwParent)
{
  m_hglobal = GlobalAlloc(GMEM_ZEROINIT, KiB); // 1 KiB
  assert(m_hglobal != nullptr);

  m_font  = L"Segoe UI";
  m_w_font = 9;

  if (m_hwnd_parent == nullptr)
  {
    m_hwnd_parent = GetActiveWindow();
  }

  if (IsWindow(m_hwnd_parent))
  {
    auto font = get_font_W(m_hwnd_parent);
    if (!font.name.empty())
    {
      m_font  = font.name;
      m_w_font = font.size;
    }
  }

  m_shape.x = x;
  m_shape.y = y;
  m_shape.cx = cx;
  m_shape.cy = cy;
  m_shape.style = style;
  m_shape.dwExtendedStyle = exstyle;

  if (!m_font.empty() && m_w_font != -1)
  {
    m_shape.style |= DS_SETFONT;
  }
}

WDTDialog::~WDTDialog()
{
  if (m_hglobal != nullptr)
  {
    GlobalFree(m_hglobal);
  }
}

const std::vector<vu::WDTControl>& WDTDialog::controls() const
{
  return m_controls;
}

void WDTDialog::add(const WDTControl& control)
{
  m_controls.push_back(control);
  m_shape.cdit = static_cast<WORD>(m_controls.size());
}

void WDTDialog::serialize(void** pptr)
{
  auto ptr = *pptr;

  PTR_COPY_NEXT(ptr, m_shape);
  PTR_COPY_NEXT(ptr, m_w_menu);
  PTR_COPY_NEXT(ptr, m_w_class);
  PTR_COPY_NEXT_EX(ptr, m_caption.c_str(), sizeof(wchar) * m_caption.length());
  PTR_COPY_NEXT(ptr, NullChar);
  PTR_COPY_NEXT(ptr, m_w_font);
  PTR_COPY_NEXT_EX(ptr, m_font.c_str(), sizeof(wchar) * m_font.length());
  PTR_COPY_NEXT(ptr, NullChar);

  for (auto& control : m_controls)
  {
    control.serialize(&ptr);
  }

  *pptr = ptr;
}

INT_PTR WDTDialog::do_modal(DLGPROC pfn_dlg_proc, WDTDialog* ptr_parent)
{
  m_last_error_code = ERROR_SUCCESS;

  if (m_hglobal == nullptr)
  {
    m_last_error_code = GetLastError();
    return -1;
  }

  auto ptr = GlobalLock(m_hglobal);
  if (ptr == nullptr)
  {
    m_last_error_code = GetLastError();
    return -1;
  }

  serialize(&ptr);

  GlobalUnlock(m_hglobal);

  auto ret = DialogBoxIndirectParamA(
    GetModuleHandle(0), LPDLGTEMPLATE(m_hglobal), m_hwnd_parent, pfn_dlg_proc, LPARAM(ptr_parent));

  m_last_error_code = GetLastError();

  return ret;
}

/**
 * InputDialog
 */

InputDialog::InputDialog(
  const std::wstring& label, HWND hwnd_parent, bool number_only, const std::wstring& placeholder)
  : IDC_LABEL(0x1001)
  , IDC_INPUT(0x1002)
  , m_label(label)
  , m_placeholder(placeholder)
  , m_number_only(number_only)
  , vu::WDTDialog(L"Input Dialog"
  , DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
  , WS_EX_DLGMODALFRAME
  , 0, 0, 179, 60
  , hwnd_parent)
{
  this->add(WDTControl(
    L"Label",
    WDTControl::CT_STATIC,
    IDC_LABEL,
    7, 5, 165, 8,
    WS_CHILD | WS_VISIBLE | SS_LEFT | BF_FLAT)
  );

  this->add(WDTControl(
    L"",
    WDTControl::CT_EDIT,
    IDC_INPUT,
    7, 18, 165, 14,
    WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER | (m_number_only ? ES_NUMBER : 0))
  );

  this->add(WDTControl(
    L"OK",
    WDTControl::CT_BUTTON,
    IDOK,
    66, 39, 50, 14,
    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT)
  );

  this->add(WDTControl(
    L"Cancel",
    WDTControl::CT_BUTTON,
    IDCANCEL,
    121, 39, 50, 14,
    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT)
  );
}

InputDialog::~InputDialog()
{
}

void InputDialog::placeholder(const std::wstring& pl)
{
  m_placeholder = pl;
}

void InputDialog::label(const std::wstring& label)
{
  m_label = label;
}

vu::VariantW& InputDialog::input()
{
  return m_input;
}

INT_PTR InputDialog::do_modal()
{
  return WDTDialog::do_modal(DLGPROC(DlgProc), this);
}

LRESULT CALLBACK InputDialog::DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
  static InputDialog* ptr_self = nullptr;
  switch (msg)
  {
  case WM_INITDIALOG:
  {
    if (ptr_self == nullptr)
    {
      ptr_self = reinterpret_cast<InputDialog*>(lp);
    }

    if (ptr_self != nullptr)
    {
      SetWindowTextW(GetDlgItem(hwnd, ptr_self->IDC_LABEL), ptr_self->m_label.c_str());
      SetWindowTextW(GetDlgItem(hwnd, ptr_self->IDC_INPUT), ptr_self->m_placeholder.c_str());
    }

    RECT rc = { 0 };
    GetWindowRect(hwnd, &rc);
    int x = -(rc.right  - rc.left) / 2;
    int y = -(rc.bottom - rc.top)  / 2;

    if (auto hwnd_parent = GetParent(hwnd))
    {
      RECT rc = { 0 };
      GetWindowRect(hwnd_parent, &rc);
      x += rc.left + (rc.right  - rc.left) / 2;
      y += rc.top  + (rc.bottom - rc.top)  / 2;
    }
    else
    {
      x += GetSystemMetrics(SM_CXSCREEN) / 2;
      y += GetSystemMetrics(SM_CYSCREEN) / 2;
    }

    SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    SetFocus(GetDlgItem(hwnd, ptr_self->IDC_INPUT));
  }
  break;

  case WM_COMMAND:
  {
    switch (LOWORD(wp))
    {
    case IDOK:
    {
      assert(ptr_self != nullptr);

      wchar_t s[KiB] = { 0 };
      GetWindowTextW(GetDlgItem(hwnd, ptr_self->IDC_INPUT), s, sizeof(s) / sizeof(s[0]));
      ptr_self->input() << s;

      EndDialog(hwnd, IDOK);
    }
    break;

    case IDCANCEL:
    {
      EndDialog(hwnd, IDCANCEL);
    }
    break;

    default:
      break;
    }
  }
  break;

  case WM_CLOSE:
  {
    EndDialog(hwnd, IDCANCEL);
  }
  break;

  default:
    break;
  }

  return FALSE;
}

} // namespace vu