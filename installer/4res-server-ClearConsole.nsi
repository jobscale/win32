; ------------------------------------------------------------------
; wget �ōŐV���r�W�������擾����
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=exts -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT     "pLook"
!Define PS_PRODUCTNAME "�v���X�^�[ ExTrapper for Resource �N���[���R���\�[��"
!Define PS_EXT_CTRL_CMD "$PROGRAMFILES\Plustar\eXtreme tRapper\pControler.exe"
!Define PS_START_PTOOL  "runptools"
!Define PS_STOP_TOOLS   "termptools"
!Define PS_CHK_TOOLS    "islifeptools"
!Define PS_SRV_ENABLE   "stopctrlenable"
!Define PS_SRV_DISABLE  "stopctrldisable"
!Define PS_PLOOK_WAIT   200
!Define PS_TOOLS_WAIT   200
!Define PS_CHWIN_WAIT   200
!Define PS_CRONS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExTrapper4Res-ClearConsole.exe"
InstallDir "$PROGRAMFILES\Plustar\eXtreme tRapper"

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
SetCompressor /SOLID lzma

!define MUI_ICON               "icons\setup-tools.ico"
Icon "${MUI_ICON}"

Caption    "${PS_PRODUCTNAME}"

Subcaption 3 " "
XPStyle on
AutoCloseWindow true
ChangeUI all "${NSISDIR}\Contrib\UIs\LoadingBar_Icon.exe"

ShowInstDetails   nevershow
ShowUnInstDetails nevershow

;--------------------------------
; Install Section
;
section "Install"
  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Client" "DisplayName"
  StrCmp $0 "" PathGood
    Messagebox MB_OK 'ExTrapper�̊Ǘ��T�[�o�ł͂���܂���B���̍�Ƃ͊Ǘ��T�[�o�ōs���Ă��������B'
    Quit
  PathGood:

  SetDetailsView    hide

  SetOutPath "$INSTDIR"

  StrCpy $0 "�v���X�^�[ ExTrapper for Resource�̃R���\�[���f�[�^�̏��������s���܂����H$\r$\n$\r$\n"
  StrCpy $0 "$0�N���C�A���g�̓��C�Z���X�̃N���C�A���g���Ɋ�Â��ĊǗ����Ă���܂��B$\r$\n"
  StrCpy $0 "$0�N���C�A���g�����ւ����ꍇ�ȂǂɎ��s���Ă��������B"
  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoWorkJob
    Quit
  DoWorkJob:

  !insertmacro Plustar_DetailPrint "��������..." 100

  ;; �_�~�[�\��
  !insertmacro Plustar_DetailPrint "�m�F��..." 100

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "rescc" "i" "${PLUSTAR_VERSION}"

  ;; ������...�������[�h�𔭐�������
  !insertmacro Plustar_Search_Keyword

  ;; �T�[�r�X�̒�~
  !insertmacro Plustar_DetailPrint "�폜��..." 100

  ;; �T�[�r�X������\�ɂ���
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_ENABLE}'
  Sleep 1000

  ;; pControler.exe�������Ă����ꍇ�ɂ͒�~���s��
  !insertmacro Plustar_KillProc "pControler.exe" ${PS_CRONS_WAIT} 100

  ;; �T�[�r�X�̒�~
  SimpleSC::StopService "MSLOOK"
  !insertmacro Plustar_CheckProcStopService "MSLOOK" 100

  ;; �R�ꂽ�ꍇ�͌ʂɒ�~
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_TOOLS}' '${PS_CHK_TOOLS}' 0
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_PLOOK}' '${PS_CHK_PLOOK}' 0

  Sleep 1500

  ;; �N���C�A���g�L�^�t�@�C���̍폜
  Delete "$INSTDIR\pConsole.dat"

  ;; �T�[�r�X�̊J�n
  !insertmacro Plustar_DetailPrint "�T�[�r�X�J�n��..." 100

  SimpleSC::StartService "MSLOOK" ""
  !insertmacro Plustar_CheckProcService "MSLOOK" 500

  SetDetailsPrint None
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_START_PTOOL}'
  Sleep 10000

  ;; pControler�̋N���m�F
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_START_PTOOL}' '${PS_CHK_TOOLS}' ${PS_TOOLS_WAIT}

  ;; �T�[�r�X�����s�\�ɂ���
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_DISABLE}'

  !insertmacro Plustar_DetailPrint "��������..." 1000
  !insertmacro Plustar_DetailPrint "�J�n..." 100

  ExecShell "open" "http://localhost:12082/"

  SetOutPath "$INSTDIR"

  SetDetailsPrint None

  SetAutoClose true
SectionEnd
