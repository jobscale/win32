; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=pads -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pADs Tools"
!Define PS_PRODUCTNAME  "Plustar ADs Tools"
!Define PS_TOOLS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExtADs-Tools.exe"
InstallDir "$PROGRAMFILES\Plustar\pADs"

;管理者権限が必要
;;RequestExecutionLevel admin

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
!define MUI_HEADERIMAGE_BITMAP "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON               "icons\setup-pads.ico"
!define MUI_UNICON             "icons\setup-pads.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

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

  ; none：メッセージなし
  ; textonly：テキスト表示
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "pADst" "i" "${PLUSTAR_VERSION}"

  ;; 起動・停止コマンド
  File "..\pExtADs-restart-cmd.exe"
  File "..\pExtADs-stop-cmd.exe"

  CreateShortCut "$DESKTOP\pAds[起動].lnk" "$INSTDIR\pExtADs-restart-cmd.exe" "" ""
  CreateShortCut "$DESKTOP\pAds[停止].lnk" "$INSTDIR\pExtADs-stop-cmd.exe"    "" ""

  !insertmacro Plustar_DetailPrint "インストール完了" 2000
  !insertmacro Plustar_CountDown_Message 5 "秒後に自動でインストーラは終了します。"

  SetAutoClose true
SectionEnd
