@ECHO OFF

SETLOCAL DisableDelayedExpansion

SET VU_NAME=Vutils
:: SET VU_VSVER=current

ECHO *** %VU_NAME% static library for MSVC %VU_VSVER% ***

IF NOT EXIST "%VU_VSDEV%" (
	ECHO Visual Studio %VU_VSVER% with Platform Toolset %VU_TSVER% not found on your machine.
	ECHO Please select an existing version to build the solution.
	GOTO L_EXIT
)

IF [%VU_CI_BUILD%]==[1] (
	SET "confirm=1"
	GOTO L_CI_BUILD
)

SET /p response=Are you sure to build solution (Y/[N]) ? 
SET "confirm=0"
IF [%response%]==[Y] SET confirm=1
IF [%response%]==[y] SET confirm=1

:L_CI_BUILD

:: SET VU_VSDEV="path\to\VsDevCmd.bat"

IF [%confirm%]==[1] (
	:: Initialize
	CALL "%VU_VSDEV%"
	CD ..
	:: 64-bit
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MD  /p:Platform=x64 /p:LanguageStandard="stdcpp17" /p:PlatformToolset=%VU_TSVER% -m
	IF [%VU_CI_BUILD%]==[] (
		MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MDd /p:Platform=x64 /p:LanguageStandard="stdcpp17" /p:PlatformToolset=%VU_TSVER% -m
	)
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MT  /p:Platform=x64 /p:LanguageStandard="stdcpp17" /p:PlatformToolset=%VU_TSVER% -m
	IF [%VU_CI_BUILD%]==[] (
		MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MTd /p:Platform=x64 /p:LanguageStandard="stdcpp17" /p:PlatformToolset=%VU_TSVER% -m
	)
	:: 32-bit
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MD  /p:Platform=x86 /p:LanguageStandard="stdcpp17" /p:PlatformToolset=%VU_TSVER% -m
	IF [%VU_CI_BUILD%]==[] (
		MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MDd /p:Platform=x86 /p:LanguageStandard="stdcpp17" /p:PlatformToolset=%VU_TSVER% -m
	)
	MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MT  /p:Platform=x86 /p:LanguageStandard="stdcpp17" /p:PlatformToolset=%VU_TSVER% -m
	IF [%VU_CI_BUILD%]==[] (
		MSBuild %VU_NAME%.sln /t:%VU_NAME% /p:Configuration=MTd /p:Platform=x86 /p:LanguageStandard="stdcpp17" /p:PlatformToolset=%VU_TSVER% -m
	)
) ELSE (
  ECHO Canceled
)

ECHO.

:L_EXIT

IF NOT [%VU_CI_BUILD%]==[1] (
	PAUSE
)
