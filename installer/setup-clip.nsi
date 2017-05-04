; ------------------------------------------------------------------
; General
;

!Define PS_PRODUCT     "MSNST"
!Define PS_PRODUCTNAME "Network Setup tools"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\sChecker.exe"
InstallDir "$WINDIR"

; --------------------------------
; Pages
;
BrandingText "Network Setup tools"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON   "icons\setup.ico"
!define MUI_UNICON "icons\setup.ico"

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

  ;; アイコンはwindowsディレクトリに入れる
  SetOutPath "$WINDIR"
  File "..\src\pLook\res\lib\favicon_clip.ico"
  SetOutPath "$INSTDIR"

  ;; IEのお気に入りにクリップ
  CreateShortCut "$FAVORITES\クリップ(人気).lnk"     "http://clip.plustar.jp/" ""
  CreateShortCut "$FAVORITES\クリップ(動画).lnk"     "http://clip.plustar.jp/index.php?category=%25E5%258B%2595%25E7%2594%25BB" ""
  CreateShortCut "$FAVORITES\クリップ-新着.lnk"      "http://feeds.plustar.jp/clip/new" ""
  CreateShortCut "$FAVORITES\クリップ-人気.lnk"      "http://feeds.plustar.jp/clip" ""
  CreateShortCut "$FAVORITES\クリップに登録する.lnk" "javascript:q=(document.location.href);void(open('http://clip.plustar.jp/submit.php?url='+escape(q),'','resizable,location,menubar,toolbar,scrollbars,status'));" ""

  ;; 作業フォルダーをwindowsにする
  SetOutPath "$WINDIR"
  ;; QUICKLAUNCHにアイコンを登録してみる
  CreateShortCut  "$QUICKLAUNCH\クリップ.lnk" "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  ;; メニューにクリップの登録
  CreateShortCut  "$SMPROGRAMS\クリップ.lnk"  "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  ;; デスクトップにクリップの登録
  ;;CreateShortCut  "$DESKTOP\クリップ.lnk"   "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"

  ;; all usersに変更
  SetShellVarContext all
  CreateShortCut "$STARTMENU\クリップ(人気).lnk" "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  CreateShortCut "$STARTMENU\クリップ(動画).lnk" "http://clip.plustar.jp/index.php?category=%25E5%258B%2595%25E7%2594%25BB" "" "$WINDIR\favicon_clip.ico"

  SetOutPath "$INSTDIR"

  ;; IE7の検索にクリップを追加
  ;;WriteRegDWORD HKCU "Software\Google\GoogleToolbarNotifier"             "ShowTrayIcon" 0
  ;;WriteRegDWORD HKCU "Software\Google\GoogleToolbarNotifier"             "KeepDS" 0
  ;;WriteRegStr   HKCU "Software\Microsoft\Internet Explorer\SearchScopes" "DefaultScope" '{DD0C8368-63FA-43A8-AB62-0D585897E500}'
  ;;WriteRegStr   HKCU "Software\Microsoft\Internet Explorer\SearchScopes\{DD0C8368-63FA-43A8-AB62-0D585897E500}" "DisplayName" "検索"
  ;;WriteRegStr   HKCU "Software\Microsoft\Internet Explorer\SearchScopes\{DD0C8368-63FA-43A8-AB62-0D585897E500}" "URL" "http://clip.plustar.jp/search.php?type=bx&search={searchTerms}"
  ;;WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\SearchScopes\{DD0C8368-63FA-43A8-AB62-0D585897E500}" "Codepage" 0x0000fde9

  ;; デフォ検索を変更
  ;;WriteRegStr HKCU "Software\Microsoft\Internet Explorer\SearchUrl" "" "http://clip.plustar.jp/search.php?type=bx&search=%s"
  ;;WriteRegStr HKCU "Software\Microsoft\Search Assistant"            "DefaultSearchURL" "http://clip.plustar.jp/search.php?type=bx&search=%s"

  ;; GoogleツールバーにCLIPアイコン追加
  ;;SetOutPath "$LOCALAPPDATA\Google"
  ;;File /r /x ".svn" "..\external\Custom Buttons"
  ;;SetOutPath "$INSTDIR"

  ;; システムのHKCUのrunに登録
  SetShellVarContext current
  SetOutPath "$WINDIR"
  CreateShortCut  "$SMSTARTUP\start.lnk" "$WINDIR\sChecker.exe" "/S"
  SetOutPath "$INSTDIR"

  SetAutoClose true
SectionEnd

;--------------------------------
; Uninstall Section
;

Section "Uninstall"
SectionEnd
