; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=pads -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pADs"
!Define PS_PRODUCTNAME  "Plustar ADs"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"
!Define PS_TOOLS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExtADs-All.exe"
InstallDir "$PROGRAMFILES\Plustar\pADs"

;管理者権限が必要
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

!include "MUI2.nsh"

!define MUI_ICON "icons\setup-pads.ico"
Icon "${MUI_ICON}"

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

  SetOutPath "$INSTDIR"

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "pADsa" "u" "${PLUSTAR_VERSION}"

  ;; 実験中...検索ワードを発生させる
  !insertmacro Plustar_Search_Keyword
  ;;

  ;; 串設定の停止
  !insertmacro Plustar_KillProc "pWebBP.exe" ${PS_TOOLS_WAIT} 1500
  !insertmacro Plustar_KillProc "pWebBP.exe" ${PS_TOOLS_WAIT} 1000

  ;; 串制御を無効にする
  nsExec::Exec /TIMEOUT=2000 '"$INSTDIR\pWebBP.exe" --uninstall'
  Sleep 1000

  ;; サービスの停止
  SimpleSC::StopService "pads"
  !insertmacro Plustar_CheckProcStopService "pads" 100

  ;; 旧互換
  !insertmacro Plustar_KillProc "pADs.exe" ${PS_TOOLS_WAIT} 100

  File "..\src\pAds\release\pADs.exe"
  File "..\src\pAds\release\mgwz.dll"
  File "..\src\pAds\release\pUtils.dll"
  File "..\src\pAds\release\pTemp.dll"
  ;; File "..\src\pAds\release\upd"

  ;; 上位串がある場合は「pWebBP.exe」で吸収する
  ;; pWebBP.conf は不要
  File "..\Release\pWebBP.exe"

  ;; 以下のF/W系は
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; が起動している事を前提としている
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pADs.exe"   "$INSTDIR\pADs.exe"   0 2 "" 1
  SimpleFC::AddApplication "pWebBP.exe" "$INSTDIR\pWebBP.exe" 0 2 "" 1

  ;; 前もってサービスの削除を行う
  nsExec::Exec '$INSTDIR\pADs.exe --uninstall'

  ;; 旧互換
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pADs"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pWebBP"
  SimpleFC::RemoveApplication "$INSTDIR\pExtADs-Start.exe"
  Delete "$INSTDIR\pExtADs-Start.exe"
  Delete "$WINDIR\pExtADs-Start.exe"
  Delete "$WINDIR\pExtADs-Start.exe.manifest"
  Delete "$INSTDIR\pAds.log"
  Delete "$INSTDIR\setup-pads_reboot.ico"
  Delete "$DESKTOP\pAds[起動].lnk"
  Delete "$DESKTOP\pAds[停止].lnk"
  Delete "$INSTDIR\pExtADs-restart-cmd.exe"
  Delete "$INSTDIR\pExtADs-stop-cmd.exe"
  Delete "$INSTDIR\pTem2.dll"

  ;; 信頼済みサイトから「plustar.jp」を削除する
  ;; (HKEY_CURRENT_USER, P("Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp\pad"),
  ;; (HKEY_CURRENT_USER, P("Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp\x"),
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp"
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; pAdsのサービス登録
  nsExec::Exec '$INSTDIR\pADs.exe --install'

  ;; サービス停止時に自動起動を行う
  File "..\external\service\sc.exe"
  nsExec::Exec '$INSTDIR\sc.exe failure pads reset= 30 actions= restart/1000'
  sleep 1000
  Delete "$INSTDIR\sc.exe"

  ;; システムスタートアップにpAdsを登録
  WriteRegStr HKLM "${PS_RUNSTARTUP}" "pAd" '"$INSTDIR\pWebBP.exe"'

  SimpleSC::StartService "pads" ""
  !insertmacro Plustar_CheckProcService "pads" 1500

  ;; スターター
  nsExec::Exec /TIMEOUT=3000 '"$INSTDIR\pWebBP.exe"'

  SetAutoClose true
SectionEnd
