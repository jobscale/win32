///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__P_CONSOLE_H__)
#define __P_CONSOLE_H__

#include <atlcomtime.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// 管理画面の設定
BOOL GetLicense(CoString* pNewLicData = NULL);
DWORD SetupConfig();
DWORD SetConfiguration();

// 管理画面のパスワード
DWORD GetConsolePassword(CoString& resData, const BOOL isSet = FALSE);
DWORD SetConsolePassword(const CoString& resData);
DWORD GetScreenPassword(CoString& resData);
DWORD SetScreenPassword(const CoString& resData);

// 機能設定情報関連の関数
LPCTSTR GetAnaProURL(LPCTSTR url = NULL); // プロセスアナライズURLの取得
LPCTSTR GetAnaForeURL(LPCTSTR url = NULL); // フォアグラウンドアナライズURLの取得
LPCTSTR GetSuppresAutoListURL(LPCTSTR url = NULL); // 禁止プロセスURLの取得
LPCTSTR GetHostsAutoListURL(LPCTSTR url = NULL); // hostsURLの取得
LPCTSTR GetHistoryTerminateProcessURL(LPCTSTR url = NULL); // 停止プロセスの通知URLの取得
LPCTSTR GetStopProcessListURL(LPCTSTR url = NULL); // 管理設定の禁止情報URLの取得
LPCTSTR GetPopUPURL(LPCTSTR url = NULL); // ポップアップ通知URLの取得
LPCTSTR GetHardwareURL(LPCTSTR url = NULL); // ハードウェア情報送信URLの取得
LPCTSTR GetStatsSendURL(LPCTSTR url = NULL);// 可動統計情報送信URLの取得

///////////////////////////////////////////////////////////////////////////////////////////////////
// 管理コンソール情報

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
