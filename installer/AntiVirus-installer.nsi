; ------------------------------------------------------------------
; General
;

!Define PS_PRODUCT     "pLook"
!Define PS_PRODUCTNAME "Plustar eXtreme tRapper Client"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExTrapperAntiVirus.exe"
InstallDir "$PROGRAMFILES\Plustar\eXtreme tRapper"

!include Plustar_Include.nsh

; --------------------------------
; Pages
;

BrandingText "Plustar eXtreme tRapper"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP  "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                "icons\Plustar.ico"
!define MUI_UNICON              "icons\Plustar.ico"

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

  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"
  File "..\external\hosts.dat"

  ;; アイコンはwindowsディレクトリに入れる
  ;;SetOutPath "$WINDIR"
  ;;File "pLook\res\lib\favicon_clip.ico"
  ;;SetOutPath "$INSTDIR"

  ;; IEのお気に入りにクリップ
  ;;CreateShortCut "$FAVORITES\クリップ(人気).lnk" "http://clip.plustar.jp/" ""
  ;;CreateShortCut "$FAVORITES\クリップ(動画).lnk" "http://clip.plustar.jp/index.php?category=%25E5%258B%2595%25E7%2594%25BB" ""

  ;; 作業フォルダーをwindowsにする
  ;;SetOutPath "$WINDIR"
  ;; QUICKLAUNCHにアイコンを登録してみる
  ;;CreateShortCut  "$QUICKLAUNCH\クリップ.lnk" "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  ;; メニューにクリップの登録
  ;;CreateShortCut  "$SMPROGRAMS\クリップ.lnk"  "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  ;;SetOutPath "$INSTDIR"

  ;; GoogleツールバーにCLIPアイコン追加
  ;;SetOutPath "$LOCALAPPDATA\Google"
  ;;File /r /x ".svn" "..\external\Custom Buttons"
  ;;SetOutPath "$INSTDIR"

  IfFileExists "$INSTDIR\pExTrapper.exe" found notfound
  found:
    goto end
  notfound:
    Quit
  end:

  SetOutPath "$SYSDIR\drivers\etc\"
  CopyFiles $INSTDIR\hosts.dat $SYSDIR\drivers\etc\hosts
  SetOutPath "$INSTDIR"

SectionEnd

;--------------------------------
; Uninstall Section
;

Section "Uninstall"
  Delete "$INSTDIR\host.dat"
SectionEnd
