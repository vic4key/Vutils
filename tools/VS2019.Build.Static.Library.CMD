@ECHO OFF

SET VU_VSVER=2019
SET VU_TSVER=v142
SET VU_VSDEV=C:\Program Files (x86)\Microsoft Visual Studio\2019\%%L\Common7\Tools\VsDevCmd.bat

CALL Others\VSX.Update.License.Type.CMD
CALL Others\VSX.Build.Static.Library.CMD