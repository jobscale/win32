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
OutFile    "..\pExtADs-Uninstall.exe"
InstallDir "$PROGRAMFILES\Plustar\pADs"

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

;; メッセージ変更
!define MUI_TEXT_INSTALLING_TITLE    "アンインストール"
!define MUI_TEXT_INSTALLING_SUBTITLE "${PS_PRODUCTNAME}をアンインストールしています。しばらくお待ちください。"
!define MUI_TEXT_FINISH_TITLE        "アンインストールの完了"
!define MUI_TEXT_FINISH_SUBTITLE     "アンインストールに成功しました。"
!define MUI_TEXT_ABORT_TITLE         "アンインストールの中止"
!define MUI_TEXT_ABORT_SUBTITLE      "アンインストールは正常に完了されませんでした。"
!define MUI_TEXT_ABORTWARNING        "アンインストールを中止しますか？"

; --------------------------------
; Languages
;
!insertmacro MUI_LANGUAGE "Japanese"

ShowInstDetails   nevershow
ShowUnInstDetails nevershow

;--------------------------------
; Install Section
section "Install"
  !insertmacro Plustar_DetailPrint "アンインストール中..." 100

  ; none：メッセージなし
  ; textonly：テキスト表示
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "d" "${PLUSTAR_VERSION}"

  ;; 実験中...検索ワードを発生させる
  !insertmacro Plustar_Search_Keyword
  ;;

  !insertmacro Plustar_DetailPrint "pAds停止中..." 100

  ;; サービスの停止
  SimpleSC::StopService "pads"
  !insertmacro Plustar_CheckProcStopService "pads" 100

  !insertmacro Plustar_KillProc "pWebBP.exe" ${PS_TOOLS_WAIT} 1500
  !insertmacro Plustar_KillProc "pWebBP.exe" ${PS_TOOLS_WAIT} 1000
  !insertmacro Plustar_KillProc "pADs.exe"   ${PS_TOOLS_WAIT} 100

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

  ;; 旧互換
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pADs"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pWebBP"
  SimpleFC::RemoveApplication "$INSTDIR\pExtADs-Start.exe"
  SimpleFC::RemoveApplication "$WINDIR\pExtADs-Start.exe"
  Delete "$WINDIR\pExtADs-Start.exe"
  Delete "$WINDIR\pExtADs-Start.exe.manifest"

  ;; 信頼済みサイトから「plustar.jp」を削除する
  ;; (HKEY_CURRENT_USER, P("Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp\pad"),
  ;; (HKEY_CURRENT_USER, P("Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp\x"),
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\plustar.jp"
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; スタートアップから削除
  nsExec::Exec '$INSTDIR\pADs.exe --uninstall'
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pAd"

  ;; 串制御を無効にする
  nsExec::Exec /TIMEOUT=2000 '"$INSTDIR\pWebBP.exe" --uninstall'
  Sleep 1000

  ;; F/W登録の解除
  SimpleFC::RemoveApplication "$INSTDIR\pADs.exe"
  SimpleFC::RemoveApplication "$INSTDIR\pWebBP.exe"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; 不要になったファイルを消す
  !insertmacro Plustar_DetailPrint "ファイル削除中..." 1000
  Delete "$INSTDIR\*.cab"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.msi"
  Delete "$INSTDIR\*.log"
  Delete "$INSTDIR\*.txt"
  ;; アップデート関連
  Delete "$INSTDIR\upd"
  Delete "$INSTDIR\inst"
  Delete "$INSTDIR\inst.exe"
  RmDir /r "$INSTDIR"

  !insertmacro Plustar_DetailPrint "アンインストール完了" 2000

  !insertmacro Plustar_CountDown_Message 5 "秒後に自動でアンインストーラは終了します。"

  SetAutoClose true
SectionEnd
