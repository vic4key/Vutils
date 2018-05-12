# INSTALLATION

1. Checkout repository `Vutils` to `Your\Path\Vutils\`.

2. Add the enviroment `Vutils` with value `Your\Path\Vutils\` (the last slash `\` character must be included).
	* For `MinGW`
		+ Add/append the enviroment `CPLUS_INCLUDE_PATH` with value `%Vutils%include`
		+ Add/append the enviroment `LIBRARY_PATH` with value `%Vutils%lib`.

3. Build `Vutils` :

	* 3.1. For `Microsoft Visual C++`
		+ Load solution `Vutils.sln` into VS IDE.
		+ If your VS IDE is greater than VS 2012, you should upgrade the `Platform Toolset` of the Vutils solution to your VS IDE Platform Toolset by using `Upgrade VC++ Compiler and Libraries` in the context menu of the project.
		+ Go to menu `Build` > `Batch Build...` then `Select All` > `Build`.
		+ Building completed with no error you will get : `Build: 8 succeeded, 0 failed, 0 up-to-date, 0 skipped`

	* 3.2. For `MinGW`
		+ Make sure MinGW installed on your machine.
		+ Go to `Your\Path\Vutils\tools\`
		+ Run `MinGW.Build.Static.Library.CMD`.
		+ Building completed you will get no error.

	* 3.3. For `C++ Builder`
		+ 3.3.1. \<Later\>

4. Configuration

	* 4.1. For `Microsoft Visual C++`
		+ Create a new project or open your existings project.
		+ In the `Solution Explorer` right click on your project and select `Properties`.
		+ In the combo-box `Configuration:` select `All Configurations`.
		+ In the combo-box `Platform:` select `All Platforms`.
		+ Select `VC++ Directories` :
		+ Add `$(Vutils)include` to `Include Directories`.
		+ Add `$(Vutils)lib\$(Platform)` to `Library Directories`.
		+ Click OK button to save the configurations.

	* 4.2. For `MinGW`
		+ Create a new project or open your existings project.
		+ Using `-lVutils` for linking.
		+ If enabled `SOCKET`, insert `-DVU_SOCKET_ENABLED -lws2_32` for linking.
		+ If enabled `GUID`, insert `-DVU_GUID_ENABLED -lrpcrt4` for linking.
		+ Eg. `G++ main.cpp -std=c++0x -lVutils -DVU_SOCKET_ENABLED -lws2_32 -DVU_GUID_ENABLED -lrpcrt4 -o Test.exe`

	* 4.3. For `C++ Builder`
		+ \<Later\>

5. Usage
	* 5.1. Insert `#include <Vu.h>` or `#include <Boob.h>` to your project.
	* 5.2. Use the namespace `vu` to start coding.

6. Enjoy