// main.cpp : Defines the entry point for the console application.
//

/* MinGW build EXE with static library

G++ main.cpp -std=c++0x -lVutils -lgdi32 -lole32 -lcomdlg32 -o Test.exe && Test.exe
G++ main.cpp -std=c++0x -lVutils -DVU_INET_ENABLED -DVU_GUID_ENABLED -DVU_WMI_ENABLED -lws2_32 -lgdi32 -lrpcrt4 -lole32 -loleaut32 -lwbemuuid -lcomdlg32 -lwinhttp -o Test.exe && Test.exe

G++ main.cpp -std=c++0x -municode -lVutils -DUNICODE -D_UNICODE -lgdi32 -lole32 -lcomdlg32 -o Test.exe && Test.exe
G++ main.cpp -std=c++0x -municode -lVutils -DUNICODE -D_UNICODE -DVU_INET_ENABLED -DVU_GUID_ENABLED -DVU_WMI_ENABLED -lws2_32 -lgdi32 -lrpcrt4 -lole32 -loleaut32 -lwbemuuid -lcomdlg32 -lwinhttp -o Test.exe && Test.exe

*/

#define _CRT_SECURE_NO_WARNINGS

#ifdef _MSC_VER
// // UNICODE
// #ifdef UNICODE
// #undef UNICODE
// #endif
// // _UNICODE
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
#include "Sample.IATHooking.h"
#include "Sample.INLHooking.h"
#include "Sample.WMHooking.h"
#include "Sample.INIFile.h"
#include "Sample.Registry.h"
#include "Sample.Process.h"
#include "Sample.StopWatch.h"
#include "Sample.FileSystem.h"
#include "Sample.FileMapping.h"
#include "Sample.PEFile.h"
#include "Sample.GUID.h"
#include "Sample.Singleton.h"
#include "Sample.InputDialog.h"
#include "Sample.ThreadPool.h"
#include "Sample.WMI.h"
#include "Sample.Service.h"
#include "Sample.Picker.h"
#include "Sample.Crypt.h"
#include "Sample.Window.h"
#include "Sample.Template.h"
#include "Sample.RESTClient.h"

int _tmain(int argc, _TCHAR* argv[])
{
  std::tcout
    << _T("Windows Application")
    << _T(" ")
    << (vu::is_64bits(nullptr) ? _T("64-bit") : _T("32-bit"))
    #if defined(_DEBUG) || defined(__DEBUG__)
    << _T(" (Debug)")
    #endif // _DEBUG
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
  // VU_SM_ADD_SAMPLE(Picker);
  // VU_SM_ADD_SAMPLE(Socket);
  // VU_SM_ADD_SAMPLE(AsyncSocket);
  // VU_SM_ADD_SAMPLE(INLHooking);
  // VU_SM_ADD_SAMPLE(IATHooking);
  // VU_SM_ADD_SAMPLE(WMHooking);
  // VU_SM_ADD_SAMPLE(INIFile);
  // VU_SM_ADD_SAMPLE(Registry);
  // VU_SM_ADD_SAMPLE(Process);
  // VU_SM_ADD_SAMPLE(ServiceManager);
  // VU_SM_ADD_SAMPLE(StopWatch);
  // VU_SM_ADD_SAMPLE(FileSystem);
  // VU_SM_ADD_SAMPLE(FileMapping);
  // VU_SM_ADD_SAMPLE(PEFile);
  // VU_SM_ADD_SAMPLE(UIDGlobal);
  // VU_SM_ADD_SAMPLE(Singleton);
  // VU_SM_ADD_SAMPLE(InputDialog);
  // VU_SM_ADD_SAMPLE(ThreadPool);
  // VU_SM_ADD_SAMPLE(WMIProvider);
  // VU_SM_ADD_SAMPLE(Crypt);
  // VU_SM_ADD_SAMPLE(Window);
  // VU_SM_ADD_SAMPLE(Template);
  // VU_SM_ADD_SAMPLE(RESTClient);

  VU_SM_RUN();

  return 0;
}
