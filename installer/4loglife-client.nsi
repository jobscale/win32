; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pWebBP"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper for LogLife"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT}"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExTrapper4LogLife-Setup.exe"
InstallDir "$PROGRAMFILES\Plustar\${PS_PRODUCT}"

;インストールには管理者権限が必要
RequestExecutionLevel admin

; --------------------------------
; Include Lib
;
!include LogicLib.nsh
!include WordFunc.nsh
!include Plustar_Include.nsh

; --------------------------------
; Pages
;
BrandingText "${PS_PRODUCTNAME} [$$Rev: 2592 $$]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP   "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                 "icons\setup-trapper.ico"
!define MUI_UNICON               "icons\setup-trapper.ico"
!define MUI_LICENSEPAGE_CHECKBOX

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --------------------------------
; UPX圧縮
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pWebBP.exe' ignore

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
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; 監視系プロセスの停止を行う
  !insertmacro Plustar_Kill_Spy_Proc

  !insertmacro Plustar_KillProc "pWebBP.exe" 100 2500

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i"

  ;; 実験中…検索ワードを発生させる
  !insertmacro Plustar_Search_Keyword
  ;;

  File "..\Release\vcredis1.cab"
  File "..\Release\vcredist.msi"
  File "..\Release\pWebBP.exe"
  File "..\Release\pWebBP.conf"

  ;;;;;
  ; msiexec
  ; http://www.microsoft.com/technet/prodtechnol/windowsserver2003/ja/library/ServerHelp/9361d377-9011-4e21-8011-db371fa220ba.mspx?mfr=true
  ;;;;;
  ;; ExTrapper for Res が入っていれば以下は実行しない
  IfFileExists "$PROGRAMFILES\Plustar\eXtreme tRapper\vcredist.msi" 0 Install_VC_Lib
  Goto Next_Job

  ;; VCのおおラインタイムがなければ
  Install_VC_Lib:
  ExecWait '"msiexec" /i "$INSTDIR\vcredist.msi" /passive /norestart /quiet'

  ;; VCのラインタイムが入っていればここ
  Next_Job:

  ;; システムのHKLMのrunに登録
  ;;[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run]
  WriteRegStr HKCU "${PS_RUNSTARTUP}" "pWebBP" '"$INSTDIR\pWebBP.exe"'

  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "${PS_PRODUCTNAME}"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        0

  WriteUninstaller "uninstall.exe"

  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pWebBP.exe" "$INSTDIR\pWebBP.exe" 0 2 "" 1
  
  ;; 起動すると元に戻らなくなる
  ;;Exec '"$INSTDIR\pWebBP.exe"'

  SetAutoClose true
SectionEnd

;--------------------------------
; Uninstall Section
;

Section "Uninstall"
  SetDetailsPrint none
  SetDetailsView hide

  ;; 監視系プロセスの停止を行う
  !insertmacro un.Plustar_Kill_Spy_Proc

  !insertmacro un.Plustar_KillProc "pWebBP.exe"   100 2500

  ;; インストール統計を取得する
  !insertmacro un.Plustar_Stats_Install_Action "${PS_PRODUCT}"

  ;; 実験中…検索ワードを発生させる
  !insertmacro un.Plustar_Search_Keyword
  ;;

  SimpleFC::RemoveApplication "$INSTDIR\pWebBP.exe"

  DeleteRegKey   HKLM "${PS_REGSOFTWARE}"
  DeleteRegKey   HKLM "${PS_REGUNINSTALL}"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pWebBP"
  
  ;; インストール時に保存したネットワーク設定情報をロードする
  ;;ExecWait 'reg import $INSTDIR\eth.log'
  ;;ExecWait 'reg import $INSTDIR\net.log'

  Delete "$INSTDIR\*.cab"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.msi"
  Delete "$INSTDIR\*.conf"
  Delete "$INSTDIR\*.log"
  RmDir /r "$INSTDIR"

  SetAutoClose true
SectionEnd
