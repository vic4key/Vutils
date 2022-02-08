@ECHO OFF

SET VU_VSVER=2022
SET VU_TSVER=v143
SET VU_VSDEV=%ProgramFiles%\Microsoft Visual Studio\%VU_VSVER%\%%L\Common7\Tools\VsDevCmd.bat

CALL Others\VSX.Update.License.Type.CMD
CALL Others\VSX.Build.Static.Library.CMD