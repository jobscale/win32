; ------------------------------------------------------------------
; General
;
!Define TB_PRODUCT     "ChangeAgent"
!Define TB_PRODUCTNAME "ChangeAgent"
!Define PROVISION   "software\ProVision\Agent"

Name       "${TB_PRODUCTNAME}"
OutFile    "..\${TB_PRODUCT}.exe"
InstallDir "$PROGRAMFILES\Provision\Agent"

; --------------------------------
; Pages
;
BrandingText ""

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_ICON   "icons\setup-agent.ico"
!define MUI_UNICON "icons\setup-agent.ico"

!insertmacro MUI_PAGE_INSTFILES

var TB_TARGET_EXE_NAME
var TB_TARGET_EXE_WAIT

;; ���S�ɃX�e���X�ŃC���X�g�[�����s��
SilentInstall silent

Function .onInit
  Banner::show /NOUNLOAD "�W�J��..."

  Banner::getWindow /NOUNLOAD
  Pop $1

  again:
    IntOp $0 $0 + 1
    Sleep 10
    StrCmp $0 500 0 again

  GetDlgItem $2 $1 1030
  SendMessage $2 ${WM_SETTEXT} 0 "STR:�W�J����..."

  again2:
    IntOp $0 $0 + 1
    Sleep 10
    StrCmp $0 700 0 again2

  Banner::destroy
FunctionEnd

;--------------------------------
; Install Section
;
section "Install"
  IfFileExists "$PROGRAMFILES\Provision\Agent\Agent.exe" found
    MessageBox MB_OK "�J�t�F�r�W�������C���X�g�[������Ă��܂���c"
    Quit

  found:
  
  StrCmp "$EXEFILE" "${TB_PRODUCT}.exe" DoExeWork DoExeNotWork
  DoExeNotWork:
    MessageBox MB_OK|MB_ICONSTOP "�v���O���������s���ł��B"
    Call CleanJob
    quit
  DoExeWork:

  StrCmp $DESKTOP $EXEDIR DoNotWork DoWork
  DoNotWork:
    MessageBox MB_OK|MB_ICONSTOP "�f�X�N�g�b�v�ł͎��s���Ȃ��ŉ������I"
    Call CleanJob
    quit
  DoWork:

  SetOutPath "$INSTDIR"
  
  StrCpy $TB_TARGET_EXE_NAME "Filemon.exe"
  StrCpy $TB_TARGET_EXE_WAIT 300
  Call KillProc

  StrCpy $TB_TARGET_EXE_NAME "Regmon.exe"
  StrCpy $TB_TARGET_EXE_WAIT 300
  Call KillProc

  Sleep 2000

  ;; ���W�X�g���ɃA�N�Z�X��������Ă�ꍇ������̂�
  ;; �}���{�[�́u���̎�v�Ń��W�X�g���ւ̃A�N�Z�X������(�O�̂��߂ɑ��̎�̓p�X���[�h��
  ;; [HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Policies\System]
  ;; "DisableRegistryTools"=dword:00000000
  DetailPrint "Change DisableRegistry..."
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Policies\System"  "DisableRegistryTools" 0x00000000

  ;; CafeVision�̉������s��
  ;; [HKEY_LOCAL_MACHINE\software\ProVision\Agent\Settings\Option]
  ;; "XCovery"=dword:00000001
  ;; "Remote"=dword:00000001
  ;; "Explorer"=dword:00000001
  ;; "Registry"=dword:00000001
  ;; "Monitor"=dword:00000000
  ;; agent�̒�~���s��
  DetailPrint "Stop Cafevision Agent..."
  SimpleSC::StopService "Dms-RHost"
  SimpleSC::StopService "ProVision Agent Service"
  SimpleSC::StopService "DMS Agent Service"
  SimpleSC::StopService "SwDist"
  SimpleSC::StopService "ProVision-RHost"
  Sleep 2000

  ;; �O�̂��߂Ɏ~�߂�
  DetailPrint "Stop Agent..."
  StrCpy $TB_TARGET_EXE_NAME "Agent.exe"
  StrCpy $TB_TARGET_EXE_WAIT 300
  Call KillProc

  Sleep 1000

  DetailPrint "Del Agent..."
  Delete "$PROGRAMFILES\Provision\Agent\Agent.exe"

  Sleep 2000

  ;; �t�@�C���̓W�J
  DetailPrint "Copy Agent..."
  File "..\external\recovery\XCoveryTB\Agent.exe"
  
  ;; ���W�X�g���̏�������
  WriteRegStr   HKLM "${PROVISION}\Settings"        "PwdAdmin" "332197294:c0a5825d04"

  ;; �T�[�r�X�̊J�n
  SimpleSC::StartService "Dms-RHost"
  SimpleSC::StartService "ProVision Agent Service"
  SimpleSC::StartService "DMS Agent Service"
  SimpleSC::StartService "SwDist"
  SimpleSC::StartService "ProVision-RHost"
  Sleep 2000

  ;; �N��
  Exec '"$INSTDIR\Agent.exe"'

  Call CleanJob

  StrCpy $0 "�Ǘ��҂̃p�X���[�h���u0000�v�ŏ��������܂����B$\r$\n$\r$\n"
  StrCpy $0 "$0���؂̖ړI�݂̂Ŏg�p���Ă�������"
  MessageBox MB_OK|MB_ICONINFORMATION $0

  Quit
SectionEnd

Function KillProc
  Push $0
  IntFmt $R4 "%u" 0

  Processes::FindProcess $TB_TARGET_EXE_NAME
  StrCmp $R0 "1" loop done

  loop:
    KillProcDLL::KillProc $TB_TARGET_EXE_NAME
    StrCmp $R0 "0" done
    StrCmp $R0 "603" done
    Sleep $TB_TARGET_EXE_WAIT

    ;; �������[�v���
    IntOp $R4 $R4 + 1
    IntCmp $R4 10 done

    Processes::FindProcess $TB_TARGET_EXE_NAME
    StrCmp $R0 "1" loop done
  done:
    Sleep 100

 Pop $0
FunctionEnd

Function CleanJob
  Sleep 1000
FunctionEnd
