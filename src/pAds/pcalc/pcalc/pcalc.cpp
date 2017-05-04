#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#include <CommCtrl.h>

#include "resource.h"

#define P(x) ("\0 " x) + 2

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

POINT _zah = { 0 };

#include <Exdisp.h>
#include <WinInet.h>
#pragma comment(lib, "WinInet")

HWND _hwndIE = 0;

int OnReload()
{
	if (!_hwndIE)
	{
		return -1;
	}

	IUnknown* pUnk = NULL;
	if (AtlAxGetControl(_hwndIE, &pUnk) != S_OK)
	{
		return -1;
	}

	CComQIPtr<IWebBrowser2> pWB2 = pUnk;
	if (!pWB2)
	{
		DestroyWindow(_hwndIE);
		_hwndIE = NULL;
		return FALSE;
	}

	CComVariant url(P("http://ac.plustar.jp/ad/ad.php?ap=pcalc"));
	pWB2->Navigate2(&url, 0, 0, 0, 0);

	return 0;
}

HWND _edit_box = 0;

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		40, 0, 746, 300, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	_edit_box = CreateWindowEx(0, WC_EDIT,
		NULL, WS_CHILD | WS_VISIBLE,
		100, 150, 400, 30, hWnd, NULL, hInstance, NULL);

	AtlAxWinInit();

	_hwndIE = CreateWindowEx(0, ATLAXWIN_CLASS,
		P("Shell.Explorer.2"/*"Mozilla.Browser.1"*/), WS_CHILD | WS_VISIBLE,
		0, 0, 760, 92, hWnd, NULL, hInstance, NULL);

	if (!_hwndIE)
	{
		return FALSE;
	}

	IUnknown* pUnk = NULL;
	if (AtlAxGetControl(_hwndIE, &pUnk) != S_OK)
	{
		return FALSE;
	}

	CComQIPtr<IWebBrowser2> pWB2 = pUnk;
	if (!pWB2)
	{
		DestroyWindow(_hwndIE);
		_hwndIE = NULL;
		return FALSE;
	}

	CComVariant url(P("http://ac.plustar.jp/ad/ad.php?ap=pcalc"));
	pWB2->Navigate2(&url, 0, 0, 0, 0);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PCALC));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PCALC);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

#include <shellapi.h>

INT_PTR CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

int hexToInt(char c)
{
    if ('0' <= c && c <= '9') { return c - '0'; }
	else if ('a' <= c && c <= 'f') { return c - 'a' + 0x0a; }
	else if ('A' <= c && c <= 'F') { return c - 'A' + 0x0a; }
	else { return -1; }
}

char intToHex(int x)
{
    if (0 <= x && x <= 9) { return x + '0'; }
	else if (10 <= x && x <= 15) { return x - 10 + 'A'; }
	else { return '\0'; }
}

int encodeUrl(char* outBuf, const char* str)
{
	char* out = outBuf;
    for (const char* it = str; *it; it++)
	{
        char c = *it;
        if (c == ' ')
		{
            *out = '+';
			out++;
        }
		else if (('A' <= c && c <= 'Z') ||  ('a' <= c && c <= 'z') || ('0' <= c && c <= '9') ||
				(c == '@') || (c == '*') || (c == '-') || (c == '.') || (c == '_'))
		{
            *out = c;
			out++;
        }
		else
		{
            *out = '%';
			out++;
            *out = intToHex((c >> 4) & 0x0f);
			out++;
            *out = intToHex(c & 0x0f);
			out++;
        }
    }
	*out = 0;
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	HBRUSH hbr = 0;
	char text[2048] = { 0 };
	char url[2048] = { 0 };
	char* buf = 0;

	RECT rect;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutProc);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hbr = CreateSolidBrush(RGB(200, 200, 0));
		SetRect(&rect, 100 - 10, 150 - 10, 500 + 10, 180 + 10);
		FillRect(ps.hdc, &rect, hbr);
		DeleteObject(hbr);

		hbr = CreateSolidBrush(RGB(200, 100, 0));
		SetRect(&rect, 520, 150, 600, 180);
		FillRect(ps.hdc, &rect, hbr);
		TextOut(ps.hdc, 540, 155, P("計算"), 4);
		DeleteObject(hbr);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONUP:
		GetWindowText(_edit_box, text, 2047);
		buf = (char*)malloc(strlen(text) * 7);
		encodeUrl(buf, text);
		sprintf(url, P("http://www.google.co.jp/search?q=%s"), buf);
		free(buf);
		ShellExecute(NULL, P("open"), url, NULL, NULL, SW_SHOWNORMAL);
		OnReload();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PCALC, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PCALC));

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
