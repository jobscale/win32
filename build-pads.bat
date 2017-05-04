@echo off

setlocal

echo start build.>build-installer-pads.log
echo.>>build-installer-pads.log

PATH=%PATH%;..\util\soft\NSIS\bin

: if exist ..\util\soft\wget\wget.exe (
: echo -                                          wget succeeded
: ) else (
: echo -                                               get wget !!!!!!!!!!
: echo - svn+ssh://dev.plustar.jp/svn/util/trunk/soft/wget
: pause
: endlocal
: exit
: )

del /F /Q .\src\pAds\release\upd 2>nul
del /F /Q .\pExtADs*.exe 2>nul
del /F /Q .\build-installer-pads.log.log 2>nul

echo build "pExtADs-restart-cmd.exe"
if exist pExtADs-restart-cmd.exe del /f pExtADs-restart-cmd.exe
makensis.exe installer\4res-pAds-start-cmd.nsi>>build-installer-pads.log 2>&1
if exist pExtADs-restart-cmd.exe (
echo -                                               succeeded
) else (
echo -                                               build error !!!!!!!!!!
)

echo build "pExtADs-stop-cmd.exe"
if exist pExtADs-stop-cmd.exe del /f pExtADs-stop-cmd.exe
makensis.exe installer\4res-pAds-stop-cmd.nsi>>build-installer-pads.log 2>&1
if exist pExtADs-stop-cmd.exe (
echo -                                               succeeded
) else (
echo -                                               build error !!!!!!!!!!
)

echo build "pExtADs.exe"
if exist pExtADs.exe del /f pExtADs.exe
makensis.exe installer\4res-pAds.nsi>>build-installer-pads.log 2>&1
if exist pExtADs.exe (
echo -                                               succeeded
) else (
echo -                                               build error !!!!!!!!!!
)

echo build "pExtADs-All.exe"
if exist pExtADs-All.exe del /f pExtADs-All.exe
makensis.exe installer\4res-pAds-all.nsi>>build-installer-pads.log 2>&1
if exist pExtADs-All.exe (
echo -                                               succeeded
) else (
echo -                                               build error !!!!!!!!!!
)

echo build "pExtADs-pADs-update.exe"
if exist pExtADs-pADs-update.exe del /f pExtADs-pADs-update.exe
makensis.exe installer\4res-pAds-pAds-update.nsi>>build-installer-pads.log 2>&1
if exist pExtADs-pADs-update.exe (
echo -                                               succeeded
) else (
echo -                                               build error !!!!!!!!!!
)

echo build "pExtADs-pTemp-update.exe"
if exist pExtADs-pTemp-update.exe del /f pExtADs-pTemp-update.exe
makensis.exe installer\4res-pAds-pTemp-update.nsi>>build-installer-pads.log 2>&1
if exist pExtADs-pTemp-update.exe (
echo -                                               succeeded
) else (
echo -                                               build error !!!!!!!!!!
)

echo build "pExtADs-pUtils-update.exe"
if exist pExtADs-pUtils-update.exe del /f pExtADs-pUtils-update.exe
makensis.exe installer\4res-pAds-pUtils-update.nsi>>build-installer-pads.log 2>&1
if exist pExtADs-pUtils-update.exe (
echo -                                               succeeded
) else (
echo -                                               build error !!!!!!!!!!
)

echo build "pExtADs-pWebBP-update.exe"
if exist pExtADs-pWebBP-update.exe del /f pExtADs-pWebBP-update.exe
makensis.exe installer\4res-pAds-pWebBP-update.nsi>>build-installer-pads.log 2>&1
if exist pExtADs-pWebBP-update.exe (
echo -                                               succeeded
) else (
echo -                                               build error !!!!!!!!!!
)

echo build "pExtADs-Uninstall.exe"
if exist pExtADs-Uninstall.exe del /f pExtADs-Uninstall.exe
makensis.exe installer\4res-pAds-uninstall.nsi>>build-installer-pads.log 2>&1
if exist pExtADs-Uninstall.exe (
echo -                                               succeeded
) else (
echo -                                               build error !!!!!!!!!!
)

echo build "pExtADs-Tools.exe"
if exist pExtADs-Tools.exe del /f pExtADs-Tools.exe
makensis.exe installer\4res-pAds-Tools.nsi>>build-installer-pads.log 2>&1
if exist pExtADs-Tools.exe (
echo -                                               succeeded
) else (
echo -                                               build error !!!!!!!!!!
)

echo build "pExtADs-Tester.exe"
if exist pExtADs-Tester.exe del /f pExtADs-Tester.exe
makensis.exe installer\4res-pAds-Test.nsi>>build-installer-pads.log 2>&1
if exist pExtADs-Tester.exe (
echo -                                               succeeded
) else (
echo -                                               build error !!!!!!!!!!
)

pause

endlocal
