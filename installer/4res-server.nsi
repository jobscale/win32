; ------------------------------------------------------------------
; wget �ōŐV���r�W�������擾����
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=exts -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pLook"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper for Resource Server"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Server"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"
!Define PS_EXT_CTRL_CMD "$PROGRAMFILES\Plustar\eXtreme tRapper\pControler.exe"
!Define PS_START_PTOOL  "runptools"
!Define PS_STOP_PLOOK   "termplook"
!Define PS_STOP_TOOLS   "termptools"
!Define PS_STOP_CHWIN   "termchwin"
!Define PS_STOP_CTRL    "termpcontroler"
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
OutFile    "..\pExTrapper4Res-Setup-Server.exe"
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
BrandingText "${PS_PRODUCTNAME} [Rev:${PLUSTAR_VERSION}]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP   "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                 "icons\setup-extrapper.ico"
!define MUI_UNICON               "icons\setup-extrapper.ico"

!define MUI_PAGE_CUSTOMFUNCTION_SHOW LicenseShow
!insertmacro MUI_PAGE_LICENSE    ".\Lic\LICENSE-4res"
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

Caption "${PS_PRODUCTNAME}"

; --------------------------------
; UPX���k
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pExTrapper.exe'   ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pTools.exe'       ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pLook.dll'        ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pSpice.dll'       ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\imgctl.dll'       ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pCheckWindow.exe' ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pControler.exe'   ignore

; --------------------------------
; Languages
;
!insertmacro MUI_LANGUAGE "Japanese"

LicenseForceSelection radiobuttons

ShowInstDetails   nevershow
ShowUnInstDetails nevershow

;; ���C�Z���X�m�F
Function LicenseShow
  ScrollLicense::Set /NOUNLOAD
FunctionEnd

Function .onGUIEnd
  ScrollLicense::Unload
FunctionEnd

;--------------------------------
; �N���O
;
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
  ;; �Ǘ������ɓ����Ă���ꍇ�͏㏑�����s��
  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Server" "DisplayName"
  StrCmp $0 "Plustar eXtreme tRapper Server" GoToInstall_0
    IfFileExists "$INSTDIR\pExTrapper.exe" Found_Ext NotFound_Ext
    ;; �N���C�A���g�ƃT�[�o�̔�����s��
    Found_Ext:

      IfFileExists "$PROGRAMFILES\plustar\eXtreme tRapper\lib\favicon_ps.ico" Do_Install_Ext Not_Install_Ext
      Do_Install_Ext:
        goto GoToInstall_0
      Not_Install_Ext:
        ;; �N���C�A���g���C���X�g�[������Ă���ꍇ�̓C���X�g�[�����~����
        Messagebox MB_OK '�N���C�A���g�o�[�W�������C���X�g�[������Ă��܂��B�N���C�A���g�ւ̓C���X�g�[���͂ł��܂���B'
        Quit

    ;; �V�K�̏ꍇ�͖������ɃC���X�g�[�����s��
    NotFound_Ext:
      goto GoToInstall_0
    Quit

  GoToInstall_0:

  ;; �Â��o�[�W�����������Ă���ꍇ
  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Client" "DisplayName"
  StrCmp $0 "" PathGood
    Messagebox MB_OK '���łɁA�N���C�A���g�o�[�W�������C���X�g�[������Ă��܂��B�N���C�A���g�o�[�W��������T�[�o�o�[�W�����ւ̏㏑���C���X�g�[���͂ł��܂���B�N���C�A���g�o�[�W�������A���C���X�g�[����ɍēx�s���Ă��������B'
    Quit
  PathGood:

  SetDetailsPrint none
  SetDetailsView  hide

  !insertmacro Plustar_DetailPrint "�t�@�C���W�J��..." 100

  SetOutPath "$INSTDIR"

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "ress" "i" "${PLUSTAR_VERSION}"

  ;; ������...�������[�h�𔭐�������
  !insertmacro Plustar_Search_Keyword

  ;; ���o�[�W�����̌݊�
  SimpleSC::StopService "CRON"
  !insertmacro Plustar_CheckProcStopService "CRON" 100

  !insertmacro Plustar_KillProc "ExT4Res-Start.exe" ${PS_CRONS_WAIT} 100
  !insertmacro Plustar_KillProc "ExT4Res-Check.exe" ${PS_CRONS_WAIT} 100

  SimpleSC::StopService "Plustar eXtreme tRapper"
  !insertmacro Plustar_CheckProcStopService "Plustar eXtreme tRapper" 100
  ;; �� �����܂�

  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_STOP_CTRL}'
  Sleep 1000

  ;; pControler.exe�������Ă����ꍇ�ɂ͒�~���s��
  !insertmacro Plustar_KillProc "pControler.exe" ${PS_CRONS_WAIT} 100

  ;; �T�[�r�X������\�ɂ���
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_ENABLE}'
  Sleep 1000

  ;; �T�[�r�X�̒�~
  SimpleSC::StopService "MSLOOK"
  !insertmacro Plustar_CheckProcStopService "MSLOOK" 100

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

  Delete "$INSTDIR\*.commander"

  File "..\Release\vcredis1.cab"
  File "..\Release\vcredist.msi"
  File "..\Release\pExTrapper.exe"
  File "..\Release\pTools.exe"
  File "..\Release\pLook.dll"
  File "..\Release\*.wav"
  File "..\Release\pSpice.dll"
  File "..\Release\imgctl.dll"
  File "..\Release\pCheckWindow.exe"
  File "..\Release\pControler.exe"
  File "..\reboot.exe"
  File /r /x ".svn" "..\src\pLook\res\lib"

  !insertmacro Plustar_DetailPrint "VC++�����^�C���C���X�g�[����..." 100
  ExecWait '"msiexec" /i "$INSTDIR\vcredist.msi" /passive /norestart /quiet'

  !insertmacro Plustar_DetailPrint "�V���[�g�J�b�g�쐬��..." 100
  ;; �f�X�N�g�b�v�V���[�g�J�b�g
  CreateShortCut "$DESKTOP\Plustar ExTrapper - �Ǘ��R���\�[��.lnk" "http://localhost:12082/" "" "$INSTDIR\lib\favicon_trapper.ico"
  CreateShortCut "$DESKTOP\Plustar ExTrapper - ���u�[�g.lnk" "$INSTDIR\reboot.exe" "" "$INSTDIR\lib\favicon_trapper_reboot.ico"

  CreateDirectory "$SMPROGRAMS\Plustar ExTrapper"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\�Ǘ��R���\�[��.lnk" "http://localhost:12082/" ""  "$INSTDIR\lib\favicon_trapper.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\�Ǘ��R���\�[��(�q����Ȃ��ꍇ).lnk" "http://127.0.0.1:12082/" ""  "$INSTDIR\lib\favicon_trapper.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\Analyzer - ���O�C��.lnk" "http://ext.plustar.jp/analyze/" ""  "$INSTDIR\lib\favicon_trapper.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\������Ѓv���X�^�[.lnk" "http://www.plustar.jp/" "" "$INSTDIR\lib\favicon_ps.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\���u�[�g.lnk" "$INSTDIR\reboot.exe" "" "$INSTDIR\lib\favicon_trapper_reboot.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\ExTrapper for Resource�}�j���A��.lnk" "http://docs.plustar.jp/trapper/" "ExTrapper" "$INSTDIR\lib\favicon_trapper.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\���N���C�A���g�A���C���X�g�[���[.lnk" "http://dl.plustar.jp/trapper/tools/pExTrapper-Uninstall-Client.exe" ""  "$INSTDIR\lib\favicon_unextrapper.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\���R���\�[���������c�[��.lnk" "http://dl.plustar.jp/trapper/tools/pExTrapper4Res-ClearConsole.exe" ""  "$INSTDIR\lib\favicon_tools.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\���}�V���\���������c�[��.lnk" "http://dl.plustar.jp/trapper/tools/pExTrapper4Res-ClearHard.exe" ""  "$INSTDIR\lib\favicon_tools.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\��IP�A�h���X�ύX.lnk" "http://ext.plustar.jp/store/" ""  "$INSTDIR\lib\favicon_tools.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\�����C�Z���X�擾.lnk" "http://ext.plustar.jp/store/" ""  "$INSTDIR\lib\favicon_tools.ico"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; ���o�[�W�����Ō��݂͕s�v�t�@�C���̍폜���s��
  ;; eXtreme tRapper�̍폜(���݂�Exec�N��)
  SimpleSC::RemoveService "Plustar eXtreme tRapper"

  ;; �V�łł�pExtrapper.exe��pTools.exe�����݂��ɊĎ�����̂ŕs�v
  ;; ������u�[�g��pExtrapper.exe�ōs��
  Delete "$WINDIR\ExT4Res-Start.exe"
  Delete "$WINDIR\ExT4Res-Check.exe"
  Delete "$SYSDIR\crontab"
  Delete "$SYSDIR\crons.exe"
  Delete "$INSTDIR\shirent.sys"
  SimpleSC::RemoveService "CRON"

  ;; �V�łł�pControler.exe����̃L�b�N�ɂȂ�̂ŕs�v
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pExtrapper"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools.exe"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pCheckWindow"
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  !insertmacro Plustar_DetailPrint "�A���C���X�g�[���쐬��..." 1000
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "Plustar eXtreme tRapper Server"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        0

  WriteUninstaller "uninstall.exe"

  ;; �ȉ���F/W�n��
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; ���N�����Ă��鎖��O��Ƃ��Ă���
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pExTrapper.exe"   "$INSTDIR\pExTrapper.exe"   0 2 "" 1
  SimpleFC::AddApplication "pLook.dll"        "$INSTDIR\pLook.dll"        0 2 "" 1
  SimpleFC::AddApplication "pTools.exe"       "$INSTDIR\pTools.exe"       0 2 "" 1
  SimpleFC::AddApplication "pCheckWindow.exe" "$INSTDIR\pCheckWindow.exe" 0 2 "" 1
  SimpleFC::AddApplication "pControler.exe"   "$INSTDIR\pControler.exe"   0 2 "" 1

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
  !insertmacro Plustar_DetailPrint "ExTrapper�N����..." 1000
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

  IfFileExists "$INSTDIR\pExtrapper.lic" found notfound
  found:
    goto end
  notfound:
    Messagebox MB_OK "�u$INSTDIR�v��$\r$\n���C�Z���X�t�@�C��(pExtrapper.lic)��o�^��$\r$\n�}�V���̍ċN�����s���Ă��������B"
  end:

  !insertmacro Plustar_DetailPrint "�C���X�g�[�������I" 2000

  !insertmacro Plustar_CountDown_Message 5 "�b��Ɏ����ŃC���X�g�[���͏I�����܂��B"

  SetAutoClose true
SectionEnd

;--------------------------------
; Uninstall Section
;
Section "Uninstall"
  SetDetailsPrint none
  SetDetailsView hide

  !insertmacro un.Plustar_DetailPrint "�폜��..." 100

  ;; �C���X�g�[�����v���擾����
  !insertmacro un.Plustar_Stats_Install_Action "ress" "${PLUSTAR_VERSION}"

  ;; ���o�[�W�����̌݊�
  ;; cron�̒�~
  SimpleSC::StopService "CRON"
  !insertmacro un.Plustar_CheckProcStopService "CRON" 100

  !insertmacro un.Plustar_KillProc "ExT4Res-Start.exe" ${PS_CRONS_WAIT} 100
  !insertmacro un.Plustar_KillProc "ExT4Res-Check.exe" ${PS_CRONS_WAIT} 100

  SimpleSC::StopService "Plustar eXtreme tRapper"
  !insertmacro un.Plustar_CheckProcStopService "Plustar eXtreme tRapper" 100
  ;; �� �����܂�

  ;; �T�[�r�X������\�ɂ���
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_ENABLE}'
  Sleep 1000

  SimpleSC::StopService "MSLOOK"
  !insertmacro un.Plustar_CheckProcStopService "MSLOOK" 100

  ;; pControler.exe�������Ă����ꍇ�ɂ͒�~���s��
  !insertmacro un.Plustar_KillProc "pControler.exe" ${PS_CRONS_WAIT} 100

  ;; pExtrapper��pTools�𓯎��ɒ�~����
  !insertmacro un.Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}
  !insertmacro un.Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}

  ;; �R�}���h�ɂ��v���Z�X�̒�~���s��
  !insertmacro un.Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_TOOLS}' '${PS_CHK_TOOLS}' ${PS_TOOLS_WAIT}
  !insertmacro un.Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_PLOOK}' '${PS_CHK_PLOOK}' ${PS_PLOOK_WAIT}

  ;; �R�ꂽ�ꍇ�͌ʂɒ�~
  !insertmacro un.Plustar_KillProc "pTools.exe"     ${PS_TOOLS_WAIT} 100
  !insertmacro un.Plustar_KillProc "pExTrapper.exe" ${PS_PLOOK_WAIT} 100

  ;; cron�̍폜
  SimpleSC::RemoveService "CRON"
  Delete "$WINDIR\ExT4Res-Start.exe"
  Delete "$WINDIR\ExT4Res-Check.exe"
  Delete "$SYSDIR\crontab"
  Delete "$SYSDIR\crons.exe"

  ;; �T�[�r�X�̍폜
  SimpleSC::RemoveService "Plustar eXtreme tRapper"
  SimpleSC::RemoveService "MSLOOK"

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

  !insertmacro un.Plustar_DetailPrint "�f�B���N�g���폜��..." 2000
  Delete "$INSTDIR\*.cab"
  Delete "$INSTDIR\*.dat"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.msi"
  Delete "$INSTDIR\*.commander"
  Delete "$INSTDIR\*.wav"
  Delete "$INSTDIR\*.lic"
  Delete "$INSTDIR\*.sys"
  Delete "$INSTDIR\*.log"
  Delete "$DESKTOP\pExTrapper.lic"
  Delete "$WINDIR\pExTrapper.lic"
  Delete "$SYSDIR\pExTrapper.lic"
  Delete "$INSTDIR\pExTrapper-Uninstall-Client.exe"
  RmDir /r "$INSTDIR\lib"
  RmDir /r "$INSTDIR"

  Delete "$DESKTOP\ExTrapper - ���u�[�g.lnk"
  Delete "$DESKTOP\Plustar ExTrapper - �Ǘ��R���\�[��.lnk"
  Delete "$DESKTOP\Plustar ExTrapper - �Ǘ��R���\�[��(�q����Ȃ��ꍇ).lnk"
  Delete "$DESKTOP\Plustar ExTrapper for Analyzer - ���O�C��.lnk"
  Delete "$DESKTOP\Plustar ExTrapper - �|�b�v�A�b�v���[�J�[.lnk"
  Delete "$DESKTOP\Plustar ExTrapper - ���u�[�g.lnk"
  Delete "$DESKTOP\Plustar ExTrapper - �N���C�A���g�A���C���X�g�[���[.lnk"

  RmDir /r "$SMPROGRAMS\Plustar ExTrapper"

  !insertmacro un.Plustar_DetailPrint "�A���C���X�g�[�������I" 2000

  !insertmacro un.Plustar_CountDown_Message 5 "�b��Ɏ����ŃC���X�g�[���͏I�����܂��B"

 SetAutoClose true
SectionEnd
