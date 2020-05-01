// main.cpp : Defines the entry point for the console application.
//

/* MinGW build EXE with static library
G++ main.cpp -std=c++0x -lVutils -lgdi32 -o Test.exe && Test.exe
G++ main.cpp -std=c++0x -lVutils -DVU_SOCKET_ENABLED -lws2_32 -DVU_GUID_ENABLED -lrpcrt4 -lgdi32 -o Test.exe && Test.exe
*/

// #ifdef _UNICODE
// #undef _UNICODE
// #endif

#include <windows.h>
#include <tchar.h>

#include <Vu.h>

#include "Sample.Manager.h"
#include "Sample.Misc.h"
#include "Sample.Math.h"
#include "Sample.DF.h"
#include "Sample.Socket.h"
#include "Sample.APIHook.h"
#include "Sample.WMHook.h"
#include "Sample.INIFile.h"
#include "Sample.Registry.h"
#include "Sample.Service.h"
#include "Sample.FileSystem.h"
#include "Sample.FileMapping.h"
#include "Sample.PEFile.h"
#include "Sample.GUID.h"
#include "Sample.Singleton.h"
#include "Sample.InputDialog.h"

int _tmain(int argc, _TCHAR* argv[])
{
  std::tstring s = _T("Windows");
  auto arch = vu::GetProcessorArchitecture();
  if (arch == vu::PA_X64)
  {
    s += _T(" 64-bit");
    if (vu::IsWow64() == vu::eWow64::WOW64_YES)
    {
      s += _T(" (WOW64)");
    }
  }
  else if (arch == vu::PA_X86)
  {
    s += _T(" 32-bit");
  }
  std::tcout << s << std::endl;

  s.clear();
  #ifdef _UNICODE
  s += _T("Encoding: UNICODE");
  #else
  s += _T("Encoding: ANSI");
  #endif // _UNICODE
  std::tcout << s << std::endl;

  std::tcout << std::endl << _T("--- *.* ---") << std::endl;

  VU_SM_INIT();

  // VU_SM_ADD_SAMPLE(Misc);
  // VU_SM_ADD_SAMPLE(Math);
  // VU_SM_ADD_SAMPLE(DF);
  // VU_SM_ADD_SAMPLE(Socket);
  // VU_SM_ADD_SAMPLE(APIHook);
  // VU_SM_ADD_SAMPLE(WMHook);
  // VU_SM_ADD_SAMPLE(INIFile);
  // VU_SM_ADD_SAMPLE(Registry);
  // VU_SM_ADD_SAMPLE(Service);
  // VU_SM_ADD_SAMPLE(FileSystem);
  // VU_SM_ADD_SAMPLE(FileMapping);
  // VU_SM_ADD_SAMPLE(PEFile);
  // VU_SM_ADD_SAMPLE(GlobalUID);
  // VU_SM_ADD_SAMPLE(Singleton);
  // VU_SM_ADD_SAMPLE(InputDialog);

  VU_SM_RUN();

  return 0;
}
