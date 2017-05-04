; ---------------------
; Plustar_Include.nsh
; ---------------------
;
!ifndef ___PLUSTAR_INCLUDE__NSH___
!define ___PLUSTAR_INCLUDE__NSH___

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; �v�����g���O�\��
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F��
;;
;; ����  �F
;;        _PRINT_MSG       �F���b�Z�[�W
;;        _SLEEP_AFTER_MSG �F���b�Z�[�W�\����E�F�C�g
;; �߂�l�F�Ȃ�
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
;; �Ď��n�v���Z�X�̒�~
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F��
;;
;; ����  �F�Ȃ�
;; �߂�l�F�Ȃ�
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
;; �C���X�g�[�����Ɍ������[�h�𔭐�������
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F��
;;
;; ����  �F�Ȃ�
;; �߂�l�F�Ȃ�
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_Search_Keyword
  ;; �������c�������[�h�𔭐�������
  ;; �u�v���X�^�[ �l�b�g���[�N�v

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
;; �C���X�g�[�����v�擾
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F��
;;
;; ����  �F
;;        _PRODUCT_NAME    �F ���i��
;;        _ACTION_TYPE     �F i/�C���X�g�[��
;;                            u/�A�b�v�f�[�g
;;                            d/�A���C���X�g�[��
;;                            e/�G���[������
;;        _PRODUCT_VERSION �F ���r�W����
;; �߂�l�F�Ȃ�
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_Stats_Install_Action _PRODUCT_NAME _ACTION_TYPE _PRODUCT_VERSION
  ;; �C���X�g�[�����v���擾����
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

  ;; �f�B�X�v���C�T�C�Y
  psinst::PStats_get_disp
  Pop $R2

  ;; ����������
  psinst::PStats_get_memory
  Pop $R3

  ;; CPU
  psinst::PStats_get_cpu
  Pop $R4

  ;; �O���{
  psinst::PStats_get_grab
  Pop $R5

  ;; OS�ƃT�[�r�X�p�b�N
  psinst::PStats_get_os
  Pop $R6

  ;; IE
  psinst::PStats_get_ie
  Pop $R7

  ;; Flash
  psinst::PStats_get_flash
  Pop $R8

  ;; �uService Pack 3�v -> �u3�v
  ${Plustar_StrRep} "$R9" "$R6" "Service Pack " ""

  ;; ���r�W��������ԍ�(���l)�݂̂𔲂��o��
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
;; �v���Z�X�̒�~
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F��
;;
;; ����  �F
;;        _PRODUCT_EXE_NAME �F �Ώۃv���Z�X���s��
;;        _EXE_EXIT_TIME    �F �Ώے�~�E�F�C�g����(�_s)
;;        _EXIT_WATI_TIME   �F �Ώے�~��E�F�C�g����(�_s)
;; �߂�l�F�Ȃ�
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

    ;; �������[�v���
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
;; �v���Z�X�̒�~(�m�F����)
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F��
;;
;; ����  �F
;;        _PRODUCT_EXE_NAME �F �Ώۃv���Z�X���s��
;;        _EXE_EXIT_TIME    �F �Ώے�~�E�F�C�g����(�_s)
;;        _EXIT_WATI_TIME   �F �Ώے�~��E�F�C�g����(�_s)
;; �߂�l�F�Ȃ�
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_KillProc_Check _PRODUCT_EXE_NAME _EXE_EXIT_TIME _EXIT_WATI_TIME _MSG
  !define ID ${__LINE__}
  IntFmt $R4 "%u" 0

  Processes::FindProcess "${_PRODUCT_EXE_NAME}"
  StrCmp $R0 "1" loop_${ID} done_${ID}

  loop_${ID}:
    MessageBox MB_OK|MB_ICONSTOP "${_MSG}���I�����ĉ������B"
    Sleep 1000

    ;; �������[�v���
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
;; �T�[�r�X�N���m�F
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F�~
;;
;; ����  �F
;;        _SERVICE_EXE_NAME �F �T�[�r�X��
;;        _EXIT_WATI_TIME   �F �N����̒�~����
;; �߂�l�F�Ȃ�
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_CheckProcService _SERVICE_EXE_NAME _EXIT_WATI_TIME
  !define ID ${__LINE__}
  IntFmt $R4 "%u" 0

  ;; �N������܂ŏ����҂�
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

    ;; �������[�v���
    IntOp $R4 $R4 + 1
    IntCmp $R4 10 done_${ID}

    StrCmp $1 "4" done_${ID} loop_${ID}
  done_${ID}:
    Sleep ${_EXIT_WATI_TIME}

  !undef ID
!macroend
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; �T�[�r�X��~�m�F
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F��
;;
;; ����  �F
;;        _SERVICE_EXE_NAME �F �T�[�r�X��
;;        _EXIT_WATI_TIME   �F �N����̒�~����
;; �߂�l�F�Ȃ�
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_CheckProcStopService _SERVICE_EXE_NAME _EXIT_WATI_TIME
  !define ID ${__LINE__}
  IntFmt $R4 "%u" 0

  ;; ��~����܂ŏ����҂�
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

    ;; �������[�v���
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
;; IE�̃o�[�W�����擾
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F�~
;;
;; Based on Yazno's function, http://yazno.tripod.com/
;; Updated 2007-06-02 Ch. Bergmann (Yazno)
;; Returns on top of stack
;; 1�F1-7 (Installed IE Version)
;; or
;; 0�F (IE is not installed)
;;
;; Usage:
;;   Call GetIEVersion
;;   Pop $R0
;;   ; at this point $R0 is "5" or whatnot
;; ����  �F�Ȃ�
;; �߂�l�F1�F�L���b�V���폜���ł������Ȃ�
;;       �F0�F�L���b�V���폜�Ȃ�����
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
    StrCpy $R0 '1' ; default to ie7�{

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
;; pControler�ł̐���
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F��
;;
;; ����  �F
;;        _PS_EXT_CTRL_CMD �F �R�}���h��(pControler.exe)
;;        _PS_PCON_CMD     �F ���s�R�}���h��
;;        _PS_CHK_CMD      �F �`�F�b�N�R�}���h��
;;        _PS_WAIT         �F �E�F�C�g
;; �߂�l�F�Ȃ�
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!macro Plustar_ExtpControlerCmd _PS_EXT_CTRL_CMD _PS_PCON_CMD _PS_CHK_CMD _PS_WAIT
  !define ID ${__LINE__}
  IntFmt $R4 "%u" 0

  Sleep ${_PS_WAIT}
  nsExec::Exec /TIMEOUT=2000 '"${_PS_EXT_CTRL_CMD}" --${_PS_CHK_CMD}'
  Pop $0
  IntCmp $0 -1 done_${ID} loop_${ID} loop_${ID}

  loop_${ID}:
    ;; �N���X���Œ�~
    nsExec::Exec /TIMEOUT=2000 '"${_PS_EXT_CTRL_CMD}" --${_PS_PCON_CMD}'

    ;; �������[�v���
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
;; Extrapper��~
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F��
;;
;; ����  �F
;;        _PS_EXT_CTRL_CMD �F �R�}���h��(pControler.exe)
;;        _PS_PCON_CMD     �F ���s�R�}���h��
;; �߂�l�F�Ȃ�
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
;; �J�E���g�_�E��
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F��
;;
;; ����  �F
;;        _PS_COUNT_TIME �F �J�E���g�_�E��
;;        _PS_MSG        �F ���b�Z�[�W
;; �߂�l�F�Ȃ�
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
;; PHP�Ō����Ƃ����str_replace�֐�
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F��
;;
;; ����  �F
;;        _PS_OUTPUT �F �߂�
;;        _PS_STRING �F �Ώە�����
;;        _PS_OLD    �F �ϊ��O������
;;        _PS_NEW    �F �ϊ��㕶����
;; �߂�l�F�Ȃ�
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
;; �l�b�g���[�N�m�F
;;
;; �C���X�g�[��     �F��
;; �A���C���X�g�[�� �F�~
;;
;; ����  �F�Ȃ�
;; �߂�l�F1�F����
;;       �F0�F�l�b�g���[�N���m�ł���
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
