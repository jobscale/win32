; ---------------------
; Plustar_Include.nsh
; ---------------------
;
!ifndef ___PLUSTAR_INCLUDE__NSH___
!define ___PLUSTAR_INCLUDE__NSH___

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; プリントログ表示
;;
;; インストール     ：◯
;; アンインストール ：◯
;;
;; 引数  ：
;;        _PRINT_MSG       ：メッセージ
;;        _SLEEP_AFTER_MSG ：メッセージ表示後ウェイト
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_DetailPrint _PRINT_MSG _SLEEP_AFTER_MSG
  SetDetailsPrint textonly
  DetailPrint "${_PRINT_MSG}"
  SetDetailsPrint listonly
  Sleep ${_SLEEP_AFTER_MSG}
!macroend
!macro un.Plustar_DetailPrint _PRINT_MSG _SLEEP_AFTER_MSG
  !insertmacro Plustar_DetailPrint "${_PRINT_MSG}" ${_SLEEP_AFTER_MSG}
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 監視系プロセスの停止
;;
;; インストール     ：◯
;; アンインストール ：◯
;;
;; 引数  ：なし
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_Kill_Spy_Proc
  !insertmacro Plustar_KillProc "PROCEXP.EXE" 100 100
  !insertmacro Plustar_KillProc "Filemon.exe" 100 100
  !insertmacro Plustar_KillProc "Regmon.exe"  100 100
  !insertmacro Plustar_KillProc "taskmgr.exe" 100 100
!macroend
!macro un.Plustar_Kill_Spy_Proc
  !insertmacro Plustar_Kill_Spy_Proc
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; インストール時に検索ワードを発生させる
;;
;; インストール     ：◯
;; アンインストール ：◯
;;
;; 引数  ：なし
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_Search_Keyword
  ;; 実験中…検索ワードを発生させる
  ;; 「プラスター ネットワーク」

  StrCpy $3 "http://www.google.co.jp/"
  Push $3
  internetplugin::getpage
  Pop $3

  StrCpy $3 "http://www.yahoo.co.jp/"
  Push $3
  internetplugin::getpage
  Pop $3

  StrCpy $3 "http://www.bing.com/"
  Push $3
  internetplugin::getpage
  Pop $3

  StrCpy $3 "http://www.naver.jp/"
  Push $3
  internetplugin::getpage
  Pop $3
  Sleep 1000

  StrCpy $3 "http://www.google.co.jp/#hl=ja&source=hp&biw=847&bih=654&q=%E3%83%97%E3%83%A9%E3%82%B9%E3%82%BF%E3%83%BC+%E3%83%8D%E3%83%83%E3%83%88%E3%83%AF%E3%83%BC%E3%82%AF&aq=f&aqi=&aql=&oq=&pbx=1&bav=on.2,or.r_gc.r_pw."
  Push $3
  internetplugin::getpage
  Pop $3
  Sleep 100

  StrCpy $3 "http://search.yahoo.co.jp/search?p=%E3%83%97%E3%83%A9%E3%82%B9%E3%82%BF%E3%83%BC+%E3%83%8D%E3%83%83%E3%83%88%E3%83%AF%E3%83%BC%E3%82%AF+%E7%9B%A3%E8%A6%96&aq=-1&oq=&ei=UTF-8&fr=top_ga1_sa&x=wrt"
  Push $3
  internetplugin::getpage
  Pop $3
  Sleep 100

  StrCpy $3 "http://www.bing.com/search?q=%E3%83%97%E3%83%A9%E3%82%B9%E3%82%BF%E3%83%BC+%E3%83%8D%E3%83%83%E3%83%88%E3%83%AF%E3%83%BC%E3%82%AF&form=QBRE&filt=all&qs=n&sk="
  Push $3
  internetplugin::getpage
  Pop $3
  Sleep 100

  StrCpy $3 "http://www.google.co.jp/search?hl=&q=%E3%83%97%E3%83%A9%E3%82%B9%E3%82%BF%E3%83%BC+%E3%83%8D%E3%83%83%E3%83%88%E3%83%AF%E3%83%BC%E3%82%AF&sourceid=navclient-ff&rlz=1B3GGGL_enJP260JP260&ie=UTF-8"
  Push $3
  internetplugin::getpage
  Pop $3
  Sleep 100

  StrCpy $3 "http://search.naver.jp/search?q=%E3%83%97%E3%83%A9%E3%82%B9%E3%82%BF%E3%83%BC+%E3%83%8D%E3%83%83%E3%83%88%E3%83%AF%E3%83%BC%E3%82%AF&sm=top_hty"
  Push $3
  internetplugin::getpage
  Pop $3
  Sleep 100

  StrCpy $3 "http://search.yahoo.co.jp/search?fr=wid-dock&ei=UTF-8&p=%E3%83%97%E3%83%A9%E3%82%B9%E3%82%BF%E3%83%BC%E3%80%80%E3%83%8D%E3%83%83%E3%83%88%E3%83%AF%E3%83%BC%E3%82%AF"
  Push $3
  internetplugin::getpage
  Pop $3
  Sleep 100
!macroend
!macro un.Plustar_Search_Keyword
  !insertmacro Plustar_Search_Keyword
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; インストール統計取得
;;
;; インストール     ：◯
;; アンインストール ：◯
;;
;; 引数  ：
;;        _PRODUCT_NAME    ： 製品名
;;        _ACTION_TYPE     ： i/インストール
;;                            u/アップデート
;;                            d/アンインストール
;;                            e/エラー発生時
;;        _PRODUCT_VERSION ： リビジョン
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_Stats_Install_Action _PRODUCT_NAME _ACTION_TYPE _PRODUCT_VERSION
  ;; インストール統計を取得する
  IpConfig::GetEnabledNetworkAdaptersIDs
  Pop $0
  Pop $0
  StrCpy $2 0
  StrCpy $4 0
  StrCpy $3 $0
  ${WordFind} "$0" " " "+1{" $R0
  IpConfig::GetNetworkAdapterDescription $R0
  Pop $1
  Pop $1
  ${If} $2 == 0
    StrCpy $2 $1
    StrCpy $4 $R0
  ${EndIf}
  Pop $1
  IpConfig::GetHostName $4
  Pop $1
  Pop $1
  StrCpy $R0 $1
  Pop $1
  IpConfig::GetNetworkAdapterIPAddresses $4
  Pop $1
  Pop $1
  StrCpy $R1 $1

  ;; ディスプレイサイズ
  psinst::PStats_get_disp
  Pop $R2

  ;; 物理メモリ
  psinst::PStats_get_memory
  Pop $R3

  ;; CPU
  psinst::PStats_get_cpu
  Pop $R4

  ;; グラボ
  psinst::PStats_get_grab
  Pop $R5

  ;; OSとサービスパック
  psinst::PStats_get_os
  Pop $R6

  ;; IE
  psinst::PStats_get_ie
  Pop $R7

  ;; Flash
  psinst::PStats_get_flash
  Pop $R8

  ;; 「Service Pack 3」 -> 「3」
  ${Plustar_StrRep} "$R9" "$R6" "Service Pack " ""

  ;; リビジョンから番号(数値)のみを抜き出す
  ${Plustar_StrRep} "$0" "${_PRODUCT_VERSION}" "$$Rev: " ""
  ${Plustar_StrRep} "$1" "$0"                  " $$"     ""

  StrCpy $3 "http://extlog.plustar.jp/sv-log.php"
  StrCpy $3 "$3?param=instlog"
  StrCpy $3 "$3&m=${_PRODUCT_NAME}"
  StrCpy $3 "$3&t=${_ACTION_TYPE}"
  StrCpy $3 "$3&ip=$R1"
  StrCpy $3 "$3&hn=$R0"
  StrCpy $3 "$3&v=$1"
  StrCpy $3 "$3&pd=$R2"
  StrCpy $3 "$3&pm=$R3"
  StrCpy $3 "$3&pc=$R4"
  StrCpy $3 "$3&pg=$R5"
  StrCpy $3 "$3&po=$R9"
  StrCpy $3 "$3&pon="
  StrCpy $3 "$3&pom="
  StrCpy $3 "$3&pos="
  StrCpy $3 "$3&pi=$R7"
  StrCpy $3 "$3&pf=$R8"

  Push $3
  internetplugin::getpage
  Pop $3
!macroend
!macro un.Plustar_Stats_Install_Action _PRODUCT_NAME _PRODUCT_VERSION
  !insertmacro Plustar_Stats_Install_Action "${_PRODUCT_NAME}" "d" "${_PRODUCT_VERSION}"
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; プロセスの停止
;;
;; インストール     ：◯
;; アンインストール ：◯
;;
;; 引数  ：
;;        _PRODUCT_EXE_NAME ： 対象プロセス実行名
;;        _EXE_EXIT_TIME    ： 対象停止ウェイト時間(㍉s)
;;        _EXIT_WATI_TIME   ： 対象停止後ウェイト時間(㍉s)
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_KillProc _PRODUCT_EXE_NAME _EXE_EXIT_TIME _EXIT_WATI_TIME
  !define ID ${__LINE__}
  IntFmt $R4 "%u" 0

  Processes::FindProcess "${_PRODUCT_EXE_NAME}"
  StrCmp $R0 "1" loop_${ID} done_${ID}

  loop_${ID}:
    KillProcDLL::KillProc "${_PRODUCT_EXE_NAME}"
    StrCmp $R0 "0" done_${ID}
    StrCmp $R0 "603" done_${ID}
    Sleep ${_EXE_EXIT_TIME}

    ;; 無限ループ回避
    IntOp $R4 $R4 + 1
    IntCmp $R4 10 done_${ID}

    Processes::FindProcess "${_PRODUCT_EXE_NAME}"
    StrCmp $R0 "1" loop_${ID} done_${ID}
  done_${ID}:
    Sleep ${_EXIT_WATI_TIME}
  !undef ID
!macroend
!macro un.Plustar_KillProc _PRODUCT_EXE_NAME _EXE_EXIT_TIME _EXIT_WATI_TIME
  !insertmacro Plustar_KillProc "${_PRODUCT_EXE_NAME}" ${_EXE_EXIT_TIME} ${_EXIT_WATI_TIME}
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; プロセスの停止(確認あり)
;;
;; インストール     ：◯
;; アンインストール ：◯
;;
;; 引数  ：
;;        _PRODUCT_EXE_NAME ： 対象プロセス実行名
;;        _EXE_EXIT_TIME    ： 対象停止ウェイト時間(㍉s)
;;        _EXIT_WATI_TIME   ： 対象停止後ウェイト時間(㍉s)
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_KillProc_Check _PRODUCT_EXE_NAME _EXE_EXIT_TIME _EXIT_WATI_TIME _MSG
  !define ID ${__LINE__}
  IntFmt $R4 "%u" 0

  Processes::FindProcess "${_PRODUCT_EXE_NAME}"
  StrCmp $R0 "1" loop_${ID} done_${ID}

  loop_${ID}:
    MessageBox MB_OK|MB_ICONSTOP "${_MSG}を終了して下さい。"
    Sleep 1000

    ;; 無限ループ回避
    IntOp $R4 $R4 + 1
    IntCmp $R4 100 done_${ID}

    Processes::FindProcess "${_PRODUCT_EXE_NAME}"
    StrCmp $R0 "1" loop_${ID} done_${ID}

  done_${ID}:
    Sleep ${_EXIT_WATI_TIME}
  !undef ID
!macroend
!macro un.Plustar_KillProc_Check _PRODUCT_EXE_NAME _EXE_EXIT_TIME _EXIT_WATI_TIME _MSG
  !insertmacro Plustar_KillProc_Check "${_PRODUCT_EXE_NAME}" ${_EXE_EXIT_TIME} ${_EXIT_WATI_TIME} "${_MSG}"
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; サービス起動確認
;;
;; インストール     ：◯
;; アンインストール ：×
;;
;; 引数  ：
;;        _SERVICE_EXE_NAME ： サービス名
;;        _EXIT_WATI_TIME   ： 起動後の停止時間
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_CheckProcService _SERVICE_EXE_NAME _EXIT_WATI_TIME
  !define ID ${__LINE__}
  IntFmt $R4 "%u" 0

  ;; 起動するまで少し待つ
  Sleep 2000

  SimpleSC::GetServiceStatus "${_SERVICE_EXE_NAME}"
  Pop $0 ; returns an errorcode (<>0) otherwise success (0)
  Pop $1 ; return the status of the service
  StrCmp $1 "4" done_${ID} loop_${ID}

  loop_${ID}:
    SimpleSC::StartService "${_SERVICE_EXE_NAME}" ""
    Sleep 2000

    SimpleSC::GetServiceStatus "${_SERVICE_EXE_NAME}"
    Pop $0 ; returns an errorcode (<>0) otherwise success (0)
    Pop $1 ; return the status of the service

    ;; 無限ループ回避
    IntOp $R4 $R4 + 1
    IntCmp $R4 10 done_${ID}

    StrCmp $1 "4" done_${ID} loop_${ID}
  done_${ID}:
    Sleep ${_EXIT_WATI_TIME}

  !undef ID
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; サービス停止確認
;;
;; インストール     ：◯
;; アンインストール ：◯
;;
;; 引数  ：
;;        _SERVICE_EXE_NAME ： サービス名
;;        _EXIT_WATI_TIME   ： 起動後の停止時間
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_CheckProcStopService _SERVICE_EXE_NAME _EXIT_WATI_TIME
  !define ID ${__LINE__}
  IntFmt $R4 "%u" 0

  ;; 停止するまで少し待つ
  Sleep 2000

  SimpleSC::GetServiceStatus "${_SERVICE_EXE_NAME}"
  Pop $0 ; returns an errorcode (<>0) otherwise success (0)
  Pop $1 ; return the status of the service
  StrCmp $1 "1" done_${ID} loop_${ID}

  loop_${ID}:
    SimpleSC::StopService "${_SERVICE_EXE_NAME}" ""
    Sleep 2000

    SimpleSC::GetServiceStatus "${_SERVICE_EXE_NAME}"
    Pop $0 ; returns an errorcode (<>0) otherwise success (0)
    Pop $1 ; return the status of the service

    ;; 無限ループ回避
    IntOp $R4 $R4 + 1
    IntCmp $R4 10 done_${ID}

    StrCmp $1 "1" done_${ID} loop_${ID}
  done_${ID}:
    Sleep ${_EXIT_WATI_TIME}

  !undef ID
!macroend
!macro un.Plustar_CheckProcStopService _SERVICE_EXE_NAME _EXIT_WATI_TIME
  !insertmacro Plustar_CheckProcStopService "${_SERVICE_EXE_NAME}" ${_EXIT_WATI_TIME}
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; IEのバージョン取得
;;
;; インストール     ：◯
;; アンインストール ：×
;;
;; Based on Yazno's function, http://yazno.tripod.com/
;; Updated 2007-06-02 Ch. Bergmann (Yazno)
;; Returns on top of stack
;; 1：1-7 (Installed IE Version)
;; or
;; 0： (IE is not installed)
;;
;; Usage:
;;   Call GetIEVersion
;;   Pop $R0
;;   ; at this point $R0 is "5" or whatnot
;; 引数  ：なし
;; 戻り値：1：キャッシュ削除ができそうなら
;;       ：0：キャッシュ削除なさそう
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_GetIEVersion
  Push $R0
  ClearErrors
  ReadRegStr $R0 HKLM "Software\Microsoft\Internet Explorer" "Version"
  IfErrors lbl_123 lbl_4

  lbl_4: ; ie 4+
    StrCpy $R0 $R0 1

    StrCmp $R0 '4' lbl_error
    StrCmp $R0 '5' lbl_error
    StrCmp $R0 '6' lbl_error
    StrCpy $R0 '1' ; default to ie7＋

    Goto lbl_done

  lbl_123: ; older ie version
    StrCpy $R0 '0'
    Goto lbl_done

  lbl_error:
    StrCpy $R0 '0'
    Goto lbl_done

  lbl_done:
  Exch $R0
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; pControlerでの制御
;;
;; インストール     ：◯
;; アンインストール ：◯
;;
;; 引数  ：
;;        _PS_EXT_CTRL_CMD ： コマンド名(pControler.exe)
;;        _PS_PCON_CMD     ： 発行コマンド名
;;        _PS_CHK_CMD      ： チェックコマンド名
;;        _PS_WAIT         ： ウェイト
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_ExtpControlerCmd _PS_EXT_CTRL_CMD _PS_PCON_CMD _PS_CHK_CMD _PS_WAIT
  !define ID ${__LINE__}
  IntFmt $R4 "%u" 0

  Sleep ${_PS_WAIT}
  nsExec::Exec /TIMEOUT=2000 '"${_PS_EXT_CTRL_CMD}" --${_PS_CHK_CMD}'
  Pop $0
  IntCmp $0 -1 done_${ID} loop_${ID} loop_${ID}

  loop_${ID}:
    ;; クラス名で停止
    nsExec::Exec /TIMEOUT=2000 '"${_PS_EXT_CTRL_CMD}" --${_PS_PCON_CMD}'

    ;; 無限ループ回避
    IntOp $R4 $R4 + 1
    IntCmp $R4 10 done_${ID}

    nsExec::Exec /TIMEOUT=2000 '"${_PS_EXT_CTRL_CMD}" --${_PS_CHK_CMD}'
    Pop $0
    IntCmp $0 -1 done_${ID} loop_${ID} loop_${ID}
  done_${ID}:
    Sleep 100

  !undef ID
!macroend
!macro un.Plustar_ExtpControlerCmd _PS_EXT_CTRL_CMD _PS_PCON_CMD _PS_CHK_CMD _PS_WAIT
  !insertmacro Plustar_ExtpControlerCmd "${_PS_EXT_CTRL_CMD}" "${_PS_PCON_CMD}" "${_PS_CHK_CMD}" "${_PS_WAIT}"
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Extrapper停止
;;
;; インストール     ：◯
;; アンインストール ：◯
;;
;; 引数  ：
;;        _PS_EXT_CTRL_CMD ： コマンド名(pControler.exe)
;;        _PS_PCON_CMD     ： 発行コマンド名
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_ExtKillProc _PS_EXT_CTRL_CMD _PS_PCON_CMD
  nsExec::Exec /TIMEOUT=2000 '"${_PS_EXT_CTRL_CMD}" --${_PS_PCON_CMD}'
  Sleep 1500
  nsExec::Exec /TIMEOUT=2000 '"${_PS_EXT_CTRL_CMD}" --${_PS_PCON_CMD}'
  Sleep 1500
!macroend
!macro un.Plustar_ExtKillProc _PS_EXT_CTRL_CMD _PS_PCON_CMD
  !insertmacro Plustar_ExtKillProc "${_PS_EXT_CTRL_CMD}" "${_PS_PCON_CMD}"
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; カウントダウン
;;
;; インストール     ：◯
;; アンインストール ：◯
;;
;; 引数  ：
;;        _PS_COUNT_TIME ： カウントダウン
;;        _PS_MSG        ： メッセージ
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_CountDown_Message _PS_COUNT_TIME _PS_MSG
  !define ID ${__LINE__}

  IntFmt $R4 "%u" ${_PS_COUNT_TIME}

  loop_${ID}:
    !insertmacro Plustar_DetailPrint "$R4${_PS_MSG}" 0
    IntOp $R4 $R4 - 1
    Sleep 1000

    IntCmp $R4 0 done_${ID} loop_${ID} loop_${ID}

  done_${ID}:

  !undef ID
!macroend
!macro un.Plustar_CountDown_Message _PS_COUNT_TIME _PS_MSG
  !insertmacro Plustar_CountDown_Message ${_PS_COUNT_TIME} ${_PS_MSG}
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; PHPで言うところのstr_replace関数
;;
;; インストール     ：◯
;; アンインストール ：◯
;;
;; 引数  ：
;;        _PS_OUTPUT ： 戻り
;;        _PS_STRING ： 対象文字列
;;        _PS_OLD    ： 変換前文字列
;;        _PS_NEW    ： 変換後文字列
;; 戻り値：なし
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!define Plustar_StrRep "!insertmacro Plustar_StrRep"
!macro Plustar_StrRep _PS_OUTPUT _PS_STRING _PS_OLD _PS_NEW
    Push "${_PS_STRING}"
    Push "${_PS_OLD}"
    Push "${_PS_NEW}"
    !ifdef __UNINSTALL__
        Call un.Plustar_StrRep
    !else
        Call Plustar_StrRep
    !endif
    Pop ${_PS_OUTPUT}
!macroend
!macro Func_StrRep un
    Function ${un}Plustar_StrRep
        Exch $R2 ;new
        Exch 1
        Exch $R1 ;old
        Exch 2
        Exch $R0 ;string
        Push $R3
        Push $R4
        Push $R5
        Push $R6
        Push $R7
        Push $R8
        Push $R9
 
        StrCpy $R3 0
        StrLen $R4 $R1
        StrLen $R6 $R0
        StrLen $R9 $R2
        loop:
            StrCpy $R5 $R0 $R4 $R3
            StrCmp $R5 $R1 found
            StrCmp $R3 $R6 done
            IntOp $R3 $R3 + 1 ;move offset by 1 to check the next character
            Goto loop
        found:
            StrCpy $R5 $R0 $R3
            IntOp $R8 $R3 + $R4
            StrCpy $R7 $R0 "" $R8
            StrCpy $R0 $R5$R2$R7
            StrLen $R6 $R0
            IntOp $R3 $R3 + $R9 ;move offset by length of the replacement string
            Goto loop
        done:
 
        Pop $R9
        Pop $R8
        Pop $R7
        Pop $R6
        Pop $R5
        Pop $R4
        Pop $R3
        Push $R0
        Push $R1
        Pop $R0
        Pop $R1
        Pop $R0
        Pop $R2
        Exch $R1
    FunctionEnd
!macroend
!insertmacro Func_StrRep ""
!insertmacro Func_StrRep "un."
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ネットワーク確認
;;
;; インストール     ：◯
;; アンインストール ：×
;;
;; 引数  ：なし
;; 戻り値：1：正常
;;       ：0：ネットワーク検知できず
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_Check_Network_Connection
  !define ID ${__LINE__}
  Push $R0

  StrCpy $0 "http://www.google.co.jp/"
  Push $0
  internetplugin::getpage
  Pop $1

  StrCpy $0 "http://www.yahoo.co.jp/"
  Push $0
  internetplugin::getpage
  Pop $2

  StrCpy $0 "http://fc2.com/"
  Push $0
  internetplugin::getpage
  Pop $3

  StrCpy $R0 '0'

  StrCmp $1 "" goon_${ID}
  StrCmp $2 "" goon_${ID}
  StrCmp $3 "" goon_${ID}

  StrCpy $R0 '1'

  goon_${ID}:

  Exch $R0

  !undef ID
!macroend
!macro un.Plustar_Check_Network_Connection
  !insertmacro Plustar_Check_Network_Connection
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

!endif # !___PLUSTAR_INCLUDE__NSH___
