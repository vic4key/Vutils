# What is Vutils ?

Vutils or Vic Utilities is an utility library written in [Modern C++](http://modernescpp.com/index.php/what-is-modern-c) and for [Modern C++](http://modernescpp.com/index.php/what-is-modern-c).

It helps your programming go easier, faster and simpler.

## Information

- [x] Windows 32-bit & 64-bit
- [x] TCHAR - ANSI & UNICODE
- [x] MinGW
	- [x] for 32-bit application
	- [x] for 64-bit application
- [x] MS Visual Studio
	- [x] for 32-bit application
	- [x] for 64-bit application
- [x] C++ Builder
	- [x] for 32-bit application
	- [x] for 64-bit application

## Features

* Classes
	* [GUID - Globally/Universally Unique Identifier](<https://en.wikipedia.org/wiki/Universally_unique_identifier>)
	* [WMI - Windows Management Instrumentation](<https://docs.microsoft.com/en-us/windows/win32/wmisdk/wmi-start-page>)
	* [Buffer](<https://www.google.com/search?q=buffer+class>)
	* [Run-time Link Library](<https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking>)
	* Path
	* Process
	* Registry
	* [Service Manager](<https://docs.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/what-is-a-driver->)
	* [Sync/Async Socket](<https://docs.plm.automation.siemens.com/content/pl4x/18.1/T4EA/en_US/Teamcenter_Gateway-Technical_Connectivity_Guide/synchronous_vs_asynchronous.html>)
	* [Thread Pool](<https://en.wikipedia.org/wiki/Thread_pool>)
	* [INI File](<https://en.wikipedia.org/wiki/INI_file>)
	* [PE File Parser](<https://en.wikipedia.org/wiki/Portable_Executable>)
	* [Stop Watch](<https://www.google.com/search?q=stopwatch+execution+time>)
	* File System
	* [File Mapping](<https://docs.microsoft.com/en-us/windows/win32/memory/file-mapping>)
	* [IAT/INL API Hooking](<https://en.wikipedia.org/wiki/Hooking>)
	* [Windows Message Hooking](<https://docs.microsoft.com/en-us/windows/win32/winmsg/hooks>)
	* [Critical Section](<https://en.wikipedia.org/wiki/Critical_section>)
	* [Singleton Template](<https://en.wikipedia.org/wiki/Singleton_pattern>)
	* [Input Dialog](<https://www.google.com/search?q=input+dialog&source=lnms&tbm=isch>)
	* [In-Memory Dialog](<https://docs.microsoft.com/en-us/cpp/mfc/using-a-dialog-template-in-memory>)
	* AOB Find Pattern
	* Font/Color/File/Directory Picker
	* Cryptography
	* Miscellaneous

* Functions
	* Math
	* Date/Time
	* Data Types
	* Message Box Formatting
	* Message Debug Formatting
	* String Formatting/Manipulation
	* File/Directory Manipulation
	* Process Manipulation
	* Window Manipulation
	* [Window Message Decoder](<https://wiki.winehq.org/List_Of_Windows_Messages>)
	* Miscellaneous

## License

Released under the [MIT](LICENSE.md) license

## Requirements

Vutils requires the C++ Compiler that supported at least C++ 11

Belows are IDE minimum version that supported C++ 11

* MS Visual C++ 2012 or later
* MinGW 4.6.2 or later
* C++ Builder 10 or later

## Installation

* Clone/Download the repository [`Vutils`](<https://github.com/vic4key/Vutils.git>) to your machine
* Run the batch file `tools\Getting.Started.CMD`

* Build
	* For `MS Visual Studio C++`
		* Run batch file `tools\VS<version>.Build.Static.Library.CMD` that `<version>` is your Visual Studio version
	* For `MinGW`
		* Run batch file `tools\MinGW.Build.Static.Library.CMD`
	* For `C++ Builder`
		* \<later\>

* Configuration
	* For `MS Visual Studio C++` (optional - automatic included to the VS global settings)
	* For `MinGW`
		* Include : (optional)
		* Library : `-lVutils`
			* If `SOCKET` enabled, insert option`-DVU_SOCKET_ENABLED -lws2_32`
			* If `GUID` enabled, insert option `-DVU_GUID_ENABLED -lrpcrt4`
			* If `WMI` enabled, insert option `-DVU_WMI_ENABLED -lole32 -loleaut32 -lwbemuuid`
	* For `C++ Builder` (later)

* Usage
	* Insert `#include <vu>` to your project
	* Let's get started the namespace `vu`
	* *Note: If building error or conflict, add `#define VU_NO_EX` before `#include <vu>` to disable all extended utilities*

## Examples

* [Test\Sample.WMI.h](Test/Sample.WMI.h)
* [Test\Sample.Process.h](Test/Sample.Process.h)
* [Test\Sample.Singleton.h](Test/Sample.Singleton.h)
* [Test\Sample.StopWatch.h](Test/Sample.StopWatch.h)
* [Test\Sample.ThreadPool.h](Test/Sample.ThreadPool.h)
* [Test\Sample.AsyncSocket.h](Test/Sample.AsyncSocket.h)
* [Test\Sample.WMHooking.h](Test/Sample.WMHooking.h)
* [Test\Sample.INLHooking.h](Test/Sample.INLHooking.h)
* [Test\Sample.IATHooking.h](Test/Sample.IATHooking.h)
* See more examples in the [`Test`](Test/) project

## Contact
Feel free to contact via [Twitter](https://twitter.com/vic4key) / [Gmail](mailto:vic4key@gmail.com) / [Blog](https://blog.vic.onl/) / [Website](https://vic.onl/)