; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=ginfo -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT     "pGetInfo"
!Define PS_PRODUCTNAME "PsGetInfo"
!Define PS_MUICACHE    "Software\Microsoft\Windows\ShellNoRoam\MUICache"
!Define PsTOOLS        "Software\Sysinternals"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\${PS_PRODUCT}.exe"
InstallDir "$DESKTOP\PS"

; --------------------------------
; Include Lib
;
!include LogicLib.nsh
!include WordFunc.nsh
!include "Time.nsh"
!include Plustar_Include.nsh
!include Plustar_Version.nsh

; --------------------------------
; Pages
;
BrandingText "Windows GetInfomation Tools [Rev:${PLUSTAR_VERSION}]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_ICON   "icons\setup.ico"
!define MUI_UNICON "icons\setup.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --------------------------------
; Languages
;
!insertmacro MUI_LANGUAGE "Japanese"

;--------------------------------
; Install Section
;

;; ログ表示を有効にする
ShowInstDetails show

var t1 ;; 日
var t2 ;; 月
var t3 ;; 年
var t4 ;; 時間
var t5 ;; 分
var t6 ;; 秒

Function .onInit
  Banner::show /NOUNLOAD "展開中..."

  Banner::getWindow /NOUNLOAD
  Pop $1

  again:
    IntOp $0 $0 + 1
    Sleep 1
    StrCmp $0 500 0 again

  GetDlgItem $2 $1 1030
  SendMessage $2 ${WM_SETTEXT} 0 "STR:展開完了..."

  again2:
    IntOp $0 $0 + 1
    Sleep 1
    StrCmp $0 700 0 again2

  Banner::destroy
FunctionEnd

section "Install"
  ;; 実行統計を取得する
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "${PLUSTAR_VERSION}"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; 古いファイルの削除を行う
  Delete "$DESKTOP\pGetInfo.exe"
  Delete "$DESKTOP\data.7z"
  RMDir /r "$DESKTOP\PS"

  StrCmp "$EXEFILE" "${PS_PRODUCT}.exe" DoExeWork DoExeNotWork
  DoExeNotWork:
    MessageBox MB_OK|MB_ICONSTOP "プログラム名が不正です。"
    Call CleanJob
    quit
  DoExeWork:

  StrCmp $DESKTOP $EXEDIR DoNotWork DoWork
  DoNotWork:
    MessageBox MB_OK|MB_ICONSTOP "デスクトップでは実行しないで下さい！"
    Call CleanJob
    quit
  DoWork:

  SetOutPath "$INSTDIR"

  ;;HideWindow

  ; ----------- Full-screen screenshot ----------------
  ; Put filename of screenshot onto stack
  StrCpy $0 "$INSTDIR\Fullscreen_Screenshot.bmp"
  Push $0

  ; Capture full screen
  nsScreenshot::Grab_FullScreen

  ; $1 contains "ok" if the screenshot was taken
  ; $1 contains "error" if something went wrong
  Pop $1
  StrCmp $1 "ok" ok_full_pic 0
  DetailPrint "FullScreen screenshot error"
  Goto next_pic_1
  ok_full_pic:

  ; Pop image dimensions from stack (only available when plugin returned "ok")
  Pop $R1
  Pop $R2
  DetailPrint "Fullscreen screenshot saved : $R1x$R2"

  next_pic_1:

  ; ----------- Taskbar screenshot ----------------
  ; Put window handle of taskbar window onto stack
  FindWindow $R0 "Shell_TrayWnd"
  Push $R0

  ; Put filename of screenshot onto stack
  StrCpy $0 "$INSTDIR\Taskbar_Screenshot.bmp"
  Push $0

  ; Capture Taskbar window
  nsScreenshot::Grab

  Pop $1
  ; $1 contains "ok" if the screenshot was taken
  ; $1 contains "error" if something went wrong
  StrCmp $1 "ok" ok_taskbar_pic 0
  DetailPrint "System Tray screenshot error"
  Goto next_pic_2
  ok_taskbar_pic:

  ; Pop image dimensions from stack (only available when plugin returned "ok")
  Pop $R1
  Pop $R2
  DetailPrint "Taskbar screenshot saved : $R1x$R2"

  next_pic_2:

  ; ----------- Tray screenshot ----------------
  ; Put window handle of taskbar window onto stack
  ; Notice the second call to FindWindow, needed because it's a child window
  FindWindow $R0 "Shell_TrayWnd"
  FindWindow $R0 "TrayNotifyWnd" "" $R0
  Push $R0

  ; Put filename of screenshot onto stack
  StrCpy $0 "$INSTDIR\Tray_Screenshot.bmp"
  Push $0

  ; Capture Taskbar window
  nsScreenshot::Grab

  Pop $1
  ; $1 contains "ok" if the screenshot was taken
  ; $1 contains "error" if something went wrong
  StrCmp $1 "ok" ok_tray_pic 0
  DetailPrint "Tray screenshot error"
  Goto next_pic_3
  ok_tray_pic:

  ; Pop image dimensions from stack (only available when plugin returned "ok")
  Pop $R1
  Pop $R2
  DetailPrint "Tray screenshot saved : $R1x$R2"

  next_pic_3:

  ;;ShowWindow $HWNDPARENT "${SW_HIDE}"

  ;; MicrosoftのPsTools系の使用許可ライセンスを強制登録する
  WriteRegDWORD HKCU "${PsTOOLS}\psexec"     "EulaAccepted" 0x00000001
  WriteRegDWORD HKCU "${PsTOOLS}\psfile"     "EulaAccepted" 0x00000001
  WriteRegDWORD HKCU "${PsTOOLS}\psgetsid"   "EulaAccepted" 0x00000001
  WriteRegDWORD HKCU "${PsTOOLS}\Psinfo"     "EulaAccepted" 0x00000001
  WriteRegDWORD HKCU "${PsTOOLS}\pskill"     "EulaAccepted" 0x00000001
  WriteRegDWORD HKCU "${PsTOOLS}\pslist"     "EulaAccepted" 0x00000001
  WriteRegDWORD HKCU "${PsTOOLS}\psloggedon" "EulaAccepted" 0x00000001
  WriteRegDWORD HKCU "${PsTOOLS}\psloglist"  "EulaAccepted" 0x00000001
  WriteRegDWORD HKCU "${PsTOOLS}\pspasswd"   "EulaAccepted" 0x00000001
  WriteRegDWORD HKCU "${PsTOOLS}\psservice"  "EulaAccepted" 0x00000001
  WriteRegDWORD HKCU "${PsTOOLS}\psshutdown" "EulaAccepted" 0x00000001
  WriteRegDWORD HKCU "${PsTOOLS}\pssuspend"  "EulaAccepted" 0x00000001

  ;; レジストリにアクセス制限されてる場合があるので
  ;; マンボーは「窓の手」でレジストリへのアクセスが制限(念のために窓の手はパスワードつき
  ;; [HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Policies\System]
  ;; "DisableRegistryTools"=dword:00000000
  DetailPrint "Change DisableRegistry..."
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Policies\System"  "DisableRegistryTools" 0x00000000

  SetOutPath "$TEMP"
  File "..\external\pGetInfo\7z.exe"
  File "..\external\pGetInfo\7z.dll"

  ;; パスを元に戻す
  SetOutPath "$INSTDIR"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "Get reg..."
  File "..\external\pGetInfo\pReg.bat"
  nsExec::Exec '$INSTDIR\pReg.bat'
  Delete "$INSTDIR\pReg.bat"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "Get net..."
  File "..\external\pGetInfo\pNet.bat"
  nsExec::Exec '$INSTDIR\pNet.bat'
  Delete "$INSTDIR\pNet.bat"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "Get dir..."
  File "..\external\pGetInfo\pDir.bat"
  nsExec::Exec '$INSTDIR\pDir.bat'
  Delete "$INSTDIR\pDir.bat"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "Get netcmd..."
  File "..\external\pGetInfo\pNetCmd.bat"
  nsExec::Exec '$INSTDIR\pNetCmd.bat'
  Delete "$INSTDIR\pNetCmd.bat"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; pdh(Windows Performance Data Helper DLL)は最新版がSYSDIRに入ってる
  DetailPrint "Get pstools..."
  File "..\external\pGetInfo\pTools.bat"
  File "..\external\PsTools\psservice.exe"
  File "..\external\PsTools\psgetsid.exe"
  File "..\external\PsTools\Psinfo.exe"
  File "..\external\PsTools\pslist.exe"
  File "..\external\PsTools\Fport.exe"
  ;;File "..\external\PsTools\pdh.dll"
  ;;CopyFiles /SILENT "$INSTDIR\pdh.dll" "$SYSDIR"
  nsExec::Exec '$INSTDIR\pTools.bat'
  Delete "$INSTDIR\pTools.bat"
  Delete "$INSTDIR\psservice.exe"
  Delete "$INSTDIR\psgetsid.exe"
  Delete "$INSTDIR\Psinfo.exe"
  Delete "$INSTDIR\pslist.exe"
  Delete "$INSTDIR\Fport.exe"
  ;;Delete "$INSTDIR\pdh.dll"
  ;;Delete "$SYSDIR\pdh.dll"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "Create dir..."
  CreateDirectory "$INSTDIR\user\COOKIES"
  CreateDirectory "$INSTDIR\user\HISTORY"
  CreateDirectory "$INSTDIR\user\FAVORITES"
  CreateDirectory "$INSTDIR\user\QUICKLAUNCH"
  CreateDirectory "$INSTDIR\user\SMSTARTUP"
  CreateDirectory "$INSTDIR\user\SMPROGRAMS"
  CreateDirectory "$INSTDIR\user\STARTMENU"
  CreateDirectory "$INSTDIR\user\DESKTOP"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "Get User Profile..."
  CopyFiles /SILENT "$COOKIES"       "$INSTDIR\user\COOKIES"
  CopyFiles /SILENT "$HISTORY"       "$INSTDIR\user\HISTORY"
  CopyFiles /SILENT "$FAVORITES"     "$INSTDIR\user\FAVORITES"
  CopyFiles /SILENT "$QUICKLAUNCH"   "$INSTDIR\user\QUICKLAUNCH"
  CopyFiles /SILENT "$SMSTARTUP"     "$INSTDIR\user\SMSTARTUP"
  CopyFiles /SILENT "$SMPROGRAMS"    "$INSTDIR\user\SMPROGRAMS"
  CopyFiles /SILENT "$STARTMENU"     "$INSTDIR\user\STARTMENU"
  CopyFiles /SILENT "$DESKTOP\*"     "$INSTDIR\user\DESKTOP"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; all usersに変更
  SetShellVarContext all
  DetailPrint "Get AllUser Profile..."
  CopyFiles /SILENT "$COOKIES"       "$INSTDIR\user\COOKIES"
  CopyFiles /SILENT "$HISTORY"       "$INSTDIR\user\HISTORY"
  CopyFiles /SILENT "$FAVORITES"     "$INSTDIR\user\FAVORITES"
  CopyFiles /SILENT "$QUICKLAUNCH"   "$INSTDIR\user\QUICKLAUNCH"
  CopyFiles /SILENT "$SMSTARTUP"     "$INSTDIR\user\SMSTARTUP"
  CopyFiles /SILENT "$SMPROGRAMS"    "$INSTDIR\user\SMPROGRAMS"
  CopyFiles /SILENT "$STARTMENU"     "$INSTDIR\user\STARTMENU"
  CopyFiles /SILENT "$DESKTOP\*"     "$INSTDIR\user\DESKTOP"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; 元のユーザに戻す
  SetShellVarContext current

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; テンポラリで作業を行う
  SetOutPath "$TEMP"

  ${time::GetLocalTime} $0
  ${time::TimeString} "$0" $t1 $t2 $t3 $t4 $t5 $t6

  ;;MessageBox MB_OK|MB_ICONSTOP "$0::$t1::$t2::$t3::$t4::$t5::$t6"

  DetailPrint "Compress PS..."
  nsExec::Exec '"$TEMP\7z.exe" a "$DESKTOP\data-$t3$t2$t1-$t4$t5$t6.7z" "$DESKTOP\PS"'
  Sleep 1000

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "Dlete Temp dir..."
  Delete "$INSTDIR\*"
  RMDir /r "$DESKTOP\PS"
  Delete "$TEMP\7z.*"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "お疲れ様です..."
  CopyFiles /SILENT "$DESKTOP\data-$t3$t2$t1-$t4$t5$t6.7z" "$EXEDIR"

  StrCpy $0 "デスクトップに作成された「data-$t3$t2$t1-$t4$t5$t6.7z」を$\r$\n$\r$\n"
  StrCpy $0 "$0実行ディレクトリ：$\r$\n"
  StrCpy $0 "$0 $EXEDIR$\r$\n$\r$\n"
  StrCpy $0 "$0にコピーしました。"
  MessageBox MB_OK|MB_ICONINFORMATION $0

  StrCpy $0 "「data-$t3$t2$t1-$t4$t5$t6.7z」のファイル確認は完了しましたか？$\r$\n"
  StrCpy $0 "$0「data-$t3$t2$t1-$t4$t5$t6.7z」を削除します！"
  MessageBox MB_OK|MB_ICONQUESTION $0
  Delete "$DESKTOP\data-$t3$t2$t1-$t4$t5$t6.7z"

  StrCpy $0 "P2Pテストを続けますか？$\r$\n$\r$\n"
  StrCpy $0 "$0通常はここまでで十分です。$\r$\n"
  StrCpy $0 "$0ご協力大変ありがとうございました。"
  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoWorkTestP2P
  ;;MessageBox MB_YESNO|MB_ICONQUESTION "P2Pテストを続けますか？" IDYES DoWorkTestP2P
    Call CleanJob
    Quit
  DoWorkTestP2P:

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "プロセスのお掃除..."

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "Remove nProtect..."
  StrCpy $0 "nProtectの解除を行いますか？$\r$\n"
  StrCpy $0 "$0意味はないけど…$\r$\n$\r$\n"
  StrCpy $0 "$0(趣味です"
  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkNextTimeCrack
    File "..\external\pGetInfo\gguninst.exe"
    nsExec::Exec '$TEMP\gguninst.exe'
    Delete "$TEMP\gguninst.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\gguninst.exe"
  DoWorkNextTimeCrack:

  SimpleSC::StopService "W32Time"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;;DetailPrint "その前に時計を狂わせて..."

  ;;StrCpy $0 "日付設定を2025年にしますか？$\r$\n$\r$\n"
  ;;StrCpy $0 "$0!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!$\r$\n"
  ;;StrCpy $0 "$0!!ローカルマシンでは日付を変更しないでください。!!$\r$\n"
  ;;StrCpy $0 "$0!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!$\r$\n$\r$\n"
  ;;StrCpy $0 "$0ま～意味はないけどね..."
  ;;MessageBox MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2 $0 IDNO DoWorkTimeCrack
  ;;  nsExec::Exec 'date 2025-12-31'
  ;;  DetailPrint "日付2025年12月31日に設定しました..."
  ;;DoWorkTimeCrack:

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "不要なサービスを止めます..."

  DetailPrint "Stop Service ADClientService..."
  SimpleSC::StopService "ADClientService"

  DetailPrint "Stop Service Dms-RHost..."
  SimpleSC::StopService "Dms-RHost"

  DetailPrint "Stop Service ProVision Agent Service..."
  SimpleSC::StopService "ProVision Agent Service"

  DetailPrint "Stop Service DMS Agent Service..."
  SimpleSC::StopService "DMS Agent Service"

  DetailPrint "Stop Service NPACSVC..."
  SimpleSC::StopService "NPACSVC"

  DetailPrint "Stop Service SwDist..."
  SimpleSC::StopService "SwDist"

  DetailPrint "Stop Service ProVision Agent..."
  SimpleSC::StopService "ProVision Agent"

  DetailPrint "Stop Service ProVision-RHost..."
  SimpleSC::StopService "ProVision-RHost"

  DetailPrint "Stop Service stisvc..."
  SimpleSC::StopService "stisvc"

  DetailPrint "Stop Service MpsSvc..."
  SimpleSC::StopService "MpsSvc"

  DetailPrint "Stop Service WerSvc..."
  SimpleSC::StopService "WerSvc"

  DetailPrint "Stop Service WinDefend..."
  SimpleSC::StopService "WinDefend"

  DetailPrint "Stop Service SDRSVC..."
  SimpleSC::StopService "SDRSVC"

  DetailPrint "Stop Service CLTNetCnService..."
  SimpleSC::StopService "CLTNetCnService"

  DetailPrint "Stop Service Avg7Alrt..."
  SimpleSC::StopService "Avg7Alrt"

  DetailPrint "Stop Service Avg7UpdSvc..."
  SimpleSC::StopService "Avg7UpdSvc"

  DetailPrint "Stop Service AvgCoreSvc..."
  SimpleSC::StopService "AvgCoreSvc"

  DetailPrint "Stop Service AVGEMS..."
  SimpleSC::StopService "AVGEMS"

  DetailPrint "Stop Service AVGFwSrv..."
  SimpleSC::StopService "AVGFwSrv"

  DetailPrint "Stop Service bdss..."
  SimpleSC::StopService "bdss"

  DetailPrint "Stop Service XCOMM..."
  SimpleSC::StopService "XCOMM"

  DetailPrint "Stop Service LiveUpdate Notice Ex..."
  SimpleSC::StopService "LiveUpdate Notice Ex"

  DetailPrint "Stop Service WSearch..."
  SimpleSC::StopService "WSearch"

  DetailPrint "Stop Service WinRM..."
  SimpleSC::StopService "WinRM"

  DetailPrint "Stop Service srservice..."
  SimpleSC::StopService "srservice"

  DetailPrint "Stop Service SPBBCSvc..."
  SimpleSC::StopService "SPBBCSvc"

  DetailPrint "Stop Service ccSetMgr..."
  SimpleSC::StopService "ccSetMgr"

  DetailPrint "Stop Service ccPwdSvc..."
  SimpleSC::StopService "ccPwdSvc"

  DetailPrint "Stop Service ccProxy..."
  SimpleSC::StopService "ccProxy"

  DetailPrint "Stop Service SNDSrvc..."
  SimpleSC::StopService "SNDSrvc"

  DetailPrint "Stop Service ccEvtMgr..."
  SimpleSC::StopService "ccEvtMgr"

  DetailPrint "Stop Service Nmnsrvc..."
  SimpleSC::StopService "Nmnsrvc"

  DetailPrint "Stop Service AMON..."
  SimpleSC::StopService "AMON"

  DetailPrint "Stop Service IMON..."
  SimpleSC::StopService "IMON"

  DetailPrint "Stop Service DMON..."
  SimpleSC::StopService "DMON"

  DetailPrint "Stop Service EMON..."
  SimpleSC::StopService "EMON"

  DetailPrint "Stop Service eeCtrl..."
  SimpleSC::StopService "eeCtrl"

  DetailPrint "Stop Service Fdcpsbie..."
  SimpleSC::StopService "Fdcpsbie"

  DetailPrint "Stop Service NAVENG..."
  SimpleSC::StopService "NAVENG"

  DetailPrint "Stop Service NAVEX15..."
  SimpleSC::StopService "NAVEX15"

  DetailPrint "Stop Service SRTSP..."
  SimpleSC::StopService "SRTSP"

  DetailPrint "Stop Service Alerter..."
  SimpleSC::StopService "Alerter"

  DetailPrint "Stop Service ERSvc..."
  SimpleSC::StopService "ERSvc"

  DetailPrint "Stop Service IPSSVC..."
  SimpleSC::StopService "IPSSVC"

  DetailPrint "Stop Service PolocyAgent..."
  SimpleSC::StopService "PolocyAgent"

  DetailPrint "Stop Service RDSessMgr..."
  SimpleSC::StopService "RDSessMgr"

  DetailPrint "Stop Service RemoteRegistory..."
  SimpleSC::StopService "RemoteRegistory"

  DetailPrint "Stop Service seclogon..."
  SimpleSC::StopService "seclogon"

  DetailPrint "Stop Service SamSs..."
  SimpleSC::StopService "SamSs"

  DetailPrint "Stop Service wscsvc..."
  SimpleSC::StopService "wscsvc"

  DetailPrint "Stop Service ShellHWDetection..."
  SimpleSC::StopService "ShellHWDetection"

  DetailPrint "Stop Service SCardSvr..."
  SimpleSC::StopService "SCardSvr"

  DetailPrint "Stop Service SSDPSRV..."
  SimpleSC::StopService "SSDPSRV"

  DetailPrint "Stop Service SENS..."
  SimpleSC::StopService "SENS"

  DetailPrint "Stop Service SUService..."
  SimpleSC::StopService "SUService"

  DetailPrint "Stop Service Schedule..."
  SimpleSC::StopService "Schedule"

  DetailPrint "Stop Service LmHosts..."
  SimpleSC::StopService "LmHosts"

  DetailPrint "Stop Service UPS..."
  SimpleSC::StopService "UPS"

  DetailPrint "Stop Service upnphost..."
  SimpleSC::StopService "upnphost"

  DetailPrint "Stop Service msvsmon80..."
  SimpleSC::StopService "msvsmon80"

  DetailPrint "Stop Service VSS..."
  SimpleSC::StopService "VSS"

  DetailPrint "Stop Service WebClient..."
  SimpleSC::StopService "WebClient"

  DetailPrint "Stop Service idsvc..."
  SimpleSC::StopService "idsvc"

  DetailPrint "Stop Service WudfSvc..."
  SimpleSC::StopService "WudfSvc"

  DetailPrint "Stop Service SharedAccess..."
  SimpleSC::StopService "SharedAccess"

  DetailPrint "Stop Service MSIServer..."
  SimpleSC::StopService "MSIServer"

  DetailPrint "Stop Service Wmi..."
  SimpleSC::StopService "Wmi"

  DetailPrint "Stop Service FontCache3.0.0.0..."
  SimpleSC::StopService "FontCache3.0.0.0"

  DetailPrint "Stop Service spupdsvc..."
  SimpleSC::StopService "spupdsvc"

  DetailPrint "Stop Service Dot3svc..."
  SimpleSC::StopService "Dot3svc"

  DetailPrint "Stop Service WZCSVC..."
  SimpleSC::StopService "WZCSVC"

  DetailPrint "Stop Service WmiApSrv..."
  SimpleSC::StopService "WmiApSrv"

  DetailPrint "Stop Service KWatchSvc..."
  SimpleSC::StopService "KWatchSvc"

  DetailPrint "Stop Service KISSvc..."
  SimpleSC::StopService "KISSvc"

  DetailPrint "Stop Service KPfwSvc..."
  SimpleSC::StopService "KPfwSvc"

  DetailPrint "Stop Service KNetWch..."
  SimpleSC::StopService "KNetWch"

  DetailPrint "Stop Service KSecDD..."
  SimpleSC::StopService "KSecDD"

  DetailPrint "Stop Service KWatch3..."
  SimpleSC::StopService "KWatch3"

  DetailPrint "Stop Service ClipSrv..."
  SimpleSC::StopService "ClipSrv"

  DetailPrint "Stop Service BITS..."
  SimpleSC::StopService "BITS"

  DetailPrint "Stop Service ALG..."
  SimpleSC::StopService "ALG"

  DetailPrint "Stop Service Browser..."
  SimpleSC::StopService "Browser"

  DetailPrint "Stop Service Dnscache..."
  SimpleSC::StopService "Dnscache"

  DetailPrint "Stop Service helpsvc..."
  SimpleSC::StopService "helpsvc"

  DetailPrint "Stop Service easdrv..."
  SimpleSC::StopService "easdrv"

  DetailPrint "Stop Service ekrn..."
  SimpleSC::StopService "ekrn"

  DetailPrint "Stop Service ImapiService..."
  SimpleSC::StopService "ImapiService"

;; ここらあたりでネットが普通になる
  DetailPrint "Stop Service CiSvc..."
  SimpleSC::StopService "CiSvc"

  DetailPrint "Stop Service PolicyAgent..."
  SimpleSC::StopService "PolicyAgent"

  DetailPrint "Stop Service JavaQuickStarterService..."
  SimpleSC::StopService "JavaQuickStarterService"

  DetailPrint "Stop Service IPSec..."
  SimpleSC::StopService "IPSec"

  DetailPrint "Stop Service iPod Service..."
  SimpleSC::StopService "iPod Service"

  DetailPrint "Stop Service SysmonLog..."
  SimpleSC::StopService "SysmonLog"

  DetailPrint "Stop Service npggsvc..."
  SimpleSC::StopService "npggsvc"

  DetailPrint "Stop Service npkcmsvc..."
  SimpleSC::StopService "npkcmsvc"

  DetailPrint "Stop Service eamon..."
  SimpleSC::StopService "eamon"

  DetailPrint "Stop Service ehdrv..."
  SimpleSC::StopService "ehdrv"

  DetailPrint "Stop Service epfwtdir..."
  SimpleSC::StopService "epfwtdir"

  DetailPrint "Stop Service epfwwfpr..."
  SimpleSC::StopService "epfwwfpr"

  DetailPrint "Stop Service mnmsrvc..."
  SimpleSC::StopService "mnmsrvc"

  DetailPrint "Stop Service Netlogon..."
  SimpleSC::StopService "Netlogon"

  DetailPrint "Stop Service NetDDE..."
  SimpleSC::StopService "NetDDE"

  ;;DetailPrint "Stop Service Nla..."
  ;;SimpleSC::StopService "Nla"

  DetailPrint "Stop Service RasAuto..."
  SimpleSC::StopService "RasAuto"

  DetailPrint "Stop Service RemoteAccess..."
  SimpleSC::StopService "RemoteAccess"

  DetailPrint "Stop Service NtmsSvc..."
  SimpleSC::StopService "NtmsSvc"

  DetailPrint "Stop Service NetDDEdsdm..."
  SimpleSC::StopService "NetDDEdsdm"

  DetailPrint "Stop Service SKRDSvr..."
  SimpleSC::StopService "SKRDSvr"

  DetailPrint "Stop Service SkEchoSvr..."
  SimpleSC::StopService "SkEchoSvr"

  DetailPrint "Stop Service SKBlackd..."
  SimpleSC::StopService "SKBlackd"

  DetailPrint "Stop Service Syunkai Client..."
  SimpleSC::StopService "Syunkai Client"

  DetailPrint "Stop Service SKFIL..."
  SimpleSC::StopService "SKFIL"

  DetailPrint "Stop Service SKFIL..."
  SimpleSC::StopService "SKFIL"

  DetailPrint "Stop Service NVSvc..."
  SimpleSC::StopService "NVSvc"

  DetailPrint "Stop Service MDM..."
  SimpleSC::StopService "MDM"

  DetailPrint "Stop Service Bonjour Service..."
  SimpleSC::StopService "Bonjour Service"

  DetailPrint "Stop Service wuauserv..."
  SimpleSC::StopService "wuauserv"

  DetailPrint "Stop Service Apple Mobile Device..."
  SimpleSC::StopService "Apple Mobile Device"

  DetailPrint "Stop Service TrkWks..."
  SimpleSC::StopService "TrkWks"

  DetailPrint "Stop Service SKFSD..."
  SimpleSC::StopService "SKFSD"

  DetailPrint "Stop Service nod32drv..."
  SimpleSC::StopService "nod32drv"

  DetailPrint "Stop Service HidServ..."
  SimpleSC::StopService "HidServ"

  DetailPrint "Stop Service Fax..."
  SimpleSC::StopService "Fax"

  DetailPrint "Stop Service FastUserSwitchingCompatibility..."
  SimpleSC::StopService "FastUserSwitchingCompatibility"

  DetailPrint "Stop Service NetTcpPortSharing..."
  SimpleSC::StopService "NetTcpPortSharing"

  DetailPrint "Stop Service HTTPFilter..."
  SimpleSC::StopService "HTTPFilter"

  DetailPrint "Stop Service LMIMaint..."
  SimpleSC::StopService "LMIMaint"

  DetailPrint "Stop Service Messenger..."
  SimpleSC::StopService "Messenger"

  DetailPrint "Stop Service MSDTC..."
  SimpleSC::StopService "MSDTC"

  DetailPrint "Stop Service NOD32krn..."
  SimpleSC::StopService "NOD32krn"

  DetailPrint "Stop Service RemoteRegistry..."
  SimpleSC::StopService "RemoteRegistry"

  DetailPrint "Stop Service RSVP..."
  SimpleSC::StopService "RSVP"

  DetailPrint "Stop Service stllssvr..."
  SimpleSC::StopService "stllssvr"

  DetailPrint "Stop Service SwPrv..."
  SimpleSC::StopService "SwPrv"

  DetailPrint "Stop Service Themes..."
  SimpleSC::StopService "Themes"

  DetailPrint "Stop Service TlntSvr..."
  SimpleSC::StopService "TlntSvr"

  DetailPrint "Stop Service usnjsvc..."
  SimpleSC::StopService "usnjsvc"

  DetailPrint "Stop Service usprserv..."
  SimpleSC::StopService "usprserv"

  DetailPrint "Stop Service WMPNetworkSvc..."
  SimpleSC::StopService "WMPNetworkSvc"

  ;;DetailPrint "Stop Service xmlprov..."
  ;;SimpleSC::StopService "xmlprov"

  DetailPrint "Stop Service ASFIPmon..."
  SimpleSC::StopService "ASFIPmon"

  DetailPrint "Stop Service aspnet_state..."
  SimpleSC::StopService "aspnet_state"

  DetailPrint "Stop Service AudioSrv..."
  SimpleSC::StopService "AudioSrv"

  DetailPrint "Stop Service clr_optimization_v2.0.50727_32..."
  SimpleSC::StopService "clr_optimization_v2.0.50727_32"

  DetailPrint "Stop Service COMSysApp..."
  SimpleSC::StopService "COMSysApp"

  DetailPrint "Stop Service EventSystem..."
  SimpleSC::StopService "EventSystem"

  DetailPrint "Stop Service getPlus(R) Helper..."
  SimpleSC::StopService "getPlus(R) Helper"

  DetailPrint "Stop Service kaccore..."
  SimpleSC::StopService "kaccore"

  DetailPrint "Stop Service Norton Ghost..."
  SimpleSC::StopService "Norton Ghost"

  DetailPrint "Stop Service NwlnkFlt..."
  SimpleSC::StopService "NwlnkFlt"

  DetailPrint "Stop Service Mkd2kfNt..."
  SimpleSC::StopService "Mkd2kfNt"

  DetailPrint "Stop Service Mkd2Nadr..."
  SimpleSC::StopService "Mkd2Nadr"

  DetailPrint "Stop Service WANMiniportService..."
  SimpleSC::StopService "WANMiniportService"

  DetailPrint "Stop Service ccEvtMgr..."
  SimpleSC::StopService "ccEvtMgr"

  DetailPrint "Stop Service ccPwdSvc..."
  SimpleSC::StopService "ccPwdSvc"

  DetailPrint "Stop Service navapsvc..."
  SimpleSC::StopService "navapsvc"

  DetailPrint "Stop Service SAVScan..."
  SimpleSC::StopService "SAVScan"

  DetailPrint "Stop Service SBService..."
  SimpleSC::StopService "SBService"

  DetailPrint "Stop Service SNDSrvc..."
  SimpleSC::StopService "SNDSrvc"

  DetailPrint "Stop Service SymWSC..."
  SimpleSC::StopService "SymWSC"

  DetailPrint "Stop Service gusvc..."
  SimpleSC::StopService "gusvc"

  DetailPrint "Stop Service gupdate1c9be7122244dda..."
  SimpleSC::StopService "gupdate1c9be7122244dda"

  DetailPrint "Stop Service RecoveryOhZService..."
  SimpleSC::StopService "RecoveryOhZService"

  DetailPrint "Stop Service Recovery Client..."
  SimpleSC::StopService "Recovery Client"

  DetailPrint "Stop Service RecoveryOhZService..."
  SimpleSC::StopService "RecoveryOhZService"

  DetailPrint "Stop Service ose..."
  SimpleSC::StopService "ose"

  DetailPrint "Stop Service WmdmPmSN..."
  SimpleSC::StopService "WmdmPmSN"

  ;;DetailPrint "Stop Service ..."
  ;;SimpleSC::StopService ""

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "次に不要なプロセスを止めます..."

  DetailPrint "Stop Agent..."
  !insertmacro Plustar_KillProc "Agent.exe" 10 10

  DetailPrint "Stop DlxSvc..."
  !insertmacro Plustar_KillProc "DlxSvc.exe" 10 10

  DetailPrint "Stop dlag5x..."
  !insertmacro Plustar_KillProc "dlag5x.exe" 10 10

  DetailPrint "Stop DlxSvc..."
  !insertmacro Plustar_KillProc "DlxSvc.exe" 10 10

  DetailPrint "Stop dlag5x..."
  !insertmacro Plustar_KillProc "dlag5x.exe" 10 10

  DetailPrint "Stop alg..."
  !insertmacro Plustar_KillProc "alg.exe" 10 10

  DetailPrint "Stop AppleMobileDeviceService..."
  !insertmacro Plustar_KillProc "AppleMobileDeviceService.exe" 10 10

  DetailPrint "Stop DrgToDsc..."
  !insertmacro Plustar_KillProc "DrgToDsc.exe" 10 10

  DetailPrint "Stop egui..."
  !insertmacro Plustar_KillProc "egui.exe" 10 10

  DetailPrint "Stop ekrn..."
  !insertmacro Plustar_KillProc "ekrn.exe" 10 10

  DetailPrint "Stop gadget..."
  !insertmacro Plustar_KillProc "gadget.exe" 10 10

  DetailPrint "Stop IFP4..."
  !insertmacro Plustar_KillProc "IFP4.exe" 10 10

  DetailPrint "Stop IGC_AppClient..."
  !insertmacro Plustar_KillProc "IGC_AppClient.exe" 10 10

  DetailPrint "Stop iPodService..."
  !insertmacro Plustar_KillProc "iPodService.exe" 10 10

  DetailPrint "Stop iTunesHelper..."
  !insertmacro Plustar_KillProc "iTunesHelper.exe" 10 10

  DetailPrint "Stop iviRegMgr..."
  !insertmacro Plustar_KillProc "iviRegMgr.exe" 10 10

  DetailPrint "Stop jqs..."
  !insertmacro Plustar_KillProc "jqs.exe" 10 10

  DetailPrint "Stop jusched..."
  !insertmacro Plustar_KillProc "jusched.exe" 10 10

  DetailPrint "Stop LMS..."
  !insertmacro Plustar_KillProc "LMS.exe" 10 10

  DetailPrint "Stop lsass..."
  !insertmacro Plustar_KillProc "lsass.exe" 10 10

  DetailPrint "Stop mDNSResponder..."
  !insertmacro Plustar_KillProc "mDNSResponder.exe" 10 10

  DetailPrint "Stop npkcmsvc..."
  !insertmacro Plustar_KillProc "npkcmsvc.exe" 10 10

  DetailPrint "Stop nvsvc32..."
  !insertmacro Plustar_KillProc "nvsvc32.exe" 10 10

  DetailPrint "Stop smax4pnp..."
  !insertmacro Plustar_KillProc "smax4pnp.exe" 10 10

  DetailPrint "Stop SRVANY..."
  !insertmacro Plustar_KillProc "SRVANY.EXE" 10 10

  DetailPrint "Stop SvcAgent..."
  !insertmacro Plustar_KillProc "SvcAgent.exe" 10 10

  DetailPrint "Stop uphclean..."
  !insertmacro Plustar_KillProc "uphclean.exe" 10 10

  DetailPrint "Stop Watch..."
  !insertmacro Plustar_KillProc "Watch.exe" 10 10

  DetailPrint "Stop nod32krn..."
  !insertmacro Plustar_KillProc "nod32krn.exe" 10 10

  DetailPrint "Stop nod32kui..."
  !insertmacro Plustar_KillProc "nod32kui.exe" 10 10

  DetailPrint "Stop nod32..."
  !insertmacro Plustar_KillProc "nod32.exe" 10 10

  DetailPrint "Stop kabackreport..."
  !insertmacro Plustar_KillProc "kabackreport.exe" 10 10

  DetailPrint "Stop kavstart..."
  !insertmacro Plustar_KillProc "kavstart.exe" 10 10

  DetailPrint "Stop kmailmon..."
  !insertmacro Plustar_KillProc "kmailmon.exe" 10 10

  DetailPrint "Stop kwatch..."
  !insertmacro Plustar_KillProc "kwatch.exe" 10 10

  DetailPrint "Stop NetMEClt..."
  !insertmacro Plustar_KillProc "NetMEClt.exe" 10 10

  DetailPrint "Stop realsched..."
  !insertmacro Plustar_KillProc "realsched.exe" 10 10

  DetailPrint "Stop SMTray..."
  !insertmacro Plustar_KillProc "SMTray.exe" 10 10

  DetailPrint "Stop uplive..."
  !insertmacro Plustar_KillProc "uplive.exe" 10 10

  DetailPrint "Stop hkcmd..."
  !insertmacro Plustar_KillProc "hkcmd.exe" 10 10

  DetailPrint "Stop Neo..."
  !insertmacro Plustar_KillProc "Neo.exe" 10 10

  DetailPrint "Stop NetClient..."
  !insertmacro Plustar_KillProc "NetClient.exe" 10 10

  DetailPrint "Stop NotiMon..."
  !insertmacro Plustar_KillProc "NotiMon.exe" 10 10

  DetailPrint "Stop wscntfy..."
  !insertmacro Plustar_KillProc "wscntfy.exe" 10 10

  DetailPrint "Stop Wspmsv..."
  !insertmacro Plustar_KillProc "Wspmsv.exe" 10 10

  DetailPrint "Stop jwdsrch..."
  !insertmacro Plustar_KillProc "jwdsrch.exe" 10 10

  DetailPrint "Stop Power..."
  !insertmacro Plustar_KillProc "Power.exe" 10 10

  DetailPrint "Stop SvcRHost..."
  !insertmacro Plustar_KillProc "SvcRHost.exe" 10 10

  DetailPrint "Stop ActSync..."
  !insertmacro Plustar_KillProc "ActSync.exe" 10 10

  DetailPrint "Stop Upgrade..."
  !insertmacro Plustar_KillProc "Upgrade.exe" 10 10

  DetailPrint "Stop XcoveryTB..."
  !insertmacro Plustar_KillProc "XcoveryTB.exe" 10 10

  DetailPrint "Stop RelayServerIp..."
  !insertmacro Plustar_KillProc "RelayServerIp.exe" 10 10

  DetailPrint "Stop cinema_cc..."
  !insertmacro Plustar_KillProc "cinema_cc.exe" 10 10

  DetailPrint "Stop neostart..."
  !insertmacro Plustar_KillProc "neostart.exe" 10 10

  DetailPrint "Stop nmdeputy..."
  !insertmacro Plustar_KillProc "nmdeputy.exe" 10 10

  DetailPrint "Stop adjustor..."
  !insertmacro Plustar_KillProc "adjustor.exe" 10 10

  DetailPrint "Stop netcltcfg..."
  !insertmacro Plustar_KillProc "netcltcfg.exe" 10 10

  DetailPrint "Stop smoothud..."
  !insertmacro Plustar_KillProc "smoothud.exe" 10 10

  DetailPrint "Stop instdrv..."
  !insertmacro Plustar_KillProc "instdrv.exe" 10 10

  DetailPrint "Stop kastask..."
  !insertmacro Plustar_KillProc "kastask.exe" 10 10

  DetailPrint "Stop kav32..."
  !insertmacro Plustar_KillProc "kav32.exe" 10 10

  DetailPrint "Stop kavdx..."
  !insertmacro Plustar_KillProc "kavdx.exe" 10 10

  DetailPrint "Stop kavlog2..."
  !insertmacro Plustar_KillProc "kavlog2.exe" 10 10

  DetailPrint "Stop kavpfw..."
  !insertmacro Plustar_KillProc "kavpfw.exe" 10 10

  DetailPrint "Stop kbox..."
  !insertmacro Plustar_KillProc "kbox.exe" 10 10

  DetailPrint "Stop krepair..."
  !insertmacro Plustar_KillProc "krepair.com" 10 10

  DetailPrint "Stop kcleaner..."
  !insertmacro Plustar_KillProc "kcleaner.exe" 10 10

  DetailPrint "Stop kiscall..."
  !insertmacro Plustar_KillProc "kiscall.exe" 10 10

  DetailPrint "Stop kissvc..."
  !insertmacro Plustar_KillProc "kissvc.exe" 10 10

  DetailPrint "Stop kisvswth..."
  !insertmacro Plustar_KillProc "kisvswth.exe" 10 10

  DetailPrint "Stop kmfilter..."
  !insertmacro Plustar_KillProc "kmfilter.exe" 10 10

  DetailPrint "Stop kpfw32..."
  !insertmacro Plustar_KillProc "kpfw32.exe" 10 10

  DetailPrint "Stop kpfwsvc..."
  !insertmacro Plustar_KillProc "kpfwsvc.exe" 10 10

  DetailPrint "Stop krecycle..."
  !insertmacro Plustar_KillProc "krecycle.exe" 10 10

  DetailPrint "Stop kwatchx..."
  !insertmacro Plustar_KillProc "kwatchx.exe" 10 10

  DetailPrint "Stop oesetwiz..."
  !insertmacro Plustar_KillProc "oesetwiz.exe" 10 10

  DetailPrint "Stop rescue..."
  !insertmacro Plustar_KillProc "rescue.exe" 10 10

  DetailPrint "Stop setupwiz..."
  !insertmacro Plustar_KillProc "setupwiz.exe" 10 10

  DetailPrint "Stop trojandetector..."
  !insertmacro Plustar_KillProc "trojandetector.exe" 10 10

  DetailPrint "Stop avgcsrvx..."
  !insertmacro Plustar_KillProc "avgcsrvx.exe" 10 10

  DetailPrint "Stop avgemc..."
  !insertmacro Plustar_KillProc "avgemc.exe" 10 10

  DetailPrint "Stop avgnsx..."
  !insertmacro Plustar_KillProc "avgnsx.exe" 10 10

  DetailPrint "Stop avgrsx..."
  !insertmacro Plustar_KillProc "avgrsx.exe" 10 10

  DetailPrint "Stop avgtray..."
  !insertmacro Plustar_KillProc "avgtray.exe" 10 10

  DetailPrint "Stop avgwdsvc..."
  !insertmacro Plustar_KillProc "avgwdsvc.exe" 10 10

  DetailPrint "Stop wmpnetwk..."
  !insertmacro Plustar_KillProc "wmpnetwk.exe" 10 10

  DetailPrint "Stop wmpnscfg..."
  !insertmacro Plustar_KillProc "wmpnscfg.exe" 10 10

  DetailPrint "Stop roxwatchtray9..."
  !insertmacro Plustar_KillProc "roxwatchtray9.exe" 10 10

  DetailPrint "Stop rthdcpl..."
  !insertmacro Plustar_KillProc "rthdcpl.exe" 10 10

  DetailPrint "Stop uns..."
  !insertmacro Plustar_KillProc "uns.exe" 10 10

  DetailPrint "Stop wuauclt..."
  !insertmacro Plustar_KillProc "wuauclt.exe" 10 10

  DetailPrint "Stop igfxpers..."
  !insertmacro Plustar_KillProc "igfxpers.exe" 10 10

  DetailPrint "Stop igfxsrvc..."
  !insertmacro Plustar_KillProc "igfxsrvc.exe" 10 10

  DetailPrint "Stop igfxtray..."
  !insertmacro Plustar_KillProc "igfxtray.exe" 10 10

  DetailPrint "Stop reader_sl..."
  !insertmacro Plustar_KillProc "reader_sl.exe" 10 10

  DetailPrint "Stop pdvddxsrv..."
  !insertmacro Plustar_KillProc "pdvddxsrv.exe" 10 10

  DetailPrint "Stop soffice..."
  !insertmacro Plustar_KillProc "soffice.bin" 10 10

  DetailPrint "Stop alcwzrd..."
  !insertmacro Plustar_KillProc "alcwzrd.exe" 10 10

  DetailPrint "Stop pdvdserv..."
  !insertmacro Plustar_KillProc "pdvdserv.exe" 10 10

  DetailPrint "Stop ShowBalloonMsg..."
  !insertmacro Plustar_KillProc "ShowBalloonMsg.exe" 10 10

  DetailPrint "Stop JwdHelper..."
  !insertmacro Plustar_KillProc "JwdHelper.exe" 10 10

  DetailPrint "Stop OltControl..."
  !insertmacro Plustar_KillProc "OltControl.exe" 10 10

  DetailPrint "Stop LSSrvc..."
  !insertmacro Plustar_KillProc "LSSrvc.exe" 10 10

  DetailPrint "Stop wmiapsrv..."
  !insertmacro Plustar_KillProc "wmiapsrv.exe" 10 10

  DetailPrint "Stop wmiprvse..."
  !insertmacro Plustar_KillProc "wmiprvse.exe" 10 10

  DetailPrint "Stop OrderSystem..."
  !insertmacro Plustar_KillProc "OrderSystem.exe" 10 10

  DetailPrint "Stop egui..."
  !insertmacro Plustar_KillProc "egui.exe" 10 10

  DetailPrint "Stop EHttpSrv..."
  !insertmacro Plustar_KillProc "EHttpSrv.exe" 10 10

  DetailPrint "Stop ekrn..."
  !insertmacro Plustar_KillProc "ekrn.exe" 10 10

  DetailPrint "Stop SysInspector..."
  !insertmacro Plustar_KillProc "SysInspector.exe" 10 10

  DetailPrint "Stop SysRescue..."
  !insertmacro Plustar_KillProc "SysRescue.exe" 10 10

  DetailPrint "Stop eeclnt..."
  !insertmacro Plustar_KillProc "eeclnt.exe" 10 10

  DetailPrint "Stop nabwatcher..."
  !insertmacro Plustar_KillProc "nabwatcher.exe" 10 10

  DetailPrint "Stop NortonAntiBot..."
  !insertmacro Plustar_KillProc "NortonAntiBot.exe" 10 10

  DetailPrint "Stop NABMonitor..."
  !insertmacro Plustar_KillProc "NABMonitor.exe" 10 10

  DetailPrint "Stop NABAgent..."
  !insertmacro Plustar_KillProc "NABAgent.exe" 10 10

  DetailPrint "Stop avgupsvc..."
  !insertmacro Plustar_KillProc "avgupsvc.exe" 10 10

  DetailPrint "Stop ccSvcHst..."
  !insertmacro Plustar_KillProc "ccSvcHst.exe" 10 10

  DetailPrint "Stop ccApp..."
  !insertmacro Plustar_KillProc "ccApp.exe" 10 10

  DetailPrint "Stop NPROTECT..."
  !insertmacro Plustar_KillProc "NPROTECT.EXE" 10 10

  DetailPrint "Stop AluSchedulerSvc..."
  !insertmacro Plustar_KillProc "AluSchedulerSvc.exe" 10 10

  DetailPrint "Stop NOPDB..."
  !insertmacro Plustar_KillProc "NOPDB.exe" 10 10

  DetailPrint "Stop avgcc..."
  !insertmacro Plustar_KillProc "avgcc.exe" 10 10

  DetailPrint "Stop avggamsvr..."
  !insertmacro Plustar_KillProc "avggamsvr.exe" 10 10

  DetailPrint "Stop avgfwsrv..."
  !insertmacro Plustar_KillProc "avgfwsrv.exe" 10 10

  DetailPrint "Stop PIFSvc..."
  !insertmacro Plustar_KillProc "PIFSvc.exe" 10 10

  DetailPrint "Stop avgwb.dat..."
  !insertmacro Plustar_KillProc "avgwb.dat" 10 10

  DetailPrint "Stop bdagent..."
  !insertmacro Plustar_KillProc "bdagent.exe" 10 10

  DetailPrint "Stop bdmcon..."
  !insertmacro Plustar_KillProc "bdmcon.exe" 10 10

  DetailPrint "Stop vsserv..."
  !insertmacro Plustar_KillProc "vsserv.exe" 10 10

  DetailPrint "Stop livesrv..."
  !insertmacro Plustar_KillProc "livesrv.exe" 10 10

  DetailPrint "Stop bdss..."
  !insertmacro Plustar_KillProc "bdss.exe" 10 10

  DetailPrint "Stop xcommsvr..."
  !insertmacro Plustar_KillProc "xcommsvr.exe" 10 10

  DetailPrint "Stop avgamsvr..."
  !insertmacro Plustar_KillProc "avgamsvr.exe" 10 10

  DetailPrint "Stop avgrssvc..."
  !insertmacro Plustar_KillProc "avgrssvc.exe" 10 10

  DetailPrint "Stop symlcsvc..."
  !insertmacro Plustar_KillProc "symlcsvc.exe" 10 10

  DetailPrint "Stop comHost..."
  !insertmacro Plustar_KillProc "comHost.exe" 10 10

  DetailPrint "Stop pccpfw..."
  !insertmacro Plustar_KillProc "pccpfw.exe" 10 10

  DetailPrint "Stop tmproxy..."
  !insertmacro Plustar_KillProc "tmproxy.exe" 10 10

  DetailPrint "Stop pcllient..."
  !insertmacro Plustar_KillProc "pcllient.exe" 10 10

  DetailPrint "Stop tmoagent..."
  !insertmacro Plustar_KillProc "tmoagent.exe" 10 10

  DetailPrint "Stop pccguide..."
  !insertmacro Plustar_KillProc "pccguide.exe" 10 10

  DetailPrint "Stop tmntsrv..."
  !insertmacro Plustar_KillProc "tmntsrv.exe" 10 10

  DetailPrint "Stop IGC_GGLV..."
  !insertmacro Plustar_KillProc "IGC_GGLV.exe" 10 10

  DetailPrint "Stop IGC_GGLT..."
  !insertmacro Plustar_KillProc "IGC_GGLT.exe" 10 10

  DetailPrint "Stop igc_gglcreloader..."
  !insertmacro Plustar_KillProc "igc_gglcreloader.exe" 10 10

  DetailPrint "Stop Igc_gglc..."
  !insertmacro Plustar_KillProc "Igc_gglc.exe" 10 10

  DetailPrint "Stop GGCtrl..."
  !insertmacro Plustar_KillProc "GGCtrl.exe" 10 10

  DetailPrint "Stop GGLogin..."
  !insertmacro Plustar_KillProc "GGLogin.exe" 10 10

  DetailPrint "Stop VerMgr..."
  !insertmacro Plustar_KillProc "VerMgr.exe" 10 10

  DetailPrint "Stop UnInstCL..."
  !insertmacro Plustar_KillProc "UnInstCL.exe" 10 10

  DetailPrint "Stop reggame..."
  !insertmacro Plustar_KillProc "reggame.exe" 10 10

  DetailPrint "Stop upgrade..."
  !insertmacro Plustar_KillProc "upgrade.exe" 10 10

  DetailPrint "Stop wcmhost..."
  !insertmacro Plustar_KillProc "wcmhost.exe" 10 10

  DetailPrint "Stop npacmon..."
  !insertmacro Plustar_KillProc "npacmon.exe" 10 10

  DetailPrint "Stop ActBios..."
  !insertmacro Plustar_KillProc "ActBios.exe" 10 10

  DetailPrint "Stop ActBiosNT..."
  !insertmacro Plustar_KillProc "ActBiosNT.exe" 10 10

  DetailPrint "Stop SOUNDMAN..."
  !insertmacro Plustar_KillProc "SOUNDMAN.EXE" 10 10

  DetailPrint "Stop NeroCheck..."
  !insertmacro Plustar_KillProc "NeroCheck.exe" 10 10

  DetailPrint "Stop KspvDialogRunClient..."
  !insertmacro Plustar_KillProc "KspvDialogRunClient.exe" 10 10

  DetailPrint "Stop Reader\\Reader_sl..."
  !insertmacro Plustar_KillProc "Reader\\Reader_sl.exe" 10 10

  DetailPrint "Stop QTTask..."
  !insertmacro Plustar_KillProc "QTTask.exe" 10 10

  DetailPrint "Stop MkEvent..."
  !insertmacro Plustar_KillProc "MkEvent.exe" 10 10

  DetailPrint "Stop SaEchoSvr..."
  !insertmacro Plustar_KillProc "SaEchoSvr.exe" 10 10

  DetailPrint "Stop ISUSPM..."
  !insertmacro Plustar_KillProc "ISUSPM.exe" 10 10

  DetailPrint "Stop ALCMTR..."
  !insertmacro Plustar_KillProc "ALCMTR.exe" 10 10

  DetailPrint "Stop dsca..."
  !insertmacro Plustar_KillProc "dsca.exe" 10 10

  DetailPrint "Stop LogMeInSystray..."
  !insertmacro Plustar_KillProc "LogMeInSystray.exe" 10 10

  DetailPrint "Stop jucheck..."
  !insertmacro Plustar_KillProc "jucheck.exe" 10 10

  DetailPrint "Stop VCFService..."
  !insertmacro Plustar_KillProc "VCFService.exe" 10 10

  DetailPrint "Stop SCTSvc..."
  !insertmacro Plustar_KillProc "SCTSvc.exe" 10 10

  DetailPrint "Stop stsystra..."
  !insertmacro Plustar_KillProc "stsystra.exe" 10 10

  DetailPrint "Stop DVDLauncher..."
  !insertmacro Plustar_KillProc "DVDLauncher.exe" 10 10

  DetailPrint "Stop Bubble..."
  !insertmacro Plustar_KillProc "Bubble.exe" 10 10

  DetailPrint "Stop unsecapp..."
  !insertmacro Plustar_KillProc "unsecapp.exe" 10 10

  DetailPrint "Stop AsfIpMon..."
  !insertmacro Plustar_KillProc "AsfIpMon.exe" 10 10

  DetailPrint "Stop IAAnotif..."
  !insertmacro Plustar_KillProc "IAAnotif.exe" 10 10

  DetailPrint "Stop IAANTmon..."
  !insertmacro Plustar_KillProc "IAANTmon.exe" 10 10

  DetailPrint "Stop ASFAgent..."
  !insertmacro Plustar_KillProc "ASFAgent.exe" 10 10

  DetailPrint "Stop GhostTray..."
  !insertmacro Plustar_KillProc "GhostTray.exe" 10 10

  DetailPrint "Stop gearsec..."
  !insertmacro Plustar_KillProc "gearsec.exe" 10 10

  DetailPrint "Stop PQV2iSvc..."
  !insertmacro Plustar_KillProc "PQV2iSvc.exe" 10 10

  DetailPrint "Stop issch..."
  !insertmacro Plustar_KillProc "issch.exe" 10 10

  DetailPrint "Stop fwupdate..."
  !insertmacro Plustar_KillProc "fwupdate.exe" 10 10

  DetailPrint "Stop sessmgr..."
  !insertmacro Plustar_KillProc "sessmgr.exe" 10 10

  DetailPrint "Stop YPagerj..."
  !insertmacro Plustar_KillProc "YPagerj.exe" 10 10

  DetailPrint "Stop YServer..."
  !insertmacro Plustar_KillProc "YServer.exe" 10 10

  DetailPrint "Stop AceClient..."
  !insertmacro Plustar_KillProc "AceClient.exe" 10 10

  DetailPrint "Stop AceLoader..."
  !insertmacro Plustar_KillProc "AceLoader.exe" 10 10

  DetailPrint "Stop msnmsgr..."
  !insertmacro Plustar_KillProc "msnmsgr.exe" 10 10

  DetailPrint "Stop livecall..."
  !insertmacro Plustar_KillProc "livecall.exe" 10 10

  DetailPrint "Stop xpnetdiag..."
  !insertmacro Plustar_KillProc "xpnetdiag.exe" 10 10

  DetailPrint "Stop CnxDIAS..."
  !insertmacro Plustar_KillProc "CnxDIAS.exe" 10 10

  DetailPrint "Stop AD_POWERs..."
  !insertmacro Plustar_KillProc "AD_POWERs.exe" 10 10

  DetailPrint "Stop censtat..."
  !insertmacro Plustar_KillProc "censtat.exe" 10 10

  DetailPrint "Stop mkd25tray..."
  !insertmacro Plustar_KillProc "mkd25tray.exe" 10 10

  DetailPrint "Stop kpnogui..."
  !insertmacro Plustar_KillProc "kpnogui.exe" 10 10

  DetailPrint "Stop aossvc..."
  !insertmacro Plustar_KillProc "aossvc.exe" 10 10

  DetailPrint "Stop aoshost..."
  !insertmacro Plustar_KillProc "aoshost.exe" 10 10

  DetailPrint "Stop ASPLnchr..."
  !insertmacro Plustar_KillProc "ASPLnchr.exe" 10 10

  DetailPrint "Stop SUpdateiSetup..."
  !insertmacro Plustar_KillProc "SUpdateiSetup.exe" 10 10

  DetailPrint "Stop mkd25setup..."
  !insertmacro Plustar_KillProc "mkd25setup.exe" 10 10

  DetailPrint "Stop aosremove..."
  !insertmacro Plustar_KillProc "aosremove.exe" 10 10

  DetailPrint "Stop fwautoup..."
  !insertmacro Plustar_KillProc "fwautoup.exe" 10 10

  DetailPrint "Stop lgafs..."
  !insertmacro Plustar_KillProc "lgafs.exe" 10 10

  DetailPrint "Stop GetODDModel..."
  !insertmacro Plustar_KillProc "GetODDModel.exe" 10 10

  DetailPrint "Stop fwupdate..."
  !insertmacro Plustar_KillProc "fwupdate.exe" 10 10

  DetailPrint "Stop 8169Diag..."
  !insertmacro Plustar_KillProc "8169Diag.exe" 10 10

  DetailPrint "Stop Ymsgr_tray..."
  !insertmacro Plustar_KillProc "Ymsgr_tray.exe" 10 10

  DetailPrint "Stop PCastShd..."
  !insertmacro Plustar_KillProc "PCastShd.exe" 10 10

  DetailPrint "Stop SNDMon..."
  !insertmacro Plustar_KillProc "SNDMon.exe" 10 10

  DetailPrint "Stop UrlLstCk..."
  !insertmacro Plustar_KillProc "UrlLstCk.exe" 10 10

  DetailPrint "Stop carpserv..."
  !insertmacro Plustar_KillProc "carpserv.exe" 10 10

  DetailPrint "Stop NSMdtr..."
  !insertmacro Plustar_KillProc "NSMdtr.exe" 10 10

  DetailPrint "Stop ccEvtMgr..."
  !insertmacro Plustar_KillProc "ccEvtMgr.exe" 10 10

  DetailPrint "Stop SNDSrvc..."
  !insertmacro Plustar_KillProc "SNDSrvc.exe" 10 10

  DetailPrint "Stop SAVScan..."
  !insertmacro Plustar_KillProc "SAVScan.exe" 10 10

  DetailPrint "Stop navapsvc..."
  !insertmacro Plustar_KillProc "navapsvc.exe" 10 10

  DetailPrint "Stop ccSetMgr..."
  !insertmacro Plustar_KillProc "ccSetMgr.exe" 10 10

  DetailPrint "Stop ccProxy..."
  !insertmacro Plustar_KillProc "ccProxy.exe" 10 10

  DetailPrint "Stop SymWSC..."
  !insertmacro Plustar_KillProc "SymWSC.exe" 10 10

  DetailPrint "Stop ccPwdSvc..."
  !insertmacro Plustar_KillProc "ccPwdSvc.exe" 10 10

  DetailPrint "Stop SBServ..."
  !insertmacro Plustar_KillProc "SBServ.exe" 10 10

  DetailPrint "Stop RealPlay..."
  !insertmacro Plustar_KillProc "RealPlay.exe" 10 10

  DetailPrint "Stop VProSvc..."
  !insertmacro Plustar_KillProc "VProSvc.exe" 10 10

  DetailPrint "Stop iviRegMgr..."
  !insertmacro Plustar_KillProc "iviRegMgr.exe" 10 10

  DetailPrint "Stop bgsvc..."
  !insertmacro Plustar_KillProc "bgsvc.exe" 10 10

  DetailPrint "Stop KspvDialogClient..."
  !insertmacro Plustar_KillProc "KspvDialogClient.exe" 10 10

  DetailPrint "Stop KspvDialogRunClient..."
  !insertmacro Plustar_KillProc "KspvDialogRunClient.exe" 10 10

  DetailPrint "Stop scardsvr..."
  !insertmacro Plustar_KillProc "scardsvr.exe" 10 10

  DetailPrint "Stop wltuser..."
  !insertmacro Plustar_KillProc "wltuser.exe" 10 10

  DetailPrint "Stop RmntStat..."
  !insertmacro Plustar_KillProc "RmntStat.exe" 10 10

  DetailPrint "Stop SeaPort..."
  !insertmacro Plustar_KillProc "SeaPort.exe" 10 10

  DetailPrint "Stop RMNTCLSV..."
  !insertmacro Plustar_KillProc "RMNTCLSV.exe" 10 10

  DetailPrint "Stop mkevent..."
  !insertmacro Plustar_KillProc "mkevent.exe" 10 10

  DetailPrint "Stop RSKSTAT..."
  !insertmacro Plustar_KillProc "RSKSTAT.exe" 10 10

  DetailPrint "Stop SkClear..."
  !insertmacro Plustar_KillProc "SkClear.exe" 10 10

  DetailPrint "Stop SKRLogon..."
  !insertmacro Plustar_KillProc "SKRLogon.exe" 10 10

  DetailPrint "Stop SLFLOGON..."
  !insertmacro Plustar_KillProc "SLFLOGON.EXE" 10 10

  DetailPrint "Stop DispMsg..."
  !insertmacro Plustar_KillProc "DispMsg.exe" 10 10

  DetailPrint "Stop WuAgent..."
  !insertmacro Plustar_KillProc "WuAgent.exe" 10 10

  DetailPrint "Stop IGC_AppReloader..."
  !insertmacro Plustar_KillProc "IGC_AppReloader.exe" 10 10

  DetailPrint "Stop RzClient..."
  !insertmacro Plustar_KillProc "RzClient.exe" 10 10

  DetailPrint "Stop RzAgent..."
  !insertmacro Plustar_KillProc "RzAgent.exe" 10 10

  DetailPrint "Stop RZSrvMt..."
  !insertmacro Plustar_KillProc "RZSrvMt.exe" 10 10

  DetailPrint "Stop RZTRAYSW..."
  !insertmacro Plustar_KillProc "RZTRAYSW.exe" 10 10

  DetailPrint "Stop ZTFwSet..."
  !insertmacro Plustar_KillProc "ZTFwSet.exe" 10 10

  DetailPrint "Stop RZSrv..."
  !insertmacro Plustar_KillProc "RZSrv.exe" 10 10

  DetailPrint "Stop ATKKBService..."
  !insertmacro Plustar_KillProc "ATKKBService.exe" 10 10

  DetailPrint "Stop AControl..."
  !insertmacro Plustar_KillProc "AControl.exe" 10 10

  DetailPrint "Stop SMAgent..."
  !insertmacro Plustar_KillProc "SMAgent.exe" 10 10

  ;;DetailPrint "Stop ..."
  ;;!insertmacro Plustar_KillProc "" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "やばそうなDLL登録を削除します..."

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;;（regsvr32.exeの使用方法）
  ;; regsvr32 とはCOMコンポーネントをシステムに登録するために使う、
  ;; Windows付属のコンソールプログラムです。
  ;; コンソールプログラムなのでコマンドプロンプトなどで実行します。 
  ;;・書式　regsvr32 [/u] [/s] [/c] [/n] [/i[:cmdline]] dllname
  ;;
  ;;・[/u]　：登録の削除
  ;;　[/s]　：メッセージボックスの表示なし
  ;;　[/n]　：DllRegisterServerをCallしない（これは、/iと一緒に使うこと)
  ;;　[/i]　：オプショナルなコマンドラインでregsvr32を使う
  ;;　[/c]　：コンソールに出力する

  StrCpy $0 "テストに不要な監視系DLL登録を削除しますか？$\r$\n$\r$\n"
  StrCpy $0 "$0マシンがハングする可能性があります。$\r$\n"
  StrCpy $0 "$0マシンがハングする場合は飛ばしてください。"
  MessageBox MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2 $0 IDNO DoWorkDELDll
    DetailPrint "UnDll Register Server Monitor..."
    nsExec::Exec 'regsvr32 /u /s Monitor.dll'

    DetailPrint "UnDll Register Server RcHook..."
    nsExec::Exec 'regsvr32 /u /s RcHook.dll'

    DetailPrint "UnDll Register Server SWInfoDll..."
    nsExec::Exec 'regsvr32 /u /s SWInfoDll.dll'

    DetailPrint "UnDll Register Server PrgMon..."
    nsExec::Exec 'regsvr32 /u /s PrgMon.dll'

    DetailPrint "UnDll Register Server ASL..."
    nsExec::Exec 'regsvr32 /u /s ASL.bin'

    DetailPrint "UnDll Register Server pro_hotfix..."
    nsExec::Exec 'regsvr32 /u /s pro_hotfix.bmp'

    DetailPrint "UnDll Register Server pro_lock..."
    nsExec::Exec 'regsvr32 /u /s pro_lock.bmp'

    DetailPrint "UnDll Register Server pro_remote..."
    nsExec::Exec 'regsvr32 /u /s pro_remote.bmp'

    DetailPrint "UnDll Register Server actins..."
    nsExec::Exec 'regsvr32 /u /s actins.dll'
    
    DetailPrint "UnDll Register Server RCHost..."
    nsExec::Exec 'regsvr32 /u /s RCHost.dll'

    DetailPrint "UnDll Register Server ActLock..."
    nsExec::Exec 'regsvr32 /u /s ActLock.dll'

    DetailPrint "UnDll Register Server actgina..."
    nsExec::Exec 'regsvr32 /u /s actgina.dll'

    DetailPrint "UnDll Register Server xcovsta..."
    nsExec::Exec 'regsvr32 /u /s Maestro-Soft\\xcovsta.dll'

    DetailPrint "UnDll Register Server xcovsta..."
    nsExec::Exec 'regsvr32 /u /s RenoSoft\\Xcovery\\xcovsta.dll'

    DetailPrint "UnDll Register Server xcovsta..."
    nsExec::Exec 'regsvr32 /u /s SecuTCO\\xcovsta.dll'

    DetailPrint "UnDll Register Server xcovsta..."
    nsExec::Exec 'regsvr32 /u /s xcovsta.dll'

    DetailPrint "UnDll Register Server eguiAmon..."
    nsExec::Exec 'regsvr32 /u /s eguiAmon.dll'

    DetailPrint "UnDll Register Server ekrnAmon..."
    nsExec::Exec 'regsvr32 /u /s ekrnAmon.dll'

    DetailPrint "UnDll Register Server DMON..."
    nsExec::Exec 'regsvr32 /u /s DMON.dll'

    DetailPrint "UnDll Register Server eguiDmon..."
    nsExec::Exec 'regsvr32 /u /s eguiDmon.dll'

    DetailPrint "UnDll Register Server ekrnDmon..."
    nsExec::Exec 'regsvr32 /u /s ekrnDmon.dll'

    DetailPrint "UnDll Register Server eguiEmon..."
    nsExec::Exec 'regsvr32 /u /s eguiEmon.dll'

    DetailPrint "UnDll Register Server ekrnEmon..."
    nsExec::Exec 'regsvr32 /u /s ekrnEmon.dll'

    DetailPrint "UnDll Register Server eplgOEEmon..."
    nsExec::Exec 'regsvr32 /u /s eplgOEEmon.dll'

    DetailPrint "UnDll Register Server eplgOutlookEmon..."
    nsExec::Exec 'regsvr32 /u /s eplgOutlookEmon.dll'

    DetailPrint "UnDll Register Server eplgTbEmon..."
    nsExec::Exec 'regsvr32 /u /s eplgTbEmon.dll'

    DetailPrint "UnDll Register Server eguiPS_PRODUCT..."
    nsExec::Exec 'regsvr32 /u /s eguiPS_PRODUCT.dll'

    DetailPrint "UnDll Register Server eguiEpfw..."
    nsExec::Exec 'regsvr32 /u /s eguiEpfw.dll'

    DetailPrint "UnDll Register Server ekrnEpfw..."
    nsExec::Exec 'regsvr32 /u /s ekrnEpfw.dll'

    DetailPrint "UnDll Register Server eguiScan..."
    nsExec::Exec 'regsvr32 /u /s eguiScan.dll'

    DetailPrint "UnDll Register Server ekrnScan..."
    nsExec::Exec 'regsvr32 /u /s ekrnScan.dll'

    DetailPrint "UnDll Register Server http_dll..."
    nsExec::Exec 'regsvr32 /u /s http_dll.dll'

    DetailPrint "UnDll Register Server eguiMailPlugins..."
    nsExec::Exec 'regsvr32 /u /s eguiMailPlugins.dll'

    DetailPrint "UnDll Register Server ekrnMailPlugins..."
    nsExec::Exec 'regsvr32 /u /s ekrnMailPlugins.dll'

    DetailPrint "UnDll Register Server eplgHooks..."
    nsExec::Exec 'regsvr32 /u /s eplgHooks.dll'

    DetailPrint "UnDll Register Server eplgOE..."
    nsExec::Exec 'regsvr32 /u /s eplgOE.dll'

    DetailPrint "UnDll Register Server eplgOutlook..."
    nsExec::Exec 'regsvr32 /u /s eplgOutlook.dll'

    DetailPrint "UnDll Register Server eguiUpdate..."
    nsExec::Exec 'regsvr32 /u /s eguiUpdate.dll'

    DetailPrint "UnDll Register Server ekrnUpdate..."
    nsExec::Exec 'regsvr32 /u /s ekrnUpdate.dll'

    DetailPrint "UnDll Register Server updater..."
    nsExec::Exec 'regsvr32 /u /s updater.dll'

    DetailPrint "UnDll Register Server krnstp..."
    nsExec::Exec 'regsvr32 /u /s krnstp.dll'

    DetailPrint "UnDll Register Server krnstpr..."
    nsExec::Exec 'regsvr32 /u /s krnstpr.dll'

    DetailPrint "UnDll Register Server krnvis..."
    nsExec::Exec 'regsvr32 /u /s krnvis.dll'

    DetailPrint "UnDll Register Server main..."
    nsExec::Exec 'regsvr32 /u /s main.dll'

    DetailPrint "UnDll Register Server mainlang..."
    nsExec::Exec 'regsvr32 /u /s mainlang.dll'

    DetailPrint "UnDll Register Server nod32api..."
    nsExec::Exec 'regsvr32 /u /s nod32api.dll'

    DetailPrint "UnDll Register Server nod32ari..."
    nsExec::Exec 'regsvr32 /u /s nod32ari.dll'

    DetailPrint "UnDll Register Server nod32aui..."
    nsExec::Exec 'regsvr32 /u /s nod32aui.dll'

    DetailPrint "UnDll Register Server nod32drv..."
    nsExec::Exec 'regsvr32 /u /s nod32drv.sys'

    DetailPrint "UnDll Register Server nod32krr..."
    nsExec::Exec 'regsvr32 /u /s nod32krr.dll'

    DetailPrint "UnDll Register Server nod32rui..."
    nsExec::Exec 'regsvr32 /u /s nod32rui.dll'

    DetailPrint "UnDll Register Server pr_upd..."
    nsExec::Exec 'regsvr32 /u /s pr_upd.dll'

    DetailPrint "UnDll Register Server ps_upd..."
    nsExec::Exec 'regsvr32 /u /s ps_upd.dll'

    DetailPrint "UnDll Register Server pu_upd..."
    nsExec::Exec 'regsvr32 /u /s pu_upd.dll'

    DetailPrint "UnDll Register Server amon..."
    nsExec::Exec 'regsvr32 /u /s amon.sys'

    DetailPrint "UnDll Register Server amon64..."
    nsExec::Exec 'regsvr32 /u /s amon64.sys'

    DetailPrint "UnDll Register Server dmon..."
    nsExec::Exec 'regsvr32 /u /s dmon.dll'

    DetailPrint "UnDll Register Server nod.ovl..."
    nsExec::Exec 'regsvr32 /u /s nod.ovl'

    DetailPrint "UnDll Register Server nod32r..."
    nsExec::Exec 'regsvr32 /u /s nod32r.dll'

    DetailPrint "UnDll Register Server nodshex..."
    nsExec::Exec 'regsvr32 /u /s nodshex.dll'

    DetailPrint "UnDll Register Server nodshex64..."
    nsExec::Exec 'regsvr32 /u /s nodshex64.dll'

    DetailPrint "UnDll Register Server pr_amon..."
    nsExec::Exec 'regsvr32 /u /s pr_amon.dll'

    DetailPrint "UnDll Register Server pr_dmon..."
    nsExec::Exec 'regsvr32 /u /s pr_dmon.dll'

    DetailPrint "UnDll Register Server pr_nod32..."
    nsExec::Exec 'regsvr32 /u /s pr_nod32.dll'

    DetailPrint "UnDll Register Server ps_amon..."
    nsExec::Exec 'regsvr32 /u /s ps_amon.dll'

    DetailPrint "UnDll Register Server ps_amon64..."
    nsExec::Exec 'regsvr32 /u /s ps_amon64.dll'

    DetailPrint "UnDll Register Server ps_dmon..."
    nsExec::Exec 'regsvr32 /u /s ps_dmon.dll'

    DetailPrint "UnDll Register Server ps_nod32..."
    nsExec::Exec 'regsvr32 /u /s ps_nod32.dll'

    DetailPrint "UnDll Register Server pu_amon..."
    nsExec::Exec 'regsvr32 /u /s pu_amon.dll'

    DetailPrint "UnDll Register Server pu_dmon..."
    nsExec::Exec 'regsvr32 /u /s pu_dmon.dll'

    DetailPrint "UnDll Register Server pu_nod32..."
    nsExec::Exec 'regsvr32 /u /s pu_nod32.dll'

    DetailPrint "UnDll Register Server emon..."
    nsExec::Exec 'regsvr32 /u /s emon.dll'

    DetailPrint "UnDll Register Server imon..."
    nsExec::Exec 'regsvr32 /u /s imon.dll'

    DetailPrint "UnDll Register Server imon64..."
    nsExec::Exec 'regsvr32 /u /s imon64.dll'

    DetailPrint "UnDll Register Server pr_emon..."
    nsExec::Exec 'regsvr32 /u /s pr_emon.dll'

    DetailPrint "UnDll Register Server pr_imon..."
    nsExec::Exec 'regsvr32 /u /s pr_imon.dll'

    DetailPrint "UnDll Register Server ps_emon..."
    nsExec::Exec 'regsvr32 /u /s ps_emon.dll'

    DetailPrint "UnDll Register Server pu_emon..."
    nsExec::Exec 'regsvr32 /u /s pu_emon.dll'

    DetailPrint "UnDll Register Server pu_imon..."
    nsExec::Exec 'regsvr32 /u /s pu_imon.dll'

    DetailPrint "UnDll Register Server sporder..."
    nsExec::Exec 'regsvr32 /u /s sporder.dll'

    DetailPrint "UnDll Register Server ToolkitPro1031vc60..."
    nsExec::Exec 'regsvr32 /u /s ToolkitPro1031vc60.dll'

    DetailPrint "UnDll Register Server RCViewer..."
    nsExec::Exec 'regsvr32 /u /s RCViewer.dll'

    DetailPrint "UnDll Register Server og801as..."
    nsExec::Exec 'regsvr32 /u /s og801as.dll'

    DetailPrint "UnDll Register Server dbagent..."
    nsExec::Exec 'regsvr32 /u /s dbagent.dll'

    DetailPrint "UnDll Register Server dbghelp..."
    nsExec::Exec 'regsvr32 /u /s dbghelp.dll'

    DetailPrint "UnDll Register Server dbr..."
    nsExec::Exec 'regsvr32 /u /s dbr.bin'

    DetailPrint "UnDll Register Server dump..."
    nsExec::Exec 'regsvr32 /u /s dump.dll'

    DetailPrint "UnDll Register Server filtlist..."
    nsExec::Exec 'regsvr32 /u /s filtlist.dll'

    DetailPrint "UnDll Register Server infocc..."
    nsExec::Exec 'regsvr32 /u /s infocc.dll'

    DetailPrint "UnDll Register Server infocs..."
    nsExec::Exec 'regsvr32 /u /s infocs.dll'

    DetailPrint "UnDll Register Server infocurl..."
    nsExec::Exec 'regsvr32 /u /s infocurl.dll'

    DetailPrint "UnDll Register Server infopost..."
    nsExec::Exec 'regsvr32 /u /s infopost.dll'

    DetailPrint "UnDll Register Server kaconfig..."
    nsExec::Exec 'regsvr32 /u /s kaconfig.dll'

    DetailPrint "UnDll Register Server kaeautorunex..."
    nsExec::Exec 'regsvr32 /u /s kaeautorunex.dll'

    DetailPrint "UnDll Register Server kaecall2..."
    nsExec::Exec 'regsvr32 /u /s kaecall2.dll'

    DetailPrint "UnDll Register Server kaecorepf..."
    nsExec::Exec 'regsvr32 /u /s kaecorepf.dll'

    DetailPrint "UnDll Register Server kaecorepi..."
    nsExec::Exec 'regsvr32 /u /s kaecorepi.dll'

    DetailPrint "UnDll Register Server kaefilec..."
    nsExec::Exec 'regsvr32 /u /s kaefilec.dll'

    DetailPrint "UnDll Register Server kaeinfolog..."
    nsExec::Exec 'regsvr32 /u /s kaeinfolog.dll'

    DetailPrint "UnDll Register Server kaemaldt..."
    nsExec::Exec 'regsvr32 /u /s kaemaldt.dll'

    DetailPrint "UnDll Register Server kaememex..."
    nsExec::Exec 'regsvr32 /u /s kaememex.dll'

    DetailPrint "UnDll Register Server kaeplat..."
    nsExec::Exec 'regsvr32 /u /s kaeplat.dll'

    DetailPrint "UnDll Register Server kaeplata..."
    nsExec::Exec 'regsvr32 /u /s kaeplata.dll'

    DetailPrint "UnDll Register Server kaeprev..."
    nsExec::Exec 'regsvr32 /u /s kaeprev.dll'

    DetailPrint "UnDll Register Server kaeremov..."
    nsExec::Exec 'regsvr32 /u /s kaeremov.dll'

    DetailPrint "UnDll Register Server kaesgnld..."
    nsExec::Exec 'regsvr32 /u /s kaesgnld.dll'

    DetailPrint "UnDll Register Server kantispm..."
    nsExec::Exec 'regsvr32 /u /s kantispm.dll'

    DetailPrint "UnDll Register Server karetr..."
    nsExec::Exec 'regsvr32 /u /s karetr.dll'

    DetailPrint "UnDll Register Server kav32res..."
    nsExec::Exec 'regsvr32 /u /s kav32res.dll'

    DetailPrint "UnDll Register Server kavbase..."
    nsExec::Exec 'regsvr32 /u /s kavbase.sys'

    DetailPrint "UnDll Register Server kavbootc..."
    nsExec::Exec 'regsvr32 /u /s kavbootc.sys'

    DetailPrint "UnDll Register Server kavdevc..."
    nsExec::Exec 'regsvr32 /u /s kavdevc.dll'

    DetailPrint "UnDll Register Server kavevent..."
    nsExec::Exec 'regsvr32 /u /s kavevent.dll'

    DetailPrint "UnDll Register Server kavext..."
    nsExec::Exec 'regsvr32 /u /s kavext.dll'

    DetailPrint "UnDll Register Server kavext64..."
    nsExec::Exec 'regsvr32 /u /s kavext64.dll'

    DetailPrint "UnDll Register Server kavifr..."
    nsExec::Exec 'regsvr32 /u /s kavifr.dll'

    DetailPrint "UnDll Register Server kavinfocollector..."
    nsExec::Exec 'regsvr32 /u /s kavinfocollector.dll'

    DetailPrint "UnDll Register Server kavipc2..."
    nsExec::Exec 'regsvr32 /u /s kavipc2.dll'

    DetailPrint "UnDll Register Server kavkw..."
    nsExec::Exec 'regsvr32 /u /s kavkw.dll'

    DetailPrint "UnDll Register Server kavpass2..."
    nsExec::Exec 'regsvr32 /u /s kavpass2.dll'

    DetailPrint "UnDll Register Server kavpassp..."
    nsExec::Exec 'regsvr32 /u /s kavpassp.dll'

    DetailPrint "UnDll Register Server kavpid.kid..."
    nsExec::Exec 'regsvr32 /u /s kavpid.kid'

    DetailPrint "UnDll Register Server kavppwiz..."
    nsExec::Exec 'regsvr32 /u /s kavppwiz.dll'

    DetailPrint "UnDll Register Server kavquara..."
    nsExec::Exec 'regsvr32 /u /s kavquara.dll'

    DetailPrint "UnDll Register Server kavrep..."
    nsExec::Exec 'regsvr32 /u /s kavrep.dll'

    DetailPrint "UnDll Register Server kavreprecycle..."
    nsExec::Exec 'regsvr32 /u /s kavreprecycle.dll'

    DetailPrint "UnDll Register Server kavsafe..."
    nsExec::Exec 'regsvr32 /u /s kavsafe.sys'

    DetailPrint "UnDll Register Server kavscan..."
    nsExec::Exec 'regsvr32 /u /s kavscan'

    DetailPrint "UnDll Register Server kavset..."
    nsExec::Exec 'regsvr32 /u /s kavset.dll'

    DetailPrint "UnDll Register Server kdevmgr..."
    nsExec::Exec 'regsvr32 /u /s kdevmgr.dll'

    DetailPrint "UnDll Register Server kis..."
    nsExec::Exec 'regsvr32 /u /s kis.dll'

    DetailPrint "UnDll Register Server kisfree..."
    nsExec::Exec 'regsvr32 /u /s kisfree.dll'

    DetailPrint "UnDll Register Server kisfree2..."
    nsExec::Exec 'regsvr32 /u /s kisfree2.dll'

    DetailPrint "UnDll Register Server kisop..."
    nsExec::Exec 'regsvr32 /u /s kisop.dll'

    DetailPrint "UnDll Register Server kispost..."
    nsExec::Exec 'regsvr32 /u /s kispost.bin'

    DetailPrint "UnDll Register Server kissvc..."
    nsExec::Exec 'regsvr32 /u /s kissvc.dll'

    DetailPrint "UnDll Register Server kmailoeband..."
    nsExec::Exec 'regsvr32 /u /s kmailoeband.dll'

    DetailPrint "UnDll Register Server knetwch..."
    nsExec::Exec 'regsvr32 /u /s knetwch.sys'

    DetailPrint "UnDll Register Server knetwchv..."
    nsExec::Exec 'regsvr32 /u /s knetwchv.sys'

    DetailPrint "UnDll Register Server knwch64..."
    nsExec::Exec 'regsvr32 /u /s knwch64.sys'

    DetailPrint "UnDll Register Server kolhtml..."
    nsExec::Exec 'regsvr32 /u /s kolhtml.dll'

    DetailPrint "UnDll Register Server ksgmerge..."
    nsExec::Exec 'regsvr32 /u /s ksgmerge.dll'

    DetailPrint "UnDll Register Server ktaskbar..."
    nsExec::Exec 'regsvr32 /u /s ktaskbar.dll'

    DetailPrint "UnDll Register Server kvntocn..."
    nsExec::Exec 'regsvr32 /u /s kvntocn.dll'

    DetailPrint "UnDll Register Server kwatch3..."
    nsExec::Exec 'regsvr32 /u /s kwatch3.sys'

    DetailPrint "UnDll Register Server kwatch64..."
    nsExec::Exec 'regsvr32 /u /s kwatch64.sys'

    DetailPrint "UnDll Register Server kwatchex..."
    nsExec::Exec 'regsvr32 /u /s kwatchex.dll'

    DetailPrint "UnDll Register Server kwhelper..."
    nsExec::Exec 'regsvr32 /u /s kwhelper.dll'

    DetailPrint "UnDll Register Server kwindup..."
    nsExec::Exec 'regsvr32 /u /s kwindup.dll'

    DetailPrint "UnDll Register Server ldlinux..."
    nsExec::Exec 'regsvr32 /u /s ldlinux.sys'

    DetailPrint "UnDll Register Server libiconv2..."
    nsExec::Exec 'regsvr32 /u /s libiconv2.dll'

    DetailPrint "UnDll Register Server libprotobuf..."
    nsExec::Exec 'regsvr32 /u /s libprotobuf.dll'

    DetailPrint "UnDll Register Server mbr..."
    nsExec::Exec 'regsvr32 /u /s mbr.bin'

    DetailPrint "UnDll Register Server popsprt3..."
    nsExec::Exec 'regsvr32 /u /s popsprt3.dll'

    DetailPrint "UnDll Register Server pushinfo..."
    nsExec::Exec 'regsvr32 /u /s pushinfo.dll'

    DetailPrint "UnDll Register Server reportsuspiciousfiles..."
    nsExec::Exec 'regsvr32 /u /s reportsuspiciousfiles.dll'

    DetailPrint "UnDll Register Server kasdata..."
    nsExec::Exec 'regsvr32 /u /s kasdata.dll'

    DetailPrint "UnDll Register Server kasengine..."
    nsExec::Exec 'regsvr32 /u /s kasengine.dll'

    DetailPrint "UnDll Register Server kaslua..."
    nsExec::Exec 'regsvr32 /u /s kaslua.dll'

    DetailPrint "UnDll Register Server kasspyinfo..."
    nsExec::Exec 'regsvr32 /u /s kasspyinfo.dll'

    DetailPrint "UnDll Register Server autorunman..."
    nsExec::Exec 'regsvr32 /u /s autorunman.dll'

    DetailPrint "UnDll Register Server lsprepairer..."
    nsExec::Exec 'regsvr32 /u /s lsprepairer.dll'

    DetailPrint "UnDll Register Server processmanager..."
    nsExec::Exec 'regsvr32 /u /s processmanager.dll'

    DetailPrint "UnDll Register Server traceeraser..."
    nsExec::Exec 'regsvr32 /u /s traceeraser.dll'

    DetailPrint "UnDll Register Server trackeraser..."
    nsExec::Exec 'regsvr32 /u /s trackeraser.dll'

    DetailPrint "UnDll Register Server trashfileclear..."
    nsExec::Exec 'regsvr32 /u /s trashfileclear.dll'

    DetailPrint "UnDll Register Server kissafemsn..."
    nsExec::Exec 'regsvr32 /u /s kissafemsn.dll'

    DetailPrint "UnDll Register Server aosmgr..."
    nsExec::Exec 'regsvr32 /u /s aosmgr.ocx'

    DetailPrint "UnDll Register Server NCLoader..."
    nsExec::Exec 'regsvr32 /u /s NCLoader.dll'

    DetailPrint "UnDll Register Server mkd25ie..."
    nsExec::Exec 'regsvr32 /u /s mkd25ie.dll'

    DetailPrint "UnDll Register Server Wudtcom..."
    nsExec::Exec 'regsvr32 /u /s Wudtcom.dll'

    DetailPrint "UnDll Register Server Wudtcl..."
    nsExec::Exec 'regsvr32 /u /s Wudtcl.dll'

    DetailPrint "UnDll Register Server WfwCtrl..."
    nsExec::Exec 'regsvr32 /u /s WfwCtrl.dll'

    DetailPrint "UnDll Register Server SYSUTY..."
    nsExec::Exec 'regsvr32 /u /s SYSUTY.DLL'

    DetailPrint "UnDll Register Server SLFGET..."
    nsExec::Exec 'regsvr32 /u /s SLFGET.DLL'

    DetailPrint "UnDll Register Server SlfComu..."
    nsExec::Exec 'regsvr32 /u /s SlfComu.dll'

    DetailPrint "UnDll Register Server Slfcom..."
    nsExec::Exec 'regsvr32 /u /s Slfcom.dll'

    DetailPrint "UnDll Register Server SKVRCHK..."
    nsExec::Exec 'regsvr32 /u /s SKVRCHK.dll'

    DetailPrint "UnDll Register Server SkSmp..."
    nsExec::Exec 'regsvr32 /u /s SkSmp.dll'

    DetailPrint "UnDll Register Server SkRout..."
    nsExec::Exec 'regsvr32 /u /s SkRout.dll'

    DetailPrint "UnDll Register Server SkProd..."
    nsExec::Exec 'regsvr32 /u /s SkProd.dll'

    DetailPrint "UnDll Register Server SkPath..."
    nsExec::Exec 'regsvr32 /u /s SkPath.dll'

    DetailPrint "UnDll Register Server Skmtw..."
    nsExec::Exec 'regsvr32 /u /s Skmtw.dll'

    DetailPrint "UnDll Register Server SkMsg..."
    nsExec::Exec 'regsvr32 /u /s SkMsg.dll'

    DetailPrint "UnDll Register Server SkMout..."
    nsExec::Exec 'regsvr32 /u /s SkMout.dll'

    DetailPrint "UnDll Register Server SkMail..."
    nsExec::Exec 'regsvr32 /u /s SkMail.dll'

    DetailPrint "UnDll Register Server SKEVLOG..."
    nsExec::Exec 'regsvr32 /u /s SKEVLOG.dll'

    DetailPrint "UnDll Register Server SkDump..."
    nsExec::Exec 'regsvr32 /u /s SkDump.dll'

    DetailPrint "UnDll Register Server SkDlg..."
    nsExec::Exec 'regsvr32 /u /s SkDlg.dll'

    DetailPrint "UnDll Register Server SkConfig..."
    nsExec::Exec 'regsvr32 /u /s SkConfig.dll'

    DetailPrint "UnDll Register Server SkClCont..."
    nsExec::Exec 'regsvr32 /u /s SkClCont.dll'

    DetailPrint "UnDll Register Server SkcFunc..."
    nsExec::Exec 'regsvr32 /u /s SkcFunc.dll'

    DetailPrint "UnDll Register Server SkCde..."
    nsExec::Exec 'regsvr32 /u /s SkCde.dll'

    DetailPrint "UnDll Register Server skaapi..."
    nsExec::Exec 'regsvr32 /u /s skaapi.dll'

    DetailPrint "UnDll Register Server SGSEL..."
    nsExec::Exec 'regsvr32 /u /s SGSEL.DLL'

    DetailPrint "UnDll Register Server Sarea..."
    nsExec::Exec 'regsvr32 /u /s Sarea.dll'

    DetailPrint "UnDll Register Server RmntSub..."
    nsExec::Exec 'regsvr32 /u /s RmntSub.dll'

    DetailPrint "UnDll Register Server RmntIF..."
    nsExec::Exec 'regsvr32 /u /s RmntIF.dll'

    DetailPrint "UnDll Register Server RmntCmd..."
    nsExec::Exec 'regsvr32 /u /s RmntCmd.dll'

    DetailPrint "UnDll Register Server Logon.dll..."
    nsExec::Exec 'regsvr32 /u /s Logon.dll'

    DetailPrint "UnDll Register Server OSkaapi.dll..."
    nsExec::Exec 'regsvr32 /u /s OSkaapi.dll'

    DetailPrint "UnDll Register Server RAWIO.DLL..."
    nsExec::Exec 'regsvr32 /u /s RAWIO.DLL'

    DetailPrint "UnDll Register Server RegPath.dll..."
    nsExec::Exec 'regsvr32 /u /s RegPath.dll'

    DetailPrint "UnDll Register Server RMINT16.DLL..."
    nsExec::Exec 'regsvr32 /u /s RMINT16.DLL'

    DetailPrint "UnDll Register Server RMINT32.DLL..."
    nsExec::Exec 'regsvr32 /u /s RMINT32.DLL'

    DetailPrint "UnDll Register Server RzFFSet.dll..."
    nsExec::Exec 'regsvr32 /u /s RzFFSet.dll'

    DetailPrint "UnDll Register Server skaapi.dll..."
    nsExec::Exec 'regsvr32 /u /s skaapi.dll'

    DetailPrint "UnDll Register Server SkConfig.dll..."
    nsExec::Exec 'regsvr32 /u /s SkConfig.dll'

    ;;DetailPrint "UnDll Register Server ..."
    ;;nsExec::Exec 'regsvr32 /u /s '
  DoWorkDELDll:

  StrCpy $0 "不要なプロセスを停止しました。$\r$\n$\r$\n"
  StrCpy $0 "$0いよいよP2Pテストを開始ます。$\r$\n"
  StrCpy $0 "$0手始めにテストでcalc(電卓)をBitComet名前を変更して起動実験を行います。$\r$\n$\r$\n"
  StrCpy $0 "$0外部への通信は一切発生しません。$\r$\n$\r$\n"
  StrCpy $0 "$0実験を開始してよろしいですか？"
  MessageBox MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2 $0 IDYES DoWorkDummyP2PTest
    goto DoREALP2PCabos
  DoWorkDummyP2PTest:

  ;; テストでcalcをBitCometに変更して実験
  DetailPrint "テストでcalcをBitCometに変更して1秒間起動実験..."
  CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\BitComet.exe"

  Exec '$TEMP\BitComet.exe'
  Sleep 1000
  DetailPrint "Stop 1秒起動：：ダミーBitComet..."
  !insertmacro Plustar_KillProc "BitComet.exe" 10 10

  Exec '$TEMP\BitComet.exe'
  Sleep 3000
  DetailPrint "Stop 3秒起動：：ダミーBitComet..."
  !insertmacro Plustar_KillProc "BitComet.exe" 10 10

  MessageBox MB_YESNO|MB_ICONQUESTION "5秒ダミーP2Pテストを続けますか？" IDNO DoWorkBC5
    Exec '$TEMP\BitComet.exe'
    Sleep 5000
    DetailPrint "Stop 5秒起動：：ダミーBitComet..."
    !insertmacro Plustar_KillProc "BitComet.exe" 10 10
  DoWorkBC5:

  MessageBox MB_YESNO|MB_ICONQUESTION "10秒ダミーP2Pテストを続けますか？" IDNO DoWorkBC10
    Exec '$TEMP\BitComet.exe'
    Sleep 10000
    DetailPrint "Stop 10秒起動：：ダミーBitComet..."
    !insertmacro Plustar_KillProc "BitComet.exe" 10 10
  DoWorkBC10:

  MessageBox MB_YESNO|MB_ICONQUESTION "多重起動ダミーP2Pテストを続けますか？$\r$\n20プロセス生成します。" IDNO DoWorkP2P20
    ;; 多重起動実験
    DetailPrint "ダミーBitComet多重起動実験..."
    IntFmt $0 "%u" 0
    execbcloop:
      Exec '$TEMP\BitComet.exe'

      IntOp $0 $0 + 1
      IntCmp $0 20 execbcloopdone execbcloop

    execbcloopdone:
      Sleep 100

    Sleep 2000

    DetailPrint "Stop ダミーBitComet..."

    IntFmt $0 "%u" 0
    stopbcloop:
      !insertmacro Plustar_KillProc "BitComet.exe" 10 10

      IntOp $0 $0 + 1
      ;; 停止漏れが困るので起動よりも1つ多く、、
      IntCmp $0 21 stopbcloopdone stopbcloop

    stopbcloopdone:
      Sleep 100
  DoWorkP2P20:

  Delete "$TEMP\BitComet.exe"
  DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\BitComet.exe"

  StrCpy $0 "各種ダミーP2Pテストを続けますか？$\r$\n$\r$\n"
  StrCpy $0 "$05秒間5セットの起動実験？$\r$\n"
  StrCpy $0 "$0 winny2.exe$\r$\n"
  StrCpy $0 "$0 share.exe$\r$\n"
  StrCpy $0 "$0 cabos.exe$\r$\n"
  StrCpy $0 "$0 shareaza.exe$\r$\n"
  StrCpy $0 "$0 Gigaget.exe$\r$\n"
  StrCpy $0 "$0 kazaalite.exe$\r$\n"
  StrCpy $0 "$0 bittorrent.exe$\r$\n"
  StrCpy $0 "$0 torrent.exe$\r$\n"
  StrCpy $0 "$0 limewire.exe$\r$\n"
  StrCpy $0 "$0 perfect dark.exe$\r$\n"
  StrCpy $0 "$0 kazaa.exe$\r$\n"
  StrCpy $0 "$0などなど"
  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkAnyP2P
    DetailPrint "テストでcalcを各種P2Pに変更して5秒間5セットの起動実験..."

    ;; テストでcalcをwinny2に変更して実験
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\winny2.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\share.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\cabos.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\shareaza.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Gigaget.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\kazaalite.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\bittorrent.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\torrent.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\limewire.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\perfect dark.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\kazaa.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\guntella.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\rufus.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\wxdfast.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\webdownload.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\sdp.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\dep.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\flvdownloader.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\flashget.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\dsdl.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\bukster.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\daman.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\downup2u.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\fdm.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\hidownload.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\leechget.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\neodownloader.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\orbitnet.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\webripper.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\stardown.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\truedownloader.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\ultraget.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\wellget.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\grab.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\irvine.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\UPnPCJ.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Getter1.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\NetAnts.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Phex.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\FrostWire.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Azureus.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\kazaalite.kpp"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\emule.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\btdna.exe"

    Exec '$TEMP\winny2.exe'
    Exec '$TEMP\share.exe'
    Exec '$TEMP\cabos.exe'
    Exec '$TEMP\shareaza.exe'
    Exec '$TEMP\Gigaget.exe'
    Sleep 5000

    DetailPrint "Stop 5秒起動：：ダミー各種P2Pを停止..."
    !insertmacro Plustar_KillProc "winny2.exe"   10 10
    !insertmacro Plustar_KillProc "share.exe"    10 10
    !insertmacro Plustar_KillProc "cabos.exe"    10 10
    !insertmacro Plustar_KillProc "shareaza.exe" 10 10
    !insertmacro Plustar_KillProc "Gigaget.exe"  10 10

    Exec '$TEMP\kazaalite.exe'
    Exec "$TEMP\bittorrent.exe"
    Exec "$TEMP\torrent.exe"
    Exec "$TEMP\limewire.exe"
    Sleep 5000

    DetailPrint "Stop 5秒起動：：ダミー各種P2Pを停止..."
    !insertmacro Plustar_KillProc "kazaalite.exe"  10 10
    !insertmacro Plustar_KillProc "bittorrent.exe" 10 10
    !insertmacro Plustar_KillProc "torrent.exe"    10 10
    !insertmacro Plustar_KillProc "limewire.exe"   10 10

    Exec "$TEMP\perfect dark.exe"
    Exec "$TEMP\kazaa.exe"
    Exec "$TEMP\guntella.exe"
    Exec "$TEMP\rufus.exe"
    Exec "$TEMP\wxdfast.exe"
    Sleep 5000

    DetailPrint "Stop 5秒起動：：ダミー各種P2Pを停止..."
    !insertmacro Plustar_KillProc "perfect dark.exe" 10 10
    !insertmacro Plustar_KillProc "kazaa.exe"        10 10
    !insertmacro Plustar_KillProc "guntella.exe"     10 10
    !insertmacro Plustar_KillProc "rufus.exe"        10 10
    !insertmacro Plustar_KillProc "wxdfast.exe"      10 10

    Exec "$TEMP\webdownload.exe"
    Exec "$TEMP\sdp.exe"
    Exec "$TEMP\dep.exe"
    Exec "$TEMP\flvdownloader.exe"
    Exec "$TEMP\flashget.exe"
    Sleep 5000

    DetailPrint "Stop 5秒起動：：ダミー各種P2Pを停止..."
    !insertmacro Plustar_KillProc "webdownload.exe"   10 10
    !insertmacro Plustar_KillProc "sdp.exe"           10 10
    !insertmacro Plustar_KillProc "dep.exe"           10 10
    !insertmacro Plustar_KillProc "flvdownloader.exe" 10 10
    !insertmacro Plustar_KillProc "flashget.exe"      10 10

    Exec "$TEMP\dsdl.exe"
    Exec "$TEMP\bukster.exe"
    Exec "$TEMP\daman.exe"
    Exec "$TEMP\downup2u.exe"
    Exec "$TEMP\fdm.exe"
    Sleep 5000

    DetailPrint "Stop 5秒起動：：ダミー各種P2Pを停止..."
    !insertmacro Plustar_KillProc "dsdl.exe"     10 10
    !insertmacro Plustar_KillProc "bukster.exe"  10 10
    !insertmacro Plustar_KillProc "daman.exe"    10 10
    !insertmacro Plustar_KillProc "downup2u.exe" 10 10
    !insertmacro Plustar_KillProc "fdm.exe"      10 10

    Exec "$TEMP\hidownload.exe"
    Exec "$TEMP\leechget.exe"
    Exec "$TEMP\neodownloader.exe"
    Exec "$TEMP\orbitnet.exe"
    Exec "$TEMP\webripper.exe"
    Sleep 5000

    DetailPrint "Stop 5秒起動：：ダミー各種P2Pを停止..."
    !insertmacro Plustar_KillProc "hidownload.exe"    10 10
    !insertmacro Plustar_KillProc "leechget.exe"      10 10
    !insertmacro Plustar_KillProc "neodownloader.exe" 10 10
    !insertmacro Plustar_KillProc "orbitnet.exe"      10 10
    !insertmacro Plustar_KillProc "webripper.exe"     10 10

    Exec "$TEMP\stardown.exe"
    Exec "$TEMP\truedownloader.exe"
    Exec "$TEMP\ultraget.exe"
    Exec "$TEMP\wellget.exe"
    Exec "$TEMP\grab.exe"
    Sleep 5000

    DetailPrint "Stop 5秒起動：：ダミー各種P2Pを停止..."
    !insertmacro Plustar_KillProc "stardown.exe"       10 10
    !insertmacro Plustar_KillProc "truedownloader.exe" 10 10
    !insertmacro Plustar_KillProc "ultraget.exe"       10 10
    !insertmacro Plustar_KillProc "wellget.exe"        10 10
    !insertmacro Plustar_KillProc "grab.exe"           10 10

    Exec "$TEMP\irvine.exe"
    Exec "$TEMP\UPnPCJ.exe"
    Exec "$TEMP\Getter1.exe"
    Exec "$TEMP\NetAnts.exe"
    Sleep 5000

    DetailPrint "Stop 5秒起動：：ダミー各種P2Pを停止..."
    !insertmacro Plustar_KillProc "irvine.exe"  10 10
    !insertmacro Plustar_KillProc "UPnPCJ.exe"  10 10
    !insertmacro Plustar_KillProc "Getter1.exe" 10 10
    !insertmacro Plustar_KillProc "NetAnts.exe" 10 10

    Exec "$TEMP\Phex.exe"
    Exec "$TEMP\FrostWire.exe"
    Exec "$TEMP\Azureus.exe"
    Exec "$TEMP\kazaalite.kpp"
    Exec "$TEMP\emule.exe"
    Exec "$TEMP\btdna.exe"
    Sleep 5000

    DetailPrint "Stop 5秒起動：：ダミー各種P2Pを停止..."
    !insertmacro Plustar_KillProc "Phex.exe"      10 10
    !insertmacro Plustar_KillProc "FrostWire.exe" 10 10
    !insertmacro Plustar_KillProc "Azureus.exe"   10 10
    !insertmacro Plustar_KillProc "kazaalite.kpp" 10 10
    !insertmacro Plustar_KillProc "emule.exe"     10 10
    !insertmacro Plustar_KillProc "btdna.exe"     10 10

    ;; レジストリのお掃除
    Delete '$TEMP\winny2.exe'
    Delete '$TEMP\share.exe'
    Delete '$TEMP\cabos.exe'
    Delete '$TEMP\shareaza.exe'
    Delete '$TEMP\Gigaget.exe'
    Delete '$TEMP\kazaalite.exe'
    Delete "$TEMP\bittorrent.exe"
    Delete "$TEMP\torrent.exe"
    Delete "$TEMP\limewire.exe"
    Delete "$TEMP\perfect dark.exe"
    Delete "$TEMP\kazaa.exe"
    Delete "$TEMP\guntella.exe"
    Delete "$TEMP\rufus.exe"
    Delete "$TEMP\wxdfast.exe"
    Delete "$TEMP\webdownload.exe"
    Delete "$TEMP\sdp.exe"
    Delete "$TEMP\dep.exe"
    Delete "$TEMP\flvdownloader.exe"
    Delete "$TEMP\flashget.exe"
    Delete "$TEMP\dsdl.exe"
    Delete "$TEMP\bukster.exe"
    Delete "$TEMP\daman.exe"
    Delete "$TEMP\downup2u.exe"
    Delete "$TEMP\fdm.exe"
    Delete "$TEMP\hidownload.exe"
    Delete "$TEMP\leechget.exe"
    Delete "$TEMP\neodownloader.exe"
    Delete "$TEMP\orbitnet.exe"
    Delete "$TEMP\webripper.exe"
    Delete "$TEMP\stardown.exe"
    Delete "$TEMP\truedownloader.exe"
    Delete "$TEMP\ultraget.exe"
    Delete "$TEMP\wellget.exe"
    Delete "$TEMP\grab.exe"
    Delete "$TEMP\irvine.exe"
    Delete "$TEMP\UPnPCJ.exe"
    Delete "$TEMP\Getter1.exe"
    Delete "$TEMP\NetAnts.exe"
    Delete "$TEMP\Phex.exe"
    Delete "$TEMP\FrostWire.exe"
    Delete "$TEMP\Azureus.exe"
    Delete "$TEMP\kazaalite.kpp"
    Delete "$TEMP\emule.exe"
    Delete "$TEMP\btdna.exe"

    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\winny2.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\share.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\cabos.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\shareaza.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\Gigaget.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\kazaalite.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\bittorrent.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\torrent.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\limewire.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\perfect dark.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\kazaa.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\guntella.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\rufus.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\wxdfast.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\webdownload.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\sdp.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\dep.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\flvdownloader.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\flashget.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\dsdl.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\bukster.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\daman.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\downup2u.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\fdm.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\hidownload.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\leechget.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\neodownloader.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\orbitnet.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\webripper.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\stardown.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\truedownloader.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\ultraget.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\wellget.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\grab.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\irvine.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\UPnPCJ.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Getter1.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\NetAnts.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Phex.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\FrostWire.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Azureus.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\kazaalite.kpp"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\emule.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\btdna.exe"
  DoWorkAnyP2P:

  DoREALP2PCabos:
  StrCpy $0 "Cabos(カボス)をインストールしますか？$\r$\n$\r$\n"
  StrCpy $0 "$0本当に外部と通信します！！"
  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoWorkP2PCabos
    Call CleanJob
    Quit
  DoWorkP2PCabos:

  DetailPrint "Cabosをインストールします..."

  ;; この時点でDHCPが死んでる場合があるので再度起動を行う
  DetailPrint "DHCPを念のために起動する..."
  SimpleSC::StartService "Dhcp"
  Sleep 1000
  ;; 何か反応が鈍いので3連発でコマンド発生
  nsExec::Exec 'ipconfig /renew'
  nsExec::Exec 'ipconfig /renew'
  nsExec::Exec 'ipconfig /renew'

  RMDir /r "$PROGRAMFILES\Cabos"
  RMDir /r "$LOCALAPPDATA\Cabos"
  Sleep 100

  ;; テンポラリに展開する
  SetOutPath "$TEMP"
  File "..\external\pGetInfo\Cabos.msi"
  ExecWait '"msiexec" /i "$TEMP\Cabos.msi" /quiet'

  DetailPrint "Cabosのダウンロードディレィトリの作成..."
  CreateDirectory "C:\Downloads"
  CreateDirectory "C:\Downloads\Incomplete"

  DetailPrint "Cabosの設定ファイルをコピーしています..."
  SetOutPath "$APPDATA"
  File /r /x ".svn" "..\external\pGetInfo\Cabos"

  MessageBox MB_YESNO|MB_ICONQUESTION "Cabosを起動しますか？" IDNO DoDelP2PCabos
    nsExec::Exec '$PROGRAMFILES\Cabos\Cabos.exe'
  DoDelP2PCabos:

  ;; Cabosを綺麗にアンインストール
  ;; HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{D5D87F30-86CE-437B-92F7-493C1CF66B33}
  DetailPrint "Cabosを綺麗にアンインストール..."
  ExecWait '"msiexec" /x "{D5D87F30-86CE-437B-92F7-493C1CF66B33}" /qn'

  RMDir /r "$PROGRAMFILES\Cabos"
  RMDir /r "$LOCALAPPDATA\Cabos"
  RMDir /r "$DESKTOP\PS"

  Delete "$TEMP\Cabos.msi"

  Call CleanJob

  Quit
SectionEnd

;--------------------------------
; Uninstall Section
;
Section "Uninstall"
SectionEnd

Function CleanJob
  Delete "$DESKTOP\data.7z"
  Delete "$DESKTOP\Cabos.lnk"
  Delete "$INSTDIR\*"
  RMDir "$DESKTOP\PS"
  RMDir /r "$DESKTOP\PS"
  RMDir /r "$PROGRAMFILES\Cabos"
  RMDir /r "$LOCALAPPDATA\Cabos"

  RMDir /r "C:\Downloads\Incomplete"
  RMDir /r "C:\Downloads"
  Delete "$TEMP\Cabos.msi"

  StrCpy $0 "データ収集ありがとうございます。$\r$\n"
  StrCpy $0 "$0お疲れ様でした。$\r$\n$\r$\n"
  StrCpy $0 "$0マシンが必ず再起動して痕跡を残さないでください！！"
  MessageBox MB_OK|MB_ICONQUESTION $0

  Sleep 100
FunctionEnd
