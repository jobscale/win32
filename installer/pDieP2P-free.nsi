; ------------------------------------------------------------------
; wget �ōŐV���r�W�������擾����
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=pdie -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pDieP2P-free"
!Define PS_PRODUCTNAME  "�v���X�^�[ �Ď�����"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT}"
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
BrandingText "${PS_PRODUCTNAME} [Rev:${PLUSTAR_VERSION}]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP   "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                 "icons\setup-plustar.ico"
!define MUI_UNICON               "icons\setup-unextrapper.ico"

!define MUI_PAGE_CUSTOMFUNCTION_SHOW LicenseShow
!insertmacro MUI_PAGE_LICENSE    "Lic\LICENSE-pDieP2P"
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --------------------------------
; UPX���k
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods pDieP2P-free\Release\pDieP2P-free.exe' ignore

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
; Install Section
;
Section "Install"
  ;;SetDetailsPrint none
  SetDetailsView    hide

  !insertmacro Plustar_DetailPrint "�t�@�C���W�J��..." 0

  SetOutPath "$INSTDIR"

  ;; �C���X�g�[�����v���擾����
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "${PLUSTAR_VERSION}"

  ; pDieP2P-free�̒�~
  !insertmacro Plustar_KillProc "pDieP2P-free.exe" 300 100

  File "..\Release\vcredis1.cab"
  File "..\Release\vcredist.msi"
  File "..\src\pDieP2P-free\Release\pDieP2P-free.exe"

  !insertmacro Plustar_DetailPrint "VC++�����^�C���C���X�g�[����..." 100
  ExecWait '"msiexec" /i "$INSTDIR\vcredist.msi" /passive /norestart /quiet'

  !insertmacro Plustar_DetailPrint "�A���C���X�g�[���쐬��..." 1000
  ;; �V�X�e����HKLM��run�ɓo�^
  ;;[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run]
  WriteRegStr HKLM "${PS_RUNSTARTUP}" "pDieP2P-free" '"$INSTDIR\pDieP2P-free.exe"'

  !insertmacro Plustar_DetailPrint "�A���C���X�g�[���쐬��..." 1000
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "${PS_PRODUCTNAME}"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        0

  WriteUninstaller "uninstall.exe"

  Exec '"$INSTDIR\pDieP2P-free.exe"'
  !insertmacro Plustar_DetailPrint "�N����..." 1000

  !insertmacro Plustar_DetailPrint "�C���X�g�[�������I" 2000

  ;;SetAutoClose true
SectionEnd

;--------------------------------
; Uninstall Section
;
Section "Uninstall"
  SetDetailsPrint none
  SetDetailsView hide

  !insertmacro un.Plustar_DetailPrint "�폜��..." 0

  !insertmacro un.Plustar_KillProc "pDieP2P-free.exe" 300 100

  ;; �C���X�g�[�����v���擾����
  !insertmacro un.Plustar_Stats_Install_Action "${PS_PRODUCT}" "${PLUSTAR_VERSION}"

  DeleteRegKey HKLM "${PS_REGSOFTWARE}"
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pDieP2P-free"

  !insertmacro un.Plustar_DetailPrint "�f�B���N�g���폜��..." 2000
  Delete "$INSTDIR\*.cab"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\pDieP2P-free.exe"
  RmDir /r "$INSTDIR"

  !insertmacro un.Plustar_DetailPrint "�C���X�g�[������" 2000

  ;;SetAutoClose true
SectionEnd
