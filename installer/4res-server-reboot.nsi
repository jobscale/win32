; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pLook"
!Define PS_PRODUCTNAME  "�v���X�^�[ ExTrapper for Resource �T�[�o"
!Define PS_EXT_CTRL_CMD "$PROGRAMFILES\Plustar\eXtreme tRapper\pControler.exe"
!Define PS_START_PTOOL  "runptools"
!Define PS_STOP_TOOLS   "termptools"
!Define PS_STOP_EXT     "finish"
!Define PS_CHK_TOOLS    "islifeptools"
!Define PS_SRV_ENABLE   "stopctrlenable"
!Define PS_SRV_DISABLE  "stopctrldisable"
!Define PS_PLOOK_WAIT   200
!Define PS_TOOLS_WAIT   200
!Define PS_CHWIN_WAIT   200
!Define PS_CRONS_WAIT   200

!Define /date BUILD_TIME "%Y-%m-%d"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\reboot.exe"
InstallDir "$WINDIR"

;�C���X�g�[���ɂ͊Ǘ��Ҍ������K�v
RequestExecutionLevel admin

; --------------------------------
; Include Lib
;
!include "Time.nsh"
!include Plustar_Include.nsh

; --------------------------------
; Pages
;
SetCompressor /SOLID lzma
!define MUI_ICON icons\setup-trapper_reboot.ico
Icon "${MUI_ICON}"

Caption "${PS_PRODUCTNAME}"

Subcaption 3 " "
XPStyle on
AutoCloseWindow true
ChangeUI all "${NSISDIR}\Contrib\UIs\LoadingBar_Icon.exe"

var t1 ;; ��
var t2 ;; ��
var t3 ;; �N
var t4 ;; ����
var t5 ;; ��
var t6 ;; �b

;--------------------------------
; Install Section
;
section "Install"
  SetDetailsView    hide

  ;; �Ǘ��̃`�F�b�N
  IfFileExists "$PROGRAMFILES\plustar\eXtreme tRapper\lib\favicon_ps.ico" found notfound
  found:
    goto do_job
  notfound:
    Quit
  do_job:

  StrCpy $0 "�v���X�^�[ ExTrapper for Resource�̍ċN�����s���܂����H"
  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoWorkReBoot
    Quit
  DoWorkReBoot:

  !insertmacro Plustar_CountDown_Message 5 "�b���҂����������B������..."

  !insertmacro Plustar_DetailPrint "��������..." 1000

  ;; pControler.exe�������Ă����ꍇ�ɂ͒�~���s��
  !insertmacro Plustar_KillProc "pControler.exe" ${PS_CRONS_WAIT} 100

  ;; �T�[�r�X������\�ɂ���
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_ENABLE}'
  Sleep 1000

  !insertmacro Plustar_DetailPrint "�T�[�o��~��..." 100
  ;; �T�[�r�X�̒�~
  SimpleSC::StopService "MSLOOK"
  !insertmacro Plustar_CheckProcStopService "MSLOOK" 100

  ;; pExtrapper��pTools�𓯎��ɒ�~����
  !insertmacro Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}

  Sleep 1500

  !insertmacro Plustar_DetailPrint "��~�m�F��..." 100
  ;; �R�ꂽ�ꍇ�͌ʂɒ�~
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_TOOLS}' '${PS_CHK_TOOLS}' 0
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_PLOOK}' '${PS_CHK_PLOOK}' 0

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; �܂������Ă���悤�ł���΋�����~���s��
  ;; �������A�Ǘ��}�V����Ana�f�[�^�͕ۑ�����Ȃ�
  !insertmacro Plustar_KillProc "pExtrapper.exe" ${PS_PLOOK_WAIT} 100
  !insertmacro Plustar_KillProc "pTools.exe"     ${PS_TOOLS_WAIT} 100
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ${time::GetLocalTime} $0
  ${time::TimeString} "$0" $t1 $t2 $t3 $t4 $t5 $t6

  IntFmt $t1 "%0.2d" $t1
  IntFmt $t2 "%0.2d" $t2
  IntFmt $t4 "%0.2d" $t4
  IntFmt $t5 "%0.2d" $t5
  IntFmt $t6 "%0.2d" $t6

  ;; all users�ɕύX
  SetShellVarContext all

  LogEx::Init  "$APPDATA\pLook\pDebug\logdata_$t3$t2$t1.log"
  LogEx::Write "[SERVER REBOOT][$t4:$t5:$t6]------------------------------"
  LogEx::Close

  !insertmacro Plustar_DetailPrint "�ċN����..." 100
  ;; pExtrapper�̋N��
  SimpleSC::StartService "MSLOOK" ""
  !insertmacro Plustar_CheckProcService "MSLOOK" 500

  !insertmacro Plustar_DetailPrint "�N���m�F��..." 100
  SetDetailsPrint none
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_START_PTOOL}'
  Sleep 1000

  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_START_PTOOL}' '${PS_CHK_TOOLS}' ${PS_TOOLS_WAIT}

  ;; �T�[�r�X�����s�\�ɂ���
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_DISABLE}'

  StrCpy $0 "�v���X�^�[ ExTrapper for Resource�̍ċN�����s���܂����B"
  MessageBox MB_OK|MB_ICONQUESTION $0

  SetDetailsPrint None
  SetAutoClose    true
SectionEnd
