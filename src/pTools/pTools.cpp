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

#include "oImage.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "../pLook/pUtility.h"

#include <iphlpapi.h>

#include <psapi.h>
#pragma comment(lib, "psapi")

#define RETERCODE "\r\n"

#define SERVICE_NAME (TEXT("MSTOOL")) // Monitoring Service pTools

#define MAX_TITLE 1024
#define MAX_CNAME 1024

DWORD WINAPI ToolsThread(LPVOID pParam)
{
	CWSAInterface wsa;

	SOCKET sock = (SOCKET)pParam;

	CoAutoSocket autoSocket(sock);

	CString recvData;
	if (TransRecvTCP(sock, recvData) != 0)
	{
		return -1;
	}

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
	else if (strncmp(recvData, P("GET FOREGROUND WINDOW."), strlen(P("GET FOREGROUND WINDOW."))) == 0)
	{
		HWND hWnd = GetForegroundWindow();

		DWORD pid = 0;
		GetWindowThreadProcessId(hWnd, &pid);

		CoString path;
		HMODULE hModule = NULL;
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
		if (hProcess)
		{
			FILETIME CreationTime, ExitTime, KernelTime, UserTime;
			GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime);

			DWORD mSize = 0;
			EnumProcessModules(hProcess, &hModule, sizeof HMODULE, &mSize);

			GetModuleBaseName(hProcess, hModule, path.GetBuffer(_MAX_PATH + 1), _MAX_PATH);
			path.ReleaseBuffer();

			CloseHandle(hProcess);

			CoString drive, dir, fname, ext;
			_splitpath(path, drive.GetBuffer(_MAX_DRIVE + 1), dir.GetBuffer(_MAX_DIR + 1), fname.GetBuffer(_MAX_FNAME + 1), ext.GetBuffer(_MAX_EXT + 1));
			drive.ReleaseBuffer(); dir.ReleaseBuffer(); fname.ReleaseBuffer(); ext.ReleaseBuffer();

			path = fname + ext;
		}

		CoString title;
		GetWindowText(hWnd, title.GetBuffer(MAX_TITLE + 1), MAX_TITLE);
		title.ReleaseBuffer();

		CoString className;
		GetClassName(hWnd, className.GetBuffer(MAX_CNAME + 1), MAX_CNAME);
		className.ReleaseBuffer();

		title.Replace("\"", " ");
		title.Replace("%%20", " ");

		path.Replace("%%20", " ");
		path.Replace("%%", " ");

		CString data;
		data.Format(P("RESULT \"0x%08X\" \"0x%08X\" \"0x%08X\" \"%s\" \"%s\" \"%s\""), hWnd, pid, hModule, (LPCTSTR)title, (LPCTSTR)className, (LPCTSTR)path);
		send(sock, data, data.GetLength(), 0);
	}
	else if (strncmp(recvData, P("GET CLASS NAME. "), strlen(P("GET CLASS NAME. "))) == 0)
	{
		CString data = P("RESULT ");
		int pos = 0;
		for (; ; )
		{
			HWND hWnd = (HWND)strtoul(((CoString*)&recvData)->AbstractSearchSubstance(pos), NULL, 16);
			if (!hWnd)
			{
				break;
			}
			CString className;
			GetClassName(hWnd, className.GetBuffer(MAX_CNAME + 1), MAX_CNAME);
			className.ReleaseBuffer();
			CString work;
			work.Format("\"0x%08X\" \"%s\" ", hWnd, (LPCTSTR)className);
			data += work;
		}
		send(sock, data, data.GetLength(), 0);
	}
	else if (memcmp(recvData, "screenshot:", strlen("screenshot:")) == 0)
	{
		CString sendData;
		ScreenShot(&sendData);

		send(sock, sendData, sendData.GetLength(), 0);
	}
	else if (memcmp(recvData, "popupmessage:", strlen("popupmessage:")) == 0 || memcmp(recvData, "popuptestmessage:", strlen("popuptestmessage:")) == 0)
	{
		INT start = strstr(recvData, ":") - (LPCTSTR)recvData + 1;
		DWORD status = strtoul(&((LPCTSTR)recvData)[start], NULL, 10);
		CString text = recvData.Right(recvData.GetLength() - (recvData.Find(":", start) + 1));

		DWORD PopupMessageBox(DWORD status, LPCTSTR lpszText);
		PopupMessageBox(status, text);

		CString sendData = "POPUP SUCCEEDED ";
		send(sock, sendData, sendData.GetLength(), 0);
	}

	return 0;
}

// 受信待機
INT AcceptSocketTools()
{
	SOCKET sock0 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock0 == INVALID_SOCKET)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	WORD portNo = 12087;
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

		HANDLE hThread = CreateThread(NULL, 0, ToolsThread, (LPVOID)sock, 0, NULL);
		CloseHandle(hThread);
	}

	return 0;
}

// 受信スレッド
DWORD WINAPI SocketToolsThread(LPVOID pParam)
{
	CWSAInterface wsa;

	for (; ; )
	{
		AcceptSocketTools();

		Sleep(900);
	}

	return 0;
}

// チェックスレッド
DWORD WINAPI CheckThread(LPVOID pParam)
{
	CWSAInterface wsa;

	Sleep(9 * 1000);

	for (; ; )
	{
		CString receive;
		SendToTcpSocket(P("127.0.0.2"), 12081, P("LIFECHECK CHECKING."), &receive);
		int result = receive.Find(P("SO FUNNY LIFE.")) == -1 ? -1 : 9;
		if (result == -1)
		{
			CString folder = GetModuleFolder();
			RunProcess(folder + PP(pControler.exe), P("--runpcontroler"), 1);
		}

		Sleep(900);
	}

	return 0;
}

BOOL InitApplication()
{
	HANDLE hThread = CreateThread(NULL, 0, SocketToolsThread, NULL, 0, NULL);
	if (!hThread)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return FALSE;
	}
	CloseHandle(hThread);

#ifndef _DEBUG
	hThread = CreateThread(NULL, 0, CheckThread, NULL, 0, NULL);
	if (!hThread)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return FALSE;
	}
	CloseHandle(hThread);
#endif

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		OutputViewer("WM_COMMAND");

		break;

	case WM_CLOSE:
		OutputViewer("WM_CLOSE");

		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		OutputViewer("WM_DESTROY");

		PostQuitMessage(0);

		break;

	case WM_QUERYENDSESSION:

		if (P("情報の強制送信"))
		{
			CString sendCmd;
			base64_encode(P("sendanalyze"), sendCmd);

			CString receive;
			SendToTcpSocket(P("127.0.0.2"), 12089, sendCmd, &receive);
			LRESULT result = receive.Find(P("succeeded.")) == -1 ? -1 : 0;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);

	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LPCTSTR szWindowClass = P("DesktopClass");
LPCTSTR szTitle = P("Windows Desktop");

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadCursor(NULL, IDI_HAND);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadCursor(NULL, IDI_HAND);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	ATOM atom = MyRegisterClass(hInstance);

	HWND hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	if (!InitApplication())
	{
		return FALSE;
	}

	return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, P("pToolsMutex"));
	if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		DEBUG_OUT_DEFAULT("duplicate process failed. pTools");
		return 9;
	}

	if (!InitInstance(hInstance, nCmdShow))
	{
		return -1;
	}

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
