; ------------------------------------------------------------------
; wget �ōŐV���r�W�������擾����
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

;�Ǘ��Ҍ������K�v
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
  !insertmacro Plustar_DetailPrint "�t�@�C���W�J��..." 0

  ; none�F���b�Z�[�W�Ȃ�
  ; textonly�F�e�L�X�g�\��
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "pADst" "i" "${PLUSTAR_VERSION}"

  ;; �N���E��~�R�}���h
  File "..\pExtADs-restart-cmd.exe"
  File "..\pExtADs-stop-cmd.exe"

  CreateShortCut "$DESKTOP\pAds[�N��].lnk" "$INSTDIR\pExtADs-restart-cmd.exe" "" ""
  CreateShortCut "$DESKTOP\pAds[��~].lnk" "$INSTDIR\pExtADs-stop-cmd.exe"    "" ""

  !insertmacro Plustar_DetailPrint "�C���X�g�[������" 2000
  !insertmacro Plustar_CountDown_Message 5 "�b��Ɏ����ŃC���X�g�[���͏I�����܂��B"

  SetAutoClose true
SectionEnd
