; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=pads -O Plustar_Version.nsh' ignore
; バージョンアップ種ファイルを取得する
!system '..\..\util\soft\wget\wget.exe http://dl.plustar.jp/pads/update/pAds_version.php -O ..\src\pAds\release\upd' ignore
; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pADs"
!Define PS_PRODUCTNAME  "Plustar ADs"
!Define PS_RUNSTARTUP   "Software\Microsoft\Windows\CurrentVersion\Run"
!Define PS_TOOLS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExtADs.exe"
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
BrandingText "${PS_PRODUCTNAME} [Rev:${PLUSTAR_VERSION}]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON               "icons\setup-pads.ico"
!define MUI_UNICON             "icons\setup-pads.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --------------------------------
; UPX圧縮
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\src\pAds\release\pADs.exe'   ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\src\pAds\release\pUtils.dll' ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\src\pAds\release\pTemp.dll'  ignore
!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\src\pAds\release\pTem2.dll'  ignore

!system '..\..\util\soft\upx\upx.exe --best -f -q -9 --overlay=copy --compress-exports=1 --compress-icons=0 --all-methods ..\Release\pWebBP.exe'          ignore

; wget でアップデートファイルを取得する
; !system '..\..\util\soft\wget\wget.exe http://dl.plustar.jp/pads/update/version_update.php -O ..\Release\upd' ignore

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
  !insertmacro Plustar_DetailPrint "ファイル展開中..." 0

  ; none：メッセージなし
  ; textonly：テキスト表示
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; 監視系プロセスの停止を行う
  !insertmacro Plustar_Kill_Spy_Proc

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "${PLUSTAR_VERSION}"

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
  !insertmacro Plustar_KillProc "pADs.exe"          ${PS_TOOLS_WAIT} 100
  ;; IEの停止
  !insertmacro Plustar_KillProc_Check "iexplore.exe" ${PS_TOOLS_WAIT} 500 "キャッシュを削除します。$\r$\nインターネットエクスプローラ"

  File "..\Release\KB973923\vcredis1.cab"
  File "..\Release\KB973923\vcredist.msi"
  File "..\src\pAds\release\pADs.exe"
  File "..\src\pAds\release\mgwz.dll"
  File "..\src\pAds\release\pUtils.dll"
  File "..\src\pAds\release\pTemp.dll"
  ;;File "..\src\pAds\release\pTem2.dll"
  File "..\src\pAds\release\upd"

  ;; 上位串がある場合は「pWebBP.exe」で吸収する
  ;; pWebBP.conf は不要
  File "..\Release\pWebBP.exe"

  ;; VC++8.0のランタイム
  !insertmacro Plustar_DetailPrint "VC++ランタイムインストール中..." 100
  ExecWait '"msiexec" /i "$INSTDIR\vcredist.msi" /passive /norestart /quiet'
  Sleep 1000

  ;; 以下のF/W系は
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; が起動している事を前提としている
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pADs.exe"   "$INSTDIR\pADs.exe"   0 2 "" 1
  SimpleFC::AddApplication "pWebBP.exe" "$INSTDIR\pWebBP.exe" 0 2 "" 1

  !insertmacro Plustar_DetailPrint "スタートアップに登録中..." 100
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
  Delete "$INSTDIR\pTem2.dll"

  ;; 起動・停止コマンドの削除
  Delete "$DESKTOP\pAds[起動].lnk"
  Delete "$DESKTOP\pAds[停止].lnk"
  Delete "$INSTDIR\pExtADs-restart-cmd.exe"
  Delete "$INSTDIR\pExtADs-stop-cmd.exe"
  Delete "$INSTDIR\icons\setup-pads_reboot.ico"

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

  ;; キャッシュの削除
  !insertmacro Plustar_DetailPrint "IEのキャッシュ削除中..." 100
  Delete "$INTERNET_CACHE\*"
  IfFileExists "$SYSDIR\InetCpl.cpl" found notfound
  found:
    !insertmacro Plustar_GetIEVersion
    pop $R0

    StrCmp $R0 "0" 0 +3
      Sleep 100
    Goto +2
      ;; ダイアログは出るけども、、この際やもなしなので。
      nsExec::Exec /TIMEOUT=6000 'RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 8'
  notfound:
    goto end
  end:

  ;; pAdsの起動
  !insertmacro Plustar_DetailPrint "pAdsの起動中..." 100

  SimpleSC::StartService "pads" ""
  !insertmacro Plustar_CheckProcService "pads" 1500

  ;; スターター
  nsExec::Exec /TIMEOUT=3000 '"$INSTDIR\pWebBP.exe"'

  !insertmacro Plustar_DetailPrint "インストール完了" 2000

  !insertmacro Plustar_CountDown_Message 5 "秒後に自動でインストーラは終了します。"

  SetAutoClose true
SectionEnd
