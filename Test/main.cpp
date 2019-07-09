// main.cpp : Defines the entry point for the console application.
//

/* MinGW build EXE with static library
G++ main.cpp -std=c++0x -lVutils -o Test.exe && Test.exe
G++ main.cpp -std=c++0x -lVutils -DVU_SOCKET_ENABLED -lws2_32 -DVU_GUID_ENABLED -lrpcrt4 -o Test.exe && Test.exe
*/

// #include "stdafx.h"

/*
#ifdef _UNICODE
#undef _UNICODE
#endif*/

#include <Windows.h>
#include <tchar.h>
#include <conio.h>
#include <ctime>
#include <limits>
#include <memory>
#include <cstdio>
#include <limits>
#include <algorithm>
#include <iostream>

// Remove min & max macros
#ifdef min
#undef minx
#endif
#ifdef max
#undef max
#endif

#include <vu.h>

#define SEPERATOR() std::tcout << _T("----------------------------------------") << std::endl;

typedef int (WINAPI *PfnMessageBoxA)(HWND   hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT   uType);
typedef int (WINAPI *PfnMessageBoxW)(HWND   hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT   uType);

PfnMessageBoxA pfnMessageBoxA = nullptr;
PfnMessageBoxW pfnMessageBoxW = nullptr;

int WINAPI HfnMessageBoxA(HWND   hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT   uType)
{
  lpText = "MessageBoxA -> Hooked";
  return pfnMessageBoxA(hWnd, lpText, lpCaption, uType);
}

int WINAPI HfnMessageBoxW(HWND   hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT   uType)
{
  lpText = L"MessageBoxW -> Hooked";
  return pfnMessageBoxW(hWnd, lpText, lpCaption, uType);
}

/* MessageBoxA
00000000778F12B8 | 48 83 EC 38              | sub rsp,38                                                                 |
00000000778F12BC | 45 33 DB                 | xor r11d,r11d                                                              |
00000000778F12BF | 44 39 1D 76 0E 02 00     | cmp dword ptr ds:[77912135],r11d                                           |
00000000778F12C6 | 74 2E                    | je user32.778F12F6                                                         |
00000000778F12C8 | 65 48 8B 04 25 30 00 00  | mov rax,qword ptr gs:[30]                                                  |
00000000778F12D1 | 4C 8B 50 48              | mov r10,qword ptr ds:[rax+48]                                              |
---
000000013F0D2888 | 44 39 1C 25 35 21 91 77  | cmp dword ptr ds:[77912135],r11d                                           |
*/

int _tmain(int argc, _TCHAR* argv[])
{
  // Determine Processor Architecture
  vu::eProcessorArchitecture arch = vu::GetProcessorArchitecture();
  if (arch == vu::PA_X64)
  {
    // Determine Wow64
    std::string s;
    s.clear();
    vu::eWow64 result = vu::IsWow64();
    if (result == vu::eWow64::WOW64_YES)
    {
      s.assign("WOW64");
    }
    else if (result == vu::WOW64_NO)
    {
      s.assign("Non-WOW64");
    }
    else
    {
      s.assign("Unknown");
    }
    std::cout << vu::FormatA("Windows 64-bit (%s)", s.c_str()) << std::endl;
  }
  else if (arch == vu::PA_X86)
  {
    std::tcout << _T("Windows 32-bit") << std::endl;
  }
  else
  {
    std::tcout << _T("Unknown") << std::endl;
  }

  // Determine encoding
#ifdef _UNICODE
  printf("Encoding: UNICODE\n");
#else
  printf("Encoding: ANSI\n");
#endif // _UNICODE

  // Seperate
  std::cout << std::endl;
  std::tcout << _T("--- *.* ---") << std::endl;

  // Misc
  /*vu::Box(vu::GetConsoleWindowHandle(), _T("I'm %s. I'm %d years old."), _T("Vic P"), 26);
  vu::Msg(_T("I'm %s. I'm %d years old."), _T("Vic P"), 26);

  std::tcout << vu::Fmt(_T("I'm %s. I'm %d years old. (A)"), _T("Vic P"), 26) << std::endl;

  std::tcout  << vu::LowerString(_T("I Love You")) << std::endl;
  std::tcout << vu::UpperString(_T("I Love You")) << std::endl;

  std::tcout << vu::TrimString(_T("   THIS IS A TRIM STRING   ")) << std::endl;

  std::tcout << vu::LastError() << std::endl;

  std::vector<std::tstring>  l;

  l.clear();
  l = vu::SplitString(_T("THIS IS A SPLIT STRING"), _T(" "));
  for (auto e: l) std::tcout << e << _T("|");
  std::tcout << std::endl;

  l.clear();
  l = vu::MultiStringToList(_T("THIS\0IS\0A\0MULTI\0STRING\0\0"));
  for (auto& e: l) std::tcout << e << _T("|");
  std::tcout << std::endl;

  std::tcout << vu::DateTimeToString(time(NULL)) << std::endl;

  std::cout  << vu::ToStringA(L"THIS IS A WIDE STRING") << std::endl;
  std::wcout << vu::ToStringW("THIS IS AN ANSI STRING") << std::endl;

  std::tstring envValue = vu::GetEnviroment(_T("PATH"));
  auto env = vu::SplitString(envValue, _T(";"));
  for (auto e: env) {
    std::tcout << e << std::endl;
  }

  std::vector<vu::ulong> PIDs;
  PIDs.clear();

  PIDs = vu::NameToPID(_T("Explorer.exe"));
  for (auto& PID: PIDs) {
  std::tcout << PID << std::endl;
  }

  if (!PIDs.empty()) {
  std::tcout  << vu::PIDToName(*PIDs.begin()) << std::endl;
  }*/

  // Data Type Information
  /*std::tcout << (FundamentalTypeIsSigned(vu::UIntPtr) ? _T("Signed") : _T("Unsigned")) << std::endl;
  std::tcout << (FundamentalTypeIsExact(vu::UIntPtr) ? _T("Exact") : _T("Non-Exact")) << std::endl;
  std::tcout << FundamentalTypeGetDigits(vu::UIntPtr) << std::endl;
  std::tcout << std::hex << FundamentalTypeGetLowest(vu::UIntPtr) << std::endl;
  std::tcout << std::hex << FundamentalTypeGetHighest(vu::UIntPtr) << std::endl;*/

  // File/Directory
  /*const std::tstring FILE_DIR  =  _T("C:\\Intel\\Logs\\");
  const std::tstring FILE_PATH =  _T("C:\\Intel\\Logs\\IntelGFX.log");

  std::tcout  << (vu::IsDirectoryExists(FILE_DIR) ? _T("Directory is exist") : _T("Directory isn't exist")) << std::endl;
  std::tcout  << (vu::IsFileExists(FILE_PATH) ? _T("File is exist") : _T("File isn't exist")) << std::endl;

  std::tcout  << vu::FileType(FILE_PATH) << std::endl;

  std::tcout  << vu::ExtractFilePath(FILE_PATH) << std::endl;
  std::tcout  << vu::ExtractFilePath(FILE_PATH, false) << std::endl;

  std::tcout  << vu::ExtractFileName(FILE_PATH) << std::endl;
  std::tcout  << vu::ExtractFileName(FILE_PATH, false) << std::endl;

  std::tcout  << vu::GetCurrentDirectory() << std::endl;
  std::tcout  << vu::GetCurrentDirectory(false) << std::endl;

  std::tcout  << vu::GetCurFilePath() << std::endl;
  // Not complete
  bool process64Bit = false;
  std::vector<vu::ulong> explorersPID = vu::NameToPID(process64Bit ? _T("explorer.exe") : _T("IDMan.exe"));
  if (explorersPID.size() == 0) {
    return 1;
  }

  vu::ulong explorerPID = *explorersPID.begin();

  std::cout  << std::hex << vu::RemoteGetModuleHandleA(explorerPID, "kernel32.dll") << std::endl;
  std::tcout << vu::LastError().c_str() << std::endl;

  std::wcout << std::hex << vu::RemoteGetModuleHandleW(explorerPID, L"kernel32.dll") << std::endl;
  std::tcout << vu::LastError().c_str() << std::endl;*/

  // CSocket
  /*vu::CSocket socket;

  const std::string REQ_HOST = "ipv4.download.thinkbroadband.com";
  std::string REQ_CONTENT;
  REQ_CONTENT.clear();
  REQ_CONTENT.append("GET /5MB.zip\r\n");
  REQ_CONTENT.append("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,* / *;q=0.8\r\n");
  REQ_CONTENT.append("Accept-Encoding: gzip, deflate\r\n");
  REQ_CONTENT.append("Accept-Language: en-US,en;q=0.5\r\n");
  REQ_CONTENT.append("Connection: keep-alive\r\n");
  REQ_CONTENT.append("DNT: 1\r\n");
  REQ_CONTENT.append("Host: ipv4.download.thinkbroadband.com\r\n");
  REQ_CONTENT.append("Upgrade-Insecure-Requests: 1\r\n");
  REQ_CONTENT.append("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:59.0) Gecko/20100101 Firefox/59.0\r\n");
  REQ_CONTENT.append("\r\n");

  if (socket.Socket(vu::SAF_INET, vu::ST_STREAM) != vu::VU_OK)
  {
    std::tcout << _T("Socket -> Create -> Failed") << std::endl;
    return 1;
  }

  std::tcout << _T("Socket -> Create -> Success") << std::endl;

  if (socket.Connect(REQ_HOST, 80) != vu::VU_OK)
  {
    std::tcout << _T("Socket -> Connect -> Failed") << std::endl;
    return 1;
  }

  std::tcout << _T("Socket -> Connect -> Success") << std::endl;

  // if (socket.Send(REQ_CONTENT.data(), int(REQ_CONTENT.length())) == SOCKET_ERROR)
  // {
  //   std::tcout << _T("Socket -> Send -> Failed") << std::endl;
  //   return 1;
  // }

  // vu::CFileSystemA src7z;
  // src7z.Init("5MB.zip", vu::eFileModeFlags::FM_CREATEALWAY);
  // if (src7z.IsReady())
  // {
  //   vu::CBinary D(1024);
  //   vu::IResult N = -1, nRecvBytes = 0;
  //   do
  //   {
  //     N = socket.Recv(D);
  //     if (N > 0)
  //     {
  //       src7z.Write(D.GetpData(), D.GetUsedSize());
  //       nRecvBytes += N;
  //     }
  //   }
  //   while (N > 0);
  //   src7z.Close();
  // }

  if (!socket.Close())
  {
    std::tcout << _T("Socket -> Close -> Failed") << std::endl;
    return 1;
  }

  std::tcout << _T("Socket -> Close -> Success") << std::endl;*/

  // CDynHook
  /*vu::CDynHook API[2];

  if (!VU_ATTACH_API(API[0], user32.dll, MessageBoxA))
  {
    std::cout << "API::ATTACH -> MessageBoxA -> Failure" << std::endl;
    return 1;
  }

  std::cout << "API::ATTACH -> MessageBoxA -> Success" << std::endl;

  if (!VU_ATTACH_API(API[1], user32.dll, MessageBoxW))
  {
    std::cout << "API::ATTACH -> MessageBoxW -> Failure" << std::endl;
    return 1;
  }

  std::cout << "API::ATTACH -> MessageBoxW -> Success" << std::endl;

  MessageBoxA(vu::GetConsoleWindowHandle(), "The first message.", "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindowHandle(), L"The first message.", L"W", MB_OK);

  if (!VU_DETACH_API(API[0], user32.dll, MessageBoxA))
  {
    std::cout << "API::DETACH -> MessageBoxA -> Failure" << std::endl;
  }

  std::cout << "API::DETACH -> MessageBoxA -> Success" << std::endl;

  if (!VU_DETACH_API(API[1], user32.dll, MessageBoxW))
  {
    std::cout << "API::DETACH -> MessageBoxW -> Failure" << std::endl;
  }

  std::cout << "API::DETACH -> MessageBoxW -> Success" << std::endl;

  MessageBoxA(vu::GetConsoleWindowHandle(), "The second message.", "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindowHandle(), L"The second message.", L"W", MB_OK);*/

  // CINIFileA
  /*vu::CINIFileA ini(vu::GetCurFilePath() + _T(".ini"));

  ini.SetCurrentSection(_T("Section"));

  std::vector<std::tstring> l;
  l.clear();

  l = ini.ReadSectionNames();
  for (auto i = l.begin(); i != l.end(); i++)
  {
    _tprintf(_T("[%s]\n"), (*i).c_str());
  }

  std::cout << std::endl;

  l = ini.ReadSection();
  for (auto i = l.begin(); i != l.end(); i++)
  {
    _tprintf(_T("[%s]\n"), (*i).c_str());
  }

  printf("\n");

  if (ini.WriteInteger(_T("KeyInt"), 702))
  {
    _tprintf(_T("KeyInt = %d\n"), ini.ReadInteger(_T("KeyInt"), 0));
  }

  if (ini.WriteBool(_T("KeyBool"), true))
  {
    _tprintf(_T("KeyBool = %s\n"), ini.ReadBool(_T("KeyBool"), 0) ? _T("True") : _T("False"));
  }

  if (ini.WriteFloat(_T("KeyFloat"), 7.02f))
  {
    _tprintf(_T("KeyFloat = %.2f\n"), ini.ReadFloat(_T("KeyFloat"), 0.F));
  }

  if (ini.WriteString(_T("KeyString"), _T("Vic P.")))
  {
    std::tstring s = ini.ReadString(_T("KeyString"), _T(""));
    _tprintf(_T("KeyString = '%s'\n"), s.c_str());
  }

  struct TStruct
  {
    char a;
    int b;
    float c;
  } Input =
  {
    'X',
    702,
    7.02f
  };

  if (ini.WriteStruct(_T("KeyStruct"), &Input, sizeof(Input)))
  {
    std::shared_ptr<void> p = ini.ReadStruct(_T("KeyStruct"), sizeof(TStruct));
    TStruct * Output = (TStruct*)p.get();
    _tprintf(_T("Value = [%c, %d, %.2f]\n"), Output->a, Output->b, Output->c);
  }*/

  // CRegistry
  /*vu::CRegistry reg(vu::HKLM, _T("SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting")/ *_T("SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting")* /);
  if (!reg.KeyExists())
  {
    std::tcout << _T("Reg -> Exist -> Failed") << vu::LastError(reg.GetLastErrorCode()) << std::endl;
    return 1;
  }

  if (!reg.OpenKey())
  {
    std::tcout << _T("Reg -> Open-> Failed") << vu::LastError(reg.GetLastErrorCode()) << std::endl;
    return 1;
  }

  reg.SetReflectionKey(vu::eRegReflection::RR_ENABLE);

  std::tcout << _T("Is Reflection Disabled ? ")
             << (reg.QueryReflectionKey() == vu::eRegReflection::RR_DISABLED ? _T("True") : _T("False"))
             << std::endl;

  std::vector<std::tstring> l;

  std::tcout << _T("\n[ErrorPort] -> ");
  std::tstring result = reg.ReadString(_T("ErrorPort"), _T("<No>"));
  std::tcout << result << std::endl;

  std::tcout << _T("\n[MS]") << std::endl;
  l.clear();
  l = reg.ReadMultiString(_T("MS"), l);
  for (auto e: l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << _T("\n[Keys]") << std::endl;
  l.clear();
  l = reg.EnumKeys();
  for (auto e : l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << _T("\n[Values]") << std::endl;
  l.clear();
  l = reg.EnumValues();
  for (auto e : l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << std::endl;

  / *reg.WriteBinary(_T("RegBinary"), (void*)_T("1234567890"), 10 * sizeof(vu::tchar));
  std::tcout << _T("RegBinary\t") << (vu::tchar*)reg.ReadBinary(_T("RegBinary"), nullptr).get() << std::endl;

  reg.WriteBool(_T("RegBool"), true);
  std::tcout << _T("RegBool\t") << reg.ReadBool(_T("RegBool"), false) << std::endl;

  reg.WriteExpandString(_T("RegExpandString"), _T("%CatEngine%"));
  std::tcout << _T("RegExpandString\t") << reg.ReadExpandString(_T("RegExpandString"), _T("")) << std::endl;

  reg.WriteFloat(_T("RegFloat"), 16.09F);
  std::tcout << _T("RegFloat\t") << reg.ReadFloat(_T("RegFloat"), 7.02F) << std::endl;

  reg.WriteInteger(_T("RegInt"), 1609);
  std::tcout << _T("RegInt\t") << reg.ReadInteger(_T("RegInt"), 702) << std::endl;

  l.clear();
  l.push_back(_T("String 1"));
  l.push_back(_T("String 2"));
  l.push_back(_T("String 3"));
  reg.WriteMultiString(_T("RegMultiString"), l);
  l.clear();
  std::tcout << _T("RegMultiString") << std::endl;
  l = reg.ReadMultiString(_T("RegMultiString"), l);
  for (auto e : l) {
    std::tcout << _T("\t") << e << std::endl;
  }

  reg.WriteString(_T("RegString"), _T("This is a string"));
  std::tcout << _T("RegString\t") << reg.ReadString(_T("RegString"), _T("")) << std::endl;* /

  if (!reg.CloseKey())
  {
    std::tcout << _T("Reg -> Close ->Failed") << vu::LastError(reg.GetLastErrorCode()) << std::endl;
  }*/

  // CService
  /*if (!vu::IsAdministrator())
  {
    std::tcout << _T("You are not Administrator") << std::endl;
  }
  else
  {
    std::tcout << _T("You are Administrator") << std::endl;

    // CService
    vu::CService srv;

    if (srv.Init())
    {
      std::tcout << _T("Service -> Initialize -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Initialize -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;

    std::tcout << _T("Enter to create service...") << std::endl; getch();

    if (srv.Create(_T("C:\\Windows\\System32\\drivers\\VBoxUSBMon.sys")))
    {
      std::tcout << _T("Service -> Create -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Create -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;

    std::tcout << _T("Enter to start service...") << std::endl; getch();

    if (srv.Start())
    {
      std::tcout << _T("Service -> Start -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Start -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;

    std::tcout << _T("Enter to stop service...") << std::endl; getch();

    if (srv.Stop())
    {
      std::tcout << _T("Service -> Stop -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Stop -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;

    std::tcout << _T("Enter to close service...") << std::endl; getch();

    if (srv.Close())
    {
      std::tcout << _T("Service -> Close -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Close -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;

    if (srv.Destroy())
    {
      std::tcout << _T("Service -> Release -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Release -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;
  }*/

  // CFileSystem
  /*const std::tstring FILE_NAME = _T("Test.txt");
  vu::CFileSystem file;

  std::string s = "This is a test string";
  file.Init(FILE_NAME, vu::FM_CREATEALWAY);
  file.Write(s.c_str(), (vu::ulong)s.length());
  file.Close();

  char D[MAXBYTE] = {0};
  file.Init(FILE_NAME, vu::FM_OPENEXISTING);
  file.Read(&D, sizeof(D));
  file.Close();

  std::tcout << _T("D = ") << D << std::endl;

  vu::CFileSystem::Iterate(_T("C:\\Intel\\Logs"), _T("*.*"), [](const vu::TFSObject& FSObject) -> bool
  {
    std::tcout << FSObject.Directory << _T(" | ") << FSObject.Name << _T(" | ") << FSObject.Size << std::endl;
    return true;
  });*/

  // CFileMapping
  /*vu::CFileMapping fm;

  void* p = nullptr;

  if (fm.Init() != vu::VU_OK)
  {
    std::tcout << _T("Init -> Failed") << vu::LastError(fm.GetLastErrorCode()) << std::endl;
  }
  if (fm.Create(_T("Global\\Sample"), KB) != vu::VU_OK)
  {
    std::tcout << _T("Create -> Failed") << vu::LastError(fm.GetLastErrorCode()) << std::endl;
  }
  p = fm.View();
  if (p == nullptr)
  {
    std::tcout << _T("View -> Failed") << vu::LastError(fm.GetLastErrorCode()) << std::endl;
  }

  if (fm.Init(true, _T("C:\\Intel\\Logs\\IntelGFX.log")) != vu::VU_OK)
  {
    std::tcout << _T("Init -> Failed ") << vu::LastError(fm.GetLastErrorCode()) << std::endl;
  }
  if (fm.Create(_T(""), fm.GetFileSize()) != vu::VU_OK)
  {
    std::tcout << _T("Create -> Failed ") << vu::LastError(fm.GetLastErrorCode()) << std::endl;
  }
  p = fm.View();
  if (p == nullptr)
  {
    std::tcout << _T("View -> Failed ") << vu::LastError(fm.GetLastErrorCode()) << std::endl;
  }

  std::tcout << std::hex << _T("File Mapping at ") << p << std::endl;

  if (p != nullptr && fm.GetFileSize() != -1)
  {
    vu::HexDump(p, fm.GetFileSize());
  }*/

  // CPEFile
  /*// vu::CPEFileT<vu::pe32> pe(_T("C:\\Program Files\\Process Hacker 2\\x86\\ProcessHacker.exe"));
  vu::CPEFileT<vu::pe64> pe(_T("C:\\Program Files\\Process Hacker 2\\ProcessHacker.exe"));

  vu::VUResult result = pe.Parse();
  if (result != vu::VU_OK)
  {
    std::tstring s;
    if (result == 8)
    {
      s = _T(" (Used wrong type data for the current PE file format)");
    }
    if (result == 9)
    {
      s = _T(" (The curent type data was not supported)");
    }
    std::tcout << _T("PE -> Parse -> Failure") << vu::LastError() << s << std::endl;
    return 1;
  }

  void* pBase = pe.GetpBase();
  if (pBase == nullptr)
  {
    std::tcout << _T("PE -> GetpBase -> Failure") << vu::LastError() << std::endl;
    return 1;
  }

  SEPERATOR()

  auto sections = pe.GetSetionHeaderList();
  if (sections.size() == 0)
  {
    std::tcout << _T("PE -> GetSetionHeaderList -> Failure") << vu::LastError() << std::endl;
    return 1;
  }

  for (auto section: sections)
  {
    printf(
      "%+10s %08X %08X %08X %08X\n",
      section->Name,
      section->PointerToRawData,
      section->SizeOfRawData,
      section->VirtualAddress,
      section->Misc.VirtualSize
    );
  }

  SEPERATOR()

  auto pPEHeader = pe.GetpPEHeader();
  if (pPEHeader == nullptr)
  {
    std::tcout << _T("PE -> pPEHeader -> NULL") << std::endl;
    return 1;
  }

  std::cout << std::hex << pe.Offset2RVA(0x001A02FF) << std::endl;
  std::cout << std::hex << pe.RVA2Offset(0x001AA4FF) << std::endl;

  if (true)
  {
    return 0;
  }

  auto IIDs = pe.GetImportDescriptorList();
  if (IIDs.size() == 0)
  {
    std::tcout << _T("PE -> GetImportDescriptorList -> Failure") << vu::LastError() << std::endl;
    return 1;
  }

  for (auto IID: IIDs)
  {
    printf("%+20s %08X %08X %08X %08X\n",
      ((char*)pBase + pe.RVA2Offset(IID->Name)),
      IID->Name,
      IID->FirstThunk,
      IID->OriginalFirstThunk,
      IID->Characteristics
    );
  }

  SEPERATOR()

  auto DLLs = pe.GetDLLInfoList();
  if (DLLs.size() == 0)
  {
    std::tcout << _T("PE -> GetDLLList -> Failure") << vu::LastError() << std::endl;
    return 1;
  }

  for (auto DLL: DLLs)
  {
    printf("%08X, '%s'\n", DLL.IIDID, DLL.Name.c_str());
  }

  SEPERATOR()

  auto fns = pe.GetFunctionInfoList();
  if (fns.size() == 0)
  {
    std::tcout << _T("PE -> GetFunctionInfoList -> Failure") << vu::LastError() << std::endl;
    return 1;
  }

  for (auto e : fns)
  {
    auto s = vu::FormatA("IIDID = %08X, Hint = %04X, '%s'", e.IIDID, e.Hint, e.Name.c_str());
    std::cout << s << std::endl;
  }

  SEPERATOR()

  auto DLL = pe.FindImportedDLL("KERNEL32.DLL");
  if (DLL.Name != "")
  {
    printf("%08X, '%s'\n", DLL.IIDID, DLL.Name.c_str());
  }

  SEPERATOR()

  auto fn = pe.FindImportedFunction("GetLastError");
  if (fn.RVA != 0)
  {
    printf("%08X, %04X, '%s'\n", fn.IIDID, fn.Hint, fn.Name.c_str());
  }*/

  // CGUID
  /*for (auto& e : std::vector<int>(5))
  {
    vu::CGUID guid(true);
    std::tcout << guid.AsString() << std::endl;
    std::tcout << vu::CGUID::ToString(vu::CGUID::ToGUID(guid.AsString())) << std::endl;
  }*/

  // Singleton
  /*class CDog : public vu::CSingletonT<CDog>
  {
  public:
    void Speak()
    {
      std::cout << VU_FUNCTION_INFO << std::endl;
    };
  };

  class CCat : public vu::CSingletonT<CCat>
  {
  public:
    void Speak()
    {
      std::cout << VU_FUNCTION_INFO << std::endl;
    };
  };

  CDog::Instance()->Speak();
  CCat::Instance()->Speak();*/

  return 0;
}
