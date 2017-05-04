; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=pads -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;

!Define PS_PRODUCT     "pExtADs-Tester"
!Define PS_PRODUCTNAME "Plustar pAds Tester"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\${PS_PRODUCT}.exe"
InstallDir "$TEMP\${PS_PRODUCT}"

;インストールには管理者権限が必要
RequestExecutionLevel admin

; --------------------------------
; Include Lib
;
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
!define MUI_ICON                 "icons\setup.ico"
!define MUI_UNICON               "icons\setup.ico"
!define MUI_LICENSEPAGE_CHECKBOX

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
ShowInstDetails show

section "Install"
  SetOutPath "$INSTDIR"

  StrCpy $0 "広告は出ていますか？$\r$\n$\r$\n"
  StrCpy $0 "$0出ている場合は「はい」を押して下さい。$\r$\n"
  StrCpy $0 "$0出ていない場合は「いいえ」を押して下さい。$\r$\n$\r$\n"
  StrCpy $0 "$0広告が出るまで表示が完了してから5秒の待ちが必要です。$\r$\n$\r$\n"
  StrCpy $0 "$0数ページ遷移もして下さい。"

  DetailPrint "pAdsのテストを行います..."

  DetailPrint "IE停止確認..."
  MessageBox MB_OK "テスト前にIEを強制終了させます。"
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://www.plustar.jp/index.html"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoWorkTest00
    MessageBox MB_OK "通常判定テストです。"
    Call CleanErrTest
  DoWorkTest00:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://video.fc2.com/a/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest10
    MessageBox MB_OK "アダルト判定テストです。"
    Call CleanErrTest
  DoWorkTest10:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://www.777town.net/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest11
    MessageBox MB_OK "オンラインゲームサイト判定テストです。"
    Call CleanErrTest
  DoWorkTest11:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://lineage2.plaync.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest12
    MessageBox MB_OK "同じくオンラインゲームサイト判定テストです。"
    Call CleanErrTest
  DoWorkTest12:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://jiqoo.jp/index.php"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest13
    MessageBox MB_OK "お客様ポータル判定テストです。"
    Call CleanErrTest
  DoWorkTest13:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://www.facebook.com/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoWorkTest01
    MessageBox MB_OK "通常判定テストです。"
    Call CleanErrTest
  DoWorkTest01:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://launcher.pso2.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest19
    MessageBox MB_OK "オンラインゲームサイト判定テストです。"
    Call CleanErrTest
  DoWorkTest19:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://pigg.ameba.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest14
    MessageBox MB_OK "デザイン潰れサイト判定テストです。"
    Call CleanErrTest
  DoWorkTest14:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://net-cash.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest15
    MessageBox MB_OK "デザイン潰れサイト判定テストです。"
    Call CleanErrTest
  DoWorkTest15:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://www.yamato-hd.co.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest16
    MessageBox MB_OK "一部上場企業サイト判定です。"
    Call CleanErrTest
  DoWorkTest16:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://mail.livedoor.com/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest17
    MessageBox MB_OK "メールサイト判定です。"
    Call CleanErrTest
  DoWorkTest17:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://www.kantei.go.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest18
    MessageBox MB_OK "政府関係サイト判定です。"
    Call CleanErrTest
  DoWorkTest18:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "テストケース実行..."
  ExecShell "" "iexplore.exe" "http://www.plustar.jp/lab/blog/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoWorkTest02
    MessageBox MB_OK "通常判定テストです。"
    Call CleanErrTest
  DoWorkTest02:
  DetailPrint "IE停止..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  Call CleanPassTest
SectionEnd

;--------------------------------
; Uninstall Section
;
Section "Uninstall"
SectionEnd

Function CleanErrTest
  ;; IEの停止を行う
  DetailPrint "IEの停止を行います..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  StrCpy $0 "問題を報告して下さい。$\r$\n"
  StrCpy $0 "$0テストにご協力頂きありがとうございます。"
  MessageBox MB_OK|MB_ICONQUESTION $0

  DetailPrint $0
  Sleep 1000
  Quit
FunctionEnd

Function CleanPassTest
  ;; IEの停止を行う
  DetailPrint "IEの停止を行います..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  StrCpy $0 "問題なさそうです。$\r$\n"
  StrCpy $0 "$0リリースを行なって下さい。"
  MessageBox MB_OK|MB_ICONQUESTION $0

  DetailPrint $0
FunctionEnd
