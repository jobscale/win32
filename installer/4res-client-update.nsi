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
!Define PS_START_PTOOL  "runptools"
!Define PS_STOP_PLOOK   "termplook"
!Define PS_STOP_TOOLS   "termptools"
!Define PS_STOP_CHWIN   "termchwin"
!Define PS_START_EXT    "start"
!Define PS_STOP_EXT     "finish"
!Define PS_CHK_PLOOK    "islifeplook"
!Define PS_CHK_TOOLS    "islifeptools"
!Define PS_CHK_CHWIN    "islifechwin"
!Define PS_SRV_ENABLE   "stopctrlenable"
!Define PS_SRV_DISABLE  "stopctrldisable"
!Define PS_PLOOK_WAIT   200
!Define PS_TOOLS_WAIT   200
!Define PS_CHWIN_WAIT   200
!Define PS_CRONS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExTrapper-Setup-Client-Update.exe"
InstallDir "$PROGRAMFILES\Plustar\eXtreme tRapper"

; --------------------------------
; Include Lib
;
!include WordFunc.nsh
!include Plustar_Include.nsh
!include Plustar_Version.nsh

; --------------------------------
; Pages
;
SetCompressor /SOLID lzma

!include "MUI2.nsh"

!define MUI_ICON icons\setup-extrapper.ico
Icon "${MUI_ICON}"

; --------------------------------
; UPX���k
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pExTrapper.exe'   ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pTools.exe'       ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pLook.dll'        ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pSpice.dll'       ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\imgctl.dll'       ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pCheckWindow.exe' ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pControler.exe'   ignore

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

  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Server" "DisplayName"
  StrCmp $0 "" PathGood
    Quit
  PathGood:

  SetOutPath "$INSTDIR"

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "resc" "u" "${PLUSTAR_VERSION}"

  ;; �������c�������[�h�𔭐�������
  !insertmacro Plustar_Search_Keyword

  SetOutPath "$INSTDIR"

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
  !insertmacro Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}

  ;; �R�ꂽ�ꍇ�͌ʂɒ�~
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_TOOLS}' '${PS_CHK_TOOLS}' 0
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_PLOOK}' '${PS_CHK_PLOOK}' 0

  ;; ���o�[�W�����̌݊�
  !insertmacro Plustar_KillProc "pTools.exe"     ${PS_TOOLS_WAIT} 100
  !insertmacro Plustar_KillProc "pExTrapper.exe" ${PS_PLOOK_WAIT} 100

  Sleep 2500
  ;; �� �����܂�

  File "..\Release\pExTrapper.exe"
  File "..\Release\pTools.exe"
  File "..\Release\pLook.dll"
  File "..\Release\pSpice.dll"
  File "..\Release\imgctl.dll"
  File "..\Release\pCheckWindow.exe"
  File "..\Release\pControler.exe"

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

  Delete "$INSTDIR\shirent.sys"

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

  ;; ���o�[�W�����Ō��݂͕s�v�t�@�C���̍폜���s��
  ;; eXtreme tRapper�̍폜(���݂�Exec�N��)
  SimpleSC::RemoveService "Plustar eXtreme tRapper"
  SimpleSC::RemoveService "Plustar eXtreme tRapper Server"
  SimpleSC::RemoveService "mstool"

  ;; �ȉ��̃f�B���N�g���̃t�@�C����S�ď���
  ;; C:\Documents and Settings\All Users\Application Data\pLook\pDebug
  Delete "$APPDATA\pLook\pDebug\*";
  Delete "$INSTDIR\*.commander"

  ;; ���̃��[�U�ɖ߂�
  SetShellVarContext current

  ;; �R���p�l���A���C���X�g�[�����폜����
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"
  Delete "$INSTDIR\uninstall.exe"

  ;; �V�łł�pControler.exe����̃L�b�N�ɂȂ�̂ŕs�v
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pExtrapper"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools.exe"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pCheckWindow"

  ;; �s�v�ȃp�����[�^�̍폜
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "listup_seconds"
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "ignore_socket_list"
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "logging_messagebox"
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "logging_url"
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "upload_counts"
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "stealth"
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; �ȉ���F/W�n��
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; ���N�����Ă��鎖��O��Ƃ��Ă���
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pExTrapper.exe"   "$INSTDIR\pExTrapper.exe"    0 2 "" 1
  SimpleFC::AddApplication "pLook.dll"        "$INSTDIR\pLook.dll"         0 2 "" 1
  SimpleFC::AddApplication "pTools.exe"       "$INSTDIR\pTools.exe"        0 2 "" 1
  SimpleFC::AddApplication "pCheckWindow.exe" "$INSTDIR\pCheckWindow.exe"  0 2 "" 1
  SimpleFC::AddApplication "pControler.exe"   "$INSTDIR\pControler.exe"    0 2 "" 1

  ;; �O�����ăT�[�r�X�̍폜���s��
  SimpleSC::RemoveService "MSLOOK"

  ;; pExtrapper�̃T�[�r�X�o�^
  ;; SimpleSC::InstallService [name_of_service][display_name][service_type][start_type][service_commandline][dependencies][account][password]
  SimpleSC::InstallService "MSLOOK" "Ms Look" "16" "2" "$INSTDIR\pExTrapper.exe" "" "" ""

  ;; �T�[�r�X��~���Ɏ����N�����s��
  File "..\external\service\sc.exe"
  nsExec::Exec '$INSTDIR\sc.exe failure MSLOOK reset= 30 actions= restart/1000'
  sleep 1000
  Delete "$INSTDIR\sc.exe"

  ;; pExtrapper�̋N��
  ;; SimpleSC::StartService "MyService" ""
  SimpleSC::StartService "MSLOOK" ""
  !insertmacro Plustar_CheckProcService "MSLOOK" 500

  ;; �V�X�e���X�^�[�g�A�b�v��pTools��o�^
  WriteRegStr HKLM "${PS_RUNSTARTUP}" "pControler" '"${PS_EXT_CTRL_CMD}" --${PS_START_PTOOL}'

  ;; pTools���N��
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_START_PTOOL}'
  Sleep 1000

  ;; pControler�̋N���m�F
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_START_PTOOL}' '${PS_CHK_TOOLS}' ${PS_TOOLS_WAIT}

  ;; �T�[�r�X�����s�\�ɂ���
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_DISABLE}'

  SetAutoClose true
SectionEnd
