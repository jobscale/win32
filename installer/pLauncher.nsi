; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=plan -O Plustar_Version.nsh' ignore

!Define PS_PRODUCT      "pLauncher"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper Launcher"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT}"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExTrapper-Setup-${PS_PRODUCT}.exe"
InstallDir "$PROGRAMFILES\Plustar\${PS_PRODUCT}"

;インストールには管理者権限が必要
RequestExecutionLevel admin

; --------------------------------
; Include Lib
;
!include LogicLib.nsh
!include WordFunc.nsh
!include Plustar_Include.nsh
!include Plustar_Version.nsh

; --------------------------------
; Pages
;
BrandingText "${PS_PRODUCTNAME} [Rev:${PLUSTAR_VERSION}]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP   "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                 "icons\setup-pTop.ico"
!define MUI_UNICON               "icons\setup-pTop.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;; メッセージ変更
!define MUI_TEXT_INSTALLING_SUBTITLE "${PS_PRODUCTNAME}略して「pLauncher」をインストールしています。"

; --------------------------------
; UPX圧縮
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\src\pLauncher\Release\pLauncher.exe' ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\src\pCmd\Release\pCmd.exe'           ignore

; --------------------------------
; Languages
;
!insertmacro MUI_LANGUAGE "Japanese"

ShowInstDetails   nevershow
ShowUnInstDetails nevershow

;--------------------------------
; Install Section
;
section "Install"
  !insertmacro Plustar_DetailPrint "ファイル展開中..." 0

  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; 監視系プロセスの停止を行う
  !insertmacro Plustar_Kill_Spy_Proc

  !insertmacro Plustar_KillProc "pLauncher.exe" 100 1000
  !insertmacro Plustar_KillProc "pCmd.exe"      100 100

  ;; サービスの停止
  SimpleSC::StopService "plads"
  !insertmacro Plustar_CheckProcStopService "plads" 100

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "${PLUSTAR_VERSION}"

  ;; 実験中...検索ワードを発生させる
  !insertmacro Plustar_Search_Keyword

  ;; 互換
  ;;nsExec::Exec /TIMEOUT=5000 'regsvr32.exe /u /s "$INSTDIR\xulrunner\mozctlx.dll"'
  ;;RmDir /r "$INSTDIR\xulrunner"

  ;;File /r /x ".svn" "..\external\xulrunner"
  ;;File "..\Release\KB973923\vcredis1.cab"
  ;;File "..\Release\KB973923\vcredist.msi"

  File "..\src\pLauncher\Release\pLauncher.exe"
  File "..\src\pCmd\Release\pCmd.exe"

  ;; VC++8.0のランタイム
  ;;!insertmacro Plustar_DetailPrint "VC++ランタイムインストール中..." 100
  ;;ExecWait '"msiexec" /i "$INSTDIR\vcredist.msi" /passive /norestart /quiet'
  ;;Sleep 1000

  ;; 以下のF/W系は
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; が起動している事を前提としている
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pLauncher.exe" "$INSTDIR\pLauncher.exe" 0 2 "" 1
  SimpleFC::AddApplication "pCmd.exe"      "$INSTDIR\pCmd.exe"      0 2 "" 1

  ;; スタートアップに登録
  !insertmacro Plustar_DetailPrint "スタートアップに登録中..." 100
  CreateShortCut  "$SMSTARTUP\pLauncher.lnk" "$INSTDIR\pLauncher.exe" ""

  ;; plauncherのレジストリ登録
  WriteRegStr HKCR "plauncher" "" 'URL:pCmd Protocol'
  WriteRegStr HKCR "plauncher" "URL Protocol" ''
  WriteRegStr HKCR "plauncher\shell\open\command" "" '"$INSTDIR\\pCmd.exe" %1'

  ;; purlのレジストリ登録
  WriteRegStr HKCR "purl" "" 'URL:pCmd Protocol'
  WriteRegStr HKCR "purl" "URL Protocol" ''
  WriteRegStr HKCR "purl\shell\open\command" "" '"$INSTDIR\\pCmd.exe" %1'

  ;; アンイントーラの作成
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "${PS_PRODUCTNAME}"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        0
  WriteUninstaller "uninstall.exe"

  SimpleSC::StartService "plads" ""
  !insertmacro Plustar_CheckProcService "plads" 1500

  ;; COMの登録
  !insertmacro Plustar_DetailPrint "COMコンポーネントの登録中..." 100
  ;;nsExec::Exec /TIMEOUT=5000 'regsvr32.exe /s "$INSTDIR\xulrunner\mozctlx.dll"'

  nsExec::Exec /TIMEOUT=3000 '"$INSTDIR\pLauncher.exe"'

  !insertmacro Plustar_DetailPrint "インストール完了" 2000

  !insertmacro Plustar_CountDown_Message 5 "秒後に自動でインストーラは終了します。"

  SetAutoClose true
SectionEnd

;--------------------------------
; Uninstall Section
Section "Uninstall"
  SetDetailsPrint none
  SetDetailsView hide

  !insertmacro un.Plustar_DetailPrint "削除中..." 100

  ;; インストール統計を取得する
  !insertmacro un.Plustar_Stats_Install_Action "${PS_PRODUCT}" "${PLUSTAR_VERSION}"

  ;; pControler.exeが動いていた場合には停止を行う
  !insertmacro un.Plustar_KillProc "pLauncher.exe" 100 100
  !insertmacro un.Plustar_KillProc "pCmd.exe"      100 100

  !insertmacro Plustar_DetailPrint "COMコンポーネントの登録中..." 100
  ;;nsExec::Exec /TIMEOUT=5000 'regsvr32.exe /u /s "$INSTDIR\xulrunner\mozctlx.dll"'

  DeleteRegKey HKLM "${PS_REGUNINSTALL}"
  DeleteRegKey HKCR "plauncher"
  DeleteRegKey HKCR "purl"

  ;; F/W登録の解除
  SimpleFC::RemoveApplication "$INSTDIR\pLauncher.exe"
  SimpleFC::RemoveApplication "$INSTDIR\pCmd.exe"

  !insertmacro un.Plustar_DetailPrint "ディレクトリ削除中..." 2000
  Delete "$INSTDIR\*.cab"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.msi"
  RmDir /r "$INSTDIR\xulrunner"
  RmDir /r "$INSTDIR"
  Delete "$SMSTARTUP\pLauncher.lnk"

  !insertmacro un.Plustar_DetailPrint "アンインストール完了！" 2000

  !insertmacro un.Plustar_CountDown_Message 5 "秒後に自動でインストーラは終了します。"

  SetAutoClose true
SectionEnd
