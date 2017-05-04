; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pLook"
!Define PS_PRODUCTNAME  "プラスター ExTrapper 管理ツール"
!Define PS_EXT_CTRL_CMD "$PROGRAMFILES\Plustar\eXtreme tRapper\pControler.exe"
!Define PS_START_PTOOL  "runptools"
!Define PS_STOP_PLOOK   "termplook"
!Define PS_STOP_TOOLS   "termptools"
!Define PS_STOP_CTRL    "termpcontroler"
!Define PS_STOP_CHWIN   "termchwin"
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
OutFile    "..\pExT4Res-Stop-Server.exe"
InstallDir "$PROGRAMFILES\Plustar\eXtreme tRapper"

;インストールには管理者権限が必要
RequestExecutionLevel admin

; --------------------------------
; Include Lib
;
!include LogicLib.nsh
!include WordFunc.nsh
!include Plustar_Include.nsh

; --------------------------------
; Pages
;
SetCompressor /SOLID lzma

!define MUI_ICON               "icons\setup-tools.ico"
Icon "${MUI_ICON}"

Caption "${PS_PRODUCTNAME}"

Subcaption 3 " "
XPStyle on
AutoCloseWindow true
ChangeUI all "${NSISDIR}\Contrib\UIs\LoadingBar_Icon.exe"

; --------------------------------
; Languages
;
;;!insertmacro MUI_LANGUAGE "Japanese"

ShowInstDetails   nevershow
ShowUnInstDetails nevershow

;--------------------------------
; Install Section
;
section "Install"
  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Client" "DisplayName"
  StrCmp $0 "" PathGood
    Messagebox MB_OK 'ExTrapperの管理サーバではありません。この作業は管理サーバで行ってください。'
    Quit
  PathGood:

  SetDetailsView    hide

  SetOutPath "$INSTDIR"

  StrCpy $0 "pExTrapperを停止しますか？"
  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoStopExtrapper
  Quit

  DoStopExtrapper:

  DetailPrint 'ExTrapper 停止中...'

  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_STOP_CTRL}'
  Sleep 1000

  ;; pControler.exeが動いていた場合には停止を行う
  !insertmacro Plustar_KillProc "pControler.exe" ${PS_CRONS_WAIT} 100

  ;; サービス制御を可能にする
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_ENABLE}'
  Sleep 1000

  ;; サービスの停止
  SimpleSC::StopService "MSLOOK"
  !insertmacro Plustar_CheckProcStopService "MSLOOK" 100

  Sleep 1500

  ;; コマンドによるプロセスの停止を行う
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_CHWIN}' '${PS_CHK_CHWIN}' 0

  Sleep 1500

  ;; pExtrapperとpToolsを同時に停止する
  !insertmacro Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}
  !insertmacro Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}

  ;; 漏れた場合は個別に停止
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_TOOLS}' '${PS_CHK_TOOLS}' 0
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_PLOOK}' '${PS_CHK_PLOOK}' 0

  SetDetailsPrint None

  ;; ダミー表示
  DetailPrint 'プラスター ExTrapper for Resource確認完了...'

  SetAutoClose true
SectionEnd
