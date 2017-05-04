///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

// pCheckWindow.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "../oCommon/oTools.h"
#include "../oCommon/oSocketTools.h"

#include <winsock2.h>
#include <atlstr.h>

#include "pCheckWindow.h"

#include "oShirent.h"

#include <psapi.h>
#pragma comment(lib, "psapi")
#include <atlcomtime.h>
#include <winioctl.h>

#define MAX_LOADSTRING 100

#pragma warning(disable : 4996)

class CTomBSTR : public CComBSTR
{
public:
	CTomBSTR(int size)
		: CComBSTR(size)
	{
	}
	operator LPSTR()
	{
		return (LPSTR)(BSTR)*this;
	}
	operator LPVOID()
	{
		return (LPVOID)(BSTR)*this;
	}
};

#define CComBSTR CTomBSTR

enum ENUM_GMP
{
	GMP_FULLPATH,
	GMP_FILE,
	GMP_PATH,
	GMP_DIR,
};

LRESULT GetModulePath(HWND hWnd, LPSTR path, LPSTR stime, LPDWORD ppid, ENUM_GMP gmp = GMP_FULLPATH)
{
	CComBSTR fpath(_MAX_PATH + 1);
	if (hWnd)
	{
		GetWindowThreadProcessId(hWnd, ppid);
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, *ppid);
		HMODULE hModule = NULL;
		DWORD mSize = 0;
		EnumProcessModules(hProcess, &hModule, sizeof HMODULE, &mSize);
		FILETIME CreationTime, ExitTime, KernelTime, UserTime;
		GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime);
		COleDateTime cre = CreationTime;
		strcpy(stime, cre.Format("%Y/%m/%d %H:%M:%S"));
		GetModuleBaseName(hProcess, hModule, fpath, _MAX_PATH);
		CloseHandle(hProcess);
	}
	else
	{
		GetModuleFileName(NULL, fpath, _MAX_PATH);
	}

	CComBSTR drive(_MAX_DRIVE + 1), dir(_MAX_DIR + 1), fname(_MAX_FNAME + 1), ext(_MAX_EXT + 1);
	_splitpath(fpath, drive, dir, fname, ext);

	switch (gmp)
	{
	case GMP_FULLPATH:
		sprintf(path, "%s", (LPSTR)fpath);
		break;
	case GMP_DIR:
		sprintf(path, "%s%s", (LPSTR)drive, (LPSTR)dir);
		break;
	case GMP_FILE:
		sprintf(path, "%s%s", (LPSTR)fname, (LPSTR)ext);
		break;
	case GMP_PATH:
		sprintf(path, "%s%s%s", (LPSTR)drive, (LPSTR)dir, (LPSTR)fname);
		break;
	}

	return 0;
}

#define MAX_TITLE 1024
#define MAX_CNAME 1024
#define MAX_LINE 4096
#define MAX_TIME 24

struct WADATA
{
	LPSTR afname;
	LPSTR atime;
	DWORD apid;
	LPSTR atitle;
	LPSTR aclass;
	LPSTR atext;
	LPSTR ahead;
	LPSTR apath;
};

LPCTSTR szTitle = "Permanent Window"; // タイトル バーのテキスト
LPCTSTR szWindowClass = "_CLASS_REG_PCW_"; // メイン ウィンドウ クラス名

LRESULT WriteActive(HWND hWnd, BOOL bTop = FALSE)
{
	WADATA wa = { 0 };

	CComBSTR cafname(_MAX_PATH + 1);
	CComBSTR catime(MAX_TIME + 1);
	CComBSTR catitle(MAX_TITLE + 1);
	CComBSTR caclass(MAX_CNAME + 1);
	CComBSTR catext(MAX_LINE + 1);
	CComBSTR cahead(_MAX_PATH);
	CComBSTR capath(_MAX_PATH);

	wa.afname = cafname;
	wa.atime = catime;
	wa.atitle = catitle;
	wa.aclass = caclass;
	wa.atext = catext;
	wa.ahead = cahead;
	wa.apath = capath;

	GetModulePath(hWnd, wa.afname, wa.atime, &wa.apid, GMP_FILE);

	GetWindowText(hWnd, wa.atitle, MAX_TITLE);

	GetClassName(hWnd, wa.aclass, MAX_CNAME);

	if (strcmp(szWindowClass, wa.aclass) == 0)
	{
		return 0;
	}

	COleDateTime now = COleDateTime::GetCurrentTime();

	sprintf_s(wa.atext, MAX_LINE, "\"%s\", \"%s\", \"%d\", \"%s\", \"%s\", \"%s\"\n", now.Format("%Y/%m/%d %H:%M:%S"), wa.atime, wa.apid, wa.afname, wa.aclass, wa.atitle);

	SendToTcpSocket("127.0.0.1", 12086, wa.atext);

	return 0;
}

LRESULT WriteActive()
{
	HWND hActiveWnd = GetForegroundWindow();
	if (hActiveWnd)
	{
		WriteActive(hActiveWnd);
	}

	return 0;
}

struct THREADHANDLE
{
	DWORD threadID;
	BOOL bStop;
};

THREADHANDLE& GetThread()
{
	static THREADHANDLE thread = { 0 };
	return thread;
}

DWORD WINAPI WINAPI CheckThread(LPVOID pParam)
{
	THREADHANDLE& thread = *(THREADHANDLE*)pParam;

	Sleep(3000);

	for (; !thread.bStop; )
	{
		WriteActive();

		Sleep(1000);
	}

	return 0;
}

LRESULT StartChecker()
{
	THREADHANDLE& thread = GetThread();
	if (thread.threadID)
	{
		return 0;
	}

	HANDLE hThread = CreateThread(NULL, 0, CheckThread, &thread, 0, &thread.threadID);
	if (!hThread)
	{
		return -1;
	}
	CloseHandle(hThread);

	return (LRESULT)thread.threadID;
}

LRESULT StopChecker()
{
	THREADHANDLE& thread = GetThread();
	if (!thread.threadID)
	{
		return 0;
	}

	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, thread.threadID);
	thread.bStop = TRUE;
	WaitForMultipleObjects(1, &hThread, TRUE, -1);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case 1:
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		StopChecker();
		StopDriver();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

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
	HWND hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

	// ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	StartDriver();

	StartChecker();

	return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// グローバル文字列を初期化しています。
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	DWORD WINAPI SocketWaitingThread(LPVOID pParam);
	HANDLE hThread = CreateThread(NULL, 0, SocketWaitingThread, NULL, 0, NULL);
	CloseHandle(hThread);

	HACCEL hAccelTable = LoadAccelerators(hInstance, NULL);

	// メイン メッセージ ループ:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
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
		StopChecker();

		StopDriver();

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
		if ("")
		{
			return 0;
		}

		static CString backup;
		CString check = recvData.Right(recvData.GetLength() - 23);
		if (backup == check)
		{
			return 0;
		}
		backup = check;

		CString path = GetModuleFolder(1);
		path += ".log";

		HANDLE hFile = CreateFile(path, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER num = { 0 };
			SetFilePointerEx(hFile, num, NULL, FILE_END);
			DWORD dwSize = 0;
			WriteFile(hFile, recvData, recvData.GetLength(), &dwSize, NULL);
			SetEndOfFile(hFile);
			CloseHandle(hFile);
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

	WORD portNo = 12086;
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
