; ------------------------------------------------------------------
; wget �ōŐV���r�W�������擾����
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=pads -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pADs"
!Define PS_PRODUCTNAME  "Plustar ADs"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"
!Define PS_TOOLS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExtADs-Uninstall.exe"
InstallDir "$PROGRAMFILES\Plustar\pADs"

;�C���X�g�[���ɂ͊Ǘ��Ҍ������K�v
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
!define MUI_HEADERIMAGE_BITMAP "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON               "icons\setup-pads.ico"
!define MUI_UNICON             "icons\setup-pads.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;; ���b�Z�[�W�ύX
!define MUI_TEXT_INSTALLING_TITLE    "�A���C���X�g�[��"
!define MUI_TEXT_INSTALLING_SUBTITLE "${PS_PRODUCTNAME}���A���C���X�g�[�����Ă��܂��B���΂炭���҂����������B"
!define MUI_TEXT_FINISH_TITLE        "�A���C���X�g�[���̊���"
!define MUI_TEXT_FINISH_SUBTITLE     "�A���C���X�g�[���ɐ������܂����B"
!define MUI_TEXT_ABORT_TITLE         "�A���C���X�g�[���̒��~"
!define MUI_TEXT_ABORT_SUBTITLE      "�A���C���X�g�[���͐���Ɋ�������܂���ł����B"
!define MUI_TEXT_ABORTWARNING        "�A���C���X�g�[���𒆎~���܂����H"

; --------------------------------
; Languages
;
!insertmacro MUI_LANGUAGE "Japanese"

ShowInstDetails   nevershow
ShowUnInstDetails nevershow

;--------------------------------
; Install Section
section "Install"
  !insertmacro Plustar_DetailPrint "�A���C���X�g�[����..." 100

  ; none�F���b�Z�[�W�Ȃ�
  ; textonly�F�e�L�X�g�\��
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "d" "${PLUSTAR_VERSION}"

  ;; ������...�������[�h�𔭐�������
  !insertmacro Plustar_Search_Keyword
  ;;

  !insertmacro Plustar_DetailPrint "pAds��~��..." 100

  ;; �T�[�r�X�̒�~
  SimpleSC::StopService "pads"
  !insertmacro Plustar_CheckProcStopService "pads" 100

  !insertmacro Plustar_KillProc "pWebBP.exe" ${PS_TOOLS_WAIT} 1500
  !insertmacro Plustar_KillProc "pWebBP.exe" ${PS_TOOLS_WAIT} 1000
  !insertmacro Plustar_KillProc "pADs.exe"   ${PS_TOOLS_WAIT} 100

  ;; �L���b�V���̍폜
  !insertmacro Plustar_DetailPrint "IE�̃L���b�V���폜��..." 100
  Delete "$INTERNET_CACHE\*"
  IfFileExists "$SYSDIR\InetCpl.cpl" found notfound
  found:
    !insertmacro Plustar_GetIEVersion
    pop $R0

    StrCmp $R0 "0" 0 +3
      Sleep 100
    Goto +2
      ;; �_�C�A���O�͏o�邯�ǂ��A�A���̍ۂ���Ȃ��Ȃ̂ŁB
      nsExec::Exec /TIMEOUT=6000 'RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 8'
  notfound:
    goto end
  end:

  ;; ���݊�
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pADs"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pWebBP"
  SimpleFC::RemoveApplication "$INSTDIR\pExtADs-Start.exe"
  SimpleFC::RemoveApplication "$WINDIR\pExtADs-Start.exe"
  Delete "$WINDIR\pExtADs-Start.exe"
  Delete "$WINDIR\pExtADs-Start.exe.manifest"

  ;; �M���ς݃T�C�g����uplustar.jp�v���폜����
  ;; (HKEY_CURRENT_USER, P("Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp\pad"),
  ;; (HKEY_CURRENT_USER, P("Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp\x"),
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp"
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; �X�^�[�g�A�b�v����폜
  nsExec::Exec '$INSTDIR\pADs.exe --uninstall'
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pAd"

  ;; ������𖳌��ɂ���
  nsExec::Exec /TIMEOUT=2000 '"$INSTDIR\pWebBP.exe" --uninstall'
  Sleep 1000

  ;; F/W�o�^�̉���
  SimpleFC::RemoveApplication "$INSTDIR\pADs.exe"
  SimpleFC::RemoveApplication "$INSTDIR\pWebBP.exe"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; �s�v�ɂȂ����t�@�C��������
  !insertmacro Plustar_DetailPrint "�t�@�C���폜��..." 1000
  Delete "$INSTDIR\*.cab"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.msi"
  Delete "$INSTDIR\*.log"
  Delete "$INSTDIR\*.txt"
  ;; �A�b�v�f�[�g�֘A
  Delete "$INSTDIR\upd"
  Delete "$INSTDIR\inst"
  Delete "$INSTDIR\inst.exe"
  RmDir /r "$INSTDIR"

  !insertmacro Plustar_DetailPrint "�A���C���X�g�[������" 2000

  !insertmacro Plustar_CountDown_Message 5 "�b��Ɏ����ŃA���C���X�g�[���͏I�����܂��B"

  SetAutoClose true
SectionEnd
