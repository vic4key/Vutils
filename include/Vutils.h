/**********************************************************/
/*  Name:     Vic Utilities aka Vutils                    */
/*  Version:  1.0                                         */
/*  Platform: Windows 32-bit & 64-bit                     */
/*  Type:     C++ Library for MSVC++/C++MinGW/C++Builder  */
/*  Author:   Vic P. aka vic4key                          */
/*  Mail:     vic4key[at]gmail.com                        */
/*  Website:  https://vic.onl/                            */
/**********************************************************/

#ifndef VUTILS_H
#define VUTILS_H

/**
 * Note :
 * 1. Available for both Windows 32-bit and 64-bit.
 * 2. Available for MSVC++, C++ Builder, C++ MingGW.
 * 3. Force BYTE alignment of structures.
 * 4. Finally, remember to use `vu` namespace.
 */

/* Vutils Version */

#define VU_VERSION  0x01000000 // Version 01.00.0000

/* The Conditions of Vutils */

#if !defined(_WIN32) && !defined(_WIN64)
#error Vutils required Windows 32-bit/64-bit platform
#endif

#ifndef __cplusplus
#error Vutils required C++ compiler
#endif

/* Vutils Configurations */

// VU_NO_EX - To disable all extended utilities

// Default are enabled for MSVC and C++ Builder. For MinGW, see detail at README.md or INSTALL.md.
// VU_SOCKET_ENABLED  - The Socket
// VU_GUID_ENABLED    - The Globally/Universally Unique Identifier a.k.a GUID
// VU_WMI_ENABLED     - The Windows Management Instrumentation a.k.a WMI

#ifndef VU_NO_EX

#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#define VU_SOCKET_ENABLED
#define VU_GUID_ENABLED
#define VU_WMI_ENABLED
#endif

#endif // VU_NO_EX

/* Header Inclusions */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsvc.h>

#ifdef VU_SOCKET_ENABLED
#include <winsock2.h>
#endif // VU_SOCKET_ENABLED

#include <set>
#include <cmath>
#include <ctime>
#include <mutex>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <cassert>
#include <functional>
#include <thread>
#include <numeric>
#include <type_traits>

#ifdef _MSC_VER
#pragma warning(push)
#endif // _MSC_VER

/* Vutils Options */

// MSVC
#ifdef _MSC_VER
#pragma pack(1)     // Force byte alignment of structures
#endif

// C++ Builder
#ifdef __BCPLUSPLUS__
#pragma option -a1  // Force byte alignment of structures
#endif

// MingGW
#ifdef __MINGW32__
#pragma pack(1)     // Force byte alignment of structures
#endif

/* ----------------------------------- Vutils Declarations -------------------------------------- */

#include "inline/std.inl"
#include "inline/com.inl"

namespace threadpool11
{
  class Pool;
  typedef std::function<void()> FnTask;
}

using namespace threadpool11;

namespace vu
{

#include "inline/defs.inl"
#include "inline/types.inl"
#include "inline/spechrs.inl"

/* Vutils Constants */

const VUResult VU_OK  = 0;

const size_t MAX_SIZE = MAXBYTE;

/* ------------------------------------ Public Function(s) -------------------------------------- */

/**
 * Misc Working
 */

class CBuffer;

bool vuapi is_administrator();
bool set_privilege_A(const std::string&  privilege, const bool enable);
bool set_privilege_W(const std::wstring& privilege, const bool enable);
std::string vuapi  get_enviroment_A(const std::string  name);
std::wstring vuapi get_enviroment_W(const std::wstring name);
std::pair<bool, size_t> find_pattern_A(const CBuffer& buffer, const std::string&  pattern);
std::pair<bool, size_t> find_pattern_W(const CBuffer& buffer, const std::wstring& pattern);
std::pair<bool, size_t> find_pattern_A(const void* ptr, const size_t size, const std::string& pattern);
std::pair<bool, size_t> find_pattern_W(const void* ptr, const size_t size, const std::wstring& pattern);

/**
 * Math Working
 */

bool vuapi is_flag_on(ulongptr flags, ulongptr flag);
intptr vuapi gcd(ulongptr count, ...); // UCLN
intptr vuapi lcm(ulongptr count, ...); // BCNN
void vuapi hex_dump(const void* data, int size);

#include "template/math.tpl"

/**
 * String Formatting
 */

typedef enum _ENCODING_TYPE
{
  ET_UNKNOWN      = -1,
  ET_UTF8         = 0, // "ANSI/UTF-8", "ANSI/UTF-8"
  ET_UTF8_BOM     = 1, // "UTF-8 BOM", "UTF-8 BOM"
  ET_UTF16_LE     = 2, // "Unicode", "UTF-16 Little Endian"
  ET_UTF16_BE     = 3, // "Unicode BE", "UTF-16 Big Endian"
  ET_UTF16_LE_BOM = 4, // "Unicode BOM", "UTF-16 Little Endian BOM"
  ET_UTF16_BE_BOM = 5, // "Unicode BE BOM", "UTF-16 Big Endian BOM"
  ET_UTF32_LE_BOM = 6, // "UTF-32 LE BOM", "UTF-32 Little Endian BOM"
  ET_UTF32_BE_BOM = 7, // "UTF-32 BE BOM", "UTF-32 Big Endian BOM"
} eEncodingType;

typedef enum _STD_BYTES : int
{
  SI  = 1000, // 1 KB  = 1000 bytes
  IEC = 1024, // 1 KiB = 1024 bytes
} eStdByte;

std::string vuapi format_A(const std::string format, ...);
std::wstring vuapi format_W(const std::wstring format, ...);
void vuapi msg_debug_A(const std::string format, ...);
void vuapi msg_debug_W(const std::wstring format, ...);
int vuapi msg_box_A(const std::string format, ...);
int vuapi msg_box_W(const std::wstring format, ...);
int vuapi msg_box_A(HWND hwnd, const std::string format, ...);
int vuapi msg_box_W(HWND hwnd, const std::wstring format, ...);
int vuapi msg_box_A(HWND hwnd, uint type, const std::string& caption, const std::string format, ...);
int vuapi msg_box_W(HWND hwnd, uint type, const std::wstring& caption, const std::wstring format, ...);
std::string vuapi get_last_error_A(ulong code = -1);
std::wstring vuapi get_last_error_W(ulong code = -1);
std::string vuapi format_date_time_A(const time_t t, const std::string format);
std::wstring vuapi format_date_time_W(const time_t t, const std::wstring format);
std::string vuapi date_time_to_string_A(const time_t t);
std::wstring vuapi date_time_to_string_W(const time_t t);
eEncodingType vuapi determine_encoding_type(const void* data, const size_t size);
std::string vuapi format_bytes_A(long long bytes, eStdByte std = eStdByte::IEC, int digits = 2);
std::wstring vuapi format_bytes_W(long long bytes, eStdByte std = eStdByte::IEC, int digits = 2);

/**
 * String Working
 */

typedef enum class _TRIM_STRING
{
  TS_LEFT  = 0,
  TS_RIGHT = 1,
  TS_BOTH  = 2,
} eTrimType;

std::string vuapi lower_string_A(const std::string& string);
std::wstring vuapi lower_string_W(const std::wstring& string);
std::string vuapi upper_string_A(const std::string& string);
std::wstring vuapi upper_string_W(const std::wstring& string);
std::string vuapi to_string_A(const std::wstring& string);
std::wstring vuapi to_string_W(const std::string& string);
std::vector<std::string> vuapi split_string_A(const std::string& string, const std::string& seperate, bool remempty = false);
std::vector<std::wstring> vuapi split_string_W(const std::wstring& string, const std::wstring& seperate, bool remempty = false);
std::vector<std::string> vuapi multi_string_to_list_A(const char* ptr_multi_string);
std::vector<std::wstring> vuapi multi_string_to_list_W(const wchar* ptr_multi_string);
std::unique_ptr<char[]> vuapi list_to_multi_string_A(const std::vector<std::string>& strings);
std::unique_ptr<wchar[]> vuapi list_to_multi_string_W(const std::vector<std::wstring>& strings);
std::string vuapi load_rs_string_A(const uint id, const std::string& module_name = "");
std::wstring vuapi load_rs_string_W(const uint id, const std::wstring& module_name = L"");
std::string vuapi trim_string_A(const std::string& string, const eTrimType& type = eTrimType::TS_BOTH, const std::string& chars = " \t\n\r\f\v");
std::wstring vuapi trim_string_W(const std::wstring& string, const eTrimType& type = eTrimType::TS_BOTH, const std::wstring& chars = L" \t\n\r\f\v");
std::string vuapi replace_string_A(const std::string& text, const std::string& from, const std::string& to);
std::wstring vuapi replace_string_W(const std::wstring& text, const std::wstring& from, const std::wstring& to);
bool vuapi starts_with_A(const std::string& text, const std::string& with);
bool vuapi starts_with_W(const std::wstring& text, const std::wstring& with);
bool vuapi ends_with_A(const std::string& text, const std::string& with);
bool vuapi ends_with_W(const std::wstring& text, const std::wstring& with);

/**
 * Process Working
 */

typedef enum _XBit
{
  x86 = 4,
  x64 = 8,
} eXBit;

typedef enum _WOW64
{
  WOW64_ERROR = -1,
  WOW64_NO    = 0,
  WOW64_YES   = 1,
} eWow64;

#ifndef PROCESSOR_ARCHITECTURE_NEUTRAL
#define PROCESSOR_ARCHITECTURE_NEUTRAL 11
#endif

typedef enum _PROCESSOR_ARCHITECTURE
{
  PA_X86     = PROCESSOR_ARCHITECTURE_INTEL,
  PA_MIPS    = PROCESSOR_ARCHITECTURE_MIPS,
  PA_ALPHA   = PROCESSOR_ARCHITECTURE_ALPHA,
  PA_PPC     = PROCESSOR_ARCHITECTURE_PPC,
  PA_SHX     = PROCESSOR_ARCHITECTURE_SHX,
  PA_ARM     = PROCESSOR_ARCHITECTURE_ARM,
  PA_ITANIUM = PROCESSOR_ARCHITECTURE_IA64,
  PA_ALPHA64 = PROCESSOR_ARCHITECTURE_ALPHA64,
  PA_MSIL    = PROCESSOR_ARCHITECTURE_MSIL,
  PA_X64     = PROCESSOR_ARCHITECTURE_AMD64,
  PA_WOW64   = PROCESSOR_ARCHITECTURE_IA32_ON_WIN64,
  PA_NEUTRAL = PROCESSOR_ARCHITECTURE_NEUTRAL,
  PA_UNKNOWN = PROCESSOR_ARCHITECTURE_UNKNOWN,
} eProcessorArchitecture;

typedef struct _BLOCK
{
  void*  Address;
  SIZE_T Size;
} TBlock;

eProcessorArchitecture get_processor_architecture();
bool vuapi is_64bits(HANDLE hp = INVALID_HANDLE_VALUE);
bool vuapi is_64bits(ulong pid = INVALID_PID_VALUE);
eWow64 vuapi is_wow64(HANDLE hp = INVALID_HANDLE_VALUE);
eWow64 vuapi is_wow64(ulong pid = INVALID_PID_VALUE); /* -1: error, 0: false, 1: true */
ulong vuapi get_parent_pid(ulong child_pid);
ulong vuapi get_main_thread_id(ulong pid);
std::vector<ulong> vuapi name_to_pid_A(const std::string& name);
std::vector<ulong> vuapi name_to_pid_W(const std::wstring& name);
std::string vuapi pid_to_name_A(ulong pid);
std::wstring vuapi pid_to_name_W(ulong pid);
HMODULE vuapi remote_get_module_handle_A(ulong pid, const std::string& module_name); // TODO: Uncompleted.
HMODULE vuapi remote_get_module_handle_W(ulong pid, const std::wstring& module_name);
VUResult vuapi inject_dll_A(ulong pid, const std::string& dll_file_path, bool wait_loading = true);
VUResult vuapi inject_dll_W(ulong pid, const std::wstring& dll_file_path, bool wait_loading = true);
bool vuapi rpm(const HANDLE hp, const void* address, void* buffer, const SIZE_T size, const bool force = false);
bool vuapi rpm_ex(const eXBit bit, const HANDLE hp, const void* address, void* buffer, const SIZE_T size, const bool force = false, const SIZE_T n_offsets = 0, ...);
bool vuapi wpm(const HANDLE hp, const void* address, const void* buffer, const SIZE_T size, const bool force = false);
bool vuapi wpm_ex(const eXBit bit, const HANDLE hp, const void* address, const void* buffer, const SIZE_T size, const bool force = false, const SIZE_T n_offsets = 0, ...);

/**
 * Window Working
 */

typedef struct _FONT_A
{
  std::string Name;
  int  Size;
  bool Italic;
  bool Underline;
  bool StrikeOut;
  int  Weight;
  int  CharSet;
  int  Orientation;
  _FONT_A()
    : Name(""), Size(-1)
    , Italic(false), Underline(false), StrikeOut(false)
    , Weight(0), CharSet(ANSI_CHARSET), Orientation(0) {}
} TFontA;

typedef struct _FONT_W
{
  std::wstring Name;
  int  Size;
  bool Italic;
  bool Underline;
  bool StrikeOut;
  int  Weight;
  int  CharSet;
  int  Orientation;
  _FONT_W()
    : Name(L""), Size(-1)
    , Italic(false), Underline(false), StrikeOut(false)
    , Weight(0), CharSet(ANSI_CHARSET), Orientation(0) {}
} TFontW;

HWND vuapi get_console_window();
HWND vuapi find_top_window(ulong pid);
HWND vuapi find_main_window(HWND hwnd);
std::string  vuapi decode_wm_A(const ulong wm);
std::wstring vuapi decode_wm_W(const ulong wm);
TFontA vuapi get_font_A(HWND hwnd);
TFontW vuapi get_font_W(HWND hwnd);

/**
 * File/Directory Working
 */

enum ePathSep
{
  WIN,
  POSIX,
};

enum eDiskType : int
{
  Unspecified = 0,
  HDD = 3,
  SSD = 4,
  SCM = 5,
};

#if defined(VU_WMI_ENABLED)
eDiskType vuapi get_disk_type_A(const char drive);
eDiskType vuapi get_disk_type_W(const wchar_t drive);
#endif // VU_WMI_ENABLED

bool vuapi is_directory_exists_A(const std::string& directory);
bool vuapi is_directory_exists_W(const std::wstring& directory);
bool vuapi is_file_exists_A(const std::string& file_path);
bool vuapi is_file_exists_W(const std::wstring& file_path);
std::string vuapi get_file_type_A(const std::string& file_path);
std::wstring vuapi get_file_type_W(const std::wstring& file_path);
std::string vuapi extract_file_directory_A(const std::string& file_path, bool last_slash = true);
std::wstring vuapi extract_file_directory_W(const std::wstring& file_path, bool last_slash = true);
std::string vuapi extract_file_name_A(const std::string& file_path, bool extension = true);
std::wstring vuapi extract_file_name_W(const std::wstring& file_path, bool extension = true);
std::string vuapi get_current_file_path_A();
std::wstring vuapi get_current_file_path_W();
std::string vuapi get_current_directory_A(bool last_slash = true);
std::wstring vuapi get_current_directory_W(bool last_slash = true);
std::string vuapi get_contain_directory_A(bool last_slash = true);
std::wstring vuapi get_contain_directory_W(bool last_slash = true);
std::string vuapi get_file_name_from_handle_A(HANDLE hf);
std::wstring vuapi get_file_name_from_handle_W(HANDLE hf);
std::string vuapi join_path_A(const std::string& left, const std::string& right, const ePathSep separator = ePathSep::WIN);
std::wstring vuapi join_path_W(const std::wstring& left, const std::wstring& right, const ePathSep separator = ePathSep::WIN);
std::string vuapi normalize_path_A(const std::string& path, const ePathSep separator = ePathSep::WIN);
std::wstring vuapi normalize_path_W(const std::wstring& path, const ePathSep separator = ePathSep::WIN);
std::string vuapi undecorate_cpp_symbol_A(const std::string& name, const ushort flags = 0);   // UNDNAME_COMPLETE
std::wstring vuapi undecorate_cpp_symbol_W(const std::wstring& name, const ushort flags = 0); // UNDNAME_COMPLETE

/*----------- The definition of common function(s) which compatible both ANSI & UNICODE ----------*/

#ifdef _UNICODE
/* Misc Working */
#define set_privilege set_privilege_W
#define get_enviroment get_enviroment_W
#define find_pattern find_pattern_W
/* String Formatting */
#define format format_W
#define msg_debug msg_debug_W
#define msg_box msg_box_W
#define get_last_error get_last_error_W
#define date_time_to_string date_time_to_string_W
#define format_date_time format_date_time_W
#define format_bytes format_bytes_W
/* String Working */
#define lower_string lower_string_W
#define upper_string upper_string_W
#define split_string split_string_W
#define multi_string_to_list multi_string_to_list_W
#define list_to_multi_string list_to_multi_string_W
#define load_rs_string load_rs_string_W
#define trim_string trim_string_W
#define replace_string replace_string_W
#define starts_with starts_with_W
#define ends_with ends_with_W
/* Window Working */
#define decode_wm decode_wm_W
#define get_font get_font_W
/* Process Working */
#define name_to_pid name_to_pid_W
#define pid_to_name pid_to_name_W
#define remote_get_module_handle remote_get_module_handle_W
#define inject_dll inject_dll_W
/* File/Directory Working */
#define is_directory_exists is_directory_exists_W
#define is_file_exists is_file_exists_W
#define get_file_type get_file_type_W
#define extract_file_directory extract_file_directory_W
#define extract_file_name extract_file_name_W
#define get_current_file_path get_current_file_path_W
#define get_current_directory get_current_directory_W
#define get_contain_directory get_contain_directory_W
#define get_file_name_from_handle get_file_name_from_handle_W
#define get_disk_type get_disk_type_W
#define join_path join_path_W
#define normalize_path normalize_path_W
#define undecorate_cpp_symbol undecorate_cpp_symbol_W
#else
/* Misc Working */
#define set_privilege set_privilege_A
#define get_enviroment get_enviroment_A
#define find_pattern find_pattern_A
/* String Formatting */
#define format format_A
#define msg_debug msg_debug_A
#define msg_box msg_box_A
#define get_last_error get_last_error_A
#define date_time_to_string date_time_to_string_A
#define format_date_time format_date_time_A
#define format_bytes format_bytes_A
/* String Working */
#define lower_string lower_string_A
#define upper_string upper_string_A
#define split_string split_string_A
#define multi_string_to_list multi_string_to_list_A
#define load_rs_string load_rs_string_A
#define trim_string trim_string_A
#define replace_string replace_string_A
#define starts_with starts_with_A
#define ends_with ends_with_A
/* Window Working */
#define decode_wm decode_wm_A
#define get_font get_font_A
/* Process Working */
#define name_to_pid name_to_pid_A
#define pid_to_name pid_to_name_A
#define remote_get_module_handle remote_get_module_handle_A
#define inject_dll inject_dll_A
/* File/Directory Working */
#define is_directory_exists is_directory_exists_A
#define is_file_exists is_file_exists_A
#define get_file_type get_file_type_A
#define extract_file_directory extract_file_directory_A
#define extract_file_name extract_file_name_A
#define get_current_file_path get_current_file_path_A
#define get_current_directory get_current_directory_A
#define get_contain_directory get_contain_directory_A
#define get_file_name_from_handle get_file_name_from_handle_A
#define get_disk_type get_disk_type_A
#define join_path join_path_A
#define normalize_path normalize_path_A
#define undecorate_cpp_symbol undecorate_cpp_symbol_A
#endif

/* -------------------------------------- Public Class(es) -------------------------------------- */

/**
 * Windows Last Error
 */

class CLastError
{
public:
  CLastError() : m_last_error_code(ERROR_SUCCESS) {};
  virtual ~CLastError() {};

  CLastError& operator=(const CLastError& right)
  {
    m_last_error_code = right.m_last_error_code;
    return *this;
  }

  virtual void vuapi set_last_error_code(ulong last_error_code)
  {
    m_last_error_code = last_error_code;
  }

  virtual ulong vuapi get_last_error_code()
  {
    return m_last_error_code;
  }

  virtual std::string vuapi get_last_error_message_A()
  {
    return get_last_error_A(m_last_error_code);
  }

  virtual std::wstring vuapi get_last_error_message_W()
  {
    return get_last_error_W(m_last_error_code);
  }

protected:
  ulong m_last_error_code;
};

/**
 * Singleton Pattern
 */

#include "template/singleton.tpl"

/**
 * Globally Unique Identifier
 */

#ifdef VU_GUID_ENABLED

struct sGUID
{
  unsigned long  data1;
  unsigned short data2;
  unsigned short data3;
  unsigned char  data4[8];

  const sGUID& operator = (const sGUID &right) const;
  bool operator == (const sGUID &right) const;
  bool operator != (const sGUID &right) const;
};

class CGUIDX
{
public:
  CGUIDX(bool create = false);
  virtual ~CGUIDX();

  const CGUIDX& operator = (const CGUIDX& right);
  bool operator == (const CGUIDX& right) const;
  bool operator != (const CGUIDX& right) const;

  bool create();

  const sGUID& GUID() const;
  void  GUID(const sGUID& guid);

protected:
  sGUID m_guid;
  VUResult m_status;
};

class CGUIDA : public CGUIDX
{
public:
  CGUIDA(bool create = false) : CGUIDX(create) {}
  virtual ~CGUIDA() {}

  void parse(const std::string& guid);
  std::string as_string() const;

  static const std::string to_string(const sGUID& guid);
  static const sGUID to_guid(const std::string& guid);
};

class CGUIDW : public CGUIDX
{
public:
  CGUIDW(bool create = false) : CGUIDX(create) {}
  virtual ~CGUIDW() {}

  void parse(const std::wstring& guid);
  std::wstring as_string() const;

  static const std::wstring to_string(const sGUID& guid);
  static const sGUID to_guid(const std::wstring& guid);
};

#endif // VU_GUID_ENABLED

/**
 * CBuffer
 */

class CBuffer
{
public:
  CBuffer();
  CBuffer(const void* ptr, const size_t size);
  CBuffer(const size_t size);
  CBuffer(const CBuffer& right);
  virtual ~CBuffer();

  const CBuffer& operator=(const CBuffer& right);
  bool  operator==(const CBuffer& right) const;
  bool  operator!=(const CBuffer& right) const;
  byte& operator[](const size_t offset);
  CBuffer operator()(int begin, int end) const;

  byte*  get_ptr_bytes() const;
  void*  get_ptr_data() const;
  size_t get_size() const;

  bool empty() const;

  void reset();
  void fill(const byte v = 0);
  bool resize(const size_t size);
  bool replace(const void* ptr, const size_t size);
  bool replace(const CBuffer& right);
  bool match(const void* ptr, const size_t size) const;
  size_t find(const void* ptr, const size_t size) const;
  CBuffer till(const void* ptr, const size_t size) const;
  CBuffer slice(int begin, int end) const;

  bool append(const void* pData, const size_t size);
  bool append(const CBuffer& right);

  std::string  to_string_A() const;
  std::wstring to_string_W() const;

  bool save_to_file(const std::string&  file_path);
  bool save_to_file(const std::wstring& file_path);

private:
  bool create(void* ptr, const size_t size, const bool clean = true);
  bool destroy();

private:
  void*  m_ptr_data;
  size_t m_size;
};

/**
 * Library
 */

/**
 * @brief Gets a function in a module by name.
 * @define typedef type (cc *PfnF)(types args);
 * @define PfnF pfnF = nullptr;
 * @param[in] M The module name.
 * @param[in] F The function name.
 * @return  The function.
 */
#define VU_GET_API(M, F) pfn ## F = (Pfn ## F)vu::CLibrary::quick_get_proc_address(_T( # M ), _T( # F ))

class CLibraryA : public CLastError
{
public:
  CLibraryA(const std::string& module_name);
  virtual ~CLibraryA();

  const HMODULE& vuapi handle() const;
  bool  vuapi available();
  void* vuapi get_proc_address(const std::string& function_name);
  static void* vuapi quick_get_proc_address(const std::string& module_name, const std::string& function_name);

private:
  HMODULE m_module_handle;
};

class CLibraryW : public CLastError
{
public:
  CLibraryW(const std::wstring& module_name);
  virtual ~CLibraryW();

  const HMODULE& vuapi handle() const;
  bool  vuapi available();
  void* vuapi get_proc_address(const std::wstring& function_name);
  static void* vuapi quick_get_proc_address(const std::wstring& module_name, const std::wstring& function_name);

private:
  HMODULE m_module_handle;
};

/**
 * Socket
 */

#ifdef VU_SOCKET_ENABLED

#ifndef MSG_NONE
#define MSG_NONE 0
#endif // MSG_NONE

class CSocket : public CLastError
{
public:
  typedef int AddressFamily;
  typedef int Type;
  typedef int Protocol;
  typedef int Flags;
  typedef int Shutdowns;

  struct TSocket
  {
    SOCKET s;
    sockaddr_in sai;
    char ip[15];
  };

  struct TEndPoint
  {
    std::string Host;
    ushort Port;

    TEndPoint(const std::string& host, const ushort port) : Host(host), Port(port) {}
  };

public:
  CSocket(const AddressFamily af = AF_INET, const Type type = SOCK_STREAM, const Protocol proto = IPPROTO_IP, bool wsa = true);
  virtual ~CSocket();

  bool vuapi available();

  void vuapi attach(const SOCKET&  socket);
  void vuapi attach(const TSocket& socket);
  void vuapi detach();

  VUResult vuapi bind(const TEndPoint& endpoint);
  VUResult vuapi bind(const std::string& address, const ushort port);

  VUResult vuapi listen(const int maxcon = SOMAXCONN);
  VUResult vuapi accept(TSocket& socket);

  VUResult vuapi connect(const TEndPoint& endpoint);
  VUResult vuapi connect(const std::string& address, const ushort port);

  IResult vuapi send(const char* pData, int size, const Flags flags = MSG_NONE);
  IResult vuapi send(const CBuffer& data, const Flags flags = MSG_NONE);

  IResult vuapi recv(char* pData, int size, const Flags flags = MSG_NONE);
  IResult vuapi recv(CBuffer& data, const Flags flags = MSG_NONE);
  IResult vuapi recv_all(CBuffer& data, const Flags flags = MSG_NONE);

  IResult vuapi send_to(const char* pData, const int size, const TSocket& socket);
  IResult vuapi send_to(const CBuffer& data, const TSocket& socket);

  IResult vuapi recv_from(char* pData, int size, const TSocket& socket);
  IResult vuapi recv_from(CBuffer& data, const TSocket& socket);
  IResult vuapi recv_all_from(CBuffer& data, const TSocket& socket);

  IResult vuapi close();

  const WSADATA& vuapi get_wsa_data() const;
  const AddressFamily vuapi get_af() const;
  const Type vuapi get_type() const;
  const Protocol vuapi  get_protocol() const;

  SOCKET& vuapi get_socket();
  const sockaddr_in vuapi get_local_sai();
  const sockaddr_in vuapi get_remote_sai();
  std::string vuapi get_host_name();

  VUResult vuapi set_option(const int level, const int opt, const std::string& val, const int size);
  VUResult vuapi enable_non_blocking(bool state = true);
  VUResult vuapi shutdown(const Shutdowns flags);

private:
  bool vuapi valid(const SOCKET& socket);
  bool vuapi parse(const TSocket& socket);
  bool vuapi is_host_name(const std::string& s);
  std::string vuapi get_host_address(const std::string& Name);

private:
  bool m_wsa;
  Type m_type;
  WSADATA m_wsa_data;
  AddressFamily m_af;
  Protocol m_proto;
  sockaddr_in m_sai;
  SOCKET m_socket;
};

class CAsyncSocket
{
public:
  typedef std::function<void(CSocket& client)> FnPrototype;

  typedef enum _FN_TYPE : uint
  {
    OPEN,
    CLOSE,
    RECV,
    SEND,
    UNDEFINED,
  } eFnType;

  CAsyncSocket(const vu::CSocket::AddressFamily af = AF_INET, const vu::CSocket::Type type = SOCK_STREAM, const vu::CSocket::Protocol proto = IPPROTO_IP);
  virtual ~CAsyncSocket();

  std::set<SOCKET> vuapi get_clients();

  bool vuapi available();
  bool vuapi running();

  VUResult vuapi bind(const CSocket::TEndPoint& endpoint);
  VUResult vuapi bind(const std::string& address, const ushort port);
  VUResult vuapi listen(const int maxcon = SOMAXCONN);
  VUResult vuapi run();
  VUResult vuapi stop();
  IResult  vuapi close();

  virtual void on(const eFnType type, const FnPrototype fn); // must be mapping before call Run(...)

  virtual void on_open(CSocket&  client);
  virtual void on_close(CSocket& client);
  virtual void on_send(CSocket&  client);
  virtual void on_recv(CSocket&  client);

protected:
  void vuapi initialze();
  VUResult vuapi loop();

  IResult vuapi do_open(WSANETWORKEVENTS&  Events, SOCKET& Socket);
  IResult vuapi do_recv(WSANETWORKEVENTS&  Events, SOCKET& Socket);
  IResult vuapi do_send(WSANETWORKEVENTS&  Events, SOCKET& Socket);
  IResult vuapi do_close(WSANETWORKEVENTS& Events, SOCKET& Socket);

protected:
  vu::CSocket m_server;
  bool m_running;
  DWORD m_n_events;
  SOCKET m_sockets[WSA_MAXIMUM_WAIT_EVENTS];
  WSAEVENT m_events[WSA_MAXIMUM_WAIT_EVENTS];
  FnPrototype m_functions[eFnType::UNDEFINED];
  std::mutex m_mutex;
};

#endif // VU_SOCKET_ENABLED

/**
 * API Hooking - Inline
 */

 /**
 * @brief Hook/Unhook a function in a module by name.
 * @define The prefix of redirection function must be  : Hfn
 * @define The prefix of real function pointer must be : pfn
 * @param[in] O The CINLHook instance.
 * @param[in] M The module name.
 * @param[in] F The function name.
 * @return  true if the function succeeds. Otherwise false.
 */
#define VU_API_INL_OVERRIDE(O, M, F) O.install(_T( # M ), _T( # F ), (void*)&Hfn ## F, (void**)&pfn ## F)
#define VU_API_INL_RESTORE(O, M, F) O.uninstall(_T( # M ), _T( # F ), (void**)&pfn ## F)

typedef enum _MEMORY_ADDRESS_TYPE
{
  MAT_NONE = 0,
  MAT_8    = 1,
  MAT_16   = 2,
  MAT_32   = 3,
  MAT_64   = 4,
} eMemoryAddressType;

typedef struct _MEMORY_INSTRUCTION
{
  ulong offset;   // Offset of the current instruction.
  ulong position; // Position of the memory address in the current instruction.
  eMemoryAddressType memory_address_type;
  union           // Memory Instruction value.
  {
    uchar   A8;
    ushort  A16;
    ulong   A32;
    ulong64 A64;
  } mao;
  union           // Memory Instruction value.
  {
    uchar   A8;
    ushort  A16;
    ulong   A32;
    ulong64 A64;
  } man;
} TMemoryInstruction;

class CINLHookX
{
protected:
  typedef struct _REDIRECT
  {
    ushort   jmp;
    ulong    unknown;
    ulongptr address;
  } TRedirect;

  bool m_hooked;
  std::vector<TMemoryInstruction> m_memory_instructions;

  static const ulong JMP_OPCODE_SIZE = 6;

  #ifdef _M_IX86
  static const ulong MIN_HOOK_SIZE   = 10;
  #else  // _M_AMD64
  static const ulong MIN_HOOK_SIZE   = 14;
  #endif // _M_IX86

public:
  CINLHookX() : m_hooked(false) {};
  virtual ~CINLHookX() {};

  bool vuapi attach(void* ptr_function, void* ptr_hook_function, void** pptr_old_function);
  bool vuapi detach(void* ptr_function, void** pptr_old_function);
};

class CINLHookA: public CINLHookX
{
public:
  CINLHookA() {};
  virtual ~CINLHookA() {};

  bool vuapi install(
    const std::string& module_name,
    const std::string& function_name,
    void* ptr_hook_function, void** pptr_old_function
  );
  bool vuapi uninstall(
    const std::string& module_name,
    const std::string& function_name,
    void** pptr_old_function
  );
};

class CINLHookW: public CINLHookX
{
public:
  CINLHookW() {};
  virtual ~CINLHookW() {};

  bool vuapi install(
    const std::wstring& module_name,
    const std::wstring& function_name,
    void* ptr_hook_function,
    void** pptr_old_function
  );
  bool vuapi uninstall(
    const std::wstring& module_name,
    const std::wstring& function_name,
    void** pptr_old_function
  );
};

/**
 * API Hooking - IAT
 */

#define VU_API_IAT_OVERRIDE(O, M, F)\
  vu::CIATHookManager::Instance().install(\
    _T( # O ), _T( # M ), _T( # F ),\
    (const void*)(reinterpret_cast<void*>(&Hfn ## F)),\
    (const void**)(reinterpret_cast<void**>(&pfn ## F)))

#define VU_API_IAT_RESTORE(O, M, F)\
  vu::CIATHookManager::Instance().uninstall(\
    _T( # O ), _T( # M ), _T( # F ))

struct IATElement;

class CIATHookManagerA : public CSingletonT<CIATHookManagerA>
{
public:
  CIATHookManagerA();
  virtual ~CIATHookManagerA();

  VUResult install(
    const std::string& target,
    const std::string& module,
    const std::string& function,
    const void* replacement = nullptr,
    const void** original = nullptr
  );

  VUResult uninstall(
    const std::string& target,
    const std::string& module,
    const std::string& function,
    const void** replacement = nullptr
  );

  /**
   * Iterate all imported-functions in a module.
   * @param[out] module   The imported-module name.
   * @param[out] function The imported-function name.
   * @param[out,in] pOFT  The Original First Thunk that point to INT <Import Name Table>.
   * @param[out,in] pFT   The First Thunk that point to IAT <Import Address Table>.
   * @return true to continue or false to exit iteration.
   */
  VUResult iterate(const std::string& module, std::function<bool(
    const std::string& module,
    const std::string& function,
    PIMAGE_THUNK_DATA& pOFT,
    PIMAGE_THUNK_DATA& pFT)> fn);

private:
  enum IATAction
  {
    IAT_INSTALL,
    IAT_UNINSTALL,
  };

  typedef std::vector<IATElement> IATElements;

  IATElements m_iat_elements;

private:
  IATElements::iterator find(const IATElement& element);

  IATElements::iterator find(
    const std::string& target,
    const std::string& module,
    const std::string& function);

  bool exist(
    const std::string& target,
    const std::string& module,
    const std::string& function);

  VUResult perform(const IATAction action, IATElement& element);
};

class CIATHookManagerW : public CSingletonT<CIATHookManagerW>
{
public:
  CIATHookManagerW();
  virtual ~CIATHookManagerW();

  VUResult install(
    const std::wstring& target,
    const std::wstring& module,
    const std::wstring& function,
    const void* replacement = nullptr,
    const void** original = nullptr
  );

  VUResult uninstall(
    const std::wstring& target,
    const std::wstring& module,
    const std::wstring& function,
    const void** replacement = nullptr
  );
};

/**
 * Windows Messages Hooking
 */

class CWMHookX : public CLastError
{
public:
  CWMHookX();
  virtual ~CWMHookX();

  VUResult vuapi uninstall(int Type);

public:
  static HHOOK m_handles[WH_MAXHOOK];

protected:
  virtual VUResult set_windows_hook_ex_X(int type, HMODULE hmodule, HOOKPROC pfn_hook_function);

protected:
  ulong m_pid;
};

class CWMHookA : public CWMHookX
{
public:
  CWMHookA(ulong pid, const std::string& dll_file_path);
  virtual ~CWMHookA();

  VUResult vuapi install(int type, const std::string& function_name);

private:
  CLibraryA m_library;
};

class CWMHookW : public CWMHookX
{
public:
  CWMHookW(ulong pid, const std::wstring& dll_file_path);
  virtual ~CWMHookW();

  VUResult vuapi install(int type, const std::wstring& function_name);

private:
  CLibraryW m_library;
};

/**
 * File System
 */

typedef enum _FS_MODE_FLAGS
{
  FM_CREATENEW = 1,    // CREATE_NEW         = 1,
  FM_CREATEALWAY,      // CREATE_ALWAYS      = 2,
  FM_OPENEXISTING,     // OPEN_EXISTING      = 3,
  FM_OPENALWAYS,       // OPEN_ALWAYS        = 4,
  FM_TRUNCATEEXISTING, // TRUNCATE_EXISTING  = 5,
} eFSModeFlags;

typedef enum _FS_ATTRIBUTE_FLAGS
{
  FA_UNKNOWN          = 0x00000000,
  FA_READONLY         = 0x00000001,   // FILE_ATTRIBUTE_READONLY             = $00000001;
  FA_HIDDEN           = 0x00000002,   // FILE_ATTRIBUTE_HIDDEN               = $00000002;
  FA_SYSTEM           = 0x00000004,   // FILE_ATTRIBUTE_SYSTEM               = $00000004;
  FA_DIRECTORY        = 0x00000010,   // FILE_ATTRIBUTE_DIRECTORY            = $00000010;
  FA_ARCHIVE          = 0x00000020,   // FILE_ATTRIBUTE_ARCHIVE              = $00000020;
  FA_DEVICE           = 0x00000040,   // FILE_ATTRIBUTE_DEVICE               = $00000040;
  FA_NORMAL           = 0x00000080,   // FILE_ATTRIBUTE_NORMAL               = $00000080;
  FA_TEMPORARY        = 0x00000100,   // FILE_ATTRIBUTE_TEMPORARY            = $00000100;
  FA_SPARSEFILE       = 0x00000200,   // FILE_ATTRIBUTE_SPARSE_FILE          = $00000200;
  FA_REPARSEPOINT     = 0x00000400,   // FILE_ATTRIBUTE_REPARSE_POINT        = $00000400;
  FA_COMPRESSED       = 0x00000800,   // FILE_ATTRIBUTE_COMPRESSED           = $00000800;
  FA_OFFLINE          = 0x00001000,   // FILE_ATTRIBUTE_OFFLINE              = $00001000;
  FANOTCONTENTINDEXED = 0x00002000,// FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  = $00002000;
  FAENCRYPTED         = 0x00004000,   // FILE_ATTRIBUTE_ENCRYPTED           = $00004000;
} eFSAttributeFlags;

typedef enum _FS_SHARE_FLAGS
{
  FS_NONE      = 0x00000000,
  FS_READ      = 0x00000001,
  FS_WRITE     = 0x00000002,
  FS_DELETE    = 0x00000004,
  FS_READWRITE = FS_READ | FS_WRITE,
  FS_ALLACCESS = FS_READ | FS_WRITE | FS_DELETE,
} eFSShareFlags;

typedef enum _FS_GENERIC_FLAGS
{
  FG_ALL       = GENERIC_ALL,
  FG_EXECUTE   = GENERIC_EXECUTE,
  FG_WRITE     = GENERIC_WRITE,
  FG_READ      = GENERIC_READ,
  FG_READWRITE = GENERIC_READ | GENERIC_WRITE,
} eFSGenericFlags;

typedef enum _MOVE_METHOD_FLAGS
{
  MM_BEGIN   = FILE_BEGIN,
  MM_CURRENT = FILE_CURRENT,
  MM_END     = FILE_END,
} eMoveMethodFlags;

typedef struct _FS_OBJECT_A
{
  std::string directory;
  std::string name;
  int64 size;
  ulong attributes;
} TFSObjectA;

typedef struct _FS_OBJECT_W
{
  std::wstring directory;
  std::wstring name;
  int64 size;
  ulong attributes;
} TFSObjectW;

class CFileSystemX : public CLastError
{
public:
  CFileSystemX();
  virtual ~CFileSystemX();

  virtual bool vuapi ready();
  virtual bool vuapi valid(HANDLE handle);
  virtual ulong vuapi get_file_size();

  virtual const CBuffer vuapi read_as_buffer();
  virtual bool vuapi read(void* ptr_buffer, ulong size);
  virtual bool vuapi read(ulong offset, void* ptr_buffer, ulong size, eMoveMethodFlags flags = MM_BEGIN);

  virtual bool vuapi write(const void* ptr_buffer, ulong size);
  virtual bool vuapi write(ulong offset, const void* ptr_buffer, ulong size, eMoveMethodFlags flags = MM_BEGIN);

  virtual bool vuapi seek(ulong offset, eMoveMethodFlags flags);
  virtual bool vuapi io_control(ulong code, void* ptr_send_buffer, ulong send_size, void* ptr_recv_buffer, ulong recv_size);

  virtual bool vuapi close();

protected:
  HANDLE m_handle;

private:
  ulong m_read_size, m_wrote_size;
};

class CFileSystemA: public CFileSystemX
{
public:
  CFileSystemA();
  CFileSystemA(const std::string& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags = FG_READWRITE, eFSShareFlags fs_flags = FS_ALLACCESS, eFSAttributeFlags fa_flags = FA_NORMAL);
  virtual ~CFileSystemA();

  bool vuapi initialize(const std::string& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags = FG_READWRITE, eFSShareFlags fs_flags = FS_ALLACCESS, eFSAttributeFlags fa_flags = FA_NORMAL);
  const std::string vuapi read_as_string(bool remove_bom = true);

  static const std::string vuapi quick_read_as_string(const std::string& file_path, bool force_bom = true);
  static CBuffer quick_read_as_buffer(const std::string& file_path);
  static bool iterate(const std::string& path, const std::string& pattern, const std::function<bool(const TFSObjectA& fso)> fn_callback);
};

class CFileSystemW: public CFileSystemX
{
public:
  CFileSystemW();
  CFileSystemW(const std::wstring& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags = FG_READWRITE, eFSShareFlags fs_flags = FS_ALLACCESS, eFSAttributeFlags fa_flags = FA_NORMAL);
  virtual ~CFileSystemW();

  bool vuapi initialize(const std::wstring& file_path, eFSModeFlags fm_flags, eFSGenericFlags fg_flags = FG_READWRITE, eFSShareFlags fs_flags = FS_ALLACCESS, eFSAttributeFlags fa_flags = FA_NORMAL);
  const std::wstring vuapi read_as_string(bool remove_bom = true);

  static const std::wstring vuapi quick_read_as_string(const std::wstring& file_path, bool remove_bom = true);
  static CBuffer vuapi quick_read_as_buffer(const std::wstring& file_path);
  static bool vuapi iterate(const std::wstring& path, const std::wstring& pattern, const std::function<bool(const TFSObjectW& fso)> fn_callback);
};

/**
 * Service Working
 */

#define VU_SERVICE_ALL_TYPES  -1
#define VU_SERVICE_ALL_STATES -1

 /**
  * CServiceManagerT
  */

template <typename ServiceT>
class CServiceManagerT : public CLastError
{
public:
  typedef std::vector<ServiceT> TServices;

  CServiceManagerT();
  virtual ~CServiceManagerT();

  virtual void Refresh();
  virtual TServices GetServices(
    ulong types = VU_SERVICE_ALL_TYPES, ulong states = VU_SERVICE_ALL_STATES);

protected:
  virtual VUResult Initialize() = 0;

protected:
  bool m_Initialized;
  SC_HANDLE m_Manager;
  TServices m_Services;
};

/**
 * CServiceManagerA
 */

typedef CServiceManagerT<ENUM_SERVICE_STATUS_PROCESSA> CServiceManagerTA;

class CServiceManagerA : public CServiceManagerTA, public CSingletonT<CServiceManagerA>
{
public:
  CServiceManagerA();
  virtual ~CServiceManagerA();

  TServices Find(const std::string& str, bool exact = false, bool nameonly = false);
  std::unique_ptr<TServices::value_type> Query(const std::string& service_name);
  int GetState(const std::string& service_name);

  TServices GetDependents(const std::string& service_name, const ulong states = VU_SERVICE_ALL_STATES);
  TServices GetDependencies(const std::string& service_name, const ulong states = VU_SERVICE_ALL_STATES);

  std::unique_ptr<SERVICE_STATUS> Control(const TServices::value_type* pService, const ulong ctrlcode);
  std::unique_ptr<SERVICE_STATUS> Control(const std::string& name, const ulong ctrlcode);

  VUResult Create(
    const std::string& file_path,
    const std::string& service_name,
    const std::string& display_name,
    const ulong service_type = SERVICE_KERNEL_DRIVER,
    const ulong access_type = SERVICE_ALL_ACCESS,
    const ulong start_type = SERVICE_DEMAND_START,
    const ulong ctrl_type = SERVICE_ERROR_NORMAL
  );

  VUResult Delete(const std::string& name);
  VUResult Start(const std::string& name);
  VUResult Stop(const std::string& name);

protected:
  virtual VUResult Initialize();
};

/**
 * CServiceManagerW
 */

typedef CServiceManagerT<ENUM_SERVICE_STATUS_PROCESSW> CServiceManagerTW;

class CServiceManagerW : public CServiceManagerTW, public CSingletonT<CServiceManagerW>
{
public:
  CServiceManagerW();
  virtual ~CServiceManagerW();

  TServices Find(const std::wstring& str, bool exact = false, bool nameonly = false);
  std::unique_ptr<TServices::value_type> Query(const std::wstring& service_name);
  int GetState(const std::wstring& service_name); // https://docs.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-controlservice#remarks

  TServices GetDependents(const std::wstring& service_name, const ulong states = VU_SERVICE_ALL_STATES);
  TServices GetDependencies(const std::wstring& service_name, const ulong states = VU_SERVICE_ALL_STATES);

  std::unique_ptr<SERVICE_STATUS> Control(const TServices::value_type* pService, const ulong ctrlcode);
  std::unique_ptr<SERVICE_STATUS> Control(const std::wstring& name, const ulong ctrlcode);

  VUResult Create(
    const std::wstring& file_path,
    const std::wstring& service_name,
    const std::wstring& display_name,
    const ulong service_type = SERVICE_KERNEL_DRIVER,
    const ulong access_type = SERVICE_ALL_ACCESS,
    const ulong start_type = SERVICE_DEMAND_START,
    const ulong ctrl_type = SERVICE_ERROR_NORMAL
  );

  VUResult Delete(const std::wstring& name);
  VUResult Start(const std::wstring& name);
  VUResult Stop(const std::wstring& name);

protected:
  virtual VUResult Initialize();
};

/**
 * File Mapping
 */

typedef enum _DESIRED_ACCESS
{
  DA_UNKNOWN = -1,
  DA_ALL_ACCESS = FILE_MAP_ALL_ACCESS,
  DA_READ = FILE_MAP_READ,
  DA_WRITE = FILE_MAP_WRITE,
  DA_COPY = FILE_MAP_COPY,
  DA_EXECUTE = FILE_MAP_EXECUTE,
  // DA_LARGE_PAGES = FILE_MAP_LARGE_PAGES,
  // DA_TARGETS_INVALID = FILE_MAP_TARGETS_INVALID,
} eFMDesiredAccess;

typedef enum _PAGE_PROTECTION
{
  PP_EXECUTE_READ = PAGE_EXECUTE_READ,
  PP_EXECUTE_READ_WRITE = PAGE_EXECUTE_READWRITE,
  PP_EXECUTE_WRITE_COPY = PAGE_EXECUTE_WRITECOPY,
  PP_READ_ONLY = PAGE_READONLY,
  PP_READ_WRITE = PAGE_READWRITE,
  PP_WRITE_COPY = PAGE_WRITECOPY,
  PP_SEC_COMMIT = SEC_COMMIT,
  PP_SEC_IMAGE = SEC_IMAGE,
  PP_SEC_IMAGE_NO_EXECUTE = SEC_IMAGE_NO_EXECUTE,
  PP_SEC_LARGE_PAGES = SEC_LARGE_PAGES,
  PP_SEC_NO_CACHE = SEC_NOCACHE,
  PP_SEC_RESERVE = SEC_RESERVE,
  PP_SEC_WRITE_COMBINE = SEC_WRITECOMBINE,
} ePageProtection;

class CFileMappingX : public CLastError
{
public:
  CFileMappingX();
  virtual ~CFileMappingX();

  void* vuapi View(
    eFMDesiredAccess fmDesiredAccess = eFMDesiredAccess::DA_ALL_ACCESS,
    ulong ulMaxFileOffsetLow = 0,
    ulong ulMaxFileOffsetHigh = 0,
    ulong ulNumberOfBytesToMap = 0 // The mapping extends from the specified offset to the end.
  );

  ulong vuapi GetFileSize();
  void vuapi Close();

protected:
  bool Valid(HANDLE Handle);

protected:
  HANDLE m_FileHandle;
  HANDLE m_MapHandle;
  void* m_pData;
};

/**
 * CFileMappingA
 */

class CFileMappingA : public CFileMappingX
{
public:
  CFileMappingA();
  virtual ~CFileMappingA();

  VUResult vuapi CreateWithinFile(
    const std::string& FileName,
    ulong ulMaxSizeLow  = 0, // The file to map, Hi & Low = 0 is mapping whole file as default.
    ulong ulMaxSizeHigh = 0, // The file to map, Hi & Low = 0 is mapping whole file as default.
    eFSGenericFlags fgFlag = eFSGenericFlags::FG_ALL,
    eFSShareFlags fsFlag = eFSShareFlags::FS_ALLACCESS,
    eFSModeFlags fmFlag = eFSModeFlags::FM_OPENEXISTING,
    eFSAttributeFlags faFlag = eFSAttributeFlags::FA_NORMAL,
    ePageProtection ppFlag = ePageProtection::PP_EXECUTE_READ_WRITE
  );

  /**
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi CreateNamedSharedMemory(
    const std::string& MappingName,
    ulong ulMaxSizeLow, // The mapping size for file mapping object.
    ulong ulMaxSizeHigh = 0,
    ePageProtection ppFlag = ePageProtection::PP_EXECUTE_READ_WRITE
  );

  /**
   * Just for Named Shared Memory.
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi Open(
    const std::string& MappingName,
    eFMDesiredAccess fmDesiredAccess = eFMDesiredAccess::DA_ALL_ACCESS,
    bool bInheritHandle = false
  );
};

/**
 * CFileMappingW
 */

class CFileMappingW : public CFileMappingX
{
public:
  CFileMappingW();
  virtual ~CFileMappingW();

  VUResult vuapi CreateWithinFile(
    const std::wstring& FileName,
    ulong ulMaxSizeLow  = 0, // The file to map, Hi & Low = 0 is mapping whole file as default.
    ulong ulMaxSizeHigh = 0, // The file to map, Hi & Low = 0 is mapping whole file as default.
    eFSGenericFlags fgFlag = eFSGenericFlags::FG_ALL,
    eFSShareFlags fsFlag = eFSShareFlags::FS_ALLACCESS,
    eFSModeFlags fmFlag = eFSModeFlags::FM_OPENEXISTING,
    eFSAttributeFlags faFlag = eFSAttributeFlags::FA_NORMAL,
    ePageProtection ppFlag = ePageProtection::PP_EXECUTE_READ_WRITE
  );

  /**
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi CreateNamedSharedMemory(
    const std::wstring& MappingName,
    ulong ulMaxSizeLow, // The mapping size for file mapping object.
    ulong ulMaxSizeHigh = 0,
    ePageProtection ppFlag = ePageProtection::PP_EXECUTE_READ_WRITE
  );

  /**
   * Just for Named Shared Memory.
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi Open(
    const std::wstring& MappingName,
    eFMDesiredAccess fmDesiredAccess = eFMDesiredAccess::DA_ALL_ACCESS,
    bool bInheritHandle = false
  );
};

/**
 * INI File
 */

class CINIFileA : public CLastError
{
public:
  CINIFileA();
  CINIFileA(const std::string& FilePath);
  virtual ~CINIFileA();

  void SetCurrentFilePath(const std::string& FilePath);
  void SetCurrentSection(const std::string& Section);

  std::vector<std::string> vuapi ReadSection(const std::string& Section, ulong ulMaxSize = MAXBYTE);
  std::vector<std::string> vuapi ReadSection(ulong ulMaxSize = MAXBYTE);

  std::vector<std::string> vuapi ReadSectionNames(ulong ulMaxSize = MAXBYTE);

  int vuapi ReadInteger(const std::string& Section, const std::string& Key, int Default);
  bool vuapi ReadBool(const std::string& Section, const std::string& Key, bool Default);
  float vuapi ReadFloat(const std::string& Section, const std::string& Key, float Default);
  std::string vuapi ReadString(
    const std::string& Section,
    const std::string& Key,
    const std::string& Default
  );
  std::unique_ptr<uchar[]> vuapi ReadStruct(
    const std::string& Section,
    const std::string& Key,
    ulong ulSize
  );

  int vuapi ReadInteger(const std::string& Key, int Default);
  bool vuapi ReadBool(const std::string& Key, bool Default);
  float vuapi ReadFloat(const std::string& Key, float Default);
  std::string vuapi ReadString(const std::string& Key, const std::string& Default);
  std::unique_ptr<uchar[]> vuapi ReadStruct(const std::string& Key, ulong ulSize);

  bool vuapi WriteInteger(const std::string& Section, const std::string& Key, int Value);
  bool vuapi WriteBool(const std::string& Section, const std::string& Key, bool Value);
  bool vuapi WriteFloat(const std::string& Section, const std::string& Key, float Value);
  bool vuapi WriteString(
    const std::string& Section,
    const std::string& Key,
    const std::string& Value
  );
  bool vuapi WriteStruct(
    const std::string& Section,
    const std::string& Key,
    void* pStruct,
    ulong ulSize
  );

  bool vuapi WriteInteger(const std::string& Key, int Value);
  bool vuapi WriteBool(const std::string& Key, bool Value);
  bool vuapi WriteFloat(const std::string& Key, float Value);
  bool vuapi WriteString(const std::string& Key, const std::string& Value);
  bool vuapi WriteStruct(const std::string& Key, void* pStruct, ulong ulSize);

private:
  void ValidFilePath();

private:
  std::string m_FilePath;
  std::string m_Section;
};

class CINIFileW : public CLastError
{
public:
  CINIFileW();
  CINIFileW(const std::wstring& FilePath);
  virtual ~CINIFileW();

  void SetCurrentFilePath(const std::wstring& FilePath);
  void SetCurrentSection(const std::wstring& Section);

  std::vector<std::wstring> vuapi ReadSection(
    const std::wstring& Section,
    ulong ulMaxSize = MAXBYTE
  );
  std::vector<std::wstring> vuapi ReadSection(ulong ulMaxSize = MAXBYTE);

  std::vector<std::wstring> vuapi ReadSectionNames(ulong ulMaxSize = MAXBYTE);

  int vuapi ReadInteger(const std::wstring& Section, const std::wstring& Key, int Default);
  bool vuapi ReadBool(const std::wstring& Section, const std::wstring& Key, bool Default);
  float vuapi ReadFloat(const std::wstring& Section, const std::wstring& Key, float Default);
  std::wstring vuapi ReadString(
    const std::wstring& Section,
    const std::wstring& Key,
    const std::wstring& Default
  );
  std::unique_ptr<uchar[]> vuapi ReadStruct(
    const std::wstring& Section,
    const std::wstring& Key,
    ulong ulSize
  );

  int vuapi ReadInteger(const std::wstring& Key, int Default);
  bool vuapi ReadBool(const std::wstring& Key, bool Default);
  float vuapi ReadFloat(const std::wstring& Key, float Default);
  std::wstring vuapi ReadString(const std::wstring& Key, const std::wstring& Default);
  std::unique_ptr<uchar[]> vuapi ReadStruct(const std::wstring& Key, ulong ulSize);

  bool vuapi WriteInteger(const std::wstring& Section, const std::wstring& Key, int Value);
  bool vuapi WriteBool(const std::wstring& Section, const std::wstring& Key, bool Value);
  bool vuapi WriteFloat(const std::wstring& Section, const std::wstring& Key, float Value);
  bool vuapi WriteString(
    const std::wstring& Section,
    const std::wstring& Key,
    const std::wstring& Value
  );
  bool vuapi WriteStruct(
    const std::wstring& Section,
    const std::wstring& Key,
    void* pStruct,
    ulong ulSize
  );

  bool vuapi WriteInteger(const std::wstring& Key, int Value);
  bool vuapi WriteBool(const std::wstring& Key, bool Value);
  bool vuapi WriteFloat(const std::wstring& Key, float Value);
  bool vuapi WriteString(const std::wstring& Key, const std::wstring& Value);
  bool vuapi WriteStruct(const std::wstring& Key, void* pStruct, ulong ulSize);

private:
  void ValidFilePath();

private:
  std::wstring m_FilePath;
  std::wstring m_Section;
};

/**
 * Registry
 */

// Refer to winreg.h
// Reserved Key Handles.
//

// #define HKEY_CLASSES_ROOT                 (( HKEY ) (ULONG_PTR)((LONG)0x80000000) )
// #define HKEY_CURRENT_USER                 (( HKEY ) (ULONG_PTR)((LONG)0x80000001) )
// #define HKEY_LOCAL_MACHINE                (( HKEY ) (ULONG_PTR)((LONG)0x80000002) )
// #define HKEY_USERS                        (( HKEY ) (ULONG_PTR)((LONG)0x80000003) )
// #define HKEY_PERFORMANCE_DATA             (( HKEY ) (ULONG_PTR)((LONG)0x80000004) )
// #define HKEY_PERFORMANCE_TEXT             (( HKEY ) (ULONG_PTR)((LONG)0x80000050) )
// #define HKEY_PERFORMANCE_NLSTEXT          (( HKEY ) (ULONG_PTR)((LONG)0x80000060) )
// #define HKEY_CURRENT_CONFIG               (( HKEY ) (ULONG_PTR)((LONG)0x80000005) )
// #define HKEY_DYN_DATA                     (( HKEY ) (ULONG_PTR)((LONG)0x80000006) )
// #define HKEY_CURRENT_USER_LOCAL_SETTINGS  (( HKEY ) (ULONG_PTR)((LONG)0x80000007) )

typedef enum _HKEY : ulongptr
{
  HKCR = ulongptr(0x80000000),
  HKCU = ulongptr(0x80000001),
  HKLM = ulongptr(0x80000002),
  HKU  = ulongptr(0x80000003),
  HKPD = ulongptr(0x80000004),
  HKCF = ulongptr(0x80000005),
} eRegRoot;

#ifndef KEY_WOW64_64KEY
#define KEY_WOW64_64KEY 0x0100
#endif

#ifndef KEY_WOW64_32KEY
#define KEY_WOW64_32KEY 0x0200
#endif

#ifndef KEY_WOW64_RES
#define KEY_WOW64_RES 0x0300
#endif

typedef enum _REG_ACCESS
{
  RA_UNKNOWN            = -1,
  RA_QUERY_VALUE        = KEY_QUERY_VALUE,
  RA_SET_VALUE          = KEY_SET_VALUE,
  RA_CREATE_SUB_KEY     = KEY_CREATE_SUB_KEY,
  RA_ENUMERATE_SUB_KEYS = KEY_ENUMERATE_SUB_KEYS,
  RA_NOTIFY             = KEY_NOTIFY,
  RA_CREATE_LINK        = KEY_CREATE_LINK,
  RA_WOW64_64KEY        = KEY_WOW64_64KEY,
  RA_WOW64_32KEY        = KEY_WOW64_32KEY,
  RA_WOW64_RES          = KEY_WOW64_RES,
  RA_READ               = KEY_READ,
  RA_WRITE              = KEY_WRITE,
  RA_EXECUTE            = KEY_EXECUTE,
  RA_ALL_ACCESS         = KEY_ALL_ACCESS,
} eRegAccess;

typedef enum _REG_REFLECTION
{
  RR_ERROR    = -1,
  RR_DISABLED = 0,
  RR_ENABLED  = 1,
  RR_DISABLE  = 2,
  RR_ENABLE   = 3,
} eRegReflection;

class CRegistryX : public CLastError
{
public:
  CRegistryX();
  virtual ~CRegistryX();

  HKEY vuapi GetCurrentKeyHandle();
  eRegReflection vuapi QueryReflectionKey();
  bool vuapi SetReflectionKey(eRegReflection RegReflection);
  bool vuapi CloseKey();

protected:
  HKEY m_HKRootKey;
  HKEY m_HKSubKey;
};

class CRegistryA : public CRegistryX
{
public:
  CRegistryA();
  CRegistryA(eRegRoot RegRoot);
  CRegistryA(eRegRoot RegRoot, const std::string& SubKey);
  virtual ~CRegistryA();

  ulong vuapi GetSizeOfMultiString(const char* lpcszMultiString);
  ulong vuapi GetDataSize(const std::string& ValueName, ulong ulType);

  bool vuapi CreateKey();
  bool vuapi CreateKey(const std::string& SubKey);
  bool vuapi KeyExists();
  bool vuapi KeyExists(const std::string& SubKey);
  bool vuapi OpenKey(eRegAccess RegAccess = eRegAccess::RA_ALL_ACCESS);
  bool vuapi OpenKey(const std::string& SubKey, eRegAccess RegAccess = eRegAccess::RA_ALL_ACCESS);
  bool vuapi DeleteKey();
  bool vuapi DeleteKey(const std::string& SubKey);
  bool vuapi DeleteValue(const std::string& Value);

  std::vector<std::string> vuapi EnumKeys();
  std::vector<std::string> vuapi EnumValues();

  bool vuapi WriteInteger(const std::string& ValueName, int Value);
  bool vuapi WriteBool(const std::string& ValueName, bool Value);
  bool vuapi WriteFloat(const std::string& ValueName, float Value);
  bool vuapi WriteString(const std::string& ValueName, const std::string& Value);
  bool vuapi WriteMultiString(const std::string& ValueName, const char* lpValue);
  bool vuapi WriteMultiString(const std::string& ValueName, const std::vector<std::string>& Value);
  bool vuapi WriteExpandString(const std::string& ValueName, const std::string& Value);
  bool vuapi WriteBinary(const std::string& ValueName, void* pData, ulong ulSize);

  int vuapi ReadInteger(const std::string& ValueName, int Default);
  bool vuapi ReadBool(const std::string& ValueName, bool Default);
  float vuapi ReadFloat(const std::string& ValueName, float Default);
  std::string vuapi ReadString(const std::string& ValueName, const std::string& Default);
  std::vector<std::string> vuapi ReadMultiString(
    const std::string& ValueName,
    const std::vector<std::string> Default
  );
  std::string vuapi ReadExpandString(const std::string& ValueName, const std::string& Default);
  std::unique_ptr<uchar[]> vuapi ReadBinary(const std::string& ValueName, const void* pDefault);

private:
  std::string m_SubKey;
};

class CRegistryW : public CRegistryX
{
public:
  CRegistryW();
  CRegistryW(eRegRoot RegRoot);
  CRegistryW(eRegRoot RegRoot, const std::wstring& SubKey);
  virtual ~CRegistryW();

  ulong vuapi GetSizeOfMultiString(const wchar* lpcwszMultiString);
  ulong vuapi GetDataSize(const std::wstring& ValueName, ulong ulType);

  bool vuapi CreateKey();
  bool vuapi CreateKey(const std::wstring& SubKey);
  bool vuapi KeyExists();
  bool vuapi KeyExists(const std::wstring& SubKey);
  bool vuapi OpenKey(eRegAccess RegAccess = eRegAccess::RA_ALL_ACCESS);
  bool vuapi OpenKey(const std::wstring& SubKey, eRegAccess RegAccess = eRegAccess::RA_ALL_ACCESS);
  bool vuapi DeleteKey();
  bool vuapi DeleteKey(const std::wstring& SubKey);
  bool vuapi DeleteValue(const std::wstring& Value);

  std::vector<std::wstring> vuapi EnumKeys();
  std::vector<std::wstring> vuapi EnumValues();

  bool vuapi WriteInteger(const std::wstring& ValueName, int Value);
  bool vuapi WriteBool(const std::wstring& ValueName, bool Value);
  bool vuapi WriteFloat(const std::wstring& ValueName, float Value);
  bool vuapi WriteString(const std::wstring& ValueName, const std::wstring& Value);
  bool vuapi WriteMultiString(const std::wstring& ValueName, const wchar* Value);
  bool vuapi WriteMultiString(const std::wstring& ValueName, const std::vector<std::wstring> Value);
  bool vuapi WriteExpandString(const std::wstring& ValueName, const std::wstring& Value);
  bool vuapi WriteBinary(const std::wstring& ValueName, void* pData, ulong ulSize);

  int vuapi ReadInteger(const std::wstring& ValueName, int Default);
  bool vuapi ReadBool(const std::wstring& ValueName, bool Default);
  float vuapi ReadFloat(const std::wstring& ValueName, float Default);
  std::wstring vuapi ReadString(const std::wstring& ValueName, const std::wstring& Default);
  std::vector<std::wstring> vuapi ReadMultiString(
    const std::wstring& ValueName,
    const std::vector<std::wstring> Default
  );
  std::wstring vuapi ReadExpandString(const std::wstring& ValueName, const std::wstring& Default);
  std::unique_ptr<uchar[]> vuapi ReadBinary(const std::wstring& ValueName, const void* Default);

private:
  std::wstring m_SubKey;
};

/**
 * Critical Section
 */

typedef CRITICAL_SECTION    TCriticalSection, *PCriticalSection;

class CCriticalSection
{
public:
  CCriticalSection();
  virtual ~CCriticalSection();

  void vuapi Init();
  void vuapi Enter();
  void vuapi Leave();
  void vuapi Destroy();

  TCriticalSection& vuapi GetCurrentSection();

private:
  TCriticalSection m_CriticalSection;
};

/**
 * Stop Watch
 */

class CStopWatch
{
public:
  typedef std::pair<unsigned long, float> TDuration;

public:
  CStopWatch();
  ~CStopWatch();
  void Start(bool reset = false);
  const TDuration Stop();
  const TDuration Duration();
  const TDuration Total();

private:
  bool m_Reset;
  std::clock_t m_Count, m_Delta;
  std::vector<std::clock_t> m_DeltaHistory;
};

/**
 * CScopeStopWatchX
 */

class CScopeStopWatchX
{
public:
  CScopeStopWatchX();
  virtual ~CScopeStopWatchX();

  void Active(bool state = true);
  void Reset();

protected:
  virtual void Start(bool reset = false);
  virtual void Stop();

protected:
  bool m_Activated;
  CStopWatch m_Watcher;
};

class CScopeStopWatchA : public CScopeStopWatchX
{
public:
  typedef std::function<void(const std::string& id, const CStopWatch::TDuration& duration)> FnLogging;

  CScopeStopWatchA(const std::string& prefix, const FnLogging fnLogging = Message);
  virtual ~CScopeStopWatchA();

  void Log(const std::string& id = "");

  static void Message(const std::string& id, const CStopWatch::TDuration& duration);
  static void Console(const std::string& id, const CStopWatch::TDuration& duration);

private:
  std::string m_Prefix;
  FnLogging m_fnLogging;
};

class CScopeStopWatchW : public CScopeStopWatchX
{
public:
  typedef std::function<void(const std::wstring& id, const CStopWatch::TDuration& duration)> FnLogging;

  CScopeStopWatchW(const std::wstring& prefix, const FnLogging fnLogging = Message);
  virtual ~CScopeStopWatchW();

  void Log(const std::wstring& id = L"");

  static void Message(const std::wstring& id, const CStopWatch::TDuration& duration);
  static void Console(const std::wstring& id, const CStopWatch::TDuration& duration);

private:
  std::wstring m_Prefix;
  FnLogging m_fnLogging;
};

/**
 * PE File
 */

const ulong MAX_IDD = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;

typedef ulong32 pe32;
typedef ulong64 pe64;

#ifdef _M_IX86
typedef pe32 peX;
#else  // _M_AMD64
typedef pe64 peX;
#endif // _M_IX86

typedef IMAGE_DOS_HEADER TDOSHeader, *PDOSHeader;
typedef IMAGE_FILE_HEADER TFileHeader, *PFileHeader;
typedef _IMAGE_SECTION_HEADER TSectionHeader, *PSectionHeader;
typedef IMAGE_IMPORT_BY_NAME TImportByName, *PImportByName;
typedef IMAGE_IMPORT_DESCRIPTOR TImportDescriptor, *PImportDescriptor;
typedef IMAGE_DATA_DIRECTORY TDataDirectory, *PDataDirectory;

// IMAGE_OPTIONAL_HEADER

template <typename T>
struct TOptHeaderT
{
  ushort  Magic;
  uchar   MajorLinkerVersion;
  uchar   MinorLinkerVersion;
  ulong   SizeOfCode;
  ulong   SizeOfInitializedData;
  ulong   SizeOfUninitializedData;
  ulong   AddressOfEntryPoint;
  ulong   BaseOfCode;
  ulong   BaseOfData;
  T       ImageBase;
  ulong   SectionAlignment;
  ulong   FileAlignment;
  ushort  MajorOperatingSystemVersion;
  ushort  MinorOperatingSystemVersion;
  ushort  MajorImageVersion;
  ushort  MinorImageVersion;
  ushort  MajorSubsystemVersion;
  ushort  MinorSubsystemVersion;
  ulong   Win32VersionValue;
  ulong   SizeOfImage;
  ulong   SizeOfHeaders;
  ulong   CheckSum;
  ushort  Subsystem;
  ushort  DllCharacteristics;
  T       SizeOfStackReserve;
  T       SizeOfStackCommit;
  T       SizeOfHeapReserve;
  T       SizeOfHeapCommit;
  ulong   LoaderFlags;
  ulong   NumberOfRvaAndSizes;
  // IMAGE_DATA_DIRECTORY - https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-imagedirectoryentrytodataex
  union
  {
    struct
    {
      TDataDirectory Export;        // IMAGE_DIRECTORY_ENTRY_EXPORT
      TDataDirectory Import;        // IMAGE_DIRECTORY_ENTRY_IMPORT
      TDataDirectory Resource;      // IMAGE_DIRECTORY_ENTRY_RESOURCE
      TDataDirectory Exception;     // IMAGE_DIRECTORY_ENTRY_EXCEPTION
      TDataDirectory Security;      // IMAGE_DIRECTORY_ENTRY_SECURITY
      TDataDirectory Relocation;    // IMAGE_DIRECTORY_ENTRY_BASERELOC
      TDataDirectory Debug;         // IMAGE_DIRECTORY_ENTRY_DEBUG
      TDataDirectory Architecture;  // IMAGE_DIRECTORY_ENTRY_ARCHITECTURE
      TDataDirectory Global;        // IMAGE_DIRECTORY_ENTRY_GLOBALPTR
      TDataDirectory TLS;           // IMAGE_DIRECTORY_ENTRY_TLS
      TDataDirectory Config;        // IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG
      TDataDirectory Bound;         // IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT
      TDataDirectory IAT;           // IMAGE_DIRECTORY_ENTRY_IAT
      TDataDirectory Delay;         // IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT
      TDataDirectory CLR;           // IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR
      TDataDirectory Reversed;      // Reversed
    };
    TDataDirectory DataDirectory[MAX_IDD];
  };
};

template<> struct TOptHeaderT<pe64>
{
  ushort  Magic;
  uchar   MajorLinkerVersion;
  uchar   MinorLinkerVersion;
  ulong   SizeOfCode;
  ulong   SizeOfInitializedData;
  ulong   SizeOfUninitializedData;
  ulong   AddressOfEntryPoint;
  ulong   BaseOfCode;
  // ulong   BaseOfData // not used for 64-bit
  ulong64 ImageBase;
  ulong   SectionAlignment;
  ulong   FileAlignment;
  ushort  MajorOperatingSystemVersion;
  ushort  MinorOperatingSystemVersion;
  ushort  MajorImageVersion;
  ushort  MinorImageVersion;
  ushort  MajorSubsystemVersion;
  ushort  MinorSubsystemVersion;
  ulong   Win32VersionValue;
  ulong   SizeOfImage;
  ulong   SizeOfHeaders;
  ulong   CheckSum;
  ushort  Subsystem;
  ushort  DllCharacteristics;
  ulong64 SizeOfStackReserve;
  ulong64 SizeOfStackCommit;
  ulong64 SizeOfHeapReserve;
  ulong64 SizeOfHeapCommit;
  ulong   LoaderFlags;
  ulong   NumberOfRvaAndSizes;
  // IMAGE_DATA_DIRECTORY - https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-imagedirectoryentrytodataex
  union
  {
    struct
    {
      TDataDirectory Export;        // IMAGE_DIRECTORY_ENTRY_EXPORT
      TDataDirectory Import;        // IMAGE_DIRECTORY_ENTRY_IMPORT
      TDataDirectory Resource;      // IMAGE_DIRECTORY_ENTRY_RESOURCE
      TDataDirectory Exception;     // IMAGE_DIRECTORY_ENTRY_EXCEPTION
      TDataDirectory Security;      // IMAGE_DIRECTORY_ENTRY_SECURITY
      TDataDirectory Relocation;    // IMAGE_DIRECTORY_ENTRY_BASERELOC
      TDataDirectory Debug;         // IMAGE_DIRECTORY_ENTRY_DEBUG
      TDataDirectory Architecture;  // IMAGE_DIRECTORY_ENTRY_ARCHITECTURE
      TDataDirectory Global;        // IMAGE_DIRECTORY_ENTRY_GLOBALPTR
      TDataDirectory TLS;           // IMAGE_DIRECTORY_ENTRY_TLS
      TDataDirectory Config;        // IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG
      TDataDirectory Bound;         // IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT
      TDataDirectory IAT;           // IMAGE_DIRECTORY_ENTRY_IAT
      TDataDirectory Delay;         // IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT
      TDataDirectory CLR;           // IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR
      TDataDirectory Reversed;      // Reversed
    };
    TDataDirectory DataDirectory[MAX_IDD];
  };
};

typedef TOptHeaderT<ulong32> TOptHeader32, *POptHeader32;
typedef TOptHeaderT<ulong64> TOptHeader64, *POptHeader64;

// IMAGE_NT_HEADERS

template <typename T>
struct TNTHeaderT
{
  ulong Signature;
  TFileHeader FileHeader;
  TOptHeaderT<T> OptHeader;
};

typedef TNTHeaderT<ulong32> TNTHeader32,  *PNTHeader32;
typedef TNTHeaderT<ulong64> TNTHeader64, *PNTHeader64;

// PE_HEADER

#define TPEHeaderT TNTHeaderT

typedef TNTHeader32 TPEHeader32, *PPEHeader32;
typedef TNTHeader64 TPEHeader64, *PPEHeader64;

// IMAGE_THUNK_DATA

template <typename T>
struct TThunkDataT
{
  union
  {
    T ForwarderString;
    T Function;
    T Ordinal;
    T AddressOfData; // PIMAGE_IMPORT_BY_NAME
  } u1;
};

typedef TThunkDataT<ulong32>  TThunkData32, *PThunkData32;
typedef TThunkDataT<ulong64>  TThunkData64, *PThunkData64;

/* The common types (32-bit & 64-bit)  */

#ifdef _WIN64
typedef TNTHeader64   TNTHeader,  *PNTHeader;
typedef TOptHeader64  TOptHeader, *POptHeader;
typedef TThunkData64  TThunkData, *PThunkData;
typedef TPEHeader64   TPEHeader,  *PPEHeader;
#else // _WIN32
typedef TNTHeader32   TNTHeader,  *PNTHeader;
typedef TOptHeader32  TOptHeader, *POptHeader;
typedef TThunkData32  TThunkData, *PThunkData;
typedef TPEHeader32   TPEHeader,  *PPEHeader;
#endif

typedef struct
{
  ulong IIDID;
  std::string Name;
  PImportDescriptor pIID;
} TExIID;

typedef struct
{
  ulong IIDID;
  std::string Name;
  //ulong NumberOfFuctions;
} TImportModule;

template<typename T>
struct TImportFunctionT
{
  ulong IIDID;
  std::string Name;
  T Ordinal;
  ushort Hint;
  T RVA;
};

typedef TImportFunctionT<ulong32> TFunctionInfo32;
typedef TImportFunctionT<ulong64> TFunctionInfo64;

typedef enum _IMPORTED_FUNCTION_FIND_BY
{
  IFFM_HINT,
  IFFM_NAME,
} eImportedFunctionFindMethod;

template<typename T>
struct TRelocationEntryT
{
  int Type;
  // T Offset;
  T RVA;
  // T VA;
  T Value;
};

template <typename T>
class CPEFileTX
{
public:
  CPEFileTX();
  virtual ~CPEFileTX();

  void* vuapi GetpBase();
  TPEHeaderT<T>* vuapi GetpPEHeader();

  T vuapi RVA2Offset(T RVA, bool InCache = true);
  T vuapi Offset2RVA(T Offset, bool InCache = true);

  const std::vector<PSectionHeader>& vuapi GetSetionHeaders(bool InCache = true);

  const std::vector<PImportDescriptor>& vuapi GetImportDescriptors(bool InCache = true);
  const std::vector<TImportModule> vuapi GetImportModules(bool InCache = true);
  const std::vector<TImportFunctionT<T>> vuapi GetImportFunctions(bool InCache = true); // Didn't include import by index

  const TImportModule* vuapi FindImportModule(const std::string& ModuleName, bool InCache = true);

  const TImportFunctionT<T>* vuapi FindImportFunction(
    const std::string& FunctionName,
    bool InCache = true
  );
  const TImportFunctionT<T>* vuapi FindImportFunction(
    const ushort FunctionHint,
    bool InCache = true
  );
  const TImportFunctionT<T>* vuapi FindImportFunction(
    const TImportFunctionT<T>& ImportFunction,
    eImportedFunctionFindMethod Method,
    bool InCache = true);

  const std::vector<TRelocationEntryT<T>> vuapi GetRelocationEntries(bool InCache = true);

protected:
  bool m_Initialized;

  void* m_pBase;

  TDOSHeader* m_pDosHeader;
  TPEHeaderT<T>* m_pPEHeader;

private:
  T m_OrdinalFlag;

  std::vector<TExIID> m_ExIDDs;

  std::vector<PSectionHeader> m_SectionHeaders;
  std::vector<PImportDescriptor> m_ImportDescriptors;
  std::vector<TImportModule> m_ImportModules;
  std::vector<TImportFunctionT<T>> m_ImportFunctions;
  std::vector<TRelocationEntryT<T>> m_RelocationEntries;

protected:
  const std::vector<TExIID>& vuapi GetExIIDs(bool InCache = true);
};

template <typename T>
class CPEFileTA : public CPEFileTX<T>
{
public:
  CPEFileTA() {};
  CPEFileTA(const std::string& PEFilePath);
  ~CPEFileTA();

  VUResult vuapi Parse(const std::string& PEFilePath = "");

private:
  std::string m_FilePath;
  CFileMappingA m_FileMap;
};

template <typename T>
class CPEFileTW : public CPEFileTX<T>
{
public:
  CPEFileTW() {};
  CPEFileTW(const std::wstring& PEFilePath);
  ~CPEFileTW();

  VUResult vuapi Parse();

private:
  std::wstring m_FilePath;
  CFileMappingW m_FileMap;
};

/**
 * CFundamental
 */

class CFundamentalA
{
public:
  CFundamentalA();
  virtual ~CFundamentalA();

  template<typename T>
  friend CFundamentalA& operator<<(CFundamentalA& stream, T v)
  {
    stream.m_data << v;
    return stream;
  }

  std::stringstream& vuapi data();
  std::string vuapi to_string() const;
  int vuapi to_integer() const;
  long vuapi to_long() const;
  bool vuapi to_boolean() const;
  float vuapi to_float() const;
  double vuapi to_double() const;

private:
  std::stringstream m_data;
};

class CFundamentalW
{
public:
  CFundamentalW();
  virtual ~CFundamentalW();

  template<typename T>
  friend CFundamentalW& operator<<(CFundamentalW& stream, T v)
  {
    stream.m_data << v;
    return stream;
  }

  std::wstringstream& vuapi data();
  std::wstring vuapi to_string() const;
  int vuapi to_integer() const;
  long vuapi to_long() const;
  bool vuapi to_boolean() const;
  float vuapi to_float() const;
  double vuapi to_double() const;

private:
  std::wstringstream m_data;
};

/**
 * CWDTControl
 */

class CWDTControl
{
public:
  enum eControlClass : WORD
  {
    CT_BUTTON     = 0x0080,
    CT_EDIT       = 0x0081,
    CT_STATIC     = 0x0082,
    CT_LIST_BOX   = 0x0083,
    CT_SCROLL_BAR = 0x0084,
    CT_COMBO_BOX  = 0x0085,
    CT_UNKNOW     = 0xFFFF,
  };

  CWDTControl(
    const std::wstring& caption,
    const eControlClass type,
    const WORD  id,
    const short x,
    const short y,
    const short cx,
    const short cy,
    const DWORD style,
    const DWORD exstyle = 0
  );

  virtual ~CWDTControl();

  void serialize(void** pptr);

protected:
  std::wstring m_caption;
  DLGITEMTEMPLATE m_shape;
  WORD m_w_class;
  WORD m_w_type;
  WORD m_w_data;
};

class CWDTDialog : public CLastError
{
public:
  CWDTDialog(
    const std::wstring& caption,
    const DWORD style,
    const DWORD exstyle,
    const short x,
    const short y,
    const short cx,
    const short cy,
    HWND hwnd_parent = nullptr
  );

  virtual ~CWDTDialog();

  void add(const CWDTControl& control);
  void serialize(void** pptr);
  INT_PTR do_modal(DLGPROC pfnDlgProc, CWDTDialog* ptr_parent);

  const std::vector<CWDTControl>& controls() const;

protected:
  HGLOBAL m_h_global;
  std::wstring m_caption;
  std::wstring m_font;
  HWND m_hwnd_parent;
  std::vector<CWDTControl> m_controls;
  DLGTEMPLATE m_shape;
  WORD m_w_menu;
  WORD m_w_class;
  WORD m_w_font;
};

class CInputDialog : public CWDTDialog
{
public:
  const WORD IDC_LABEL;
  const WORD IDC_INPUT;

  CInputDialog(const std::wstring& label, const HWND hwnd_parent = nullptr, bool number_only = false);
  virtual ~CInputDialog();

  void label(const std::wstring& label);
  const std::wstring& label() const;
  CFundamentalW& input();

  INT_PTR do_modal();

  static LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

private:
  std::wstring  m_label;
  CFundamentalW m_input;
  bool m_number_only;
};

/**
 * Process
 */

typedef struct _PROCESS_CPU_COUNTERS
{
  double Usage;
} PROCESS_CPU_COUNTERS;

typedef struct _PROCESS_TIME_COUNTERS
{
  FILETIME CreationTime;
  FILETIME ExitTime;
  FILETIME KernelTime;
  FILETIME UserTime;
} PROCESS_TIME_COUNTERS;

// Structure for GetProcessMemoryInfo()

typedef struct _PROCESS_MEMORY_COUNTERS {
  DWORD cb;
  DWORD PageFaultCount;
  SIZE_T PeakWorkingSetSize;
  SIZE_T WorkingSetSize;
  SIZE_T QuotaPeakPagedPoolUsage;
  SIZE_T QuotaPagedPoolUsage;
  SIZE_T QuotaPeakNonPagedPoolUsage;
  SIZE_T QuotaNonPagedPoolUsage;
  SIZE_T PagefileUsage;
  SIZE_T PeakPagefileUsage;
} PROCESS_MEMORY_COUNTERS;
typedef PROCESS_MEMORY_COUNTERS* PPROCESS_MEMORY_COUNTERS;

typedef IO_COUNTERS PROCESS_IO_COUNTERS;

typedef struct tagTHREADENTRY32
{
  DWORD   dwSize;
  DWORD   cntUsage;
  DWORD   th32ThreadID;       // this thread
  DWORD   th32OwnerProcessID; // Process this thread is associated with
  LONG    tpBasePri;
  LONG    tpDeltaPri;
  DWORD   dwFlags;
} THREADENTRY32;
typedef THREADENTRY32* PTHREADENTRY32;
typedef THREADENTRY32* LPTHREADENTRY32;

typedef struct _MODULEINFO_PTR {
  DWORD_PTR lpBaseOfDll;
  DWORD_PTR SizeOfImage;
  DWORD_PTR EntryPoint;
} MODULEINFO_PTR, * LPMODULEINFO_PTR;

#define MEM_ALL_STATE (MEM_COMMIT | MEM_FREE | MEM_RESERVE)
#define MEM_ALL_TYPE  (MEM_IMAGE | MEM_MAPPED | MEM_PRIVATE)
#define PAGE_ALL_PROTECTION -1

class CProcessX : public CLastError
{
public:
  typedef std::vector<THREADENTRY32> threads;
  typedef std::vector<MEMORY_BASIC_INFORMATION> memories;

  CProcessX();
  // CProcessX(const ulong pid);
  virtual ~CProcessX();

  // CProcessX(CProcessX& right);
  // CProcessX& operator=(CProcessX& right);
  bool operator==(CProcessX& right);
  bool operator!=(CProcessX& right);

  const ulong  pid() const;
  const HANDLE handle() const;
  const eWow64 wow64() const;
  const eXBit  bits() const;

  bool ready();

  bool attach(const ulong pid);
  bool attach(const HANDLE hp);

  bool read_memory(const ulongptr address, CBuffer& buffer);
  bool read_memory(const ulongptr address, void* ptr_data, const size_t size);
  bool write_memory(const ulongptr address, const CBuffer& buffer);
  bool write_memory(const ulongptr address, const void* ptr_data, const size_t size);

  PROCESS_CPU_COUNTERS get_cpu_information(const double interval = 1.); // 1 second
  PROCESS_MEMORY_COUNTERS get_memory_information();
  PROCESS_TIME_COUNTERS get_time_information();
  PROCESS_IO_COUNTERS get_io_information();

  const threads& get_threads();
  const memories& get_memories(const ulong state = MEM_ALL_STATE, const ulong type  = MEM_ALL_TYPE, const ulong protection = PAGE_ALL_PROTECTION);

protected:
  virtual void parse();

private:
  bool close(const HANDLE hp);
  HANDLE open(const ulong pid);
  double get_cpu_percent_usage();

protected:
  eXBit  m_bit;
  ulong  m_pid;
  HANDLE m_handle;
  eWow64 m_wow64;

  int64_t m_last_system_time_UTC;
  int64_t m_last_system_time_per_core_UTC;

  threads m_threads;
  memories m_memories;
};

#ifndef MAX_MODULE_NAME32
#define MAX_MODULE_NAME32 255
#endif // !MAX_MODULE_NAME32

typedef struct tagMODULEENTRY32
{
  DWORD   dwSize;
  DWORD   th32ModuleID;       // This module
  DWORD   th32ProcessID;      // owning process
  DWORD   GlblcntUsage;       // Global usage count on the module
  DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
  BYTE*   modBaseAddr;        // Base address of module in th32ProcessID's context
  DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
  HMODULE hModule;            // The hModule of this module in th32ProcessID's context
  char    szModule[MAX_MODULE_NAME32 + 1];
  char    szExePath[MAX_PATH];
} MODULEENTRY32;
typedef MODULEENTRY32* PMODULEENTRY32;
typedef MODULEENTRY32* LPMODULEENTRY32;

class CProcessA : public CProcessX
{
public:
  typedef std::vector<MODULEENTRY32> modules;

  CProcessA();
  // CProcessA(const ulong PID);
  virtual ~CProcessA();

  // CProcessA(CProcessA& right);
  // CProcessA& operator=(CProcessA& right);
  bool operator==(CProcessA& right);
  bool operator!=(CProcessA& right);
  friend std::ostream& operator<<(std::ostream& os, CProcessA& process);

  const std::string& name() const;
  const modules& get_modules();

  const MODULEENTRY32 get_module_information();

protected:
  virtual void parse();

protected:
  std::string m_name;
  modules m_modules;
};

typedef struct tagMODULEENTRY32W
{
  DWORD   dwSize;
  DWORD   th32ModuleID;       // This module
  DWORD   th32ProcessID;      // owning process
  DWORD   GlblcntUsage;       // Global usage count on the module
  DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
  BYTE*   modBaseAddr;        // Base address of module in th32ProcessID's context
  DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
  HMODULE hModule;            // The hModule of this module in th32ProcessID's context
  WCHAR   szModule[MAX_MODULE_NAME32 + 1];
  WCHAR   szExePath[MAX_PATH];
} MODULEENTRY32W;
typedef MODULEENTRY32W* PMODULEENTRY32W;
typedef MODULEENTRY32W* LPMODULEENTRY32W;

class CProcessW : public CProcessX
{
public:
  typedef std::vector<MODULEENTRY32W> modules;

  CProcessW();
  // CProcessW(const ulong pid);
  virtual ~CProcessW();

  // CProcessW(CProcessW& right);
  // CProcessW& operator=(CProcessW& right);
  bool operator==(CProcessW& right);
  bool operator!=(CProcessW& right);
  friend std::ostream& operator<<(std::ostream& os, CProcessW& process);

  const std::wstring& name() const;
  const modules& get_modules();

  const MODULEENTRY32W get_module_information();

protected:
  virtual void parse();

protected:
  std::wstring m_name;
  modules m_modules;
};

#ifdef Vutils_EXPORTS
#define threadpool11_EXPORTING
#endif // Vutils_EXPORTS

#define MAX_NTHREADS -1

class CThreadPool
{
public:
  CThreadPool(size_t nthreads = MAX_NTHREADS);
  virtual ~CThreadPool();

  void AddTask(FnTask&& fn);
  void Launch();

  size_t WorkerCount() const;
  size_t WorkQueueCount() const;

  size_t ActiveWorkerCount() const;
  size_t InactiveWorkerCount() const;

private:
  Pool* m_pTP;
};

#include "template/stlthread.tpl"

/**
 * CPath
 */

class CPathA
{
public:
  CPathA(const ePathSep Separator = ePathSep::WIN);
  CPathA(const std::string& Path, const ePathSep Separator = ePathSep::WIN);
  CPathA(const CPathA& right);
  virtual ~CPathA();

  const CPathA& operator=(const CPathA& right);
  const CPathA& operator=(const std::string& right);
  const CPathA& operator+=(const CPathA& right);
  const CPathA& operator+=(const std::string& right);
  CPathA operator+(const CPathA& right);
  CPathA operator+(const std::string& right);
  bool operator==(const CPathA& right);
  bool operator!=(const CPathA& right);

  CPathA& Trim(const eTrimType TrimType = eTrimType::TS_BOTH);
  CPathA& Normalize();
  CPathA& Join(const std::string& Path);
  CPathA& Finalize();

  CPathA FileName(bool Extension = true) const;
  CPathA FileDirectory(bool Slash = true) const;

  bool Exists() const;

  std::string AsString() const;

  friend std::ostream& operator<<(std::ostream& os, CPathA& path);

private:
  ePathSep m_Sep;
  std::string m_Path;
};

class CPathW
{
public:
  CPathW(const ePathSep Separator = ePathSep::WIN);
  CPathW(const std::wstring& Path, const ePathSep Separator = ePathSep::WIN);
  CPathW(const CPathW& right);
  virtual ~CPathW();

  const CPathW& operator=(const CPathW& right);
  const CPathW& operator=(const std::wstring& right);
  const CPathW& operator+=(const CPathW& right);
  const CPathW& operator+=(const std::wstring& right);
  CPathW operator+(const CPathW& right);
  CPathW operator+(const std::wstring& right);
  bool operator==(const CPathW& right);
  bool operator!=(const CPathW& right);

  CPathW& Trim(const eTrimType TrimType = eTrimType::TS_BOTH);
  CPathW& Normalize();
  CPathW& Join(const std::wstring& Path);
  CPathW& Finalize();

  CPathW FileName(bool Extension = true) const;
  CPathW FileDirectory(bool Slash = true) const;

  bool Exists() const;

  std::wstring AsString() const;

  friend std::wostream& operator<<(std::wostream& os, CPathW& path);

private:
  ePathSep m_Sep;
  std::wstring m_Path;
};

/**
 * Windows Management Instrumentation
 */

#ifdef VU_WMI_ENABLED

class CWMIProviderX;

/**
 * CWMIProviderA
 */

class CWMIProviderA
{
public:
  CWMIProviderA();
  virtual ~CWMIProviderA();

  virtual bool Ready();
  bool Begin(const std::string& ns);
  bool End();

  IEnumWbemClassObject* Query(const std::string& qs);
  bool Query(const std::string& qs, const std::function<bool(IWbemClassObject& object)> fn);

private:
  CWMIProviderX* m_pImpl;
};

/**
 * CWMIProviderW
 */

class CWMIProviderW
{
public:
  CWMIProviderW();
  virtual ~CWMIProviderW();

  virtual bool Ready();
  bool Begin(const std::wstring& ns);
  bool End();

  IEnumWbemClassObject* Query(const std::wstring& qs);
  bool Query(const std::wstring& qs, const std::function<bool(IWbemClassObject& object)> fn);

private:
  CWMIProviderX* m_pImpl;
};

#endif // VU_WMI_ENABLED

/*---------- The definition of common structure(s) which compatible both ANSI & UNICODE ----------*/

#ifdef _UNICODE
#define TFSObject TFSObjectW
#else
#define TFSObject TFSObjectA
#endif

/*------------ The definition of common Class(es) which compatible both ANSI & UNICODE -----------*/

#ifdef _UNICODE
#define CGUID CGUIDW
#define CINLHook CINLHookW
#define CIATHookManager CIATHookManagerW
#define CWMHook CWMHookW
#define CProcess CProcessW
#define CServiceManager CServiceManagerW
#define CLibrary CLibraryW
#define CFileSystem CFileSystemW
#define CFileMapping CFileMappingW
#define CINIFile CINIFileW
#define CRegistry CRegistryW
#define CPEFileT CPEFileTW
#define CPath CPathW
#define CScopeStopWatch CScopeStopWatchW
#define CWMIProvider CWMIProviderW
#define CFundamental CFundamentalW
#else
#define CGUID CGUIDA
#define CINLHook CINLHookA
#define CIATHookManager CIATHookManagerA
#define CWMHook CWMHookA
#define CProcess CProcessA
#define CServiceManager CServiceManagerA
#define CLibrary CLibraryA
#define CFileSystem CFileSystemA
#define CFileMapping CFileMappingA
#define CINIFile CINIFileA
#define CRegistry CRegistryA
#define CPEFileT CPEFileTA
#define CPath CPathA
#define CScopeStopWatch CScopeStopWatchA
#define CWMIProvider CWMIProviderA
#define CFundamental CFundamentalA
#endif

} // namespace vu

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // VUTILS_H