///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#include "../oCommon/oTools.h"
#include "../oCommon/oSocketTools.h"
#include "../oCommon/oEthernet.h"
#include "../oCommon/oBase64.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "pProcess.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "oHttpConf.h"
#include "oCpuUsage.h"

#include "pUtility.h"
#include "pLook.h"

#include <lmcons.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// �I�u�W�F�N�g�����֐�

COMMAND_CTRL_DATA::COMMAND_CTRL_DATA()
	: cpu_usage(0)
	, ulFreeBytesAvailable(0)
	, ulTotalNumberOfBytes(0)
	, ulTotalNumberOfFreeBytes(0)
	, in_packet(0)
	, out_packet(0)
{
	COleDateTimeSpan tick(0, 0, 0, GetTickCount() / 1000);
	COleDateTime pcboot = COleDateTime::GetCurrentTime() - tick;
	pcboot.GetAsSystemTime(boot_time);

	ZeroMemory(&time_span, sizeof time_span);
	GetHostInfo(hostname, ipaddr, ipmask, username, macaddr, adapterName);
	ZeroMemory(&info_mem, sizeof info_mem);
	info_mem.dwLength = sizeof info_mem;
}

LRESULT COMMAND_CTRL_DATA::UpdateHostInfo()
{
	return GetHostInfo(hostname, ipaddr, ipmask, username, macaddr, adapterName, TRUE);
}

// �Ǘ��p�X���[�h�̓Ǎ�
LPCTSTR LoadConsolePassword()
{
	// �Ǎ�
	CoString base64data;
	MyGetProfileString(P("Setting"), P("pass_console"), base64data.GetBuffer(2048), 2048);
	base64data.ReleaseBuffer();
	static CoString resData;
	base64_decode(base64data, resData, 2);
	if (resData.IsEmpty())
	{
		resData = "plustar";
	}
	return resData;
}

// �Ǘ��p�X���[�h�̕ۑ�
void SaveConsolePassword(const CoString& resData)
{
	// �ۑ�
	CoString base64data;
	base64_encode(resData, base64data, 2);
	MyWriteProfileString(P("Setting"), P("pass_console"), base64data);
}

// �Ǘ��p�X���[�h�̎擾
DWORD GetConsolePassword(CoString& resData, const BOOL isSet/* = FALSE*/)
{
	static CoString defaPass = CoString("pLook:") + LoadConsolePassword();
	if (isSet)
	{
		defaPass = "pLook:";
		defaPass += resData;

		SaveConsolePassword(resData);
	}
	else
	{
		resData = defaPass;
	}

	return 0;
}

// �Ǘ��p�X���[�h�̐ݒ�
DWORD SetConsolePassword(const CoString& resData)
{
	GetConsolePassword(*(CoString*)&resData, TRUE);

	return 0;
}

DWORD GetScreenPassword(CoString& resData)
{
	CString work;
	MyGetProfileString(P("Setting"), P("screen_pass"), work.GetBuffer(MAX_PATH), MAX_PATH);
	work.ReleaseBuffer();

	if (work.IsEmpty())
	{
		resData = P("plustar");
	}
	else
	{
		base64_decode(work, resData);
	}

	return 0;
}

DWORD SetScreenPassword(const CoString& resData)
{
	CoString base64;
	base64_encode(resData, base64);

	MyWriteProfileString(P("Setting"), P("screen_pass"), base64);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// �@�\�ݒ���֘A�̊֐�

// �v���Z�X�A�i���C�YURL�̎擾
LPCTSTR GetAnaProURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[0];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// �t�H�A�O���E���h�A�i���C�YURL�̎擾
LPCTSTR GetAnaForeURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[1];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// �֎~�v���Z�XURL�̎擾
LPCTSTR GetSuppresAutoListURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[2];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// hostsURL�̎擾
LPCTSTR GetHostsAutoListURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[3];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// ��~�v���Z�X�̒ʒmURL�̎擾
LPCTSTR GetHistoryTerminateProcessURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[4];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// �Ǘ��ݒ��~���URL�̎擾
LPCTSTR GetStopProcessListURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[14];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// �|�b�v�A�b�v�ʒmURL�̎擾
LPCTSTR GetPopUPURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[10];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// �n�[�h�E�F�A��񑗐MURL�̎擾
LPCTSTR GetHardwareURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[11];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// �����v��񑗐MURL�̎擾
LPCTSTR GetStatsSendURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[12];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// �ݒ�t�@�C�����̎擾
LPCTSTR GetConfigFilePath()
{
	static CoString path;
	if (path.IsEmpty())
	{
		CoString modulePath = GetModuleFolder();
		path = modulePath + "pExTrapper.conf";
	}
	return path;
}

// �������̐ݒ�
DWORD SetupConfig()
{
	DWORD first = MyGetProfileDWORD(P("Setting"), P("first"), 1);
	if (first)
	{
		if (!MyWriteProfileDWORD(P("Setting"), P("first"), 0))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}

		// P2P���f�t�H���g��ON�ɂ���
		CoString data;
		GetSuppressionList(data);
		if (data.IsEmpty())
		{
			void SaveKillP2PType(CoString type);
			SaveKillP2PType("0"); // ON
			// �Ǘ��ݒ�̎�����~���X�g�̑��M
			LRESULT UploadSuppressionListData();
			UploadSuppressionListData();
		}
	}

	return 0;
}

// �@�\�ݒ���̓ǂݍ���
DWORD SetConfiguration()
{
	static CoString path = GetConfigFilePath();

	HANDLE hFile = INVALID_HANDLE_VALUE;
	if (NativityFile(path, 1, TRUE, &hFile) != 0)
	{
		return -1;
	}
	DWORD dwReadHigh = 0;
    DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	CoString targ;
	ReadFile(hFile, targ.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
	CloseHandle(hFile);

	INT pos = targ.FindNoCase(P("anafore_url:"));
	if (pos != -1)
	{
		CoString url = targ.AbstractSearchSubstance(pos);
		GetAnaForeURL(url);
	}

	pos = targ.FindNoCase(P("anapro_url:"));
	if (pos != -1)
	{
		CoString url = targ.AbstractSearchSubstance(pos);
		GetAnaProURL(url);
	}

	pos = targ.FindNoCase(P("suppres_auto_url:"));
	if (pos != -1)
	{
		CoString url = targ.AbstractSearchSubstance(pos);
		GetSuppresAutoListURL(url);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ���C�Z���X�֘A�̊֐�

// ���C�Z���X�t�@�C�����̎擾
LPCTSTR GetLicenseFilePath()
{
	static CoString path;
	if (path.IsEmpty())
	{
		CoString modulePath = GetModuleFolder();
		path = modulePath + "pExTrapper.lic";
	}
	return path;
}

// �A�i���C�Y�E�v���Z�X�E���C�Z���X�t�@�C�����̎擾
LPCTSTR GetAnaProLicenseFilePath()
{
	static CoString path;
	if (path.IsEmpty())
	{
		CoString modulePath = GetModuleFolder();
		path = modulePath + "pExAnaPro.lic";
	}
	return path;
}

// ���C�Z���X�̎擾�i�N������j
BOOL GetLicenseCore(CoString* pNewLicData = NULL)
{
	BOOL isLicFile = TRUE;

	// Format "���j�[�N������","�L������","�A�b�v�O���[�h�Ǘ�������","���C�Z���X������","�X�ܖ�","�N���C�A���g��"
	CoString licPath = GetLicenseFilePath();
	HANDLE hFile = INVALID_HANDLE_VALUE;
	if (NativityFile(licPath, 1, TRUE, &hFile) != 0)
	{
		isLicFile = FALSE;

		CoString licAnaPath = GetAnaProLicenseFilePath();
		hFile = INVALID_HANDLE_VALUE;
		if (NativityFile(licAnaPath, 1, TRUE, &hFile) != 0)
		{
			return FALSE;
		}
	}

	CoString binary;
	if (pNewLicData)
	{
		binary = *pNewLicData;
		DWORD dwSize = 0;
		WriteFile(hFile, (LPCTSTR)binary, binary.GetLength(), &dwSize, NULL);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
	}
	else
	{
		DWORD dwReadHigh = 0;
		DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
		ReadFile(hFile, binary.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
		CloseHandle(hFile);
	}

	CoString targ;
	base64_decode(binary, targ);
	INT pos = 0;
	CoString unique = targ.AbstractSearchSubstance(pos);
	CoString expireLic = targ.AbstractSearchSubstance(pos);
	CoString gradeText = targ.AbstractSearchSubstance(pos);
	CoString assessment_name = targ.AbstractSearchSubstance(pos);
	CoString shopName = targ.AbstractSearchSubstance(pos);
	CoString massLic = targ.AbstractSearchSubstance(pos);

	// �f�[�^�̊m�F�i�t�@�C���̐��������m�F����K�v������j
	if (expireLic.GetLength() != 8)
	{
		return FALSE;
	}
	int year = ((expireLic[0] - '0') * 1000) + ((expireLic[1] - '0') * 100) + ((expireLic[2] - '0') * 10) + (expireLic[3] - '0');
	int mon = ((expireLic[4] - '0') * 10) + (expireLic[5] - '0');
	int mday = ((expireLic[6] - '0') * 10) + (expireLic[7] - '0');

	COleDateTime expire_day(year, mon, mday, 0, 0, 0);

	GetShopUnique(unique);
	GetShopLicense(shopName);
	GetMassLicense(strtoul(massLic, NULL, 10));
	GetAsessNameLicense(assessment_name);
	GetTrapperUpGrade(strtoul(gradeText, NULL, 10));
	
	if (isLicFile)
	{
		GetExpireLicense(&expire_day);
	}

	return TRUE;
}

// ���C�Z���X�̎擾
BOOL GetLicense(CoString* pNewLicData/* = NULL*/)
{
	static BOOL isLicense = FALSE;
	if (!isLicense || pNewLicData)
	{
		isLicense = GetLicenseCore(pNewLicData);
	}
	return isLicense;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// �Ǘ��R���\�[���֘A�̏���

CoString& GetConsole()
{
	static CoString consoleIP;
	return consoleIP;
}

CCriticalSection& CommandCtrlDataCritical()
{
	static CCriticalSection section;
	return section;
}

COMMAND_CTRL_DATA& GetCommandCtrlData()
{
	static COMMAND_CTRL_DATA data;

	return data;
}

DWORD MasterConsoleIP(CoString& masterConsoleIP, const SYSTEMTIME* sysTime/* = NULL*/)
{
	CoCriticalSectionCtrl critical(CommandCtrlDataCritical());

	COMMAND_CTRL_DATA& cmdData = GetCommandCtrlData();

	// �Ǘ��̎擾
	if (sysTime == NULL)
	{
		masterConsoleIP = cmdData.masterConsoleIP;
		return 0;
	}

	// �Ǘ��̓o�^
	if (cmdData.masterConsoleIP.IsEmpty())
	{
		cmdData.masterLiveTime = COleDateTime::GetCurrentTime();
		cmdData.masterSysTime = *sysTime;
		cmdData.masterConsoleIP = masterConsoleIP;

		return 0;
	}

	// �Ǘ��̍X�V
	if (cmdData.masterConsoleIP == masterConsoleIP)
	{
		cmdData.masterLiveTime = COleDateTime::GetCurrentTime();

		return 0;
	}

	// ��ɋN�����Ă���Ǘ���o�^
	if (cmdData.masterSysTime > *sysTime)
	{
		cmdData.masterLiveTime = COleDateTime::GetCurrentTime();
		cmdData.masterSysTime = *sysTime;
		cmdData.masterConsoleIP = masterConsoleIP;

		return 0;
	}

	// 5���ȏ�ʐM���Ă��Ȃ��Ǘ���j��
	// �V�����Ǘ���o�^�i�Ǘ��[���̌����j
	COleDateTimeSpan fiveMinutes(0, 0, 5, 0);
	if (cmdData.masterLiveTime + fiveMinutes < COleDateTime::GetCurrentTime())
	{
		cmdData.masterLiveTime = COleDateTime::GetCurrentTime();
		cmdData.masterSysTime = *sysTime;
		cmdData.masterConsoleIP = masterConsoleIP;

		return 0;
	}

	// �Ǘ��̍X�V���Ȃ�
	masterConsoleIP = cmdData.masterConsoleIP;

	return 0;
}

// �B��̊Ǘ��[��
BOOL IsThisMasterConsole()
{
	// ���C�Z���X�t�@�C��
	if (GetMassLicense() == 0)
	{
		return FALSE;
	}

	CoCriticalSectionCtrl critical(CommandCtrlDataCritical());

	COMMAND_CTRL_DATA& cmdData = GetCommandCtrlData();

	if (cmdData.masterConsoleIP.Compare(cmdData.ipaddr) != 0)
	{
		if (cmdData.masterSysTime > cmdData.boot_time)
		{
			return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

// �Ǘ��[���̔���
BOOL IsConsole(LPCTSTR pIP)
{
	CoCriticalSectionCtrl critical(CommandCtrlDataCritical());

	COMMAND_CTRL_DATA& cmdData = GetCommandCtrlData();

	if (cmdData.masterConsoleIP.Compare(pIP) != 0)
	{
		return FALSE;
	}

	return TRUE;
}

DWORD GetCommandCtrlData(COMMAND_CTRL_DATA& ccdata, BOOL isUpdate/* = FALSE*/)
{
	CoCriticalSectionCtrl critical(CommandCtrlDataCritical());

	COMMAND_CTRL_DATA& data = GetCommandCtrlData();

	if (isUpdate || data.macaddr.IsEmpty())
	{
		data.UpdateHostInfo();
	}

	ccdata = data;

	return 0;
}

// �[�����\�[�X�̎擾
COMMAND_CTRL_DATA& UpdateCommandCtrlData()
{
	CoCriticalSectionCtrl critical(CommandCtrlDataCritical());

	COMMAND_CTRL_DATA& data = GetCommandCtrlData();

	// CPU ���p��
	static CCpuUsageForNT s_CpuUsageForNT;
	data.cpu_usage = s_CpuUsageForNT.GetCpuUsageForNT();

	// ���������
	GlobalMemoryStatus(&data.info_mem);

	// �h���C�u���
	CoString sysDir;
	GetSystemDirectory(sysDir.GetBuffer(_MAX_PATH), _MAX_PATH);
	sysDir.ReleaseBuffer();
	if (!GetDiskFreeSpaceEx(sysDir, (ULARGE_INTEGER*)&data.ulFreeBytesAvailable, (ULARGE_INTEGER*)&data.ulTotalNumberOfBytes, (ULARGE_INTEGER*)&data.ulTotalNumberOfFreeBytes))
	{
		DEBUG_OUT_DEFAULT("failed.");
	}

	// �l�b�g���[�N�g�p
	data.in_packet = IfInAverage();
	data.out_packet = IfOutAverage();

	// TCP Connections
	DWORD TCPConnections(CoConnectionsArray& connectionsm, DWORD flag = 0);
	TCPConnections(data.connections);

	return data;
}

// �[�����\�[�X�̎擾
DWORD WINAPI CommandCtrlDataThread(LPVOID pParam)
{
	CWSAInterface wsa;

	for (; ; )
	{
		UpdateCommandCtrlData();

		Sleep(7200);
	}

	return 0;
}

COMMAND_CTRL_DATA& UpdateProcessCtrlData()
{
	CoCriticalSectionCtrl critical(CommandCtrlDataCritical());

	COMMAND_CTRL_DATA& data = GetCommandCtrlData();

	// �N���v���Z�X
	data.nowProcess.RemoveAll();
	GetEnumProcess(data.nowProcess, ReportUseAction);

	return data;
}

// �v���Z�X���̍X�V
DWORD WINAPI ProcessCtrlDataThread(LPVOID pParam)
{
	CWSAInterface wsa;

	for (; ; )
	{
		UpdateProcessCtrlData();

		Sleep(9200);
	}

	return 0;
}
