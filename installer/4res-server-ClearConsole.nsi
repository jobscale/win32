; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=exts -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT     "pLook"
!Define PS_PRODUCTNAME "プラスター ExTrapper for Resource クリーンコンソール"
!Define PS_EXT_CTRL_CMD "$PROGRAMFILES\Plustar\eXtreme tRapper\pControler.exe"
!Define PS_START_PTOOL  "runptools"
!Define PS_STOP_TOOLS   "termptools"
!Define PS_CHK_TOOLS    "islifeptools"
!Define PS_SRV_ENABLE   "stopctrlenable"
!Define PS_SRV_DISABLE  "stopctrldisable"
!Define PS_PLOOK_WAIT   200
!Define PS_TOOLS_WAIT   200
!Define PS_CHWIN_WAIT   200
!Define PS_CRONS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExTrapper4Res-ClearConsole.exe"
InstallDir "$PROGRAMFILES\Plustar\eXtreme tRapper"

;インストールには管理者権限が必要
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
SetCompressor /SOLID lzma

!define MUI_ICON               "icons\setup-tools.ico"
Icon "${MUI_ICON}"

Caption    "${PS_PRODUCTNAME}"

Subcaption 3 " "
XPStyle on
AutoCloseWindow true
ChangeUI all "${NSISDIR}\Contrib\UIs\LoadingBar_Icon.exe"

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

  StrCpy $0 "プラスター ExTrapper for Resourceのコンソールデータの初期化を行いますか？$\r$\n$\r$\n"
  StrCpy $0 "$0クライアントはライセンスのクライアント数に基づいて管理しております。$\r$\n"
  StrCpy $0 "$0クライアントを入れ替えた場合などに実行してください。"
  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDYES DoWorkJob
    Quit
  DoWorkJob:

  !insertmacro Plustar_DetailPrint "初期化中..." 100

  ;; ダミー表示
  !insertmacro Plustar_DetailPrint "確認中..." 100

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "rescc" "i" "${PLUSTAR_VERSION}"

  ;; 実験中...検索ワードを発生させる
  !insertmacro Plustar_Search_Keyword

  ;; サービスの停止
  !insertmacro Plustar_DetailPrint "削除中..." 100

  ;; サービス制御を可能にする
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_ENABLE}'
  Sleep 1000

  ;; pControler.exeが動いていた場合には停止を行う
  !insertmacro Plustar_KillProc "pControler.exe" ${PS_CRONS_WAIT} 100

  ;; サービスの停止
  SimpleSC::StopService "MSLOOK"
  !insertmacro Plustar_CheckProcStopService "MSLOOK" 100

  ;; 漏れた場合は個別に停止
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_TOOLS}' '${PS_CHK_TOOLS}' 0
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_PLOOK}' '${PS_CHK_PLOOK}' 0

  Sleep 1500

  ;; クライアント記録ファイルの削除
  Delete "$INSTDIR\pConsole.dat"

  ;; サービスの開始
  !insertmacro Plustar_DetailPrint "サービス開始中..." 100

  SimpleSC::StartService "MSLOOK" ""
  !insertmacro Plustar_CheckProcService "MSLOOK" 500

  SetDetailsPrint None
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_START_PTOOL}'
  Sleep 10000

  ;; pControlerの起動確認
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_START_PTOOL}' '${PS_CHK_TOOLS}' ${PS_TOOLS_WAIT}

  ;; サービス制御を不可能にする
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_DISABLE}'

  !insertmacro Plustar_DetailPrint "初期化中..." 1000
  !insertmacro Plustar_DetailPrint "開始..." 100

  ExecShell "open" "http://localhost:12082/"

  SetOutPath "$INSTDIR"

  SetDetailsPrint None

  SetAutoClose true
SectionEnd
