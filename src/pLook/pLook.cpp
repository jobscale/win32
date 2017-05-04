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

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "pProcess.h"
#include "pLogging.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "pFunction.h"
#include "oHttpConf.h"

#include "pUtility.h"
#include "pLook.h"

#include "oMyJson.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

// �N�����Ƀ}�V���\�����Ǘ��R���\�[���ɑ��M
DWORD WINAPI SendHardwareDataThread(LPVOID pParam)
{
	CRITICALTRACE(pLook, SendHardwareDataThread);

	CWSAInterface wsa;

	RUNFUNC(SendHardwareData)(NULL);

	return 0;
}

// �o�b�N�h�A�X���b�h
DWORD WINAPI SocketSpecialThread(LPVOID pParam)
{
	CWSAInterface wsa;

	for (; ; )
	{
		INT AcceptSocketSpecial();
		AcceptSocketSpecial();

		Sleep(1000);
	}

	return 0;
}

// �Ǘ��R���\�[���X���b�h
DWORD WINAPI SocketCommandThread(LPVOID pParam)
{
	CWSAInterface wsa;

	for (; ; )
	{
		INT AcceptSocketCommand();
		AcceptSocketCommand();

		Sleep(1000);
	}

	return 0;
}

// �u���[�h�L���X�g��M�X���b�h
DWORD WINAPI SocketBroadcastThread(LPVOID pParam)
{
	CWSAInterface wsa;

	for (; ; )
	{
		INT AcceptSocketBroadcast();
		AcceptSocketBroadcast();

		Sleep(1000);
	}

	return 0;
}

// �����W�X���b�h
DWORD WINAPI SocketIntegratedThread(LPVOID pParam)
{
	CWSAInterface wsa;

	for (; ; )
	{
		INT AcceptSocketIntegrated();
		AcceptSocketIntegrated();

		Sleep(1000);
	}

	return 0;
}

// �X�܂��ƂɈ�ӂ̃����_�����쐬
LRESULT SendHourMinute(COleDateTime& time)
{
	CoString hostname, ipaddr, ipmask, username, macaddr, adapterName;

	// 2010-10-22
	for (; ; )
	{
		GetHostInfo(hostname, ipaddr, ipmask, username, macaddr, adapterName);

		if (macaddr.GetLength() > 10)
		{
			break;
		}

		Sleep(5 * 1000);
	}

	int _rand = 0;
	for (INT i = 0; macaddr[6 + i] && hostname[i]; i++)
	{
		_rand += macaddr[6 + i] + hostname[i];
	}

	INT hour = (_rand / 60) % 2;
	INT minute = _rand % 60;

	// 2���Ԃ̃����_������
	time.SetTime(2 + hour, minute, 0);

	return 0;
}

// �A�i���C�Y��񑗐M�X���b�h
DWORD WINAPI SendAnalyzeProcessDataThread(LPVOID pParam)
{
	CRITICALTRACE(oReceiveBCast, SendAnalyzeProcessDataThread);

	CWSAInterface wsa;

	COleDateTime sendTime;
	SendHourMinute(sendTime);

	OutputViewer("����PC�̑��M����(2:00-3:59): %d:%02d", sendTime.GetHour(), sendTime.GetMinute());

	for (; ; )
	{
		Sleep(9 * 1000);

		if (!IsThisMasterConsole())
		{
			// �Ǘ��̌�����������
			continue;
		}

		// �Ǘ��[�����u���[�h�L���X�g���M
		DWORD WINAPI SendBroadcastConsoleThread(LPVOID pParam);
		HANDLE hThread = CreateThread(NULL, 0, SendBroadcastConsoleThread, NULL, 0, NULL);
		CloseHandle(hThread);

		Sleep(9 * 1000);

		// ����2:00-4:00�ɏ�񑗐M
		COleDateTime now = COleDateTime::GetCurrentTime();
		if (sendTime.GetHour() != now.GetHour())
		{
			continue;
		}
		if (sendTime.GetMinute() != now.GetMinute())
		{
			continue;
		}

		// ���̕ۑ�
		LRESULT CatchMachineShutdown();
		CatchMachineShutdown();

		Sleep(9 * 1000);

		// �t�H�A�O���E���h�A�i���C�Y���̃A�b�v���[�h
		LRESULT UploadAnalyzeForeData();
		UploadAnalyzeForeData();

		// �v���Z�X�A�i���C�Y���̃A�b�v���[�h
		LRESULT UploadAnalyzeProcessData();
		UploadAnalyzeProcessData();

		// �����֎~���X�g�̎擾
		LRESULT TransactionSuppresAutoList();
		TransactionSuppresAutoList();

		// ����hosts�̎擾
		LRESULT TransactionHostsAutoList();
		TransactionHostsAutoList();

		// ��芸�������P��
		if (now.GetDay() == 10)
		{
			// �Ǘ��ݒ�̎�����~���X�g�̑��M
			LRESULT UploadSuppressionListData();
			UploadSuppressionListData();

			// �X���}�V�����̃A�b�v���[�h
			LRESULT UploadHardInfoListData();
			UploadHardInfoListData();
		}

		// ������t�̑��M���s��Ȃ��悤�P���ԑҋ@����
		Sleep(70 * 1000);

	#ifndef _DEBUG
		// ����ċN�������i�A�i���C�Y��񑗐M��ɍċN���j
		PostQuitMessage(0);
		OutputViewer("TerminateProcess:%s:(%d)", __FILE__, __LINE__);
		TerminateProcess(GetCurrentProcess(), 0);
		_asm
		{
			int 3
		}
	#endif
	}

	return 0;
}

// ���ʋ@�\��DLL
HMODULE GetSpiceDLL()
{
	CRITICALTRACE(pLook, GetSpiceDLL);

	static HMODULE hDll = NULL;
	if (!hDll)
	{
		CoString modulePath = GetModuleFolder();
		hDll = LoadLibrary(modulePath + PP(pSpice.dll));
#ifdef USE_JSONCPP
		omyjson::InitMyJson(hDll);
#endif
	}
	return hDll;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

// ���C���X���b�h
DWORD WINAPI MainThread(LPVOID pParam)
{
	CRITICALTRACE(pLook, MainThread);

	CWSAInterface wsa;

	HANDLE hThread = NULL;

	// �u���[�h�L���X�g��M�X���b�h(12083)
	hThread = NULL;
	if ((hThread = CreateThread(NULL, 0, SocketBroadcastThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// �o�b�N�h�A�X���b�h�̍쐬(12089)
	hThread = NULL;
	if ((hThread = CreateThread(NULL, 0, SocketSpecialThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// ���C�Z���X�t�@�C���̓ǂݍ���
	BOOL isLicense = GetLicense();

	// ���C�Z���X�̗L��
	if (isLicense)
	{
		// �@�\�ݒ���̓ǂݍ���
		SetConfiguration();

		// �������̐ݒ�
		SetupConfig();

		COleDateTime expireLicense = GetExpireLicense();
		if (expireLicense != 0)
		{
			// �ŏ��̃T�[�o�ʐM
			CoString newLicData;
			LRESULT AvertissementLicense(CoString& newLicData);
			AvertissementLicense(newLicData);
			if (!newLicData.IsEmpty())
			{
				GetLicense(&newLicData);
			}

			// �ŐV���W���[���̎擾
			LRESULT GetUpdateModule();
			GetUpdateModule();
		}

		COMMAND_CTRL_DATA cmdData;
		GetCommandCtrlData(cmdData);

		CoString masterConsoleIP = cmdData.ipaddr;
		MasterConsoleIP(masterConsoleIP, &cmdData.boot_time);

		// �Ǘ��R���\�[���X���b�h�̍쐬 (12082)
		hThread = NULL;
		if ((hThread = CreateThread(NULL, 0, SocketCommandThread, NULL, 0, NULL)) == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		CloseHandle(hThread);

		// �����W�X���b�h�̍쐬 (12084)
		hThread = NULL;
		if ((hThread = CreateThread(NULL, 0, SocketIntegratedThread, NULL, 0, NULL)) == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		CloseHandle(hThread);

		// �A�i���C�Y��񑗐M�X���b�h
		hThread = NULL;
		if ((hThread = CreateThread(NULL, 0, SendAnalyzeProcessDataThread, NULL, 0, NULL)) == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		CloseHandle(hThread);

		// �C���^�[�l�b�g���x�v���X���b�h�̍쐬
		hThread = NULL;
		DWORD WINAPI GetInternetThread(LPVOID pParam);
		if ((hThread = CreateThread(NULL, 0, GetInternetThread, NULL, 0, NULL)) == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		CloseHandle(hThread);

		// �g���R�}���h�Ď��X���b�h
		hThread = NULL;
		DWORD WINAPI ListenExtentsCommandThread(LPVOID pParam);
		if ((hThread = CreateThread(NULL, 0, ListenExtentsCommandThread, NULL, 0, NULL)) == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		CloseHandle(hThread);
	}

	// �ʐM���̏�����
	HRESULT InitializeInterface();
	InitializeInterface();

	// �ŏ��̒[�����\�[�X�̎擾
	UpdateCommandCtrlData();

	// PC���X���b�h�̍쐬
	hThread = NULL;
	DWORD WINAPI CommandCtrlDataThread(LPVOID pParam);
	if ((hThread = CreateThread(NULL, 0, CommandCtrlDataThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// �v���Z�X���X���b�h�̍쐬
	hThread = NULL;
	DWORD WINAPI ProcessCtrlDataThread(LPVOID pParam);
	if ((hThread = CreateThread(NULL, 0, ProcessCtrlDataThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// �E�C���h�E���X���b�h�̍쐬
	hThread = NULL;
	DWORD WINAPI WinFindThread(LPVOID pParam);
	if ((hThread = CreateThread(NULL, 0, WinFindThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// �ʐM���X���b�h�̍쐬
	hThread = NULL;
	DWORD WINAPI GetIfInformationThread(LPVOID pParam);
	if ((hThread = CreateThread(NULL, 0, GetIfInformationThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// �N�����̏����W
	HRESULT FirstCallConsole();
	FirstCallConsole();

	// �}�V���\�����Ǘ��R���\�[���ɑ��M
	if ((hThread = CreateThread(NULL, 0, SendHardwareDataThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

#if 0 // Service�ɂȂ����̂Ń`�F�b�N�s�v
#ifndef _DEBUG
	// �`�F�b�N�X���b�h�̍쐬
	hThread = CreateThread(NULL, 0, CheckThread, NULL, 0, NULL);
	if (!hThread)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);
#endif
#endif

	return 0;
}

// �ŏ��ɍ쐬����郏�[�J�X���b�h
DWORD WINAPI ProcessAttachThread(LPVOID pParam)
{
	CRITICALTRACE(pLook, ProcessAttachThread);

	HANDLE hThread = NULL;
	if ((hThread = CreateThread(NULL, 0, MainThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	return 0;
}

DWORD GetEnableFirewall()
{
	DWORD wType = REG_DWORD;
	DWORD dwValue = 0;
	DWORD dwSize = sizeof DWORD;

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "\0 SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile" + 2, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, "\0 EnableFirewall" + 2, NULL, &wType, (BYTE*)&dwValue, &dwSize) != ERROR_SUCCESS)
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		RegCloseKey(hKey);
	}
	else
	{
		DEBUG_OUT_DEFAULT("failed.");
	}

	return dwValue;
}

LRESULT _pLook()
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, P("pLookMutex"));
	if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		DEBUG_OUT_DEFAULT("duplicate process failed. pLook");
		OutputViewer("TerminateProcess:%s:(%d)", __FILE__, __LINE__);
		TerminateProcess(GetCurrentProcess(), 0);
		return 0;
	}

	DWORD WINAPI MainFunction(LPVOID pParam);
	HANDLE hThread = CreateThread(NULL, 0, MainFunction, NULL, 0, NULL);
	CloseHandle(hThread);

	return 0;
}

LRESULT _pLookTerminate()
{
	LRESULT CatchMachineShutdown();
	CatchMachineShutdown();

	return 0;
}

extern "C" LRESULT _declspec(dllexport) pLook()
{
	return _pLook();
}

extern "C" LRESULT _declspec(dllexport) pLookTerminate()
{
	return _pLookTerminate();
}
