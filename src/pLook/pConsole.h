///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__P_CONSOLE_H__)
#define __P_CONSOLE_H__

#include <atlcomtime.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// �Ǘ���ʂ̐ݒ�
BOOL GetLicense(CoString* pNewLicData = NULL);
DWORD SetupConfig();
DWORD SetConfiguration();

// �Ǘ���ʂ̃p�X���[�h
DWORD GetConsolePassword(CoString& resData, const BOOL isSet = FALSE);
DWORD SetConsolePassword(const CoString& resData);
DWORD GetScreenPassword(CoString& resData);
DWORD SetScreenPassword(const CoString& resData);

// �@�\�ݒ���֘A�̊֐�
LPCTSTR GetAnaProURL(LPCTSTR url = NULL); // �v���Z�X�A�i���C�YURL�̎擾
LPCTSTR GetAnaForeURL(LPCTSTR url = NULL); // �t�H�A�O���E���h�A�i���C�YURL�̎擾
LPCTSTR GetSuppresAutoListURL(LPCTSTR url = NULL); // �֎~�v���Z�XURL�̎擾
LPCTSTR GetHostsAutoListURL(LPCTSTR url = NULL); // hostsURL�̎擾
LPCTSTR GetHistoryTerminateProcessURL(LPCTSTR url = NULL); // ��~�v���Z�X�̒ʒmURL�̎擾
LPCTSTR GetStopProcessListURL(LPCTSTR url = NULL); // �Ǘ��ݒ�̋֎~���URL�̎擾
LPCTSTR GetPopUPURL(LPCTSTR url = NULL); // �|�b�v�A�b�v�ʒmURL�̎擾
LPCTSTR GetHardwareURL(LPCTSTR url = NULL); // �n�[�h�E�F�A��񑗐MURL�̎擾
LPCTSTR GetStatsSendURL(LPCTSTR url = NULL);// �����v��񑗐MURL�̎擾

///////////////////////////////////////////////////////////////////////////////////////////////////
// �Ǘ��R���\�[�����

struct PACKET_DATA
{
	SYSTEMTIME data_time;
	DWORD packet_byte;
};

struct COMMAND_CTRL_DATA
{
	CoString hostname;
	CoString ipaddr;
	CoString ipmask;
	CoString username;
	CoString macaddr;
	CoString adapterName;

	SYSTEMTIME boot_time;
	SYSTEMTIME time_span;
	CoString masterConsoleIP;
	COleDateTime masterSysTime;
	COleDateTime masterLiveTime;
	DWORD cpu_usage;
	CoProcessArray nowProcess;
	MEMORYSTATUS info_mem;
	UINT64 ulFreeBytesAvailable;
	UINT64 ulTotalNumberOfBytes;
	UINT64 ulTotalNumberOfFreeBytes;
	DWORD in_packet;
	DWORD out_packet;
	CoConnectionsArray connections;

	COMMAND_CTRL_DATA();
	LRESULT UpdateHostInfo();
};

DWORD GetCommandCtrlData(COMMAND_CTRL_DATA& ccdata, BOOL isUpdate = FALSE);
COMMAND_CTRL_DATA& UpdateCommandCtrlData();

DWORD MasterConsoleIP(CoString& masterConsoleIP, const SYSTEMTIME* sysTime = NULL);
BOOL IsThisMasterConsole();
BOOL IsConsole(LPCTSTR pIP);

DWORD LoadConsoleData();
DWORD SaveConsoleData();


#endif // __P_CONSOLE_H__
