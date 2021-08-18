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

  HWND hwConsole = NULL;

  PfnGetConsoleWindow pfnGetConsoleWindow = (PfnGetConsoleWindow)CLibrary::QuickGetProcAddress(
    _T("kernel32.dll"),
    _T("GetConsoleWindow")
  );

  if (pfnGetConsoleWindow)
  {
    hwConsole = pfnGetConsoleWindow();
  }

  return hwConsole;
}

typedef std::pair<ulong, HWND> PairPIDHWND;

BOOL CALLBACK fnFindTopWindowCallback(HWND hWnd, LPARAM lParam)
{
  auto& params = *(PairPIDHWND*)lParam;

  DWORD thePID = 0;

  if (GetWindowThreadProcessId(hWnd, &thePID) && thePID == params.first)
  {
    params.second = hWnd;
    return FALSE;
  }

  return TRUE;
}

HWND vuapi find_top_window(ulong ulPID)
{
  PairPIDHWND params(ulPID, nullptr);

  auto ret = EnumWindows(fnFindTopWindowCallback, (LPARAM)&params);

  return params.second;
}

HWND vuapi find_main_window(HWND hWnd)
{
  if (!IsWindow(hWnd))
  {
    return nullptr;
  }

  auto hWndParent = GetParent(hWnd);
  if (hWndParent == nullptr)
  {
    return hWnd;
  }

  return find_main_window(hWndParent);
}

#define WM_DEF(id, name) { id, (char*) # name}

std::string vuapi decode_wm_A(const ulong id)
{
  static struct { ulong id; char* text; } m[] = \
  {
    WM_DEF(0x0000, WM_NULL),
    WM_DEF(0x0001, WM_CREATE),
    WM_DEF(0x0002, WM_DESTROY),
    WM_DEF(0x0003, WM_MOVE),
    WM_DEF(0x0005, WM_SIZE),
    WM_DEF(0x0006, WM_ACTIVATE),
    WM_DEF(0x0007, WM_SETFOCUS),
    WM_DEF(0x0008, WM_KILLFOCUS),
    WM_DEF(0x000A, WM_ENABLE),
    WM_DEF(0x000B, WM_SETREDRAW),
    WM_DEF(0x000C, WM_SETTEXT),
    WM_DEF(0x000D, WM_GETTEXT),
    WM_DEF(0x000E, WM_GETTEXTLENGTH),
    WM_DEF(0x000F, WM_PAINT),
    WM_DEF(0x0010, WM_CLOSE),
    WM_DEF(0x0011, WM_QUERYENDSESSION),
    WM_DEF(0x0013, WM_QUERYOPEN),
    WM_DEF(0x0016, WM_ENDSESSION),
    WM_DEF(0x0012, WM_QUIT),
    WM_DEF(0x0014, WM_ERASEBKGND),
    WM_DEF(0x0015, WM_SYSCOLORCHANGE),
    WM_DEF(0x0018, WM_SHOWWINDOW),
    WM_DEF(0x001A, WM_WININICHANGE),
    WM_DEF(0x001B, WM_DEVMODECHANGE),
    WM_DEF(0x001C, WM_ACTIVATEAPP),
    WM_DEF(0x001D, WM_FONTCHANGE),
    WM_DEF(0x001E, WM_TIMECHANGE),
    WM_DEF(0x001F, WM_CANCELMODE),
    WM_DEF(0x0020, WM_SETCURSOR),
    WM_DEF(0x0021, WM_MOUSEACTIVATE),
    WM_DEF(0x0022, WM_CHILDACTIVATE),
    WM_DEF(0x0023, WM_QUEUESYNC),
    WM_DEF(0x0024, WM_GETMINMAXINFO),
    WM_DEF(0x0026, WM_PAINTICON),
    WM_DEF(0x0027, WM_ICONERASEBKGND),
    WM_DEF(0x0028, WM_NEXTDLGCTL),
    WM_DEF(0x002A, WM_SPOOLERSTATUS),
    WM_DEF(0x002B, WM_DRAWITEM),
    WM_DEF(0x002C, WM_MEASUREITEM),
    WM_DEF(0x002D, WM_DELETEITEM),
    WM_DEF(0x002E, WM_VKEYTOITEM),
    WM_DEF(0x002F, WM_CHARTOITEM),
    WM_DEF(0x0030, WM_SETFONT),
    WM_DEF(0x0031, WM_GETFONT),
    WM_DEF(0x0032, WM_SETHOTKEY),
    WM_DEF(0x0033, WM_GETHOTKEY),
    WM_DEF(0x0037, WM_QUERYDRAGICON),
    WM_DEF(0x0039, WM_COMPAREITEM),
    WM_DEF(0x003D, WM_GETOBJECT),
    WM_DEF(0x0041, WM_COMPACTING),
    WM_DEF(0x0044, WM_COMMNOTIFY),
    WM_DEF(0x0046, WM_WINDOWPOSCHANGING),
    WM_DEF(0x0047, WM_WINDOWPOSCHANGED),
    WM_DEF(0x0048, WM_POWER),
    WM_DEF(0x004A, WM_COPYDATA),
    WM_DEF(0x004B, WM_CANCELJOURNAL),
    WM_DEF(0x004E, WM_NOTIFY),
    WM_DEF(0x0050, WM_INPUTLANGCHANGEREQUEST),
    WM_DEF(0x0051, WM_INPUTLANGCHANGE),
    WM_DEF(0x0052, WM_TCARD),
    WM_DEF(0x0053, WM_HELP),
    WM_DEF(0x0054, WM_USERCHANGED),
    WM_DEF(0x0055, WM_NOTIFYFORMAT),
    WM_DEF(0x007B, WM_CONTEXTMENU),
    WM_DEF(0x007C, WM_STYLECHANGING),
    WM_DEF(0x007D, WM_STYLECHANGED),
    WM_DEF(0x007E, WM_DISPLAYCHANGE),
    WM_DEF(0x007F, WM_GETICON),
    WM_DEF(0x0080, WM_SETICON),
    WM_DEF(0x0081, WM_NCCREATE),
    WM_DEF(0x0082, WM_NCDESTROY),
    WM_DEF(0x0083, WM_NCCALCSIZE),
    WM_DEF(0x0084, WM_NCHITTEST),
    WM_DEF(0x0085, WM_NCPAINT),
    WM_DEF(0x0086, WM_NCACTIVATE),
    WM_DEF(0x0087, WM_GETDLGCODE),
    WM_DEF(0x0088, WM_SYNCPAINT),
    WM_DEF(0x00A0, WM_NCMOUSEMOVE),
    WM_DEF(0x00A1, WM_NCLBUTTONDOWN),
    WM_DEF(0x00A2, WM_NCLBUTTONUP),
    WM_DEF(0x00A3, WM_NCLBUTTONDBLCLK),
    WM_DEF(0x00A4, WM_NCRBUTTONDOWN),
    WM_DEF(0x00A5, WM_NCRBUTTONUP),
    WM_DEF(0x00A6, WM_NCRBUTTONDBLCLK),
    WM_DEF(0x00A7, WM_NCMBUTTONDOWN),
    WM_DEF(0x00A8, WM_NCMBUTTONUP),
    WM_DEF(0x00A9, WM_NCMBUTTONDBLCLK),
    WM_DEF(0x00AB, WM_NCXBUTTONDOWN),
    WM_DEF(0x00AC, WM_NCXBUTTONUP),
    WM_DEF(0x00AD, WM_NCXBUTTONDBLCLK),
    WM_DEF(0x00FE, WM_INPUT_DEVICE_CHANGE),
    WM_DEF(0x00FF, WM_INPUT),
    WM_DEF(0x0100, WM_KEYFIRST),
    WM_DEF(0x0100, WM_KEYDOWN),
    WM_DEF(0x0101, WM_KEYUP),
    WM_DEF(0x0102, WM_CHAR),
    WM_DEF(0x0103, WM_DEADCHAR),
    WM_DEF(0x0104, WM_SYSKEYDOWN),
    WM_DEF(0x0105, WM_SYSKEYUP),
    WM_DEF(0x0106, WM_SYSCHAR),
    WM_DEF(0x0107, WM_SYSDEADCHAR),
    WM_DEF(0x0109, WM_UNICHAR),
    WM_DEF(0x0109, WM_KEYLAST),
    WM_DEF(0x0108, WM_KEYLAST),
    WM_DEF(0x010D, WM_IME_STARTCOMPOSITION),
    WM_DEF(0x010E, WM_IME_ENDCOMPOSITION),
    WM_DEF(0x010F, WM_IME_COMPOSITION),
    WM_DEF(0x010F, WM_IME_KEYLAST),
    WM_DEF(0x0110, WM_INITDIALOG),
    WM_DEF(0x0111, WM_COMMAND),
    WM_DEF(0x0112, WM_SYSCOMMAND),
    WM_DEF(0x0113, WM_TIMER),
    WM_DEF(0x0114, WM_HSCROLL),
    WM_DEF(0x0115, WM_VSCROLL),
    WM_DEF(0x0116, WM_INITMENU),
    WM_DEF(0x0117, WM_INITMENUPOPUP),
    WM_DEF(0x0119, WM_GESTURE),
    WM_DEF(0x011A, WM_GESTURENOTIFY),
    WM_DEF(0x011F, WM_MENUSELECT),
    WM_DEF(0x0120, WM_MENUCHAR),
    WM_DEF(0x0121, WM_ENTERIDLE),
    WM_DEF(0x0122, WM_MENURBUTTONUP),
    WM_DEF(0x0123, WM_MENUDRAG),
    WM_DEF(0x0124, WM_MENUGETOBJECT),
    WM_DEF(0x0125, WM_UNINITMENUPOPUP),
    WM_DEF(0x0126, WM_MENUCOMMAND),
    WM_DEF(0x0127, WM_CHANGEUISTATE),
    WM_DEF(0x0128, WM_UPDATEUISTATE),
    WM_DEF(0x0129, WM_QUERYUISTATE),
    WM_DEF(0x0132, WM_CTLCOLORMSGBOX),
    WM_DEF(0x0133, WM_CTLCOLOREDIT),
    WM_DEF(0x0134, WM_CTLCOLORLISTBOX),
    WM_DEF(0x0135, WM_CTLCOLORBTN),
    WM_DEF(0x0136, WM_CTLCOLORDLG),
    WM_DEF(0x0137, WM_CTLCOLORSCROLLBAR),
    WM_DEF(0x0138, WM_CTLCOLORSTATIC),
    WM_DEF(0x0200, WM_MOUSEFIRST),
    WM_DEF(0x0200, WM_MOUSEMOVE),
    WM_DEF(0x0201, WM_LBUTTONDOWN),
    WM_DEF(0x0202, WM_LBUTTONUP),
    WM_DEF(0x0203, WM_LBUTTONDBLCLK),
    WM_DEF(0x0204, WM_RBUTTONDOWN),
    WM_DEF(0x0205, WM_RBUTTONUP),
    WM_DEF(0x0206, WM_RBUTTONDBLCLK),
    WM_DEF(0x0207, WM_MBUTTONDOWN),
    WM_DEF(0x0208, WM_MBUTTONUP),
    WM_DEF(0x0209, WM_MBUTTONDBLCLK),
    WM_DEF(0x020A, WM_MOUSEWHEEL),
    WM_DEF(0x020B, WM_XBUTTONDOWN),
    WM_DEF(0x020C, WM_XBUTTONUP),
    WM_DEF(0x020D, WM_XBUTTONDBLCLK),
    WM_DEF(0x020E, WM_MOUSEHWHEEL),
    WM_DEF(0x020E, WM_MOUSELAST),
    WM_DEF(0x020D, WM_MOUSELAST),
    WM_DEF(0x020A, WM_MOUSELAST),
    WM_DEF(0x0209, WM_MOUSELAST),
    WM_DEF(0x0210, WM_PARENTNOTIFY),
    WM_DEF(0x0211, WM_ENTERMENULOOP),
    WM_DEF(0x0212, WM_EXITMENULOOP),
    WM_DEF(0x0213, WM_NEXTMENU),
    WM_DEF(0x0214, WM_SIZING),
    WM_DEF(0x0215, WM_CAPTURECHANGED),
    WM_DEF(0x0216, WM_MOVING),
    WM_DEF(0x0218, WM_POWERBROADCAST),
    WM_DEF(0x0219, WM_DEVICECHANGE),
    WM_DEF(0x0220, WM_MDICREATE),
    WM_DEF(0x0221, WM_MDIDESTROY),
    WM_DEF(0x0222, WM_MDIACTIVATE),
    WM_DEF(0x0223, WM_MDIRESTORE),
    WM_DEF(0x0224, WM_MDINEXT),
    WM_DEF(0x0225, WM_MDIMAXIMIZE),
    WM_DEF(0x0226, WM_MDITILE),
    WM_DEF(0x0227, WM_MDICASCADE),
    WM_DEF(0x0228, WM_MDIICONARRANGE),
    WM_DEF(0x0229, WM_MDIGETACTIVE),
    WM_DEF(0x0230, WM_MDISETMENU),
    WM_DEF(0x0231, WM_ENTERSIZEMOVE),
    WM_DEF(0x0232, WM_EXITSIZEMOVE),
    WM_DEF(0x0233, WM_DROPFILES),
    WM_DEF(0x0234, WM_MDIREFRESHMENU),
    WM_DEF(0x0238, WM_POINTERDEVICECHANGE),
    WM_DEF(0x0239, WM_POINTERDEVICEINRANGE),
    WM_DEF(0x023A, WM_POINTERDEVICEOUTOFRANGE),
    WM_DEF(0x0240, WM_TOUCH),
    WM_DEF(0x0241, WM_NCPOINTERUPDATE),
    WM_DEF(0x0242, WM_NCPOINTERDOWN),
    WM_DEF(0x0243, WM_NCPOINTERUP),
    WM_DEF(0x0245, WM_POINTERUPDATE),
    WM_DEF(0x0246, WM_POINTERDOWN),
    WM_DEF(0x0247, WM_POINTERUP),
    WM_DEF(0x0249, WM_POINTERENTER),
    WM_DEF(0x024A, WM_POINTERLEAVE),
    WM_DEF(0x024B, WM_POINTERACTIVATE),
    WM_DEF(0x024C, WM_POINTERCAPTURECHANGED),
    WM_DEF(0x024D, WM_TOUCHHITTESTING),
    WM_DEF(0x024E, WM_POINTERWHEEL),
    WM_DEF(0x024F, WM_POINTERHWHEEL),
    WM_DEF(0x0251, WM_POINTERROUTEDTO),
    WM_DEF(0x0252, WM_POINTERROUTEDAWAY),
    WM_DEF(0x0253, WM_POINTERROUTEDRELEASED),
    WM_DEF(0x0281, WM_IME_SETCONTEXT),
    WM_DEF(0x0282, WM_IME_NOTIFY),
    WM_DEF(0x0283, WM_IME_CONTROL),
    WM_DEF(0x0284, WM_IME_COMPOSITIONFULL),
    WM_DEF(0x0285, WM_IME_SELECT),
    WM_DEF(0x0286, WM_IME_CHAR),
    WM_DEF(0x0288, WM_IME_REQUEST),
    WM_DEF(0x0290, WM_IME_KEYDOWN),
    WM_DEF(0x0291, WM_IME_KEYUP),
    WM_DEF(0x02A1, WM_MOUSEHOVER),
    WM_DEF(0x02A3, WM_MOUSELEAVE),
    WM_DEF(0x02A0, WM_NCMOUSEHOVER),
    WM_DEF(0x02A2, WM_NCMOUSELEAVE),
    WM_DEF(0x02B1, WM_WTSSESSION_CHANGE),
    WM_DEF(0x02c0, WM_TABLET_FIRST),
    WM_DEF(0x02df, WM_TABLET_LAST),
    WM_DEF(0x02E0, WM_DPICHANGED),
    WM_DEF(0x02E2, WM_DPICHANGED_BEFOREPARENT),
    WM_DEF(0x02E3, WM_DPICHANGED_AFTERPARENT),
    WM_DEF(0x02E4, WM_GETDPISCALEDSIZE),
    WM_DEF(0x0300, WM_CUT),
    WM_DEF(0x0301, WM_COPY),
    WM_DEF(0x0302, WM_PASTE),
    WM_DEF(0x0303, WM_CLEAR),
    WM_DEF(0x0304, WM_UNDO),
    WM_DEF(0x0305, WM_RENDERFORMAT),
    WM_DEF(0x0306, WM_RENDERALLFORMATS),
    WM_DEF(0x0307, WM_DESTROYCLIPBOARD),
    WM_DEF(0x0308, WM_DRAWCLIPBOARD),
    WM_DEF(0x0309, WM_PAINTCLIPBOARD),
    WM_DEF(0x030A, WM_VSCROLLCLIPBOARD),
    WM_DEF(0x030B, WM_SIZECLIPBOARD),
    WM_DEF(0x030C, WM_ASKCBFORMATNAME),
    WM_DEF(0x030D, WM_CHANGECBCHAIN),
    WM_DEF(0x030E, WM_HSCROLLCLIPBOARD),
    WM_DEF(0x030F, WM_QUERYNEWPALETTE),
    WM_DEF(0x0310, WM_PALETTEISCHANGING),
    WM_DEF(0x0311, WM_PALETTECHANGED),
    WM_DEF(0x0312, WM_HOTKEY),
    WM_DEF(0x0317, WM_PRINT),
    WM_DEF(0x0318, WM_PRINTCLIENT),
    WM_DEF(0x0319, WM_APPCOMMAND),
    WM_DEF(0x031A, WM_THEMECHANGED),
    WM_DEF(0x031D, WM_CLIPBOARDUPDATE),
    WM_DEF(0x031E, WM_DWMCOMPOSITIONCHANGED),
    WM_DEF(0x031F, WM_DWMNCRENDERINGCHANGED),
    WM_DEF(0x0320, WM_DWMCOLORIZATIONCOLORCHANGED),
    WM_DEF(0x0321, WM_DWMWINDOWMAXIMIZEDCHANGE),
    WM_DEF(0x0323, WM_DWMSENDICONICTHUMBNAIL),
    WM_DEF(0x0326, WM_DWMSENDICONICLIVEPREVIEWBITMAP),
    WM_DEF(0x033F, WM_GETTITLEBARINFOEX),
    WM_DEF(0x0358, WM_HANDHELDFIRST),
    WM_DEF(0x035F, WM_HANDHELDLAST),
    WM_DEF(0x0360, WM_QUERYAFXWNDPROC),
    WM_DEF(0x0361, WM_SIZEPARENT),
    WM_DEF(0x0362, WM_SETMESSAGESTRING),
    WM_DEF(0x0363, WM_IDLEUPDATECMDUI),
    WM_DEF(0x0364, WM_INITIALUPDATE),
    WM_DEF(0x0365, WM_COMMANDHELP),
    WM_DEF(0x0366, WM_HELPHITTEST),
    WM_DEF(0x0367, WM_EXITHELPMODE),
    WM_DEF(0x0368, WM_RECALCPARENT),
    WM_DEF(0x0369, WM_SIZECHILD),
    WM_DEF(0x036A, WM_KICKIDLE),
    WM_DEF(0x036B, WM_QUERYCENTERWND),
    WM_DEF(0x036C, WM_DISABLEMODAL),
    WM_DEF(0x036D, WM_FLOATSTATUS),
    WM_DEF(0x036E, WM_ACTIVATETOPLEVEL),
    WM_DEF(0x036F, WM_RESERVED_036F),
    WM_DEF(0x0371, WM_RESERVED_0371),
    WM_DEF(0x0372, WM_RESERVED_0372),
    WM_DEF(0x0374, WM_SOCKET_DEAD),
    WM_DEF(0x0377, WM_OCC_LOADFROMSTORAGE),
    WM_DEF(0x0378, WM_OCC_INITNEW),
    WM_DEF(0x037A, WM_OCC_LOADFROMSTREAM_EX),
    WM_DEF(0x037B, WM_OCC_LOADFROMSTORAGE_EX),
    WM_DEF(0x037E, WM_RESERVED_037E),
    WM_DEF(0x037F, WM_AFXLAST),
    WM_DEF(0x0380, WM_PENWINFIRST),
    WM_DEF(0x038F, WM_PENWINLAST),
    WM_DEF(0x0400, WM_USER),
  };

  std::string result = "";

  for (auto e : m)
  {
    if (e.id == id)
    {
      result = e.text;
      break;
    }
  }

  return result;
}

std::wstring vuapi decode_wm_W(const ulong id)
{
  return to_string_W(decode_wm_A(id));
}

TFontA vuapi get_font_A(HWND hw)
{
  TFontA result;

  if (IsWindow(hw))
  {
    HDC hdc = GetDC(hw);
    {
      HFONT hf = (HFONT)SendMessageA(hw, WM_GETFONT, 0, 0);
      if (hf != nullptr)
      {
        LOGFONTA lf = { 0 };
        GetObjectA(hf, sizeof(lf), &lf);
        result.Name  = lf.lfFaceName;
        result.Size = -MulDiv(lf.lfHeight, 72, GetDeviceCaps(hdc, LOGPIXELSY)); // 72 pixel/inch
        result.Italic = lf.lfItalic != FALSE;
        result.Underline = lf.lfUnderline != FALSE;
        result.StrikeOut = lf.lfStrikeOut != FALSE;
        result.Weight = lf.lfWeight;
        result.CharSet = lf.lfCharSet;
        result.Orientation = lf.lfOrientation;
      }
    }
    ReleaseDC(hw, hdc);
  }

  return result;
}

TFontW vuapi get_font_W(HWND hw)
{
  TFontW result;

  if (IsWindow(hw))
  {
    HDC hdc = GetDC(hw);
    {
      HFONT hf = (HFONT)SendMessageW(hw, WM_GETFONT, 0, 0);
      if (hf != nullptr)
      {
        LOGFONTW lf = { 0 };
        GetObjectW(hf, sizeof(lf), &lf);
        result.Name = lf.lfFaceName;
        result.Size = -MulDiv(lf.lfHeight, 72, GetDeviceCaps(hdc, LOGPIXELSY)); // 72 pixel/inch
        result.Italic = lf.lfItalic != FALSE;
        result.Underline = lf.lfUnderline != FALSE;
        result.StrikeOut = lf.lfStrikeOut != FALSE;
        result.Weight = lf.lfWeight;
        result.CharSet = lf.lfCharSet;
        result.Orientation = lf.lfOrientation;
      }
    }
    ReleaseDC(hw, hdc);
  }

  return result;
}

/**
 * CWDT
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
 * CWDTControl
 */

// https://docs.microsoft.com/en-us/windows/win32/dlgbox/dialog-box-overviews
// https://docs.microsoft.com/en-us/windows/win32/dlgbox/using-dialog-boxes#creating-a-template-in-memory
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-dlgtemplate
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-dlgitemtemplate

const WCHAR NullChar = L'\0';

CWDTControl::CWDTControl(
  const std::wstring& caption,
  const CWDTControl::eControlClass type,
  const WORD  id,
  const short x,
  const short y,
  const short cx,
  const short cy,
  const DWORD style,
  const DWORD exstyle
) : m_Caption(caption), m_wType(type), m_wClass(-1), m_wData(0)
{
  m_Shape.x = x;
  m_Shape.y = y;
  m_Shape.cx = cx;
  m_Shape.cy = cy;
  m_Shape.id = id;
  m_Shape.style = style;
  m_Shape.dwExtendedStyle = exstyle;
}

CWDTControl::~CWDTControl()
{
}

void CWDTControl::Serialize(void** pptr)
{
  auto ptr = *pptr;

  PTR_ALIGN(ptr, DWORD_PTR);
  PTR_COPY_NEXT(ptr, m_Shape);
  PTR_COPY_NEXT(ptr, m_wClass);
  PTR_COPY_NEXT(ptr, m_wType);
  PTR_COPY_NEXT_EX(ptr, m_Caption.c_str(), sizeof(wchar) * m_Caption.length());
  PTR_COPY_NEXT(ptr, NullChar);
  PTR_COPY_NEXT(ptr, m_wData);

  *pptr = ptr;
}

/**
 * CWDTDialog
 */

CWDTDialog::CWDTDialog(
  const std::wstring& caption,
  const DWORD style,
  const DWORD exstyle,
  const short x,
  const short y,
  const short cx,
  const short cy,
  HWND hwParent
) : m_Caption(caption), m_wClass(0), m_wMenu(0), m_hwParent(hwParent)
{
  m_hGlobal = GlobalAlloc(GMEM_ZEROINIT, KiB); // 1 KiB
  assert(m_hGlobal != nullptr);

  m_Font  = L"Segoe UI";
  m_wFont = 9;

  if (m_hwParent == nullptr)
  {
    m_hwParent = GetActiveWindow();
  }

  if (IsWindow(m_hwParent))
  {
    auto font = get_font_W(m_hwParent);
    if (!font.Name.empty())
    {
      m_Font  = font.Name;
      m_wFont = font.Size;
    }
  }

  m_Shape.x = x;
  m_Shape.y = y;
  m_Shape.cx = cx;
  m_Shape.cy = cy;
  m_Shape.style = style;
  m_Shape.dwExtendedStyle = exstyle;

  if (!m_Font.empty() && m_wFont != -1)
  {
    m_Shape.style |= DS_SETFONT;
  }
}

CWDTDialog::~CWDTDialog()
{
  if (m_hGlobal != nullptr)
  {
    GlobalFree(m_hGlobal);
  }
}

const std::vector<vu::CWDTControl>& CWDTDialog::Controls() const
{
  return m_Controls;
}

void CWDTDialog::Add(const CWDTControl& control)
{
  m_Controls.push_back(control);
  m_Shape.cdit = static_cast<WORD>(m_Controls.size());
}

void CWDTDialog::Serialize(void** pptr)
{
  auto ptr = *pptr;

  PTR_COPY_NEXT(ptr, m_Shape);
  PTR_COPY_NEXT(ptr, m_wMenu);
  PTR_COPY_NEXT(ptr, m_wClass);
  PTR_COPY_NEXT_EX(ptr, m_Caption.c_str(), sizeof(wchar) * m_Caption.length());
  PTR_COPY_NEXT(ptr, NullChar);
  PTR_COPY_NEXT(ptr, m_wFont);
  PTR_COPY_NEXT_EX(ptr, m_Font.c_str(), sizeof(wchar) * m_Font.length());
  PTR_COPY_NEXT(ptr, NullChar);

  for (auto& control : m_Controls)
  {
    control.Serialize(&ptr);
  }

  *pptr = ptr;
}

INT_PTR CWDTDialog::DoModal(DLGPROC pfnDlgProc, CWDTDialog* pSelf)
{
  m_LastErrorCode = ERROR_SUCCESS;

  if (m_hGlobal == nullptr)
  {
    m_LastErrorCode = GetLastError();
    return -1;
  }

  auto ptr = GlobalLock(m_hGlobal);
  if (ptr == nullptr)
  {
    m_LastErrorCode = GetLastError();
    return -1;
  }

  Serialize(&ptr);

  GlobalUnlock(m_hGlobal);

  auto ret = DialogBoxIndirectParamA(
    GetModuleHandle(0), LPDLGTEMPLATE(m_hGlobal), 0, pfnDlgProc, LPARAM(pSelf));

  m_LastErrorCode = GetLastError();

  return ret;
}

/**
 * CInputDialog
 */

CInputDialog::CInputDialog(const std::wstring& label, HWND hwParent, bool numberonly)
  : IDC_LABEL(0x1001)
  , IDC_INPUT(0x1002)
  , m_Label(label)
  , m_NumberOnly(numberonly)
  , vu::CWDTDialog(L"Input Dialog"
  , DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
  , WS_EX_DLGMODALFRAME
  , 0, 0, 179, 60
  , hwParent
)
{
  Add(CWDTControl(
    L"Label",
    CWDTControl::CT_STATIC,
    IDC_LABEL,
    7, 5, 165, 8,
    WS_CHILD | WS_VISIBLE | SS_LEFT | BF_FLAT)
  );

  Add(CWDTControl(
    L"",
    CWDTControl::CT_EDIT,
    IDC_INPUT,
    7, 18, 165, 14,
    WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER | (m_NumberOnly ? ES_NUMBER : 0))
  );

  Add(CWDTControl(
    L"OK",
    CWDTControl::CT_BUTTON,
    IDOK,
    66, 39, 50, 14,
    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT)
  );

  Add(CWDTControl(
    L"Cancel",
    CWDTControl::CT_BUTTON,
    IDCANCEL,
    121, 39, 50, 14,
    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT)
  );
}

CInputDialog::~CInputDialog()
{
}

void CInputDialog::Label(const std::wstring& label)
{
  m_Label = label;
}

const std::wstring& CInputDialog::Label() const
{
  return m_Label;
}

vu::CFundamentalW& CInputDialog::Input()
{
  return m_Input;
}

INT_PTR CInputDialog::DoModal()
{
  return CWDTDialog::DoModal(DLGPROC(DlgProc), this);
}

LRESULT CALLBACK CInputDialog::DlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
  static CInputDialog* pSelf = nullptr;
  switch (msg)
  {
  case WM_INITDIALOG:
  {
    if (pSelf == nullptr)
    {
      pSelf = reinterpret_cast<CInputDialog*>(lp);
    }

    if (pSelf != nullptr)
    {
      SetWindowTextW(GetDlgItem(hw, pSelf->IDC_LABEL), pSelf->Label().c_str());
    }

    RECT rc = { 0 };
    GetWindowRect(hw, &rc);
    int X = (GetSystemMetrics(SM_CXSCREEN) - rc.right)  / 2;
    int Y = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;
    SetWindowPos(hw, nullptr, X, Y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    SetFocus(GetDlgItem(hw, pSelf->IDC_INPUT));
  }
  break;

  case WM_COMMAND:
  {
    switch (LOWORD(wp))
    {
    case IDOK:
    {
      assert(pSelf != nullptr);

      wchar_t s[KiB] = { 0 };
      GetWindowTextW(GetDlgItem(hw, pSelf->IDC_INPUT), s, sizeof(s) / sizeof(s[0]));
      pSelf->Input() << s;

      EndDialog(hw, IDOK);
    }
    break;

    case IDCANCEL:
    {
      EndDialog(hw, IDCANCEL);
    }
    break;

    default:
      break;
    }
  }
  break;

  case WM_CLOSE:
  {
    EndDialog(hw, IDCANCEL);
  }
  break;

  default:
    break;
  }

  return FALSE;
}

} // namespace vu