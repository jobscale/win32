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

HMODULE g_hModule = NULL;

LRESULT StartLooking()
{
	g_hModule = LoadLibrary("pLook");

	typedef LRESULT (WINAPI* pLookProc)();
	pLookProc pLook = (pLookProc)GetProcAddress(g_hModule, "pLook");
	if (pLook)
	{
		return pLook();
	}

	return 0;
}

LRESULT EndLooking()
{
	typedef LRESULT (WINAPI* pLookTerminateProc)();
	pLookTerminateProc pLookTerminate = (pLookTerminateProc)GetProcAddress(g_hModule, "pLookTerminate");
	if (pLookTerminate)
	{
		return pLookTerminate();
	}

	return 0;
}

#define SERVICE_NAME (TEXT("MSLOOK")) // Monitoring Service pLook

struct SERVICE_CTRL
{
	SERVICE_STATUS_HANDLE h;
	SERVICE_STATUS s;
};

static SERVICE_CTRL g_ctrl = { 0 };

DWORD WINAPI HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	// Initialize Variables for Service Control
	g_ctrl.s.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ctrl.s.dwWin32ExitCode = NO_ERROR;
	g_ctrl.s.dwServiceSpecificExitCode = 0;
	g_ctrl.s.dwCheckPoint = 1;
	g_ctrl.s.dwWaitHint = 3000;
	g_ctrl.s.dwControlsAccepted = SERVICE_ACCEPT_POWEREVENT;

	BOOL bRet = FALSE;

	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP:

		// Set STOP_PENDING status.
		g_ctrl.s.dwCurrentState = SERVICE_STOP_PENDING;
		bRet = SetServiceStatus(g_ctrl.h, &g_ctrl.s);
		if (!bRet)
		{
			break;
		}

		// SERVICE SPECIFIC STOPPING CODE HERE.
		// ...
		// ...

		EndLooking();

		// Set STOPPED status.
		g_ctrl.s.dwCheckPoint = 0;
		g_ctrl.s.dwWaitHint = 0;
		g_ctrl.s.dwCurrentState = SERVICE_STOPPED;
		bRet = SetServiceStatus(g_ctrl.h, &g_ctrl.s);
		if (!bRet)
		{
			break;
		}

		return NO_ERROR;

	case SERVICE_CONTROL_POWEREVENT:

		OutputViewer("POWEREVENT: 0x%08X, 0x%08X", dwEventType, lpEventData);

		return ERROR_CALL_NOT_IMPLEMENTED;

	default:

		OutputViewer("OtherEvent: 0x%08X, 0x%08X", dwEventType, lpEventData);

		return ERROR_CALL_NOT_IMPLEMENTED;
	}

	return NO_ERROR;
}

void WINAPI ServiceMain(DWORD dwArgc, PTSTR* pszArgv)
{
	// Register Service Control Handler
	g_ctrl.h = RegisterServiceCtrlHandlerEx(SERVICE_NAME, HandlerEx, NULL);
	if (!g_ctrl.h)
	{
		return;
	}

	// Initialize Variables for Service Control
	g_ctrl.s.dwServiceType = SERVICE_WIN32;
	g_ctrl.s.dwWin32ExitCode = NO_ERROR;
	g_ctrl.s.dwServiceSpecificExitCode = 0;
	g_ctrl.s.dwCheckPoint = 1;
	g_ctrl.s.dwWaitHint = 1000;
	g_ctrl.s.dwCurrentState = SERVICE_START_PENDING;
	g_ctrl.s.dwControlsAccepted = SERVICE_ACCEPT_POWEREVENT;

	// Entering Starting Service.
	BOOL bRet = SetServiceStatus(g_ctrl.h, &g_ctrl.s);
	if (!bRet)
	{
		return;
	}

	// APPLICATION SPECIFIC INITIALIZATION CODE
	// ...
	// ...

	// Finish Initializing.

	g_ctrl.s.dwCurrentState = SERVICE_RUNNING;
	g_ctrl.s.dwCheckPoint = 0;
	g_ctrl.s.dwWaitHint = 0;
	g_ctrl.s.dwControlsAccepted = SERVICE_ACCEPT_POWEREVENT;
	bRet = SetServiceStatus(g_ctrl.h, &g_ctrl.s);
	if (!bRet)
	{
		return;
	}

	//
	// Service Main Code.
	//

	StartLooking();

	for (; g_ctrl.s.dwCurrentState != SERVICE_STOP_PENDING && g_ctrl.s.dwCurrentState != SERVICE_STOPPED; )
	{
		Sleep(1 * 1000);
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, P("pExTrapperMutex"));
	if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		DEBUG_OUT_DEFAULT("duplicate process failed. pExTrapper");
		return 9;
	}

	DWORD WINAPI SocketWaitingThread(LPVOID pParam);
	HANDLE hThread = CreateThread(NULL, 0, SocketWaitingThread, NULL, 0, NULL);
	CloseHandle(hThread);

#ifndef _DEBUG
	if ("")
	{
		SERVICE_TABLE_ENTRY ServiceTable[] = {
			{ SERVICE_NAME, ServiceMain },
			{ NULL, NULL }
		};

		BOOL bRet = StartServiceCtrlDispatcher(ServiceTable);

		TerminateProcess(GetCurrentProcess(), 0);

		return 0;
	}
#endif

	StartLooking();

	// メイン メッセージ ループ:
	MSG msg = { 0 };
	for (; GetMessage(&msg, NULL, 0, 0); )
	{
		if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

// 受信処理
DWORD WINAPI SocketTransThread(LPVOID pParam)
{
	CWSAInterface wsa;

	SOCKET sock = (SOCKET)pParam;

	CoAutoSocket autoSocket(sock);

	CString recvData;
	TransRecvTCP(sock, recvData);

	if (strncmp(recvData, P("STOP CONTROL. "), strlen(P("STOP CONTROL. "))) == 0)
	{
		g_ctrl.s.dwServiceType = SERVICE_WIN32;
		g_ctrl.s.dwWin32ExitCode = NO_ERROR;
		g_ctrl.s.dwServiceSpecificExitCode = 0;
		g_ctrl.s.dwCheckPoint = 1;
		g_ctrl.s.dwWaitHint = 1000;
		g_ctrl.s.dwCurrentState = SERVICE_RUNNING;

		if (recvData.Right(7).Compare(P(" ENABLE")) == 0)
		{
			g_ctrl.s.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_POWEREVENT;
			BOOL bRet = SetServiceStatus(g_ctrl.h, &g_ctrl.s);
			if (!bRet)
			{
				return 0;
			}
		}
		else if (recvData.Right(8).Compare(P(" DISABLE")) == 0)
		{
			g_ctrl.s.dwControlsAccepted = SERVICE_ACCEPT_POWEREVENT;
			BOOL bRet = SetServiceStatus(g_ctrl.h, &g_ctrl.s);
			if (!bRet)
			{
				return 0;
			}
		}

		send(sock, P("SO FUNNY CONTROL."), (int)strlen(P("SO FUNNY CONTROL.")), 0);
	}
	else
	{
		if ("")
		{
			return 0;
		}
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

	WORD portNo = 12085;
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
