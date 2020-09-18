@ECHO OFF

SETLOCAL EnableDelayedExpansion

SET VU_NAME=Vutils

ECHO *** Create NuGet Package for %VU_NAME% ***

cd ..
packages\NuGet.CommandLine.5.6.0\tools\NuGet.exe pack Vutils.nuspec -OutputDirectory nupkg

PAUSE