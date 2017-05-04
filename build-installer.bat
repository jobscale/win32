@echo off

setlocal

echo start build.>build-installer.log
echo.>>build-installer.log

PATH=../util\soft\NSIS\bin

if exist ..\util\soft\upx\upx.exe (
echo - succeeded upx
) else (
echo - get upx !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo - svn+ssh://dev.plustar.jp/svn/util/trunk/soft/upx
pause
endlocal
exit
)

del /F /Q .\src\pAds\release\upd 2>nul
del /F /Q .\*.exe 2>nul
del /F /Q .\*.log 2>nul
del /F /Q .\Release\*.ex~ 2>nul
del /F /Q .\Release\*.dl~ 2>nul
del /F /Q .\Release\*.00* 2>nul
del /F /Q .\src\pDieP2P-free\Release\*.ex~ 2>nul

echo build "reboot.exe"
if exist reboot.exe del /f reboot.exe
makensis.exe installer\4res-server-reboot.nsi>>build-installer.log 2>&1
if exist reboot.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pExTrapper-Uninstall-Client.exe"
if exist pExTrapper-Uninstall-Client.exe del /f pExTrapper-Uninstall-Client.exe
makensis.exe installer\4res-client-uninstall.nsi>>build-installer.log 2>&1
if exist pExTrapper-Uninstall-Client.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

rem echo build "sChecker.exe"
rem if exist sChecker.exe del /f sChecker.exe
rem makensis.exe installer\setup-clip.nsi>>build-installer.log 2>&1
rem if exist sChecker.exe (
rem echo - succeeded
rem ) else (
rem echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
rem )

echo build "pExTrapper-Setup-Client.exe"
if exist pExTrapper-Setup-Client.exe del /f pExTrapper-Setup-Client.exe
makensis.exe installer\4res-client.nsi>>build-installer.log 2>&1
if exist pExTrapper-Setup-Client.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pExTrapper4Res-Setup-Server.exe"
if exist pExTrapper4Res-Setup-Server.exe del /f pExTrapper4Res-Setup-Server.exe
makensis.exe installer\4res-server.nsi>>build-installer.log 2>&1
if exist pExTrapper4Res-Setup-Server.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pExTrapper4Ana-Setup-Server.exe"
if exist pExTrapper4Ana-Setup-Server.exe del /f pExTrapper4Ana-Setup-Server.exe
makensis.exe installer\4ana-server.nsi>>build-installer.log 2>&1
if exist pExTrapper4Ana-Setup-Server.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pPower-Setup.exe"
if exist pPower-Setup.exe del /f pPower-Setup.exe
makensis.exe installer\power.nsi>>build-installer.log 2>&1
if exist pPower-Setup.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pWeb-Setup.exe"
if exist pWebSet-Setup.exe del /f pWebSet-Setup.exe
makensis.exe installer\pWebSet.nsi>>build-installer.log 2>&1
if exist pWebSet-Setup.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pDieP2P-free-Setup.exe"
if exist pDieP2P-free-Setup.exe del /f pDieP2P-free-Setup.exe
makensis.exe installer\pDieP2P-free.nsi>>build-installer.log 2>&1
if exist pDieP2P-free-Setup.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

rem echo build "pExTrapper-Setup-Client-AntiVirus.exe"
rem if exist pExTrapper-Setup-Client-AntiVirus.exe del /f pExTrapper-Setup-Client-AntiVirus.exe
rem makensis.exe installer\4res-client-sub.nsi>>build-installer.log 2>&1
rem if exist pExTrapper-Setup-Client-AntiVirus.exe (
rem echo - succeeded
rem ) else (
rem echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
rem )

rem echo build "pExTrapperAntiVirus.exe"
rem if exist pExTrapperAntiVirus.exe del /f pExTrapperAntiVirus.exe
rem makensis.exe installer\4res-client-sub-main.nsi>>build-installer.log 2>&1
rem if exist pExTrapperAntiVirus.exe (
rem echo - succeeded
rem ) else (
rem echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
rem )

echo build "pExTrapper-Setup-pTop.exe"
if exist pExTrapper-Setup-pTop.exe del /f pExTrapper-Setup-pTop.exe
makensis.exe installer\ptop.nsi>>build-installer.log 2>&1
if exist pExTrapper-Setup-pTop.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pExTrapper-Setup-pPopMaker.exe"
if exist pExTrapper-Setup-pPopMaker.exe del /f pExTrapper-Setup-pPopMaker.exe
makensis.exe installer\ppopmaker.nsi>>build-installer.log 2>&1
if exist pExTrapper-Setup-pPopMaker.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pExTrapper-Setup-Client-Update.exe"
if exist pExTrapper-Setup-Client-Update.exe del /f pExTrapper-Setup-Client-Update.exe
makensis.exe installer\4res-client-update.nsi>>build-installer.log 2>&1
if exist pExTrapper-Setup-Client-Update.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pExTrapper4Res-Setup-Server-Update.exe"
if exist pExTrapper4Res-Setup-Server-Update.exe del /f pExTrapper4Res-Setup-Server-Update.exe
makensis.exe installer\4res-server-update.nsi>>build-installer.log 2>&1
if exist pExTrapper4Res-Setup-Server-Update.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pExTrapper4LogLife-Setup.exe"
if exist pExTrapper4LogLife-Setup.exe del /f pExTrapper4LogLife-Setup.exe
makensis.exe installer\4loglife-client.nsi>>build-installer.log 2>&1
if exist pExTrapper4LogLife-Setup.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "ChangeAgent.exe"
if exist ChangeAgent.exe del /f ChangeAgent.exe
makensis.exe installer\pChange-Agent.nsi>>build-installer.log 2>&1
if exist ChangeAgent.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "ChangeAgent-test.exe"
if exist ChangeAgent-test.exe del /f ChangeAgent-test.exe
makensis.exe installer\pChange-Agent-test.nsi>>build-installer.log 2>&1
if exist ChangeAgent-test.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pGetInfo.exe"
if exist pGetInfo.exe del /f pGetInfo.exe
makensis.exe installer\pGetInfo.nsi>>build-installer.log 2>&1
if exist pGetInfo.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pExTrapperStopTestTools.exe"
if exist pExTrapperStopTestTools.exe del /f pExTrapperStopTestTools.exe
makensis.exe installer\4res-server-TestTools.nsi>>build-installer.log 2>&1
if exist pExTrapperStopTestTools.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pExTrapper4Res-ClearConsole.exe"
if exist pExTrapper4Res-ClearConsole.exe del /f pExTrapper4Res-ClearConsole.exe
makensis.exe installer\4res-server-ClearConsole.nsi>>build-installer.log 2>&1
if exist pExTrapper4Res-ClearConsole.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pExTrapper4Res-ClearHard.exe"
if exist pExTrapper4Res-ClearHard.exe del /f pExTrapper4Res-ClearHard.exe
makensis.exe installer\4res-server-ClearHard.nsi>>build-installer.log 2>&1
if exist pExTrapper4Res-ClearHard.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

echo build "pExT4Res-Stop-Server.exe"
if exist pExT4Res-Stop-Server.exe del /f pExT4Res-Stop-Server.exe
makensis.exe installer\4res-server-stop.nsi>>build-installer.log 2>&1
if exist pExT4Res-Stop-Server.exe (
echo - succeeded
) else (
echo - build error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)

build-pads.bat

pause

endlocal
