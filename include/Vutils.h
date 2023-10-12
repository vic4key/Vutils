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
#elif (defined(_MSC_VER) && _MSC_VER < 1700) || (defined(__MINGW32__) && __cplusplus < 201103L)
// C++11 (MSVC 2012+ or MinGW 4.6+)
// https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B#Internal_version_numbering
// https://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros
#error Vutils required C++11 or newer
#endif

/* Vutils Configurations */

// VU_NO_EX - To disable all extended utilities

// Default are enabled for MSVC and C++ Builder. For MinGW, see detail at README.md or INSTALL.md.
// VU_INET_ENABLED    - The Network Communications (Socket, HTTP, HTTP, etc) a.k.a INET
// VU_GUID_ENABLED    - The Globally/Universally Unique Identifier a.k.a GUID
// VU_WMI_ENABLED     - The Windows Management Instrumentation a.k.a WMI

#ifndef VU_NO_EX

#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#define VU_INET_ENABLED
#define VU_GUID_ENABLED
#define VU_WMI_ENABLED
#endif

#endif // VU_NO_EX

#if defined(min) || defined(max)
#define VU_min_max_macro_defined
#endif

/* Header Inclusions */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef VU_INET_ENABLED

#ifndef _WINSOCK2API_
#if defined(_WINDOWS_) || defined(_WINSOCKAPI_)
#define WINDOWS_OR_WINSOCKAPI
#endif // _WINDOWS_ || _WINSOCKAPI_
#endif // _WINSOCK2API_

#ifdef WINDOWS_OR_WINSOCKAPI
#error Vutils required to include above Windows.h or WinSock.h
#else  // WINDOWS_OR_WINSOCKAPI
#include <winsock2.h>
#endif // WINDOWS_OR_WINSOCKAPI

#endif // VU_INET_ENABLED

#include <windows.h>

#ifndef _WIN_SVC_
#include <winsvc.h>
#endif // _WIN_SVC_

#include <set>
#include <map>
#include <cmath>
#include <ctime>
#include <mutex>
#include <regex>
#include <string>
#include <vector>
#include <thread>
#include <memory>
#include <numeric>
#include <sstream>
#include <cassert>
#include <functional>
#include <type_traits>
#include <unordered_map>

#ifdef _MSC_VER
#pragma warning(push)
#endif // _MSC_VER

#ifdef _MSC_VER
#pragma pack(push)
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
  typedef std::function<void()> fn_task_t;
} // threadpool11

using namespace threadpool11;

// BigInt

#include "3rdparty/BI/BigInt.hpp"

// WinHTTP Wrapper

#include "3rdparty/WHW/Types.h"

namespace WinHttpWrapper
{
  class HttpRequest;
}

using namespace WinHttpWrapper;

// Name Operator

#include "template/nameop.tpl"

namespace vu
{

#include "inline/defs.inl"
#include "inline/types.inl"
#include "inline/spechrs.inl"

/* Vutils Constants */

const VUResult VU_OK  = 0;

const size_t MAX_SIZE = MAXBYTE;

/* Vutils Third Party */

typedef BigInt BigNumber;

/* ------------------------------------ Public Function(s) -------------------------------------- */

/**
 * Misc Working
 */

class Buffer;

bool vuapi is_administrator();
bool set_privilege_A(const std::string&  privilege, const bool enable);
bool set_privilege_W(const std::wstring& privilege, const bool enable);
std::string vuapi get_env_A(const std::string& name);
std::wstring vuapi get_env_W(const std::wstring& name);
bool vuapi set_env_A(const std::string& name, const std::string& value);
bool vuapi set_env_W(const std::wstring& name, const std::wstring& value);
std::vector<size_t> find_pattern_A(
  const Buffer& buffer, const std::string&  pattern, const bool first_match_only);
std::vector<size_t> find_pattern_W(
  const Buffer& buffer, const std::wstring& pattern, const bool first_match_only);
std::vector<size_t> find_pattern_A(
  const void* ptr, const size_t size, const std::string& pattern, const bool first_match_only);
std::vector<size_t> find_pattern_W(
  const void* ptr, const size_t size, const std::wstring& pattern, const bool first_match_only);
bool vuapi use_std_io_console_window();

#include "template/misc.tpl"

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

enum class text_encoding : int
{
  TE_UNKNOWN      = -1,
  TE_UTF8         = 0, // "ANSI/UTF-8", "ANSI/UTF-8"
  TE_UTF8_BOM     = 1, // "UTF-8 BOM", "UTF-8 BOM"
  TE_UTF16_LE     = 2, // "Unicode", "UTF-16 Little Endian"
  TE_UTF16_BE     = 3, // "Unicode BE", "UTF-16 Big Endian"
  TE_UTF16_LE_BOM = 4, // "Unicode BOM", "UTF-16 Little Endian BOM"
  TE_UTF16_BE_BOM = 5, // "Unicode BE BOM", "UTF-16 Big Endian BOM"
  //TE_UTF32_LE_BOM = 6, // "UTF-32 LE BOM", "UTF-32 Little Endian BOM"
  //TE_UTF32_BE_BOM = 7, // "UTF-32 BE BOM", "UTF-32 Big Endian BOM"
};

enum class data_unit : int
{
  SI  = 1000, // 1 KB  = 1000 bytes
  IEC = 1024, // 1 KiB = 1024 bytes
};

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
text_encoding vuapi detect_text_encoding(const void* data, const size_t size);
std::string vuapi format_bytes_A(long long bytes, data_unit unit = data_unit::IEC, int digits = 2);
std::wstring vuapi format_bytes_W(long long bytes, data_unit unit = data_unit::IEC, int digits = 2);
std::string vuapi to_hex_string_A(const byte* ptr, const size_t size);
std::wstring vuapi to_hex_string_W(const byte* ptr, const size_t size);
bool vuapi to_hex_bytes_A(const std::string& text, std::vector<byte>& bytes);
bool vuapi to_hex_bytes_W(const std::wstring& text, std::vector<byte>& bytes);
void vuapi url_encode_A(const std::string& text, std::string& result);
void vuapi url_encode_W(const std::wstring& text, std::wstring& result);
void vuapi url_decode_A(const std::string& text, std::string& result);
void vuapi url_decode_W(const std::wstring& text, std::wstring& result);

/**
 * String Working
 */

enum class trim_type
{
  TS_LEFT  = 0,
  TS_RIGHT = 1,
  TS_BOTH  = 2,
};

std::string vuapi lower_string_A(const std::string& string);
std::wstring vuapi lower_string_W(const std::wstring& string);
std::string vuapi upper_string_A(const std::string& string);
std::wstring vuapi upper_string_W(const std::wstring& string);
std::string vuapi to_string_A(const std::wstring& string, const bool utf8 = false); // ANSI or UTF-8
std::wstring vuapi to_string_W(const std::string& string, const bool utf8 = false); // ANSI or UTF-8
std::vector<std::string> vuapi split_string_A(
  const std::string& string, const std::string& separator, bool remove_empty = false);
std::vector<std::wstring> vuapi split_string_W(
  const std::wstring& string, const std::wstring& separator, bool remove_empty = false);
std::string vuapi join_string_A(const std::vector<std::string> parts, const std::string& separator = "");
std::wstring vuapi join_string_W(const std::vector<std::wstring> parts, const std::wstring& separator = L"");
std::vector<std::string> vuapi multi_string_to_list_A(const char* ps_multi_string);
std::vector<std::wstring> vuapi multi_string_to_list_W(const wchar* ps_multi_string);
std::unique_ptr<char[]> vuapi list_to_multi_string_A(const std::vector<std::string>& strings);
std::unique_ptr<wchar[]> vuapi list_to_multi_string_W(const std::vector<std::wstring>& strings);
std::string vuapi load_rs_string_A(const uint id, const std::string& module_name = "");
std::wstring vuapi load_rs_string_W(const uint id, const std::wstring& module_name = L"");
std::string vuapi trim_string_A(
  const std::string& string,
  const trim_type& type = trim_type::TS_BOTH,
  const std::string& chars = " \t\n\r\f\v");
std::wstring vuapi trim_string_W(
  const std::wstring& string,
  const trim_type& type = trim_type::TS_BOTH,
  const std::wstring& chars = L" \t\n\r\f\v");
std::string vuapi replace_string_A(
  const std::string& text, const std::string& from, const std::string& to);
std::wstring vuapi replace_string_W(
  const std::wstring& text, const std::wstring& from, const std::wstring& to);
bool vuapi starts_with_A(const std::string& text, const std::string& with, bool ignore_case = false);
bool vuapi starts_with_W(const std::wstring& text, const std::wstring& with, bool ignore_case = false);
bool vuapi ends_with_A(const std::string& text, const std::string& with, bool ignore_case = false);
bool vuapi ends_with_W(const std::wstring& text, const std::wstring& with, bool ignore_case = false);
bool vuapi contains_string_A(const std::string& text, const std::string& test, bool ignore_case = false);
bool vuapi contains_string_W(const std::wstring& text, const std::wstring& test, bool ignore_case = false);
bool vuapi compare_string_A(const std::string& vl, const std::string& vr, bool ignore_case = false);
bool vuapi compare_string_W(const std::wstring& vl, const std::wstring& vr, bool ignore_case = false);
std::string vuapi regex_replace_string_A(
  const std::string& text,
  const std::regex& pattern,
  const std::string& replacement,
  std::regex_constants::match_flag_type flags = std::regex_constants::match_default);
std::wstring vuapi regex_replace_string_W(
  const std::wstring& text,
  const std::wregex& pattern,
  const std::wstring& replacement,
  std::regex_constants::match_flag_type flags = std::regex_constants::match_default);
std::string vuapi find_closest_string_A(
  const std::string& string, const std::vector<std::string>& string_list);
std::wstring vuapi find_closest_string_W(
  const std::wstring& string, const std::vector<std::wstring>& string_list);

/**
 * Process Working
 */

enum arch
{
  x86 = 4,
  x64 = 8,
};

enum class wow_64
{
  WOW64_ERROR = -1,
  WOW64_NO    = 0,
  WOW64_YES   = 1,
};

#ifndef PROCESSOR_ARCHITECTURE_NEUTRAL
#define PROCESSOR_ARCHITECTURE_NEUTRAL 11
#endif

enum processor_architecture
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
};

struct MemoryBlock
{
  void*  address;
  SIZE_T size;
};

processor_architecture get_processor_architecture();
bool vuapi is_64bits(HANDLE hp = INVALID_HANDLE_VALUE);
bool vuapi is_64bits(ulong pid = INVALID_PID_VALUE);
wow_64 vuapi is_wow64(HANDLE hp = INVALID_HANDLE_VALUE);
wow_64 vuapi is_wow64(ulong pid = INVALID_PID_VALUE); /* -1: error, 0: false, 1: true */
ulong vuapi get_parent_pid(ulong child_pid);
ulong vuapi get_main_thread_id(ulong pid);
std::vector<ulong> vuapi name_to_pid_A(const std::string& name);
std::vector<ulong> vuapi name_to_pid_W(const std::wstring& name);
std::string vuapi pid_to_name_A(ulong pid);
std::wstring vuapi pid_to_name_W(ulong pid);
HMODULE vuapi remote_get_module_handle_A(ulong pid, const std::string& module_name); // TODO: uncompleted.
HMODULE vuapi remote_get_module_handle_W(ulong pid, const std::wstring& module_name);
VUResult vuapi inject_dll_A(ulong pid, const std::string& dll_file_path, bool wait_loading = true);
VUResult vuapi inject_dll_W(ulong pid, const std::wstring& dll_file_path, bool wait_loading = true);
bool vuapi read_memory(
  const HANDLE hp,
  const void* address,
  void* buffer,
  const SIZE_T size,
  const bool force = false);
bool vuapi read_memory_ex(
  const arch bit,
  const HANDLE hp,
  const void* address,
  void* buffer,
  const SIZE_T size,
  const bool force = false,
  const SIZE_T n_offsets = 0, ...);
bool vuapi write_memory(const HANDLE hp,
  const void* address,
  const void* buffer,
  const SIZE_T size,
  const bool force = false);
bool vuapi write_memory_ex(const arch bit,
  const HANDLE hp,
  const void* address,
  const void* buffer,
  const SIZE_T size,
const bool force = false, const SIZE_T n_offsets = 0, ...);

/**
 * Window Working
 */

struct FontA
{
  std::string name;
  int  size;
  bool italic;
  bool under_line;
  bool strike_out;
  int  weight;
  int  char_set;
  int  orientation;
  FontA()
    : name(""), size(-1)
    , italic(false), under_line(false), strike_out(false)
    , weight(0), char_set(ANSI_CHARSET), orientation(0) {}
};

struct FontW
{
  std::wstring name;
  int  size;
  bool italic;
  bool under_line;
  bool strike_out;
  int  weight;
  int  char_set;
  int  orientation;
  FontW()
    : name(L""), size(-1)
    , italic(false), under_line(false), strike_out(false)
    , weight(0), char_set(ANSI_CHARSET), orientation(0) {}
};

// #ifdef _UNICODE
// #define Font FontW
// #else
// #define Font FontA
// #endif

typedef std::vector<MONITORINFOEXA> Monitors_A;
typedef std::vector<MONITORINFOEXW> Monitors_W;
#ifdef _UNICODE
#define Monitors Monitors_W
#else
#define Monitors Monitors_A
#endif

HWND vuapi get_console_window();
HWND vuapi find_top_window(ulong pid);
HWND vuapi find_top_window(HWND hwnd);
HWND vuapi find_main_window(ulong pid);
HWND vuapi find_main_window(HWND hwnd);
FontA vuapi get_font_A(HWND hwnd);
FontW vuapi get_font_W(HWND hwnd);
bool vuapi is_window_full_screen(HWND hwnd);
LONG vuapi conv_font_height_to_size(LONG height, HWND hwnd = nullptr);
LONG vuapi conv_font_size_to_height(LONG size, HWND hwnd = nullptr);
bool vuapi get_monitors_A(Monitors_A& monitors);
bool vuapi get_monitors_W(Monitors_W& monitors);

/**
 * Decode Constants
 */

// Window Message
std::string  vuapi decode_const_window_message_A(const ulong id);
std::wstring vuapi decode_const_window_message_W(const ulong id);
ulong vuapi decode_const_window_message_A(const std::string& str);
ulong vuapi decode_const_window_message_W(const std::wstring& str);

// Process Page Protection
std::string vuapi decode_const_process_page_protection_A(const ulong id);
std::wstring vuapi decode_const_process_page_protection_W(const ulong id);
ulong vuapi decode_const_process_page_protection_A(const std::string& str);
ulong vuapi decode_const_process_page_protection_W(const std::wstring& str);

// Process Mamory State
std::string vuapi decode_const_process_memory_state_A(const ulong id);
std::wstring vuapi decode_const_process_memory_state_W(const ulong id);
ulong vuapi decode_const_process_memory_state_A(const std::string& str);
ulong vuapi decode_const_process_memory_state_W(const std::wstring& str);

// HTTP Status
std::string vuapi decode_const_http_status_A(const ulong code);
std::wstring vuapi decode_const_http_status_W(const ulong code);

#ifdef _UNICODE
#define decode_const_window_message decode_const_window_message_W
#define decode_const_process_page_protection decode_const_process_page_protection_W
#define decode_const_http_status decode_const_http_status_W
#else
#define decode_const_window_message decode_const_window_message_A
#define decode_const_process_page_protection decode_const_process_page_protection_A
#define decode_const_http_status decode_const_http_status_A
#endif

/**
 * File/Directory Working
 */

enum class path_separator
{
  WIN,
  POSIX,
};

enum class disk_type : int
{
  Unspecified = 0,
  HDD = 3,
  SSD = 4,
  SCM = 5,
};

#if defined(VU_WMI_ENABLED)
disk_type vuapi get_disk_type_A(const char drive);
disk_type vuapi get_disk_type_W(const wchar_t drive);
#endif // VU_WMI_ENABLED

#if defined(_MSC_VER) || defined(__BCPLUSPLUS__) // LNK

struct LNKA
{
  std::string path;
  std::string argument;
  std::string directory;
  std::string description;
  std::pair<std::string, int> icon;
  ushort hotkey;
  int window;

  LNKA()
    : path("")
    , argument("")
    , directory("")
    , description("")
    , hotkey(0x0000)
    , window(SW_NORMAL)
  {
    icon.first = "";
    icon.second = 0;
  }
};

struct LNKW
{
  std::wstring path;
  std::wstring argument;
  std::wstring directory;
  std::wstring description;
  std::pair<std::wstring, int> icon;
  ushort hotkey;
  int window;

  LNKW()
    : path(L"")
    , argument(L"")
    , directory(L"")
    , description(L"")
    , hotkey(0x0000)
    , window(SW_NORMAL)
  {
    icon.first = L"";
    icon.second = 0;
  }
};

#ifdef _UNICODE
#define LNK LNKW
#else
#define LNK LNKA
#endif

std::unique_ptr<LNKA> parse_shortcut_lnk_A(HWND hwnd, const std::string& lnk_file_path);
std::unique_ptr<LNKW> parse_shortcut_lnk_W(HWND hwnd, const std::wstring& lnk_file_path);

vu::VUResult create_shortcut_lnk_A(const std::string& lnk_file_path, const LNKA& lnk);
vu::VUResult create_shortcut_lnk_W(const std::wstring& lnk_file_path, const LNKW& lnk);

#ifdef _UNICODE
#define parse_shortcut_lnk parse_shortcut_lnk_W
#define create_shortcut_lnk create_shortcut_lnk_W
#else
#define parse_shortcut_lnk parse_shortcut_lnk_A
#define create_shortcut_lnk create_shortcut_lnk_A
#endif

#endif // LNK

bool vuapi is_directory_exists_A(const std::string& directory);
bool vuapi is_directory_exists_W(const std::wstring& directory);
bool vuapi is_file_exists_A(const std::string& file_path);
bool vuapi is_file_exists_W(const std::wstring& file_path);
std::unique_ptr<std::vector<vu::byte>> vuapi read_file_binary_A(const std::string& file_path);
std::unique_ptr<std::vector<vu::byte>> vuapi read_file_binary_W(const std::wstring& file_path);
bool vuapi read_file_binary_A(const std::string& file_path, std::vector<byte>& data);
bool vuapi read_file_binary_W(const std::wstring& file_path, std::vector<byte>& data);
bool vuapi write_file_binary_A(const std::string& file_path, const std::vector<byte>& data);
bool vuapi write_file_binary_W(const std::wstring& file_path, const std::vector<byte>& data);
bool vuapi write_file_binary_A(const std::string& file_path, const byte* data, const size_t size);
bool vuapi write_file_binary_W(const std::wstring& file_path, const byte* data, const size_t size);
bool vuapi is_file_name_valid_A(const std::string& file_name);
bool vuapi is_file_name_valid_W(const std::wstring& file_name);
std::string vuapi correct_file_name_A(const std::string& file_name, const char replacement_char = '-');
std::wstring vuapi correct_file_name_W(const std::wstring& file_name, const wchar_t replacement_char = L'-');
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
std::string vuapi join_path_A(
  const std::string& left, const std::string& right, const path_separator separator = path_separator::WIN);
std::wstring vuapi join_path_W(
  const std::wstring& left, const std::wstring& right, const path_separator separator = path_separator::WIN);
std::string vuapi normalize_path_A(
  const std::string& path, const path_separator separator = path_separator::WIN);
std::wstring vuapi normalize_path_W(
  const std::wstring& path, const path_separator separator = path_separator::WIN);
std::string vuapi undecorate_cpp_symbol_A(
  const std::string& name, const ushort flags = 0);   // UNDNAME_COMPLETE
std::wstring vuapi undecorate_cpp_symbol_W(
  const std::wstring& name, const ushort flags = 0); // UNDNAME_COMPLETE
arch vuapi get_pe_file_bits_A(const std::string& file_path);
arch vuapi get_pe_file_bits_W(const std::wstring& file_path);
bool check_path_permissions_A(const std::string& path, ulong generic_access_rights);
bool check_path_permissions_W(const std::wstring& path, ulong generic_access_rights);
bool copy_to_clipboard_A(const std::string& text);
bool copy_to_clipboard_W(const std::wstring& text);

/**
 * Wrapper
 */

// Base64

bool vuapi crypt_b64encode_A(const std::vector<byte>& data, std::string& text);
bool vuapi crypt_b64encode_W(const std::vector<byte>& data, std::wstring& text);
bool vuapi crypt_b64decode_A(const std::string& text, std::vector<byte>& data);
bool vuapi crypt_b64decode_W(const std::wstring& text, std::vector<byte>& data);

// MD5

std::string  vuapi crypt_md5_buffer_A(const std::vector<byte>& data);
std::wstring vuapi crypt_md5_buffer_W(const std::vector<byte>& data);
std::string  vuapi crypt_md5_text_A(const std::string& text);
std::wstring vuapi crypt_md5_text_W(const std::wstring& text);
std::string  vuapi crypt_md5_file_A(const std::string& file_path);
std::wstring vuapi crypt_md5_file_W(const std::wstring& file_path);

// CRC
//  @refer to https://reveng.sourceforge.io/crc-catalogue/all.htm
//  @refer to https://www.sunshine2k.de/coding/javascript/crc/crc_js.html
//  @refer to https://toolslick.com/programming/hashing/crc-calculator
//  @refer to https://crccalc.com/

enum class crypt_bits : int
{
  _8   = 8,   // CRC-8/SMBUS
  _16  = 16,  // CRC-16/ARC, CRC-16/LHA, CRC-IBM
  _32  = 32,  // CRC-32/HDLC, CRC-32/ADCCP, CRC-32/V-42, CRC-32/XZ, PKZIP
  _64  = 64,  // CRC-64/ECMA-182

  _160 = 160, // SHA-1   (20 bytes)
  _224 = 224, // SHA-1   (28 bytes)
  _256 = 256, // SHA-256 (32 bytes)
  _384 = 384, // SHA-384 (48 bytes)
  _512 = 512, // SHA-512 (64 bytes)

  Unspecified = -1,
};

uint64 vuapi crypt_crc_text_A(const std::string& text, const crypt_bits bits);
uint64 vuapi crypt_crc_text_W(const std::wstring& text, const crypt_bits bits);
uint64 vuapi crypt_crc_file_A(const std::string& file_path, const crypt_bits bits);
uint64 vuapi crypt_crc_file_W(const std::wstring& file_path, const crypt_bits bits);
uint64 vuapi crypt_crc_buffer(const std::vector<byte>& data, const crypt_bits bits);

// Note: For reduce library size so only enabled 32/64-bits of parametrized CRC algorithms
uint64 vuapi crypt_crc_buffer(const std::vector<byte>& data,
  uint8_t bits, uint64 poly, uint64 init, bool ref_in, bool ref_out, uint64 xor_out, uint64 check);

// SHA

enum class sha_version
{
  _1 = 1,
  _2 = 2,
  _3 = 3,
};

std::string vuapi crypt_sha_text_A(
  const std::string& text, const sha_version version, const crypt_bits bits);
std::wstring vuapi crypt_sha_text_W(
  const std::wstring& text, const sha_version version, const crypt_bits bits);
std::string vuapi crypt_sha_file_A(
  const std::string& file_path, const sha_version version, const crypt_bits bits);
std::wstring vuapi crypt_sha_file_W(
  const std::wstring& file_path, const sha_version version, const crypt_bits bits);
void vuapi crypt_sha_buffer(
  const std::vector<byte>& data,
  const sha_version version,
  const crypt_bits bits,
  std::vector<byte>& hash);

/*----------- The definition of common function(s) which compatible both ANSI & UNICODE ----------*/

#ifdef _UNICODE
/* Misc Working */
#define set_privilege set_privilege_W
#define get_env get_env_W
#define set_env set_env_W
#define find_pattern find_pattern_W
/* String Formatting */
#define format format_W
#define msg_debug msg_debug_W
#define msg_box msg_box_W
#define get_last_error get_last_error_W
#define date_time_to_string date_time_to_string_W
#define format_date_time format_date_time_W
#define format_bytes format_bytes_W
#define to_hex_string to_hex_string_W
#define to_hex_bytes to_hex_bytes_W
#define url_encode url_encode_W
#define url_decode url_decode_W
/* String Working */
#define lower_string lower_string_W
#define upper_string upper_string_W
#define split_string split_string_W
#define join_string join_string_W
#define multi_string_to_list multi_string_to_list_W
#define list_to_multi_string list_to_multi_string_W
#define load_rs_string load_rs_string_W
#define trim_string trim_string_W
#define replace_string replace_string_W
#define starts_with starts_with_W
#define ends_with ends_with_W
#define contains_string contains_string_W
#define compare_string compare_string_W
#define regex_replace_string regex_replace_string_W
#define find_closest_string find_closest_string_W
/* Window Working */
#define get_font get_font_W
#define get_monitors get_monitors_W
/* Process Working */
#define name_to_pid name_to_pid_W
#define pid_to_name pid_to_name_W
#define remote_get_module_handle remote_get_module_handle_W
#define inject_dll inject_dll_W
/* File/Directory Working */
#define is_directory_exists is_directory_exists_W
#define is_file_exists is_file_exists_W
#define get_file_type get_file_type_W
#define is_file_name_valid is_file_name_valid_W
#define correct_file_name correct_file_name_W
#define read_file_binary read_file_binary_W
#define write_file_binary write_file_binary_W
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
#define get_pe_file_bits get_pe_file_bits_W
#define check_path_permissions check_path_permissions_W
#define copy_to_clipboard copy_to_clipboard_W
/* Network Working */
#define decode_http_status decode_http_status_W
/* Cryptography */
#define crypt_b64encode crypt_b64encode_W
#define crypt_b64decode crypt_b64decode_W
#define crypt_md5_buffer crypt_md5_buffer_W
#define crypt_md5_text crypt_md5_text_W
#define crypt_md5_file crypt_md5_file_W
#define crypt_crc_text crypt_crc_text_W
#define crypt_crc_file crypt_crc_file_W
#define crypt_sha_text crypt_sha_text_W
#define crypt_sha_file crypt_sha_file_W
#else // _UNICODE
/* Misc Working */
#define set_privilege set_privilege_A
#define get_env get_env_A
#define set_env set_env_A
#define find_pattern find_pattern_A
/* String Formatting */
#define format format_A
#define msg_debug msg_debug_A
#define msg_box msg_box_A
#define get_last_error get_last_error_A
#define date_time_to_string date_time_to_string_A
#define format_date_time format_date_time_A
#define format_bytes format_bytes_A
#define to_hex_string to_hex_string_A
#define to_hex_bytes to_hex_bytes_A
#define url_encode url_encode_A
#define url_decode url_decode_A
/* String Working */
#define lower_string lower_string_A
#define upper_string upper_string_A
#define split_string split_string_A
#define join_string join_string_A
#define multi_string_to_list multi_string_to_list_A
#define load_rs_string load_rs_string_A
#define trim_string trim_string_A
#define replace_string replace_string_A
#define starts_with starts_with_A
#define ends_with ends_with_A
#define contains_string contains_string_A
#define compare_string compare_string_A
#define regex_replace_string regex_replace_string_A
#define find_closest_string find_closest_string_A
/* Window Working */
#define get_font get_font_A
#define get_monitors get_monitors_A
/* Process Working */
#define name_to_pid name_to_pid_A
#define pid_to_name pid_to_name_A
#define remote_get_module_handle remote_get_module_handle_A
#define inject_dll inject_dll_A
/* File/Directory Working */
#define is_directory_exists is_directory_exists_A
#define is_file_exists is_file_exists_A
#define get_file_type get_file_type_A
#define is_file_name_valid is_file_name_valid_A
#define correct_file_name correct_file_name_A
#define read_file_binary read_file_binary_A
#define write_file_binary write_file_binary_A
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
#define get_pe_file_bits get_pe_file_bits_A
#define check_path_permissions check_path_permissions_A
#define copy_to_clipboard copy_to_clipboard_A
/* Network Working */
#define decode_http_status decode_http_status_A
/* Cryptography */
#define crypt_b64encode crypt_b64encode_A
#define crypt_b64decode crypt_b64decode_A
#define crypt_md5_buffer crypt_md5_buffer_A
#define crypt_md5_text crypt_md5_text_A
#define crypt_md5_file crypt_md5_file_A
#define crypt_crc_text crypt_crc_text_A
#define crypt_crc_file crypt_crc_file_A
#define crypt_sha_text crypt_sha_text_A
#define crypt_sha_file crypt_sha_file_A
#endif

/* Debug Assertion */

#ifdef _DEBUG
void _required(const bool s, const std::string& m = "");
std::string _required_info(const char* m, const char* f, unsigned int n, const char* s);
#define vu_required_with_message(s, m) vu::_required((s), vu::_required_info(m, __FILE__, __LINE__, VU_FUNC_INFO))
#else  // _DEBUG
#define vu_required_with_message(s, m) static_cast<void>(0)
#endif // _DEBUG

#define vu_required(c) vu_required_with_message((c), "<none>")

/* -------------------------------------- Public Class(es) -------------------------------------- */

/**
 * Windows Last Error
 */

class LastError
{
public:
  LastError() : m_last_error_code(ERROR_SUCCESS) {};
  virtual ~LastError() {};

  LastError& operator=(const LastError& right)
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
    return get_last_error_A(this->get_last_error_code());
  }

  virtual std::wstring vuapi get_last_error_message_W()
  {
    return get_last_error_W(this->get_last_error_code());
  }

protected:
  ulong m_last_error_code;
};

#ifdef _UNICODE
#define get_last_error_message get_last_error_message_W
#else
#define get_last_error_message get_last_error_message_A
#endif

/**
 * Singleton Pattern
 */

#include "template/singleton.tpl"

/**
 * Globally Unique Identifier
 */

#ifdef VU_GUID_ENABLED

struct Guid
{
  unsigned long  data1;
  unsigned short data2;
  unsigned short data3;
  unsigned char  data4[8];

  const Guid& operator = (const Guid &right) const;
  bool operator == (const Guid &right) const;
  bool operator != (const Guid &right) const;
};

class GUIDX
{
public:
  GUIDX(bool create = false);
  virtual ~GUIDX();

  const GUIDX& operator = (const GUIDX& right);
  bool operator == (const GUIDX& right) const;
  bool operator != (const GUIDX& right) const;

  bool create();

  const Guid& GUID() const;
  void  GUID(const Guid& guid);

protected:
  Guid m_guid;
  VUResult m_status;
};

class GUIDA : public GUIDX
{
public:
  GUIDA(bool create = false) : GUIDX(create) {}
  virtual ~GUIDA() {}

  void parse(const std::string& guid);
  std::string as_string() const;

  static const std::string to_string(const Guid& guid);
  static const Guid to_guid(const std::string& guid);
};

class GUIDW : public GUIDX
{
public:
  GUIDW(bool create = false) : GUIDX(create) {}
  virtual ~GUIDW() {}

  void parse(const std::wstring& guid);
  std::wstring as_string() const;

  static const std::wstring to_string(const Guid& guid);
  static const Guid to_guid(const std::wstring& guid);
};

#endif // VU_GUID_ENABLED

/**
 * Buffer
 */

class Buffer
{
public:
  Buffer();
  Buffer(const void* ptr, const size_t size);
  Buffer(const size_t size);
  Buffer(const Buffer& right);
  virtual ~Buffer();

  const Buffer& operator=(const Buffer& right);
  bool  operator==(const Buffer& right) const;
  bool  operator!=(const Buffer& right) const;
  byte& operator[](const size_t offset);
  std::unique_ptr<Buffer> operator()(int begin, int end) const;

  byte*  bytes() const;
  void*  pointer() const;
  size_t size() const;

  bool empty() const;

  void reset();
  void fill(const byte v = 0);
  bool resize(const size_t size);
  bool replace(const void* ptr, const size_t size);
  bool replace(const Buffer& right);
  bool match(const void* ptr, const size_t size) const;
  size_t find(const void* ptr, const size_t size) const;
  std::unique_ptr<Buffer> till(const void* ptr, const size_t size) const;
  std::unique_ptr<Buffer> slice(int begin, int end) const;

  bool append(const void* ptr, const size_t size);
  bool append(const Buffer& right);

  std::unique_ptr<std::string>  to_string_A() const;
  std::unique_ptr<std::wstring> to_string_W() const;

  bool save_to_file(const std::string&  file_path);
  bool save_to_file(const std::wstring& file_path);

private:
  bool create(void* ptr, const size_t size, const bool clean = true);
  bool destroy();

private:
  void*  m_ptr;
  size_t m_size;
};

/**
 * Variant
 */

template <class T>
class VariantT
{
public:
  VariantT();
  VariantT(const VariantT& right);
  virtual ~VariantT();

  VariantT& operator=(const VariantT& right);

  T& data()
  {
    return *m_data;
  }

  template<typename V>
  friend VariantT& operator<<(VariantT& stream, V v)
  {
    stream.data() << v;
    return stream;
  }

  bool empty() const;

  int to_int() const;
  unsigned int to_uint() const;
  __int64 to_int64() const;
  unsigned __int64 to_uint64() const;
  bool to_bool() const;
  float to_float() const;
  double to_double() const;
  std::unique_ptr<byte[]> to_bytes() const;

protected:
  std::unique_ptr<T> m_data;
};

#define VariantTA VariantT<std::stringstream>

class VariantA : public VariantTA
{
public:
  VariantA();
  VariantA(const VariantA& right);
  virtual ~VariantA();

  std::string to_string() const;
};

#define VariantTW VariantT<std::wstringstream>

class VariantW : public VariantTW
{
public:
  VariantW();
  VariantW(const VariantW& right);
  virtual ~VariantW();

  std::wstring to_string() const;
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
#define VU_GET_API(M, F) pfn ## F = (Pfn ## F)vu::Library::quick_get_proc_address(_T( # M ), _T( # F ))

class LibraryA : public LastError
{
public:
  LibraryA(const std::string& module_name);
  virtual ~LibraryA();

  const HMODULE& vuapi handle() const;
  bool  vuapi available();
  void* vuapi get_proc_address(const std::string& function_name);
  static void* vuapi quick_get_proc_address(
    const std::string& module_name, const std::string& function_name);

private:
  HMODULE m_module_handle;
};

class LibraryW : public LastError
{
public:
  LibraryW(const std::wstring& module_name);
  virtual ~LibraryW();

  const HMODULE& vuapi handle() const;
  bool  vuapi available();
  void* vuapi get_proc_address(const std::wstring& function_name);
  static void* vuapi quick_get_proc_address(
    const std::wstring& module_name, const std::wstring& function_name);

private:
  HMODULE m_module_handle;
};

/**
 * Socket
 */

#ifdef VU_INET_ENABLED

#ifndef MSG_NONE
#define MSG_NONE 0
#endif // MSG_NONE

#define VU_DEFAULT_SEND_RECV_TIMEOUT 3 // 3 seconds
#define VU_DEFAULT_SEND_RECV_BLOCK_SIZE KiB // 1 KiB

class Socket : public LastError
{
public:
  typedef int address_family_t;
  typedef int protocol_t;
  typedef int type_t;
  typedef int flags_t;
  typedef int shutdowns_t;

  struct Handle
  {
    SOCKET s;
    sockaddr_in sai;
    char ip[15];
  };

  struct Endpoint
  {
    std::string host;
    ushort port;

    Endpoint(const std::string& host, const ushort port) : host(host), port(port) {}
  };

  struct Options
  {
    struct
    {
      int recv;
    } timeout; // in seconds

    Options()
    {
      this->timeout.recv = VU_DEFAULT_SEND_RECV_TIMEOUT;
    }
  };

public:
  Socket(
    const address_family_t af = AF_INET,
    const type_t type = SOCK_STREAM,
    const protocol_t proto = IPPROTO_IP,
    const bool wsa = true,
    const Options* options = nullptr
  );
  virtual ~Socket();

  SOCKET& vuapi handle();
  const WSADATA& vuapi wsa_data() const;
  const address_family_t vuapi af() const;
  const type_t vuapi type() const;
  const protocol_t vuapi  protocol() const;

  bool vuapi available();

  void vuapi attach(const SOCKET&  socket);
  void vuapi attach(const Handle& socket);
  void vuapi detach();

  VUResult vuapi bind(const Endpoint& endpoint);
  VUResult vuapi bind(const std::string& address, const ushort port);

  VUResult vuapi listen(const int maxcon = SOMAXCONN);
  VUResult vuapi accept(Handle& socket);

  VUResult vuapi connect(const Endpoint& endpoint);
  VUResult vuapi connect(const std::string& address, const ushort port);

  VUResult vuapi disconnect(const shutdowns_t flags = SD_BOTH);

  IResult vuapi send(const char* ptr_data, int size, const flags_t flags = MSG_NONE);
  IResult vuapi send(const Buffer& data, const flags_t flags = MSG_NONE);

  IResult vuapi recv(char* ptr_data, int size, const flags_t flags = MSG_NONE);
  IResult vuapi recv(Buffer& data, const flags_t flags = MSG_NONE);
  IResult vuapi recv_all(Buffer& data, const flags_t flags = MSG_NONE);

  IResult vuapi send_to(const char* ptr_data, const int size, const Handle& socket);
  IResult vuapi send_to(const Buffer& data, const Handle& socket);

  IResult vuapi recv_from(char* ptr_data, int size, const Handle& socket);
  IResult vuapi recv_from(Buffer& data, const Handle& socket);
  IResult vuapi recv_all_from(Buffer& data, const Handle& socket);

  IResult vuapi close();

  const sockaddr_in vuapi get_local_sai();
  const sockaddr_in vuapi get_remote_sai();
  std::string vuapi get_host_name();

  Options& options();

  VUResult vuapi set_option(const int level, const int option, const void* value, const int size);
  VUResult vuapi enable_non_blocking(bool state = true);

private:
  bool vuapi valid(const SOCKET& socket);
  bool vuapi parse(const Handle& socket);
  bool vuapi is_host_name(const std::string& s);
  std::string vuapi get_host_address(const std::string& name);

private:
  bool m_wsa;
  type_t m_type;
  WSADATA m_wsa_data;
  address_family_t m_af;
  protocol_t m_proto;
  sockaddr_in m_sai;
  SOCKET m_socket;
  Options m_options;
  bool m_self;
};

class AsyncSocket : public LastError
{
public:
  typedef std::function<void(Socket& client)> fn_prototype_t;

  enum function : uint
  {
    CONNECT,
    OPEN,
    CLOSE,
    RECV,
    SEND,
    UNDEFINED,
  };

  enum class side_type : uint
  {
    SERVER,
    CLIENT,
  };

  AsyncSocket(
    const vu::Socket::address_family_t af = AF_INET,
    const vu::Socket::type_t type = SOCK_STREAM,
    const vu::Socket::protocol_t proto = IPPROTO_IP,
    const vu::Socket::Options* options = nullptr
  );
  virtual ~AsyncSocket();

  side_type vuapi side();

  bool vuapi available();
  bool vuapi running();

  /**
   * https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaeventselect?redirectedfrom=MSDN#return-value
   * Note: After connected (FD_CONNECT), client will be auto generated first event FD_WRITE.
   */
  VUResult vuapi connect(const Socket::Endpoint& endpoint);
  VUResult vuapi connect(const std::string& address, const ushort port);

  VUResult vuapi bind(const Socket::Endpoint& endpoint);
  VUResult vuapi bind(const std::string& address, const ushort port);

  /**
   * https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaeventselect?redirectedfrom=MSDN#return-value
   * Note: After accepted (FD_ACCEPT), server will be auto generated first event FD_WRITE.
   */
  VUResult vuapi listen(const int maxcon = SOMAXCONN);

  VUResult vuapi run();
  VUResult vuapi run_in_thread();

  VUResult vuapi stop();
  IResult  vuapi close();

  std::set<SOCKET> vuapi get_connections();
  VUResult vuapi disconnect_connections(const Socket::shutdowns_t flags = SD_BOTH);

  IResult vuapi send(const SOCKET& connection, const char* ptr_data, int size, const Socket::flags_t flags = MSG_NONE);
  IResult vuapi send(const SOCKET& connection, const Buffer& data, const Socket::flags_t flags = MSG_NONE);

  virtual void on(const function type, const fn_prototype_t fn); // must be mapping before call run(...)

  virtual void on_connect(Socket& connection);
  virtual void on_open(Socket&  connection);
  virtual void on_close(Socket& connection);
  virtual void on_send(Socket&  connection);
  virtual void on_recv(Socket&  connection);

protected:
  void vuapi initialze();
  VUResult vuapi loop();

  IResult vuapi do_connect(WSANETWORKEVENTS& events, SOCKET& connection);
  IResult vuapi do_open(WSANETWORKEVENTS&  events, SOCKET& connection);
  IResult vuapi do_recv(WSANETWORKEVENTS&  events, SOCKET& connection);
  IResult vuapi do_send(WSANETWORKEVENTS&  events, SOCKET& connection);
  IResult vuapi do_close(WSANETWORKEVENTS& events, SOCKET& connection);

protected:
  vu::Socket m_socket;
  side_type m_side;
  bool m_running;
  DWORD m_n_events;
  SOCKET m_connections[WSA_MAXIMUM_WAIT_EVENTS];
  WSAEVENT m_events[WSA_MAXIMUM_WAIT_EVENTS];
  fn_prototype_t m_functions[function::UNDEFINED];
  std::mutex m_mutex;
  HANDLE m_thread;
};

#endif // VU_INET_ENABLED

/**
 * API Hooking - Inline
 */

 /**
 * @brief Hook/Unhook a function in a module by name.
 * @define The prefix of redirection function must be  : Hfn
 * @define The prefix of real function pointer must be : pfn
 * @param[in] O The INLHooking instance.
 * @param[in] M The module name.
 * @param[in] F The function name.
 * @return  true if the function succeeds. Otherwise false.
 */
#define VU_API_INL_OVERRIDE(O, M, F) O.install(_T( # M ), _T( # F ), (void*)&Hfn ## F, (void**)&pfn ## F)
#define VU_API_INL_RESTORE(O, M, F) O.uninstall(_T( # M ), _T( # F ), (void**)&pfn ## F)

enum class memory_address_type
{
  MAT_NONE = 0,
  MAT_8    = 1,
  MAT_16   = 2,
  MAT_32   = 3,
  MAT_64   = 4,
};

struct MemoryInstruction
{
  ulong offset;   // Offset of the current instruction.
  ulong position; // Position of the memory address in the current instruction.
  memory_address_type mat;
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
};

class INLHookingX
{
protected:
  struct RedirectInstruction
  {
    ushort   jmp;
    ulong    unknown;
    ulongptr address;
  };

  bool m_hooked;
  std::vector<MemoryInstruction> m_memory_instructions;

  static const ulong JMP_OPCODE_SIZE = 6;

  #ifdef _M_IX86
  static const ulong MIN_HOOK_SIZE   = 10;
  #else  // _M_AMD64
  static const ulong MIN_HOOK_SIZE   = 14;
  #endif // _M_IX86

public:
  INLHookingX() : m_hooked(false) {};
  virtual ~INLHookingX() {};

  bool vuapi attach(void* ptr_function, void* ptr_hook_function, void** pptr_old_function);
  bool vuapi detach(void* ptr_function, void** pptr_old_function);
};

class INLHookingA: public INLHookingX
{
public:
  INLHookingA() {};
  virtual ~INLHookingA() {};

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

class INLHookingW: public INLHookingX
{
public:
  INLHookingW() {};
  virtual ~INLHookingW() {};

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
  vu::IATHooking::instance().install(\
    _T( # O ), _T( # M ), _T( # F ),\
    (const void*)(reinterpret_cast<void*>(&Hfn ## F)),\
    (const void**)(reinterpret_cast<void**>(&pfn ## F)))

#define VU_API_IAT_RESTORE(O, M, F)\
  vu::IATHooking::instance().uninstall(\
    _T( # O ), _T( # M ), _T( # F ))

struct IATElement;

class IATHookingA : public SingletonT<IATHookingA>
{
public:
  IATHookingA();
  virtual ~IATHookingA();

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
   * @param[out,in] ptr_iat The Original First Thunk that point to INT <Import Name Table>.
   * @param[out,in] ptr_int The First Thunk that point to IAT <Import Address Table>.
   * @return true to continue or false to exit iteration.
   */
  VUResult iterate(const std::string& module, std::function<bool(
    const std::string& module,
    const std::string& function,
    PIMAGE_THUNK_DATA& ptr_iat,
    PIMAGE_THUNK_DATA& ptr_int)> fn);

private:
  enum iat_action
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

  VUResult perform(const iat_action action, IATElement& element);
};

class IATHookingW : public SingletonT<IATHookingW>
{
public:
  IATHookingW();
  virtual ~IATHookingW();

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

class WMHookingX : public LastError
{
public:
  WMHookingX();
  virtual ~WMHookingX();

  VUResult vuapi uninstall(int Type);

public:
  static HHOOK m_handles[WH_MAXHOOK];

protected:
  virtual VUResult set_windows_hook_ex_X(int type, HMODULE hmodule, HOOKPROC pfn_hook_function);

protected:
  ulong m_pid;
};

class WMHookingA : public WMHookingX
{
public:
  WMHookingA(ulong pid, const std::string& dll_file_path);
  virtual ~WMHookingA();

  VUResult vuapi install(int type, const std::string& function_name);

private:
  LibraryA m_library;
};

class WMHookingW : public WMHookingX
{
public:
  WMHookingW(ulong pid, const std::wstring& dll_file_path);
  virtual ~WMHookingW();

  VUResult vuapi install(int type, const std::wstring& function_name);

private:
  LibraryW m_library;
};

/**
 * File System
 */

enum fs_mode
{
  FM_CREATENEW = 1,    // CREATE_NEW         = 1,
  FM_CREATEALWAY,      // CREATE_ALWAYS      = 2,
  FM_OPENEXISTING,     // OPEN_EXISTING      = 3,
  FM_OPENALWAYS,       // OPEN_ALWAYS        = 4,
  FM_TRUNCATEEXISTING, // TRUNCATE_EXISTING  = 5,
};

enum fs_attribute
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
  FANOTCONTENTINDEXED = 0x00002000,   // FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  = $00002000;
  FAENCRYPTED         = 0x00004000,   // FILE_ATTRIBUTE_ENCRYPTED            = $00004000;
};

enum fs_share
{
  FS_NONE      = 0x00000000,
  FS_READ      = 0x00000001,
  FS_WRITE     = 0x00000002,
  FS_DELETE    = 0x00000004,
  FS_READWRITE = fs_share::FS_READ | FS_WRITE,
  FS_ALLACCESS = fs_share::FS_READ | FS_WRITE | FS_DELETE,
};

enum fs_generic
{
  FG_ALL = GENERIC_ALL,
  FG_EXECUTE = GENERIC_EXECUTE,
  FG_WRITE = GENERIC_WRITE,
  FG_READ = GENERIC_READ,
  FG_READWRITE = GENERIC_READ | GENERIC_WRITE,
};

enum fs_position_at
{
  PA_BEGIN = FILE_BEGIN,
  PA_CURRENT = FILE_CURRENT,
  PA_END = FILE_END,
};

struct FSObjectA
{
  std::string directory;
  std::string name;
  int64 size;
  ulong attributes;
};

struct FSObjectW
{
  std::wstring directory;
  std::wstring name;
  int64 size;
  ulong attributes;
};

#ifdef _UNICODE
#define FSObject FSObjectW
#else
#define FSObject FSObjectA
#endif

class FileSystemX : public LastError
{
public:
  FileSystemX();
  virtual ~FileSystemX();

  virtual bool vuapi ready();
  virtual bool vuapi valid(HANDLE handle);
  virtual ulong vuapi get_file_size();

  virtual std::unique_ptr<Buffer> vuapi read_as_buffer();
  virtual bool vuapi read(void* ptr_buffer, ulong size);
  virtual bool vuapi read(
    ulong offset, void* ptr_buffer, ulong size, fs_position_at flags = fs_position_at::PA_BEGIN);

  virtual bool vuapi write(const void* ptr_buffer, ulong size);
  virtual bool vuapi write(
    ulong offset, const void* ptr_buffer, ulong size, fs_position_at flags = fs_position_at::PA_BEGIN);

  virtual bool vuapi seek(ulong offset, fs_position_at flags);
  virtual bool vuapi io_control(
    ulong code, void* ptr_send_buffer, ulong send_size, void* ptr_recv_buffer, ulong recv_size);

  virtual bool vuapi close();

protected:
  HANDLE m_handle;

private:
  ulong m_read_size, m_wrote_size;
};

class FileSystemA : public FileSystemX
{
public:
  FileSystemA();
  FileSystemA(
    const std::string& file_path,
    fs_mode fm_flags,
    fs_generic fg_flags = fs_generic::FG_READWRITE,
    fs_share fs_flags = fs_share::FS_ALLACCESS, fs_attribute fa_flags = fs_attribute::FA_NORMAL);
  virtual ~FileSystemA();

  bool vuapi initialize(const std::string& file_path,
    fs_mode fm_flags,
    fs_generic fg_flags = fs_generic::FG_READWRITE,
    fs_share fs_flags = fs_share::FS_ALLACCESS,
    fs_attribute fa_flags = fs_attribute::FA_NORMAL);

  std::unique_ptr<std::string> vuapi read_as_text();

  static std::unique_ptr<std::string> vuapi quick_read_as_text(const std::string& file_path);

  static std::unique_ptr<Buffer> quick_read_as_buffer(const std::string& file_path);

  static bool iterate(
    const std::string& path,
    const std::string& pattern,
    const std::function<bool(const FSObjectA& fso)> fn_callback);
};

class FileSystemW : public FileSystemX
{
public:
  FileSystemW();
  FileSystemW(
    const std::wstring& file_path,
    fs_mode fm_flags,
    fs_generic fg_flags = fs_generic::FG_READWRITE,
    fs_share fs_flags = fs_share::FS_ALLACCESS,
    fs_attribute fa_flags = fs_attribute::FA_NORMAL);
  virtual ~FileSystemW();

  bool vuapi initialize(
    const std::wstring& file_path,
    fs_mode fm_flags,
    fs_generic fg_flags = fs_generic::FG_READWRITE,
    fs_share fs_flags = fs_share::FS_ALLACCESS,
    fs_attribute fa_flags = fs_attribute::FA_NORMAL);

  std::unique_ptr<std::wstring> vuapi read_as_text();

  static std::unique_ptr<std::wstring>vuapi quick_read_as_text(const std::wstring& file_path);

  static std::unique_ptr<Buffer> vuapi quick_read_as_buffer(const std::wstring& file_path);

  static bool vuapi iterate(
    const std::wstring& path,
    const std::wstring& pattern,
    const std::function<bool(const FSObjectW& fso)> fn_callback);
};

/**
 * Service Working
 */

#define VU_SERVICE_ALL_TYPES  -1
#define VU_SERVICE_ALL_STATES -1

 /**
  * ServiceManagerT
  */

template <typename service_t>
class ServiceManagerT : public LastError
{
public:
  typedef std::vector<service_t> TServices;

  ServiceManagerT();
  virtual ~ServiceManagerT();

  virtual void refresh();
  virtual TServices get_services(
    ulong types = VU_SERVICE_ALL_TYPES, ulong states = VU_SERVICE_ALL_STATES);

protected:
  virtual VUResult initialize() = 0;

protected:
  bool m_initialized;
  SC_HANDLE m_hmanager;
  TServices m_services;
};

/**
 * ServiceManagerA
 */

typedef ServiceManagerT<ENUM_SERVICE_STATUS_PROCESSA> ServiceManagerTA;

class ServiceManagerA : public ServiceManagerTA, public SingletonT<ServiceManagerA>
{
public:
  ServiceManagerA();
  virtual ~ServiceManagerA();

  TServices find(const std::string& str, bool exact = false, bool name_only = false);
  std::unique_ptr<TServices::value_type> query(const std::string& service_name);
  int get_state(const std::string& service_name); // https://docs.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-controlservice#remarks

  TServices get_dependents(
    const std::string& service_name, const ulong states = VU_SERVICE_ALL_STATES);
  TServices get_dependencies(
    const std::string& service_name, const ulong states = VU_SERVICE_ALL_STATES);

  std::unique_ptr<SERVICE_STATUS> control(
    const TServices::value_type* ptr_service, const ulong ctrl_code);
  std::unique_ptr<SERVICE_STATUS> control(
    const std::string& name, const ulong ctrl_code);

  VUResult install(
    const std::string& file_path,
    const std::string& service_name,
    const std::string& display_name,
    const ulong service_type = SERVICE_KERNEL_DRIVER,
    const ulong access_type = SERVICE_ALL_ACCESS,
    const ulong start_type = SERVICE_DEMAND_START,
    const ulong ctrl_type = SERVICE_ERROR_NORMAL
  );

  VUResult uninstall(const std::string& name);
  VUResult start(const std::string& name);
  VUResult stop(const std::string& name);

protected:
  virtual VUResult initialize();
};

/**
 * ServiceManagerW
 */

typedef ServiceManagerT<ENUM_SERVICE_STATUS_PROCESSW> ServiceManagerTW;

class ServiceManagerW : public ServiceManagerTW, public SingletonT<ServiceManagerW>
{
public:
  ServiceManagerW();
  virtual ~ServiceManagerW();

  TServices find(const std::wstring& str, bool exact = false, bool name_only = false);
  std::unique_ptr<TServices::value_type> query(const std::wstring& service_name);
  int get_state(const std::wstring& service_name); // https://docs.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-controlservice#remarks

  TServices get_dependents(
    const std::wstring& service_name, const ulong states = VU_SERVICE_ALL_STATES);
  TServices get_dependencies(
    const std::wstring& service_name, const ulong states = VU_SERVICE_ALL_STATES);

  std::unique_ptr<SERVICE_STATUS> control(
    const TServices::value_type* ptr_service, const ulong ctrl_code);
  std::unique_ptr<SERVICE_STATUS> control(
    const std::wstring& name, const ulong ctrl_code);

  VUResult install(
    const std::wstring& file_path,
    const std::wstring& service_name,
    const std::wstring& display_name,
    const ulong service_type = SERVICE_KERNEL_DRIVER,
    const ulong access_type = SERVICE_ALL_ACCESS,
    const ulong start_type = SERVICE_DEMAND_START,
    const ulong ctrl_type = SERVICE_ERROR_NORMAL
  );

  VUResult uninstall(const std::wstring& name);
  VUResult start(const std::wstring& name);
  VUResult stop(const std::wstring& name);

protected:
  virtual VUResult initialize();
};

/**
 * File Mapping
 */

enum page_protection
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
};

class FileMappingX : public LastError
{
public:
  enum desired_access
  {
    DA_UNKNOWN = -1,
    DA_ALL_ACCESS = FILE_MAP_ALL_ACCESS,
    DA_READ = FILE_MAP_READ,
    DA_WRITE = FILE_MAP_WRITE,
    DA_COPY = FILE_MAP_COPY,
    DA_EXECUTE = FILE_MAP_EXECUTE,
    // DA_LARGE_PAGES = FILE_MAP_LARGE_PAGES,
    // DA_TARGETS_INVALID = FILE_MAP_TARGETS_INVALID,
  };

  FileMappingX();
  virtual ~FileMappingX();

  void* vuapi view(
    desired_access the_desired_access = desired_access::DA_ALL_ACCESS,
    ulong max_file_offset_low = 0,
    ulong max_file_offset_high = 0,
    ulong number_of_bytes_to_map = 0 // The mapping extends from the specified offset to the end.
  );

  ulong vuapi get_file_size();
  void vuapi close();

protected:
  bool valid(HANDLE handle);

protected:
  HANDLE m_file_handle;
  HANDLE m_map_handle;
  void* m_ptr_data;
};

/**
 * FileMappingA
 */

class FileMappingA : public FileMappingX
{
public:
  FileMappingA();
  virtual ~FileMappingA();

  VUResult vuapi create_within_file(
    const std::string& file_path,
    ulong max_size_low  = 0, // The file to map, Hi & Low = 0 is mapping whole file as default.
    ulong max_size_high = 0, // The file to map, Hi & Low = 0 is mapping whole file as default.
    fs_generic fg_flags = fs_generic::FG_ALL,
    fs_share fs_flags = fs_share::FS_ALLACCESS,
    fs_mode fm_flags = fs_mode::FM_OPENEXISTING,
    fs_attribute fa_flags = fs_attribute::FA_NORMAL,
    page_protection pp_flags = page_protection::PP_EXECUTE_READ_WRITE
  );

  /**
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi create_named_shared_memory(
    const std::string& mapping_name,
    ulong max_size_low, // The mapping size for file mapping object.
    ulong max_size_high = 0,
    page_protection pp_flags = page_protection::PP_EXECUTE_READ_WRITE
  );

  /**
   * Just for Named Shared Memory.
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi open(
    const std::string& mapping_name,
    desired_access desired_access = desired_access::DA_ALL_ACCESS,
    bool inherit_handle = false
  );
};

/**
 * FileMappingW
 */

class FileMappingW : public FileMappingX
{
public:
  FileMappingW();
  virtual ~FileMappingW();

  VUResult vuapi create_within_file(
    const std::wstring& file_path,
    ulong max_size_low = 0, // The file to map, Hi & Low = 0 is mapping whole file as default.
    ulong max_size_high = 0, // The file to map, Hi & Low = 0 is mapping whole file as default.
    fs_generic fg_flags = fs_generic::FG_ALL,
    fs_share fs_flags = fs_share::FS_ALLACCESS,
    fs_mode fm_flags = fs_mode::FM_OPENEXISTING,
    fs_attribute fa_flags = fs_attribute::FA_NORMAL,
    page_protection pp_flags = page_protection::PP_EXECUTE_READ_WRITE
  );

  /**
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi create_named_shared_memory(
    const std::wstring& mapping_name,
    ulong max_size_low, // The mapping size for file mapping object.
    ulong max_size_high = 0,
    page_protection pp_flags = page_protection::PP_EXECUTE_READ_WRITE
  );

  /**
   * Just for Named Shared Memory.
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi open(
    const std::wstring& mapping_name,
    desired_access desired_access = desired_access::DA_ALL_ACCESS,
    bool inherit_handle = false
  );
};

/**
 * INI File
 */

template <class INIFileX, class VariantX, class StringT>
class INISectionT
{
public:
  INISectionT(INIFileX& ini, const StringT& section) : m_ini(ini), m_section(section) {}
  virtual ~INISectionT() {}

  VariantX read(const StringT& key)
  {
    return m_ini.read(m_section, key);
  }

  template <typename T>
  bool write(const StringT& key, const T val)
  {
    VariantX var;
    var << val;
    return this->write_variant(key, var);
  }

  template <typename Ptr>
  bool write(const std::string& key, const Ptr ptr, const size_t size)
  {
    VariantA var;
    var << to_hex_string_A((const byte*)ptr, size);
    return this->write_variant(key, var);
  }

  template <typename Ptr>
  bool write(const std::wstring& key, const Ptr ptr, const size_t size)
  {
    VariantW var;
    var << to_hex_string_W((const byte*)ptr, size);
    return this->write_variant(key, var);
  }

protected:
  bool write_variant(const StringT& key, const VariantX& var)
  {
    return m_ini.write_string(m_section, key, var.to_string());
  }

protected:
  INIFileX& m_ini;
  StringT m_section;
};

#define INISectionTA INISectionT<INIFileA, VariantA, std::string>
#define INISectionTW INISectionT<INIFileW, VariantW, std::wstring>

class INIFileA : public LastError
{
public:
  INIFileA();
  INIFileA(const std::string& file_path);
  virtual ~INIFileA();

  class Section : public INISectionTA
  {
  public:
    Section(INIFileA& ini, const std::string& section) : INISectionT(ini, section) {}
    virtual ~Section() {}
  };

  friend class INISectionTA;

  std::unique_ptr<Section> section(const std::string& name);
  bool read_section_names(std::vector<std::string>& section_names, const ulong max_chars = KiB);

  VariantA read(const std::string& section, const std::string& key);
  bool write(const std::string& section, const std::string& key, const VariantA& var);

protected:
  std::string read_string(const std::string& section, const std::string& key, const std::string& def);
  bool write_string(const std::string& section, const std::string& key, const std::string& val);

protected:
  std::string m_file_path;
};

class INIFileW : public LastError
{
public:
  INIFileW();
  INIFileW(const std::wstring& file_path);
  virtual ~INIFileW();

  class Section : public INISectionTW
  {
  public:
    Section(INIFileW& ini, const std::wstring& section) : INISectionT(ini, section) {}
    virtual ~Section() {}
  };

  friend class INISectionTW;

  std::unique_ptr<Section> section(const std::wstring& name);
  bool read_section_names(std::vector<std::wstring>& section_names, const ulong max_chars = KiB);

  VariantW read(const std::wstring& section, const std::wstring& key);
  bool write(const std::wstring& section, const std::wstring& key, const VariantW& var);

protected:
  std::wstring read_string(const std::wstring& section, const std::wstring& key, const std::wstring& def);
  bool write_string(const std::wstring& section, const std::wstring& key, const std::wstring& val);

protected:
  std::wstring m_file_path;
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

enum class registry_key : ulongptr
{
  HKCR = ulongptr(0x80000000),
  HKCU = ulongptr(0x80000001),
  HKLM = ulongptr(0x80000002),
  HKU  = ulongptr(0x80000003),
  HKPD = ulongptr(0x80000004),
  HKCF = ulongptr(0x80000005),
};

#ifndef KEY_WOW64_64KEY
#define KEY_WOW64_64KEY 0x0100
#endif

#ifndef KEY_WOW64_32KEY
#define KEY_WOW64_32KEY 0x0200
#endif

#ifndef KEY_WOW64_RES
#define KEY_WOW64_RES 0x0300
#endif

enum class registry_access
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
};

enum class registry_reflection
{
  RR_ERROR    = -1,
  RR_DISABLED = 0,
  RR_ENABLED  = 1,
  RR_DISABLE  = 2,
  RR_ENABLE   = 3,
};

class RegistryX : public LastError
{
public:
  RegistryX();
  virtual ~RegistryX();

  HKEY vuapi get_current_key_handle();
  registry_reflection vuapi query_reflection_key();
  bool vuapi set_reflection_key(registry_reflection reg_reflection);
  bool vuapi close_key();

protected:
  HKEY m_hk_root_key;
  HKEY m_hk_sub_key;
};

class RegistryA : public RegistryX
{
public:
  RegistryA();
  RegistryA(registry_key reg_root);
  RegistryA(registry_key reg_root, const std::string& sub_key);
  virtual ~RegistryA();

  ulong vuapi set_size_of_multi_string(const char* multi_string);
  ulong vuapi get_data_size(const std::string& value_name, ulong type);

  bool vuapi create_key();
  bool vuapi create_key(const std::string& sub_key);
  bool vuapi key_exists();
  bool vuapi key_exists(const std::string& sub_key);
  bool vuapi open_key(registry_access reg_access = registry_access::RA_ALL_ACCESS);
  bool vuapi open_key(const std::string& sub_key, registry_access reg_access = registry_access::RA_ALL_ACCESS);
  bool vuapi delete_key();
  bool vuapi delete_key(const std::string& sub_key);
  bool vuapi delete_value(const std::string& value);

  std::vector<std::string> vuapi enum_keys();
  std::vector<std::string> vuapi enum_values();

  bool vuapi write_integer(const std::string& name, int value);
  bool vuapi write_bool(const std::string& name, bool value);
  bool vuapi write_float(const std::string& name, float value);
  bool vuapi write_string(const std::string& name, const std::string& value);
  bool vuapi write_multi_string(const std::string& name, const char* ps_value);
  bool vuapi write_multi_string(const std::string& name, const std::vector<std::string>& value);
  bool vuapi write_expand_string(const std::string& name, const std::string& value);
  bool vuapi write_binary(const std::string& name, void* ptr_data, ulong size);

  int vuapi read_integer(const std::string& name, int default_value);
  bool vuapi read_bool(const std::string& name, bool default_value);
  float vuapi read_float(const std::string& name, float default_value);
  std::string vuapi read_string(
    const std::string& name, const std::string& default_value);
  std::vector<std::string> vuapi read_multi_string(
    const std::string& name, const std::vector<std::string> default_value);
  std::string vuapi read_expand_string(
    const std::string& name, const std::string& default_value);
  std::unique_ptr<uchar[]> vuapi read_binary(
    const std::string& name, const void* pdefault_value);

private:
  std::string m_sub_key;
};

class RegistryW : public RegistryX
{
public:
  RegistryW();
  RegistryW(registry_key reg_root);
  RegistryW(registry_key reg_root, const std::wstring& sub_key);
  virtual ~RegistryW();

  ulong vuapi set_size_of_multi_string(const wchar* multi_string);
  ulong vuapi get_data_size(const std::wstring& value_name, ulong type);

  bool vuapi create_key();
  bool vuapi create_key(const std::wstring& sub_key);
  bool vuapi key_exists();
  bool vuapi key_exists(const std::wstring& sub_key);
  bool vuapi open_key(registry_access reg_access = registry_access::RA_ALL_ACCESS);
  bool vuapi open_key(const std::wstring& sub_key, registry_access reg_access = registry_access::RA_ALL_ACCESS);
  bool vuapi delete_key();
  bool vuapi delete_key(const std::wstring& sub_key);
  bool vuapi delete_value(const std::wstring& value);

  std::vector<std::wstring> vuapi enum_keys();
  std::vector<std::wstring> vuapi enum_values();

  bool vuapi write_integer(const std::wstring& value_name, int value);
  bool vuapi write_bool(const std::wstring& value_name, bool value);
  bool vuapi write_float(const std::wstring& value_name, float value);
  bool vuapi write_string(const std::wstring& value_name, const std::wstring& value);
  bool vuapi write_multi_string(const std::wstring& value_name, const wchar* ps_value);
  bool vuapi write_multi_string(const std::wstring& value_name, const std::vector<std::wstring> value);
  bool vuapi write_expand_string(const std::wstring& value_name, const std::wstring& value);
  bool vuapi write_binary(const std::wstring& value_name, void* ptr_data, ulong size);

  int vuapi read_integer(const std::wstring& value_name, int default_value);
  bool vuapi read_bool(const std::wstring& value_name, bool default_value);
  float vuapi read_float(const std::wstring& value_name, float default_value);
  std::wstring vuapi read_string(
    const std::wstring& value_name, const std::wstring& default_value);
  std::vector<std::wstring> vuapi read_multi_string(
    const std::wstring& value_name, const std::vector<std::wstring> default_value);
  std::wstring vuapi read_expand_string(
    const std::wstring& value_name, const std::wstring& default_value);
  std::unique_ptr<uchar[]> vuapi read_binary(
    const std::wstring& value_name, const void* default_value);

private:
  std::wstring m_sub_key;
};

/**
 * ThreadLock
 */

class ThreadLock
{
public:
  ThreadLock();
  virtual ~ThreadLock();

  void lock();
  void unlock();

private:
  CRITICAL_SECTION m_cs;
};

/**
 * GlobalThreadLock
 */

class _GlobalThreadLock
{
public:
  _GlobalThreadLock(int thread_lock_id);
  virtual ~_GlobalThreadLock();

private:
  int m_thread_lock_id;
  static std::map<int, std::unique_ptr<ThreadLock>> m_list;
};

#define ScopedThreadLock() _GlobalThreadLock _scoped_thread_lock(__COUNTER__)

/**
 * ThreadSignal
 */

class ThreadSignal
{
public:
  ThreadSignal(const bool waiting = false);
  ThreadSignal(const ThreadSignal& right);

  virtual ~ThreadSignal();

  operator bool() const;
  bool get() const;
  void set(bool waiting);
  void notify();
  bool wait(ulong time_out = INFINITE) const;

  bool operator==(ThreadSignal& right);
  bool operator!=(ThreadSignal& right);
  bool operator==(bool waiting);
  bool operator!=(bool waiting);
  const ThreadSignal& operator=(bool waiting);
  const ThreadSignal& operator=(const ThreadSignal& right);

private:
  HANDLE m_event;
};

/**
 * Stop Watch
 */

class StopWatch
{
public:
  typedef std::pair<unsigned long, float> TDuration;

public:
  StopWatch();
  virtual ~StopWatch();
  void start(bool reset = false);
  const TDuration stop();
  const TDuration duration();
  const TDuration total();

private:
  bool m_reset;
  std::clock_t m_count, m_delta;
  std::vector<std::clock_t> m_delta_history;
};

/**
 * ScopeStopWatchX
 */

class ScopeStopWatchX
{
public:
  ScopeStopWatchX();
  virtual ~ScopeStopWatchX();

  void reset();
  void lap();
  void active(bool state = true);

protected:
  void start(bool reset = false);
  void stop();

protected:
  bool m_activated;
  StopWatch m_watcher;
};

class ScopeStopWatchA : public ScopeStopWatchX
{
public:
  typedef std::function<void(const std::string& text, const StopWatch::TDuration& duration)> FnLogging;

  ScopeStopWatchA(
    const std::string& prefix, const std::string& suffix = " ", const FnLogging fn_logging = message);
  virtual ~ScopeStopWatchA();

  void log(const std::string format, ...);

  static void message(const std::string& text, const StopWatch::TDuration& duration);
  static void console(const std::string& text, const StopWatch::TDuration& duration);

private:
  std::string m_prefix;
  std::string m_suffix;
  FnLogging m_fn_logging;
};

class ScopeStopWatchW : public ScopeStopWatchX
{
public:
  typedef std::function<void(const std::wstring& text, const StopWatch::TDuration& duration)> FnLogging;

  ScopeStopWatchW(
    const std::wstring& prefix, const std::wstring& suffix = L" ", const FnLogging fn_logging = message);
  virtual ~ScopeStopWatchW();

  void log(const std::wstring format, ...);

  static void message(const std::wstring& text, const StopWatch::TDuration& duration);
  static void console(const std::wstring& text, const StopWatch::TDuration& duration);

private:
  std::wstring m_prefix;
  std::wstring m_suffix;
  FnLogging m_fn_logging;
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

typedef IMAGE_DOS_HEADER DOSHeader, *PDOSHeader;
typedef IMAGE_FILE_HEADER FileHeader, *PFileHeader;
typedef _IMAGE_SECTION_HEADER SectionHeader, *PSectionHeader;
typedef IMAGE_IMPORT_BY_NAME ImportByName, *PImportByName;
typedef IMAGE_IMPORT_DESCRIPTOR ImportDescriptor, *PImportDescriptor;
typedef IMAGE_DATA_DIRECTORY DataDirectory, *PDataDirectory;

// IMAGE_OPTIONAL_HEADER

template <typename T>
struct OPTHeaderT
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
      DataDirectory Export;        // IMAGE_DIRECTORY_ENTRY_EXPORT
      DataDirectory Import;        // IMAGE_DIRECTORY_ENTRY_IMPORT
      DataDirectory Resource;      // IMAGE_DIRECTORY_ENTRY_RESOURCE
      DataDirectory Exception;     // IMAGE_DIRECTORY_ENTRY_EXCEPTION
      DataDirectory Security;      // IMAGE_DIRECTORY_ENTRY_SECURITY
      DataDirectory Relocation;    // IMAGE_DIRECTORY_ENTRY_BASERELOC
      DataDirectory Debug;         // IMAGE_DIRECTORY_ENTRY_DEBUG
      DataDirectory Architecture;  // IMAGE_DIRECTORY_ENTRY_ARCHITECTURE
      DataDirectory Global;        // IMAGE_DIRECTORY_ENTRY_GLOBALPTR
      DataDirectory TLS;           // IMAGE_DIRECTORY_ENTRY_TLS
      DataDirectory Config;        // IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG
      DataDirectory Bound;         // IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT
      DataDirectory IAT;           // IMAGE_DIRECTORY_ENTRY_IAT
      DataDirectory Delay;         // IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT
      DataDirectory CLR;           // IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR
      DataDirectory Reversed;      // Reversed
    };
    DataDirectory DataDirectory[MAX_IDD];
  };
};

template<> struct OPTHeaderT<pe64>
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
      DataDirectory Export;        // IMAGE_DIRECTORY_ENTRY_EXPORT
      DataDirectory Import;        // IMAGE_DIRECTORY_ENTRY_IMPORT
      DataDirectory Resource;      // IMAGE_DIRECTORY_ENTRY_RESOURCE
      DataDirectory Exception;     // IMAGE_DIRECTORY_ENTRY_EXCEPTION
      DataDirectory Security;      // IMAGE_DIRECTORY_ENTRY_SECURITY
      DataDirectory Relocation;    // IMAGE_DIRECTORY_ENTRY_BASERELOC
      DataDirectory Debug;         // IMAGE_DIRECTORY_ENTRY_DEBUG
      DataDirectory Architecture;  // IMAGE_DIRECTORY_ENTRY_ARCHITECTURE
      DataDirectory Global;        // IMAGE_DIRECTORY_ENTRY_GLOBALPTR
      DataDirectory TLS;           // IMAGE_DIRECTORY_ENTRY_TLS
      DataDirectory Config;        // IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG
      DataDirectory Bound;         // IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT
      DataDirectory IAT;           // IMAGE_DIRECTORY_ENTRY_IAT
      DataDirectory Delay;         // IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT
      DataDirectory CLR;           // IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR
      DataDirectory Reversed;      // Reversed
    };
    DataDirectory DataDirectory[MAX_IDD];
  };
};

typedef OPTHeaderT<ulong32> OPTHeader32, *POptHeader32;
typedef OPTHeaderT<ulong64> OPTHeader64, *POptHeader64;

// IMAGE_NT_HEADERS

template <typename T>
struct NTHeaderT
{
  ulong Signature;
  FileHeader FileHeader;
  OPTHeaderT<T> OptHeader;
};

typedef NTHeaderT<ulong32> NTHeader32, *PNTHeader32;
typedef NTHeaderT<ulong64> NTHeader64, *PNTHeader64;

// PE_HEADER

#define TPEHeaderT NTHeaderT

typedef NTHeader32 PEHeader32, *PPEHeader32;
typedef NTHeader64 PEHeader64, *PPEHeader64;

// IMAGE_THUNK_DATA

template <typename T>
struct ThunkDataT
{
  union
  {
    T ForwarderString;
    T Function;
    T Ordinal;
    T AddressOfData; // PIMAGE_IMPORT_BY_NAME
  } u1;
};

typedef ThunkDataT<ulong32> ThunkData32;
typedef ThunkDataT<ulong64> ThunkData64;

/* The common types (32-bit & 64-bit)  */

#ifdef _WIN64
typedef NTHeader64   NTHeader,  *PNTHeader;
typedef OPTHeader64  OPTHeader, *POPTHeader;
typedef PEHeader64   PEHeader,  *PPEHeader;
typedef ThunkData64  ThunkData, *PThunkData;
#else // _WIN32
typedef NTHeader32   NTHeader,  *PNTHeader;
typedef OPTHeader32  OPTHeader, *POPTHeader;
typedef ThunkData32  ThunkData, *PThunkData;
typedef PEHeader32   PEHeader,  *PPEHeader;
#endif

struct ImportDescriptorEx
{
  ulong iid_id;
  std::string name;
  PImportDescriptor ptr_iid;
};

struct ImportModule
{
  ulong iid_id;
  std::string name;
  // ulong number_of_functions;
};

template<typename T>
struct ImportFunctionT
{
  ulong iid_id;
  std::string name;
  T ordinal;
  ushort hint;
  T rva;
};

typedef ImportFunctionT<ulong32> ImportFunction32T;
typedef ImportFunctionT<ulong64> ImportFunction64T;

template<typename T>
struct RelocationEntryT
{
  int type;
  // T Offset;
  T rva;
  // T VA;
  T value;
};

template <typename T>
class PEFileTX
{
public:
  enum class find_by
  {
    HINT,
    NAME,
  };

  PEFileTX();
  virtual ~PEFileTX();

  void* vuapi get_ptr_base();
  TPEHeaderT<T>* vuapi get_ptr_pe_header();

  T vuapi rva_to_offset(T RVA, bool in_cache = true);
  T vuapi offset_to_rva(T Offset, bool in_cache = true);

  const std::vector<PSectionHeader>& vuapi get_setion_headers(bool in_cache = true);

  const std::vector<PImportDescriptor>& vuapi get_import_descriptors(bool in_cache = true);
  const std::vector<ImportModule> vuapi get_import_modules(bool in_cache = true);
  const std::vector<ImportFunctionT<T>> vuapi get_import_functions(bool in_cache = true); // Did not include import by index

  const ImportModule* vuapi find_ptr_import_module(
    const std::string& module_name, bool in_cache = true);

  const ImportFunctionT<T>* vuapi find_ptr_import_function(
    const std::string& function_name, bool in_cache = true);
  const ImportFunctionT<T>* vuapi find_ptr_import_function(
    const ushort function_hint, bool in_cache = true);
  const ImportFunctionT<T>* vuapi find_ptr_import_function(
    const ImportFunctionT<T>& import_function,
    const find_by method,
    bool in_cache = true);

  const std::vector<RelocationEntryT<T>> vuapi get_relocation_entries(bool in_cache = true);

protected:
  bool m_initialized;

  void* m_ptr_base;

  DOSHeader* m_ptr_dos_header;
  TPEHeaderT<T>* m_ptr_pe_header;

private:
  T m_ordinal_flag;

  std::vector<ImportDescriptorEx> m_ex_iids;

  std::vector<PSectionHeader> m_section_headers;
  std::vector<PImportDescriptor> m_import_descriptors;
  std::vector<ImportModule> m_import_modules;
  std::vector<ImportFunctionT<T>> m_import_functions;
  std::vector<RelocationEntryT<T>> m_relocation_entries;

protected:
  const std::vector<ImportDescriptorEx>& vuapi get_ex_iids(bool in_cache = true);
};

template <typename T>
class PEFileTA : public PEFileTX<T>
{
public:
  PEFileTA() {};
  PEFileTA(const std::string& pe_file_path);
  virtual ~PEFileTA();

  VUResult vuapi parse();

private:
  std::string m_file_path;
  FileMappingA m_file_map;
};

template <typename T>
class PEFileTW : public PEFileTX<T>
{
public:
  PEFileTW() {};
  PEFileTW(const std::wstring& pe_file_path);
  virtual ~PEFileTW();

  VUResult vuapi parse();

private:
  std::wstring m_file_path;
  FileMappingW m_file_map;
};

/**
 * WDTControl
 */

class WDTControl
{
public:
  enum control_class_name : WORD
  {
    CT_BUTTON     = 0x0080,
    CT_EDIT       = 0x0081,
    CT_STATIC     = 0x0082,
    CT_LIST_BOX   = 0x0083,
    CT_SCROLL_BAR = 0x0084,
    CT_COMBO_BOX  = 0x0085,
    CT_UNKNOW     = 0xFFFF,
  };

  WDTControl(
    const std::wstring& caption,
    const control_class_name type,
    const WORD  id,
    const short x,
    const short y,
    const short cx,
    const short cy,
    const DWORD style,
    const DWORD exstyle = 0
  );

  virtual ~WDTControl();

  void serialize(void** pptr);

protected:
  std::wstring m_caption;
  DLGITEMTEMPLATE m_shape;
  WORD m_w_class;
  WORD m_w_type;
  WORD m_w_data;
};

class WDTDialog : public LastError
{
public:
  WDTDialog(
    const std::wstring& caption,
    const DWORD style,
    const DWORD exstyle,
    const short x,
    const short y,
    const short cx,
    const short cy,
    HWND hwnd_parent = nullptr
  );

  virtual ~WDTDialog();

  void add(const WDTControl& control);
  void serialize(void** pptr);
  INT_PTR do_modal(DLGPROC pfn_dlg_proc, WDTDialog* ptr_parent);

  const std::vector<WDTControl>& controls() const;

protected:
  HGLOBAL m_hglobal;
  std::wstring m_caption;
  std::wstring m_font;
  HWND m_hwnd_parent;
  std::vector<WDTControl> m_controls;
  DLGTEMPLATE m_shape;
  WORD m_w_menu;
  WORD m_w_class;
  WORD m_w_font;
};

class InputDialog : public WDTDialog
{
public:
  const WORD IDC_LABEL;
  const WORD IDC_INPUT;

  InputDialog(
    const std::wstring& label,
    const HWND hwnd_parent = nullptr,
    bool number_only = false,
    const std::wstring& placeholder = L"");
  virtual ~InputDialog();

  void placeholder(const std::wstring& pl);
  void label(const std::wstring& label);
  VariantW& input();

  bool do_modal();

  static LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

private:
  std::wstring m_placeholder;
  std::wstring m_label;
  VariantW m_input;
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
  DWORD  cb;
  DWORD  PageFaultCount;
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

#define DEF_SM_STATE      MEM_COMMIT
#define DEF_SM_PAGE       MEM_IMAGE
#define DEF_SM_PROTECTION PAGE_ALL_PROTECTION & ~(PAGE_NOACCESS | PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE)

class ProcessX : public LastError
{
public:
  typedef std::vector<THREADENTRY32> threads;
  typedef std::vector<MEMORY_BASIC_INFORMATION> memories;

  ProcessX();
  // ProcessX(const ulong pid);
  virtual ~ProcessX();

  ProcessX(ProcessX& right);
  ProcessX& operator=(ProcessX& right);
  bool operator==(ProcessX& right);
  bool operator!=(ProcessX& right);

  const ulong  pid() const;
  const HANDLE handle() const;
  const wow_64 wow64() const;
  const arch  bit() const;

  bool ready();

  bool attach(const ulong pid);
  bool attach(const HANDLE hp);

  void detach();

  bool read_memory(const ulongptr address, Buffer& buffer, const bool force = false);
  bool read_memory(const ulongptr address, void* ptr_data, const size_t size, const bool force = false);
  bool write_memory(const ulongptr address, const Buffer& buffer, const bool force = false);
  bool write_memory(const ulongptr address, const void* ptr_data, const size_t size, const bool force = false);

  void execute_code_at(const ulongptr address, void* ptr_params = nullptr, const bool wait_completed = false);

  PROCESS_CPU_COUNTERS get_cpu_information(const double interval = 1.); // 1 second
  PROCESS_MEMORY_COUNTERS get_memory_information();
  PROCESS_TIME_COUNTERS get_time_information();
  PROCESS_IO_COUNTERS get_io_information();

  const threads& get_threads();
  const memories& get_memories(
    const ulong state = MEM_ALL_STATE,
    const ulong type  = MEM_ALL_TYPE,
    const ulong protection = PAGE_ALL_PROTECTION);

protected:
  virtual void parse();

private:
  bool   close(const HANDLE hp);
  HANDLE open(const ulong pid);
  double get_cpu_percent_usage();

protected:
  arch  m_bit;
  ulong  m_pid;
  HANDLE m_handle;
  wow_64 m_wow64;

  bool m_attached;

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

class ProcessA : public ProcessX
{
public:
  typedef std::vector<MODULEENTRY32> modules;

  ProcessA();
  // ProcessA(const ulong pid);
  virtual ~ProcessA();

  ProcessA(ProcessA& right);
  ProcessA& operator=(ProcessA& right);
  bool operator==(ProcessA& right);
  bool operator!=(ProcessA& right);
  friend std::ostream& operator<<(std::ostream& os, ProcessA& process);

  bool create(
    const std::string& file_path,
    const std::string& file_dir = "",
    const std::string& command_line = "",
    const uint creation_flags = NORMAL_PRIORITY_CLASS,
    const bool inherit_handle = false,
    PROCESS_INFORMATION* ptr_pi = nullptr);

  const std::string& name() const;
  const std::string get_path();
  const modules& get_modules();

  const MODULEENTRY32 get_module_information();

  bool scan_memory(
    std::vector<size_t>& addresses,
    const std::string& pattern,
    const std::string& module_name = "",
    const bool first_match_only = false,
    const ulong state = DEF_SM_STATE,
    const ulong type = DEF_SM_PAGE,
    const ulong protection = DEF_SM_PROTECTION);

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

class ProcessW : public ProcessX
{
public:
  typedef std::vector<MODULEENTRY32W> modules;

  ProcessW();
  // ProcessW(const ulong pid);
  virtual ~ProcessW();

  ProcessW(ProcessW& right);
  ProcessW& operator=(ProcessW& right);
  bool operator==(ProcessW& right);
  bool operator!=(ProcessW& right);
  friend std::ostream& operator<<(std::ostream& os, ProcessW& process);

  bool create(
    const std::wstring& file_path,
    const std::wstring& file_dir = L"",
    const std::wstring& command_line = L"",
    const uint creation_flags = NORMAL_PRIORITY_CLASS,
    const bool inherit_handle = false,
    PROCESS_INFORMATION* ptr_pi = nullptr);

  const std::wstring& name() const;
  const std::wstring get_path();
  const modules& get_modules();

  const MODULEENTRY32W get_module_information();

  bool scan_memory(
    std::vector<size_t>& addresses,
    const std::wstring& pattern,
    const std::wstring& module_name = L"",
    const bool first_match_only = false,
    const ulong state = DEF_SM_STATE,
    const ulong type = DEF_SM_PAGE,
    const ulong protection = DEF_SM_PROTECTION);

protected:
  virtual void parse();

protected:
  std::wstring m_name;
  modules m_modules;
};

/**
 * Single Process
 */

#define VU_SINGLE_PROCESS(app) vu::SingleProcess::instance().initialize(STRINGIZE(app));
#define VU_SINGLE_PROCESS_EX(app, fn) vu::SingleProcess::instance().initialize(STRINGIZE(app), fn);

class SingleProcess : public SingletonT<SingleProcess>
{
public:
  SingleProcess();
  virtual ~SingleProcess();

  static void fn_default_ask_to_launch_new_instance(bool running);
  static void fn_default_allow_only_single_instance(bool running);

  bool initialize(
    const std::string& name,
    const std::function<void(bool running)> fn = fn_default_ask_to_launch_new_instance);
  bool initialize(
    const std::wstring& name,
    const std::function<void(bool running)> fn = fn_default_ask_to_launch_new_instance);
  void finalize();
  bool running();

private:
  HANDLE m_handle;
  std::wstring m_name;
};

#ifdef Vutils_EXPORTS
#define threadpool11_EXPORTING
#endif // Vutils_EXPORTS

/**
 * Thread Pool
 */

#define MAX_NTHREADS -1

class ThreadPool
{
public:
  ThreadPool(size_t n_threads = MAX_NTHREADS);
  virtual ~ThreadPool();

  void add_task(fn_task_t&& fn);
  void launch();

  size_t worker_count() const;
  size_t work_queue_count() const;

  size_t active_worker_count() const;
  size_t inactive_worker_count() const;

private:
  Pool* m_ptr_impl;
};

#include "template/stlthread.tpl"

/**
 * Path
 */

class PathA
{
public:
  PathA(const path_separator separator = path_separator::WIN);
  PathA(const std::string& path, const path_separator separator = path_separator::WIN);
  PathA(const PathA& right);
  virtual ~PathA();

  const PathA& operator=(const PathA& right);
  const PathA& operator=(const std::string& right);
  const PathA& operator+=(const PathA& right);
  const PathA& operator+=(const std::string& right);
  PathA operator+(const PathA& right);
  PathA operator+(const std::string& right);
  bool operator==(const PathA& right);
  bool operator!=(const PathA& right);
  friend std::ostream& operator<<(std::ostream& os, PathA& path);

  PathA& trim(const trim_type type = trim_type::TS_BOTH);
  PathA& normalize();
  PathA& join(const std::string& path);
  PathA& finalize();
  bool exists() const;
  std::string as_string() const;

  PathA extract_name(bool with_extension = true) const;
  PathA extract_directory(bool with_slash = true) const;

private:
  path_separator m_separator;
  std::string m_path;
};

class PathW
{
public:
  PathW(const path_separator separator = path_separator::WIN);
  PathW(const std::wstring& path, const path_separator separator = path_separator::WIN);
  PathW(const PathW& right);
  virtual ~PathW();

  const PathW& operator=(const PathW& right);
  const PathW& operator=(const std::wstring& right);
  const PathW& operator+=(const PathW& right);
  const PathW& operator+=(const std::wstring& right);
  PathW operator+(const PathW& right);
  PathW operator+(const std::wstring& right);
  bool operator==(const PathW& right);
  bool operator!=(const PathW& right);
  friend std::wostream& operator<<(std::wostream& os, PathW& path);

  PathW& trim(const trim_type type = trim_type::TS_BOTH);
  PathW& normalize();
  PathW& join(const std::wstring& path);
  PathW& finalize();
  bool exists() const;
  std::wstring as_string() const;

  PathW extract_name(bool with_extension = true) const;
  PathW extract_directory(bool with_slash = true) const;

private:
  path_separator m_separator;
  std::wstring m_path;
};

/**
 * WMIProvider - Windows Management Instrumentation
 */

#ifdef VU_WMI_ENABLED

class WMIProviderX;

class WMIProviderA
{
public:
  WMIProviderA();
  virtual ~WMIProviderA();

  virtual bool ready();
  bool begin(const std::string& ns);
  bool end();

  IEnumWbemClassObject* query(const std::string& qs);
  bool query(const std::string& qs, const std::function<bool(IWbemClassObject& object)> fn);

private:
  WMIProviderX* m_ptr_impl;
};

class WMIProviderW
{
public:
  WMIProviderW();
  virtual ~WMIProviderW();

  virtual bool ready();
  bool begin(const std::wstring& ns);
  bool end();

  IEnumWbemClassObject* query(const std::wstring& qs);
  bool query(const std::wstring& qs, const std::function<bool(IWbemClassObject& object)> fn);

private:
  WMIProviderX* m_ptr_impl;
};

#endif // VU_WMI_ENABLED

/**
 * Picker
 */

#ifndef _INC_COMMDLG
#define CC_RGBINIT               0x00000001
#define CC_FULLOPEN              0x00000002
#define OFN_PATHMUSTEXIST        0x00000800
#define OFN_FILEMUSTEXIST        0x00001000
#if(WINVER >= 0x0400)
#define OFN_EXPLORER             0x00080000
#endif /* WINVER >= 0x0400 */
#define CF_SCREENFONTS           0x00000001
#define CF_EFFECTS               0x00000100L
#endif // _INC_COMMDLG

#ifndef SHFOLDERAPI
#define BIF_RETURNONLYFSDIRS    0x00000001
#define BIF_EDITBOX             0x00000010
#define BIF_NEWDIALOGSTYLE      0x00000040
#define BIF_USENEWUI            (BIF_NEWDIALOGSTYLE | BIF_EDITBOX)
#endif // SHFOLDERAPI

class PickerX
{
public:
  PickerX(HWND hwnd = nullptr);
  virtual ~PickerX();

  enum class action_type
  {
    open,
    save,
  };

  bool choose_rgb_color(COLORREF& color, const ulong flags = CC_RGBINIT | CC_FULLOPEN);

protected:
  HWND m_hwnd;
};

static const ulong DEFAULT_CHOOSE_FONT_FLAGS = CF_SCREENFONTS | CF_EFFECTS;
static const ulong DEFAULT_CHOOSE_DIR_FLAGS = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
static const ulong DEFAULT_CHOOSE_FILE_FLAGS = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
static const char  DEFAULT_CHOOSE_FILE_FILTERS_A[] =  "All Files (*.*)\0*.*\0";
static const wchar DEFAULT_CHOOSE_FILE_FILTERS_W[] = L"All Files (*.*)\0*.*\0";

// PickerA

class PickerA : public PickerX
{
public:
  PickerA(HWND hwnd = nullptr);
  virtual ~PickerA();

  bool choose_log_font(LOGFONTA& font, const ulong flags = DEFAULT_CHOOSE_FONT_FLAGS);

  bool choose_directory(std::string& directory, const ulong flags = DEFAULT_CHOOSE_DIR_FLAGS);

  bool choose_file(
    const action_type action,
    std::string& file_path,
    const std::string& initial_dir = "",
    const char* filters = DEFAULT_CHOOSE_FILE_FILTERS_A,
    const ulong flags = DEFAULT_CHOOSE_FILE_FLAGS);

  bool choose_files(
    std::vector<std::string>& file_names,
    std::string& file_dir,
    const char* filters = DEFAULT_CHOOSE_FILE_FILTERS_A,
    const ulong flags = DEFAULT_CHOOSE_FILE_FLAGS);
};

// PickerW

class PickerW : public PickerX
{
public:
  PickerW(HWND hwnd = nullptr);
  virtual ~PickerW();

  bool choose_log_font(LOGFONTW& font, const ulong flags = DEFAULT_CHOOSE_FONT_FLAGS);

  bool choose_directory(std::wstring& directory, const ulong flags = DEFAULT_CHOOSE_DIR_FLAGS);

  bool choose_file(
    const action_type action,
    std::wstring& file_path,
    const std::wstring& initial_dir = L"",
    const wchar* filters = DEFAULT_CHOOSE_FILE_FILTERS_W,
    const ulong flags = DEFAULT_CHOOSE_FILE_FLAGS);

  bool choose_files(
    std::vector<std::wstring>& file_names,
    std::wstring& file_dir,
    const wchar* filters = DEFAULT_CHOOSE_FILE_FILTERS_W,
    const ulong flags = DEFAULT_CHOOSE_FILE_FLAGS);
};

/**
 * RESTClient
 */

#if defined(VU_INET_ENABLED)

enum class protocol_scheme
{
  http,
  https,
};

using namespace WinHttpWrapper;

class RESTClientW;

typedef HttpResponse http_response;

// RESTClientA

class RESTClientA
{
public:
  RESTClientA(
    const protocol_scheme scheme,
    const std::string& host,
    const int port,
    const std::string& user_agent = "Vutils",
    const std::string& server_user = "",
    const std::string& server_pass = "",
    const std::string& proxy_user  = "",
    const std::string& proxy_pass  = ""
  );

  virtual ~RESTClientA();

  bool get(const std::string& endpoint, http_response& response,
    const std::string& header = "");
  bool del(const std::string& endpoint, http_response& response,
    const std::string& header = "");
  bool put(const std::string& endpoint, http_response& response,
    const std::string& body, const std::string& header = "");
  bool post(const std::string& endpoint, http_response& response,
    const std::string& body, const std::string& header = "");

private:
  RESTClientW* m_ptr_impl;
};

// RESTClientW

class RESTClientW
{
public:
  RESTClientW(
    const protocol_scheme scheme,
    const std::wstring& host,
    const int port,
    const std::wstring& user_agent  = L"Vutils",
    const std::wstring& server_user = L"",
    const std::wstring& server_pass = L"",
    const std::wstring& proxy_user  = L"",
    const std::wstring& proxy_pass  = L""
  );

  virtual ~RESTClientW();

  bool get(const std::wstring& endpoint, http_response& response,
    const std::wstring& header = L"");
  bool del(const std::wstring& endpoint, http_response& response,
    const std::wstring& header = L"");
  bool put(const std::wstring& endpoint, http_response& response,
    const std::string& body, const std::wstring& header = L"");
  bool post(const std::wstring& endpoint, http_response& response,
    const std::string& body, const std::wstring& header = L"");

private:
  HttpRequest* m_ptr_impl;
};

#endif // VU_INET_ENABLED

/**
 * Debouncer
 */

class Debouncer : public SingletonT<Debouncer>
{
public:
  struct Timer
  {
    ulongptr m_handle;
    std::function<void()> m_function;
    Timer() : m_handle(0), m_function(nullptr) {}
  };

  void debounce(ulongptr id, ulong elapse, std::function<void()> fn);
  bool exists(ulongptr id);
  void remove(ulongptr id);
  void cleanup();

private:
  static ulongptr m_using_timer_id;
  static std::unordered_map<ulongptr, Timer*> m_timers;
  static VOID CALLBACK fn_timer_proc(HWND hWnd, UINT message, UINT_PTR idTimer, DWORD dwTime);
};

/*------------ The definition of common Class(es) which compatible both ANSI & UNICODE -----------*/

#ifdef _UNICODE
#define UIDGlobal GUIDW
#define INLHooking INLHookingW
#define IATHooking IATHookingW
#define WMHooking WMHookingW
#define Process ProcessW
#define ServiceManager ServiceManagerW
#define Library LibraryW
#define FileSystem FileSystemW
#define FileMapping FileMappingW
#define INIFile INIFileW
#define Registry RegistryW
#define PEFileT PEFileTW
#define Path PathW
#define ScopeStopWatch ScopeStopWatchW
#define WMIProvider WMIProviderW
#define Variant VariantW
#define Picker PickerW
#define RESTClient RESTClientW
#else // _UNICODE
#define UIDGlobal GUIDA
#define INLHooking INLHookingA
#define IATHooking IATHookingA
#define WMHooking WMHookingA
#define Process ProcessA
#define ServiceManager ServiceManagerA
#define Library LibraryA
#define FileSystem FileSystemA
#define FileMapping FileMappingA
#define INIFile INIFileA
#define Registry RegistryA
#define PEFileT PEFileTA
#define Path PathA
#define ScopeStopWatch ScopeStopWatchA
#define WMIProvider WMIProviderA
#define Variant VariantA
#define Picker PickerA
#define RESTClient RESTClientA
#endif // _UNICODE

} // namespace vu

#ifdef _MSC_VER
#pragma pack(pop)
#endif // _MSC_VER

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#ifndef VU_min_max_macro_defined
#undef min
#undef max
#endif // !VU_min_max_macro_defined

#endif // VUTILS_H