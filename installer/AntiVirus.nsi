; ------------------------------------------------------------------
; General
;

!Define PS_PRODUCT      "pAntiVirus"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper AntiVirus"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT}"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExTrapper-Setup-Client-AntiVirus.exe"
InstallDir "$PROGRAMFILES\Plustar\eXtreme tRapper"

!include Plustar_Include.nsh

; --------------------------------
; Pages
;

BrandingText "${PS_PRODUCTNAME}"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP  "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                "icons\setup.ico"
!define MUI_UNICON              "icons\setup.ico"

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
  File "..\pExTrapperAntiVirus.exe"

  ;; �A�C�R����windows�f�B���N�g���ɓ����
  ;;SetOutPath "$WINDIR"
  ;;File "pLook\res\lib\favicon_clip.ico"
  ;;SetOutPath "$INSTDIR"

  ;; IE�̂��C�ɓ���ɃN���b�v
  ;;CreateShortCut "$FAVORITES\�N���b�v(�l�C).lnk" "http://clip.plustar.jp/" ""
  ;;CreateShortCut "$FAVORITES\�N���b�v(����).lnk" "http://clip.plustar.jp/index.php?category=%25E5%258B%2595%25E7%2594%25BB" ""

  ;; ��ƃt�H���_�[��windows�ɂ���
  ;;SetOutPath "$WINDIR"
  ;; QUICKLAUNCH�ɃA�C�R����o�^���Ă݂�
  ;;CreateShortCut  "$QUICKLAUNCH\�N���b�v.lnk" "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  ;; ���j���[�ɃN���b�v�̓o�^
  ;;CreateShortCut  "$SMPROGRAMS\�N���b�v.lnk" "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  ;;SetOutPath "$INSTDIR"

  ;; Google�c�[���o�[��CLIP�A�C�R���ǉ�
  ;;SetOutPath "$LOCALAPPDATA\Google"
  ;;File /r /x ".svn" "..\external\Custom Buttons"
  ;;SetOutPath "$INSTDIR"

  ;; �V�X�e����HKLM��run�ɓo�^
  ;;[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run]
  WriteRegStr HKLM "${PS_RUNSTARTUP}" "pAntiVirus" '"$INSTDIR\pExTrapperAntiVirus.exe /S"'

  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "${PS_PRODUCTNAME}"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        0

  WriteUninstaller "UninstallAntiVirus.exe"

  CopyFiles $SYSDIR\drivers\etc\hosts $INSTDIR\hosts.com

  IfFileExists "$INSTDIR\pExTrapper.exe" found notfound
  found:
    goto end
  notfound:
    Quit
  end:

  ExecWait '$INSTDIR\pExTrapperAntiVirus.exe /S'

SectionEnd

;--------------------------------
; Uninstall Section
;

Section "Uninstall"
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pAntiVirus"

  CopyFiles $INSTDIR\hosts.com $SYSDIR\drivers\etc\hosts

  Delete "$INSTDIR\hosts.dat"
  Delete "$INSTDIR\hosts.com"
  Delete "$INSTDIR\pExTrapperAntiVirus.exe"
  Delete "$INSTDIR\UninstallAntiVirus.exe"
SectionEnd
