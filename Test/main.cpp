// main.cpp : Defines the entry point for the console application.
//

/* MinGW build EXE with static library
G++ main.cpp -std=c++0x -lVutils -lgdi32 -o Test.exe && Test.exe
G++ main.cpp -std=c++0x -municode -lVutils -DUNICODE -D_UNICODE -lgdi32 -o Test.exe && Test.exe
G++ main.cpp -std=c++0x -lVutils -DVU_SOCKET_ENABLED -lws2_32 -DVU_GUID_ENABLED -lrpcrt4 -lgdi32 -o Test.exe && Test.exe
G++ main.cpp -std=c++0x -municode -lVutils -DUNICODE -D_UNICODE -DVU_SOCKET_ENABLED -lws2_32 -DVU_GUID_ENABLED -lrpcrt4 -lgdi32 -o Test.exe && Test.exe
*/

#define _CRT_SECURE_NO_WARNINGS

#ifdef _MSC_VER
// #ifdef _UNICODE
// #undef _UNICODE
// #endif
#endif // _MSC_VER

#include <vu>
#include <windows.h>
#include <tchar.h>

#include "Sample.Manager.h"
#include "Sample.Misc.h"
#include "Sample.Math.h"
#include "Sample.DF.h"
#include "Sample.Socket.h"
#include "Sample.AsyncSocket.h"
#include "Sample.APIIATHook.h"
#include "Sample.APIINLHook.h"
#include "Sample.WMHook.h"
#include "Sample.INIFile.h"
#include "Sample.Registry.h"
#include "Sample.Process.h"
#include "Sample.Service.h"
#include "Sample.StopWatch.h"
#include "Sample.FileSystem.h"
#include "Sample.FileMapping.h"
#include "Sample.PEFile.h"
#include "Sample.GUID.h"
#include "Sample.Singleton.h"
#include "Sample.InputDialog.h"
#include "Sample.ThreadPool.h"

int _tmain(int argc, _TCHAR* argv[])
{
  std::tcout
    << _T("Windows Application")
    << _T(" ")
    << (vu::CProcess::Is64Bits() ? _T("64-bit") : _T("32-bit"))
    << std::endl;

  #ifdef _UNICODE
  std::tcout << _T("Encoding: UNICODE") << std::endl;
  #else
  std::tcout << _T("Encoding: ANSI") << std::endl;
  #endif // _UNICODE

  std::tcout << _T("--- *.* ---") << std::endl;

  VU_SM_INIT();

  // VU_SM_ADD_SAMPLE(Misc);
  // VU_SM_ADD_SAMPLE(Math);
  // VU_SM_ADD_SAMPLE(DF);
  // VU_SM_ADD_SAMPLE(Socket);
  // VU_SM_ADD_SAMPLE(AsyncSocket);
  // VU_SM_ADD_SAMPLE(APIINLHook);
  // VU_SM_ADD_SAMPLE(APIIATHook);
  // VU_SM_ADD_SAMPLE(WMHook);
  // VU_SM_ADD_SAMPLE(INIFile);
  // VU_SM_ADD_SAMPLE(Registry);
  // VU_SM_ADD_SAMPLE(Process);
  // VU_SM_ADD_SAMPLE(Service);
  // VU_SM_ADD_SAMPLE(StopWatch);
  // VU_SM_ADD_SAMPLE(FileSystem);
  // VU_SM_ADD_SAMPLE(FileMapping);
  // VU_SM_ADD_SAMPLE(PEFile);
  // VU_SM_ADD_SAMPLE(GlobalUID);
  // VU_SM_ADD_SAMPLE(Singleton);
  // VU_SM_ADD_SAMPLE(InputDialog);
  // VU_SM_ADD_SAMPLE(ThreadPool);

  VU_SM_RUN();

  return 0;
}
