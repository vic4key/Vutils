@ECHO OFF

SETLOCAL EnableDelayedExpansion

SET VU_NAME=Vutils
SET VU_DIR=..
SET VU_3RD=%VU_DIR%\3rdparty
SET VU_LIB=%VU_DIR%\lib
SET VU_SRC=%VU_DIR%\src
SET VU_SRC_DETAILS=%VU_SRC%\details
SET VU_INCLUDE=%VU_DIR%\include

ECHO *** %VU_NAME% static library for MinGW ***

ECHO.

IF NOT EXIST %VU_LIB% MKDIR %VU_LIB%

SET LIBs=-DVutils_EXPORTS

:: for SOCKET

SET /p response=[+] Enable SOCKET (Y/[N]) ? 
SET "conditions=0"
IF [%response%]==[Y] SET conditions=1
IF [%response%]==[y] SET conditions=1

IF [%conditions%]==[1] (
	ECHO     Socket -^> ENABLED
	SET LIBs=%LIBs% -DVU_SOCKET_ENABLED -lws2_32
) else (
	ECHO     Socket -^> DISABLED
)

ECHO.

:: for GUID

SET /p response=[+] Enable GUID (Y/[N]) ? 
SET "conditions=0"
IF [%response%]==[Y] SET conditions=1
IF [%response%]==[y] SET conditions=1

IF [%conditions%]==[1] (
	ECHO     GUID -^> ENABLED
	SET LIBs=%LIBs% -DVU_GUID_ENABLED -lrpcrt4
) else (
	ECHO     GUID -^> DISABLED
)

ECHO.

:: Execute

ECHO [+] Compiling	-^> OK

SET VU_SRC_DETAILS_CPP=
FOR /f "delims=|" %%i IN ('DIR /b %VU_SRC_DETAILS%\*.cpp') DO (
  SET "VU_SRC_DETAILS_CPP=%VU_SRC_DETAILS%\%%i !VU_SRC_DETAILS_CPP!"
)

G++ -c -std=c++0x %LIBS% -I%VU_INCLUDE% ^
	%VU_3RD%\MH\src\buffer.cpp ^
	%VU_3RD%\HDE\src\hde32.cpp ^
	%VU_3RD%\HDE\src\hde64.cpp ^
	%VU_3RD%\TP11\src\Pool.cpp ^
	%VU_3RD%\TP11\src\Worker.cpp ^
	%VU_SRC%\%VU_NAME%.cpp ^
	%VU_SRC_DETAILS_CPP%

ECHO [+] Linking	-^> OK

AR rcs %VU_LIB%\lib%VU_NAME%.a *.o

ECHO [+] Cleaning	-^> OK

DEL *.o

ECHO [+] Building	-^> OK

ECHO.

PAUSE