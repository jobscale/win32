@echo off

setlocal

echo start build.>build-installer-plancher.log
echo.>>build-installer-plancher.log

PATH=../util\soft\NSIS\bin

del /F /Q .\pExTrapper-Setup-pLauncher.exe 2>nul
del /F /Q .\build-installer-plancher.log 2>nul

echo build "pExTrapper-Setup-pLauncher.exe"
if exist pExTrapper-Setup-pLauncher.exe del /f pExTrapper-Setup-pLauncher.exe
makensis.exe installer\pLauncher.nsi>>build-installer-plancher.log 2>&1
if exist pExTrapper-Setup-pLauncher.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

pause

endlocal
