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

  ;; �A�C�R����windows�f�B���N�g���ɓ����
  SetOutPath "$WINDIR"
  File "..\src\pLook\res\lib\favicon_clip.ico"
  SetOutPath "$INSTDIR"

  ;; IE�̂��C�ɓ���ɃN���b�v
  CreateShortCut "$FAVORITES\�N���b�v(�l�C).lnk"     "http://clip.plustar.jp/" ""
  CreateShortCut "$FAVORITES\�N���b�v(����).lnk"     "http://clip.plustar.jp/index.php?category=%25E5%258B%2595%25E7%2594%25BB" ""
  CreateShortCut "$FAVORITES\�N���b�v-�V��.lnk"      "http://feeds.plustar.jp/clip/new" ""
  CreateShortCut "$FAVORITES\�N���b�v-�l�C.lnk"      "http://feeds.plustar.jp/clip" ""
  CreateShortCut "$FAVORITES\�N���b�v�ɓo�^����.lnk" "javascript:q=(document.location.href);void(open('http://clip.plustar.jp/submit.php?url='+escape(q),'','resizable,location,menubar,toolbar,scrollbars,status'));" ""

  ;; ��ƃt�H���_�[��windows�ɂ���
  SetOutPath "$WINDIR"
  ;; QUICKLAUNCH�ɃA�C�R����o�^���Ă݂�
  CreateShortCut  "$QUICKLAUNCH\�N���b�v.lnk" "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  ;; ���j���[�ɃN���b�v�̓o�^
  CreateShortCut  "$SMPROGRAMS\�N���b�v.lnk"  "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  ;; �f�X�N�g�b�v�ɃN���b�v�̓o�^
  ;;CreateShortCut  "$DESKTOP\�N���b�v.lnk"   "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"

  ;; all users�ɕύX
  SetShellVarContext all
  CreateShortCut "$STARTMENU\�N���b�v(�l�C).lnk" "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  CreateShortCut "$STARTMENU\�N���b�v(����).lnk" "http://clip.plustar.jp/index.php?category=%25E5%258B%2595%25E7%2594%25BB" "" "$WINDIR\favicon_clip.ico"

  SetOutPath "$INSTDIR"

  ;; IE7�̌����ɃN���b�v��ǉ�
  ;;WriteRegDWORD HKCU "Software\Google\GoogleToolbarNotifier"             "ShowTrayIcon" 0
  ;;WriteRegDWORD HKCU "Software\Google\GoogleToolbarNotifier"             "KeepDS" 0
  ;;WriteRegStr   HKCU "Software\Microsoft\Internet Explorer\SearchScopes" "DefaultScope" '{DD0C8368-63FA-43A8-AB62-0D585897E500}'
  ;;WriteRegStr   HKCU "Software\Microsoft\Internet Explorer\SearchScopes\{DD0C8368-63FA-43A8-AB62-0D585897E500}" "DisplayName" "����"
  ;;WriteRegStr   HKCU "Software\Microsoft\Internet Explorer\SearchScopes\{DD0C8368-63FA-43A8-AB62-0D585897E500}" "URL" "http://clip.plustar.jp/search.php?type=bx&search={searchTerms}"
  ;;WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\SearchScopes\{DD0C8368-63FA-43A8-AB62-0D585897E500}" "Codepage" 0x0000fde9

  ;; �f�t�H������ύX
  ;;WriteRegStr HKCU "Software\Microsoft\Internet Explorer\SearchUrl" "" "http://clip.plustar.jp/search.php?type=bx&search=%s"
  ;;WriteRegStr HKCU "Software\Microsoft\Search Assistant"            "DefaultSearchURL" "http://clip.plustar.jp/search.php?type=bx&search=%s"

  ;; Google�c�[���o�[��CLIP�A�C�R���ǉ�
  ;;SetOutPath "$LOCALAPPDATA\Google"
  ;;File /r /x ".svn" "..\external\Custom Buttons"
  ;;SetOutPath "$INSTDIR"

  ;; �V�X�e����HKCU��run�ɓo�^
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
