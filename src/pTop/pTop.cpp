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

// pTop.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "pTop.h"

#include "../oCommon/oBase64.h"

#include <Winuser.h>
#include <shlobj.h>

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
bool DoesFileExist( LPCTSTR lpFileName );

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PTOP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, SW_HIDE))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PTOP));

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
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_CLASSDC;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PTOP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; // MAKEINTRESOURCE(IDC_PTOP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PTOP));

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
   HWND hWnd;

	hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

	hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 128, LWA_ALPHA);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

#pragma warning(disable:4996)

const LPCTSTR GetModuleFolder(DWORD flg = 0)
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
		if (flg & 1)
		{
			strcat(moduleFolder, fname);
		}
	}
	return moduleFolder;
}

// 画面 ハンドラです。
INT_PTR CALLBACK Monitor(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
			SetWindowLong(hDlg, GWL_STYLE, 0);

			SetLayeredWindowAttributes(hDlg, RGB(0, 0, 0), 172, LWA_ALPHA);

			int cx = GetSystemMetrics(SM_CXSCREEN);
			int cy = GetSystemMetrics(SM_CYSCREEN);
			MoveWindow(hDlg, 0, 0, cx, cy, FALSE);

			CString store_name   = "プラスター・デモ店";
			CString company_name = "（株）プラスター";

			// ファイルから店舗名、会社名を摘出
			CString path = GetModuleFolder(1);
			path += ".evs"; // Encription Verify System

			CString readData;

			HANDLE hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				CString buffer;
				DWORD readied = 0;

				DWORD dwReadHigh = 0;
				DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
				for (; ReadFile(hFile, buffer.GetBuffer(dwRead), dwRead, &readied, NULL) && readied; )
				{
					buffer.GetBufferSetLength(readied);
					readData += buffer;
				}

				CloseHandle(hFile);
			}

			if (!readData.IsEmpty())
			{
				CString buffer;
				base64_decode(readData, buffer, 1);
				INT left = buffer.Find("\r\n");
				if (left != -1)
				{
					INT right = buffer.GetLength() - left - strlen("\r\n");
					store_name = buffer.Left(left);
					company_name = buffer.Right(right);
				}
			}

			// Static 1/6-5/6 Align 1/10-5/6 Center
			SetWindowPos(GetDlgItem(hDlg, IDC_TEXT), 0, cx / 6, cy / 12, cx * 4 / 6, cy * 9 / 12, SWP_NOZORDER);
			SetWindowText(GetDlgItem(hDlg, IDC_TEXT), 
				"端末使用許諾\r\n" 
				"\r\n" 
				+ store_name + "ではインターネット、パソコン、プリンタ、その他機器が正常に動作することの保障はいたしかねます。"
				"障害の復旧、不具合の解消には全力且つ最善を尽くします。ご理解とご協力お願い申し上げます。 \r\n" 
				"\r\n" 
				+ store_name + "のパソコンは個人情報保護の観点からシャットダウン・再起動でご利用前の状態にパソコンを復元します。" 
				"保存したデータも復元されますのでお客様の責任で大切なデータは外部メディアに保存してください。\r\n" 
				"個人情報の取り扱いはに十分に注意してください。\r\n" 
				"\r\n" 
				+ store_name + "では他のお客様にご迷惑がかかる行為があったとスタッフが判断した場合、直ちにご利用を中止させていただく場合もございます。 \r\n" 
				"\r\n" 
				+ store_name + "ではインターネット犯罪の撲滅に努めております。" 
				"その為に、法令に基づく端末使用状況の開示要求があれば、下記情報を開示いたします。\r\n" 
				"　・店内カメラの録画映像\r\n" 
				"　・来店履歴\r\n" 
				"　・端末利用履歴\r\n" 
				"\r\n" 
				"以上をご同意の上、端末をご利用頂きますようお願い申し上げます。\r\n" 
				+ company_name + " " + store_name + " 店長"
			);

			// Buttom 5/6 Align Center
			int buttonWidth = cx / 8;
			SetWindowPos(GetDlgItem(hDlg, IDC_AGREE)   , 0, (cx / 2) - buttonWidth - 30, cy * 5 / 6 + 20, buttonWidth, 40, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hDlg, IDC_NOTAGREE), 0, (cx / 2) + 30, cy * 5 / 6 + 20, buttonWidth, 40, SWP_NOZORDER);
		}
		return (INT_PTR)TRUE;

	case WM_CTLCOLORMSGBOX   : //           0x0132
	case WM_CTLCOLOREDIT     : //           0x0133
	case WM_CTLCOLORLISTBOX  : //           0x0134
	case WM_CTLCOLORBTN      : //           0x0135
	case WM_CTLCOLORDLG      : //           0x0136
	case WM_CTLCOLORSCROLLBAR: //           0x0137
	case WM_CTLCOLORSTATIC   : //           0x0138
		SetBkColor((HDC)wParam, RGB(0, 0, 0));
		SetBkMode((HDC)wParam, TRANSPARENT);
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (INT_PTR)GetStockObject(BLACK_BRUSH);

	case WM_MOVE:
		{
			RECT rect = { 0 };
			GetWindowRect(hDlg, &rect);
			if (rect.left != 0 || rect.top != 0)
			{
				SetWindowPos(hDlg, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
		}
		return 1;

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			{
				if (LOWORD(wParam) == IDC_AGREE)
				{
					EndDialog(hDlg, LOWORD(wParam));
					PostQuitMessage(0);

					// 入れてみた
					return (INT_PTR)TRUE;
				}
				else if (LOWORD(wParam) == IDC_NOTAGREE)
				{
					if (MessageBox(hDlg, "端末使用許諾に同意いただけない場合は端末のご利用はできません。"
							"\r\n\r\nシステムをシャットダウンします。\r\nよろしいですか？",
							"警告", MB_YESNO|MB_ICONQUESTION) == IDYES)
					{
						HANDLE hToken; 
						TOKEN_PRIVILEGES tkp;

						// Get a token for this process.
						OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES |TOKEN_QUERY, &hToken);
						// Get the LUID for the shutdown privilege.
						LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
						tkp.PrivilegeCount = 1;  // oneprivilege to set    
						tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
						// Get the shutdown privilege for this process.
						AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 

						UINT uFlags = EWX_SHUTDOWN | EWX_POWEROFF| EWX_FORCE;

						// shutdown
						ExitWindowsEx(uFlags, 0);
						return (INT_PTR)TRUE;
					}
					return (INT_PTR)TRUE;
				}
			}
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

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
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	INT_PTR resDlg = 0;

	switch (message)
	{
	case WM_CREATE:
		resDlg = DialogBox(hInst, MAKEINTRESOURCE(IDD_MONITOR), hWnd, Monitor);
		if (resDlg == IDC_AGREE)
		{
			PostQuitMessage(0);
			return 0;
		}
		else if (resDlg == IDC_NOTAGREE)
		{
			*LPDWORD(0) = 0;
			// 
		}
		break;

	case WM_SYSKEYDOWN:
		return 1;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
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
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/*
 * ファイルがあればtrueを返す
 */
bool DoesFileExist( LPCTSTR lpFileName )
{
	bool bRet = false;
	WIN32_FIND_DATA	FindData;

	HANDLE hFile = FindFirstFile( lpFileName, &FindData );
	if ( hFile != INVALID_HANDLE_VALUE )
		bRet = true;
	FindClose( hFile );
	return bRet;
}
