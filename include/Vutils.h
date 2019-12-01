/**********************************************************/
/*  Name:     Vic Utilities aka Vutils                    */
/*  Version:  1.0                                         */
/*  Platform: Windows 32-bit & 64-bit                     */
/*  Type:     C++ Library for MSVC++/C++MinGW/C++Builder  */
/*  Author:   Vic P. aka vic4key                          */
/*  Mail:     vic4key[at]gmail.com                        */
/*  Blog:     http://viclab.biz/                          */
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

#define VU_VERSION  0x01000000 // Version 1.00.000

/* The conditions of Vutils */

#if !defined(_WIN32) && !defined(_WIN64)
#error Vutils is only available for Windows (32-bit and 64-bit) platform.
#endif // !defined(_WIN32) && !defined(_WIN64)

#ifndef __cplusplus
#error Vutils is only supported C++ compiler.
#endif // __cplusplus

/* Vutils configurations */

// VU_SOCKET_ENABLED
// VU_GUID_ENABLED

// The belows are default enabled for only MSVC and C++ Builder. For MinGW, see detail at README.md :
// - The Socket(class CSocket)
// - The Globally/Universally Unique Identifier aka GUID (class CGUID)

#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#define VU_SOCKET_ENABLED
#define VU_GUID_ENABLED
#endif

/* Macros for Header Inclusion of Third Party Libraries */

#define VU_STRINGIZE_EX(s) #s
#define VU_STRINGIZE(s) VU_STRINGIZE_EX(s)
#define VU_3RD_INCL(s)  VU_STRINGIZE(../3rdparty/s)

/* Header Inclusions */

#include <Windows.h>
#include <winsvc.h>
#include <ctime>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <functional>

#ifdef WIN32_LEAN_AND_MEAN
#include <winsock.h>
#endif // WIN32_LEAN_AND_MEAN

/* Vutils options */

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

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#pragma comment(lib, "ws2_32")
#endif // _MSC_VER

/* ------------------------------------------ Hacker Disassembler Engine ------------------------------------------- */

// Hacker Disassembler Engine 32/64 C
// Copyright (c) 2008-2009, Vyacheslav Patkov
// Modified the HDE source code to make it compatible with modern C++ and usable both 32-bit & 64-bit version together

#include VU_3RD_INCL(HDE/include/hde32.h)
#include VU_3RD_INCL(HDE/include/table32.h)
#include VU_3RD_INCL(HDE/include/hde64.h)
#include VU_3RD_INCL(HDE/include/table64.h)

#if defined(_M_X64) || defined(__x86_64__)
  namespace HDE = HDE64;
#else
  namespace HDE = HDE32;
#endif

/* --------------------------------------------- Vutils Declarations ----------------------------------------------- */

/* TCHAR equivalents of STL string and stream classes */

namespace std
{
#ifdef _UNICODE
#define tcerr           wcerr
#define tcin            wcin
#define tclog           wclog
#define tcout           wcout

typedef wstring         tstring;

typedef wstringbuf      tstringbuf;
typedef wstringstream   tstringstream;
typedef wostringstream  tostringstream;
typedef wistringstream  tistringstream;

typedef wstreambuf      tstreambuf;

typedef wistream        tistream;
typedef wostream        tostream;
typedef wiostream       tiostream;

typedef wfilebuf        tfilebuf;
typedef wfstream        tfstream;
typedef wifstream       tifstream;
typedef wofstream       tofstream;

typedef wios            tios;
#else // !_UNICODE
#define tcerr           cerr
#define tcin            cin
#define tclog           clog
#define tcout           cout

typedef string          tstring;

typedef stringbuf       tstringbuf;
typedef stringstream    tstringstream;
typedef ostringstream   tostringstream;
typedef istringstream   tistringstream;

typedef streambuf       tstreambuf;

typedef istream         tistream;
typedef ostream         tostream;
typedef iostream        tiostream;

typedef filebuf         tfilebuf;
typedef fstream         tfstream;
typedef ifstream        tifstream;
typedef ofstream        tofstream;

typedef ios             tios;
#endif // _UNICODE
} // namespace std

namespace vu
{

/* Vutils Definitions */

#ifdef _WIN64
#define vuapi __stdcall
#else  // _WIN32
#define vuapi
#endif // _WIN64

#ifndef _T
#ifdef _UNICODE
#define _T(x) L ## x
#else  // !_UNICODE
#define _T(x) x
#endif // _UNICODE
#endif // _T

#ifndef MAXPATH
#define MAXPATH MAX_PATH
#endif

#define MAX_NPROCESSES 512
#define MAX_NMODULES   1024

#define KiB     1024
#define MiB     KiB*KiB
#define GiB     MiB*KiB

#define KB      1000
#define MB      KB*KB
#define GB      MB*KB

/* Other Defination */

#ifndef ERROR_INCORRECT_SIZE
#define ERROR_INCORRECT_SIZE 1462L
#endif

// MSVC
#ifdef _MSC_VER
#define VU_FUNCTION_INFO __FUNCSIG__
#define VU_FUNCTION_NAME __FUNCTION__
#endif
// C++ Builder
#ifdef __BCPLUSPLUS__
#define VU_FUNCTION_INFO __FUNC__
#define VU_FUNCTION_NAME __FUNC__
#endif
// MingGW
#ifdef __MINGW32__
#define VU_FUNCTION_INFO __PRETTY_FUNCTION__
#define VU_FUNCTION_NAME __FUNCTION__
#endif

/* Vutils Types */

typedef int                 VUResult;
typedef int                 IResult;

typedef TCHAR               tchar;
typedef wchar_t             wchar;

// 32-bit Default Data Types

typedef unsigned char       uchar;
typedef unsigned short      ushort;
typedef unsigned short      ushort;
typedef unsigned int        uint;
typedef unsigned long       ulong;
typedef unsigned long long  ulonglong;

// 32-bit Data Types

typedef short               short32;
typedef unsigned short      ushort32;
typedef int                 int32;
typedef unsigned int        uint32;
typedef int                 long32;
typedef unsigned int        ulong32;

// 64-bit Data Types

typedef __int32             short64;
typedef unsigned __int32    ushort64;
typedef __int64             int64;
typedef unsigned __int64    uint64;
typedef __int64             long64;
typedef unsigned __int64    ulong64;

// 86-64 Data Types

typedef HALF_PTR            halfptr;
typedef UHALF_PTR           uhalfptr;
typedef INT_PTR             intptr;
typedef UINT_PTR            uintptr;
typedef LONG_PTR            longptr;
typedef ULONG_PTR           ulongptr;

typedef unsigned char       UChar;
typedef HALF_PTR            HalfPtr;
typedef UHALF_PTR           UHalfPtr;
typedef INT_PTR             IntPtr;
typedef UINT_PTR            UIntPtr;
typedef LONG_PTR            LongPtr;
typedef ULONG_PTR           ULongPtr;

typedef unsigned int        pe32;
typedef unsigned __int64    pe64;

typedef CRITICAL_SECTION    TCriticalSection, *PCriticalSection;

// PE File - Data Type

const ulong MAX_IDD = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;

typedef IMAGE_DOS_HEADER TDosHeader, *PDosHeader;

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
  TDataDirectory DataDirectory[MAX_IDD];
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
  TDataDirectory DataDirectory[MAX_IDD];
};

/*template <typename T>
typedef TOptHeaderT<T> *POptHeaderT;*/

typedef TOptHeaderT<ulong32> TOptHeader32, *POptHeader32;
typedef TOptHeaderT<ulong64> TOptHeader64, *POptHeader64;

// IMAGE_NT_HEADERS

template <typename T>
struct TNtHeaderT
{
  ulong Signature;
  TFileHeader FileHeader;
  TOptHeaderT<T> OptionalHeader;
};

typedef TNtHeaderT<ulong32> TNtHeader32, *PNtHeader32;
typedef TNtHeaderT<ulong64> TNtHeader64, *PNtHeader64;

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

// PE_HEADER

template <typename T>
struct TPEHeaderT
{
  // IMAGE_NT_HEADERS
  ulong  Signature;
  // IMAGE_FILE_HEADER
  ushort Machine;
  ushort NumberOfSections;
  ulong  TimeDateStamp;
  ulong  PointerToSymbolTable;
  ulong  NumberOfSymbols;
  ushort SizeOfOptionalHeader;
  ushort Characteristics;
  // IMAGE_OPTIONAL_HEADER
  ushort Magic;
  uchar  MajorLinkerVersion;
  uchar  MinorLinkerVersion;
  ulong  SizeOfCode;
  ulong  SizeOfInitializedData;
  ulong  SizeOfUninitializedData;
  ulong  AddressOfEntryPoint;
  ulong  BaseOfCode;
  ulong  BaseOfData; // Non-exist for 64-bit
  T      ImageBase;
  ulong  SectionAlignment;
  ulong  FileAlignment;
  ushort MajorOperatingSystemVersion;
  ushort MinorOperatingSystemVersion;
  ushort MajorImageVersion;
  ushort MinorImageVersion;
  ushort MajorSubsystemVersion;
  ushort MinorSubsystemVersion;
  ulong  Win32VersionValue;
  ulong  SizeOfImage;
  ulong  SizeOfHeaders;
  ulong  CheckSum;
  ushort SubSystem;
  ushort DllCharacteristics;
  T      SizeOfStackReserve;
  T      SizeOfStackCommit;
  T      SizeOfHeapReserve;
  T      SizeOfHeapCommit;
  ulong  LoaderFlags;
  ulong  NumberOfRvaAndSizes;
  // IMAGE_DATA_DIRECTORY
  TDataDirectory Export;
  TDataDirectory Import;
  TDataDirectory Resource;
  TDataDirectory Exception;
  TDataDirectory Security;
  TDataDirectory Basereloc;
  TDataDirectory Debug;
  TDataDirectory Copyright;
  TDataDirectory Architecture;
  TDataDirectory Globalptr;
  TDataDirectory TLS;
  TDataDirectory LoadConfig;
  TDataDirectory BoundImport;
  TDataDirectory IAT;
  TDataDirectory DelayImport;
  TDataDirectory ComDescriptor;
};

template<> struct TPEHeaderT<pe64>
{
  // IMAGE_NT_HEADERS
  ulong  Signature;
  // IMAGE_FILE_HEADER
  ushort Machine;
  ushort NumberOfSections;
  ulong  TimeDateStamp;
  ulong  PointerToSymbolTable;
  ulong  NumberOfSymbols;
  ushort SizeOfOptionalHeader;
  ushort Characteristics;
  // IMAGE_OPTIONAL_HEADER
  ushort Magic;
  uchar  MajorLinkerVersion;
  uchar  MinorLinkerVersion;
  ulong  SizeOfCode;
  ulong  SizeOfInitializedData;
  ulong  SizeOfUninitializedData;
  ulong  AddressOfEntryPoint;
  ulong  BaseOfCode;
  /* ulong  BaseOfData; // Non-exist for 64-bit */
  ulong64 ImageBase;
  ulong  SectionAlignment;
  ulong  FileAlignment;
  ushort MajorOperatingSystemVersion;
  ushort MinorOperatingSystemVersion;
  ushort MajorImageVersion;
  ushort MinorImageVersion;
  ushort MajorSubsystemVersion;
  ushort MinorSubsystemVersion;
  ulong  Win32VersionValue;
  ulong  SizeOfImage;
  ulong  SizeOfHeaders;
  ulong  CheckSum;
  ushort SubSystem;
  ushort DllCharacteristics;
  ulong64 SizeOfStackReserve;
  ulong64 SizeOfStackCommit;
  ulong64 SizeOfHeapReserve;
  ulong64 SizeOfHeapCommit;
  ulong  LoaderFlags;
  ulong  NumberOfRvaAndSizes;
  // IMAGE_DATA_DIRECTORY
  TDataDirectory Export;
  TDataDirectory Import;
  TDataDirectory Resource;
  TDataDirectory Exception;
  TDataDirectory Security;
  TDataDirectory Basereloc;
  TDataDirectory Debug;
  TDataDirectory Copyright;
  TDataDirectory Architecture;
  TDataDirectory Globalptr;
  TDataDirectory TLS;
  TDataDirectory LoadConfig;
  TDataDirectory BoundImport;
  TDataDirectory IAT;
  TDataDirectory DelayImport;
  TDataDirectory ComDescriptor;
};

/*template <typename T>
typedef TPEHeaderT<T> *PTPEHeaderT;*/

typedef TPEHeaderT<ulong32> TPEHeader32, *PPEHeader32;
typedef TPEHeaderT<ulong64> TPEHeader64, *PPEHeader64;

/* The common types (32-bit & 64-bit)  */

#ifdef _WIN64
typedef TNtHeader64   TNtHeader,  *PNtHeader;
typedef TOptHeader64  TOptHeader, *POptHeader;
typedef TThunkData64  TThunkData, *PThunkData;
typedef TPEHeader64   TPEHeader,  *PPEHeader;
#else // _WIN32
typedef TNtHeader32   TNtHeader,  *PNtHeader;
typedef TOptHeader32  TOptHeader, *POptHeader;
typedef TThunkData32  TThunkData, *PThunkData;
typedef TPEHeader32   TPEHeader,  *PPEHeader;
#endif

typedef struct _SERVICE_STATUS TServiceStatus;

/* Vutils Constants */

const VUResult VU_OK  = 0;

/* Vutils Enumerates */

#ifndef PROCESSOR_ARCHITECTURE_NEUTRAL
#define PROCESSOR_ARCHITECTURE_NEUTRAL 11
#endif

typedef enum _PROCESSOR_ARCHITECTURE
{
  PA_X86      = PROCESSOR_ARCHITECTURE_INTEL,
  PA_MIPS     = PROCESSOR_ARCHITECTURE_MIPS,
  PA_ALPHA    = PROCESSOR_ARCHITECTURE_ALPHA,
  PA_PPC      = PROCESSOR_ARCHITECTURE_PPC,
  PA_SHX      = PROCESSOR_ARCHITECTURE_SHX,
  PA_ARM      = PROCESSOR_ARCHITECTURE_ARM,
  PA_ITANIUM  = PROCESSOR_ARCHITECTURE_IA64,
  PA_ALPHA64  = PROCESSOR_ARCHITECTURE_ALPHA64,
  PA_MSIL     = PROCESSOR_ARCHITECTURE_MSIL,
  PA_X64      = PROCESSOR_ARCHITECTURE_AMD64,
  PA_WOW64    = PROCESSOR_ARCHITECTURE_IA32_ON_WIN64,
  PA_NEUTRAL  = PROCESSOR_ARCHITECTURE_NEUTRAL,
  PA_UNKNOWN  = PROCESSOR_ARCHITECTURE_UNKNOWN,
} eProcessorArchitecture;

typedef enum _WOW64
{
  WOW64_ERROR = -1,
  WOW64_NO    = 0,
  WOW64_YES   = 1,
} eWow64;

typedef enum _XBit
{
  x86 = 4,
  x64 = 8,
} eXBit;

// CFileSystem

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
  FA_UNKNOWN       = 0x00000000,
  FA_READONLY      = 0x00000001,   // FILE_ATTRIBUTE_READONLY             = $00000001;
  FA_HIDDEN        = 0x00000002,   // FILE_ATTRIBUTE_HIDDEN               = $00000002;
  FA_SYSTEM        = 0x00000004,   // FILE_ATTRIBUTE_SYSTEM               = $00000004;
  FA_DIRECTORY     = 0x00000010,   // FILE_ATTRIBUTE_DIRECTORY            = $00000010;
  FA_ARCHIVE       = 0x00000020,   // FILE_ATTRIBUTE_ARCHIVE              = $00000020;
  FA_DEVICE        = 0x00000040,   // FILE_ATTRIBUTE_DEVICE               = $00000040;
  FA_NORMAL        = 0x00000080,   // FILE_ATTRIBUTE_NORMAL               = $00000080;
  FA_TEMPORARY     = 0x00000100,   // FILE_ATTRIBUTE_TEMPORARY            = $00000100;
  FA_SPARSEFILE    = 0x00000200,   // FILE_ATTRIBUTE_SPARSE_FILE          = $00000200;
  FA_REPARSEPOINT  = 0x00000400,   // FILE_ATTRIBUTE_REPARSE_POINT        = $00000400;
  FA_COMPRESSED    = 0x00000800,   // FILE_ATTRIBUTE_COMPRESSED           = $00000800;
  FA_OFFLINE       = 0x00001000,   // FILE_ATTRIBUTE_OFFLINE              = $00001000;
  FANOTCONTENTINDEXED = 0x00002000,// FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  = $00002000;
  FAENCRYPTED      = 0x00004000,   // FILE_ATTRIBUTE_ENCRYPTED           = $00004000;
} eFSAttributeFlags;

typedef enum _FS_SHARE_FLAGS
{
  FS_NONE       = 0x00000000,
  FS_READ       = 0x00000001,
  FS_WRITE      = 0x00000002,
  FS_DELETE     = 0x00000004,
  FS_READWRITE  = FS_READ | FS_WRITE,
  FS_ALLACCESS  = FS_READ | FS_WRITE | FS_DELETE,
} eFSShareFlags;

typedef enum _FS_GENERIC_FLAGS
{
  FG_ALL        = GENERIC_ALL,
  FG_EXECUTE    = GENERIC_EXECUTE,
  FG_WRITE      = GENERIC_WRITE,
  FG_READ       = GENERIC_READ,
  FG_READWRITE  = GENERIC_READ | GENERIC_WRITE,
} eFSGenericFlags;

typedef enum _MOVE_METHOD_FLAGS
{
  MM_BEGIN   = FILE_BEGIN,
  MM_CURRENT = FILE_CURRENT,
  MM_END     = FILE_END,
} eMoveMethodFlags;

typedef struct _FS_OBJECT_A
{
  std::string Directory;
  std::string Name;
  int64 Size;
  ulong Attributes;
} TFSObjectA;

typedef struct _FS_OBJECT_W
{
  std::wstring Directory;
  std::wstring Name;
  int64 Size;
  ulong Attributes;
} TFSObjectW;

// CESocket

#define AF_IRDA  26
#define AF_INET6 23

typedef enum _SOCKET_AF
{
  SAF_UNSPEC    = AF_UNSPEC,
  SAF_INET      = AF_INET,
  SAF_IPX       = AF_IPX,
  SAF_APPLETALK = AF_APPLETALK,
  SAF_NETBIOS   = AF_NETBIOS,
  SAF_INET6     = AF_INET6,
  SAF_IRDA      = AF_IRDA,
  SAF_BTH       = AF_IRDA,
} eSocketAF;

typedef enum _SOCKET_TYPE
{
  ST_NONE      = 0,
  ST_STREAM    = SOCK_STREAM,
  ST_DGRAM     = SOCK_DGRAM,
  ST_RAW       = SOCK_RAW,
  ST_RDM       = SOCK_RDM,
  ST_SEQPACKET = SOCK_RDM,
} eSocketType;

#define BTHPROTO_RFCOMM 3
#define IPPROTO_ICMPV6  58
#define IPPROTO_RM      113

typedef enum _SOCKET_PROTOCOL
{
  SP_NONE    = 0,
  SP_CMP     = IPPROTO_ICMP,
  SP_IGMP    = IPPROTO_IGMP,
  SP_RFCOMM  = BTHPROTO_RFCOMM,
  SP_TCP     = IPPROTO_TCP,
  SP_UDP     = IPPROTO_UDP,
  SP_ICMPV6  = IPPROTO_ICMPV6,
  SP_RM      = IPPROTO_RM,
} eSocketProtocol;

#define MSG_WAITALL     0x8
#define MSG_INTERRUPT   0x10
#define MSG_PARTIAL     0x8000

typedef enum _SOCKET_FLAG
{
  SF_NONE      = 0,
  SF_OOB       = MSG_OOB,
  SF_PEEK      = MSG_PEEK,
  SF_DONTROUTE = MSG_DONTROUTE,
  SF_WAITALL   = MSG_WAITALL,
  SF_PARTIAL   = MSG_PARTIAL,
  SF_INTERRUPT = MSG_INTERRUPT,
  SF_MAXIOVLEN = MSG_MAXIOVLEN,
} eSocketMessage;

typedef enum _SHUTDOWN_FLAG
{
  SF_UNKNOWN = -1,
  SF_RECEIVE = 0,
  SF_SEND,
  SF_BOTH,
} eShutdownFlag;

// CEService

typedef enum _SERVICE_ACCESS_TYPE
{
  SAT_UNKNOWN              = -1,
  SAT_QUERY_CONFIG         = SERVICE_QUERY_CONFIG,
  SAT_CHANGE_CONFIG        = SERVICE_CHANGE_CONFIG,
  SAT_QUERY_STATUS         = SERVICE_QUERY_STATUS,
  SAT_ENUMERATE_DEPENDENTS = SERVICE_ENUMERATE_DEPENDENTS,
  SAT_START                = SERVICE_START,
  SAT_STOP                 = SERVICE_STOP,
  SAT_PAUSE_CONTINUE       = SERVICE_PAUSE_CONTINUE,
  SAT_INTERROGATE          = SERVICE_INTERROGATE,
  SAT_USER_DEFINED_CONTROL = SERVICE_USER_DEFINED_CONTROL,
  SAT_ALL_ACCESS           = SERVICE_ALL_ACCESS,
  SAT_DELETE               = DELETE,
} eServiceAccessType;

typedef enum _SERVICE_TYPE
{
  ST_UNKNOWN             = -1,
  ST_KERNEL_DRIVER       = SERVICE_KERNEL_DRIVER,
  ST_SYSTEM_DRIVER       = SERVICE_FILE_SYSTEM_DRIVER,
  ST_ADAPTER             = SERVICE_ADAPTER,
  ST_RECOGNIZER_DRIVER   = SERVICE_RECOGNIZER_DRIVER,
  ST_WIN32_OWN_PROCESS   = SERVICE_WIN32_OWN_PROCESS,
  ST_WIN32_SHARE_PROCESS = SERVICE_WIN32_SHARE_PROCESS,
} eServiceType;

typedef enum _SERVICE_STATE
{
  SS_UNKNOWN          = -1,
  SS_STOPPED          = SERVICE_STOPPED,
  SS_START_PENDING    = SERVICE_START_PENDING,
  SS_STOP_PENDING     = SERVICE_STOP_PENDING,
  SS_RUNNING          = SERVICE_RUNNING,
  SS_CONTINUE_PENDING = SERVICE_CONTINUE_PENDING,
  SS_PAUSE_PENDING    = SERVICE_PAUSE_PENDING,
  SS_PAUSED           = SERVICE_PAUSED,
} eServiceState;

#ifndef SERVICE_CONTROL_PRESHUTDOWN
#define SERVICE_CONTROL_PRESHUTDOWN 0x0000000F
#endif

#ifndef SERVICE_CONTROL_TIMECHANGE
#define SERVICE_CONTROL_TIMECHANGE  0x00000010
#endif

#ifndef SERVICE_CONTROL_TRIGGEREVENT
#define SERVICE_CONTROL_TRIGGEREVENT  0x00000020
#endif

typedef enum _SERVICE_CONTROL
{
  SC_STOP                  = SERVICE_CONTROL_STOP,
  SC_PAUSE                 = SERVICE_CONTROL_PAUSE,
  SC_CONTINUE              = SERVICE_CONTROL_CONTINUE,
  SC_INTERROGATE           = SERVICE_CONTROL_INTERROGATE,
  SC_SHUTDOWN              = SERVICE_CONTROL_SHUTDOWN,
  SC_PARAMCHANGE           = SERVICE_CONTROL_PARAMCHANGE,
  SC_NETBINDADD            = SERVICE_CONTROL_NETBINDADD,
  SC_NETBINDREMOVE         = SERVICE_CONTROL_NETBINDREMOVE,
  SC_NETBINDENABLE         = SERVICE_CONTROL_NETBINDENABLE,
  SC_NETBINDDISABLE        = SERVICE_CONTROL_NETBINDDISABLE,
  SC_DEVICEEVENT           = SERVICE_CONTROL_DEVICEEVENT,
  SC_HARDWAREPROFILECHANGE = SERVICE_CONTROL_HARDWAREPROFILECHANGE,
  SC_POWEREVENT            = SERVICE_CONTROL_POWEREVENT,
  SC_SESSIONCHANGE         = SERVICE_CONTROL_SESSIONCHANGE,
  SC_PRESHUTDOWN           = SERVICE_CONTROL_PRESHUTDOWN,
  SC_TIMECHANGE            = SERVICE_CONTROL_TIMECHANGE,
  SC_TRIGGEREVENT          = SERVICE_CONTROL_TRIGGEREVENT,
} eServiceControl;

typedef enum _SERVICE_START_TYPE
{
  SST_UNKNOWN      = -1,
  SST_BOOT_START   = SERVICE_BOOT_START,
  SST_SYSTEM_START = SERVICE_SYSTEM_START,
  SST_AUTO_START   = SERVICE_AUTO_START,
  SST_DEMAND_START = SERVICE_DEMAND_START,
  SST_DISABLED     = SERVICE_DISABLED,
} eServiceStartType;

typedef enum _SERVICE_ERROR_CONTROL_TYPE
{
  SE_UNKNOWN  = -1,
  SE_IGNORE   = SERVICE_ERROR_IGNORE,
  SE_NORMAL   = SERVICE_ERROR_NORMAL,
  SE_SEVERE   = SERVICE_ERROR_SEVERE,
  SE_CRITICAL = SERVICE_ERROR_CRITICAL,
} eServiceErrorControlType;

typedef enum _SC_ACCESS_TYPE
{
  SC_CONNECT            = SC_MANAGER_CONNECT,
  SC_CREATE_SERVICE     = SC_MANAGER_CREATE_SERVICE,
  SC_ENUMERATE_SERVICE  = SC_MANAGER_ENUMERATE_SERVICE,
  SC_LOCK               = SC_MANAGER_LOCK,
  SC_QUERY_LOCK_STATUS  = SC_MANAGER_QUERY_LOCK_STATUS,
  SC_MODIFY_BOOT_CONFIG = SC_MANAGER_MODIFY_BOOT_CONFIG,
  SC_ALL_ACCESS         = SC_MANAGER_ALL_ACCESS,
} eSCAccessType;

// CERegistry

/*
// Refer to winreg.h
// Reserved Key Handles.
//

#define HKEY_CLASSES_ROOT                 (( HKEY ) (ULONG_PTR)((LONG)0x80000000) )
#define HKEY_CURRENT_USER                 (( HKEY ) (ULONG_PTR)((LONG)0x80000001) )
#define HKEY_LOCAL_MACHINE                (( HKEY ) (ULONG_PTR)((LONG)0x80000002) )
#define HKEY_USERS                        (( HKEY ) (ULONG_PTR)((LONG)0x80000003) )
#define HKEY_PERFORMANCE_DATA             (( HKEY ) (ULONG_PTR)((LONG)0x80000004) )
#define HKEY_PERFORMANCE_TEXT             (( HKEY ) (ULONG_PTR)((LONG)0x80000050) )
#define HKEY_PERFORMANCE_NLSTEXT          (( HKEY ) (ULONG_PTR)((LONG)0x80000060) )
#define HKEY_CURRENT_CONFIG               (( HKEY ) (ULONG_PTR)((LONG)0x80000005) )
#define HKEY_DYN_DATA                     (( HKEY ) (ULONG_PTR)((LONG)0x80000006) )
#define HKEY_CURRENT_USER_LOCAL_SETTINGS  (( HKEY ) (ULONG_PTR)((LONG)0x80000007) )*/

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

typedef enum _TRIM_STRING
{
  TS_LEFT  = 0,
  TS_RIGHT = 1,
  TS_BOTH  = 2,
} eTrimType;

typedef enum _ENCODING_TYPE
{
  ET_UNKNOWN      = -1,
  ET_UTF8         = 0, /* aka ANSI */
  ET_UTF8_BOM     = 1, /* EF BB BF */
  ET_UTF16_LE     = 2,
  ET_UTF16_BE     = 3,
  ET_UTF16_LE_BOM = 4, /* FF FE */
  ET_UTF16_BE_BOM = 5, /* FE FF */
} eEncodingType;

/* ------------------------------------------------ Public Macro(s) ------------------------------------------------ */

#define FundamentalTypeLengthOf(X) (sizeof(X) / sizeof(X[0]))
#define FundamentalTypeIsSigned(T) (std::is_signed<T>())
#define FundamentalTypeIsExact(T) (std::numeric_limits<T>::is_exact)
#define FundamentalTypeGetLowest(T) (std::numeric_limits<T>::lowest())
#define FundamentalTypeGetHighest(T) (std::numeric_limits<T>::max())
#define FundamentalTypeGetDigits(T) (std::numeric_limits<T>::digits)

/* ---------------------------------------------- Public Function(s) ----------------------------------------------- */

/* --- Group : Misc Working --- */

bool vuapi IsAdministrator();
std::string vuapi  GetEnviromentA(const std::string EnvName);
std::wstring vuapi GetEnviromentW(const std::wstring EnvName);

/* --- Group : Math Working --- */

bool vuapi IsFlagOn(ulongptr ulFlags, ulongptr ulFlag);
intptr vuapi GCD(ulongptr count, ...); // UCLN
intptr vuapi LCM(ulongptr count, ...); // BCNN
void vuapi HexDump(const void* Data, int Size);

/* --- Group : String Formatting --- */

std::string vuapi FormatA(const std::string Format, ...);
std::wstring vuapi FormatW(const std::wstring Format, ...);
void vuapi MsgA(const std::string Format, ...);
void vuapi MsgW(const std::wstring Format, ...);
int vuapi BoxA(const std::string Format, ...);
int vuapi BoxW(const std::wstring Format, ...);
int vuapi BoxA(HWND hWnd, const std::string Format, ...);
int vuapi BoxW(HWND hWnd, const std::wstring Format, ...);
int vuapi BoxA(HWND hWnd, uint uType, const std::string& Caption, const std::string Format, ...);
int vuapi BoxW(HWND hWnd, uint uType, const std::wstring& Caption, const std::wstring Format, ...);
std::string vuapi LastErrorA(ulong ulErrorCode = -1);
std::wstring vuapi LastErrorW(ulong ulErrorCode = -1);
std::string vuapi FormatDateTimeA(const time_t t, const std::string Format);
std::wstring vuapi FormatDateTimeW(const time_t t, const std::wstring Format);
std::string vuapi DateTimeToStringA(const time_t t);
std::wstring vuapi DateTimeToStringW(const time_t t);
eEncodingType vuapi DetermineEncodingType(const void* Data, const size_t size);

/* --- Group : String Working --- */

std::string vuapi LowerStringA(const std::string& String);
std::wstring vuapi LowerStringW(const std::wstring& String);
std::string vuapi UpperStringA(const std::string& String);
std::wstring vuapi UpperStringW(const std::wstring& String);
std::string vuapi ToStringA(const std::wstring& String);
std::wstring vuapi ToStringW(const std::string& String);
std::vector<std::string> vuapi SplitStringA(const std::string& String, const std::string& Seperate);
std::vector<std::wstring> vuapi SplitStringW(const std::wstring& lpcwszString, const std::wstring& Seperate);
std::vector<std::string> vuapi MultiStringToListA(const char* lpcszMultiString);
std::vector<std::wstring> vuapi MultiStringToListW(const wchar* lpcwszMultiString);
std::unique_ptr<char[]> vuapi ListToMultiStringA(const std::vector<std::string>& StringList);
std::unique_ptr<wchar[]> vuapi ListToMultiStringW(const std::vector<std::wstring>& StringList);
std::string vuapi LoadResourceStringA(const UINT uID, HINSTANCE ModuleHandle = nullptr, const std::string& ModuleName = "");
std::wstring vuapi LoadResourceStringW(const UINT uID, HINSTANCE ModuleHandle = nullptr, const std::wstring& ModuleName = L"");
std::string vuapi TrimStringA(
  const std::string& String,
  const eTrimType& TrimType = eTrimType::TS_BOTH,
  const std::string& TrimChars = " \t\n\r\f\v"
);
std::wstring vuapi TrimStringW(
  const std::wstring& String,
  const eTrimType& TrimType = eTrimType::TS_BOTH,
  const std::wstring& TrimChars = L" \t\n\r\f\v"
);

/* --- Group : Process Working --- */

HWND vuapi GetConsoleWindowHandle();
eProcessorArchitecture GetProcessorArchitecture();
eWow64 vuapi IsWow64(ulong ulPID = (ulong)-1); /* -1: Error, 0: False, 1: True */
std::vector<ulong> vuapi NameToPIDA(const std::string& ProcessName, ulong ulMaxProcessNumber = MAX_NPROCESSES);
std::vector<ulong> vuapi NameToPIDW(const std::wstring& ProcessName, ulong ulMaxProcessNumber = MAX_NPROCESSES);
std::string vuapi PIDToNameA(ulong ulPID);
std::wstring vuapi PIDToNameW(ulong ulPID);
HMODULE vuapi RemoteGetModuleHandleA(ulong ulPID, const std::string& ModuleName); // Not complete
HMODULE vuapi RemoteGetModuleHandleW(ulong ulPID, const std::wstring& ModuleName);
bool vuapi RPM(
  const HANDLE hProcess,
  const void* lpAddress,
  void* lpBuffer,
  const SIZE_T ulSize,
  const bool force = false
);
bool vuapi RPMEX(
  const eXBit bit,
  const HANDLE Handle,
  const void* lpAddress,
  void* lpBuffer,
  const SIZE_T ulSize,
  const bool force = false,
  const SIZE_T nOffsets = 0,
  ...
);
bool vuapi WPM(
  const HANDLE hProcess,
  const void* lpAddress,
  const void* lpcBuffer,
  const SIZE_T ulSize,
  const bool force = false
);
bool vuapi WPMEX(
  const eXBit bit,
  const HANDLE Handle,
  const void* lpAddress,
  const void* lpBuffer,
  const SIZE_T ulSize,
  const bool force = false,
  const SIZE_T nOffsets = 0,
  ...
);

/* --- Group : File/Directory Working --- */

bool vuapi IsDirectoryExistsA(const std::string& Directory);
bool vuapi IsDirectoryExistsW(const std::wstring& Directory);
bool vuapi IsFileExistsA(const std::string& FilePath);
bool vuapi IsFileExistsW(const std::wstring& FilePath);
std::string vuapi FileTypeA(const std::string& FilePath);
std::wstring vuapi FileTypeW(const std::wstring& FilePath);
std::string vuapi ExtractFilePathA(const std::string& FilePath, bool bIncludeSlash = true);
std::wstring vuapi ExtractFilePathW(const std::wstring& FilePath, bool bIncludeSlash = true);
std::string vuapi ExtractFileNameA(const std::string& FilePath, bool bIncludeExtension = true);
std::wstring vuapi ExtractFileNameW(const std::wstring& FilePath, bool bIncludeExtension = true);
std::string vuapi GetCurrentFilePathA();
std::wstring vuapi GetCurrentFilePathW();
std::string vuapi GetCurrentDirectoryA(bool bIncludeSlash = true);
std::wstring vuapi GetCurrentDirectoryW(bool bIncludeSlash = true);

/*--------------------- The definition of common function(s) which compatible both ANSI & UNICODE -------------------*/

#ifdef _UNICODE
/* --- Group : Misc Working --- */
#define GetEnviroment GetEnviromentW
/* --- Group : String Formatting --- */
#define Fmt FormatW
#define Msg MsgW
#define Box BoxW
#define LastError LastErrorW
#define DateTimeToString DateTimeToStringW
#define FormatDateTime FormatDateTimeW
/* --- Group : String Working --- */
#define LowerString LowerStringW
#define UpperString UpperStringW
#define SplitString SplitStringW
#define MultiStringToList MultiStringToListW
#define ListToMultiString ListToMultiStringW
#define LoadResourceString LoadResourceStringW
#define TrimString TrimStringW
/* --- Group : Process Working --- */
#define NameToPID NameToPIDW
#define PIDToName PIDToNameW
#define RemoteGetModuleHandle RemoteGetModuleHandleW
/* --- Group : File/Directory Working --- */
#define IsDirectoryExists IsDirectoryExistsW
#define IsFileExists IsFileExistsW
#define FileType FileTypeW
#define ExtractFilePath ExtractFilePathW
#define ExtractFileName ExtractFileNameW
#define GetCurrentFilePath GetCurrentFilePathW
#define GetCurDirectory GetCurrentDirectoryW
#else
/* --- Group : Misc Working --- */
#define GetEnviroment GetEnviromentA
/* --- Group : String Formatting --- */
#define Fmt FormatA
#define Msg MsgA
#define Box BoxA
#define LastError LastErrorA
#define DateTimeToString DateTimeToStringA
#define FormatDateTime FormatDateTimeA
/* --- Group : String Working --- */
#define LowerString LowerStringA
#define UpperString UpperStringA
#define SplitString SplitStringA
#define MultiStringToList MultiStringToListA
#define LoadResourceString LoadResourceStringA
#define TrimString TrimStringA
/* --- Group : Process Working --- */
#define NameToPID NameToPIDA
#define PIDToName PIDToNameA
#define RemoteGetModuleHandle RemoteGetModuleHandleA
/* --- Group : File/Directory Working --- */
#define IsDirectoryExists IsDirectoryExistsA
#define IsFileExists IsFileExistsA
#define FileType FileTypeA
#define ExtractFilePath ExtractFilePathA
#define ExtractFileName ExtractFileNameA
#define GetCurrentFilePath GetCurrentFilePathA
#define GetCurDirectory GetCurrentDirectoryA
#endif

/* ------------------------------------------------ Public Class(es) ----------------------------------------------- */

class CLastError
{
public:
  CLastError() : m_LastErrorCode(ERROR_SUCCESS) {};
  virtual ~CLastError() {};

  CLastError& operator=(const CLastError& right)
  {
    m_LastErrorCode = right.m_LastErrorCode;
    return *this;
  }

  virtual void vuapi SetLastErrorCode(ulong ulLastError)
  {
    m_LastErrorCode = ulLastError;
  }

  virtual ulong vuapi GetLastErrorCode()
  {
    return m_LastErrorCode;
  }

  virtual std::string vuapi GetLastErrorMessageA()
  {
    return LastErrorA(m_LastErrorCode);
  }

  virtual std::wstring vuapi GetLastErrorMessageW()
  {
    return LastErrorW(m_LastErrorCode);
  }

protected:
  ulong m_LastErrorCode;
};

/* --- Group : Singleton --- */

/**
 * The singleton class for inheritance.
 * Eg. class CObject : public vu::CSingletonT<CObject> {}
 * CObject::Instance()->Method();
 */
template<typename T>
class CSingletonT
{
public:
  CSingletonT()
  {
    m_pInstance = static_cast<T*>(this);
  }

  virtual ~CSingletonT()
  {
  }

  static T* Instance()
  {
    if (m_pInstance == nullptr)
    {
      CSingletonT<T>::m_pInstance = new T();
    }

    return m_pInstance;
  }

  static void Destroy()
  {
    if (CSingletonT<T>::m_pInstance != nullptr)
    {
      delete CSingletonT<T>::m_pInstance;
      CSingletonT<T>::m_pInstance = nullptr;
    }
  }

protected:
  static T* m_pInstance;
};

template<typename T>
T* CSingletonT<T>::m_pInstance = nullptr;

/* --- Group : GUID --- */

#ifdef VU_GUID_ENABLED

struct sGUID
{
  unsigned long  Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char  Data4[8];

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

  bool Create();

  const sGUID& GUID() const;
  void  GUID(const sGUID& guid);

protected:
  sGUID m_GUID;
  VUResult m_Status;
};

class CGUIDA : public CGUIDX
{
public:
  CGUIDA(bool create = false) : CGUIDX(create) {}
  virtual ~CGUIDA() {}

  void Parse(const std::string& guid);
  std::string AsString() const;

  static const std::string ToString(const sGUID& guid);
  static const sGUID ToGUID(const std::string& guid);
};

class CGUIDW : public CGUIDX
{
public:
  CGUIDW(bool create = false) : CGUIDX(create) {}
  virtual ~CGUIDW() {}

  void Parse(const std::wstring& guid);
  std::wstring AsString() const;

  static const std::wstring ToString(const sGUID& guid);
  static const sGUID ToGUID(const std::wstring& guid);
};

#endif // VU_GUID_ENABLED

/* --- Group : Binary --- */

class CBinary
{
public:
  CBinary();
  CBinary(const CBinary& right);
  CBinary(const void* pData, ulong ulSize);
  CBinary(ulong ulSize);
  virtual ~CBinary();

  const CBinary& operator=(const CBinary& right);
  bool operator==(const CBinary& right) const;
  bool operator!=(const CBinary& right) const;
  void AdjustSize (const ulong ulSize);
  const ulong GetSize() const;
  void SetUsedSize(const ulong ulUsedSize);
  const ulong GetUsedSize() const;
  void SetpData(const void* pData, ulong ulSize);
  void* GetpData();
  const void* GetpData() const;

private:
  void* m_pData;
  ulong m_Size;
  ulong m_UsedSize;
};

/* --- Group : Library --- */

/**
 * @brief Gets a function in a module by name.
 * @define typedef type (cc *PfnF)(types args);
 * @define PfnF pfnF = nullptr;
 * @param[in] M The module name.
 * @param[in] F The function name.
 * @return  The function.
 */
#define VU_GET_API(M, F) pfn ## F = (Pfn ## F)vu::CLibrary::QuickGetProcAddress(_T( # M ), _T( # F ))

class CLibraryA : public CLastError
{
public:
  CLibraryA();
  CLibraryA(const std::string& ModuleName);
  CLibraryA(const std::string& ModuleName, const std::string& RoutineName);
  virtual ~CLibraryA();

  bool  vuapi IsLibraryAvailable();
  void* vuapi GetProcAddress();
  void* vuapi GetProcAddress(const std::string& ProcName);
  void* vuapi GetProcAddress(const std::string& ModuleName, const std::string& ProcName);
  static void* vuapi QuickGetProcAddress(const std::string& ModuleName, const std::string& ProcName);

private:
  std::string m_ModuleName, m_ProcName;
};

class CLibraryW : public CLastError
{
public:
  CLibraryW();
  CLibraryW(const std::wstring& ModuleName);
  CLibraryW(const std::wstring& ModuleName, const std::wstring& ProcName);
  virtual ~CLibraryW();

  bool  vuapi IsLibraryAvailable();
  void* vuapi GetProcAddress();
  void* vuapi GetProcAddress(const std::wstring& RoutineName);
  void* vuapi GetProcAddress(const std::wstring& ModuleName, const std::wstring& ProcName);
  static void* vuapi QuickGetProcAddress(const std::wstring& ModuleName, const std::wstring& ProcName);

private:
  std::wstring m_ModuleName, m_ProcName;
};

/* --- Group : Socket --- */

#ifdef VU_SOCKET_ENABLED

const std::string VU_LOCALHOST = "127.0.0.1";

typedef struct
{
  SOCKET s;
  sockaddr_in sai;
  char ip[15];
} TSocketInfomation;

struct TAccessPoint
{
  std::string Host;
  ushort Port;
};

class CSocket : public CLastError
{
private:
  WSADATA m_WSAData;
  SOCKET m_Socket;
  sockaddr_in m_Server;
  hostent m_Client;

  bool vuapi IsSocketValid(SOCKET socket);

public:
  CSocket();
  CSocket(eSocketAF SockAF, eSocketType SocketType);
  virtual ~CSocket();
  VUResult vuapi Socket(eSocketAF SocketAF, eSocketType SocketType, eSocketProtocol SocketProtocol = SP_NONE);
  VUResult vuapi Bind(const TAccessPoint& AccessPoint);
  VUResult vuapi Bind(const std::string& Address, ushort usNPort);
  VUResult vuapi Listen(int iMaxConnection = SOMAXCONN);
  VUResult vuapi Accept(TSocketInfomation& SocketInformation);
  VUResult vuapi Connect(const TAccessPoint& AccessPoint);
  VUResult vuapi Connect(const std::string& Address, ushort usPort);
  IResult vuapi Send(const char* lpData, int iLength, eSocketMessage SocketMessage = SF_NONE);
  IResult vuapi Send(const CBinary& Data, eSocketMessage SocketMessage = SF_NONE);
  IResult vuapi Send(const SOCKET socket, const char* lpData, int iLength, eSocketMessage SocketMessage = SF_NONE);
  IResult vuapi Recv(char* lpData, int iLength, eSocketMessage SocketMessage = SF_NONE);
  IResult vuapi Recv(CBinary& Data, eSocketMessage SocketMessage = SF_NONE);
  IResult vuapi Recv(SOCKET socket, char* lpData, int iLength, eSocketMessage SocketMessage = SF_NONE);
  IResult vuapi SendTo(const char* lpData, int iLength, TSocketInfomation& SocketInformation);
  IResult vuapi SendTo(const CBinary& Data, TSocketInfomation& SocketInformation);
  IResult vuapi RecvFrom(char* lpData, int iLength, TSocketInfomation& SocketInformation);
  IResult vuapi RecvFrom(CBinary& Data, TSocketInfomation& SocketInformation);
  bool vuapi Close(SOCKET socket = 0);
  SOCKET vuapi GetSocket();
  VUResult vuapi GetOption(int iLevel, int iOptName, char* pOptVal, int* lpiLength);
  VUResult vuapi SetOption(int iLevel, int iOptName, const std::string& OptVal, int iLength);
  VUResult vuapi Shutdown(eShutdownFlag ShutdownFlag);
  std::string vuapi GetLocalHostName();
  std::string vuapi GetHostByName(const std::string& Name);
  bool vuapi IsHostName(const std::string& s);
  bool vuapi BytesToIP(const TSocketInfomation& SocketInformation);
};

#endif // VU_SOCKET_ENABLED

/* --- Group : API Hooking --- */

 /**
 * @brief Hook/Unhook a function in a module by name.
 * @define The prefix of redirection function must be  : Hfn
 * @define The prefix of real function pointer must be : pfn
 * @param[in] O The CDynHook instance.
 * @param[in] M The module name.
 * @param[in] F The function name.
 * @return  true if the function succeeds. Otherwise false.
 */
#define VU_ATTACH_API(O, M, F) O.APIAttach(_T( # M ), _T( # F ), (void*)&Hfn ## F, (void**)&pfn ## F)
#define VU_DETACH_API(O, M, F) O.APIDetach(_T( # M ), _T( # F ), (void**)&pfn ## F)

class CDynHookX
{
protected:
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
    ulong Offset;   // Offset of the current instruction.
    ulong Position; // Position of the memory address in the current instruction.
    eMemoryAddressType MemoryAddressType;
    union           // Memory Instruction value.
    {
      uchar   A8;
      ushort  A16;
      ulong   A32;
      ulong64 A64;
    } MAO;
    union           // Memory Instruction value.
    {
      uchar   A8;
      ushort  A16;
      ulong   A32;
      ulong64 A64;
    } MAN;
  } TMemoryInstruction;

  typedef struct _REDIRECT
  {
    ushort   JMP;
    ulong    Unknown;
    ulongptr Address;
  } TRedirect;

  bool m_Hooked;
  std::vector<TMemoryInstruction> m_ListMemoryInstruction;

  static const ulong JMP_OPCODE_SIZE = 6;

  #ifdef _M_IX86
  static const ulong MIN_HOOK_SIZE   = 10;
  #else  // _M_AMD64
  static const ulong MIN_HOOK_SIZE   = 14;
  #endif // _M_IX86

public:
  CDynHookX() : m_Hooked(false) {};
  virtual ~CDynHookX() {};

  ulongptr vuapi JumpLength(ulongptr ulSrcAddress, ulongptr ulDestAddress, ulongptr ulInstSize);
  bool vuapi Attach(void* pProc, void* pHookProc, void** pOldProc);
  bool vuapi Detach(void* pProc, void** pOldProc);

private:
  /**
   * To handle the memory instruction.
   * @param[in] hde     The HDE struct of the current instruction.
   * @param[in] offset  The offset of current instruction. From the head of the current function.
   * @return  True if current instruction is a memory struction, False if it is not.
   */
  bool vuapi HandleMemoryInstruction(const HDE::tagHDE& hde, const ulong offset);
};

class CDynHookA: public CDynHookX
{
public:
  CDynHookA() {};
  virtual ~CDynHookA() {};

  bool vuapi APIAttach(const std::string& ModuleName, const std::string& ProcName, void* lpHookProc, void** lpOldProc);
  bool vuapi APIDetach(const std::string& ModuleName, const std::string& ProcName, void** lpOldProc);
};

class CDynHookW: public CDynHookX
{
public:
  CDynHookW() {};
  virtual ~CDynHookW() {};

  bool vuapi APIAttach(const std::wstring& ModuleName, const std::wstring& ProcName, void* lpHookProc, void** lpOldProc);
  bool vuapi APIDetach(const std::wstring& ModuleName, const std::wstring& ProcName, void** lpOldProc);
};

/* --- Group : File Working --- */

class CFileSystemX : public CLastError
{
public:
  CFileSystemX() {};
  virtual ~CFileSystemX();
  virtual bool vuapi IsFileHandleValid(HANDLE fileHandle);
  virtual bool vuapi IsReady();
  virtual ulong vuapi GetFileSize();
  virtual const CBinary vuapi ReadContent();
  virtual bool vuapi Read(void* Buffer, ulong ulSize);
  virtual bool vuapi Read(ulong ulOffset, void* Buffer, ulong ulSize, eMoveMethodFlags mmFlag = MM_BEGIN);

  virtual bool vuapi Write(const void* cBuffer, ulong ulSize);
  virtual bool vuapi Write(ulong ulOffset, const void* cBuffer, ulong ulSize, eMoveMethodFlags mmFlag = MM_BEGIN);
  virtual bool vuapi Seek(ulong ulOffset, eMoveMethodFlags mmFlag);
  virtual bool vuapi IOControl(
    ulong ulControlCode,
    void* lpSendBuffer,
    ulong ulSendSize,
    void* lpReveiceBuffer,
    ulong ulReveiceSize
  );
  virtual bool vuapi Close();

protected:
  HANDLE m_FileHandle;

private:
  ulong m_ReadSize, m_WroteSize;
};

class CFileSystemA: public CFileSystemX
{
public:
  CFileSystemA() {};
  CFileSystemA(
    const std::string& FilePath,
    eFSModeFlags fmFlag,
    eFSGenericFlags fgFlag   = FG_READWRITE,
    eFSShareFlags fsFlag     = FS_ALLACCESS,
    eFSAttributeFlags faFlag = FA_NORMAL
  );
  virtual ~CFileSystemA() {};
  bool vuapi Init(
    const std::string& FilePath,
    eFSModeFlags fmFlag,
    eFSGenericFlags fgFlag   = FG_READWRITE,
    eFSShareFlags fsFlag     = FS_ALLACCESS,
    eFSAttributeFlags faFlag = FA_NORMAL
  );
  const std::string vuapi ReadFileAsString(bool forceBOM = true);
  static const std::string vuapi QuickReadFileAsString(const std::string& FilePath, bool forceBOM = true);
  static bool Iterate(
    const std::string& Path,
    const std::string& Pattern,
    const std::function<bool(const TFSObjectA& FSObject)> fnCallback
  );
};

class CFileSystemW: public CFileSystemX
{
public:
  CFileSystemW() {};
  CFileSystemW(
    const std::wstring& FilePath,
    eFSModeFlags fmFlag,
    eFSGenericFlags fgFlag   = FG_READWRITE,
    eFSShareFlags fsFlag     = FS_ALLACCESS,
    eFSAttributeFlags faFlag = FA_NORMAL
  );
  virtual ~CFileSystemW() {};
  bool vuapi Init(
    const std::wstring& FilePath,
    eFSModeFlags fmFlag,
    eFSGenericFlags fgFlag = FG_READWRITE,
    eFSShareFlags fsFlag   = FS_ALLACCESS,
    eFSAttributeFlags faFlag = FA_NORMAL
  );
  const std::wstring vuapi ReadAsString(bool forceBOM = true);
  static const std::wstring vuapi QuickReadAsString(const std::wstring& FilePath, bool forceBOM = true);
  static bool Iterate(
    const std::wstring& Path,
    const std::wstring& Pattern,
    const std::function<bool(const TFSObjectW& FSObject)> fnCallback
  );
};

/* --- Group : Service Working --- */

class CServiceX : public CLastError
{
public:
  CServiceX() : m_Initialized(false)
  {
    m_LastErrorCode = ERROR_SUCCESS;
  };

  virtual ~CServiceX() {};

  bool vuapi Init(eSCAccessType SCAccessType = eSCAccessType::SC_ALL_ACCESS);
  bool vuapi Destroy();
  bool vuapi Control(eServiceControl ServiceControl);
  bool vuapi Start();
  bool vuapi Stop();
  bool vuapi Close();

  bool vuapi QueryStatus(TServiceStatus& ServiceStatus);

  eServiceType vuapi GetType();
  eServiceState vuapi GetState();

protected:
  SC_HANDLE m_SCMHandle, m_ServiceHandle;
  SERVICE_STATUS m_Status;
  bool m_Initialized;
};

class CServiceA: public CServiceX
{
public:
  CServiceA();
  virtual ~CServiceA();

  bool vuapi Create(
    const std::string ServiceFilePath,
    eServiceAccessType ServiceAccessType = SAT_ALL_ACCESS,
    eServiceType ServiceType = eServiceType::ST_KERNEL_DRIVER,
    eServiceStartType ServiceStartType = eServiceStartType::SST_DEMAND_START,
    eServiceErrorControlType ServiceErrorControlType = eServiceErrorControlType::SE_IGNORE
  );

  bool vuapi Open(
    const std::string& ServiceName,
    eServiceAccessType ServiceAccessType = eServiceAccessType::SAT_ALL_ACCESS
  );

  std::string vuapi GetName(const std::string& AnotherServiceDisplayName = "");
  std::string vuapi GetDisplayName(const std::string& AnotherServiceName = "");

private:
  std::string m_Name;
  std::string m_DisplayName;
  std::string m_ServiceName;
  std::string m_ServiceFileName;
  std::string m_ServiceFilePath;
};

class CServiceW: public CServiceX
{
public:
  CServiceW();
  virtual ~CServiceW();

  bool vuapi Create(
    const std::wstring& ServiceFilePath,
    eServiceAccessType ServiceAccessType = eServiceAccessType::SAT_ALL_ACCESS,
    eServiceType ServiceType = eServiceType::ST_KERNEL_DRIVER,
    eServiceStartType ServiceStartType = eServiceStartType::SST_DEMAND_START,
    eServiceErrorControlType ServiceErrorControlType = eServiceErrorControlType::SE_IGNORE
  );

  bool vuapi Open(
    const std::wstring& ServiceName,
    eServiceAccessType ServiceAccessType = eServiceAccessType::SAT_ALL_ACCESS
  );

  std::wstring vuapi GetName(const std::wstring& AnotherServiceDisplayName = L"");
  std::wstring vuapi GetDisplayName(const std::wstring& AnotherServiceName = L"");

private:
  std::wstring m_Name;
  std::wstring m_DisplayName;
  std::wstring m_ServiceName;
  std::wstring m_ServiceFileName;
  std::wstring m_ServiceFilePath;
};

/* --- Group : File Mapping --- */

class CFileMappingA : public CLastError
{
private:
  bool m_HasInit;
  bool m_MapFile;
  HANDLE m_FileHandle;
  HANDLE m_MapHandle;
  void* m_pData;

  bool IsValidHandle(HANDLE Handle);

public:
  CFileMappingA();
  virtual ~CFileMappingA();

  /**
   * bMapFile: Is View Within a File or Named Shared Memory ?
   */
  VUResult vuapi Init(
    bool bMapFile = false,
    const std::string& FileName = "",
    eFSGenericFlags fgFlag = eFSGenericFlags::FG_ALL,
    eFSShareFlags fsFlag = eFSShareFlags::FS_ALLACCESS,
    eFSModeFlags fmFlag = eFSModeFlags::FM_OPENEXISTING,
    eFSAttributeFlags faFlag = eFSAttributeFlags::FA_NORMAL
  );

  /**
   * With View Within a File:  MapName: Must be empty (not NULL).
   * With Named Shared Memory: MapName: Eg: `Local/Global\\MyFileMappingObject`.
   * ulMaxSizeLow : The low-order of the maximum size of the file mapping object.
   */
  VUResult vuapi Create(
    const std::string& MapName,
    ulong ulMaxSizeLow,
    ulong ulMaxSizeHigh = 0 // The file mapping object is equal to the current size of the file.
  );

  /**
   * Just for Named Shared Memory.
   * MapName: Eg: `Local/Global\\MyFileMappingObject`.
   */
  VUResult vuapi Open(const std::string& MapName, bool bInheritHandle = false);

  /**
   * With View Within a File, it can not be NULL.
   * With Named Shared Memory, it will be the size.
   */
  void* vuapi View(
    ulong ulMaxFileOffsetLow   = 0,
    ulong ulMaxFileOffsetHigh  = 0,
    ulong ulNumberOfBytesToMap = 0 // The mapping extends from the specified offset to the end of the file mapping.
  );

  ulong vuapi GetFileSize();
  void vuapi Close();
};

class CFileMappingW : public CLastError
{
private:
  bool m_HasInit;
  bool m_MapFile;
  HANDLE m_FileHandle;
  HANDLE m_MapHandle;
  void* m_pData;

  bool IsValidHandle(HANDLE Handle);

public:
  CFileMappingW();
  virtual ~CFileMappingW();

  /**
   * bMapFile: Is View Within a File or Named Shared Memory ?
   */
  VUResult vuapi Init(
    bool bMapFile = false,
    const std::wstring FileName = L"",
    eFSGenericFlags fgFlag = eFSGenericFlags::FG_ALL,
    eFSShareFlags fsFlag = eFSShareFlags::FS_ALLACCESS,
    eFSModeFlags fmFlag = eFSModeFlags::FM_OPENEXISTING,
    eFSAttributeFlags faFlag = eFSAttributeFlags::FA_NORMAL
  );

  /**
   * With View Within a File:  MapName: Must be empty (not NULL).
   * With Named Shared Memory: MapName: Eg: `Local/Global\\MyFileMappingObject`.
   * ulMaxSizeLow : The low-order of the maximum size of the file mapping object.
   */
  VUResult vuapi Create(
    const std::wstring& MapName,
    ulong ulMaxSizeLow,
    ulong ulMaxSizeHigh = 0 // The file mapping object is equal to the current size of the file.
  );

  /**
   * Just for Named Shared Memory.
   * MapName: Eg: `Local/Global\\MyFileMappingObject`.
   */
  VUResult vuapi Open(const std::wstring& MapName, bool bInheritHandle = false);

  /**
   * With View Within a File, it can not be NULL.
   * With Named Shared Memory, it will be the size
   */
  void* vuapi View(
    ulong ulMaxFileOffsetLow   = 0,
    ulong ulMaxFileOffsetHigh  = 0,
    ulong ulNumberOfBytesToMap = 0 // The mapping extends from the specified offset to the end of the file mapping.
  );

  ulong vuapi GetFileSize();
  void vuapi Close();
};

/* --- Group : INI File --- */

class CINIFileA : public CLastError
{
private:
  std::string m_FilePath;
  std::string m_Section;

  void ValidFilePath();

public:
  CINIFileA() {};
  CINIFileA(const std::string& FilePath);
  CINIFileA(const std::string& FilePath, const std::string& Section);
  virtual ~CINIFileA() {};

  void SetCurrentFilePath(const std::string& FilePath);
  void SetCurrentSection(const std::string& Section);

  std::vector<std::string> vuapi ReadSection(const std::string& Section, ulong ulMaxSize = MAXBYTE);
  std::vector<std::string> vuapi ReadSection(ulong ulMaxSize = MAXBYTE);

  std::vector<std::string> vuapi ReadSectionNames(ulong ulMaxSize = MAXBYTE);

  int vuapi ReadInteger(const std::string& Section, const std::string& Key, int Default);
  bool vuapi ReadBool(const std::string& Section, const std::string& Key, bool Default);
  float vuapi ReadFloat(const std::string& Section, const std::string& Key, float Default);
  std::string vuapi ReadString(const std::string& Section, const std::string& Key, const std::string& Default);
  std::unique_ptr<uchar[]> vuapi ReadStruct(const std::string& Section, const std::string& Key, ulong ulSize);

  int vuapi ReadInteger(const std::string& Key, int Default);
  bool vuapi ReadBool(const std::string& Key, bool Default);
  float vuapi ReadFloat(const std::string& Key, float Default);
  std::string vuapi ReadString(const std::string& Key, const std::string& Default);
  std::unique_ptr<uchar[]> vuapi ReadStruct(const std::string& Key, ulong ulSize);

  bool vuapi WriteInteger(const std::string& Section, const std::string& Key, int Value);
  bool vuapi WriteBool(const std::string& Section, const std::string& Key, bool Value);
  bool vuapi WriteFloat(const std::string& Section, const std::string& Key, float Value);
  bool vuapi WriteString(const std::string& Section, const std::string& Key, const std::string& Value);
  bool vuapi WriteStruct(const std::string& Section, const std::string& Key, void* pStruct, ulong ulSize);

  bool vuapi WriteInteger(const std::string& Key, int Value);
  bool vuapi WriteBool(const std::string& Key, bool Value);
  bool vuapi WriteFloat(const std::string& Key, float Value);
  bool vuapi WriteString(const std::string& Key, const std::string& Value);
  bool vuapi WriteStruct(const std::string& Key, void* pStruct, ulong ulSize);
};

class CINIFileW : public CLastError
{
private:
  std::wstring m_FilePath;
  std::wstring m_Section;

  void ValidFilePath();

public:
  CINIFileW() {};
  CINIFileW(const std::wstring& FilePath);
  CINIFileW(const std::wstring& FilePath, const std::wstring& Section);
  virtual ~CINIFileW() {};

  void SetCurrentFilePath(const std::wstring& FilePath);
  void SetCurrentSection(const std::wstring& Section);

  std::vector<std::wstring> vuapi ReadSection(const std::wstring& Section, ulong ulMaxSize = MAXBYTE);
  std::vector<std::wstring> vuapi ReadSection(ulong ulMaxSize = MAXBYTE);

  std::vector<std::wstring> vuapi ReadSectionNames(ulong ulMaxSize = MAXBYTE);

  int vuapi ReadInteger(const std::wstring& Section, const std::wstring& Key, int Default);
  bool vuapi ReadBool(const std::wstring& Section, const std::wstring& Key, bool Default);
  float vuapi ReadFloat(const std::wstring& Section, const std::wstring& Key, float Default);
  std::wstring vuapi ReadString(const std::wstring& Section, const std::wstring& Key, const std::wstring& Default);
  std::unique_ptr<uchar[]> vuapi ReadStruct(const std::wstring& Section, const std::wstring& Key, ulong ulSize);

  int vuapi ReadInteger(const std::wstring& Key, int Default);
  bool vuapi ReadBool(const std::wstring& Key, bool Default);
  float vuapi ReadFloat(const std::wstring& Key, float Default);
  std::wstring vuapi ReadString(const std::wstring& Key, const std::wstring& Default);
  std::unique_ptr<uchar[]> vuapi ReadStruct(const std::wstring& Key, ulong ulSize);

  bool vuapi WriteInteger(const std::wstring& Section, const std::wstring& Key, int Value);
  bool vuapi WriteBool(const std::wstring& Section, const std::wstring& Key, bool Value);
  bool vuapi WriteFloat(const std::wstring& Section, const std::wstring& Key, float Value);
  bool vuapi WriteString(const std::wstring& Section, const std::wstring& Key, const std::wstring& Value);
  bool vuapi WriteStruct(const std::wstring& Section, const std::wstring& Key, void* pStruct, ulong ulSize);

  bool vuapi WriteInteger(const std::wstring& Key, int Value);
  bool vuapi WriteBool(const std::wstring& Key, bool Value);
  bool vuapi WriteFloat(const std::wstring& Key, float Value);
  bool vuapi WriteString(const std::wstring& Key, const std::wstring& Value);
  bool vuapi WriteStruct(const std::wstring& Key, void* pStruct, ulong ulSize);
};

/* --- Group : Registry --- */

class CRegistryX : public CLastError
{
public:
  CRegistryX()
  {
    m_LastErrorCode = ERROR_SUCCESS;
  };

  virtual ~CRegistryX() {};

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
private:
  std::string m_SubKey;

public:
  CRegistryA();
  CRegistryA(eRegRoot RegRoot);
  CRegistryA(eRegRoot RegRoot, const std::string& SubKey);
  virtual ~CRegistryA() {};

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
  std::vector<std::string> vuapi ReadMultiString(const std::string& ValueName, const std::vector<std::string> Default);
  std::string vuapi ReadExpandString(const std::string& ValueName, const std::string& Default);
  std::unique_ptr<uchar[]> vuapi ReadBinary(const std::string& ValueName, const void* pDefault);
};

class CRegistryW : public CRegistryX
{
private:
  std::wstring m_SubKey;

public:
  CRegistryW();
  CRegistryW(eRegRoot RegRoot);
  CRegistryW(eRegRoot RegRoot, const std::wstring& SubKey);
  virtual ~CRegistryW() {};

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
  std::vector<std::wstring> vuapi ReadMultiString(const std::wstring& ValueName, const std::vector<std::wstring> Default);
  std::wstring vuapi ReadExpandString(const std::wstring& ValueName, const std::wstring& Default);
  std::unique_ptr<uchar[]> vuapi ReadBinary(const std::wstring& ValueName, const void* Default);
protected:
};

/* --- Group : Critical Section --- */

class CCriticalSection
{
private:
  TCriticalSection m_CriticalSection;

public:
  CCriticalSection() {};
  virtual ~CCriticalSection() {};

  void vuapi Init();
  void vuapi Enter();
  void vuapi Leave();
  void vuapi Destroy();

  TCriticalSection& vuapi GetCurrentSection();
};

/* --- Group : Stop Watch --- */

class CStopWatch
{
public:
  typedef std::pair<unsigned long, float> TDuration;

public:
  CStopWatch();
  ~CStopWatch();
  void Start(bool reset = true);
  const TDuration Stop();
  const TDuration Duration();

private:
  bool m_Reset;
  float m_Duration;
  std::clock_t m_Count, m_Delta;
  std::vector<std::clock_t> m_DeltaHistory;
};

/* --- Group : PE File --- */

typedef struct
{
  ulong IIDID;
  std::string Name;
  PImportDescriptor pIID;
} TExIID, *PExIID;

typedef struct
{
  ulong IIDID;
  std::string Name;
  //ulong NumberOfFuctions;
} TDLLInfo, *PDLLInfo;

template<typename T>
struct TFunctionInfoT
{
  ulong IIDID;
  std::string Name;
  T Ordinal;
  ushort Hint;
  T RVA;
};

typedef TFunctionInfoT<ulong32> TFunctionInfo32;
typedef TFunctionInfoT<ulong64> TFunctionInfo64;

typedef enum _IMPORTED_FUNCTION_FIND_BY
{
  IFFM_HINT,
  IFFM_NAME,
} eImportedFunctionFindMethod;

template <typename T>
class CPEFileTX
{
public:
  CPEFileTX();
  virtual ~CPEFileTX();

  void* vuapi GetpBase();
  TPEHeaderT<T>* vuapi GetpPEHeader();

  T vuapi RVA2Offset(T RVA);
  T vuapi Offset2RVA(T Offset);

  std::vector<PSectionHeader>& vuapi GetSetionHeaderList(bool Reget = false);
  std::vector<PImportDescriptor>& vuapi GetImportDescriptorList(bool Reget = false);
  virtual std::vector<TExIID>& vuapi GetExIIDList();
  virtual std::vector<TDLLInfo> vuapi GetDLLInfoList(bool Reget = false);
  virtual std::vector<TFunctionInfoT<T>> vuapi GetFunctionInfoList(bool Reget = false); // Didn't include import by index
  virtual TDLLInfo vuapi FindImportedDLL(const std::string& DLLName);
  virtual TFunctionInfoT<T> vuapi FindImportedFunction(const std::string& FunctionName);
  virtual TFunctionInfoT<T> vuapi FindImportedFunction(const ushort FunctionHint);
  virtual TFunctionInfoT<T> vuapi FindImportedFunction(
    const TFunctionInfoT<T>& FunctionInfo,
    eImportedFunctionFindMethod Method
  );

protected:
  bool m_Initialized;

  void* m_pBase;

  TDosHeader* m_pDosHeader;
  TPEHeaderT<T>* m_pPEHeader;

private:
  T m_OrdinalFlag;

  std::vector<PSectionHeader> m_SectionHeaderList;
  std::vector<PImportDescriptor> m_ImportDescriptorList;
  std::vector<TExIID> m_ExIDDList;
  std::vector<TFunctionInfoT<T>> m_FunctionInfoList;
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

  VUResult vuapi Parse(const std::wstring& PEFilePath = L"");

private:
  std::wstring m_FilePath;
  CFileMappingW m_FileMap;
};

/*-------------------- The definition of common structure(s) which compatible both ANSI & UNICODE -------------------*/

#ifdef _UNICODE
#define TFSObject TFSObjectW
#else
#define TFSObject TFSObjectA
#endif

/*---------------------- The definition of common Class(es) which compatible both ANSI & UNICODE --------------------*/

#ifdef _UNICODE
#define CGUID CGUIDW
#define CDynHook CDynHookW
#define CService CServiceW
#define CLibrary CLibraryW
#define CFileSystem CFileSystemW
#define CFileMapping CFileMappingW
#define CINIFile CINIFileW
#define CRegistry CRegistryW
#define CPEFileT CPEFileTW
#else
#define CGUID CGUIDA
#define CDynHook CDynHookA
#define CService CServiceA
#define CLibrary CLibraryA
#define CFileSystem CFileSystemA
#define CFileMapping CFileMappingA
#define CINIFile CIniFileA
#define CRegistry CRegistryA
#define CPEFileT CPEFileTA
#endif

} // namespace vu

#endif // VUTILS_H