; ------------------------------------------------------------------
; wget �ōŐV���r�W�������擾����
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=extc -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pLook"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper for Resource Client"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Client"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"
!Define PS_EXT_CTRL_CMD "$PROGRAMFILES\Plustar\eXtreme tRapper\pControler.exe"
!Define PS_STOP_PLOOK   "termplook"
!Define PS_STOP_TOOLS   "termptools"
!Define PS_STOP_CHWIN   "termchwin"
!Define PS_START_EXT    "start"
!Define PS_STOP_EXT     "finish"
!Define PS_CHK_PLOOK    "islifeplook"
!Define PS_CHK_TOOLS    "islifeptools"
!Define PS_CHK_CHWIN    "islifechwin"
!Define PS_SRV_ENABLE   "stopctrlenable"
!Define PS_PLOOK_WAIT   200
!Define PS_TOOLS_WAIT   200
!Define PS_CHWIN_WAIT   200
!Define PS_CRONS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExTrapper-Uninstall-Client.exe"
InstallDir "$PROGRAMFILES\Plustar\eXtreme tRapper"

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
BrandingText "${PS_PRODUCTNAME} [${PLUSTAR_VERSION}]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON               "icons\setup-unextrapper.ico"
!define MUI_UNICON             "icons\setup-unextrapper.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;; ���b�Z�[�W�ύX
!define MUI_TEXT_INSTALLING_TITLE    "�A���C���X�g�[��"
!define MUI_TEXT_INSTALLING_SUBTITLE "ExTrapper for Client���A���C���X�g�[�����Ă��܂��B���΂炭���҂����������B"
!define MUI_TEXT_FINISH_TITLE        "�A���C���X�g�[���̊���"
!define MUI_TEXT_FINISH_SUBTITLE     "�A���C���X�g�[���ɐ������܂����B"
!define MUI_TEXT_ABORT_TITLE         "�A���C���X�g�[���̒��~"
!define MUI_TEXT_ABORT_SUBTITLE      "�A���C���X�g�[���͐���Ɋ�������܂���ł����B"
!define MUI_TEXT_ABORTWARNING        "�A���C���X�g�[���𒆎~���܂����H"

; --------------------------------
; Languages
;
!insertmacro MUI_LANGUAGE "Japanese"

ShowInstDetails nevershow

;--------------------------------
; Install Section
;
section "Install"
  SetDetailsPrint none
  SetDetailsView  hide

  !insertmacro Plustar_DetailPrint "��������..." 100

  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Server" "DisplayName"
  StrCmp $0 "" PathGood
    Messagebox MB_OK '�T�[�o�o�[�W�������C���X�g�[������Ă��܂��B'
    Quit
  PathGood:

  !insertmacro Plustar_DetailPrint "�폜��..." 100

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "resc" "d" "${PLUSTAR_VERSION}"

  ;; ������...�������[�h�𔭐�������
  !insertmacro Plustar_Search_Keyword

  ;; ���o�[�W�����̌݊�
  SimpleSC::StopService "Plustar eXtreme tRapper"
  !insertmacro Plustar_CheckProcStopService "Plustar eXtreme tRapper" 100
  ;; �� �����܂�

  ;; �T�[�r�X������\�ɂ���
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_ENABLE}'
  Sleep 1000

  ;; �T�[�r�X�̒�~
  SimpleSC::StopService "MSLOOK"
  !insertmacro Plustar_CheckProcStopService "MSLOOK" 100

  ;; pControler.exe�������Ă����ꍇ�ɂ͒�~���s��
  !insertmacro Plustar_KillProc "pControler.exe" ${PS_CRONS_WAIT} 100

  ;; �R�}���h�ɂ��v���Z�X�̒�~���s��
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_CHWIN}' '${PS_CHK_CHWIN}' 0

  ;; pExtrapper��pTools�𓯎��ɒ�~����
  !insertmacro Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}

  Sleep 1500

  ;; �R�ꂽ�ꍇ�͌ʂɒ�~
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_TOOLS}' '${PS_CHK_TOOLS}' 0
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_PLOOK}' '${PS_CHK_PLOOK}' 0

  ;; ���o�[�W�����̌݊�
  !insertmacro Plustar_KillProc "pTools.exe"     ${PS_TOOLS_WAIT} 100
  !insertmacro Plustar_KillProc "pExTrapper.exe" ${PS_PLOOK_WAIT} 100
  ;; �� �����܂�

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; �s�v�ɂȂ����t�@�C��������
  Delete "$FAVORITES\�N���b�v.lnk"
  Delete "$FAVORITES\�N���b�v(�l�C).lnk"
  Delete "$FAVORITES\�N���b�v-�V��.lnk"
  Delete "$FAVORITES\�N���b�v-�l�C.lnk"
  Delete "$FAVORITES\�N���b�v�ɓo�^����.lnk"
  Delete "$FAVORITES\�N���b�v(����).lnk"
  Delete "$FAVORITES\�N���b�v(��l�T�C�g).lnk"
  Delete "$QUICKLAUNCH\�N���b�v.lnk"
  Delete "$SMSTARTUP\start.lnk"
  Delete "$WINDIR\sChecker.exe"
  Delete "$LOCALAPPDATA\Google\clip.plustar.jp.xml"
  Delete "$SMPROGRAMS\�N���b�v.lnk"
  Delete "$DESKTOP\�N���b�v.lnk"
  Delete "$STARTMENU\�N���b�v.lnk"
  Delete "$SMPROGRAMS\�N���b�v.lnk"

  ;; all users�ɕύX
  SetShellVarContext all
  Delete "$STARTMENU\�N���b�v(�l�C).lnk"
  Delete "$STARTMENU\�N���b�v(����).lnk"
  Delete "$FAVORITES\�N���b�v-�l�C.lnk"
  Delete "$FAVORITES\�N���b�v-�V��.lnk"
  Delete "$FAVORITES\�N���b�v�ɓo�^����.lnk"
  Delete "$FAVORITES\�N���b�v(����).lnk"
  Delete "$FAVORITES\�N���b�v(�l�C).lnk"
  Delete "$FAVORITES\�N���b�v.lnk"
  Delete "$STARTMENU\�N���b�v.lnk"
  Delete "$SMPROGRAMS\�N���b�v.lnk"

  ;; ���̃��[�U�ɖ߂�
  SetShellVarContext current
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; eXtreme tRapper�̍폜
  SimpleSC::RemoveService "Plustar eXtreme tRapper"
  SimpleSC::RemoveService "MSLOOK"
  SimpleSC::RemoveService "MSTOOL"
  SimpleSC::RemoveService "mslook"
  SimpleSC::RemoveService "mstool"

  SimpleFC::RemoveApplication "$INSTDIR\pExTrapper.exe"
  SimpleFC::RemoveApplication "$INSTDIR\pLook.dll"
  SimpleFC::RemoveApplication "$INSTDIR\pTools.exe"
  SimpleFC::RemoveApplication "$INSTDIR\pCheckWindow.exe"
  SimpleFC::RemoveApplication "$INSTDIR\pControler.exe"

  DeleteRegKey HKLM "${PS_REGSOFTWARE}"
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"

  ;; �X�^�[�g�A�b�v�o�^����
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pExtrapper"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools.exe"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pCheckWindow"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pControler"

  !insertmacro Plustar_DetailPrint "�f�B���N�g���폜��..." 2000
  Delete "$INSTDIR\*.cab"
  Delete "$INSTDIR\*.dat"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.msi"
  Delete "$INSTDIR\*.log"
  Delete "$INSTDIR\*.commander"
  RmDir /r "$INSTDIR"

  !insertmacro Plustar_DetailPrint "�A���C���X�g�[�������I" 2000

  !insertmacro Plustar_CountDown_Message 15 "�b���OS�̍ċN�����s�Ȃ��܂��B"

  ;; �A���C���X�R��͋������u�[�g���s��
  Reboot

  SetAutoClose true
SectionEnd
