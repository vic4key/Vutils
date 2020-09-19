@ECHO OFF

SETLOCAL EnableDelayedExpansion

SET VU_NAME=Vutils

ECHO *** %VU_NAME% Getting Started ***
ECHO.

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
PAUSE