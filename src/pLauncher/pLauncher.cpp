////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2011 Plustar Corporation. All rights reserved. プラスター
//
// ┌──┐┌┐    ┌┐┌┐┌──┐┌──┐┌──┐┌──┐
// └─┐│││    │││││┌─┘└┐┌┘│┌┐││┌┐│
// ┌─┘│││    │││││└─┐  ││  │└┘││└┴┘
// │┌─┘││    ││││└─┐│  ││  │┌┐││┌┬┐
// ││    │└─┐│└┘│┌─┘│  ││  ││││││││
// └┘    └──┘└──┘└──┘  └┘  └┘└┘└┘└┘
//
// Powered by jobscale
// Since 22 July 1973
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#pragma comment(lib, "Version")

#include <MsHTML.h>

#include <Exdisp.h>

#include <ShellAPI.h>
#include "resource.h"

#define SECOND *(1000)
#define MINUTE *(60 SECOND)

#define P(x) "\0 " x + 2

#define WM_TRAYICONMESSAGE (WM_USER + 1)

#define AUTO_OPEN 15, 0 // 無操作時の自動オープン
#define AUTO_CLOSE 10, 0 // メモリ解放のため自動クローズ

// レクラングルADの横幅 + 画面に常に見えてる部分 + バッファ
#define EDGH_LEN 16
#define CONT_LEN 354
#define RIGHT_OUT /* 14 */ 0
#define WND_WIDTH (EDGH_LEN + CONT_LEN)

// テンプレート
// 本来はJavascript内でブラウザ依存処理をすべき
#define MAIN_PAGE_ "http://pl.plustar.jp/plustar" // 一般用
#define MAIN_PAGE_7 "http://pl.plustar.jp/plustar-ie7" // 一般用(IE7対応)
#define MAIN_PAGE_W "http://pl.plustar.jp/plustar-w" // 女性用
#define MAIN_PAGE_X "http://pl.plustar.jp/plustar-x" // 子供用

BOOL IEVER8()
{
	BOOL bRet = FALSE;

	TCHAR szSysDir[_MAX_PATH];
	TCHAR szPath[_MAX_PATH];
	if(GetSystemDirectory(szSysDir, _MAX_PATH))
	{
	#pragma warning(disable:4996)
	if (!"")
	{
	_tcscpy(szPath, szSysDir);
	PathRemoveBackslash(szPath);
	_tcscat(szPath, _T("\\ieframe.dll"));
	}
	else
	{
	_tcscpy(szPath, _T("ieframe.dll"));
	}

	DWORD dwReserved;
	DWORD dwSize = GetFileVersionInfoSize(szPath, &dwReserved);
	if(dwSize == 0)
	{
	return FALSE;
	}

	LPVOID lpvVer = NULL;
	HGLOBAL hMem = GlobalAlloc(GHND, dwSize);
	if(hMem && (lpvVer = (LPVOID)GlobalLock(hMem)) == NULL )
	{
	return FALSE;
	}

	if(GetFileVersionInfo(szPath, 0, dwSize, lpvVer) == FALSE)
	{
	hMem = GlobalHandle(lpvVer);
	GlobalUnlock(hMem);
	GlobalFree(hMem);
	return FALSE;
	}

	LPDWORD lpBuffer;
	UINT uiLen;
	if(VerQueryValue(lpvVer, "\\VarFileInfo\\Translation", (LPVOID *)&lpBuffer, &uiLen) == 0)
	{
	hMem = GlobalHandle(lpvVer);
	GlobalUnlock(hMem);
	GlobalFree(hMem);
	return FALSE;
	}

	LPSTR lpszDesc;
	char szGetName[64];
	wsprintf(szGetName, "\\StringFileInfo\\%04x%04x\\FileVersion", LOWORD(*lpBuffer), HIWORD(*lpBuffer));
	if(VerQueryValue(lpvVer, szGetName, (LPVOID *)&lpszDesc, &uiLen) == 0)
	{
	}
	else
	{
	int nMajorVer = 0;
	int nMinerVer = 0;
	int nMajorLot = 0;
	int nMinerLot = 0;
	_stscanf(lpszDesc, _T("%d.%d.%d.%d"),
	&nMajorVer, &nMinerVer, &nMajorLot, &nMinerLot);
	if(nMajorVer>=8)
	bRet=TRUE;
	else
	bRet=FALSE;
	}

	hMem = GlobalHandle(lpvVer);
	GlobalUnlock(hMem);
	GlobalFree(hMem);

	}

	return bRet;
}

const char* MAIN_PAGE()
{
	if (!IEVER8())
	{
		return MAIN_PAGE_7;
	}

	return MAIN_PAGE_;
}

struct window_info {
  int x;
  int y;
  int width;
  int height;
} wndinfo;

LPCSTR szTitle = _T("");
LPCSTR szWindowClass = _T("pLauncher");
INT _locked = 0;

HINSTANCE InstanceHandle(HINSTANCE hInstance = NULL)
{
	static HINSTANCE s_hInstance = hInstance;
	return s_hInstance;
}

LRESULT AddTaskbarIcons(HWND hWnd)
{
	// Add TrayIcon
	static HICON s_hIcon = LoadIcon(InstanceHandle(), MAKEINTRESOURCE(IDI_ICON));
	NOTIFYICONDATA notif = { sizeof NOTIFYICONDATA };
	TCHAR cToolTipMsg[] = _T("プラスター ランチャー");
	notif.hWnd = hWnd;
	notif.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	notif.uCallbackMessage = WM_TRAYICONMESSAGE;
	notif.hIcon = s_hIcon;
	strcpy_s(notif.szTip, cToolTipMsg);
	Shell_NotifyIcon(NIM_ADD, &notif);

	return 0;
}

RECT rectWork = { 0 };

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	InstanceHandle(hInstance);

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWork, 0);
	rectWork.right = GetSystemMetrics(SM_CXSCREEN);

	wndinfo.width = WND_WIDTH;
	wndinfo.height = rectWork.bottom;
	wndinfo.x = rectWork.right - wndinfo.width + RIGHT_OUT;
	wndinfo.y = 0;

	HWND hWnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
			szWindowClass, szTitle, WS_POPUP | WS_VISIBLE,
			wndinfo.x, wndinfo.y, wndinfo.width, wndinfo.height,
			NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

	rectWork.right -= EDGH_LEN;

	SystemParametersInfo(SPI_SETWORKAREA, 0, &rectWork, SPIF_SENDWININICHANGE);

	// Add TrayIcon
	AddTaskbarIcons(hWnd);

  return TRUE;
}

BOOL InitWebBrowser(HWND hWnd, CComQIPtr<IWebBrowser2>& pWB2)
{
  AtlAxWinInit();

  return TRUE;
}

void MoveClose(HWND hWnd, HWND& hwndIE)
{
	for (int i = 0; i < 100; i++)
	{
		static int moved = 0;

		moved += 5;

		if (moved > CONT_LEN)
		{
			moved = CONT_LEN;
		}

		MoveWindow(hWnd, wndinfo.x + moved, wndinfo.y, wndinfo.width, wndinfo.height, TRUE);

		if (moved == CONT_LEN)
		{
			moved = 0;
			break;
		}

		Sleep(5);
	}
}

BOOL operator==(POINT& src, POINT& trg)
{
	return src.x == trg.x && src.y == trg.y;
}

BOOL operator!=(POINT& src, POINT& trg)
{
	return !(src == trg);
}

int AutoClose(BOOL bSet = FALSE)
{
	static POINT _point = { 0 };
	if (bSet)
	{
		GetCursorPos(&_point);
		return 0;
	}

	POINT point = { 0 };
	GetCursorPos(&point);

	if (point != _point)
	{
		return 1;
	}

	return 0;
}

#include <WinInet.h>
#pragma comment(lib, "WinInet")
#pragma comment(lib, "Urlmon")

BOOL LifeWebBrowser(HWND hWnd, HWND& hwndIE, CComQIPtr<IWebBrowser2>& pWB2, BOOL bLife = TRUE)
{
	if (!hwndIE && bLife)
	{
		KillTimer(hWnd, 333);

		hwndIE = CreateWindowEx(0, _T(ATLAXWIN_CLASS),
			_T("Shell.Explorer.2")/* _T("Mozilla.Browser.1")*/ , WS_CHILD | WS_VISIBLE,
			0, 0, wndinfo.width, wndinfo.height, hWnd, NULL, GetModuleHandle(NULL), NULL);

		if (!hwndIE)
		{
			return FALSE;
		}

		IUnknown* pUnk = NULL;
		if (AtlAxGetControl(hwndIE, &pUnk) != S_OK)
		{
			return FALSE;
		}

		pWB2 = pUnk;
		if (!pWB2)
		{
			DestroyWindow(hwndIE);
			hwndIE = NULL;
			return FALSE;
		}

		/*if (0)
		{
			CComVariant urlDumm(_T("about:blank"));
			pWB2->Navigate2(&urlDumm, 0, 0, 0, 0);

			// ディスパッチの取得
			IDispatch* pDisp = NULL;
			HRESULT hr = pWB2->get_Document(&pDisp);

			// ドキュメントの取得
			IHTMLDocument2 *pDocument = NULL;
			hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDocument);
			if (FAILED(hr))
			{
				return FALSE;
			}

			// コネクションコンテナの取得
			IConnectionPointContainer *pConnectionPointContainer = NULL;
			hr = pWB2->QueryInterface(&pConnectionPointContainer);
			if (FAILED(hr))
			{
				return FALSE;
			}

			// コネクションイベントの取得
			IConnectionPoint* pConnectionPoint = NULL;
			pConnectionPointContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &pConnectionPoint);
			pConnectionPointContainer->Release();

			// クッキーの取得
			DWORD dwCookie = 0;
			pConnectionPoint->Advise(static_cast<IDispatch *>(pDisp), &dwCookie);

			// エレメントの取得
			IHTMLElement *pBody = NULL;
			hr = pDocument->get_body(&pBody);
			IHTMLElement2 *pElement = NULL;
			hr = pBody->QueryInterface(IID_IHTMLElement2,(void**)&pElement);
			pElement->put_scrollTop(100); 
			long scroll_height; 
			pElement->get_scrollHeight(&scroll_height);
			long real_scroll_height;
			pElement->put_scrollTop(20000000); // ask to scroll really far down...
			pElement->get_scrollTop(&real_scroll_height);
			real_scroll_height += 20; // will return the scroll height

			// スクロールバーの設定
			// pWebBrowser2 pDocument pBody pStyle = "overflow-x:hidden;overflow-y:hidden"

			// タイムアウトの設定
			DWORD dwTimeOut = 2 * 1000;
			InternetSetOption(pWB2, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD));

			// ユーザエージェントの取得
			CHAR szUserAgent[256] = { 0 };
			DWORD dwLength = 0;
			UrlMkGetSessionOption(URLMON_OPTION_USERAGENT, szUserAgent, sizeof szUserAgent, &dwLength, 0);
		}*/

		// BrowserProxySetting
		static INTERNET_PROXY_INFO struct_IPI;
		struct_IPI.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
		struct_IPI.lpszProxy = _T("127.0.0.1:8119");
		struct_IPI.lpszProxyBypass = _T("local");

		BOOL result = InternetSetOption(NULL, INTERNET_OPTION_PROXY, &struct_IPI, sizeof INTERNET_PROXY_INFO);
		result = InternetSetOption(NULL, INTERNET_OPTION_PROXY_SETTINGS_CHANGED, 0, 0);

		CComVariant url(MAIN_PAGE());
		pWB2->Navigate2(&url, 0, 0, 0, 0);
	}
	else if (hwndIE && !bLife)
	{
		pWB2.Release();
		DestroyWindow(hwndIE);
		hwndIE = NULL;

		SetTimer(hWnd, 333, 100, NULL);
	}

	return TRUE;
}

LRESULT WindowActiveControl(HWND hWnd, HWND& hwndIE, WPARAM wParam, CComQIPtr<IWebBrowser2>& pWB2)
{
#ifdef _DEBUG
	char sss[100];
	sprintf_s(sss, sizeof sss, "[%d]" "\r\n", wParam);
	OutputDebugString(sss);
#endif

	static SYSTEMTIME syst = { 0 };
	static WPARAM prev = -1;
	if (syst.wDay == 0)
	{
		GetLocalTime(&syst);
		prev = wParam;
	}
	else
	{
		int doub = wParam == WA_INACTIVE && prev == WA_INACTIVE;
		int spoc = wParam == WA_ACTIVE && prev == WA_INACTIVE;
		int tasc = wParam == 9 && prev == WA_INACTIVE;
		int tasr = wParam == WA_INACTIVE && prev == 9;
		if (doub || spoc || tasc || tasr)
		{
			COleDateTime now = COleDateTime::GetCurrentTime();
			now -= COleDateTimeSpan(0, 0, 0, 2);
			if (now < syst)
			{
				return 0;
			}
		}
		GetLocalTime(&syst);
		prev = wParam;
	}

	RECT rect = { 0 };
	GetWindowRect(hWnd, &rect);

	HWND insertAfter = HWND_NOTOPMOST;

	if (wParam == 8)
	{
		wParam = rect.left < wndinfo.x + CONT_LEN ? WA_INACTIVE : WA_ACTIVE;
	}
	else if (wParam == 9)
	{
		wParam = rect.left < wndinfo.x + CONT_LEN ? WA_INACTIVE : WA_ACTIVE;
		if (wParam == WA_ACTIVE)
		{
			SetForegroundWindow(hWnd);
			return 0;
		}
		else
		{
			SetForegroundWindow(GetDesktopWindow());
		}
	}

	if (wParam != WA_INACTIVE)
	{
		LifeWebBrowser(hWnd, hwndIE, pWB2, TRUE);

		POINT point = { 0 };
		GetCursorPos(&point);

		if (wParam == WA_CLICKACTIVE)
		{
			ScreenToClient(hWnd, &point);
		}

		SetWindowPos(hWnd, insertAfter, wndinfo.x, wndinfo.y, wndinfo.width, wndinfo.height, SWP_SHOWWINDOW | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_SHOWNORMAL);

		if (wParam == WA_CLICKACTIVE)
		{
			ClientToScreen(hWnd, &point);
			SetCursorPos(point.x, point.y);
		}

		if (pWB2)
		{
			pWB2->ExecWB(OLECMDID_CLEARSELECTION, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
		}

		SetTimer(hWnd, 322, 1 MINUTE, NULL);
	}
	else
	{
		if (_locked)
		{
			RECT rect = rectWork;
			rect.right -= CONT_LEN;
			SystemParametersInfo(SPI_SETWORKAREA, 0, &rect, SPIF_SENDWININICHANGE);
		}
		else
		{
			KillTimer(hWnd, 322);

			MoveClose(hWnd, hwndIE);

			SystemParametersInfo(SPI_SETWORKAREA, 0, &rectWork, SPIF_SENDWININICHANGE);

			LifeWebBrowser(hWnd, hwndIE, pWB2, FALSE);
		}
	}

	return 0;
}

struct BRSHSTR
{
	HBRUSH h;
	COLORREF c;
};

BRSHSTR br[] = {
	0, RGB(0, 0, 0),
	0, RGB(11, 16, 109),
	0, RGB(11, 16, 109),
	0, RGB(11, 16, 109),
	0, RGB(20, 29, 109),
	0, RGB(30, 49, 127),
	0, RGB(41, 70, 145),
	0, RGB(53, 90, 164),
	0, RGB(64, 111, 183),
	0, RGB(75, 132, 201),
	0, RGB(87, 149, 216),
	0, RGB(101, 160, 224),
	0, RGB(116, 170, 231),
	0, RGB(130, 181, 238),
	0, RGB(145, 191, 245),
	0, RGB(200, 200, 255),
	0, RGB(240, 200, 80),
	0, RGB(190, 250, 90),
};

int InitBrush()
{
	for (int i = 0; i < sizeof br / sizeof BRSHSTR; i++)
	{
		br[i].h = CreateSolidBrush(br[i].c);
	}

	return 0;
}

void Paint(HWND hWnd, HWND& hwndIE)
{
	PAINTSTRUCT ps = { 0 };
	BeginPaint(hWnd, &ps);

	static int hbb = InitBrush();

	RECT rect = { 0 };
	GetClientRect(hWnd, &rect);
	rect.right = rect.left + EDGH_LEN;

	RECT rec1 = rect;
	rec1.right = rec1.left + 1;

	for (int i = 0; i < EDGH_LEN; i++)
	{
		FillRect(ps.hdc, &rec1, br[i].h);
		rec1.left += 1; rec1.right += 1;
	}

	SYSTEMTIME syst = { 0 };
	GetLocalTime(&syst);

	int pos = 59 - syst.wSecond;
	double height = (double)rect.bottom / 60;
	rect.top = (int)(height * pos - height * syst.wMilliseconds / 1000);
	rect.bottom = (int)(rect.top + height);

	FillRect(ps.hdc, &rect, (syst.wSecond % 5) ? br[EDGH_LEN].h : br[EDGH_LEN + 1].h);

	RECT rec2 = rect;
	rec2.right = rect.left + 3;
	rec2.bottom = rect.top + 1;
	FillRect(ps.hdc, &rec2, br[0].h);
	rec2.right = rect.left + 1;
	rec2.bottom = rect.top + 2;
	FillRect(ps.hdc, &rec2, br[0].h);

	rec2.bottom = rect.bottom;
	rec2.top = rect.bottom - 2;
	FillRect(ps.hdc, &rec2, br[0].h);
	rec2.right = rect.left + 2;
	rec2.top = rect.bottom - 1;
	FillRect(ps.hdc, &rec2, br[0].h);

	static char sect[10];
	sprintf_s(sect, sizeof sect, "%02d", syst.wSecond);

	HFONT hf = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	HFONT hf_ = (HFONT)SelectObject(ps.hdc, hf);
	SetBkMode(ps.hdc,TRANSPARENT);
	TextOut(ps.hdc, rect.left + 1, rect.top + 1, sect, strlen(sect));
	SelectObject(ps.hdc, hf_);

	EndPaint(hWnd, &ps);
}

// ダイアログプロシージャ
BOOL CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDOK:
		case IDCANCEL:
			ShowWindow(hWnd, SW_HIDE);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		return TRUE;
	}

	return FALSE;  // DefWindowProc()ではなく、FALSEを返すこと！
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static UINT s_uTaskbarRestart = 0;

	static HWND hwndIE = 0;
	static CComQIPtr<IWebBrowser2> pWB2;
	static SYSTEMTIME mouSys = { 0 };
	static POINT mouCur = { 0 };

	switch (msg) {
	case WM_CREATE:
		s_uTaskbarRestart = RegisterWindowMessage(_T("TaskbarCreated"));
		if (InitWebBrowser(hWnd, pWB2))
		{
		}
		break;
		/* FALLTHROUGH */
	case WM_DESTROY:
		LifeWebBrowser(hWnd, hwndIE, pWB2, FALSE);
		PostQuitMessage(0);
		break;
	case WM_NCDESTROY: {
		// Delete TrayIcon
		NOTIFYICONDATA notif = { sizeof NOTIFYICONDATA };
		notif.hWnd = hWnd;
		Shell_NotifyIcon(NIM_DELETE, &notif); }
		return DefWindowProc(hWnd, msg, wParam, lParam);
	case WM_ACTIVATE:
		WindowActiveControl(hWnd, hwndIE, wParam, pWB2);
		return DefWindowProc(hWnd, msg, wParam, lParam);
	case WM_TRAYICONMESSAGE:
		switch (lParam) {
		case WM_LBUTTONDOWN:
			WindowActiveControl(hWnd, hwndIE, 9, pWB2);
			break;
		case WM_RBUTTONDOWN:
			SetTimer(hWnd, 312, 100, NULL);
			break;
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_LOCKED:
			_locked = (_locked + 1) % 2;
			WindowActiveControl(hWnd, hwndIE, 9, pWB2);
			return TRUE;
		case ID_ABOUT:
			{
				static HWND hDlg = CreateDialog(InstanceHandle(), MAKEINTRESOURCE(IDD_DIALOG), NULL, DlgProc);

				RECT deRect = { 0 };
				GetWindowRect(GetDesktopWindow(), &deRect);
				RECT rect = { 0 };
				GetClientRect(hDlg, &rect);
				int left = (deRect.right - rect.right) / 2;
				int top = (deRect.bottom - rect.bottom) / 2;

				SetWindowPos(hDlg, HWND_TOP, left, top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
			}
			return TRUE;
		}
		break;
	case WM_TIMER:
		if (LOWORD(wParam) == 312)
		{
			KillTimer(hWnd, 312);

			HMENU hMenu = LoadMenu(InstanceHandle(), MAKEINTRESOURCE(IDR_MENU));
			HMENU hSubMenu = GetSubMenu(hMenu, 0);

			CheckMenuItem(hSubMenu, ID_LOCKED, MF_BYCOMMAND | _locked ? MF_CHECKED : MF_UNCHECKED);
			POINT pt = { 0 };
			GetCursorPos(&pt);
			TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_VERNEGANIMATION | TPM_LEFTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
			DestroyMenu(hMenu);

			return TRUE;
		}
		else if (LOWORD(wParam) == 322)
		{
			POINT pos = { 0 };
			GetCursorPos(&pos);

			if (mouCur != pos)
			{
				GetLocalTime(&mouSys);
				mouCur = pos;
			}
			else
			{
				COleDateTime now = COleDateTime::GetCurrentTime();
				now -= COleDateTimeSpan(0, 0, AUTO_CLOSE);
				if (now > mouSys)
				{
					_locked = 0;
					GetLocalTime(&mouSys);
					WindowActiveControl(hWnd, hwndIE, 8, pWB2);

					AutoClose(TRUE);
				}
			}
			return TRUE;
		}
		else if (LOWORD(wParam) == 333)
		{
			RECT rect = { 0 };
			GetClientRect(hWnd, &rect);
			rect.right = rect.left + EDGH_LEN;
			InvalidateRect(hWnd, &rect, FALSE);

			POINT pos = { 0 };
			GetCursorPos(&pos);

			if (mouCur != pos)
			{
				GetLocalTime(&mouSys);
				mouCur = pos;
			}
			else
			{
				COleDateTime now = COleDateTime::GetCurrentTime();
				now -= COleDateTimeSpan(0, 0, AUTO_OPEN);
				if (now > mouSys)
				{
					if (AutoClose())
					{
						GetLocalTime(&mouSys);
						WindowActiveControl(hWnd, hwndIE, 8, pWB2);
					}
				}
			}
			return TRUE;
		}
		break;
	case WM_PAINT:
		Paint(hWnd, hwndIE);
		break;
	default:
		if (msg == s_uTaskbarRestart) {
			AddTaskbarIcons(hWnd);
			break;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  return RegisterClassEx(&wcex);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR lpCmdLine,
                       int nCmdShow)
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, P("pLauncherMutex"));
	if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		OutputDebugString("duplicate process failed. pLauncher");
		return 9;
	}

	CComModule _Module;
	_Module.Init(NULL, hInstance);

	MyRegisterClass(hInstance);
	if (!InitInstance (hInstance, nCmdShow)) {
		return FALSE;
	}

	MSG msg = { 0 };

	for (; ; )
	{
		try
		{
			for (; GetMessage(&msg, NULL, 0, 0); )
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (!msg.hwnd)
			{
				break;
			}
		}
		catch (...)
		{
		}
	}

	return (int)msg.wParam;
}
