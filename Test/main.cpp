// main.cpp : Defines the entry point for the console application.
//

/* MinGW build EXE with static library
G++ main.cpp -std=c++0x -lVutils -lgdi32 -o Test.exe && Test.exe
G++ main.cpp -std=c++0x -lVutils -DVU_SOCKET_ENABLED -lws2_32 -DVU_GUID_ENABLED -lrpcrt4 -lgdi32 -o Test.exe && Test.exe
*/

// #include "stdafx.h"

// #ifdef _UNICODE
// #undef _UNICODE
// #endif

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
#include <iomanip>
#include <cassert>

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
  auto arch = vu::GetProcessorArchitecture();
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

  // Separate
  std::cout << std::endl;
  std::tcout << _T("--- *.* ---") << std::endl;

  // Misc
  /*vu::Box(vu::GetConsoleWindow(), _T("I'm %s. I'm %d years old."), _T("Vic P"), 26);
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

  std::tcout << _T("Enviroment `PATH`") << std::endl;
  std::tstring envValue = vu::GetEnviroment(_T("PATH"));
  auto env = vu::SplitString(envValue, _T(";"));
  for (auto e: env) {
    std::tcout << '\t' << e << std::endl;
  }

  std::vector<vu::ulong> PIDs;
  PIDs.clear();

  PIDs = vu::NameToPID(_T("Explorer.exe"));
  for (auto& PID: PIDs) {
  std::tcout << PID << std::endl;
  }

  if (!PIDs.empty()) {
  std::tcout  << vu::PIDToName(*PIDs.begin()) << std::endl;
  }

  // std::vector<vu::ulong> pids;
  // pids = vu::NameToPID(_T("notepad.exe")); // 64-bit
  // pids = vu::NameToPID(_T("JRuler.exe"));  // 32-bit
  // assert(!pids.empty());
  // vu::InjectDLL(pids.back(), _T("path\\to\\32-bit-dll"), true);
  // vu::InjectDLL(pids.back(), _T("path\\to\\64-bit-dll"), true);

  static std::wstring LES[] = { // List Encoding Short
	  L"ANSI/UTF-8", L"UTF-8 BOM",
    L"Unicode", L"Unicode BE",
    L"Unicode BOM", L"Unicode BE BOM",
    L"UTF-32 LE BOM", L"UTF-32 BE BOM"
  };

  static std::wstring LEL[] = { // List Encoding Long
	  L"ANSI/UTF-8", L"UTF-8 BOM",
    L"UTF-16 Little Endian", L"UTF-16 Big Endian",
    L"UTF-16 Little Endian BOM", L"UTF-16 Big Endian BOM",
    L"UTF-32 Little Endian BOM", L"UTF-32 Big Endian BOM"
  };

  vu::CFileSystem::Iterate(_T("path\\to\\example"), _T("*.txt"), [](const vu::TFSObject& FSObject) -> bool
  {
    auto filePath = FSObject.Directory + FSObject.Name;
    vu::CFileSystem fs(filePath, vu::eFSModeFlags::FM_OPENEXISTING);
    auto data = fs.ReadContent();

    auto result = vu::DetermineEncodingType(data.GetpData(), data.GetSize());
    auto es = result == -1 ? L"Unknown" : LES[result];
    auto el = result == -1 ? L"Unknown" : LEL[result];
    std::wcout
      << std::left
      << std::setw(15) << es
      << " | "
      << std::setw(25) << el
      << " | "
      << FSObject.Name.c_str()
      << std::endl;

    return true;
  });

  auto type = vu::eStdByte::SI;
  auto digits = 2;
  std::tcout << vu::FormatBytes(912, type, digits) << std::endl; // B/Bi
  std::tcout << vu::FormatBytes(91234, type, digits) << std::endl; // KB/KiB
  std::tcout << vu::FormatBytes(9123456, type, digits) << std::endl; // MB/MiB
  std::tcout << vu::FormatBytes(9123456789, type, digits) << std::endl; // GB/GiB
  std::tcout << vu::FormatBytes(9123456789101, type, digits) << std::endl; // TB/TiB
  std::tcout << vu::FormatBytes(9123456789101213, type, digits) << std::endl; // PB/PiB
  std::tcout << vu::FormatBytes(9123456789101213145, type, digits) << std::endl; // EB/EiB*/

  // Math
  /*std::cout << "GCD : " << vu::GCD(3, 9, 27, 81) << std::endl;
  std::cout << "LCM : " << vu::LCM(3, 9, 27, 81) << std::endl;*/

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

  std::tcout  << vu::GetCurDirectory() << std::endl;
  std::tcout  << vu::GetCurDirectory(false) << std::endl;

  std::tcout  << vu::GetContainDirectory() << std::endl;
  std::tcout  << vu::GetContainDirectory(false) << std::endl;

  std::tcout  << vu::GetCurrentFilePath() << std::endl;

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
  /*const std::string REQ_HOST = "ipv4.download.thinkbroadband.com";
  std::string REQ_CONTENT;
  REQ_CONTENT.clear();
  REQ_CONTENT.append("GET /5MB.zip HTTP/1.1\r\n");
  REQ_CONTENT.append("Host: ipv4.download.thinkbroadband.com\r\n");
  REQ_CONTENT.append("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:72.0) Gecko/20100101 Firefox/72.0\r\n");
  REQ_CONTENT.append("Accept-Language: en-US,en;q=0.5\r\n");
  REQ_CONTENT.append("Accept-Encoding: gzip, deflate\r\n");
  REQ_CONTENT.append("Connection: keep-alive\r\n");
  REQ_CONTENT.append("DNT: 1\r\n");
  REQ_CONTENT.append("\r\n");

  vu::CSocket socket(vu::SAF_INET, vu::ST_STREAM);

  if (socket.Connect(REQ_HOST, 80) != vu::VU_OK)
  {
    std::tcout << _T("Socket -> Connect -> Failed") << std::endl;
    return 1;
  }

  std::tcout << _T("Socket -> Connect -> Success") << std::endl;

  if (socket.Send(REQ_CONTENT.data(), int(REQ_CONTENT.length())) == SOCKET_ERROR)
  {
    std::tcout << _T("Socket -> Send -> Failed") << std::endl;
    return 1;
  }

  vu::CBinary reponse(1024);
  auto N = socket.Recv(reponse);
  assert(N > 0);

  const std::string FirstResponse = (char*)reponse.GetpData();
  const std::string HttpHeaderEnd = "\x0D\x0A\x0D\x0A";
  const std::string HttpHeaderSep = "\x0D\x0A";

  auto l = vu::SplitStringA(FirstResponse, HttpHeaderEnd);
  assert(!l.empty());

  const auto& ResponseHeader = l.at(0) + HttpHeaderEnd;
  std::cout << "Response Header:" << std::endl;
  const auto& headers = vu::SplitStringA(ResponseHeader, HttpHeaderSep, true);
  for (auto& e : headers)
  {
    std::cout << "\t" << e << std::endl;
  }

  reponse.SetpData(
    (char*)reponse.GetpData() + ResponseHeader.length(),
    N - (int)ResponseHeader.length()
  );

  vu::CFileSystemA file("5MB.zip", vu::eFSModeFlags::FM_CREATEALWAY);
  assert(file.IsReady());
  file.Write(reponse.GetpData(), reponse.GetUsedSize());

  vu::IResult nRecvBytes = 0;
  do
  {
    N = socket.Recv(reponse);
    if (N > 0)
    {
      file.Write(reponse.GetpData(), reponse.GetUsedSize());
      nRecvBytes += N;
      std::cout
        << std::left
        << "Downloaded: "
        << std::setw(15)
        << vu::FormatBytesA(nRecvBytes)
        << '\r'
        << std::flush;
    }
  }
  while (N > 0);

  std::cout << std::endl;

  if (!socket.Close())
  {
    std::tcout << _T("Socket -> Close -> Failed") << std::endl;
    return 1;
  }

  std::tcout << _T("Socket -> Close -> Success") << std::endl;*/

  // CAPIHook
  /*vu::CAPIHook API[2];

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

  MessageBoxA(vu::GetConsoleWindow(), "The first message.", "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindow(), L"The first message.", L"W", MB_OK);

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

  MessageBoxA(vu::GetConsoleWindow(), "The second message.", "A", MB_OK);
  MessageBoxW(vu::GetConsoleWindow(), L"The second message.", L"W", MB_OK);*/

  // CWMHook
  /*// extern "C" __declspec(dllexport) LRESULT CALLBACK fnXProc(int nCode, WPARAM wParam, LPARAM lParam)
  // {
  //   return CallNextHookEx(nullptr, nCode, wParam, lParam);
  // }

  #define VU_STRINGIZE(s) #s

  #ifdef _WIN64
  #define EXE_NAME _T("x64dbg.exe")
  #define DLL_PATH _T("path\\to\\x64\\Test.WH.DLL.dll")
  #define UND(s) _T(VU_STRINGIZE(s))
  #else // _WIN32
  #define EXE_NAME _T("x32dbg.exe")
  #define DLL_PATH _T("path\\to\\x86\\Test.WH.DLL.dll")
  #define UND(s) VU_STRINGIZE(_##s##@12)
  #endif // _WIN64

  auto pids = vu::NameToPID(EXE_NAME);
  assert(!pids.empty());
  auto PID = pids.back();

  vu::CWMHook wh(PID, DLL_PATH);

  assert(wh.Start(WH_CBT, UND(fnCBTProc)) == vu::VU_OK);
  assert(wh.Start(WH_MOUSE, UND(fnMouseProc)) == vu::VU_OK);
  assert(wh.Start(WH_KEYBOARD, UND(fnKeyboardProc)) == vu::VU_OK);

  fgetchar();

  assert(wh.Stop(WH_CBT) == vu::VU_OK);
  assert(wh.Stop(WH_MOUSE) == vu::VU_OK);
  assert(wh.Stop(WH_KEYBOARD) == vu::VU_OK);*/

  // CINIFileA
  /*vu::CINIFile ini(vu::GetCurrentFilePath() + _T(".ini"));

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
  /*vu::CRegistry reg(vu::HKLM, _T("SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting")); // _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\Windows Error Reporting")
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

  reg.WriteBinary(_T("RegBinary"), (void*)_T("1234567890"), 10 * sizeof(vu::tchar));
  std::tcout << _T("RegBinary\t") << (vu::tchar*)reg.ReadBinary(_T("RegBinary"), nullptr).get() << std::endl;

  reg.WriteBool(_T("RegBool"), true);
  std::tcout << _T("RegBool\t") << reg.ReadBool(_T("RegBool"), false) << std::endl;

  reg.WriteExpandString(_T("RegExpandString"), _T("%TMP%"));
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
  std::tcout << _T("RegString\t") << reg.ReadString(_T("RegString"), _T("")) << std::endl;

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
  {
    std::string s = "This is a test string";
    vu::CFileSystem file(FILE_NAME, vu::FM_CREATEALWAY);
    file.Write(s.c_str(), (vu::ulong)s.length());
  }
  {
    char D[MAXBYTE] = { 0 };
    vu::CFileSystem file(FILE_NAME, vu::FM_OPENEXISTING);
    file.Read(&D, sizeof(D));

    std::tcout << _T("D = ") << D << std::endl;
  }
  vu::CFileSystem::Iterate(_T("C:\\Intel\\Logs"), _T("*.*"), [](const vu::TFSObject& FSObject) -> bool
  {
    std::tcout << FSObject.Directory << _T(" | ") << FSObject.Name << _T(" | ") << FSObject.Size << std::endl;
    return true;
  });*/

  // CFileMapping
  /*vu::CFileMapping fm;

  void* p = nullptr;

  // if (fm.CreateNamedSharedMemory(_T("Global\\Sample"), KB) != vu::VU_OK)
  // {
  //   std::tcout << _T("Create -> Failed") << vu::LastError(fm.GetLastErrorCode()) << std::endl;
  // }

  if (fm.CreateWithinFile(_T("C:\\Intel\\Logs\\IntelGFX.log")) != vu::VU_OK)
  {
    std::tcout << _T("Create -> Failed ") << vu::LastError(fm.GetLastErrorCode()) << std::endl;
  }

  p = fm.View();
  if (p == nullptr)
  {
    std::tcout << _T("View -> Failed ") << vu::LastError(fm.GetLastErrorCode()) << std::endl;
  }

  std::tcout << std::hex << _T("File Mapping at ") << p << std::endl;

  if (p != nullptr)
  {
    if (fm.GetFileSize() != -1)
    {
      vu::HexDump(p, fm.GetFileSize());
    }
    else
    {
      std::tcout << _T("Waiting for a communication then enter ...") << std::endl; getch();
      std::cout << (char*)p << std::endl;

      *(vu::ulong*)p = 0x48474645; // EFGH
      std::tcout << _T("Wrote data to file mapping object") << std::endl; getch();
    }
  }*/

  // CPEFile
  /*vu::CPEFileT<vu::pe32> pe(_T("C:\\Program Files\\Process Hacker 2\\x86\\ProcessHacker.exe"));
  // vu::CPEFileT<vu::pe64> pe(_T("C:\\Program Files\\Process Hacker 2\\ProcessHacker.exe"));

  vu::VUResult result = pe.Parse();
  if (result != vu::VU_OK)
  {
    std::tstring s;
    if (result == 7)
    {
      s = _T(" (Used wrong type data for the current PE file format)");
    }
    if (result == 8)
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

  auto sections = pe.GetSetionHeaders();
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
  assert(pPEHeader != nullptr);

  std::cout << std::hex << pe.Offset2RVA(0x00113a92) << std::endl;
  std::cout << std::hex << pe.RVA2Offset(0x00115292) << std::endl;

  auto IIDs = pe.GetImportDescriptors();
  assert(!IIDs.empty());

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

  auto modules = pe.GetImportModules();
  assert(!modules.empty());

  for (auto e: modules)
  {
    printf("%08X, '%s'\n", e.IIDID, e.Name.c_str());
  }

  SEPERATOR()

  auto functions = pe.GetImportFunctions();
  assert(!functions.empty());

  // for (auto e : functions)
  // {
  //   auto s = vu::FormatA("IIDID = %08X, Hint = %04X, '%s'", e.IIDID, e.Hint, e.Name.c_str());
  //   std::cout << s << std::endl;
  // }

  // SEPERATOR()

  auto pModule = pe.FindImportModule("KERNEL32.DLL");
  if (pModule != nullptr)
  {
    printf("%08X, '%s'\n", pModule->IIDID, pModule->Name.c_str());
  }

  SEPERATOR()

  auto pfn = pe.FindImportFunction("GetLastError");
  if (pfn != nullptr)
  {
    printf("%08X, %04X, '%s'\n", pfn->IIDID, pfn->Hint, pfn->Name.c_str());
  }*/

  // CGUID
  /*for (auto& e : std::vector<int>(5))
  {
    vu::CGUID guid(true);
    std::tcout << guid.AsString() << std::endl;
    std::tcout << vu::CGUID::ToString(vu::CGUID::ToGUID(guid.AsString())) << std::endl;
  }*/

  // CSingleton
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

  // CInputDialog
  /*vu::CInputDialog dialog(L"How old are you?");
  if (dialog.DoModal() == IDOK)
  {
    std::wcout << dialog.Input().String() << std::endl;
  }*/

  return 0;
}
