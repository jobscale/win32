///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//
// $Date: $
// $Rev: $
// $Author: $
// $HeadURL: $
//
// $Id: $
//

// pPopMaker.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "pPopMaker.h"

const LPCTSTR GetModuleFolder()
{
	static CComBSTR _moduleFolder(_MAX_PATH, "");
	static LPTSTR moduleFolder = (LPTSTR)_moduleFolder.m_str;
	if (strlen(moduleFolder) == 0)
	{
		CComBSTR _drive(_MAX_DRIVE, "");
		LPTSTR drive = (LPTSTR)_drive.m_str;
		CComBSTR _dir(_MAX_DIR, "");
		LPTSTR dir = (LPTSTR)_dir.m_str;
		CComBSTR _fname(_MAX_FNAME, "");
		LPTSTR fname = (LPTSTR)_fname.m_str;
		CComBSTR _ext(_MAX_EXT, "");
		LPTSTR ext = (LPTSTR)_ext.m_str;

		GetModuleFileName(NULL, moduleFolder, _MAX_PATH);
		_splitpath_s(moduleFolder, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
		strcpy(moduleFolder, drive);
		strcat(moduleFolder, dir);
	}
	return moduleFolder;
}

#define MAX_LOADSTRING 100

struct RESOUCE_DATA
{
	HINSTANCE hInst;
	TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
	TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名
	HWND hWnd;

	HWND hWndEdTimeLimit;
	HWND hWndEdStatus;
	HWND hWndEdColor;
	HWND hWndEdWidth;
	HWND hWndEdHeight;
	HWND hWndEdLink;
	HWND hWndEdText;
	HWND hWndBtTest;
	HWND hWndBtCommit;
	HWND hWndBtUpdate;
	HWND hWndBtEmergencyQuake;
	HWND hWndBtEmergencyFire;
	HWND hWndBtEmergencyTraining;
};

RESOUCE_DATA& GetResouceData()
{
	static RESOUCE_DATA resData = { 0 };
	return resData;
}

// バージョン情報ボックスのメッセージ ハンドラです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

HRESULT MakeSendData(CComBSTR& sendData, DWORD isBroadcast = 0)
{
	RESOUCE_DATA& resData = GetResouceData();

	CComBSTR _buf(11, "");
	LPTSTR buf = (LPTSTR)_buf.m_str;

	DWORD sizeBuf = GetWindowTextLength(resData.hWndEdStatus);
	GetWindowText(resData.hWndEdStatus, buf, 11);
	DWORD status = strtoul(buf, NULL, 10);

	sizeBuf = GetWindowTextLength(resData.hWndEdColor);
	GetWindowText(resData.hWndEdColor, buf, 11);
	DWORD color = strtoul(buf, NULL, 10);
	if(color < 0 || color > 8)
	{
		color = 1;
	}
	sizeBuf = GetWindowTextLength(resData.hWndEdWidth);
	GetWindowText(resData.hWndEdWidth, buf, 11);
	DWORD width = strtoul(buf, NULL, 10);
	
	sizeBuf = GetWindowTextLength(resData.hWndEdHeight);
	GetWindowText(resData.hWndEdHeight, buf, 11);
	DWORD height = strtoul(buf, NULL, 10);

	RenderingLimits(width, 150, 700);
	RenderingLimits(height, 100, 500);

	DWORD sizeLink = GetWindowTextLength(resData.hWndEdLink);
	CComBSTR _lpszLink(sizeLink + 1);
	LPTSTR lpszLink = (LPTSTR)_lpszLink.m_str;
	GetWindowText(resData.hWndEdLink, lpszLink, sizeLink + 1);

	DWORD sizeText = GetWindowTextLength(resData.hWndEdText);
	CComBSTR _lpszText(sizeText + 1);
	LPTSTR lpszText = (LPTSTR)_lpszText.m_str;
	GetWindowText(resData.hWndEdText, lpszText, sizeText + 1);

	sizeBuf = GetWindowTextLength(resData.hWndEdTimeLimit);
	GetWindowText(resData.hWndEdTimeLimit, buf, 11);
	DWORD showSeconds = strtoul(buf, NULL, 10);

	INT size = sizeLink + sizeText + 1024;
	CComBSTR _cmd(size, "");
	LPTSTR cmd = (LPTSTR)_cmd.m_str;
	sprintf(cmd, "%s%d:\"%d\" \"%d\" \"%d\" \"%s\" \"%s\" \"%d\"",
		isBroadcast ? "popupmessage:" : "popuptestmessage:", status, color, width, height, lpszLink, lpszText, showSeconds);

	sendData = _cmd;

	return 0;
}

LRESULT WriteCommander(LPCTSTR fName, LPCTSTR pCmdData)
{
	RESOUCE_DATA& resData = GetResouceData();

	CComBSTR _path(_MAX_PATH, "");
	LPTSTR path = (LPTSTR)_path.m_str;
	strcpy(path, GetModuleFolder());
	strcat(path, fName);

	HANDLE hFile = CreateFile(path, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxEx(resData.hWnd, "必要なファイルのオープンに失敗しました。", "中断", MB_ICONSTOP, LANG_NEUTRAL);
		return -1;
	}
	else
	{
		DWORD dwWrite = 0;
		WriteFile(hFile, pCmdData, strlen(pCmdData), &dwWrite, NULL);
		CloseHandle(hFile);
	}

	return 0;
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

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RESOUCE_DATA& resData = GetResouceData();

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static DWORD _color = 3;

	switch (message)
	{
	case WM_CTLCOLORMSGBOX   : //           0x0132
	case WM_CTLCOLOREDIT     : //           0x0133
	case WM_CTLCOLORLISTBOX  : //           0x0134
	case WM_CTLCOLORBTN      : //           0x0135
	case WM_CTLCOLORDLG      : //           0x0136
	case WM_CTLCOLORSCROLLBAR: //           0x0137
	case WM_CTLCOLORSTATIC   : //           0x0138
		SetBkMode((HDC)wParam, TRANSPARENT);
		if ((HWND)lParam == resData.hWndEdText)
		{
			DWORD sizeCount = sizeof colorTable / sizeof COLOR_USER;
			if (_color > sizeCount - 1)
			{
				_color = 0;
			}
			HBRUSH hBrush = CreateSolidBrush(colorTable[_color].color[0]);
			return (INT_PTR)hBrush;
		}
		else
		{
			return (INT_PTR)GetStockObject(WHITE_BRUSH);
		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		if (wmEvent == EN_CHANGE)
		{
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wmEvent == EN_CHANGE && (resData.hWndEdWidth == (HWND)lParam || resData.hWndEdHeight == (HWND)lParam))
		{
			CComBSTR _buf(11, "");
			LPTSTR buf = (LPTSTR)_buf.m_str;
			GetWindowText(resData.hWndEdWidth, buf, 10);
			DWORD width = strtoul(buf, NULL, 10);
			GetWindowText(resData.hWndEdHeight, buf, 10);
			DWORD height = strtoul(buf, NULL, 10);
			MoveWindow(resData.hWndEdText, 120, 110, width, height, TRUE);
		}
		else if (wmEvent == EN_CHANGE && resData.hWndEdColor == (HWND)lParam)
		{
			CComBSTR _buf(11, "");
			LPTSTR buf = (LPTSTR)_buf.m_str;
			DWORD sizeBuf = GetWindowTextLength(resData.hWndEdColor);
			GetWindowText(resData.hWndEdColor, buf, 11);

			_color = strtoul(buf, NULL, 10);
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtTest == (HWND)lParam)
		{
			CComBSTR sendData;
			MakeSendData(sendData);

			WriteCommander("pLook.commander", (LPTSTR)sendData.m_str);
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtCommit == (HWND)lParam)
		{
			if (MessageBoxEx(hWnd, "起動しているすべての端末にポップアップメッセージを表示します。\r\n"
				"表示されるメッセージを確認しましたか？\r\n"
				"処理を中断する場合はキャンセルを選んでください。", "警告", MB_OKCANCEL | MB_ICONWARNING, LANG_NEUTRAL) == IDOK)
			{
				CComBSTR sendData;
				MakeSendData(sendData, 1);

				WriteCommander("pLook.commander", (LPTSTR)sendData.m_str);

				// URLを元に戻す
				SetWindowText(resData.hWndEdLink,"http://clip.plustar.jp/");
			}
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtUpdate == (HWND)lParam)
		{
			WriteCommander("pLook.commander", "setupupdatenow:");
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtEmergencyQuake == (HWND)lParam)
		{
			SetWindowText(resData.hWndEdText,
				"ただ今、地震が発生しました。\r\n"
				"この建物は耐震構造になっておりますので安全です。\r\n\r\n"
				"なお倒れ易い物や落下の危険のあるものを避け、\r\n"
				"次のアナウンスがあるまでしばらくお待ち下さい。\r\n"
				"エレベーターは最寄りのフロアで停止しますので\r\n"
				"運行再開の連絡があるまでお待ち下さい。\r\n\r\n"

				"Attention please!\r\n"
				"An earthquake has just occurred.\r\n"
				"This building is earthquake-proof.\r\n"
				"Please keep cool and look out for anything that has fallen or dropped.\r\n"
				"An announcement will follow.\r\n"
			);

			// 値：0 無限表示
			SetWindowText(resData.hWndEdTimeLimit, "0");
			// 値：6(赤)
			SetWindowText(resData.hWndEdColor, "6");
			// 値：400
			SetWindowText(resData.hWndEdWidth, "400");
			// 値：200
			SetWindowText(resData.hWndEdHeight, "300");
			// 値：空
			SetWindowText(resData.hWndEdLink,"");
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtEmergencyFire == (HWND)lParam)
		{
			SetWindowText(resData.hWndEdText,
				"火災が発生しました。火災が発生しました。\r\n\r\n"
				"アナウンスまたは係員の指示に従って、\r\n"
				"非常口、非常階段\r\n"
				"から避難を開始して下さい。\r\n"
				"なお、エレベーターは運行を中止しますので\r\n"
				"絶対にお乗りにならないで下さい。\r\n\r\n"

				"Emergency! Emergency!\r\n"
				"Fire alert! Fire alert!\r\n"
				"Evacuate from here to the exit immediately. \r\n"
				"It is NOT a fire-drill.It is NOT a fire-drill. \r\n"
			);

			// 値：0 無限表示
			SetWindowText(resData.hWndEdTimeLimit, "0");
			// 値：6(赤)
			SetWindowText(resData.hWndEdColor, "6");
			// 値：400
			SetWindowText(resData.hWndEdWidth, "400");
			// 値：200
			SetWindowText(resData.hWndEdHeight, "300");
			// 値：空
			SetWindowText(resData.hWndEdLink,"");
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtEmergencyTraining == (HWND)lParam)
		{
			SetWindowText(resData.hWndEdText,
				"ただ今から、火災を想定した避難訓練を開始します。\r\n\r\n"
				"アナウンス並びに係員の指示に従って、\r\n"
				"非常口、非常階段\r\n"
				"から避難を開始して下さい。\r\n"
				"皆様方の冷静、沈着な行動並びに\r\n"
				"ご協力をお願い申し上げます。\r\n\r\n"

				"Attention please!\r\n"
				"We will begin a fire drill，now.\r\n"
				"An announcement will follow.\r\n"
			);

			// 値：0 無限表示
			SetWindowText(resData.hWndEdTimeLimit, "0");
			// 値：6(赤)
			SetWindowText(resData.hWndEdColor, "2");
			// 値：400
			SetWindowText(resData.hWndEdWidth, "400");
			// 値：200
			SetWindowText(resData.hWndEdHeight, "300");
			// 値：空
			SetWindowText(resData.hWndEdLink,"");
		}
		
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(resData.hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
		EndPaint(hWnd, &ps);
		break;

	case WM_CREATE:
		CreateWindow("STATIC", "ステータス[0-7]:", WS_CHILD | SS_RIGHT, 13, 23, 130, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdStatus = CreateWindow("EDIT", "0", WS_CHILD | WS_BORDER | ES_NUMBER, 150, 20, 60, 25, hWnd, 0, resData.hInst, NULL);
		SendMessage(resData.hWndEdStatus, EM_SETLIMITTEXT, 10, 0);
		// 初期値：20秒表示
		CreateWindow("STATIC", "表示時間[秒数]:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 13, 23, 130, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdTimeLimit = CreateWindow("EDIT", "20", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 150, 20, 60, 25, hWnd, 0, resData.hInst, NULL);
		SendMessage(resData.hWndEdTimeLimit, EM_SETLIMITTEXT, 10, 0);
		// 初期値：3(青)
		CreateWindow("STATIC", "色[0-8]:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 223, 23, 130, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdColor = CreateWindow("EDIT", "3", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 360, 20, 60, 25, hWnd, 0, resData.hInst, NULL);
		SendMessage(resData.hWndEdColor, EM_SETLIMITTEXT, 10, 0);
		// 初期値：300
		CreateWindow("STATIC", "幅[150-700]:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 13, 53, 130, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdWidth = CreateWindow("EDIT", "300", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 150, 50, 60, 25, hWnd, 0, resData.hInst, NULL);
		SendMessage(resData.hWndEdWidth, EM_SETLIMITTEXT, 10, 0);
		// 初期値：250
		CreateWindow("STATIC", "高さ[100-500]:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 223, 53, 130, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdHeight = CreateWindow("EDIT", "250", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 360, 50, 60, 25, hWnd, 0, resData.hInst, NULL);
		SendMessage(resData.hWndEdHeight, EM_SETLIMITTEXT, 10, 0);

		CreateWindow("STATIC", "リンク:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 13, 83, 100, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdLink = CreateWindow("EDIT", "http://clip.plustar.jp/", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 120, 80, 400, 25, hWnd, 0, resData.hInst, NULL);
		CreateWindow("STATIC", "テキスト:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 13, 113, 100, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdText = CreateWindow("EDIT", "ネットワーク障害にお困りの方\r\n"
			"パソコンの不正使用にお困りの方\r\n\r\n"
			"プラスターが解決できるかも知れません\r\n"
			"ここをクリックして\r\n"
			"プラスターを覘いてみてください\r\n\r\n"
			"http://www.plustar.jp/", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_MULTILINE, 120, 110, 300, 300, hWnd, 0, resData.hInst, NULL);

		resData.hWndBtTest = CreateWindow("BUTTON", "テスト", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 140, 100, 25, hWnd, 0, resData.hInst, NULL);
		resData.hWndBtCommit = CreateWindow("BUTTON", "送信", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 170, 100, 25, hWnd, 0, resData.hInst, NULL);
#ifdef _DEBUG
		resData.hWndBtUpdate = CreateWindow("BUTTON", "自動更新", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 200, 100, 25, hWnd, 0, resData.hInst, NULL);
#endif
		resData.hWndBtEmergencyFire     = CreateWindow("BUTTON", "火災発生", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 230, 100, 25, hWnd, 0, resData.hInst, NULL);
		resData.hWndBtEmergencyQuake    = CreateWindow("BUTTON", "地震発生", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 260, 100, 25, hWnd, 0, resData.hInst, NULL);
		resData.hWndBtEmergencyTraining = CreateWindow("BUTTON", "訓練"    , WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 290, 100, 25, hWnd, 0, resData.hInst, NULL);

		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	RESOUCE_DATA& resData = GetResouceData();

	WNDCLASSEX wcex = { sizeof WNDCLASSEX };

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PPOPMAKER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PPOPMAKER);
	wcex.lpszClassName	= resData.szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PPOPMAKER));

	return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	RESOUCE_DATA& resData = GetResouceData();

   resData.hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   resData.hWnd = CreateWindow(resData.szWindowClass, resData.szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!resData.hWnd)
   {
      return FALSE;
   }

   ShowWindow(resData.hWnd, nCmdShow);
   UpdateWindow(resData.hWnd);

   return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	RESOUCE_DATA& resData = GetResouceData();

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, resData.szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PPOPMAKER, resData.szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PPOPMAKER));

	// メイン メッセージ ループ:
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
