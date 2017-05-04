; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=extc -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pLook"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper for Resource Client"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Client"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"
!Define PS_EXT_CTRL_CMD "$PROGRAMFILES\Plustar\eXtreme tRapper\pControler.exe"
!Define PS_START_PTOOL  "runptools"
!Define PS_STOP_PLOOK   "termplook"
!Define PS_STOP_TOOLS   "termptools"
!Define PS_STOP_CHWIN   "termchwin"
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
OutFile    "..\pExTrapper-Setup-Client-Update.exe"
InstallDir "$PROGRAMFILES\Plustar\eXtreme tRapper"

; --------------------------------
; Include Lib
;
!include WordFunc.nsh
!include Plustar_Include.nsh
!include Plustar_Version.nsh

; --------------------------------
; Pages
;
SetCompressor /SOLID lzma

!include "MUI2.nsh"

!define MUI_ICON icons\setup-extrapper.ico
Icon "${MUI_ICON}"

; --------------------------------
; UPX圧縮
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pExTrapper.exe'   ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pTools.exe'       ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pLook.dll'        ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pSpice.dll'       ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\imgctl.dll'       ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pCheckWindow.exe' ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pControler.exe'   ignore

;; 完全にステルスでインストールを行う
SilentInstall silent

;--------------------------------
; Install Section
;
section "Install"
  ; none：メッセージなし
  ; textonly：テキスト表示
  SetDetailsPrint none
  SetDetailsView  hide

  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Server" "DisplayName"
  StrCmp $0 "" PathGood
    Quit
  PathGood:

  SetOutPath "$INSTDIR"

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "resc" "u" "${PLUSTAR_VERSION}"

  ;; 実験中…検索ワードを発生させる
  !insertmacro Plustar_Search_Keyword

  SetOutPath "$INSTDIR"

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
  !insertmacro Plustar_KillProc "pControler.exe" ${PS_CRONS_WAIT} 100

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

  File "..\Release\pExTrapper.exe"
  File "..\Release\pTools.exe"
  File "..\Release\pLook.dll"
  File "..\Release\pSpice.dll"
  File "..\Release\imgctl.dll"
  File "..\Release\pCheckWindow.exe"
  File "..\Release\pControler.exe"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; 不要になったファイルを消す
  Delete "$FAVORITES\クリップ.lnk"
  Delete "$FAVORITES\クリップ(人気).lnk"
  Delete "$FAVORITES\クリップ-新着.lnk"
  Delete "$FAVORITES\クリップ-人気.lnk"
  Delete "$FAVORITES\クリップに登録する.lnk"
  Delete "$FAVORITES\クリップ(動画).lnk"
  Delete "$FAVORITES\クリップ(大人サイト).lnk"
  Delete "$QUICKLAUNCH\クリップ.lnk"
  Delete "$SMSTARTUP\start.lnk"
  Delete "$WINDIR\sChecker.exe"
  Delete "$LOCALAPPDATA\Google\clip.plustar.jp.xml"
  Delete "$SMPROGRAMS\クリップ.lnk"
  Delete "$DESKTOP\クリップ.lnk"
  Delete "$STARTMENU\クリップ.lnk"

  Delete "$INSTDIR\shirent.sys"

  ;; all usersに変更
  SetShellVarContext all
  Delete "$STARTMENU\クリップ(人気).lnk"
  Delete "$STARTMENU\クリップ(動画).lnk"
  Delete "$FAVORITES\クリップ-人気.lnk"
  Delete "$FAVORITES\クリップ-新着.lnk"
  Delete "$FAVORITES\クリップに登録する.lnk"
  Delete "$FAVORITES\クリップ(動画).lnk"
  Delete "$FAVORITES\クリップ(人気).lnk"
  Delete "$FAVORITES\クリップ.lnk"
  Delete "$STARTMENU\クリップ.lnk"
  Delete "$SMPROGRAMS\クリップ.lnk"

  ;; 旧バージョンで現在は不要ファイルの削除を行う
  ;; eXtreme tRapperの削除(現在はExec起動)
  SimpleSC::RemoveService "Plustar eXtreme tRapper"
  SimpleSC::RemoveService "Plustar eXtreme tRapper Server"
  SimpleSC::RemoveService "mstool"

  ;; 以下のディレクトリのファイルを全て消す
  ;; C:\Documents and Settings\All Users\Application Data\pLook\pDebug
  Delete "$APPDATA\pLook\pDebug\*";
  Delete "$INSTDIR\*.commander"

  ;; 元のユーザに戻す
  SetShellVarContext current

  ;; コンパネ→アンインストールを削除する
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"
  Delete "$INSTDIR\uninstall.exe"

  ;; 新版ではpControler.exeからのキックになるので不要
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pExtrapper"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools.exe"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pCheckWindow"

  ;; 不要なパラメータの削除
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "listup_seconds"
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "ignore_socket_list"
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "logging_messagebox"
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "logging_url"
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "upload_counts"
  DeleteRegValue HKLM "${PS_REGSOFTWARE}\Setting" "stealth"
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; 以下のF/W系は
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; が起動している事を前提としている
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pExTrapper.exe"   "$INSTDIR\pExTrapper.exe"    0 2 "" 1
  SimpleFC::AddApplication "pLook.dll"        "$INSTDIR\pLook.dll"         0 2 "" 1
  SimpleFC::AddApplication "pTools.exe"       "$INSTDIR\pTools.exe"        0 2 "" 1
  SimpleFC::AddApplication "pCheckWindow.exe" "$INSTDIR\pCheckWindow.exe"  0 2 "" 1
  SimpleFC::AddApplication "pControler.exe"   "$INSTDIR\pControler.exe"    0 2 "" 1

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

  ;; pExtrapperの起動
  ;; SimpleSC::StartService "MyService" ""
  SimpleSC::StartService "MSLOOK" ""
  !insertmacro Plustar_CheckProcService "MSLOOK" 500

  ;; システムスタートアップにpToolsを登録
  WriteRegStr HKLM "${PS_RUNSTARTUP}" "pControler" '"${PS_EXT_CTRL_CMD}" --${PS_START_PTOOL}'

  ;; pToolsを起動
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_START_PTOOL}'
  Sleep 1000

  ;; pControlerの起動確認
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_START_PTOOL}' '${PS_CHK_TOOLS}' ${PS_TOOLS_WAIT}

  ;; サービス制御を不可能にする
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_DISABLE}'

  SetAutoClose true
SectionEnd
