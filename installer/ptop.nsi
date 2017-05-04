; ------------------------------------------------------------------
; wget �ōŐV���r�W�������擾����
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=top -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pTop"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper pTop"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT}"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExTrapper-Setup-${PS_PRODUCT}.exe"
InstallDir "$PROGRAMFILES\Plustar\${PS_PRODUCT}"

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
!define MUI_ICON                 "icons\setup-pTop.ico"
!define MUI_UNICON               "icons\setup-pTop.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;; ���b�Z�[�W�ύX
!define MUI_TEXT_INSTALLING_SUBTITLE "${PS_PRODUCTNAME}���C���X�g�[�����Ă��܂��B"

; --------------------------------
; UPX���k
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pTop.exe' ignore

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

  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; �Ď��n�v���Z�X�̒�~���s��
  !insertmacro Plustar_Kill_Spy_Proc

  !insertmacro Plustar_KillProc "pTop.exe" 100 1000

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "${PLUSTAR_VERSION}"

  ;; ������...�������[�h�𔭐�������
  !insertmacro Plustar_Search_Keyword

  File "..\Release\pTop.exe"

  ;; �X�^�[�g�A�b�v�ɓo�^
  !insertmacro Plustar_DetailPrint "�X�^�[�g�A�b�v�ɓo�^��..." 1000
  WriteRegStr HKLM "${PS_RUNSTARTUP}" "pTop" '"$INSTDIR\pTop.exe"'

  !insertmacro Plustar_DetailPrint "�A���C���X�g�[���쐬��..." 2000
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "${PS_PRODUCTNAME}"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify" 1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair" 1

  WriteUninstaller "uninstall.exe"

  Exec '"$INSTDIR\pTop.exe"'
  !insertmacro Plustar_DetailPrint "�N����..." 1000

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

  ;; �Ď��n�v���Z�X�̒�~���s��
  !insertmacro un.Plustar_Kill_Spy_Proc

  !insertmacro un.Plustar_KillProc "pTop.exe" 100 100

  ;; �C���X�g�[�����v���擾����
  !insertmacro un.Plustar_Stats_Install_Action "${PS_PRODUCT}" "${PLUSTAR_VERSION}"

  ;; ������...�������[�h�𔭐�������
  !insertmacro un.Plustar_Search_Keyword

  DeleteRegKey HKLM "${PS_REGSOFTWARE}"
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"

  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTop"

  !insertmacro un.Plustar_DetailPrint "�f�B���N�g���폜��..." 2000
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.*"
  RmDir /r "$INSTDIR"

  !insertmacro un.Plustar_DetailPrint "�A���C���X�g�[�������I" 2000

  !insertmacro un.Plustar_CountDown_Message 5 "�b��Ɏ����ŃC���X�g�[���͏I�����܂��B"

  SetAutoClose true
SectionEnd
