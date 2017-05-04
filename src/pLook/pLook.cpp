///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
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

// 起動時にマシン構成を管理コンソールに送信
DWORD WINAPI SendHardwareDataThread(LPVOID pParam)
{
	CRITICALTRACE(pLook, SendHardwareDataThread);

	CWSAInterface wsa;

	RUNFUNC(SendHardwareData)(NULL);

	return 0;
}

// バックドアスレッド
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

// 管理コンソールスレッド
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

// ブロードキャスト受信スレッド
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

// 情報収集スレッド
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

// 店舗ごとに一意のランダムを作成
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

	// 2時間のランダム時間
	time.SetTime(2 + hour, minute, 0);

	return 0;
}

// アナライズ情報送信スレッド
DWORD WINAPI SendAnalyzeProcessDataThread(LPVOID pParam)
{
	CRITICALTRACE(oReceiveBCast, SendAnalyzeProcessDataThread);

	CWSAInterface wsa;

	COleDateTime sendTime;
	SendHourMinute(sendTime);

	OutputViewer("このPCの送信時刻(2:00-3:59): %d:%02d", sendTime.GetHour(), sendTime.GetMinute());

	for (; ; )
	{
		Sleep(9 * 1000);

		if (!IsThisMasterConsole())
		{
			// 管理の権限を失った
			continue;
		}

		// 管理端末をブロードキャスト送信
		DWORD WINAPI SendBroadcastConsoleThread(LPVOID pParam);
		HANDLE hThread = CreateThread(NULL, 0, SendBroadcastConsoleThread, NULL, 0, NULL);
		CloseHandle(hThread);

		Sleep(9 * 1000);

		// 毎日2:00-4:00に情報送信
		COleDateTime now = COleDateTime::GetCurrentTime();
		if (sendTime.GetHour() != now.GetHour())
		{
			continue;
		}
		if (sendTime.GetMinute() != now.GetMinute())
		{
			continue;
		}

		// 情報の保存
		LRESULT CatchMachineShutdown();
		CatchMachineShutdown();

		Sleep(9 * 1000);

		// フォアグラウンドアナライズ情報のアップロード
		LRESULT UploadAnalyzeForeData();
		UploadAnalyzeForeData();

		// プロセスアナライズ情報のアップロード
		LRESULT UploadAnalyzeProcessData();
		UploadAnalyzeProcessData();

		// 自動禁止リストの取得
		LRESULT TransactionSuppresAutoList();
		TransactionSuppresAutoList();

		// 自動hostsの取得
		LRESULT TransactionHostsAutoList();
		TransactionHostsAutoList();

		// 取り敢えず月１回
		if (now.GetDay() == 10)
		{
			// 管理設定の自動停止リストの送信
			LRESULT UploadSuppressionListData();
			UploadSuppressionListData();

			// 店内マシン情報のアップロード
			LRESULT UploadHardInfoListData();
			UploadHardInfoListData();
		}

		// 同一日付の送信を行わないよう１分間待機する
		Sleep(70 * 1000);

	#ifndef _DEBUG
		// 定期再起動処理（アナライズ情報送信後に再起動）
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

// 共通機能のDLL
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

// メインスレッド
DWORD WINAPI MainThread(LPVOID pParam)
{
	CRITICALTRACE(pLook, MainThread);

	CWSAInterface wsa;

	HANDLE hThread = NULL;

	// ブロードキャスト受信スレッド(12083)
	hThread = NULL;
	if ((hThread = CreateThread(NULL, 0, SocketBroadcastThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// バックドアスレッドの作成(12089)
	hThread = NULL;
	if ((hThread = CreateThread(NULL, 0, SocketSpecialThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// ライセンスファイルの読み込み
	BOOL isLicense = GetLicense();

	// ライセンスの有無
	if (isLicense)
	{
		// 機能設定情報の読み込み
		SetConfiguration();

		// 初期情報の設定
		SetupConfig();

		COleDateTime expireLicense = GetExpireLicense();
		if (expireLicense != 0)
		{
			// 最初のサーバ通信
			CoString newLicData;
			LRESULT AvertissementLicense(CoString& newLicData);
			AvertissementLicense(newLicData);
			if (!newLicData.IsEmpty())
			{
				GetLicense(&newLicData);
			}

			// 最新モジュールの取得
			LRESULT GetUpdateModule();
			GetUpdateModule();
		}

		COMMAND_CTRL_DATA cmdData;
		GetCommandCtrlData(cmdData);

		CoString masterConsoleIP = cmdData.ipaddr;
		MasterConsoleIP(masterConsoleIP, &cmdData.boot_time);

		// 管理コンソールスレッドの作成 (12082)
		hThread = NULL;
		if ((hThread = CreateThread(NULL, 0, SocketCommandThread, NULL, 0, NULL)) == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		CloseHandle(hThread);

		// 情報収集スレッドの作成 (12084)
		hThread = NULL;
		if ((hThread = CreateThread(NULL, 0, SocketIntegratedThread, NULL, 0, NULL)) == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		CloseHandle(hThread);

		// アナライズ情報送信スレッド
		hThread = NULL;
		if ((hThread = CreateThread(NULL, 0, SendAnalyzeProcessDataThread, NULL, 0, NULL)) == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		CloseHandle(hThread);

		// インターネット速度計測スレッドの作成
		hThread = NULL;
		DWORD WINAPI GetInternetThread(LPVOID pParam);
		if ((hThread = CreateThread(NULL, 0, GetInternetThread, NULL, 0, NULL)) == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		CloseHandle(hThread);

		// 拡張コマンド監視スレッド
		hThread = NULL;
		DWORD WINAPI ListenExtentsCommandThread(LPVOID pParam);
		if ((hThread = CreateThread(NULL, 0, ListenExtentsCommandThread, NULL, 0, NULL)) == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		CloseHandle(hThread);
	}

	// 通信情報の初期化
	HRESULT InitializeInterface();
	InitializeInterface();

	// 最初の端末リソースの取得
	UpdateCommandCtrlData();

	// PC情報スレッドの作成
	hThread = NULL;
	DWORD WINAPI CommandCtrlDataThread(LPVOID pParam);
	if ((hThread = CreateThread(NULL, 0, CommandCtrlDataThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// プロセス情報スレッドの作成
	hThread = NULL;
	DWORD WINAPI ProcessCtrlDataThread(LPVOID pParam);
	if ((hThread = CreateThread(NULL, 0, ProcessCtrlDataThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// ウインドウ情報スレッドの作成
	hThread = NULL;
	DWORD WINAPI WinFindThread(LPVOID pParam);
	if ((hThread = CreateThread(NULL, 0, WinFindThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// 通信情報スレッドの作成
	hThread = NULL;
	DWORD WINAPI GetIfInformationThread(LPVOID pParam);
	if ((hThread = CreateThread(NULL, 0, GetIfInformationThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	// 起動時の情報収集
	HRESULT FirstCallConsole();
	FirstCallConsole();

	// マシン構成を管理コンソールに送信
	if ((hThread = CreateThread(NULL, 0, SendHardwareDataThread, NULL, 0, NULL)) == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

#if 0 // Serviceになったのでチェック不要
#ifndef _DEBUG
	// チェックスレッドの作成
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

// 最初に作成されるワーカスレッド
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
