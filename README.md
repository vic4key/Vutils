# What is Vutils ?

Vutils or Vic Utilities is an utility library written in [Modern C++](http://modernescpp.com/index.php/what-is-modern-c) and for [Modern C++](http://modernescpp.com/index.php/what-is-modern-c).

It helps your programming go easier, faster and simpler.

## Status

![](https://github.com/vic4key/Vutils/workflows/MSVC/badge.svg)
![](https://github.com/vic4key/Vutils/workflows/MINGW/badge.svg)

## Information

- [x] Windows 32-bit & 64-bit
- [x] TCHAR - ANSI & UNICODE
- [x] MS Visual Studio
  - [x] for 32-bit application
  - [x] for 64-bit application
- [x] GNU MinGW
  - [x] for 32-bit application
  - [x] for 64-bit application
- [ ] RAD C++ Builder
  - [ ] for 32-bit application
  - [ ] for 64-bit application

## Features

* Features
  * [GUID - Globally/Universally Unique Identifier](<https://en.wikipedia.org/wiki/Universally_unique_identifier>)
  * [WMI - Windows Management Instrumentation](<https://docs.microsoft.com/en-us/windows/win32/wmisdk/wmi-start-page>)
  * [Smart Buffer Memory](<https://www.google.com/search?q=buffer+class>)
  * [Run-time Link Library](<https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking>)
  * Object-Oriented Path
  * Object-Oriented Process
  * Object-Oriented Registry
  * [Service Manager](<https://docs.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/what-is-a-driver->)
  * [Sync/Async Socket](<https://docs.plm.automation.siemens.com/content/pl4x/18.1/T4EA/en_US/Teamcenter_Gateway-Technical_Connectivity_Guide/synchronous_vs_asynchronous.html>)
  * [RESTful Client](<https://en.wikipedia.org/wiki/Representational_state_transfer>)
  * [Thread Pool](<https://en.wikipedia.org/wiki/Thread_pool>)
  * [Read/Write/Parse INI File](<https://en.wikipedia.org/wiki/INI_file>)
  * [Parse PE File](<https://en.wikipedia.org/wiki/Portable_Executable>)
  * [Create/Parse LNK File Shortcut](<https://en.wikipedia.org/wiki/Shortcut_(computing)#Microsoft_Windows>)
  * [Smart Stop Watch](<https://www.google.com/search?q=stopwatch+execution+time>)
  * [File System Helper](<https://en.wikipedia.org/wiki/File_system>)
  * [File Mapping](<https://docs.microsoft.com/en-us/windows/win32/memory/file-mapping>)
  * [IAT/INL API Hooking](<https://en.wikipedia.org/wiki/Hooking>)
  * [Windows Message Hooking](<https://docs.microsoft.com/en-us/windows/win32/winmsg/hooks>)
  * [Critical Section](<https://en.wikipedia.org/wiki/Critical_section>)
  * [Smart Singleton DP Template](<https://en.wikipedia.org/wiki/Singleton_pattern>)
  * [Input Dialog](<https://www.google.com/search?q=input+dialog&source=lnms&tbm=isch>)
  * [In-Memory Dialog](<https://docs.microsoft.com/en-us/cpp/mfc/using-a-dialog-template-in-memory>)
  * [AOB Find Pattern](<https://wiki.cheatengine.org/index.php?title=Tutorials:AOBs>)
  * [Window Message Decoder](<https://wiki.winehq.org/List_Of_Windows_Messages>)
  * Font/Color/File/Directory Picker
  * Cryptography (B64, CRC, MD5, SHA, Big Number, etc)
  * File/Directory, Window, Process, Math Manipulation
  * Shorten Formatting for Date/Time, String, Message Box, Debug Message
  * Miscellaneous

## License

Released under the [MIT](LICENSE.md) license (and its third parties per se are also released under the MIT license)

## Requirements

Vutils requires the C++ Compiler that supported at least `C++ 11`

The minimum version of the following IDEs are supported `C++ 11`

* MS Visual C++ 2012 or later
* GNU MinGW 4.6 or later
* RAD C++ Builder 10 or later

## Installation

* Clone or Download the repository [`Vutils`](<https://github.com/vic4key/Vutils.git>) to your machine

* Run the batch file `tools\Getting.Started.CMD`

* Build
  
  * For `MS Visual Studio C++`
    * Run the batch file `tools\VS<version>.Build.Static.Library.CMD` that `<version>` is your Visual Studio version
  * For `GNU MinGW`
    * Run the batch file `tools\MinGW.Build.Static.Library.CMD`
  * For `RAD C++ Builder` (later)

* Configuration

  * For `Visual Studio C++`
    * If you are using `Visual Studio 2012~2015`, you don't need to do anything, it automatic included in the global settings.
    * If you are using `Visual Studio 2017` or later, you have to copy `Directory.Build.props` file to your solution folder that contains the `.sln` file.
  * For `GNU MinGW`
    * Include : You don't need to do anything, automatic included in the global settings.
    * Library : `-lVutils` `-lgdi32 -lole32 -lcomdlg32`
      * If `SOCKET` enabled, insert option `-DVU_INET_ENABLED -lws2_32 -lwinhttp`
      * If `GUID` enabled, insert option `-DVU_GUID_ENABLED -lrpcrt4`
      * If `WMI` enabled, insert option `-DVU_WMI_ENABLED -loleaut32 -lwbemuuid`
  * For `RAD C++ Builder` (later)

* Usage
  
  * Insert `#include <vu>` to your source code.
  * Let's get started with `vu`. Eg. `vu::format(...)`, `vu::find_top_window(...)`, `vu::Socket`, `vu::Process`, `vu::Service`, etc.
  * *Note: If building error or conflict, add `#define VU_NO_EX` above of `#include <vu>` to disable all extended utilities.*

## Examples

* [Test\Sample.Misc.h](Test/Sample.Misc.h)
* [Test\Sample.WMI.h](Test/Sample.WMI.h)
* [Test\Sample.Crypt.h](Test/Sample.Crypt.h)
* [Test\Sample.Picker.h](Test/Sample.Picker.h)
* [Test\Sample.Process.h](Test/Sample.Process.h)
* [Test\Sample.Singleton.h](Test/Sample.Singleton.h)
* [Test\Sample.StopWatch.h](Test/Sample.StopWatch.h)
* [Test\Sample.ThreadPool.h](Test/Sample.ThreadPool.h)
* [Test\Sample.AsyncSocket.h](https://github.com/vic4key/async-socket-example)
* [Test\Sample.RESTClient.h](Test/Sample.RESTClient.h)
* [Test\Sample.WMHooking.h](Test/Sample.WMHooking.h)
* [Test\Sample.INLHooking.h](Test/Sample.INLHooking.h)
* [Test\Sample.IATHooking.h](Test/Sample.IATHooking.h)
* See more examples in the [`Test`](Test/) project

## Contact

Feel free to contact via [Twitter](https://twitter.com/vic4key) / [Gmail](mailto:vic4key@gmail.com) / [Blog](https://blog.vic.onl/) / [Website](https://vic.onl/)