; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pSync"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper pSync Client"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Client"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pSync-Setup-Client.exe"
InstallDir "$PROGRAMFILES\Plustar\pSync"

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
!define MUI_HEADERIMAGE_BITMAP  "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                "icons\setup-trapper.ico"
!define MUI_UNICON              "icons\setup-trapper.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --------------------------------
; UPX圧縮
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pSyncScope.exe' ignore

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
  !insertmacro Plustar_DetailPrint "ファイル展開中…" 0

  ; none：メッセージなし
  ; textonly：テキスト表示
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; インストール対象のプロセスがあれば削除する
  !insertmacro Plustar_KillProc "pSyncScope.exe" 300 100
  !insertmacro Plustar_KillProc "psync.exe"      300 100

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "$$Rev: 2592 $$"

  ;; 実験中…検索ワードを発生させる
  !insertmacro Plustar_Search_Keyword
  ;;

  File "..\Release\pSyncScope.exe"
  File "..\external\pSync\client\cygiconv-2.dll"
  File "..\external\pSync\client\cygz.dll"
  File "..\external\pSync\client\cygwin1.dll"
  File "..\external\pSync\client\psync.exe"

  !insertmacro Plustar_DetailPrint "アンインストーラ作成中…" 1000
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "${PS_PRODUCTNAME}"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        1

  WriteUninstaller "uninstall.exe"

  ;; 以下のF/W系は
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; が起動している事を前提としている
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "psync.exe" "$INSTDIR\psync.exe" 0 2 "" 1

  !insertmacro Plustar_DetailPrint "インストール完了！" 2000

  !insertmacro Plustar_CountDown_Message 5 "秒後に自動でインストーラは終了します。"

  SetAutoClose true
SectionEnd

;--------------------------------
; Uninstall Section
;
Section "Uninstall"
  SetDetailsPrint none
  SetDetailsView hide

  !insertmacro un.Plustar_DetailPrint "削除中…" 0

  ;; インストール統計を取得する
  !insertmacro un.Plustar_Stats_Install_Action "${PS_PRODUCT}" "$$Rev: 2592 $$"

  !insertmacro un.Plustar_KillProc "pSyncScope.exe" 300 100
  !insertmacro un.Plustar_KillProc "psync.exe"      300 100

  SimpleFC::RemoveApplication "$INSTDIR\psync.exe"

  DeleteRegKey HKLM "${PS_REGSOFTWARE}"
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"

  Delete "$DESKTOP\クリップ.lnk"

  !insertmacro un.Plustar_DetailPrint "ディレクトリ削除中…" 2000
  Delete "$INSTDIR\*.pss"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"
  RmDir /r "$INSTDIR"

  !insertmacro un.Plustar_DetailPrint "アンインストール完了！" 2000

  !insertmacro un.Plustar_CountDown_Message 5 "秒後に自動でインストーラは終了します。"

  SetAutoClose true
SectionEnd
