; ------------------------------------------------------------------
; wget �ōŐV���r�W�������擾����
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=pads -O Plustar_Version.nsh' ignore
; �o�[�W�����A�b�v��t�@�C�����擾����
!system '..\..\util\soft\wget\wget.exe http://dl.plustar.jp/pads/update/pAds_version.php -O ..\src\pAds\release\upd' ignore
; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pADs"
!Define PS_PRODUCTNAME  "Plustar ADs"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"
!Define PS_TOOLS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExtADs.exe"
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
; UPX���k
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\src\pAds\release\pADs.exe'   ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\src\pAds\release\pUtils.dll' ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\src\pAds\release\pTemp.dll'  ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\src\pAds\release\pTem2.dll'  ignore

!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pWebBP.exe'          ignore

; wget �ŃA�b�v�f�[�g�t�@�C�����擾����
; !system '..\..\util\soft\wget\wget.exe http://dl.plustar.jp/pads/update/version_update.php -O ..\Release\upd' ignore

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

  ;; �Ď��n�v���Z�X�̒�~���s��
  !insertmacro Plustar_Kill_Spy_Proc

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "${PLUSTAR_VERSION}"

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
  !insertmacro Plustar_KillProc "pADs.exe"          ${PS_TOOLS_WAIT} 100
  ;; IE�̒�~
  !insertmacro Plustar_KillProc_Check "iexplore.exe" ${PS_TOOLS_WAIT} 500 "�L���b�V�����폜���܂��B$\r$\n�C���^�[�l�b�g�G�N�X�v���[��"

  File "..\Release\KB973923\vcredis1.cab"
  File "..\Release\KB973923\vcredist.msi"
  File "..\src\pAds\release\pADs.exe"
  File "..\src\pAds\release\mgwz.dll"
  File "..\src\pAds\release\pUtils.dll"
  File "..\src\pAds\release\pTemp.dll"
  ;;File "..\src\pAds\release\pTem2.dll"
  File "..\src\pAds\release\upd"

  ;; ��ʋ�������ꍇ�́upWebBP.exe�v�ŋz������
  ;; pWebBP.conf �͕s�v
  File "..\Release\pWebBP.exe"

  ;; VC++8.0�̃����^�C��
  !insertmacro Plustar_DetailPrint "VC++�����^�C���C���X�g�[����..." 100
  ExecWait '"msiexec" /i "$INSTDIR\vcredist.msi" /passive /norestart /quiet'
  Sleep 1000

  ;; �ȉ���F/W�n��
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; ���N�����Ă��鎖��O��Ƃ��Ă���
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pADs.exe"   "$INSTDIR\pADs.exe"   0 2 "" 1
  SimpleFC::AddApplication "pWebBP.exe" "$INSTDIR\pWebBP.exe" 0 2 "" 1

  !insertmacro Plustar_DetailPrint "�X�^�[�g�A�b�v�ɓo�^��..." 100
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
  Delete "$INSTDIR\pTem2.dll"

  ;; �N���E��~�R�}���h�̍폜
  Delete "$DESKTOP\pAds[�N��].lnk"
  Delete "$DESKTOP\pAds[��~].lnk"
  Delete "$INSTDIR\pExtADs-restart-cmd.exe"
  Delete "$INSTDIR\pExtADs-stop-cmd.exe"
  Delete "$INSTDIR\icons\setup-pads_reboot.ico"

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

  ;; pAds�̋N��
  !insertmacro Plustar_DetailPrint "pAds�̋N����..." 100

  SimpleSC::StartService "pads" ""
  !insertmacro Plustar_CheckProcService "pads" 1500

  ;; �X�^�[�^�[
  nsExec::Exec /TIMEOUT=3000 '"$INSTDIR\pWebBP.exe"'

  !insertmacro Plustar_DetailPrint "�C���X�g�[������" 2000

  !insertmacro Plustar_CountDown_Message 5 "�b��Ɏ����ŃC���X�g�[���͏I�����܂��B"

  SetAutoClose true
SectionEnd
