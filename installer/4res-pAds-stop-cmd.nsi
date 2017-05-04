; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pADs"
!Define PS_PRODUCTNAME  "Plustar ADs"
!Define PS_TOOLS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExtADs-stop-cmd.exe"
InstallDir "$PROGRAMFILES\Plustar\pADs"

;�Ǘ��Ҍ������K�v
RequestExecutionLevel admin

; --------------------------------
; Include Lib
;
!include WordFunc.nsh
!include Plustar_Include.nsh

; --------------------------------
; Pages
;
SetCompressor /SOLID lzma
!define MUI_ICON icons\setup-pads_stop.ico
Icon "${MUI_ICON}"

Caption "${PS_PRODUCTNAME}"

Subcaption 3 " "
XPStyle on
AutoCloseWindow true
ChangeUI all "${NSISDIR}\Contrib\UIs\LoadingBar_Icon.exe"

;--------------------------------
; Install Section
;
section "Install"
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  !insertmacro Plustar_CountDown_Message 3 "�b���҂����������B��~������..."

  !insertmacro Plustar_DetailPrint "�T�[�o��~��..." 100

  ;; ���ݒ�̒�~
  !insertmacro Plustar_KillProc "pWebBP.exe" ${PS_TOOLS_WAIT} 1500
  !insertmacro Plustar_KillProc "pWebBP.exe" ${PS_TOOLS_WAIT} 1000

  ;; ������𖳌��ɂ���
  nsExec::Exec /TIMEOUT=2000 '"$INSTDIR\pWebBP.exe" --uninstall'
  Sleep 1000

  !insertmacro Plustar_DetailPrint "�ݒ������..." 100

  ;; �T�[�r�X�̒�~
  SimpleSC::StopService "pads"
  !insertmacro Plustar_CheckProcStopService "pads" 100

  !insertmacro Plustar_KillProc "pADs.exe" ${PS_TOOLS_WAIT} 100

  ;; ���O������΍폜����
  Delete "$INSTDIR\pAds.log"

  ;; �L���b�V���̍폜
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

  !insertmacro Plustar_DetailPrint "��~�����I" 1500

  SetDetailsPrint None
  SetAutoClose    true
SectionEnd
