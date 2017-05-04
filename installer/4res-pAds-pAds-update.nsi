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
OutFile    "..\pExtADs-pADs-update.exe"
InstallDir "$PROGRAMFILES\Plustar\pADs"

;管理者権限が必要
RequestExecutionLevel admin

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
  !insertmacro Plustar_Stats_Install_Action "pADse" "u" "${PLUSTAR_VERSION}"

  ;; 実験中…検索ワードを発生させる
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

  !insertmacro Plustar_KillProc "pADs.exe" ${PS_TOOLS_WAIT} 100

  File "..\src\pAds\release\pADs.exe"

  ;; キャッシュの削除
  Delete "$INTERNET_CACHE\*"

  ;; ログがあれば削除する
  Delete "$INSTDIR\pAds.log"

  ;; 前もってサービスの削除を行う
  nsExec::Exec '$INSTDIR\pADs.exe --uninstall'
  Sleep 100

  ;; 信頼済みサイトから「plustar.jp」を削除する
  ;; (HKEY_CURRENT_USER, P("Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp\pad"),
  ;; (HKEY_CURRENT_USER, P("Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp\x"),
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp"

  ;; pAdsのサービス登録
  nsExec::Exec '$INSTDIR\pADs.exe --install'

  ;; サービス停止時に自動起動を行う
  File "..\external\service\sc.exe"
  nsExec::Exec '$INSTDIR\sc.exe failure pads reset= 30 actions= restart/1000'
  sleep 1000
  Delete "$INSTDIR\sc.exe"

  ;; pAdsの起動
  SimpleSC::StartService "pads" ""
  !insertmacro Plustar_CheckProcService "pads" 1500

  ;; スターター
  nsExec::Exec /TIMEOUT=3000 '"$INSTDIR\pWebBP.exe"'

  SetAutoClose true
SectionEnd
