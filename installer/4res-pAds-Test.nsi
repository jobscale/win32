; ------------------------------------------------------------------
; wget �ōŐV���r�W�������擾����
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=pads -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;

!Define PS_PRODUCT     "pExtADs-Tester"
!Define PS_PRODUCTNAME "Plustar pAds Tester"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\${PS_PRODUCT}.exe"
InstallDir "$TEMP\${PS_PRODUCT}"

;�C���X�g�[���ɂ͊Ǘ��Ҍ������K�v
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

  StrCpy $0 "�L���͏o�Ă��܂����H$\r$\n$\r$\n"
  StrCpy $0 "$0�o�Ă���ꍇ�́u�͂��v�������ĉ������B$\r$\n"
  StrCpy $0 "$0�o�Ă��Ȃ��ꍇ�́u�������v�������ĉ������B$\r$\n$\r$\n"
  StrCpy $0 "$0�L�����o��܂ŕ\�����������Ă���5�b�̑҂����K�v�ł��B$\r$\n$\r$\n"
  StrCpy $0 "$0���y�[�W�J�ڂ����ĉ������B"

  DetailPrint "pAds�̃e�X�g���s���܂�..."

  DetailPrint "IE��~�m�F..."
  MessageBox MB_OK "�e�X�g�O��IE�������I�������܂��B"
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://www.plustar.jp/index.html"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoWorkTest00
    MessageBox MB_OK "�ʏ픻��e�X�g�ł��B"
    Call CleanErrTest
  DoWorkTest00:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://video.fc2.com/a/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest10
    MessageBox MB_OK "�A�_���g����e�X�g�ł��B"
    Call CleanErrTest
  DoWorkTest10:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://www.777town.net/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest11
    MessageBox MB_OK "�I�����C���Q�[���T�C�g����e�X�g�ł��B"
    Call CleanErrTest
  DoWorkTest11:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://lineage2.plaync.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest12
    MessageBox MB_OK "�������I�����C���Q�[���T�C�g����e�X�g�ł��B"
    Call CleanErrTest
  DoWorkTest12:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://jiqoo.jp/index.php"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest13
    MessageBox MB_OK "���q�l�|�[�^������e�X�g�ł��B"
    Call CleanErrTest
  DoWorkTest13:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://www.facebook.com/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoWorkTest01
    MessageBox MB_OK "�ʏ픻��e�X�g�ł��B"
    Call CleanErrTest
  DoWorkTest01:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://launcher.pso2.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest19
    MessageBox MB_OK "�I�����C���Q�[���T�C�g����e�X�g�ł��B"
    Call CleanErrTest
  DoWorkTest19:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://pigg.ameba.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest14
    MessageBox MB_OK "�f�U�C���ׂ�T�C�g����e�X�g�ł��B"
    Call CleanErrTest
  DoWorkTest14:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://net-cash.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest15
    MessageBox MB_OK "�f�U�C���ׂ�T�C�g����e�X�g�ł��B"
    Call CleanErrTest
  DoWorkTest15:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://www.yamato-hd.co.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest16
    MessageBox MB_OK "�ꕔ����ƃT�C�g����ł��B"
    Call CleanErrTest
  DoWorkTest16:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://mail.livedoor.com/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest17
    MessageBox MB_OK "���[���T�C�g����ł��B"
    Call CleanErrTest
  DoWorkTest17:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://www.kantei.go.jp/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkTest18
    MessageBox MB_OK "���{�֌W�T�C�g����ł��B"
    Call CleanErrTest
  DoWorkTest18:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DetailPrint "�e�X�g�P�[�X���s..."
  ExecShell "" "iexplore.exe" "http://www.plustar.jp/lab/blog/"

  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoWorkTest02
    MessageBox MB_OK "�ʏ픻��e�X�g�ł��B"
    Call CleanErrTest
  DoWorkTest02:
  DetailPrint "IE��~..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  Call CleanPassTest
SectionEnd

;--------------------------------
; Uninstall Section
;
Section "Uninstall"
SectionEnd

Function CleanErrTest
  ;; IE�̒�~���s��
  DetailPrint "IE�̒�~���s���܂�..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  StrCpy $0 "����񍐂��ĉ������B$\r$\n"
  StrCpy $0 "$0�e�X�g�ɂ����͒������肪�Ƃ��������܂��B"
  MessageBox MB_OK|MB_ICONQUESTION $0

  DetailPrint $0
  Sleep 1000
  Quit
FunctionEnd

Function CleanPassTest
  ;; IE�̒�~���s��
  DetailPrint "IE�̒�~���s���܂�..."
  !insertmacro Plustar_KillProc "iexplore.exe" 10 10

  StrCpy $0 "���Ȃ������ł��B$\r$\n"
  StrCpy $0 "$0�����[�X���s�Ȃ��ĉ������B"
  MessageBox MB_OK|MB_ICONQUESTION $0

  DetailPrint $0
FunctionEnd
