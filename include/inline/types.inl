/**
 * @file   types.inl
 * @author Vic P.
 * @brief  Inline for Types
 */

typedef int                 VUResult;
typedef int                 IResult;

typedef TCHAR               tchar;
typedef wchar_t             wchar;

// 32-bit Default Data Types

typedef unsigned char       byte;
typedef unsigned char       uchar;
typedef unsigned short      ushort;
typedef unsigned short      ushort;
typedef unsigned int        uint;
typedef unsigned long       ulong;
typedef unsigned long long  ulonglong;

// 86-64 Integer Data Types

typedef char                int8;
typedef unsigned char       uint8;
typedef short               int16;
typedef unsigned short      uint16;
typedef int                 int32;
typedef unsigned int        uint32;
typedef __int64             int64;
typedef unsigned __int64    uint64;
typedef int                 long32;
typedef unsigned int        ulong32;
typedef __int64             long64;
typedef unsigned __int64    ulong64;

// 86-64 Data Types

typedef HALF_PTR            halfptr;
typedef UHALF_PTR           uhalfptr;
typedef INT_PTR             intptr;
typedef UINT_PTR            uintptr;
typedef LONG_PTR            longptr;
typedef ULONG_PTR           ulongptr;

typedef unsigned char       Byte;
typedef unsigned char       UChar;
typedef HALF_PTR            HalfPtr;
typedef UHALF_PTR           UHalfPtr;
typedef INT_PTR             IntPtr;
typedef UINT_PTR            UIntPtr;
typedef LONG_PTR            LongPtr;
typedef ULONG_PTR           ULongPtr;

// Enumerates

enum class eXSB
{
  LSB,
  MSB,
};

enum class eByteOrder
{
  LE,
  BE,
};
