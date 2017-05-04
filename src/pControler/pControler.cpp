///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

// pControler.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "../oCommon/oTools.h"
#include "../oCommon/oSocketTools.h"
#include "../oCommon/oBase64.h"

// チェックスレッド
DWORD WINAPI CheckThread(LPVOID pParam)
{
	CWSAInterface wsa;

	Sleep(9 * 1000);

	for (; ; )
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12087, P("LIFECHECK CHECKING."), &receive);
		int result = receive.Find(P("SO FUNNY LIFE.")) == -1 ? -1 : 9;
		if (result == -1)
		{
			CString folder = GetModuleFolder();
			RunProcess(folder + PP(pControler.exe), P("--runptools"), 1);
		}

#if 0 // pExtrapper.exe がサービスとして動作していない場合、起動する
		SendToTcpSocket(P("127.0.0.2"), 12084, P("LIFECHECK CHECKING."), &receive);
		int result = receive.Find(P("SO FUNNY LIFE.")) == -1 ? -1 : 9;
		if (result == -1)
		{
			CString folder = GetModuleFolder();
			RunProcess(folder + PP(pControler.exe), P("--runplook"), 1);
		}
#endif

		Sleep(900);
	}

	return 0;
}

DWORD Initialize(HINSTANCE hInstance)
{
#ifndef _DEBUG
	// チェックスレッドの作成
	HANDLE hThread = CreateThread(NULL, 0, CheckThread, NULL, 0, NULL);
	if (!hThread)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);
#endif

	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	if (strstr(lpCmdLine, P("--daemon")) != 0)
	{
		HANDLE hMutex = CreateMutex(NULL, TRUE, P("pControlerMutex"));
		if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
		{
			DEBUG_OUT_DEFAULT("duplicate process failed. pControler");
			return 9;
		}

		DWORD result = Initialize(hInstance);

		DWORD WINAPI SocketWaitingThread(LPVOID pParam);
		HANDLE hThread = CreateThread(NULL, 0, SocketWaitingThread, NULL, 0, NULL);
		CloseHandle(hThread);

		// メイン メッセージ ループ:
		MSG msg = { 0 };
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return 0;
	}

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CWSAInterface wsa;

	int result = 0;

	// サーバーにデータをアップロード
	if (strstr(lpCmdLine, P("--upanalyze")) != 0)
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12084, P("UPLOAD ANALYZE."), &receive);
		result = receive.Find(P("SO UPLOAD ANALYZE.")) == -1 ? -1 : 0;
	}
	// 管理端末に情報を送信
	else if (strstr(lpCmdLine, P("--sendanalyze")) != 0)
	{
		CString sendCmd;
		base64_encode(P("sendanalyze"), sendCmd);

		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12089, sendCmd, &receive);
		result = receive.Find(P("succeeded.")) == -1 ? -1 : 0;
	}
	else if (strstr(lpCmdLine, P("--start")) != 0)
	{
		CString path = GetModuleFolder();
		path += PP(pTools.exe);
		RunProcess(path, NULL, 1);
	}
	else if (strstr(lpCmdLine, P("--runplook")) != 0)
	{
		CString path = GetModuleFolder();
		path += PP(pExTrapper.exe);
		RunProcess(path, P("--normal"), 1);
	}
	else if (strstr(lpCmdLine, P("--runptools")) != 0)
	{
		CString path = GetModuleFolder();
		path += PP(pTools.exe);
		RunProcess(path, NULL, 1);
	}
	else if (strstr(lpCmdLine, P("--runpcontroler")) != 0)
	{
		CString path = GetModuleFolder();
		path += PP(pControler.exe);
		RunProcess(path, P("--daemon"), 1);
	}
	else if (strstr(lpCmdLine, P("--runchwin")) != 0)
	{
		CString path = GetModuleFolder();
		path += PP(pCheckWindow.exe);
		RunProcess(path, NULL, 1);
	}
	else if (strstr(lpCmdLine, P("--finish")) != 0)
	{
		result = 3;

		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12081, P("TERMINATE CHECKING."), &receive);
		result += receive.Find(P("SO FINALY.")) == -1 ? -1 : 0;

		receive.Empty();
		SendToTcpSocket(P("127.0.0.2"), 12087, P("TERMINATE CHECKING."), &receive);
		result += receive.Find(P("SO FINALY.")) == -1 ? -1 : 0;

		receive.Empty();
		SendToTcpSocket(P("127.0.0.2"), 12081, P("TERMINATE CHECKING."), &receive);
		result += receive.Find(P("SO FINALY.")) == -1 ? -1 : 0;

		receive.Empty();
		SendToTcpSocket(P("127.0.0.2"), 12086, P("TERMINATE CHECKING."), &receive);
		result += receive.Find(P("SO FINALY.")) == -1 ? -1 : 0;
	}
	else if (strstr(lpCmdLine, P("--termplook")) != 0)
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12084, P("TERMINATE CHECKING."), &receive);
		result = receive.Find(P("SO FINALY.")) == -1 ? -1 : 0;
	}
	else if (strstr(lpCmdLine, P("--termptools")) != 0)
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12087, P("TERMINATE CHECKING."), &receive);
		result = receive.Find(P("SO FINALY.")) == -1 ? -1 : 0;
	}
	else if (strstr(lpCmdLine, P("--termpcontroler")) != 0)
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12081, P("TERMINATE CHECKING."), &receive);
		result = receive.Find(P("SO FINALY.")) == -1 ? -1 : 0;
	}
	else if (strstr(lpCmdLine, P("--termchwin")) != 0)
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12086, P("TERMINATE CHECKING."), &receive);
		result = receive.Find(P("SO FINALY.")) == -1 ? -1 : 0;
	}
	else if (strstr(lpCmdLine, P("--islifeplook")) != 0)
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12084, P("LIFECHECK CHECKING."), &receive);
		result = receive.Find(P("SO FUNNY LIFE.")) == -1 ? -1 : 9;
	}
	else if (strstr(lpCmdLine, P("--islifeptools")) != 0)
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12087, P("LIFECHECK CHECKING."), &receive);
		result = receive.Find(P("SO FUNNY LIFE.")) == -1 ? -1 : 9;
	}
	else if (strstr(lpCmdLine, P("--islifepcontroler")) != 0)
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12081, P("LIFECHECK CHECKING."), &receive);
		result = receive.Find(P("SO FUNNY LIFE.")) == -1 ? -1 : 9;
	}
	else if (strstr(lpCmdLine, P("--islifechwin")) != 0)
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12086, P("LIFECHECK CHECKING."), &receive);
		result = receive.Find(P("SO FUNNY LIFE.")) == -1 ? -1 : 9;
	}
	else if (strstr(lpCmdLine, P("--stopctrlenable")) != 0)
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12085, P("STOP CONTROL. ENABLE"), &receive);
		result = receive.Find(P("SO FUNNY CONTROL.")) == -1 ? -1 : 0;
	}
	else if (strstr(lpCmdLine, P("--stopctrldisable")) != 0)
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12085, P("STOP CONTROL. DISABLE"), &receive);
		result = receive.Find(P("SO FUNNY CONTROL.")) == -1 ? -1 : 0;
	}

	PostQuitMessage(result);

	return result;
}

// 受信処理
DWORD WINAPI SocketTransThread(LPVOID pParam)
{
	CWSAInterface wsa;

	SOCKET sock = (SOCKET)pParam;

	CoAutoSocket autoSocket(sock);

	CString recvData;
	TransRecvTCP(sock, recvData);

	if (strncmp(recvData, P("TERMINATE CHECKING."), strlen(P("TERMINATE CHECKING."))) == 0)
	{
		send(sock, "SO FINALY.", (int)strlen("SO FINALY."), 0);

		PostQuitMessage(0);
		OutputViewer("TerminateProcess:%s:(%d)", __FILE__, __LINE__);
		TerminateProcess(GetCurrentProcess(), 0);
		_asm
		{
			int 3
		}
	}
	else if (strncmp(recvData, P("LIFECHECK CHECKING."), strlen(P("LIFECHECK CHECKING."))) == 0)
	{
		send(sock, P("SO FUNNY LIFE."), (int)strlen(P("SO FUNNY LIFE.")), 0);
	}
	else
	{
	}

	return 0;
}

// 受信待機
INT SocketWaiting()
{
	SOCKET sock0 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock0 == INVALID_SOCKET)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	WORD portNo = 12081;
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portNo);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock0, (sockaddr*)&addr, sizeof addr) != 0)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	if (listen(sock0, 5) != 0)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	for (; ; )
	{
		sockaddr_in client = { 0 };
		int len = sizeof sockaddr_in;
		SOCKET sock = accept(sock0, (sockaddr*)&client, &len);
		if (sock == INVALID_SOCKET)
		{
			DEBUG_OUT_DEFAULT("failed.");
			break;
		}

		// 待機スレッドの作成
		HANDLE hThread = NULL;
		if ((hThread = CreateThread(NULL, 0, SocketTransThread, (LPVOID)sock, 0, NULL)) == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		CloseHandle(hThread);
	}

	return 0;
}

// 待機スレッド
DWORD WINAPI SocketWaitingThread(LPVOID pParam)
{
	CWSAInterface wsa;

	for (; ; )
	{
		SocketWaiting();

		Sleep(1000);
	}

	return 0;
}
