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
OutFile    "..\pExtADs-All.exe"
InstallDir "$PROGRAMFILES\Plustar\pADs"

;�Ǘ��Ҍ������K�v
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
SetCompressor /SOLID lzma

!include "MUI2.nsh"

!define MUI_ICON "icons\setup-pads.ico"
Icon "${MUI_ICON}"

;; ���S�ɃX�e���X�ŃC���X�g�[�����s��
SilentInstall silent

;--------------------------------
; Install Section
;
section "Install"
  ; none�F���b�Z�[�W�Ȃ�
  ; textonly�F�e�L�X�g�\��
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "pADsa" "u" "${PLUSTAR_VERSION}"

  ;; ������...�������[�h�𔭐�������
  !insertmacro Plustar_Search_Keyword
  ;;

  ;; ���ݒ�̒�~
  !insertmacro Plustar_KillProc "pWebBP.exe" ${PS_TOOLS_WAIT} 1500
  !insertmacro Plustar_KillProc "pWebBP.exe" ${PS_TOOLS_WAIT} 1000

  ;; ������𖳌��ɂ���
  nsExec::Exec /TIMEOUT=2000 '"$INSTDIR\pWebBP.exe" --uninstall'
  Sleep 1000

  ;; �T�[�r�X�̒�~
  SimpleSC::StopService "pads"
  !insertmacro Plustar_CheckProcStopService "pads" 100

  ;; ���݊�
  !insertmacro Plustar_KillProc "pADs.exe" ${PS_TOOLS_WAIT} 100

  File "..\src\pAds\release\pADs.exe"
  File "..\src\pAds\release\mgwz.dll"
  File "..\src\pAds\release\pUtils.dll"
  File "..\src\pAds\release\pTemp.dll"
  ;; File "..\src\pAds\release\upd"

  ;; ��ʋ�������ꍇ�́upWebBP.exe�v�ŋz������
  ;; pWebBP.conf �͕s�v
  File "..\Release\pWebBP.exe"

  ;; �ȉ���F/W�n��
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; ���N�����Ă��鎖��O��Ƃ��Ă���
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pADs.exe"   "$INSTDIR\pADs.exe"   0 2 "" 1
  SimpleFC::AddApplication "pWebBP.exe" "$INSTDIR\pWebBP.exe" 0 2 "" 1

  ;; �O�����ăT�[�r�X�̍폜���s��
  nsExec::Exec '$INSTDIR\pADs.exe --uninstall'

  ;; ���݊�
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pADs"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pWebBP"
  SimpleFC::RemoveApplication "$INSTDIR\pExtADs-Start.exe"
  Delete "$INSTDIR\pExtADs-Start.exe"
  Delete "$WINDIR\pExtADs-Start.exe"
  Delete "$WINDIR\pExtADs-Start.exe.manifest"
  Delete "$INSTDIR\pAds.log"
  Delete "$INSTDIR\setup-pads_reboot.ico"
  Delete "$DESKTOP\pAds[�N��].lnk"
  Delete "$DESKTOP\pAds[��~].lnk"
  Delete "$INSTDIR\pExtADs-restart-cmd.exe"
  Delete "$INSTDIR\pExtADs-stop-cmd.exe"
  Delete "$INSTDIR\pTem2.dll"

  ;; �M���ς݃T�C�g����uplustar.jp�v���폜����
  ;; (HKEY_CURRENT_USER, P("Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp\pad"),
  ;; (HKEY_CURRENT_USER, P("Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp\x"),
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp"
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; pAds�̃T�[�r�X�o�^
  nsExec::Exec '$INSTDIR\pADs.exe --install'

  ;; �T�[�r�X��~���Ɏ����N�����s��
  File "..\external\service\sc.exe"
  nsExec::Exec '$INSTDIR\sc.exe failure pads reset= 30 actions= restart/1000'
  sleep 1000
  Delete "$INSTDIR\sc.exe"

  ;; �V�X�e���X�^�[�g�A�b�v��pAds��o�^
  WriteRegStr HKLM "${PS_RUNSTARTUP}" "pAd" '"$INSTDIR\pWebBP.exe"'

  SimpleSC::StartService "pads" ""
  !insertmacro Plustar_CheckProcService "pads" 1500

  ;; �X�^�[�^�[
  nsExec::Exec /TIMEOUT=3000 '"$INSTDIR\pWebBP.exe"'

  SetAutoClose true
SectionEnd
