# What is Vutils ?

Vutils or Vic Utilities is an utility library written in [Modern C++](http://modernescpp.com/index.php/what-is-modern-c) and for [Modern C++](http://modernescpp.com/index.php/what-is-modern-c).
It helps your programming go easier, faster and simpler.

# Information

- [x] Windows 32-bit & 64-bit
- [x] TCHAR - ANSI & UNICODE
- [x] MinGW
	- [x] for 32-bit application
	- [x] for 64-bit application
- [x] Microsoft Visual Studio
	- [x] for 32-bit application
	- [x] for 64-bit application
- [x] C++ Builder
	- [ ] for 32-bit application
	- [x] for 64-bit application

# Features

* Classes
	* File
	* Process
	* GUID
	* Binary
	* Library
	* Socket
	* API Hooking
	* Service
	* File Mapping
	* INI File
	* Registry
	* Critical Section
	* Stop Watch
	* PE File
	* Singleton Template
	* etc

* Functions
	* Math
	* Date/Time
	* Data Types
	* Message Formatting Debug/Box
	* String Formatting/Handling
	* File/Directory
	* Misc
	* etc

# Requirements

Vutils requires the IDE / C++ Compiler that supported at least C++ 11.
Belows are the IDE/Compiler that supported C++ 11.

* Microsoft Visual C++ 2012 or later
* MinGW 4.6.2 or later
* C++ Builder 10 or later

# License

Copyright (c) Vic P.
Fully free but please have a look at [`LICENSE.md`](LICENSE.md) file for more details.

# Installation

* Checkout repository `Vutils` to `your\path\Vutils\` on your machine.

* Add the enviroment `%Vutils%` = `your\path\Vutils\`

* Configuration
	* For `Microsoft Visual Studio C++` (in IDE)
		* Include : `$(Vutils)include`
		* Library : `$(Vutils)lib\$(Platform)`
	* For `MinGW` (in Enviroment)
		* Include : `%CPLUS_INCLUDE_PATH%` = `%Vutils%include`
		* Library : `%LIBRARY_PATH%` = `%Vutils%lib`
	* For `C++ Builder` (in IDE)
		* \<Later\>

* Usage
	* Insert `#include <Vu.h>` or `#include <Boob.h>` to your project.
	* Use the namespace `vu` to start coding.

* Build
	* For `Microsoft Visual Studio C++`
		* \<NA\>
	* For `MinGW`
		* Insert `-lVutils` for linking.
		* If enabled `SOCKET`, insert `-DVU_SOCKET_ENABLED -lws2_32` for linking.
		* If enabled `GUID`, insert `-DVU_GUID_ENABLED -lrpcrt4` for linking.
		* Eg. `G++ main.cpp -std=c++0x -lVutils -DVU_SOCKET_ENABLED -lws2_32 -DVU_GUID_ENABLED -lrpcrt4 -o Test.exe`
	* For `C++ Builder`
		* \<NA\>

* More detail in [`INSTALL.md`](INSTALL.md)

# Examples

See more examples in the [`Test`](Test/main.cpp) project.

# Contact
Feel free to contact via [Twitter](https://twitter.com/vic4key) or [Gmail](mailto:vic4key@gmail.com) or [Blog](http://viclab.biz/)