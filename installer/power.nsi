; ------------------------------------------------------------------
; wget �ōŐV���r�W�������擾����
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=pow -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pPower"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper pPower"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Server"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\${PS_PRODUCT}-Setup.exe"
InstallDir "$PROGRAMFILES\Plustar\${PS_PRODUCT}"

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
BrandingText "${PS_PRODUCTNAME} [${PLUSTAR_VERSION}]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP   "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                 "icons\setup-trapper.ico"
!define MUI_UNICON               "icons\setup-trapper.ico"
!define MUI_LICENSEPAGE_CHECKBOX

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --------------------------------
; UPX���k
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pPower.exe' ignore

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
  SetDetailsPrint none
  SetDetailsView  hide

  !insertmacro Plustar_DetailPrint "�t�@�C���W�J��..." 0

  ;; pPower�̒�~
  !insertmacro Plustar_KillProc "pPower.exe" 300 100

  SetOutPath "$INSTDIR"

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "${PLUSTAR_VERSION}"

  ;; ������...�������[�h�𔭐�������
  !insertmacro Plustar_Search_Keyword

  File "..\Release\vcredis1.cab"
  File "..\Release\vcredist.msi"
  File "..\Release\pPower.exe"

  !insertmacro Plustar_DetailPrint "VC++�����^�C���C���X�g�[����..." 100
  ExecWait '"msiexec" /i "$INSTDIR\vcredist.msi" /passive /norestart /quiet'

  ;; �X�^�[�g�A�b�v�ɓo�^
  SetShellVarContext current
  CreateShortCut  "$SMSTARTUP\pPower.lnk" "$INSTDIR\pPower.exe" ""

  !insertmacro Plustar_DetailPrint "�A���C���X�g�[���쐬��..." 1000
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "${PS_PRODUCTNAME}"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        0

  WriteUninstaller "uninstall.exe"

  ;; �ȉ���F/W�n��
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; ���N�����Ă��鎖��O��Ƃ��Ă���
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pPower.exe" "$INSTDIR\pPower.exe" 0 2 "" 1

  !insertmacro Plustar_DetailPrint "�N����..." 1000
  Exec '"$INSTDIR\pPower.exe"'

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

  !insertmacro un.Plustar_DetailPrint "�폜��..." 0

  ;; pPower�̒�~
  !insertmacro un.Plustar_KillProc "pPower.exe" 100 100

  ;; �C���X�g�[�����v���擾����
  !insertmacro un.Plustar_Stats_Install_Action "${PS_PRODUCT}" "${PLUSTAR_VERSION}"

  SimpleFC::RemoveApplication "$INSTDIR\pPower.exe"

  DeleteRegKey HKLM "${PS_REGSOFTWARE}"
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"

  !insertmacro un.Plustar_DetailPrint "�f�B���N�g���폜��..." 2000
  Delete "$INSTDIR\*.cab"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.msi"
  Delete "$SMSTARTUP\pPower.lnk"
  RmDir /r "$INSTDIR"

  !insertmacro un.Plustar_DetailPrint "�A���C���X�g�[�������I" 2000

  !insertmacro un.Plustar_CountDown_Message 5 "�b��Ɏ����ŃC���X�g�[���͏I�����܂��B"

  SetAutoClose true
SectionEnd
