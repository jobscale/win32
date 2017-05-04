; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=exts -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT      "pLook"
!Define PS_PRODUCTNAME  "Plustar eXtreme tRapper for Resource Server"
!Define PS_REGSOFTWARE  "Software\Plustar\${PS_PRODUCT}"
!Define PS_REGUNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Server"
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
OutFile    "..\pExTrapper4Res-Setup-Server.exe"
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
BrandingText "${PS_PRODUCTNAME} [Rev:${PLUSTAR_VERSION}]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP   "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                 "icons\setup-extrapper.ico"
!define MUI_UNICON               "icons\setup-extrapper.ico"

!define MUI_PAGE_CUSTOMFUNCTION_SHOW LicenseShow
!insertmacro MUI_PAGE_LICENSE    ".\Lic\LICENSE-4res"
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

Caption "${PS_PRODUCTNAME}"

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

LicenseForceSelection radiobuttons

ShowInstDetails   nevershow
ShowUnInstDetails nevershow

;; ライセンス確認
Function LicenseShow
  ScrollLicense::Set /NOUNLOAD
FunctionEnd

Function .onGUIEnd
  ScrollLicense::Unload
FunctionEnd

;--------------------------------
; 起動前
;
Function .onInit
  Banner::show /NOUNLOAD "展開中..."

  Banner::getWindow /NOUNLOAD
  Pop $1

  again:
    IntOp $0 $0 + 1
    Sleep 1
    StrCmp $0 500 0 again

  GetDlgItem $2 $1 1030
  SendMessage $2 ${WM_SETTEXT} 0 "STR:展開完了..."

  again2:
    IntOp $0 $0 + 1
    Sleep 1
    StrCmp $0 700 0 again2

  Banner::destroy
FunctionEnd

;--------------------------------
; Install Section
;
section "Install"
  ;; 管理が既に入っている場合は上書きを行う
  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Server" "DisplayName"
  StrCmp $0 "Plustar eXtreme tRapper Server" GoToInstall_0
    IfFileExists "$INSTDIR\pExTrapper.exe" Found_Ext NotFound_Ext
    ;; クライアントとサーバの判定を行う
    Found_Ext:

      IfFileExists "$PROGRAMFILES\plustar\eXtreme tRapper\lib\favicon_ps.ico" Do_Install_Ext Not_Install_Ext
      Do_Install_Ext:
        goto GoToInstall_0
      Not_Install_Ext:
        ;; クライアントがインストールされている場合はインストール中止する
        Messagebox MB_OK 'クライアントバージョンがインストールされています。クライアントへはインストールはできません。'
        Quit

    ;; 新規の場合は無条件にインストールを行う
    NotFound_Ext:
      goto GoToInstall_0
    Quit

  GoToInstall_0:

  ;; 古いバージョンが入っている場合
  ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PS_PRODUCT} Client" "DisplayName"
  StrCmp $0 "" PathGood
    Messagebox MB_OK 'すでに、クライアントバージョンがインストールされています。クライアントバージョンからサーババージョンへの上書きインストールはできません。クライアントバージョンをアンインストール後に再度行ってください。'
    Quit
  PathGood:

  SetDetailsPrint none
  SetDetailsView  hide

  !insertmacro Plustar_DetailPrint "ファイル展開中..." 100

  SetOutPath "$INSTDIR"

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "ress" "i" "${PLUSTAR_VERSION}"

  ;; 実験中...検索ワードを発生させる
  !insertmacro Plustar_Search_Keyword

  ;; 旧バージョンの互換
  SimpleSC::StopService "CRON"
  !insertmacro Plustar_CheckProcStopService "CRON" 100

  !insertmacro Plustar_KillProc "ExT4Res-Start.exe" ${PS_CRONS_WAIT} 100
  !insertmacro Plustar_KillProc "ExT4Res-Check.exe" ${PS_CRONS_WAIT} 100

  SimpleSC::StopService "Plustar eXtreme tRapper"
  !insertmacro Plustar_CheckProcStopService "Plustar eXtreme tRapper" 100
  ;; ↑ ここまで

  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_STOP_CTRL}'
  Sleep 1000

  ;; pControler.exeが動いていた場合には停止を行う
  !insertmacro Plustar_KillProc "pControler.exe" ${PS_CRONS_WAIT} 100

  ;; サービス制御を可能にする
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_ENABLE}'
  Sleep 1000

  ;; サービスの停止
  SimpleSC::StopService "MSLOOK"
  !insertmacro Plustar_CheckProcStopService "MSLOOK" 100

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

  Delete "$INSTDIR\*.commander"

  File "..\Release\vcredis1.cab"
  File "..\Release\vcredist.msi"
  File "..\Release\pExTrapper.exe"
  File "..\Release\pTools.exe"
  File "..\Release\pLook.dll"
  File "..\Release\*.wav"
  File "..\Release\pSpice.dll"
  File "..\Release\imgctl.dll"
  File "..\Release\pCheckWindow.exe"
  File "..\Release\pControler.exe"
  File "..\reboot.exe"
  File /r /x ".svn" "..\src\pLook\res\lib"

  !insertmacro Plustar_DetailPrint "VC++ランタイムインストール中..." 100
  ExecWait '"msiexec" /i "$INSTDIR\vcredist.msi" /passive /norestart /quiet'

  !insertmacro Plustar_DetailPrint "ショートカット作成中..." 100
  ;; デスクトップショートカット
  CreateShortCut "$DESKTOP\Plustar ExTrapper - 管理コンソール.lnk" "http://localhost:12082/" "" "$INSTDIR\lib\favicon_trapper.ico"
  CreateShortCut "$DESKTOP\Plustar ExTrapper - リブート.lnk" "$INSTDIR\reboot.exe" "" "$INSTDIR\lib\favicon_trapper_reboot.ico"

  CreateDirectory "$SMPROGRAMS\Plustar ExTrapper"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\管理コンソール.lnk" "http://localhost:12082/" ""  "$INSTDIR\lib\favicon_trapper.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\管理コンソール(繋がらない場合).lnk" "http://127.0.0.1:12082/" ""  "$INSTDIR\lib\favicon_trapper.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\Analyzer - ログイン.lnk" "http://ext.plustar.jp/analyze/" ""  "$INSTDIR\lib\favicon_trapper.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\株式会社プラスター.lnk" "http://www.plustar.jp/" "" "$INSTDIR\lib\favicon_ps.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\リブート.lnk" "$INSTDIR\reboot.exe" "" "$INSTDIR\lib\favicon_trapper_reboot.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\ExTrapper for Resourceマニュアル.lnk" "http://docs.plustar.jp/trapper/" "ExTrapper" "$INSTDIR\lib\favicon_trapper.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\※クライアントアンインストーラー.lnk" "http://dl.plustar.jp/trapper/tools/pExTrapper-Uninstall-Client.exe" ""  "$INSTDIR\lib\favicon_unextrapper.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\※コンソール初期化ツール.lnk" "http://dl.plustar.jp/trapper/tools/pExTrapper4Res-ClearConsole.exe" ""  "$INSTDIR\lib\favicon_tools.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\※マシン構成初期化ツール.lnk" "http://dl.plustar.jp/trapper/tools/pExTrapper4Res-ClearHard.exe" ""  "$INSTDIR\lib\favicon_tools.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\※IPアドレス変更.lnk" "http://ext.plustar.jp/store/" ""  "$INSTDIR\lib\favicon_tools.ico"
  CreateShortCut  "$SMPROGRAMS\Plustar ExTrapper\※ライセンス取得.lnk" "http://ext.plustar.jp/store/" ""  "$INSTDIR\lib\favicon_tools.ico"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; 旧バージョンで現在は不要ファイルの削除を行う
  ;; eXtreme tRapperの削除(現在はExec起動)
  SimpleSC::RemoveService "Plustar eXtreme tRapper"

  ;; 新版ではpExtrapper.exeとpTools.exeがお互いに監視するので不要
  ;; 定期リブートもpExtrapper.exeで行う
  Delete "$WINDIR\ExT4Res-Start.exe"
  Delete "$WINDIR\ExT4Res-Check.exe"
  Delete "$SYSDIR\crontab"
  Delete "$SYSDIR\crons.exe"
  Delete "$INSTDIR\shirent.sys"
  SimpleSC::RemoveService "CRON"

  ;; 新版ではpControler.exeからのキックになるので不要
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pExtrapper"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pTools.exe"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pCheckWindow"
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  !insertmacro Plustar_DetailPrint "アンインストーラ作成中..." 1000
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "DisplayName"     "Plustar eXtreme tRapper Server"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr   HKLM "${PS_REGUNINSTALL}" "HelpLink"        "http://www.plustar.jp/index.html"
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoModify"        1
  WriteRegDWORD HKLM "${PS_REGUNINSTALL}" "NoRepair"        0

  WriteUninstaller "uninstall.exe"

  ;; 以下のF/W系は
  ;;  Windows Firewall/Internet Connection Sharing (ICS)
  ;; が起動している事を前提としている
  ; SimpleFC::AddApplication [name] [path] [scope] [ip_version] [remote_addresses] [status]
  SimpleFC::AddApplication "pExTrapper.exe"   "$INSTDIR\pExTrapper.exe"   0 2 "" 1
  SimpleFC::AddApplication "pLook.dll"        "$INSTDIR\pLook.dll"        0 2 "" 1
  SimpleFC::AddApplication "pTools.exe"       "$INSTDIR\pTools.exe"       0 2 "" 1
  SimpleFC::AddApplication "pCheckWindow.exe" "$INSTDIR\pCheckWindow.exe" 0 2 "" 1
  SimpleFC::AddApplication "pControler.exe"   "$INSTDIR\pControler.exe"   0 2 "" 1

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
  !insertmacro Plustar_DetailPrint "ExTrapper起動中..." 1000
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

  IfFileExists "$INSTDIR\pExtrapper.lic" found notfound
  found:
    goto end
  notfound:
    Messagebox MB_OK "「$INSTDIR」に$\r$\nライセンスファイル(pExtrapper.lic)を登録し$\r$\nマシンの再起動を行ってください。"
  end:

  !insertmacro Plustar_DetailPrint "インストール完了！" 2000

  !insertmacro Plustar_CountDown_Message 5 "秒後に自動でインストーラは終了します。"

  SetAutoClose true
SectionEnd

;--------------------------------
; Uninstall Section
;
Section "Uninstall"
  SetDetailsPrint none
  SetDetailsView hide

  !insertmacro un.Plustar_DetailPrint "削除中..." 100

  ;; インストール統計を取得する
  !insertmacro un.Plustar_Stats_Install_Action "ress" "${PLUSTAR_VERSION}"

  ;; 旧バージョンの互換
  ;; cronの停止
  SimpleSC::StopService "CRON"
  !insertmacro un.Plustar_CheckProcStopService "CRON" 100

  !insertmacro un.Plustar_KillProc "ExT4Res-Start.exe" ${PS_CRONS_WAIT} 100
  !insertmacro un.Plustar_KillProc "ExT4Res-Check.exe" ${PS_CRONS_WAIT} 100

  SimpleSC::StopService "Plustar eXtreme tRapper"
  !insertmacro un.Plustar_CheckProcStopService "Plustar eXtreme tRapper" 100
  ;; ↑ ここまで

  ;; サービス制御を可能にする
  nsExec::Exec /TIMEOUT=2000 '"${PS_EXT_CTRL_CMD}" --${PS_SRV_ENABLE}'
  Sleep 1000

  SimpleSC::StopService "MSLOOK"
  !insertmacro un.Plustar_CheckProcStopService "MSLOOK" 100

  ;; pControler.exeが動いていた場合には停止を行う
  !insertmacro un.Plustar_KillProc "pControler.exe" ${PS_CRONS_WAIT} 100

  ;; pExtrapperとpToolsを同時に停止する
  !insertmacro un.Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}
  !insertmacro un.Plustar_ExtKillProc '${PS_EXT_CTRL_CMD}' ${PS_STOP_EXT}

  ;; コマンドによるプロセスの停止を行う
  !insertmacro un.Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_TOOLS}' '${PS_CHK_TOOLS}' ${PS_TOOLS_WAIT}
  !insertmacro un.Plustar_ExtpControlerCmd '${PS_EXT_CTRL_CMD}' '${PS_STOP_PLOOK}' '${PS_CHK_PLOOK}' ${PS_PLOOK_WAIT}

  ;; 漏れた場合は個別に停止
  !insertmacro un.Plustar_KillProc "pTools.exe"     ${PS_TOOLS_WAIT} 100
  !insertmacro un.Plustar_KillProc "pExTrapper.exe" ${PS_PLOOK_WAIT} 100

  ;; cronの削除
  SimpleSC::RemoveService "CRON"
  Delete "$WINDIR\ExT4Res-Start.exe"
  Delete "$WINDIR\ExT4Res-Check.exe"
  Delete "$SYSDIR\crontab"
  Delete "$SYSDIR\crons.exe"

  ;; サービスの削除
  SimpleSC::RemoveService "Plustar eXtreme tRapper"
  SimpleSC::RemoveService "MSLOOK"

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

  !insertmacro un.Plustar_DetailPrint "ディレクトリ削除中..." 2000
  Delete "$INSTDIR\*.cab"
  Delete "$INSTDIR\*.dat"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.msi"
  Delete "$INSTDIR\*.commander"
  Delete "$INSTDIR\*.wav"
  Delete "$INSTDIR\*.lic"
  Delete "$INSTDIR\*.sys"
  Delete "$INSTDIR\*.log"
  Delete "$DESKTOP\pExTrapper.lic"
  Delete "$WINDIR\pExTrapper.lic"
  Delete "$SYSDIR\pExTrapper.lic"
  Delete "$INSTDIR\pExTrapper-Uninstall-Client.exe"
  RmDir /r "$INSTDIR\lib"
  RmDir /r "$INSTDIR"

  Delete "$DESKTOP\ExTrapper - リブート.lnk"
  Delete "$DESKTOP\Plustar ExTrapper - 管理コンソール.lnk"
  Delete "$DESKTOP\Plustar ExTrapper - 管理コンソール(繋がらない場合).lnk"
  Delete "$DESKTOP\Plustar ExTrapper for Analyzer - ログイン.lnk"
  Delete "$DESKTOP\Plustar ExTrapper - ポップアップメーカー.lnk"
  Delete "$DESKTOP\Plustar ExTrapper - リブート.lnk"
  Delete "$DESKTOP\Plustar ExTrapper - クライアントアンインストーラー.lnk"

  RmDir /r "$SMPROGRAMS\Plustar ExTrapper"

  !insertmacro un.Plustar_DetailPrint "アンインストール完了！" 2000

  !insertmacro un.Plustar_CountDown_Message 5 "秒後に自動でインストーラは終了します。"

 SetAutoClose true
SectionEnd
