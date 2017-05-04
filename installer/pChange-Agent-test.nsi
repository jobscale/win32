; ------------------------------------------------------------------
; General
;
!Define TB_PRODUCT     "ChangeAgent-test"
!Define TB_PRODUCTNAME "ChangeAgent-test"
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
    Sleep 1
    StrCmp $0 500 0 again

  GetDlgItem $2 $1 1030
  SendMessage $2 ${WM_SETTEXT} 0 "STR:�W�J����..."

  again2:
    IntOp $0 $0 + 1
    Sleep 1
    StrCmp $0 700 0 again2

  Banner::destroy
FunctionEnd

;--------------------------------
; Install Section
;
section "Install"
  StrCmp "$EXEFILE" "${TB_PRODUCT}-test.exe" DoExeWork DoExeNotWork
  DoExeNotWork:
    MessageBox MB_OK|MB_ICONSTOP "�v���O���������s���ł��B"
    Call CleanJob
    quit
  DoExeWork:

  SetOutPath "$INSTDIR"
  
  StrCpy $TB_TARGET_EXE_NAME "Filemon.exe"
  StrCpy $TB_TARGET_EXE_WAIT 300
  Call KillProc

  StrCpy $TB_TARGET_EXE_NAME "Regmon.exe"
  StrCpy $TB_TARGET_EXE_WAIT 300
  Call KillProc

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

  ;; �O�̂��߂Ɏ~�߂�
  DetailPrint "Stop Agent..."
  StrCpy $TB_TARGET_EXE_NAME "Agent.exe"
  StrCpy $TB_TARGET_EXE_WAIT 300
  Call KillProc

  DetailPrint "Del Agent..."
  Delete "$PROGRAMFILES\Provision\Agent\Agent.exe"

  ;; �t�@�C���̓W�J
  DetailPrint "Copy Agent..."
  File "..\external\recovery\XCoveryTB\Agent.exe"
  
  ;; ���W�X�g���̏�������
  WriteRegDWORD HKLM "${PROVISION}\Settings"        "ServiceMon" 0x00000000
  WriteRegDWORD HKLM "${PROVISION}\Settings"        "ListenPort" 0x32c7
  WriteRegStr   HKLM "${PROVISION}\Settings"        "PwdAdmin" "332197294:c0a5825d04"
  WriteRegDWORD HKLM "${PROVISION}\Settings\Option" "XCovery"  0x00000000
  WriteRegDWORD HKLM "${PROVISION}\Settings\Option" "Remote"   0x00000000
  WriteRegDWORD HKLM "${PROVISION}\Settings\Option" "Explorer" 0x00000000
  WriteRegDWORD HKLM "${PROVISION}\Settings\Option" "Registry" 0x00000000
  WriteRegDWORD HKLM "${PROVISION}\Settings\Option" "Monitor"  0x00000000
  WriteRegStr   HKLM "${PROVISION}\Settings\Option" "ID"       "3806443320:234676f4e03183"
  WriteRegStr   HKLM "${PROVISION}\Settings\Option" "PWD"      "165060678:d6e232b29b43046223893d2dd19c4469"

  ;; �O���Ƃ̒ʐM�𐧌�
  WriteRegDWORD HKLM "${PROVISION}\TCP/IP" "NextIndex"  0xffffffff
  WriteRegStr   HKLM "${PROVISION}\TCP/IP" "MainServer" ""
  WriteRegStr   HKLM "${PROVISION}\TCP/IP" "Server"     "*"

  ;; Agent.exe�̒ʐM����폜����
  ;;DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server1"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server2"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server3"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server4"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server5"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server6"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server7"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server8"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server9"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server10"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server11"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server12"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server13"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server14"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server15"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server16"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server17"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server18"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server19"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server20"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server21"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server22"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server23"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server24"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server25"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server26"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server27"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server28"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server29"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server30"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server31"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server32"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server33"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server34"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server35"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server36"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server37"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server38"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server39"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server40"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server41"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server42"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server43"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server44"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server45"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server46"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server47"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server48"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server49"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server50"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server51"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server52"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server53"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server54"
  DeleteRegValue HKLM "${PROVISION}\TCP/IP" "Server55"

  ;; ���O�̍폜���s��
  
  RMDir /r "$PROGRAMFILES\Provision\LogFiles\KPatch"
  RMDir /r "$PROGRAMFILES\Provision\LogFiles"

  ;; �T�[�r�X�̊J�n
  SimpleSC::StartService "Dms-RHost"
  SimpleSC::StartService "ProVision Agent Service"
  SimpleSC::StartService "DMS Agent Service"
  SimpleSC::StartService "SwDist"
  SimpleSC::StartService "ProVision-RHost"

  ;; �N��
  Exec '"$INSTDIR\Agent.exe"'

  Call CleanJob

  StrCpy $0 "�Ǘ��҂̃p�X���[�h���u0000�v�ŏ��������܂����B$\r$\n$\r$\n"
  ;;StrCpy $0 "$0�u�ύX�f�[�^�̕ۑ��v���\�ł��B$\r$\n$\r$\n"
  StrCpy $0 "$0���؂̖ړI�݂̂Ŏg�p���Ă��������c"
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
  Sleep 100
FunctionEnd
