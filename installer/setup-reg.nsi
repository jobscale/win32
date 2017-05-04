; ------------------------------------------------------------------
; General
;

!Define PS_PRODUCT     "pReg"
!Define PS_PRODUCTNAME "Plustar REG"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\${PS_PRODUCT}-Setup.exe"
InstallDir "$PROGRAMFILES\Plustar\${PS_PRODUCT}"

!Define PS_RUNSTARTUP  "Software\Microsoft\Windows\CurrentVersion\Run"
!Define RunServices "system\currentcontrolset\Services"
;インストールには管理者権限が必要
RequestExecutionLevel admin

; --------------------------------
; Include Lib
;
!include Plustar_Include.nsh

; --------------------------------
; Pages
;

BrandingText "${PS_PRODUCTNAME} [$$Rev: 2593 $$]"

SetCompressor /SOLID lzma
XPStyle on

!include "MUI2.nsh"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP   "images\plustar-logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                 "icons\setup.ico"
!define MUI_UNICON               "icons\setup.ico"
!define MUI_LICENSEPAGE_CHECKBOX

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!include "nsProcess.nsh"

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
  SetDetailsPrint none
  SetDetailsView  hide

  SetOutPath "$INSTDIR"

  ;; インストール統計を取得する
  StrCpy $3 "http://dl.plustar.jp/c.php?m=${PS_PRODUCT}&t=i"
  push $3
  internetplugin::getpage
  Pop $3
  Sleep 1500
  
  StrCpy $3 "http://dl.plustar.jp/check_lic.php?m=${PS_PRODUCT}"
  push $3
  internetplugin::getpage
  Pop $3

  ;; 今後ここで値のチェックを行う
  ;;Messagebox MB_OK $3

  ;; IEを停止
  !insertmacro Plustar_KillProc "iexplore.exe"  300 100
  !insertmacro Plustar_KillProc "iexplorer.exe" 300 100

  SetOutPath "$WINDIR"
  File "..\src\pLook\res\lib\favicon_clip.ico"
  ;; パスを戻す(念のため)
  SetOutPath "$INSTDIR"

  ;; IEのお気に入りにクリップ
  CreateShortCut "$FAVORITES\クリップ(人気).lnk" "http://clip.plustar.jp/" ""
  CreateShortCut "$FAVORITES\クリップ(動画).lnk" "http://clip.plustar.jp/index.php?category=%25E5%258B%2595%25E7%2594%25BB" ""
  CreateShortCut "$FAVORITES\クリップ-新着.lnk"  "http://feeds.plustar.jp/clip/new" ""
  CreateShortCut "$FAVORITES\クリップ-人気.lnk"  "http://feeds.plustar.jp/clip" ""

  ;; GoogleツールバーにCLIPアイコン追加
  SetOutPath "$LOCALAPPDATA\Google"
  File /r /x ".svn" "..\external\Custom Buttons"
  ;; パスを戻す(念のため)
  SetOutPath "$INSTDIR"

  ;; RSSの登録
  SetOutPath "$LOCALAPPDATA\Microsoft"
  File /r /x ".svn" "..\external\Feeds"
  SetOutPath "$INSTDIR"

  ;; システムリカバリの停止
 ;; WriteRegDWORD HKLM "${RunServices}\srservice" "Start" 4

  ;; ウイルス
  DeleteRegKey  HKLM "${RunServices}\zzz"

  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "Adobe Reader Speed Launcher"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "pPower.exe"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "igfxtray"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "igfxhkcmd"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "igfxpers"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "SoundMan"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "nwiz"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "mmva"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "kmmsoft"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "swg"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "Cmaudio"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "TkBellExe"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "SunJavaUpdateSched"
  DeleteRegValue HKLM "${PS_RUNSTARTUP}" "HBService"

  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "Adobe Reader Speed Launcher"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "pPower.exe"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "igfxtray"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "igfxhkcmd"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "igfxpers"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "SoundMan"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "mmva"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "nwiz"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "kmmsoft"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "swg"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "Cmaudio"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "TkBellExe"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "SunJavaUpdateSched"
  DeleteRegValue HKCU "${PS_RUNSTARTUP}" "HBService"

  Delete "c:\MSDOS.bat"

  Delete "c:\autorun.inf"

  Delete "c:\zzz.sys"
  Delete "$WINDIR\zzz.sys"
  Delete "$SYSDIR\zzz.sys"

  !insertmacro Plustar_KillProc "pq1o2ga.com" 300 100
  Delete "c:\pq1o2ga.com"
  Delete "$WINDIR\pq1o2ga.com"
  Delete "$SYSDIR\pq1o2ga.com"

  !insertmacro Plustar_KillProc "a0fr.bat" 300 100
  Delete "c:\a0fr.bat"
  Delete "$WINDIR\a0fr.bat"
  Delete "$SYSDIR\a0fr.bat"

  !insertmacro Plustar_KillProc "lbo0t.cmd" 300 100
  Delete "c:\lbo0t.cmd"
  Delete "$WINDIR\lbo0t.cmd"
  Delete "$SYSDIR\lbo0t.cmd"

  Delete "$WINDIR\mmvo0.dll"
  Delete "$SYSDIR\mmvo0.dll"

  Delete "$WINDIR\mmvo1.dll"
  Delete "$SYSDIR\mmvo1.dll"

  !insertmacro Plustar_KillProc "mmvo.exe" 300 100
  Delete "$WINDIR\mmvo.exe"
  Delete "$SYSDIR\mmvo.exe"

  Delete "$WINDIR\mtewdh.dll"
  Delete "$SYSDIR\mtewdh.dll"

  !insertmacro Plustar_KillProc "kavo.exe" 300 100
  Delete "$WINDIR\kavo.exe"
  Delete "$SYSDIR\kavo.exe"

  !insertmacro Plustar_KillProc "NBA.exe" 300 100
  Delete "$WINDIR\NBA.exe"
  Delete "$SYSDIR\NBA.exe"

  !insertmacro Plustar_KillProc "k119554168010.exe" 300 100
  Delete "$WINDIR\k119554168010.exe"
  Delete "$SYSDIR\k119554168010.exe"

  !insertmacro Plustar_KillProc "k11975443154.exe" 300 100
  Delete "$SYSDIR\k11975443154.exe"
  Delete "$WINDIR\k11975443154.exe"

  ;;Delete "$WINDIR\nwiz.exe"
  ;;Delete "$SYSDIR\nwiz.exe"

  !insertmacro Plustar_KillProc "0x01xx8p.exe" 300 100
  Delete "$WINDIR\Tasks\0x01xx8p.exe"
  Delete "$WINDIR\Tasks\explorer.ext"
  Delete "$WINDIR\Tasks\sysfile.brk"

  Delete "$SYSDIR\7560.dat"
  Delete "$WINDIR\7560.dat"

  Delete "$SYSDIR\a0.ext"
  Delete "$WINDIR\a0.ext"

  Delete "$WINDIR\Windows.ext"
  ;;Delete "$WINDIR\explorer.ext"
  Delete "$WINDIR\Sysfile.Brk"

  Delete "$SYSDIR\a25.ext"
  Delete "$SYSDIR\oko.exe"
  Delete "$SYSDIR\msosdohs.dat"
  Delete "$SYSDIR\msosdohs00.dll"
  Delete "$SYSDIR\msosdohs01.dll"
  Delete "$SYSDIR\ttEZZEZZ1044.dll"
  Delete "$SYSDIR\ttNNBNNB1047.dll"
  Delete "$SYSDIR\txWWQWWQ1006.dll"

  Delete "$WINDIR\HBmhly.dll"
  Delete "$SYSDIR\HBmhly.dll"

  !insertmacro Plustar_KillProc "revo.exe" 300 100
  Delete "$WINDIR\revo.exe"
  Delete "$SYSDIR\revo.exe"

  Delete "$TEMP\*"
  Delete "$INTERNET_CACHE\*"
  Delete "$COOKIES\*"

;;  WriteRegDWORD HKLM "${RunServices}\Browser" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\Dnscache" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\mnmsrvc" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\RDSessMgr" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\WebClient" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\Netlogon" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\NetDDE" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\NetDDEdsdm" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\Nla" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\RasAuto" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\LmHosts" "Start" 4
;;  ;;WriteRegDWORD HKLM "${RunServices}\Schedule" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\SENS" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\ShellHWDetection" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\seclogon" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\RemoteAccess" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\NtmsSvc" "Start" 4
;;  ;; 追加
;;  WriteRegDWORD HKLM "${RunServices}\UPS" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\WZCSVC" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\SCardSvr" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\ERSvc" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\ClipSrv" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\Alerter" "Start" 4
;;  WriteRegDWORD HKLM "${RunServices}\RDSessMgr" "Start" 4

  ;; IE7の検索にクリップを追加
  WriteRegDWORD HKCU "Software\Google\GoogleToolbarNotifier" "ShowTrayIcon" 0
  WriteRegDWORD HKCU "Software\Google\GoogleToolbarNotifier" "KeepDS" 0
  WriteRegStr HKCU "Software\Microsoft\Internet Explorer\SearchScopes" "DefaultScope" '{DD0C8368-63FA-43A8-AB62-0D585897E500}'
  WriteRegStr HKCU "Software\Microsoft\Internet Explorer\SearchScopes\{DD0C8368-63FA-43A8-AB62-0D585897E500}" "DisplayName" "検索"
  WriteRegStr HKCU "Software\Microsoft\Internet Explorer\SearchScopes\{DD0C8368-63FA-43A8-AB62-0D585897E500}" "URL" "http://clip.plustar.jp/search.php?type=bx&search={searchTerms}"
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\SearchScopes\{DD0C8368-63FA-43A8-AB62-0D585897E500}" "Codepage" 0x0000fde9

  ;; デフォ検索を変更
  WriteRegStr HKCU "Software\Microsoft\Internet Explorer\SearchUrl" "" "http://clip.plustar.jp/search.php?type=bx&search=%s"
  WriteRegStr HKCU "Software\Microsoft\Search Assistant" "DefaultSearchURL" "http://clip.plustar.jp/search.php?type=bx&search=%s"

  ;; 作業フォルダーをwindowsにする
  SetOutPath "$WINDIR"
  ;; メニューにクリップの登録
  CreateShortCut  "$SMPROGRAMS\クリップ.lnk" "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  ;;CreateShortCut  "$DESKTOP\クリップ.lnk" "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"

  ;; all usersに変更
  SetShellVarContext all
  CreateShortCut "$STARTMENU\クリップ(人気).lnk" "http://clip.plustar.jp/" "" "$WINDIR\favicon_clip.ico"
  CreateShortCut "$STARTMENU\クリップ(動画).lnk" "http://clip.plustar.jp/index.php?category=%25E5%258B%2595%25E7%2594%25BB" "" "$WINDIR\favicon_clip.ico"

  ;;File "..\sChecker.exe"
  ;; システムのrunに登録
  ;;SetShellVarContext current
  ;;CreateShortCut  "$SMSTARTUP\start.lnk" "$WINDIR\sChecker.exe" "/S"
  ;;SetOutPath "$INSTDIR"

  SetAutoClose true
SectionEnd

;--------------------------------
; Uninstall Section
;

Section "Uninstall"
SectionEnd
