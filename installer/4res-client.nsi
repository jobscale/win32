; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=extc -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pLook"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper for Resource Client"
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
OutFile    "..\pExTrapper-Setup-Client.exe"
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
!define MUI_HEADERIMAGE_BITMAP "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON               "icons\setup-extrapper.ico"
!define MUI_UNICON             "icons\setup-extrapper.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --------------------------------
; UPX圧縮
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

ShowInstDetails   nevershow
ShowUnInstDetails nevershow

;--------------------------------
; Install Section
;
section "Install"
  !insertmacro Plustar_DetailPrint "初期化中..." 100

  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Server" "DisplayName"
  StrCmp $0 "" PathGood
    Messagebox MB_OK 'すでに、サーババージョンがインストールされています。サーババージョンからクライアントバージョンへの上書きインストールはできません。サーババージョンをアンインストール後に再度行ってください。'
    Quit
  PathGood:

  !insertmacro Plustar_DetailPrint "ファイル展開中..." 0

  ; none：メッセージなし
  ; textonly：テキスト表示
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; 監視系プロセスの停止を行う
  !insertmacro Plustar_Kill_Spy_Proc

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "resc" "i" "${PLUSTAR_VERSION}"

  ;; 実験中...検索ワードを発生させる
  !insertmacro Plustar_Search_Keyword

  ;; 旧バージョンの互換
  SimpleSC::StopService "Plustar eXtreme tRapper"
  !insertmacro Plustar_CheckProcStopService "Plustar eXtreme tRapper" 100
  ;; ↑ ここまで

  ;; サービス制御を可能にする
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_ENABLE}'
  Sleep 1000

  ;; サービスの停止
  SimpleSC::StopService "MSLOOK"
  !insertmacro Plustar_CheckProcStopService "MSLOOK" 100

  ;; pControler.exeが動いていた場合には停止を行う
  !insertmacro Plustar_KillProc "pControler.exe"   ${PS_CRONS_WAIT} 100

  ;; コマンドによるプロセスの停止を行う
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_CHWIN}' '${PS_CHK_CHWIN}' 0

  ;; pExtrapperとpToolsを同時に停止する
  !insertmacro Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}
  !insertmacro Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}

  ;; 漏れた場合は個別に停止
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_TOOLS}' '${PS_CHK_TOOLS}' 0
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_PLOOK}' '${PS_CHK_PLOOK}' 0

  ;; 旧バージョンの互換
  !insertmacro Plustar_KillProc "pTools.exe"     ${PS_TOOLS_WAIT} 100
  !insertmacro Plustar_KillProc "pExTrapper.exe" ${PS_PLOOK_WAIT} 100

  Sleep 2500
  ;; ↑ ここまで

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; 不要になったファイルを消す ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; eXtreme tRapperの削除(現在はExec起動)
  SimpleSC::RemoveService "Plustar eXtreme tRapper"

  Delete "$INSTDIR\*.commander"
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  File "..\Release\vcredis1.cab"
  File "..\Release\vcredist.msi"
  File "..\Release\pExTrapper.exe"
  File "..\Release\pTools.exe"
  File "..\Release\pLook.dll"
  File "..\Release\pSpice.dll"
  File "..\Release\imgctl.dll"
  File "..\Release\pCheckWindow.exe"
  File "..\Release\pControler.exe"

  !insertmacro Plustar_DetailPrint "VC++ランタイムインストール中..." 100
  ExecWait '"msiexec" /i "$INSTDIR\vcredist.msi" /passive /norestart /quiet'

  ;; 以下のF/W系は
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; が起動している事を前提としている
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pExTrapper.exe"   "$INSTDIR\pExTrapper.exe"    0 2 "" 1
  SimpleFC::AddApplication "pLook.dll"        "$INSTDIR\pLook.dll"         0 2 "" 1
  SimpleFC::AddApplication "pTools.exe"       "$INSTDIR\pTools.exe"        0 2 "" 1
  SimpleFC::AddApplication "pCheckWindow.exe" "$INSTDIR\pCheckWindow.exe"  0 2 "" 1
  SimpleFC::AddApplication "pControler.exe"   "$INSTDIR\pControler.exe"    0 2 "" 1

  !insertmacro Plustar_DetailPrint "スタートアップに登録中..." 100

  ;; 前もってサービスの削除を行う
  SimpleSC::RemoveService "MSLOOK"

  ;; pExtrapperのサービス登録
  ;; SimpleSC::InstallService [name_of_service][display_name][service_type][start_type][service_commandline][dependencies][account][password]
  SimpleSC::InstallService "MSLOOK" "Ms Look" "16" "2" "$INSTDIR\pExTrapper.exe" "" "" ""

  ;; サービス停止時に自動起動を行う
  File "..\external\service\sc.exe"
  nsExec::Exec '$INSTDIR\sc.exe failure MSLOOK reset= 30 actions= restart/1000'
  sleep 1000
  Delete "$INSTDIR\sc.exe"

  !insertmacro Plustar_DetailPrint "ExTrapperの起動中..." 100
  ;; pExtrapperの起動
  ;; SimpleSC::StartService "MyService" ""
  SimpleSC::StartService "MSLOOK" ""
  !insertmacro Plustar_CheckProcService "MSLOOK" 1500

  ;; システムスタートアップにpToolsを登録
  WriteRegStr HKLM "${PS_RUNSTARTUP}" "pControler" '"${PS_EXT_CTRL_CMD}" --${PS_START_PTOOL}'

  ;; pToolsを起動
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_START_PTOOL}'
  Sleep 1000

  ;; pControlerの起動確認
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_START_PTOOL}' '${PS_CHK_TOOLS}' ${PS_TOOLS_WAIT}

  ;; サービス制御を不可能にする
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_DISABLE}'

  !insertmacro Plustar_DetailPrint "インストール完了" 2000

  !insertmacro Plustar_CountDown_Message 5 "秒後に自動でインストーラは終了します。"

  SetAutoClose true
SectionEnd
