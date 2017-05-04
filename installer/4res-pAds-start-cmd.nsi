; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pADs"
!Define PS_PRODUCTNAME  "Plustar ADs"
!Define PS_TOOLS_WAIT   200

Name       "${PS_PRODUCTNAME}"
OutFile    "..\pExtADs-restart-cmd.exe"
InstallDir "$PROGRAMFILES\Plustar\pADs"

;管理者権限が必要
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
!define MUI_ICON icons\setup-pads_reboot.ico
Icon "${MUI_ICON}"

Caption "${PS_PRODUCTNAME}"

Subcaption 3 " "
XPStyle on
AutoCloseWindow true
ChangeUI all "${NSISDIR}\Contrib\UIs\LoadingBar_Icon.exe"

;--------------------------------
; Install Section
;
section "Install"
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  !insertmacro Plustar_CountDown_Message 3 "秒お待ちください。起動準備中..."

  !insertmacro Plustar_DetailPrint "設定処理中..." 100

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

  ;; キャッシュの削除
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

  !insertmacro Plustar_DetailPrint "起動完了！" 1500

  SetDetailsPrint None
  SetAutoClose    true
SectionEnd
