@ECHO OFF

SETLOCAL EnableDelayedExpansion

SET VU_NAME=Vutils

ECHO *** %VU_NAME% Getting Started ***
ECHO.

IF EXIST "%Vutils%" (
	rem 	SET /p response=[+] Vutils installed. Would you like to update Y/[N] 
	rem 	SET "conditions=0"
	rem 	IF [%response%]==[Y] SET conditions=1
	rem 	IF [%response%]==[y] SET conditions=1
	ECHO Vutils already installed
	GOTO L_EXIT
)

ECHO ---
ECHO.
FOR %%I IN ("%~dp0.") DO FOR %%J IN ("%%~dpI.") DO SET VU_DIR=%%~dpnxJ\
ECHO Add/Update Enviroment `%VU_NAME%` -^> `%VU_DIR%`
SETX Vutils %VU_DIR%
ECHO.

ECHO ---
ECHO.
ECHO Add/Update Enviroment `CPLUS_INCLUDE_PATH`
SETX CPLUS_INCLUDE_PATH %VU_DIR%include;%CPLUS_INCLUDE_PATH%
ECHO.

ECHO ---
ECHO.
ECHO Add/Update Enviroment `LIBRARY_PATH`
SETX LIBRARY_PATH %VU_DIR%lib;%LIBRARY_PATH%
ECHO.

ECHO ---
ECHO.
ECHO Add/Update VS Global Settings
CScript Others/Import.To.MS.User.Props.JS
ECHO.

ECHO ---

:L_EXIT

ECHO.
PAUSE