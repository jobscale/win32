; ------------------------------------------------------------------
; wget で最新リビジョンを取得する
!system '..\..\util\soft\wget\wget.exe http://p.plustar.jp/repv.php?t=exts -O Plustar_Version.nsh' ignore

; ------------------------------------------------------------------
; General
;
!Define PS_PRODUCT     "pExTrapperStopTestTools"
!Define PS_PRODUCTNAME "pExTrapper Test Tools"
!Define PS_MUICACHE    "Software\Microsoft\Windows\ShellNoRoam\MUICache"

Name       "${PS_PRODUCTNAME}"
OutFile    "..\${PS_PRODUCT}.exe"
InstallDir "$DESKTOP\PS"

; --------------------------------
; Include Lib
;
!include LogicLib.nsh
!include WordFunc.nsh
!include "Time.nsh"
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
!define MUI_ICON               "icons\setup-extrapper.ico"
!define MUI_UNICON             "icons\setup-extrapper.ico"

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --------------------------------
; Languages
;
!insertmacro MUI_LANGUAGE "Japanese"

;--------------------------------
; Install Section
;
var PS_TARGET_EXE_WAIT
var t1 ;; 日
var t2 ;; 月
var t3 ;; 年
var t4 ;; 時間
var t5 ;; 分
var t6 ;; 秒

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

section "Install"
  StrCpy $PS_TARGET_EXE_WAIT 5000

  ${time::GetLocalTime} $0
  ${time::TimeString} "$0" $t1 $t2 $t3 $t4 $t5 $t6

  DetailPrint "テスト開始：：$t3年$t2月$t1日 $t4時$t5分$t6秒..."

  ;; インストール統計を取得する
  !insertmacro Plustar_Stats_Install_Action "${PS_PRODUCT}" "i" "${PLUSTAR_VERSION}"

  ;; テストでcalcをBitCometに変更して実験
  DetailPrint "テストでcalcをBitCometに変更して起動停止実験..."
  CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\BitComet.exe"

  Exec '$TEMP\BitComet.exe'
  Sleep 10000

  ;; 多重起動実験
  DetailPrint "ダミーBitComet多重起動実験..."
  IntFmt $0 "%u" 0
  execbcloop:
    Exec '$TEMP\BitComet.exe'

    IntOp $0 $0 + 1
    IntCmp $0 20 execbcloopdone execbcloop

  execbcloopdone:
    Sleep 100

  Sleep 10000

  Delete "$TEMP\BitComet.exe"
  DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\BitComet.exe"

  StrCpy $0 "各種ダミーP2Pテストを続けますか？$\r$\n$\r$\n"
  StrCpy $0 "$08セットの起動停止実験を行います。$\r$\n"
  StrCpy $0 "$0 winny2.exe$\r$\n"
  StrCpy $0 "$0 share.exe$\r$\n"
  StrCpy $0 "$0 cabos.exe$\r$\n"
  StrCpy $0 "$0 shareaza.exe$\r$\n"
  StrCpy $0 "$0 Gigaget.exe$\r$\n"
  StrCpy $0 "$0 kazaalite.exe$\r$\n"
  StrCpy $0 "$0 bittorrent.exe$\r$\n"
  StrCpy $0 "$0 torrent.exe$\r$\n"
  StrCpy $0 "$0 limewire.exe$\r$\n"
  StrCpy $0 "$0 perfect dark.exe$\r$\n"
  StrCpy $0 "$0 kazaa.exe$\r$\n"
  StrCpy $0 "$0などなど"
  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkAnyP2P
    DetailPrint "テストでcalcを各種P2Pに変更して8セットの起動停止実験..."

    ;; テストでcalcをwinny2に変更して実験
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\winny2.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\share.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\cabos.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\shareaza.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Gigaget.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\kazaalite.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\bittorrent.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\torrent.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\limewire.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\perfect dark.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\kazaa.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\guntella.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\rufus.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\wxdfast.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\webdownload.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\sdp.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\dep.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\flvdownloader.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\flashget.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\dsdl.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\bukster.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\daman.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\downup2u.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\fdm.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\hidownload.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\leechget.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\neodownloader.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\orbitnet.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\webripper.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\stardown.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\truedownloader.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\ultraget.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\wellget.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\grab.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\irvine.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\UPnPCJ.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Getter1.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\NetAnts.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Phex.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\FrostWire.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Azureus.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\kazaalite.kpp"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\emule.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\btdna.exe"

    Exec '$TEMP\winny2.exe'
    Exec '$TEMP\share.exe'
    Exec '$TEMP\cabos.exe'
    Exec '$TEMP\shareaza.exe'
    Exec '$TEMP\Gigaget.exe'
    Sleep $PS_TARGET_EXE_WAIT

    Exec '$TEMP\kazaalite.exe'
    Exec "$TEMP\bittorrent.exe"
    Exec "$TEMP\torrent.exe"
    Exec "$TEMP\limewire.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\perfect dark.exe"
    Exec "$TEMP\kazaa.exe"
    Exec "$TEMP\guntella.exe"
    Exec "$TEMP\rufus.exe"
    Exec "$TEMP\wxdfast.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\webdownload.exe"
    Exec "$TEMP\sdp.exe"
    Exec "$TEMP\dep.exe"
    Exec "$TEMP\flvdownloader.exe"
    Exec "$TEMP\flashget.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\dsdl.exe"
    Exec "$TEMP\bukster.exe"
    Exec "$TEMP\daman.exe"
    Exec "$TEMP\downup2u.exe"
    Exec "$TEMP\fdm.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\hidownload.exe"
    Exec "$TEMP\leechget.exe"
    Exec "$TEMP\neodownloader.exe"
    Exec "$TEMP\orbitnet.exe"
    Exec "$TEMP\webripper.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\stardown.exe"
    Exec "$TEMP\truedownloader.exe"
    Exec "$TEMP\ultraget.exe"
    Exec "$TEMP\wellget.exe"
    Exec "$TEMP\grab.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\irvine.exe"
    Exec "$TEMP\UPnPCJ.exe"
    Exec "$TEMP\Getter1.exe"
    Exec "$TEMP\NetAnts.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\Phex.exe"
    Exec "$TEMP\FrostWire.exe"
    Exec "$TEMP\Azureus.exe"
    Exec "$TEMP\kazaalite.kpp"
    Exec "$TEMP\emule.exe"
    Exec "$TEMP\btdna.exe"
    Sleep $PS_TARGET_EXE_WAIT

    ;; コピーしたファイルのお掃除
    Delete '$TEMP\winny2.exe'
    Delete '$TEMP\share.exe'
    Delete '$TEMP\cabos.exe'
    Delete '$TEMP\shareaza.exe'
    Delete '$TEMP\Gigaget.exe'
    Delete '$TEMP\kazaalite.exe'
    Delete "$TEMP\bittorrent.exe"
    Delete "$TEMP\torrent.exe"
    Delete "$TEMP\limewire.exe"
    Delete "$TEMP\perfect dark.exe"
    Delete "$TEMP\kazaa.exe"
    Delete "$TEMP\guntella.exe"
    Delete "$TEMP\rufus.exe"
    Delete "$TEMP\wxdfast.exe"
    Delete "$TEMP\webdownload.exe"
    Delete "$TEMP\sdp.exe"
    Delete "$TEMP\dep.exe"
    Delete "$TEMP\flvdownloader.exe"
    Delete "$TEMP\flashget.exe"
    Delete "$TEMP\dsdl.exe"
    Delete "$TEMP\bukster.exe"
    Delete "$TEMP\daman.exe"
    Delete "$TEMP\downup2u.exe"
    Delete "$TEMP\fdm.exe"
    Delete "$TEMP\hidownload.exe"
    Delete "$TEMP\leechget.exe"
    Delete "$TEMP\neodownloader.exe"
    Delete "$TEMP\orbitnet.exe"
    Delete "$TEMP\webripper.exe"
    Delete "$TEMP\stardown.exe"
    Delete "$TEMP\truedownloader.exe"
    Delete "$TEMP\ultraget.exe"
    Delete "$TEMP\wellget.exe"
    Delete "$TEMP\grab.exe"
    Delete "$TEMP\irvine.exe"
    Delete "$TEMP\UPnPCJ.exe"
    Delete "$TEMP\Getter1.exe"
    Delete "$TEMP\NetAnts.exe"
    Delete "$TEMP\Phex.exe"
    Delete "$TEMP\FrostWire.exe"
    Delete "$TEMP\Azureus.exe"
    Delete "$TEMP\kazaalite.kpp"
    Delete "$TEMP\emule.exe"
    Delete "$TEMP\btdna.exe"

    ;; レジストリのお掃除
    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\winny2.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\share.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\cabos.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\shareaza.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\Gigaget.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" '$TEMP\kazaalite.exe'
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\bittorrent.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\torrent.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\limewire.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\perfect dark.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\kazaa.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\guntella.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\rufus.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\wxdfast.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\webdownload.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\sdp.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\dep.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\flvdownloader.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\flashget.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\dsdl.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\bukster.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\daman.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\downup2u.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\fdm.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\hidownload.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\leechget.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\neodownloader.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\orbitnet.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\webripper.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\stardown.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\truedownloader.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\ultraget.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\wellget.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\grab.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\irvine.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\UPnPCJ.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Getter1.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\NetAnts.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Phex.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\FrostWire.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Azureus.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\kazaalite.kpp"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\emule.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\btdna.exe"
  DoWorkAnyP2P:

  StrCpy $0 "プラスター定義分のテストを行ないますか？"
  MessageBox MB_YESNO|MB_ICONQUESTION $0 IDNO DoWorkPlustarP2P
    DetailPrint "テストでcalcを各種P2Pに変更して起動停止実験..."

    ;; テストでcalcをwinny2に変更して実験
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\webdownload.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\webripper.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\wellget.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Taitoru.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Moo.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\MailMagicPro.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\MailDistributor.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Downstair.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\SnatteiServer.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Snattei.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\ReGet.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\A-Downloader.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\DLSpider3.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\MailMagic2.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\ReGetDx.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\BitBuddy.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\AresGold.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\LemonWire.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Blubster.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\Ares.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\P2P_Energy.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\NeoDownloader.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\BitThief.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\bywifidl.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\bywifici.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\bywifi.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\FrostWire.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\DCPlusPlus.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\BssUpNp.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\kaihoukun.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\ShareazaPlus.exe"
    CopyFiles /SILENT "$SYSDIR\calc.exe" "$TEMP\sharktorrent.exe"

    Exec "$TEMP\webdownload.exe"
    Exec "$TEMP\webripper.exe"
    Exec "$TEMP\wellget.exe"
    Exec "$TEMP\Taitoru.exe"
    Exec "$TEMP\Moo.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\MailMagicPro.exe"
    Exec "$TEMP\MailDistributor.exe"
    Exec "$TEMP\Downstair.exe"
    Exec "$TEMP\SnatteiServer.exe"
    Exec "$TEMP\Snattei.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\ReGet.exe"
    Exec "$TEMP\A-Downloader.exe"
    Exec "$TEMP\DLSpider3.exe"
    Exec "$TEMP\MailMagic2.exe"
    Exec "$TEMP\ReGetDx.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\BitBuddy.exe"
    Exec "$TEMP\AresGold.exe"
    Exec "$TEMP\LemonWire.exe"
    Exec "$TEMP\Blubster.exe"
    Exec "$TEMP\Ares.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\P2P_Energy.exe"
    Exec "$TEMP\NeoDownloader.exe"
    Exec "$TEMP\BitThief.exe"
    Exec "$TEMP\bywifidl.exe"
    Exec "$TEMP\bywifici.exe"
    Sleep $PS_TARGET_EXE_WAIT

    Exec "$TEMP\bywifi.exe"
    Exec "$TEMP\FrostWire.exe"
    Exec "$TEMP\DCPlusPlus.exe"
    Exec "$TEMP\BssUpNp.exe"
    Exec "$TEMP\kaihoukun.exe"
    Exec "$TEMP\ShareazaPlus.exe"
    Exec "$TEMP\sharktorrent.exe"
    Sleep $PS_TARGET_EXE_WAIT

    ;; コピーしたファイルのお掃除
    Delete "$TEMP\webdownload.exe"
    Delete "$TEMP\webripper.exe"
    Delete "$TEMP\wellget.exe"
    Delete "$TEMP\Taitoru.exe"
    Delete "$TEMP\Moo.exe"
    Delete "$TEMP\MailMagicPro.exe"
    Delete "$TEMP\MailDistributor.exe"
    Delete "$TEMP\Downstair.exe"
    Delete "$TEMP\SnatteiServer.exe"
    Delete "$TEMP\Snattei.exe"
    Delete "$TEMP\ReGet.exe"
    Delete "$TEMP\A-Downloader.exe"
    Delete "$TEMP\DLSpider3.exe"
    Delete "$TEMP\MailMagic2.exe"
    Delete "$TEMP\ReGetDx.exe"
    Delete "$TEMP\BitBuddy.exe"
    Delete "$TEMP\AresGold.exe"
    Delete "$TEMP\LemonWire.exe"
    Delete "$TEMP\Blubster.exe"
    Delete "$TEMP\Ares.exe"
    Delete "$TEMP\P2P_Energy.exe"
    Delete "$TEMP\NeoDownloader.exe"
    Delete "$TEMP\BitThief.exe"
    Delete "$TEMP\bywifidl.exe"
    Delete "$TEMP\bywifici.exe"
    Delete "$TEMP\bywifi.exe"
    Delete "$TEMP\FrostWire.exe"
    Delete "$TEMP\DCPlusPlus.exe"
    Delete "$TEMP\BssUpNp.exe"
    Delete "$TEMP\kaihoukun.exe"
    Delete "$TEMP\ShareazaPlus.exe"
    Delete "$TEMP\sharktorrent.exe"

    ;; レジストリのお掃除
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\webdownload.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\webripper.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\wellget.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Taitoru.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Moo.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\MailMagicPro.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\MailDistributor.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Downstair.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\SnatteiServer.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Snattei.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\ReGet.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\A-Downloader.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\DLSpider3.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\MailMagic2.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\ReGetDx.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\BitBuddy.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\AresGold.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\LemonWire.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Blubster.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\Ares.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\P2P_Energy.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\NeoDownloader.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\BitThief.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\bywifidl.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\bywifici.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\bywifi.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\FrostWire.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\DCPlusPlus.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\BssUpNp.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\kaihoukun.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\ShareazaPlus.exe"
    DeleteRegValue HKCU "${PS_MUICACHE}" "$TEMP\sharktorrent.exe"
  DoWorkPlustarP2P:


  ${time::GetLocalTime} $0
  ${time::TimeString} "$0" $t1 $t2 $t3 $t4 $t5 $t6

  DetailPrint "テスト終了：：$t3年$t2月$t1日 $t4時$t5分$t6秒..."
  DetailPrint "お疲れ様でした"

SectionEnd
