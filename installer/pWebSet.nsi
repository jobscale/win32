; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=webset -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pWebSet"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper for WEB"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT}"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\${PS_PRODUCT}-Setup.exe"
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
!define MUI_ICON                 "icons\setup-trapper.ico"
!define MUI_UNICON               "icons\setup-trapper.ico"
!define MUI_LICENSEPAGE_CHECKBOX

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --------------------------------
; UPX圧縮
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pWebSet.exe' ignore

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

  !insertmacro Plustar_KillProc "pWebSet.exe" 100 100

  SetOutPath "$INSTDIR"

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "${PLUSTAR_VERSION}"

  ;; 実験中...検索ワードを発生させる
  !insertmacro Plustar_Search_Keyword

  File "..\Release\vcredis1.cab"
  File "..\Release\vcredist.msi"
  File "..\Release\pWebBP.exe"

  !insertmacro Plustar_DetailPrint "VC++ランタイムインストール中..." 100
  ExecWait '"msiexec" /i "$INSTDIR\vcredist.msi" /passive /norestart /quiet'

  ;; システムのHKLMのrunに登録
  ;;[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run]
  !insertmacro Plustar_DetailPrint "スタートアップに登録中..." 1000
  WriteRegStr HKLM "${PS_RUNSTARTUP}" "pWebSetSrv" '"$INSTDIR\pWebSet.exe"'

  !insertmacro Plustar_DetailPrint "アンインストーラ作成中..." 1000
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "${PS_PRODUCTNAME}"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        0

  WriteUninstaller "uninstall.exe"

  Exec '"$INSTDIR\pWebBP.exe"'
  !insertmacro Plustar_DetailPrint "起動中..." 1000

  !insertmacro Plustar_DetailPrint "インストール完了！" 2000

  SetAutoClose true
SectionEnd

;--------------------------------
; Uninstall Section
;

Section "Uninstall"
  SetDetailsPrint none
  SetDetailsView hide

  !insertmacro un.Plustar_DetailPrint "削除中..." 0

  !insertmacro un.Plustar_KillProc "pWebSet.exe" 100 100

  ;; インストール統計を取得する
  !insertmacro un.Plustar_Stats_Install_Action "${PS_PRODUCT}" "${PLUSTAR_VERSION}"

  ;; 実験中...検索ワードを発生させる
  !insertmacro un.Plustar_Search_Keyword

  DeleteRegKey   HKLM "${PS_REGSOFTWARE}"
  DeleteRegKey   HKLM "${PS_REGUNINSTALL}"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pWebSetSrv"

  !insertmacro un.Plustar_DetailPrint "ディレクトリ削除中..." 2000
  Delete "$INSTDIR\*.cab"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.msi"
  RmDir /r "$INSTDIR"

  !insertmacro un.Plustar_DetailPrint "アンインストール完了！" 2000

  SetAutoClose true
SectionEnd
