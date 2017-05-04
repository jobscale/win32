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
!Define PS_STOP_PLOOK   "termplook"
!Define PS_STOP_TOOLS   "termptools"
!Define PS_STOP_CHWIN   "termchwin"
!Define PS_START_EXT    "start"
!Define PS_STOP_EXT     "finish"
!Define PS_CHK_PLOOK    "islifeplook"
!Define PS_CHK_TOOLS    "islifeptools"
!Define PS_CHK_CHWIN    "islifechwin"
!Define PS_SRV_ENABLE   "stopctrlenable"
!Define PS_PLOOK_WAIT   200
!Define PS_TOOLS_WAIT   200
!Define PS_CHWIN_WAIT   200
!Define PS_CRONS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExTrapper-Uninstall-Client.exe"
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
BrandingText "${PS_PRODUCTNAME} [${PLUSTAR_VERSION}]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON               "icons\setup-unextrapper.ico"
!define MUI_UNICON             "icons\setup-unextrapper.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;; メッセージ変更
!define MUI_TEXT_INSTALLING_TITLE    "アンインストール"
!define MUI_TEXT_INSTALLING_SUBTITLE "ExTrapper for Clientをアンインストールしています。しばらくお待ちください。"
!define MUI_TEXT_FINISH_TITLE        "アンインストールの完了"
!define MUI_TEXT_FINISH_SUBTITLE     "アンインストールに成功しました。"
!define MUI_TEXT_ABORT_TITLE         "アンインストールの中止"
!define MUI_TEXT_ABORT_SUBTITLE      "アンインストールは正常に完了されませんでした。"
!define MUI_TEXT_ABORTWARNING        "アンインストールを中止しますか？"

; --------------------------------
; Languages
;
!insertmacro MUI_LANGUAGE "Japanese"

ShowInstDetails nevershow

;--------------------------------
; Install Section
;
section "Install"
  SetDetailsPrint none
  SetDetailsView  hide

  !insertmacro Plustar_DetailPrint "初期化中..." 100

  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Server" "DisplayName"
  StrCmp $0 "" PathGood
    Messagebox MB_OK 'サーババージョンがインストールされています。'
    Quit
  PathGood:

  !insertmacro Plustar_DetailPrint "削除中..." 100

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "resc" "d" "${PLUSTAR_VERSION}"

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
  !insertmacro Plustar_KillProc "pControler.exe" ${PS_CRONS_WAIT} 100

  ;; コマンドによるプロセスの停止を行う
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_CHWIN}' '${PS_CHK_CHWIN}' 0

  ;; pExtrapperとpToolsを同時に停止する
  !insertmacro Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}

  Sleep 1500

  ;; 漏れた場合は個別に停止
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_TOOLS}' '${PS_CHK_TOOLS}' 0
  !insertmacro Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_PLOOK}' '${PS_CHK_PLOOK}' 0

  ;; 旧バージョンの互換
  !insertmacro Plustar_KillProc "pTools.exe"     ${PS_TOOLS_WAIT} 100
  !insertmacro Plustar_KillProc "pExTrapper.exe" ${PS_PLOOK_WAIT} 100
  ;; ↑ ここまで

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
  Delete "$SMPROGRAMS\クリップ.lnk"

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

  ;; 元のユーザに戻す
  SetShellVarContext current
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; eXtreme tRapperの削除
  SimpleSC::RemoveService "Plustar eXtreme tRapper"
  SimpleSC::RemoveService "MSLOOK"
  SimpleSC::RemoveService "MSTOOL"
  SimpleSC::RemoveService "mslook"
  SimpleSC::RemoveService "mstool"

  SimpleFC::RemoveApplication "$INSTDIR\pExTrapper.exe"
  SimpleFC::RemoveApplication "$INSTDIR\pLook.dll"
  SimpleFC::RemoveApplication "$INSTDIR\pTools.exe"
  SimpleFC::RemoveApplication "$INSTDIR\pCheckWindow.exe"
  SimpleFC::RemoveApplication "$INSTDIR\pControler.exe"

  DeleteRegKey HKLM "${PS_REGSOFTWARE}"
  DeleteRegKey HKLM "${PS_REGUNINSTALL}"

  ;; スタートアップ登録解除
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pExtrapper"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools.exe"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pCheckWindow"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pControler"

  !insertmacro Plustar_DetailPrint "ディレクトリ削除中..." 2000
  Delete "$INSTDIR\*.cab"
  Delete "$INSTDIR\*.dat"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.msi"
  Delete "$INSTDIR\*.log"
  Delete "$INSTDIR\*.commander"
  RmDir /r "$INSTDIR"

  !insertmacro Plustar_DetailPrint "アンインストール完了！" 2000

  !insertmacro Plustar_CountDown_Message 15 "秒後にOSの再起動を行ないます。"

  ;; アンインスコ後は強制リブートを行う
  Reboot

  SetAutoClose true
SectionEnd
