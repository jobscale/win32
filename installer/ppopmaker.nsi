; ------------------------------------------------------------------
; wget �ōŐV���r�W�������擾����
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=popm -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pPopMaker"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper pPopMaker"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT}"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExTrapper-Setup-${PS_PRODUCT}.exe"
InstallDir "$PROGRAMFILES\Plustar\eXtreme tRapper"

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
!define MUI_ICON                 "icons\setup-pPopMaker.ico"
!define MUI_UNICON               "icons\setup-pPopMaker.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --------------------------------
; UPX���k
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pPopMaker.exe' ignore

; --------------------------------
; Languages
;
!insertmacro MUI_LANGUAGE "Japanese"

ShowInstDetails   nevershow
ShowUnInstDetails nevershow

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
  !insertmacro Plustar_DetailPrint "�t�@�C���W�J��..." 1500

  ;; �Ǘ��ł��邩�̔�����s��
  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pLook Server" "DisplayName"
  StrCmp $0 "Plustar eXtreme tRapper Server" Do_Install_OK Dont_Install
  Dont_Install:
    Messagebox MB_OK '�T�[�o���C���X�g�[������Ă��܂���B'
    Quit
  Do_Install_OK:

  ; none�F���b�Z�[�W�Ȃ�
  ; textonly�F�e�L�X�g�\��
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  IfFileExists "$INSTDIR\lib\*.*" found notfound
  found:
    goto end
  notfound:
    ;; �C���X�g�[�����v���擾����
    !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "e" "${PLUSTAR_VERSION}"

    Messagebox MB_OK 'ExTrapper for Resource�̊Ǘ����C���X�g�[������Ă��܂���B$\r$\npPopMaker�͊Ǘ��[���ɃC���X�g�[�����s���Ă��������B'
    Quit
  end:

  ;; pPopmaker�̒�~
  !insertmacro Plustar_KillProc "pPopMaker.exe" 300 100

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "${PLUSTAR_VERSION}"

  ;; ������...�������[�h�𔭐�������
  !insertmacro Plustar_Search_Keyword

  File "..\Release\pPopMaker.exe"

  !insertmacro Plustar_DetailPrint "�A���C���X�g�[���쐬��..." 1000
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "${PS_PRODUCTNAME}"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall-popup.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        1

  WriteUninstaller "uninstall-popup.exe"

  ;; �f�X�N�g�b�v�ɃN���b�v�̓o�^
  CreateShortCut  "$DESKTOP\Plustar ExTrapper - �|�b�v�A�b�v���[�J�[.lnk" "$INSTDIR\pPopMaker.exe" "" ""

  ;; �v���O�����ɒǉ�
  CreateDirectory "$SMPROGRAMS\Plustar ExTrapper"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\�|�b�v�A�b�v���[�J�[.lnk" "$INSTDIR\pPopMaker.exe" ""  ""

  !insertmacro Plustar_DetailPrint "�N����..." 1000
  Exec '"$INSTDIR\pPopMaker.exe"'

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

  ;; pPopmaker�̒�~
  !insertmacro un.Plustar_KillProc "pPopMaker.exe" 100 100

  ;; �C���X�g�[�����v���擾����
  !insertmacro un.Plustar_Stats_Install_Action "${PS_PRODUCT}" "${PLUSTAR_VERSION}"

  DeleteRegKey HKLM "${PS_REGSOFTWARE}"
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"

  Delete "$DESKTOP\Plustar ExTrapper - �|�b�v�A�b�v���[�J�[.lnk"
  Delete "$SMPROGRAMS\Plustar ExTrapper\�|�b�v�A�b�v���[�J�[.lnk"

  !insertmacro un.Plustar_DetailPrint "�t�@�C���폜��..." 2000
  Delete "$INSTDIR\pPopMaker.exe"
  Delete "$INSTDIR\uninstall-popup.exe"

  !insertmacro un.Plustar_DetailPrint "�A���C���X�g�[�������I" 2000

  !insertmacro un.Plustar_CountDown_Message 5 "�b��ɃC���X�g�[���͎����ŏI�����܂��B"

  SetAutoClose true
SectionEnd
