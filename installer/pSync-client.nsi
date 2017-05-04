; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pSync"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper pSync Client"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Client"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pSync-Setup-Client.exe"
InstallDir "$PROGRAMFILES\Plustar\pSync"

; --------------------------------
; Include Lib
;
!include LogicLib.nsh
!include WordFunc.nsh
!include Plustar_Include.nsh

; --------------------------------
; Pages
;
BrandingText "${PS_PRODUCTNAME} [$$Rev: 2592 $$]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP  "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                "icons\setup-trapper.ico"
!define MUI_UNICON              "icons\setup-trapper.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --------------------------------
; UPX���k
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pSyncScope.exe' ignore

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
  !insertmacro Plustar_DetailPrint "�t�@�C���W�J���c" 0

  ; none�F���b�Z�[�W�Ȃ�
  ; textonly�F�e�L�X�g�\��
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; �C���X�g�[���Ώۂ̃v���Z�X������΍폜����
  !insertmacro Plustar_KillProc "pSyncScope.exe" 300 100
  !insertmacro Plustar_KillProc "psync.exe"      300 100

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "$$Rev: 2592 $$"

  ;; �������c�������[�h�𔭐�������
  !insertmacro Plustar_Search_Keyword
  ;;

  File "..\Release\pSyncScope.exe"
  File "..\external\pSync\client\cygiconv-2.dll"
  File "..\external\pSync\client\cygz.dll"
  File "..\external\pSync\client\cygwin1.dll"
  File "..\external\pSync\client\psync.exe"

  !insertmacro Plustar_DetailPrint "�A���C���X�g�[���쐬���c" 1000
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "${PS_PRODUCTNAME}"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        1

  WriteUninstaller "uninstall.exe"

  ;; �ȉ���F/W�n��
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; ���N�����Ă��鎖��O��Ƃ��Ă���
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "psync.exe" "$INSTDIR\psync.exe" 0 2 "" 1

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

  !insertmacro un.Plustar_DetailPrint "�폜���c" 0

  ;; �C���X�g�[�����v���擾����
  !insertmacro un.Plustar_Stats_Install_Action "${PS_PRODUCT}" "$$Rev: 2592 $$"

  !insertmacro un.Plustar_KillProc "pSyncScope.exe" 300 100
  !insertmacro un.Plustar_KillProc "psync.exe"      300 100

  SimpleFC::RemoveApplication "$INSTDIR\psync.exe"

  DeleteRegKey HKLM "${PS_REGSOFTWARE}"
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"

  Delete "$DESKTOP\�N���b�v.lnk"

  !insertmacro un.Plustar_DetailPrint "�f�B���N�g���폜���c" 2000
  Delete "$INSTDIR\*.pss"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"
  RmDir /r "$INSTDIR"

  !insertmacro un.Plustar_DetailPrint "�A���C���X�g�[�������I" 2000

  !insertmacro un.Plustar_CountDown_Message 5 "�b��Ɏ����ŃC���X�g�[���͏I�����܂��B"

  SetAutoClose true
SectionEnd
