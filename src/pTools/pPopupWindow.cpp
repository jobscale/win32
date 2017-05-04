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

#include "oSound.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"
#include "../ofc/oFoundationClassLibrary.h"

#include "../pLook/pUtility.h"

#include <psapi.h>
#pragma comment(lib, "psapi")

namespace PopupWnd
{

// ポップアップメッセージと音声で情報を通知する
// status
// 0 DEFAULT
// 1 ATTENTION
// 2 CAUTION
// 3 ALERT
// 4 EMERGENCY
// 5 WARNING
// 6 ERROR
// 7 FAILED

struct STATUS_DATA
{
	DWORD status;
	LPCTSTR webName;
	DWORD times;
};

static const STATUS_DATA statusTable[] = {
	0, "", 1,
	1, "", 1,
	2, "", 1,
	3, "", 1,
	4, "emergency.wav", 5,
	5, "warning.wav", 3,
	6, "", 1,
	7, "", 1,
	-1, NULL, 0
};

LPCTSTR GetWavName(HWND hWnd, const DWORD status)
{
	POPWND_DATA& popwndData = *(POPWND_DATA*)GetWindowLong(hWnd, GWLP_USERDATA);

	for (DWORD i = 0; statusTable[i].status != -1; i++)
	{
		if (status == statusTable[i].status)
		{
			popwndData.wd.times = statusTable[i].times;
			if (MyGetProfileDWORD(P("Setting"), P("audio_sound"), 0) != 0)
			{
				return NULL;
			}
			return statusTable[i].webName;
		}
	}
	return NULL;
}

int popCount = 0;

int OnCreate(HWND hWnd)
{
	POPWND_DATA& popwndData = *(POPWND_DATA*)GetWindowLong(hWnd, GWLP_USERDATA);

	return 0;
}

void OnDestroy(HWND hWnd)
{
	CloseSound(hWnd);

	POPWND_DATA& popwndData = *(POPWND_DATA*)GetWindowLong(hWnd, GWLP_USERDATA);

	CloseWindow(popwndData.hWndBtClose);

	popCount--;
}

int OnInitialize(HWND hWnd)
{
	POPWND_DATA& popwndData = *(POPWND_DATA*)GetWindowLong(hWnd, GWLP_USERDATA);

	RECT rect = { 0 };
	GetClientRect(hWnd, &rect);
	rect.right -= 16;
	rect.bottom -= 16;
	rect.left = rect.right - 100;
	rect.top = rect.bottom - 30;
	popwndData.hWndBtClose = CreateWindow("BUTTON", "閉じる", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hWnd, NULL, popwndData.hInst, NULL);

	if (popwndData.showSeconds)
	{
		SetTimer(hWnd, 2, popwndData.showSeconds * 1000, NULL);
	}

	LPCTSTR wavName = GetWavName(hWnd, popwndData.status);
	if (wavName == NULL) {
		//Todo beep音を出す。
	}
	else if (strlen(wavName) != 0)
	{
		CoString modulePath = GetModuleFolder();
		CoString path = modulePath + wavName;
		if (PlaySound(hWnd, path.GetBuffer(0)) != 0)
		{
			HRESULT PlayBeepSound(DWORD times = 1);
			PlayBeepSound(popwndData.wd.times);
		}
	}

	return 0;
}

LRESULT OnWomDone(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POPWND_DATA& popwndData = *(POPWND_DATA*)GetWindowLong(hWnd, GWLP_USERDATA);

	if (popwndData.wd.times > 0)
	{
		waveOutWrite((HWAVEOUT)wParam, (LPWAVEHDR)lParam, sizeof WAVEHDR);
		popwndData.wd.times--;
	}

	return 0;
}

void OnClose(HWND hWnd)
{
	DestroyWindow(hWnd);
}

struct COLOR_USER
{
	COLORREF color[5];
};

static const COLOR_USER colorTable[] = {
	{ RGB(255, 255, 255),  RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255) },
	{ RGB(255, 220, 220),  RGB(255, 190, 190), RGB(255, 160, 160), RGB(255, 130, 130), RGB(255, 100, 100) },
	{ RGB(220, 255, 220),  RGB(190, 255, 190), RGB(160, 255, 160), RGB(130, 255, 130), RGB(100, 255, 100) },
	{ RGB(220, 220, 255),  RGB(190, 190, 255), RGB(160, 160, 255), RGB(130, 130, 255), RGB(100, 100, 255) },
	{ RGB(220, 255, 255),  RGB(190, 255, 255), RGB(160, 255, 255), RGB(130, 255, 255), RGB(100, 255, 255) },
	{ RGB(255, 255, 220),  RGB(255, 255, 190), RGB(255, 255, 160), RGB(255, 255, 130), RGB(255, 255, 100) },
	{ RGB(255, 220, 255),  RGB(255, 190, 255), RGB(255, 160, 255), RGB(255, 130, 255), RGB(255, 100, 255) },
	{ RGB(180, 180, 180),  RGB(180, 180, 180), RGB(180, 180, 180), RGB(180, 180, 180), RGB(180, 180, 180) },
	{ RGB(220, 220, 220),  RGB(220, 220, 220), RGB(220, 220, 220), RGB(220, 220, 220), RGB(220, 220, 220) },
};

void OnPaint(HWND hWnd, HDC hDC = NULL)
{
	CPaintDC paintDC(CWnd::FromHandle(hWnd));

	CDC* dc = hDC ? CDC::FromHandle(hDC) : &paintDC;

	static DWORD sizeColor = sizeof colorTable / sizeof COLOR_USER;

	POPWND_DATA& popwndData = *(POPWND_DATA*)GetWindowLong(hWnd, GWLP_USERDATA);

	if (popwndData.color >= sizeColor)
	{
		popwndData.color = sizeColor - 1;
	}

	CoRect rect;
	GetClientRect(hWnd, rect);
	dc->FillSolidRect(rect, colorTable[popwndData.color].color[2]);
	rect.DeflateRect(1, 1, 1, 1);
	dc->FillSolidRect(rect, colorTable[popwndData.color].color[3]);
	rect.DeflateRect(1, 1, 1, 1);
	dc->FillSolidRect(rect, colorTable[popwndData.color].color[4]);
	rect.DeflateRect(1, 1, 1, 1);
	dc->FillSolidRect(rect, colorTable[popwndData.color].color[3]);
	rect.DeflateRect(1, 1, 1, 1);
	dc->FillSolidRect(rect, colorTable[popwndData.color].color[2]);
	rect.DeflateRect(1, 1, 1, 1);
	dc->FillSolidRect(rect, colorTable[popwndData.color].color[1]);
	rect.DeflateRect(1, 1, 1, 1);
	dc->FillSolidRect(rect, colorTable[popwndData.color].color[0]);
	rect.DeflateRect(2, 2, 2, 2);

//	CFont* pFont = dc->SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));

	LPCTSTR pText = popwndData.text;

	CoRect calcRect = rect;
	dc->DrawText(pText, calcRect, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);
	rect.DeflateRect((rect.Width() - calcRect.Width()) / 2, (rect.Height() - calcRect.Height()) / 2);
	dc->DrawText(pText, rect, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);

//	dc->SelectObject(pFont);
}

void OnTimer(HWND hWnd, UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		KillTimer(hWnd, nIDEvent);
	}
	else if (nIDEvent == 2)
	{
		KillTimer(hWnd, nIDEvent);
		OnClose(hWnd);
	}
}

void OnClicked(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	PostMessage(hWnd, WM_CLOSE, 0, 0);

	POPWND_DATA& popwndData = *(POPWND_DATA*)GetWindowLong(hWnd, GWLP_USERDATA);

	SHELLEXECUTEINFO sei = { sizeof SHELLEXECUTEINFO };
	sei.lpFile = popwndData.link;
	ShellExecuteEx(&sei);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POPWND_DATA& popwndData = *(POPWND_DATA*)GetWindowLong(hWnd, GWLP_USERDATA);

	int wmId, wmEvent;

	switch (uMsg)
	{
	case WM_CREATE:
		OnCreate(hWnd);
		return 0;
	
	case WM_DESTROY:
		OnDestroy(hWnd);
		break;

	case MM_WOM_DONE:
		OnWomDone(hWnd, wParam, lParam);
		return 0;

	case WM_CLOSE:
		OnClose(hWnd);
		return 0;

	case WM_PRINTCLIENT:
		OnPaint(hWnd, (HDC)wParam);
		return 0;

	case WM_PAINT:
		OnPaint(hWnd);
		return 0;

	case WM_TIMER:
		OnTimer(hWnd, wParam);
		return 0;

	case WM_LBUTTONUP:
		OnClicked(hWnd, wParam, lParam);
		return 0;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		if (wmEvent == BN_CLICKED && popwndData.hWndBtClose == (HWND)lParam)
		{
			OnClose(hWnd);
		}

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void RenderingLimits(DWORD& targ, DWORD min, DWORD max)
{
	if (targ < min)
	{
		targ = min;
	}
	else if (targ > max)
	{
		targ = max;
	}
}

int RegisterClass(HINSTANCE hinst, LPCTSTR szClassName)
{
	WNDCLASSEX wc = { sizeof WNDCLASSEX };
	wc.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wc.hInstance = hinst;
	wc.lpfnWndProc = WindowProc;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = szClassName;

	if (!RegisterClassEx(&wc))
	{
		OutputViewer("ERROR: RegisterClassEx failed.");
		return -1;
	}

	return 0;
}

int ShowPopupWindow(HINSTANCE hinst, int nCmdShow, DWORD status, LPCTSTR lpszText)
{
	CRect rect;
	SystemParametersInfo(SPI_GETWORKAREA, sizeof rect, &rect, 0);
	if (rect.right <= 800)
	{
		return -1;
	}

	static const TCHAR* szClassName = P("plustar_popupwindow_class");
	static const TCHAR* szWindowName = P("PLUSTAR INFORMATION");
	static int regclass = RegisterClass(hinst, szClassName);

	CoString text = lpszText;

	POPWND_DATA& popwndData = *(new POPWND_DATA);

	INT pos = 0;
	popwndData.hWnd = NULL;
	popwndData.status = status;
	popwndData.color = strtoul(text.AbstractSearchSubstance(pos), NULL, 10);
	popwndData.width = strtoul(text.AbstractSearchSubstance(pos), NULL, 10) + 16;
	popwndData.height = strtoul(text.AbstractSearchSubstance(pos), NULL, 10) + 38;
	popwndData.link = text.AbstractSearchSubstance(pos);
	popwndData.text = text.AbstractSearchSubstance(pos);
	popwndData.showSeconds = strtoul(text.AbstractSearchSubstance(pos), NULL, 10);
	popwndData.hInst = hinst;

	RenderingLimits(popwndData.width, 150, 700);
	RenderingLimits(popwndData.height, 100, 500);

	DWORD colorCount = sizeof colorTable / sizeof COLOR_USER;
	if (popwndData.color > colorCount - 1)
	{
		popwndData.color = 0;
	}

	DWORD statusCount = sizeof statusTable / sizeof STATUS_DATA;
	if (popwndData.status > statusCount - 1)
	{
		popwndData.status = 0;
	}

	HWND hWndParent = GetDesktopWindow();

	{
		static CCriticalSection _crit;
		CoCriticalSectionCtrl _ctrl(_crit);

		rect.bottom -= popCount * 200;
		popCount++;

		popwndData.hWnd = CreateWindowEx(WS_EX_TOPMOST, szClassName, szWindowName, WS_POPUP,
				rect.right - (popwndData.width + 3), rect.bottom - (popwndData.height + 3),
				popwndData.width, popwndData.height, hWndParent, NULL, hinst, NULL);
		if (popwndData.hWnd == NULL)
		{
			OutputViewer("ERROR: CreateWindowEx failed.");
			return -1;
		}

		SetWindowLong(popwndData.hWnd, GWLP_USERDATA, (LONG)&popwndData);

		OnInitialize(popwndData.hWnd);

		UpdateWindow(popwndData.hWnd);

		AnimateWindow(popwndData.hWnd, 1000, AW_VER_NEGATIVE);
	}

	MSG msg = { 0 };
	for (; GetMessage(&msg, popwndData.hWnd, 0, 0) > 0; )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

} // namespace PopupWnd

struct MSGBOXDATA
{
	CoString text;
	DWORD status;

	MSGBOXDATA(LPCTSTR _text, DWORD _status)
		: text(_text)
		, status(_status)
	{
	}
};

DWORD WINAPI PopupMessageBoxThread(LPVOID pVoid)
{
	MSGBOXDATA* pData = (MSGBOXDATA*)pVoid;

	HANDLE hProcess = GetCurrentProcess();
	HMODULE hModule = NULL;
	DWORD mSize = 0;
	EnumProcessModules(hProcess, &hModule, sizeof HMODULE, &mSize);
	HINSTANCE hInstance = hModule;

	PopupWnd::ShowPopupWindow(hInstance, SW_SHOWNOACTIVATE, pData->status, pData->text);

	delete pData;

	return 0;
}

DWORD PopupMessageBox(DWORD status, LPCTSTR lpszText)
{
	MSGBOXDATA* pData = new MSGBOXDATA(lpszText, status);

	HANDLE hThread = CreateThread(NULL, 0, PopupMessageBoxThread, pData, 0, NULL);
	if (!hThread)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	CloseHandle(hThread);

	return 0;
}
