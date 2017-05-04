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
// オブジェクト処理関数

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

// 管理パスワードの読込
LPCTSTR LoadConsolePassword()
{
	// 読込
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

// 管理パスワードの保存
void SaveConsolePassword(const CoString& resData)
{
	// 保存
	CoString base64data;
	base64_encode(resData, base64data, 2);
	MyWriteProfileString(P("Setting"), P("pass_console"), base64data);
}

// 管理パスワードの取得
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

// 管理パスワードの設定
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
// 機能設定情報関連の関数

// プロセスアナライズURLの取得
LPCTSTR GetAnaProURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[0];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// フォアグラウンドアナライズURLの取得
LPCTSTR GetAnaForeURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[1];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// 禁止プロセスURLの取得
LPCTSTR GetSuppresAutoListURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[2];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// hostsURLの取得
LPCTSTR GetHostsAutoListURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[3];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// 停止プロセスの通知URLの取得
LPCTSTR GetHistoryTerminateProcessURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[4];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// 管理設定停止情報URLの取得
LPCTSTR GetStopProcessListURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[14];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// ポップアップ通知URLの取得
LPCTSTR GetPopUPURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[10];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// ハードウェア情報送信URLの取得
LPCTSTR GetHardwareURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[11];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// 可動統計情報送信URLの取得
LPCTSTR GetStatsSendURL(LPCTSTR url/* = NULL*/)
{
	static CoString s_url = HTTP_PLUSTAR[12];
	if (url != NULL)
	{
		s_url = url;
	}
	return s_url;
}

// 設定ファイル名の取得
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

// 初期情報の設定
DWORD SetupConfig()
{
	DWORD first = MyGetProfileDWORD(P("Setting"), P("first"), 1);
	if (first)
	{
		if (!MyWriteProfileDWORD(P("Setting"), P("first"), 0))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}

		// P2PをデフォルトでONにする
		CoString data;
		GetSuppressionList(data);
		if (data.IsEmpty())
		{
			void SaveKillP2PType(CoString type);
			SaveKillP2PType("0"); // ON
			// 管理設定の自動停止リストの送信
			LRESULT UploadSuppressionListData();
			UploadSuppressionListData();
		}
	}

	return 0;
}

// 機能設定情報の読み込み
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
// ライセンス関連の関数

// ライセンスファイル名の取得
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

// アナライズ・プロセス・ライセンスファイル名の取得
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

// ライセンスの取得（起動初回）
BOOL GetLicenseCore(CoString* pNewLicData = NULL)
{
	BOOL isLicFile = TRUE;

	// Format "ユニーク文字列","有効期限","アップグレード管理文字列","ライセンス文字列","店舗名","クライアント数"
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

	// データの確認（ファイルの正当性を確認する必要がある）
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

// ライセンスの取得
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
// 管理コンソール関連の処理

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

	// 管理の取得
	if (sysTime == NULL)
	{
		masterConsoleIP = cmdData.masterConsoleIP;
		return 0;
	}

	// 管理の登録
	if (cmdData.masterConsoleIP.IsEmpty())
	{
		cmdData.masterLiveTime = COleDateTime::GetCurrentTime();
		cmdData.masterSysTime = *sysTime;
		cmdData.masterConsoleIP = masterConsoleIP;

		return 0;
	}

	// 管理の更新
	if (cmdData.masterConsoleIP == masterConsoleIP)
	{
		cmdData.masterLiveTime = COleDateTime::GetCurrentTime();

		return 0;
	}

	// 先に起動している管理を登録
	if (cmdData.masterSysTime > *sysTime)
	{
		cmdData.masterLiveTime = COleDateTime::GetCurrentTime();
		cmdData.masterSysTime = *sysTime;
		cmdData.masterConsoleIP = masterConsoleIP;

		return 0;
	}

	// 5分以上通信していない管理を破棄
	// 新しい管理を登録（管理端末の交換）
	COleDateTimeSpan fiveMinutes(0, 0, 5, 0);
	if (cmdData.masterLiveTime + fiveMinutes < COleDateTime::GetCurrentTime())
	{
		cmdData.masterLiveTime = COleDateTime::GetCurrentTime();
		cmdData.masterSysTime = *sysTime;
		cmdData.masterConsoleIP = masterConsoleIP;

		return 0;
	}

	// 管理の更新しない
	masterConsoleIP = cmdData.masterConsoleIP;

	return 0;
}

// 唯一の管理端末
BOOL IsThisMasterConsole()
{
	// ライセンスファイル
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

// 管理端末の判定
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

// 端末リソースの取得
COMMAND_CTRL_DATA& UpdateCommandCtrlData()
{
	CoCriticalSectionCtrl critical(CommandCtrlDataCritical());

	COMMAND_CTRL_DATA& data = GetCommandCtrlData();

	// CPU 利用率
	static CCpuUsageForNT s_CpuUsageForNT;
	data.cpu_usage = s_CpuUsageForNT.GetCpuUsageForNT();

	// メモリ情報
	GlobalMemoryStatus(&data.info_mem);

	// ドライブ情報
	CoString sysDir;
	GetSystemDirectory(sysDir.GetBuffer(_MAX_PATH), _MAX_PATH);
	sysDir.ReleaseBuffer();
	if (!GetDiskFreeSpaceEx(sysDir, (ULARGE_INTEGER*)&data.ulFreeBytesAvailable, (ULARGE_INTEGER*)&data.ulTotalNumberOfBytes, (ULARGE_INTEGER*)&data.ulTotalNumberOfFreeBytes))
	{
		DEBUG_OUT_DEFAULT("failed.");
	}

	// ネットワーク使用
	data.in_packet = IfInAverage();
	data.out_packet = IfOutAverage();

	// TCP Connections
	DWORD TCPConnections(CoConnectionsArray& connectionsm, DWORD flag = 0);
	TCPConnections(data.connections);

	return data;
}

// 端末リソースの取得
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

	// 起動プロセス
	data.nowProcess.RemoveAll();
	GetEnumProcess(data.nowProcess, ReportUseAction);

	return data;
}

// プロセス情報の更新
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
