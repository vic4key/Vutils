@ECHO OFF

SETLOCAL EnableDelayedExpansion

SET VU_NAME=Vutils
ECHO *** Howdy, I am %VU_NAME%. ***
ECHO.

FOR %%I IN ("%~dp0.") DO FOR %%J IN ("%%~dpI.") DO SET VU_DIR=%%~dpnxJ\
ECHO Added an enviroment `%VU_NAME%` -^> `%VU_DIR%`
SETX VU_TEST %VU_DIR%

ECHO.
PAUSE