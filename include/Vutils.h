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
  typedef std::function<void()> fn_task_t;
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

class Buffer;

bool vuapi is_administrator();
bool set_privilege_A(const std::string&  privilege, const bool enable);
bool set_privilege_W(const std::wstring& privilege, const bool enable);
std::string vuapi  get_enviroment_A(const std::string  name);
std::wstring vuapi get_enviroment_W(const std::wstring name);
std::pair<bool, size_t> find_pattern_A(const Buffer& buffer, const std::string&  pattern);
std::pair<bool, size_t> find_pattern_W(const Buffer& buffer, const std::wstring& pattern);
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

typedef enum class _ENCODING_TYPE : int
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

typedef enum class _STD_BYTES : int
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
std::string vuapi to_hex_string_A(const byte* ptr, const size_t size);
std::wstring vuapi to_hex_string_W(const byte* ptr, const size_t size);
bool vuapi to_hex_bytes_A(const std::string& text, std::vector<byte>& bytes);
bool vuapi to_hex_bytes_W(const std::wstring& text, std::vector<byte>& bytes);

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
std::vector<std::string> vuapi split_string_A(
  const std::string& string, const std::string& seperate, bool remove_empty = false);
std::vector<std::wstring> vuapi split_string_W(
  const std::wstring& string, const std::wstring& seperate, bool remove_empty = false);
std::vector<std::string> vuapi multi_string_to_list_A(const char* ps_multi_string);
std::vector<std::wstring> vuapi multi_string_to_list_W(const wchar* ps_multi_string);
std::unique_ptr<char[]> vuapi list_to_multi_string_A(const std::vector<std::string>& strings);
std::unique_ptr<wchar[]> vuapi list_to_multi_string_W(const std::vector<std::wstring>& strings);
std::string vuapi load_rs_string_A(const uint id, const std::string& module_name = "");
std::wstring vuapi load_rs_string_W(const uint id, const std::wstring& module_name = L"");
std::string vuapi trim_string_A(
  const std::string& string,
  const eTrimType& type = eTrimType::TS_BOTH,
  const std::string& chars = " \t\n\r\f\v");
std::wstring vuapi trim_string_W(
  const std::wstring& string,
  const eTrimType& type = eTrimType::TS_BOTH,
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
HMODULE vuapi remote_get_module_handle_A(ulong pid, const std::string& module_name); // TODO: uncompleted.
HMODULE vuapi remote_get_module_handle_W(ulong pid, const std::wstring& module_name);
VUResult vuapi inject_dll_A(ulong pid, const std::string& dll_file_path, bool wait_loading = true);
VUResult vuapi inject_dll_W(ulong pid, const std::wstring& dll_file_path, bool wait_loading = true);
bool vuapi rpm(
  const HANDLE hp,
  const void* address,
  void* buffer,
  const SIZE_T size,
  const bool force = false);
bool vuapi rpm_ex(
  const eXBit bit,
  const HANDLE hp,
  const void* address,
  void* buffer,
  const SIZE_T size,
  const bool force = false,
  const SIZE_T n_offsets = 0, ...);
bool vuapi wpm(const HANDLE hp,
  const void* address,
  const void* buffer,
  const SIZE_T size,
  const bool force = false);
bool vuapi wpm_ex(const eXBit bit,
  const HANDLE hp,
  const void* address,
  const void* buffer,
  const SIZE_T size,
const bool force = false, const SIZE_T n_offsets = 0, ...);

/**
 * Window Working
 */

typedef struct _FONT_A
{
  std::string name;
  int  size;
  bool italic;
  bool under_line;
  bool strike_out;
  int  weight;
  int  char_set;
  int  orientation;
  _FONT_A()
    : name(""), size(-1)
    , italic(false), under_line(false), strike_out(false)
    , weight(0), char_set(ANSI_CHARSET), orientation(0) {}
} sFontA;

typedef struct _FONT_W
{
  std::wstring name;
  int  size;
  bool italic;
  bool under_line;
  bool strike_out;
  int  weight;
  int  char_set;
  int  orientation;
  _FONT_W()
    : name(L""), size(-1)
    , italic(false), under_line(false), strike_out(false)
    , weight(0), char_set(ANSI_CHARSET), orientation(0) {}
} sFontW;

HWND vuapi get_console_window();
HWND vuapi find_top_window(ulong pid);
HWND vuapi find_main_window(HWND hwnd);
std::string  vuapi decode_wm_A(const ulong wm);
std::wstring vuapi decode_wm_W(const ulong wm);
sFontA vuapi get_font_A(HWND hwnd);
sFontW vuapi get_font_W(HWND hwnd);
LONG vuapi conv_font_height_to_size(LONG height, HWND hwnd = nullptr);
LONG vuapi conv_font_size_to_height(LONG size, HWND hwnd = nullptr);

/**
 * File/Directory Working
 */

enum class ePathSep
{
  WIN,
  POSIX,
};

typedef enum class _DISK_TYPE : int
{
  Unspecified = 0,
  HDD = 3,
  SSD = 4,
  SCM = 5,
} eDiskType;

#if defined(VU_WMI_ENABLED)
eDiskType vuapi get_disk_type_A(const char drive);
eDiskType vuapi get_disk_type_W(const wchar_t drive);
#endif // VU_WMI_ENABLED

#if defined(_MSC_VER) || defined(__BCPLUSPLUS__) // LNK

struct sLNKA
{
  std::string path;
  std::string directory;
  std::string argument;
  std::string description;
};

struct sLNKW
{
  std::wstring path;
  std::wstring directory;
  std::wstring argument;
  std::wstring description;
};

#ifdef _UNICODE
#define sLNK sLNKW
#else
#define sLNK sLNKA
#endif

std::unique_ptr<sLNKA> parse_shortcut_lnk_A(HWND hwnd, const std::string& lnk_file_path);
std::unique_ptr<sLNKW> parse_shortcut_lnk_W(HWND hwnd, const std::wstring& lnk_file_path);

#ifdef _UNICODE
#define parse_shortcut_lnk parse_shortcut_lnk_W
#else
#define parse_shortcut_lnk parse_shortcut_lnk_A
#endif

#endif // LNK

bool vuapi is_directory_exists_A(const std::string& directory);
bool vuapi is_directory_exists_W(const std::wstring& directory);
bool vuapi is_file_exists_A(const std::string& file_path);
bool vuapi is_file_exists_W(const std::wstring& file_path);
bool vuapi read_file_binary_A(const std::string& file_path, std::vector<byte>& data);
bool vuapi read_file_binary_W(const std::wstring& file_path, std::vector<byte>& data);
bool vuapi write_file_binary_A(const std::string& file_path, const std::vector<byte>& data);
bool vuapi write_file_binary_W(const std::wstring& file_path, const std::vector<byte>& data);
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
  const std::string& left, const std::string& right, const ePathSep separator = ePathSep::WIN);
std::wstring vuapi join_path_W(
  const std::wstring& left, const std::wstring& right, const ePathSep separator = ePathSep::WIN);
std::string vuapi normalize_path_A(
  const std::string& path, const ePathSep separator = ePathSep::WIN);
std::wstring vuapi normalize_path_W(
  const std::wstring& path, const ePathSep separator = ePathSep::WIN);
std::string vuapi undecorate_cpp_symbol_A(
  const std::string& name, const ushort flags = 0);   // UNDNAME_COMPLETE
std::wstring vuapi undecorate_cpp_symbol_W(
  const std::wstring& name, const ushort flags = 0); // UNDNAME_COMPLETE

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

enum class eBits : int
{
  _8   = 8,   // CRC-8/SMBUS
  _16  = 16,  // CRC-16/ARC, CRC-16/LHA, CRC-IBM
  _32  = 32,  // CRC-32/HDLC, CRC-32/ADCCP, CRC-32/V-42, CRC-32/XZ, PKZIP
  _64  = 64,  // CRC-64/ECMA-182

  _160 = 160, // SHA-1
  _256 = 256, // SHA-256
  _512 = 512, // SHA-512

  Unspecified = -1,
};

uint64 vuapi crypt_crc_text_A(const std::string& text, const eBits bits);
uint64 vuapi crypt_crc_text_W(const std::wstring& text, const eBits bits);
uint64 vuapi crypt_crc_file_A(const std::string& file_path, const eBits bits);
uint64 vuapi crypt_crc_file_W(const std::wstring& file_path, const eBits bits);
uint64 vuapi crypt_crc_buffer(const std::vector<byte>& data, const eBits bits);

// Note: For reduce library size so only enabled 32/64-bits of parametrised CRC algorithms
uint64 vuapi crypt_crc_buffer(const std::vector<byte>& data,
  uint8_t bits, uint64 poly, uint64 init, bool ref_in, bool ref_out, uint64 xor_out, uint64 check);

// SHA

enum class eSHA
{
  _1 = 1,
  _2 = 2,
  _3 = 3,
};

std::string vuapi crypt_sha_text_A(const std::string& text, const eSHA version, const eBits bits);
std::wstring vuapi crypt_sha_text_W(const std::wstring& text, const eSHA version, const eBits bits);
std::string vuapi crypt_sha_file_A(const std::string& file_path, const eSHA version, const eBits bits);
std::wstring vuapi crypt_sha_file_W(const std::wstring& file_path, const eSHA version, const eBits bits);
void vuapi crypt_sha_buffer(const std::vector<byte>& data, const eSHA version, const eBits bits,
  std::vector<byte>& hash);

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
#define to_hex_string to_hex_string_W
#define to_hex_bytes to_hex_bytes_W
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
#define contains_string contains_string_W
#define compare_string compare_string_W
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
/* Wrapper */
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
#define to_hex_string to_hex_string_A
#define to_hex_bytes to_hex_bytes_A
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
#define contains_string contains_string_A
#define compare_string compare_string_A
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
    return get_last_error_A(m_last_error_code);
  }

  virtual std::wstring vuapi get_last_error_message_W()
  {
    return get_last_error_W(m_last_error_code);
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

class GUIDX
{
public:
  GUIDX(bool create = false);
  virtual ~GUIDX();

  const GUIDX& operator = (const GUIDX& right);
  bool operator == (const GUIDX& right) const;
  bool operator != (const GUIDX& right) const;

  bool create();

  const sGUID& GUID() const;
  void  GUID(const sGUID& guid);

protected:
  sGUID m_guid;
  VUResult m_status;
};

class GUIDA : public GUIDX
{
public:
  GUIDA(bool create = false) : GUIDX(create) {}
  virtual ~GUIDA() {}

  void parse(const std::string& guid);
  std::string as_string() const;

  static const std::string to_string(const sGUID& guid);
  static const sGUID to_guid(const std::string& guid);
};

class GUIDW : public GUIDX
{
public:
  GUIDW(bool create = false) : GUIDX(create) {}
  virtual ~GUIDW() {}

  void parse(const std::wstring& guid);
  std::wstring as_string() const;

  static const std::wstring to_string(const sGUID& guid);
  static const sGUID to_guid(const std::wstring& guid);
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
  Buffer operator()(int begin, int end) const;

  byte*  get_ptr_bytes() const;
  void*  get_ptr() const;
  size_t get_size() const;

  bool empty() const;

  void reset();
  void fill(const byte v = 0);
  bool resize(const size_t size);
  bool replace(const void* ptr, const size_t size);
  bool replace(const Buffer& right);
  bool match(const void* ptr, const size_t size) const;
  size_t find(const void* ptr, const size_t size) const;
  Buffer till(const void* ptr, const size_t size) const;
  Buffer slice(int begin, int end) const;

  bool append(const void* ptr, const size_t size);
  bool append(const Buffer& right);

  std::string  to_string_A() const;
  std::wstring to_string_W() const;

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

#ifdef VU_SOCKET_ENABLED

#ifndef MSG_NONE
#define MSG_NONE 0
#endif // MSG_NONE

class Socket : public LastError
{
public:
  typedef int address_family_t;
  typedef int type_t;
  typedef int protocol_t;
  typedef int flags_t;
  typedef int shutdowns_t;

  struct sSocket
  {
    SOCKET s;
    sockaddr_in sai;
    char ip[15];
  };

  struct sEndPoint
  {
    std::string host;
    ushort port;

    sEndPoint(const std::string& host, const ushort port) : host(host), port(port) {}
  };

public:
  Socket(
    const address_family_t af = AF_INET,
    const type_t type = SOCK_STREAM,
    const protocol_t proto = IPPROTO_IP,
    bool  wsa = true);
  virtual ~Socket();

  bool vuapi available();

  void vuapi attach(const SOCKET&  socket);
  void vuapi attach(const sSocket& socket);
  void vuapi detach();

  VUResult vuapi bind(const sEndPoint& endpoint);
  VUResult vuapi bind(const std::string& address, const ushort port);

  VUResult vuapi listen(const int maxcon = SOMAXCONN);
  VUResult vuapi accept(sSocket& socket);

  VUResult vuapi connect(const sEndPoint& endpoint);
  VUResult vuapi connect(const std::string& address, const ushort port);

  IResult vuapi send(const char* pData, int size, const flags_t flags = MSG_NONE);
  IResult vuapi send(const Buffer& data, const flags_t flags = MSG_NONE);

  IResult vuapi recv(char* pData, int size, const flags_t flags = MSG_NONE);
  IResult vuapi recv(Buffer& data, const flags_t flags = MSG_NONE);
  IResult vuapi recv_all(Buffer& data, const flags_t flags = MSG_NONE);

  IResult vuapi send_to(const char* pData, const int size, const sSocket& socket);
  IResult vuapi send_to(const Buffer& data, const sSocket& socket);

  IResult vuapi recv_from(char* pData, int size, const sSocket& socket);
  IResult vuapi recv_from(Buffer& data, const sSocket& socket);
  IResult vuapi recv_all_from(Buffer& data, const sSocket& socket);

  IResult vuapi close();

  const WSADATA& vuapi get_wsa_data() const;
  const address_family_t vuapi get_af() const;
  const type_t vuapi get_type() const;
  const protocol_t vuapi  get_protocol() const;

  SOCKET& vuapi get_socket();
  const sockaddr_in vuapi get_local_sai();
  const sockaddr_in vuapi get_remote_sai();
  std::string vuapi get_host_name();

  VUResult vuapi set_option(const int level, const int opt, const std::string& val, const int size);
  VUResult vuapi enable_non_blocking(bool state = true);
  VUResult vuapi shutdown(const shutdowns_t flags);

private:
  bool vuapi valid(const SOCKET& socket);
  bool vuapi parse(const sSocket& socket);
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
};

class AsyncSocket
{
public:
  typedef std::function<void(Socket& client)> fn_prototype_t;

  typedef enum _FN_TYPE : uint
  {
    OPEN,
    CLOSE,
    RECV,
    SEND,
    UNDEFINED,
  } eFnType;

  AsyncSocket(
    const vu::Socket::address_family_t af = AF_INET,
    const vu::Socket::type_t type = SOCK_STREAM,
    const vu::Socket::protocol_t proto = IPPROTO_IP);
  virtual ~AsyncSocket();

  std::set<SOCKET> vuapi get_clients();

  bool vuapi available();
  bool vuapi running();

  VUResult vuapi bind(const Socket::sEndPoint& endpoint);
  VUResult vuapi bind(const std::string& address, const ushort port);
  VUResult vuapi listen(const int maxcon = SOMAXCONN);
  VUResult vuapi run();
  VUResult vuapi stop();
  IResult  vuapi close();

  virtual void on(const eFnType type, const fn_prototype_t fn); // must be mapping before call Run(...)

  virtual void on_open(Socket&  client);
  virtual void on_close(Socket& client);
  virtual void on_send(Socket&  client);
  virtual void on_recv(Socket&  client);

protected:
  void vuapi initialze();
  VUResult vuapi loop();

  IResult vuapi do_open(WSANETWORKEVENTS&  events, SOCKET& socket);
  IResult vuapi do_recv(WSANETWORKEVENTS&  events, SOCKET& socket);
  IResult vuapi do_send(WSANETWORKEVENTS&  events, SOCKET& socket);
  IResult vuapi do_close(WSANETWORKEVENTS& events, SOCKET& socket);

protected:
  vu::Socket m_server;
  bool m_running;
  DWORD m_n_events;
  SOCKET m_sockets[WSA_MAXIMUM_WAIT_EVENTS];
  WSAEVENT m_events[WSA_MAXIMUM_WAIT_EVENTS];
  fn_prototype_t m_functions[eFnType::UNDEFINED];
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

typedef enum class _MEMORY_ADDRESS_TYPE
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
} sMemoryInstruction;

class INLHookingX
{
protected:
  typedef struct _REDIRECT
  {
    ushort   jmp;
    ulong    unknown;
    ulongptr address;
  } sRedirect;

  bool m_hooked;
  std::vector<sMemoryInstruction> m_memory_instructions;

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

struct sIATElement;

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
  enum IATAction
  {
    IAT_INSTALL,
    IAT_UNINSTALL,
  };

  typedef std::vector<sIATElement> IATElements;

  IATElements m_iat_elements;

private:
  IATElements::iterator find(const sIATElement& element);

  IATElements::iterator find(
    const std::string& target,
    const std::string& module,
    const std::string& function);

  bool exist(
    const std::string& target,
    const std::string& module,
    const std::string& function);

  VUResult perform(const IATAction action, sIATElement& element);
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
  FANOTCONTENTINDEXED = 0x00002000,   // FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  = $00002000;
  FAENCRYPTED         = 0x00004000,   // FILE_ATTRIBUTE_ENCRYPTED            = $00004000;
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
} sFSObjectA;

typedef struct _FS_OBJECT_W
{
  std::wstring directory;
  std::wstring name;
  int64 size;
  ulong attributes;
} sFSObjectW;

class FileSystemX : public LastError
{
public:
  FileSystemX();
  virtual ~FileSystemX();

  virtual bool vuapi ready();
  virtual bool vuapi valid(HANDLE handle);
  virtual ulong vuapi get_file_size();

  virtual const Buffer vuapi read_as_buffer();
  virtual bool vuapi read(void* ptr_buffer, ulong size);
  virtual bool vuapi read(
    ulong offset, void* ptr_buffer, ulong size, eMoveMethodFlags flags = MM_BEGIN);

  virtual bool vuapi write(const void* ptr_buffer, ulong size);
  virtual bool vuapi write(
    ulong offset, const void* ptr_buffer, ulong size, eMoveMethodFlags flags = MM_BEGIN);

  virtual bool vuapi seek(ulong offset, eMoveMethodFlags flags);
  virtual bool vuapi io_control(
    ulong code, void* ptr_send_buffer, ulong send_size, void* ptr_recv_buffer, ulong recv_size);

  virtual bool vuapi close();

protected:
  HANDLE m_handle;

private:
  ulong m_read_size, m_wrote_size;
};

class FileSystemA: public FileSystemX
{
public:
  FileSystemA();
  FileSystemA(
    const std::string& file_path,
    eFSModeFlags fm_flags,
    eFSGenericFlags fg_flags = FG_READWRITE,
    eFSShareFlags fs_flags = FS_ALLACCESS, eFSAttributeFlags fa_flags = FA_NORMAL);
  virtual ~FileSystemA();

  bool vuapi initialize(const std::string& file_path,
    eFSModeFlags fm_flags,
    eFSGenericFlags fg_flags = FG_READWRITE,
    eFSShareFlags fs_flags = FS_ALLACCESS,
    eFSAttributeFlags fa_flags = FA_NORMAL);
  const std::string vuapi read_as_string(bool remove_bom = true);

  static const std::string vuapi quick_read_as_string(
    const std::string& file_path, bool force_bom = true);
  static Buffer quick_read_as_buffer(const std::string& file_path);
  static bool iterate(
    const std::string& path,
    const std::string& pattern,
    const std::function<bool(const sFSObjectA& fso)> fn_callback);
};

class FileSystemW: public FileSystemX
{
public:
  FileSystemW();
  FileSystemW(
    const std::wstring& file_path,
    eFSModeFlags fm_flags,
    eFSGenericFlags fg_flags = FG_READWRITE,
    eFSShareFlags fs_flags = FS_ALLACCESS,
    eFSAttributeFlags fa_flags = FA_NORMAL);
  virtual ~FileSystemW();

  bool vuapi initialize(
    const std::wstring& file_path,
    eFSModeFlags fm_flags,
    eFSGenericFlags fg_flags = FG_READWRITE,
    eFSShareFlags fs_flags = FS_ALLACCESS,
    eFSAttributeFlags fa_flags = FA_NORMAL);
  const std::wstring vuapi read_as_string(bool remove_bom = true);

  static const std::wstring vuapi quick_read_as_string(
    const std::wstring& file_path, bool remove_bom = true);
  static Buffer vuapi quick_read_as_buffer(const std::wstring& file_path);
  static bool vuapi iterate(
    const std::wstring& path,
    const std::wstring& pattern,
    const std::function<bool(const sFSObjectW& fso)> fn_callback);
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

class FileMappingX : public LastError
{
public:
  FileMappingX();
  virtual ~FileMappingX();

  void* vuapi view(
    eFMDesiredAccess fmDesiredAccess = eFMDesiredAccess::DA_ALL_ACCESS,
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
    eFSGenericFlags fg_flags = eFSGenericFlags::FG_ALL,
    eFSShareFlags fs_flags = eFSShareFlags::FS_ALLACCESS,
    eFSModeFlags fm_flags = eFSModeFlags::FM_OPENEXISTING,
    eFSAttributeFlags fa_flags = eFSAttributeFlags::FA_NORMAL,
    ePageProtection pp_flags = ePageProtection::PP_EXECUTE_READ_WRITE
  );

  /**
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi create_named_shared_memory(
    const std::string& mapping_name,
    ulong max_size_low, // The mapping size for file mapping object.
    ulong max_size_high = 0,
    ePageProtection pp_flags = ePageProtection::PP_EXECUTE_READ_WRITE
  );

  /**
   * Just for Named Shared Memory.
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi open(
    const std::string& mapping_name,
    eFMDesiredAccess desired_access = eFMDesiredAccess::DA_ALL_ACCESS,
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
    eFSGenericFlags fg_flags = eFSGenericFlags::FG_ALL,
    eFSShareFlags fs_flags = eFSShareFlags::FS_ALLACCESS,
    eFSModeFlags fm_flags = eFSModeFlags::FM_OPENEXISTING,
    eFSAttributeFlags fa_flags = eFSAttributeFlags::FA_NORMAL,
    ePageProtection pp_flags = ePageProtection::PP_EXECUTE_READ_WRITE
  );

  /**
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi create_named_shared_memory(
    const std::wstring& mapping_name,
    ulong max_size_low, // The mapping size for file mapping object.
    ulong max_size_high = 0,
    ePageProtection pp_flags = ePageProtection::PP_EXECUTE_READ_WRITE
  );

  /**
   * Just for Named Shared Memory.
   * The mapping name object, eg. Global\\FileMappingObject, Local\\FileMappingObject, etc.
   * More detail at https://docs.microsoft.com/en-us/windows/win32/termserv/kernel-object-namespaces
   */
  VUResult vuapi open(
    const std::wstring& mapping_name,
    eFMDesiredAccess desired_access = eFMDesiredAccess::DA_ALL_ACCESS,
    bool inherit_handle = false
  );
};

/**
 * INI File
 */

class INIFileA : public LastError
{
public:
  INIFileA();
  INIFileA(const std::string& file_path);
  virtual ~INIFileA();

  void set_current_file_path(const std::string& file_path);
  void set_current_section(const std::string& section);

  std::vector<std::string> vuapi read_section(const std::string& section, ulong max_size = MAXBYTE);
  std::vector<std::string> vuapi read_current_section(ulong max_size = MAXBYTE);

  std::vector<std::string> vuapi read_section_names(ulong max_size = MAXBYTE);

  int vuapi read_integer(const std::string& section, const std::string& key, int default_value);
  bool vuapi read_bool(const std::string& section, const std::string& key, bool default_value);
  float vuapi read_float(const std::string& section, const std::string& key, float default_value);
  std::string vuapi read_string(
    const std::string& section, const std::string& key, const std::string& default_value);
  std::unique_ptr<uchar[]> vuapi read_struct(
    const std::string& section, const std::string& key, ulong size);

  int vuapi read_integer(const std::string& key, int default_value);
  bool vuapi read_bool(const std::string& key, bool default_value);
  float vuapi read_float(const std::string& key, float default_value);
  std::string vuapi read_string(const std::string& key, const std::string& default_value);
  std::unique_ptr<uchar[]> vuapi read_struct(const std::string& key, ulong size);

  bool vuapi write_integer(const std::string& section, const std::string& key, int value);
  bool vuapi write_bool(const std::string& section, const std::string& key, bool value);
  bool vuapi write_float(const std::string& section, const std::string& key, float value);
  bool vuapi write_string(
    const std::string& section, const std::string& key, const std::string& Value);
  bool vuapi write_struct(
    const std::string& section, const std::string& key, void* ptr_struct, ulong size);

  bool vuapi write_integer(const std::string& key, int value);
  bool vuapi write_bool(const std::string& key, bool value);
  bool vuapi write_float(const std::string& key, float value);
  bool vuapi write_string(const std::string& key, const std::string& value);
  bool vuapi write_struct(const std::string& key, void* ptr_struct, ulong size);

private:
  void update_file_path();

private:
  std::string m_file_path;
  std::string m_section;
};

class INIFileW : public LastError
{
public:
  INIFileW();
  INIFileW(const std::wstring& file_path);
  virtual ~INIFileW();

  void set_current_file_path(const std::wstring& file_path);
  void set_current_section(const std::wstring& section);

  std::vector<std::wstring> vuapi read_section(const std::wstring& section, ulong max_size = MAXBYTE);
  std::vector<std::wstring> vuapi read_current_section(ulong max_size = MAXBYTE);

  std::vector<std::wstring> vuapi read_section_names(ulong max_size = MAXBYTE);

  int vuapi read_integer(const std::wstring& section, const std::wstring& key, int default_value);
  bool vuapi read_bool(const std::wstring& section, const std::wstring& key, bool default_value);
  float vuapi read_float(const std::wstring& section, const std::wstring& key, float default_value);
  std::wstring vuapi read_string(
    const std::wstring& section, const std::wstring& key, const std::wstring& default_value);
  std::unique_ptr<uchar[]> vuapi read_struct(
    const std::wstring& section, const std::wstring& key, ulong size);

  int vuapi read_integer(const std::wstring& key, int default_value);
  bool vuapi read_bool(const std::wstring& key, bool default_value);
  float vuapi read_float(const std::wstring& key, float default_value);
  std::wstring vuapi read_string(const std::wstring& key, const std::wstring& default_value);
  std::unique_ptr<uchar[]> vuapi read_struct(const std::wstring& key, ulong size);

  bool vuapi write_integer(const std::wstring& section, const std::wstring& key, int value);
  bool vuapi write_bool(const std::wstring& section, const std::wstring& key, bool value);
  bool vuapi write_float(const std::wstring& section, const std::wstring& key, float value);
  bool vuapi write_string(
    const std::wstring& section, const std::wstring& key, const std::wstring& value);
  bool vuapi write_struct(
    const std::wstring& section, const std::wstring& key, void* ptr_struct, ulong size);

  bool vuapi write_integer(const std::wstring& key, int value);
  bool vuapi write_bool(const std::wstring& key, bool value);
  bool vuapi write_float(const std::wstring& key, float value);
  bool vuapi write_string(const std::wstring& key, const std::wstring& value);
  bool vuapi write_struct(const std::wstring& key, void* ptr_struct, ulong size);

private:
  void update_file_path();

private:
  std::wstring m_file_path;
  std::wstring m_section;
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

typedef enum class _HKEY : ulongptr
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

typedef enum class _REG_ACCESS
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

typedef enum class _REG_REFLECTION
{
  RR_ERROR    = -1,
  RR_DISABLED = 0,
  RR_ENABLED  = 1,
  RR_DISABLE  = 2,
  RR_ENABLE   = 3,
} eRegReflection;

class RegistryX : public LastError
{
public:
  RegistryX();
  virtual ~RegistryX();

  HKEY vuapi get_current_key_handle();
  eRegReflection vuapi query_reflection_key();
  bool vuapi set_reflection_key(eRegReflection reg_reflection);
  bool vuapi close_key();

protected:
  HKEY m_hk_root_key;
  HKEY m_hk_sub_key;
};

class RegistryA : public RegistryX
{
public:
  RegistryA();
  RegistryA(eRegRoot reg_root);
  RegistryA(eRegRoot reg_root, const std::string& sub_key);
  virtual ~RegistryA();

  ulong vuapi set_size_of_multi_string(const char* multi_string);
  ulong vuapi get_data_size(const std::string& value_name, ulong type);

  bool vuapi create_key();
  bool vuapi create_key(const std::string& sub_key);
  bool vuapi key_exists();
  bool vuapi key_exists(const std::string& sub_key);
  bool vuapi open_key(eRegAccess reg_access = eRegAccess::RA_ALL_ACCESS);
  bool vuapi open_key(const std::string& sub_key, eRegAccess reg_access = eRegAccess::RA_ALL_ACCESS);
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
  RegistryW(eRegRoot reg_root);
  RegistryW(eRegRoot reg_root, const std::wstring& sub_key);
  virtual ~RegistryW();

  ulong vuapi set_size_of_multi_string(const wchar* multi_string);
  ulong vuapi get_data_size(const std::wstring& value_name, ulong type);

  bool vuapi create_key();
  bool vuapi create_key(const std::wstring& sub_key);
  bool vuapi key_exists();
  bool vuapi key_exists(const std::wstring& sub_key);
  bool vuapi open_key(eRegAccess reg_access = eRegAccess::RA_ALL_ACCESS);
  bool vuapi open_key(const std::wstring& sub_key, eRegAccess reg_access = eRegAccess::RA_ALL_ACCESS);
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
 * CriticalSection
 */

typedef CRITICAL_SECTION    TCriticalSection, *PCriticalSection;

class CriticalSection
{
public:
  CriticalSection();
  virtual ~CriticalSection();

  void vuapi initialize();
  void vuapi enter();
  void vuapi leave();
  void vuapi destroy();

private:
  TCriticalSection m_cs;
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

  void active(bool state = true);
  void reset();

protected:
  virtual void start(bool reset = false);
  virtual void stop();

protected:
  bool m_activated;
  StopWatch m_watcher;
};

class ScopeStopWatchA : public ScopeStopWatchX
{
public:
  typedef std::function<void(const std::string& id, const StopWatch::TDuration& duration)> FnLogging;

  ScopeStopWatchA(const std::string& prefix, const FnLogging fn_logging = message);
  virtual ~ScopeStopWatchA();

  void log(const std::string& id = "");

  static void message(const std::string& id, const StopWatch::TDuration& duration);
  static void console(const std::string& id, const StopWatch::TDuration& duration);

private:
  std::string m_prefix;
  FnLogging m_fn_logging;
};

class ScopeStopWatchW : public ScopeStopWatchX
{
public:
  typedef std::function<void(const std::wstring& id, const StopWatch::TDuration& duration)> FnLogging;

  ScopeStopWatchW(const std::wstring& prefix, const FnLogging fn_logging = message);
  virtual ~ScopeStopWatchW();

  void log(const std::wstring& id = L"");

  static void message(const std::wstring& id, const StopWatch::TDuration& duration);
  static void console(const std::wstring& id, const StopWatch::TDuration& duration);

private:
  std::wstring m_prefix;
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

struct sExIID
{
  ulong iid_id;
  std::string name;
  PImportDescriptor ptr_iid;
};

struct sImportModule
{
  ulong iid_id;
  std::string name;
  // ulong number_of_functions;
};

template<typename T>
struct sImportFunctionT
{
  ulong iid_id;
  std::string name;
  T ordinal;
  ushort hint;
  T rva;
};

typedef sImportFunctionT<ulong32> sImportFunction32T;
typedef sImportFunctionT<ulong64> sImportFunction64T;

typedef enum class _IMPORTED_FUNCTION_FIND_BY
{
  IFFM_HINT,
  IFFM_NAME,
} eImportedFunctionFindMethod;

template<typename T>
struct sRelocationEntryT
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
  PEFileTX();
  virtual ~PEFileTX();

  void* vuapi get_ptr_base();
  TPEHeaderT<T>* vuapi get_ptr_pe_header();

  T vuapi rva_to_offset(T RVA, bool in_cache = true);
  T vuapi offset_to_rva(T Offset, bool in_cache = true);

  const std::vector<PSectionHeader>& vuapi get_setion_headers(bool in_cache = true);

  const std::vector<PImportDescriptor>& vuapi get_import_descriptors(bool in_cache = true);
  const std::vector<sImportModule> vuapi get_import_modules(bool in_cache = true);
  const std::vector<sImportFunctionT<T>> vuapi get_import_functions(bool in_cache = true); // Did not include import by index

  const sImportModule* vuapi find_ptr_import_module(
    const std::string& module_name, bool in_cache = true);

  const sImportFunctionT<T>* vuapi find_ptr_import_function(
    const std::string& function_name, bool in_cache = true);
  const sImportFunctionT<T>* vuapi find_ptr_import_function(
    const ushort function_hint, bool in_cache = true);
  const sImportFunctionT<T>* vuapi find_ptr_import_function(
    const sImportFunctionT<T>& import_function,
    const eImportedFunctionFindMethod method,
    bool in_cache = true);

  const std::vector<sRelocationEntryT<T>> vuapi get_relocation_entries(bool in_cache = true);

protected:
  bool m_initialized;

  void* m_ptr_base;

  TDOSHeader* m_ptr_dos_header;
  TPEHeaderT<T>* m_ptr_pe_header;

private:
  T m_ordinal_flag;

  std::vector<sExIID> m_ex_iids;

  std::vector<PSectionHeader> m_section_headers;
  std::vector<PImportDescriptor> m_import_descriptors;
  std::vector<sImportModule> m_import_modules;
  std::vector<sImportFunctionT<T>> m_import_functions;
  std::vector<sRelocationEntryT<T>> m_relocation_entries;

protected:
  const std::vector<sExIID>& vuapi get_ex_iids(bool in_cache = true);
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
 * Fundamental
 */

class FundamentalA
{
public:
  FundamentalA();
  virtual ~FundamentalA();

  template<typename T>
  friend FundamentalA& operator<<(FundamentalA& stream, T v)
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

class FundamentalW
{
public:
  FundamentalW();
  virtual ~FundamentalW();

  template<typename T>
  friend FundamentalW& operator<<(FundamentalW& stream, T v)
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
 * WDTControl
 */

class WDTControl
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

  WDTControl(
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

  InputDialog(const std::wstring& label, const HWND hwnd_parent = nullptr, bool number_only = false);
  virtual ~InputDialog();

  void label(const std::wstring& label);
  const std::wstring& label() const;
  FundamentalW& input();

  INT_PTR do_modal();

  static LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

private:
  std::wstring m_label;
  FundamentalW m_input;
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

class ProcessX : public LastError
{
public:
  typedef std::vector<THREADENTRY32> threads;
  typedef std::vector<MEMORY_BASIC_INFORMATION> memories;

  ProcessX();
  // ProcessX(const ulong pid);
  virtual ~ProcessX();

  // ProcessX(ProcessX& right);
  // ProcessX& operator=(ProcessX& right);
  bool operator==(ProcessX& right);
  bool operator!=(ProcessX& right);

  const ulong  pid() const;
  const HANDLE handle() const;
  const eWow64 wow64() const;
  const eXBit  bit() const;

  bool ready();

  bool attach(const ulong pid);
  bool attach(const HANDLE hp);

  bool read_memory(const ulongptr address, Buffer& buffer);
  bool read_memory(const ulongptr address, void* ptr_data, const size_t size);
  bool write_memory(const ulongptr address, const Buffer& buffer);
  bool write_memory(const ulongptr address, const void* ptr_data, const size_t size);

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

class ProcessA : public ProcessX
{
public:
  typedef std::vector<MODULEENTRY32> modules;

  ProcessA();
  // ProcessA(const ulong pid);
  virtual ~ProcessA();

  // ProcessA(ProcessA& right);
  // ProcessA& operator=(ProcessA& right);
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

  // ProcessW(ProcessW& right);
  // ProcessW& operator=(ProcessW& right);
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
  PathA(const ePathSep separator = ePathSep::WIN);
  PathA(const std::string& path, const ePathSep separator = ePathSep::WIN);
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

  PathA& trim(const eTrimType type = eTrimType::TS_BOTH);
  PathA& normalize();
  PathA& join(const std::string& path);
  PathA& finalize();
  bool exists() const;
  std::string as_string() const;

  PathA extract_name(bool with_extension = true) const;
  PathA extract_directory(bool with_slash = true) const;

private:
  ePathSep m_separator;
  std::string m_path;
};

class PathW
{
public:
  PathW(const ePathSep separator = ePathSep::WIN);
  PathW(const std::wstring& path, const ePathSep separator = ePathSep::WIN);
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

  PathW& trim(const eTrimType type = eTrimType::TS_BOTH);
  PathW& normalize();
  PathW& join(const std::wstring& path);
  PathW& finalize();
  bool exists() const;
  std::wstring as_string() const;

  PathW extract_name(bool with_extension = true) const;
  PathW extract_directory(bool with_slash = true) const;

private:
  ePathSep m_separator;
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

  enum class action_t
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
    const action_t action,
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
    const action_t action,
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

/*---------- The definition of common structure(s) which compatible both ANSI & UNICODE ----------*/

#ifdef _UNICODE
#define sFSObject sFSObjectW
#else
#define sFSObject sFSObjectA
#endif

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
#define Fundamental FundamentalW
#define Picker PickerW
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
#define Fundamental FundamentalA
#define Picker PickerA
#endif // _UNICODE

} // namespace vu

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // VUTILS_H