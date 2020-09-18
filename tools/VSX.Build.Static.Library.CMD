@ECHO OFF

SETLOCAL EnableDelayedExpansion

SET VU_NAME=Vutils
:: SET VU_VSVER=current

ECHO *** %VU_NAME% static library for MSVC %VU_VSVER% ***

IF "%VU_VSDEV%"=="" (
	ECHO Please select a VS version to build the solution
	GOTO L_EXIT
)

SET /p response=Are you sure to build solution ? 
SET "confirm=0"
IF [%response%]==[Y] SET confirm=1
IF [%response%]==[y] SET confirm=1

:: SET VU_VSDEV="path\to\VsDevCmd.bat"

IF [%confirm%]==[1] (
	:: Initialize
	CALL "%VU_VSDEV%"
	CD ..
	:: 64-bit
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MD  /p:Platform=x64 /p:PlatformToolset=%VU_TSVER%
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MDd /p:Platform=x64 /p:PlatformToolset=%VU_TSVER%
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MT  /p:Platform=x64 /p:PlatformToolset=%VU_TSVER%
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MTd /p:Platform=x64 /p:PlatformToolset=%VU_TSVER%
	:: 32-bit
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MD  /p:Platform=x86 /p:PlatformToolset=%VU_TSVER%
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MDd /p:Platform=x86 /p:PlatformToolset=%VU_TSVER%
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MT  /p:Platform=x86 /p:PlatformToolset=%VU_TSVER%
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MTd /p:Platform=x86 /p:PlatformToolset=%VU_TSVER%
) ELSE (
  ECHO Canceled
)

ECHO.

:L_EXIT

PAUSE