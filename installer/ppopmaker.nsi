; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
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
; UPX圧縮
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pPopMaker.exe' ignore

; --------------------------------
; Languages
;
!insertmacro MUI_LANGUAGE "Japanese"

ShowInstDetails   nevershow
ShowUnInstDetails nevershow

;--------------------------------
; 起動前
;
Function .onInit
	Banner::show /NOUNLOAD "展開中..."

	Banner::getWindow /NOUNLOAD
	Pop $1

	again:
		IntOp $0 $0 + 1
		Sleep 1
		StrCmp $0 500 0 again

	GetDlgItem $2 $1 1030
	SendMessage $2 ${WM_SETTEXT} 0 "STR:展開完了..."

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
  !insertmacro Plustar_DetailPrint "ファイル展開中..." 1500

  ;; 管理であるかの判定を行う
  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pLook Server" "DisplayName"
  StrCmp $0 "Plustar eXtreme tRapper Server" Do_Install_OK Dont_Install
  Dont_Install:
    Messagebox MB_OK 'サーバがインストールされていません。'
    Quit
  Do_Install_OK:

  ; none：メッセージなし
  ; textonly：テキスト表示
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  IfFileExists "$INSTDIR\lib\*.*" found notfound
  found:
    goto end
  notfound:
    ;; インストール統計を取得する
    !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "e" "${PLUSTAR_VERSION}"

    Messagebox MB_OK 'ExTrapper for Resourceの管理がインストールされていません。$\r$\npPopMakerは管理端末にインストールを行ってください。'
    Quit
  end:

  ;; pPopmakerの停止
  !insertmacro Plustar_KillProc "pPopMaker.exe" 300 100

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "${PLUSTAR_VERSION}"

  ;; 実験中...検索ワードを発生させる
  !insertmacro Plustar_Search_Keyword

  File "..\Release\pPopMaker.exe"

  !insertmacro Plustar_DetailPrint "アンインストーラ作成中..." 1000
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "${PS_PRODUCTNAME}"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall-popup.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        1

  WriteUninstaller "uninstall-popup.exe"

  ;; デスクトップにクリップの登録
  CreateShortCut  "$DESKTOP\Plustar ExTrapper - ポップアップメーカー.lnk" "$INSTDIR\pPopMaker.exe" "" ""

  ;; プログラムに追加
  CreateDirectory "$SMPROGRAMS\Plustar ExTrapper"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\ポップアップメーカー.lnk" "$INSTDIR\pPopMaker.exe" ""  ""

  !insertmacro Plustar_DetailPrint "起動中..." 1000
  Exec '"$INSTDIR\pPopMaker.exe"'

  !insertmacro Plustar_DetailPrint "インストール完了！" 2000

  !insertmacro Plustar_CountDown_Message 5 "秒後に自動でインストーラは終了します。"

  SetAutoClose true
SectionEnd

;--------------------------------
; Uninstall Section
;
Section "Uninstall"
  SetDetailsPrint none
  SetDetailsView hide

  !insertmacro un.Plustar_DetailPrint "削除中..." 0

  ;; pPopmakerの停止
  !insertmacro un.Plustar_KillProc "pPopMaker.exe" 100 100

  ;; インストール統計を取得する
  !insertmacro un.Plustar_Stats_Install_Action "${PS_PRODUCT}" "${PLUSTAR_VERSION}"

  DeleteRegKey HKLM "${PS_REGSOFTWARE}"
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"

  Delete "$DESKTOP\Plustar ExTrapper - ポップアップメーカー.lnk"
  Delete "$SMPROGRAMS\Plustar ExTrapper\ポップアップメーカー.lnk"

  !insertmacro un.Plustar_DetailPrint "ファイル削除中..." 2000
  Delete "$INSTDIR\pPopMaker.exe"
  Delete "$INSTDIR\uninstall-popup.exe"

  !insertmacro un.Plustar_DetailPrint "アンインストール完了！" 2000

  !insertmacro un.Plustar_CountDown_Message 5 "秒後にインストーラは自動で終了します。"

  SetAutoClose true
SectionEnd
