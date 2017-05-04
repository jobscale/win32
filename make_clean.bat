@echo off

setlocal

echo Start Cleaning...
del /F /Q .\Release\*.ex~ 2>nul
del /F /Q .\Release\*.dl~ 2>nul
del /F /Q .\Release\*.00* 2>nul
del /F /Q .\src\pDieP2P-free\Release\*.ex~ 2>nul

del /F /Q .\Release\*.pdb 2>nul
del /F /Q .\Release\*.exp 2>nul
del /F /Q .\Release\*.lib 2>nul

del /F /Q .\Debug\pHardware.dat 2>nul
del /F /Q .\Debug\pConsole.dat 2>nul
del /F /Q .\Debug\pExTrapperReport.txt 2>nul

del /F /Q .\src\wxWidgets\build\msw\*.ncb 2>nul

rd /Q /S .\src\jsoncpp\Debug        2>nul
rd /Q /S .\src\jsoncpp\Release      2>nul
rd /Q /S .\src\pAnalyze\Debug       2>nul
rd /Q /S .\src\pAnalyze\Release     2>nul
rd /Q /S .\src\pCheckWindow\Debug   2>nul
rd /Q /S .\src\pCheckWindow\Release 2>nul
rd /Q /S .\src\pControler\Debug     2>nul
rd /Q /S .\src\pControler\Release   2>nul
rd /Q /S .\src\pExTrapper\Debug     2>nul
rd /Q /S .\src\pExTrapper\Release   2>nul
rd /Q /S .\src\pLicMaker\Debug      2>nul
rd /Q /S .\src\pLicMaker\Release    2>nul
rd /Q /S .\src\pLook\Debug          2>nul
rd /Q /S .\src\pLook\Release        2>nul
rd /Q /S .\src\pPopMaker\Debug      2>nul
rd /Q /S .\src\pPopMaker\Release    2>nul
rd /Q /S .\src\pPower\Debug         2>nul
rd /Q /S .\src\pPower\Release       2>nul
rd /Q /S .\src\Proximodo\Debug      2>nul
rd /Q /S .\src\Proximodo\Release    2>nul
rd /Q /S .\src\pSpice\Debug         2>nul
rd /Q /S .\src\pSpice\Release       2>nul
rd /Q /S .\src\pSyncScope\Debug     2>nul
rd /Q /S .\src\pSyncScope\Release   2>nul
rd /Q /S .\src\pTools\Debug         2>nul
rd /Q /S .\src\pTools\Release       2>nul
rd /Q /S .\src\pTop\Debug           2>nul
rd /Q /S .\src\pTop\Release         2>nul
rd /Q /S .\src\pWebProxy\Debug      2>nul
rd /Q /S .\src\pWebProxy\Release    2>nul
rd /Q /S .\src\pWebSet\Debug        2>nul
rd /Q /S .\src\pWebSet\Release      2>nul
rd /Q /S .\src\wxWidgets\lib\vc_lib 2>nul
rd /Q /S .\src\wxWidgets\build\msw\vc_msw  2>nul
rd /Q /S .\src\wxWidgets\build\msw\vc_msu  2>nul
rd /Q /S .\src\wxWidgets\build\msw\vc_mswu 2>nul
rd /Q /S .\src\wxWidgets\build\msw\vc_mswd 2>nul
echo Done!!
pause

endlocal
