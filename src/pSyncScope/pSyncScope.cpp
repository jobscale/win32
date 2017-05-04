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

// pSyncScope.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "pSyncScope.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

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
	LoadString(hInstance, IDC_PSYNCSCOPE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PSYNCSCOPE));

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

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PSYNCSCOPE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PSYNCSCOPE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   // WS_EX_ACCEPTFILES
   hWnd = CreateWindowEx(WS_EX_ACCEPTFILES, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 640, 490, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

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

LRESULT Replace(LPTSTR text)
{
	const char* find = "\n";
	const char* repr = "\r\n";

	CComBSTR buffer(strlen(text) + 1);
	LPTSTR buf = (LPTSTR)(BSTR)buffer;

	char* src = text;
	char* tar = buf;

	for (char* pos = 0; (pos = strstr(src, find)); )
	{
	int len = pos - src;
	memcpy(tar, src, len);
	tar += len;
	memcpy(tar, repr, strlen(repr));
	tar += strlen(repr);
	src = pos + strlen(find);
	}

	int len = strlen(src);
	memcpy(tar, src, len);
	tar += len;
	tar[0] = 0;

	strcpy(text, buf);

	return 0;
}

int Tail(LPTSTR text, int num)
{
	LPCTSTR find = "\r\n";
	int count = 0;
	for (LPCTSTR pos = text; (pos = strstr(pos, find)) != NULL; count++)
	{
		pos += strlen(find);
	}
	if (count <= num)
	{
		return 0;
	}

	count -= num;
	for (LPCTSTR pos = text; (pos = strstr(pos, find)) != NULL; count--)
	{
		pos += strlen(find);
		if (count == 1)
		{
			size_t length = strlen(pos);
			memcpy(text, pos, length);
			text[length] = 0;
			break;
		}
	}

	return 0;
}

struct WND_LIST
{
	HWND hWndServer;
	HWND hWndServerEdit;
	HWND hWndStatic;
	HWND hWndEdit;
	HWND hWndOutput;
	HWND hWndRsync;
};

struct WND_DATA
{
	WND_LIST* pwl;
	HWND hWnd;
	HANDLE hRead;
};

DWORD WINAPI ReadThread(LPVOID pParam)
{
	WND_DATA* pData = (WND_DATA*)pParam;

	CComBSTR txtbuf(_MAX_PATH * 2);
	LPTSTR txt = (LPTSTR)(BSTR)txtbuf;
	DWORD dwLength = 0;

	for (; ReadFile(pData->hRead, txt, _MAX_PATH, &dwLength, NULL); )
	{
		if (dwLength)
		{
			txt[dwLength] = 0;
			Replace(txt);

			int ndx = GetWindowTextLength(pData->pwl->hWndOutput);
			::SendMessage(pData->pwl->hWndOutput, EM_SETSEL, ndx, ndx);
			::SendMessage(pData->pwl->hWndOutput, EM_REPLACESEL, (WPARAM)0, (LPARAM)txt);

			ndx = GetWindowTextLength(pData->pwl->hWndOutput);
			CComBSTR textbuf(ndx + 1);
			LPTSTR text = (LPTSTR)(BSTR)textbuf;
			GetWindowText(pData->pwl->hWndOutput, text, ndx + 1);

			Tail(text, 6);
			SetWindowText(pData->pwl->hWndOutput, text);
		}
	}

	return 0;
}

DWORD WINAPI RsyncThread(LPVOID pParam)
{
	WND_DATA* pData = (WND_DATA*)pParam;

	// >rsync -avz "192.168.24.51::test/Program Files/doc/" "/cygdrive/c/Program Files/doc/"
	LPCTSTR base = "C:\\";

	CComBSTR error;

	int ndx = GetWindowTextLength(pData->pwl->hWndEdit);
	CComBSTR text(ndx + 1);
	LPTSTR buf = (LPTSTR)(BSTR)text;
	GetWindowText(pData->pwl->hWndEdit, buf, ndx + 1);

	SetWindowText(pData->pwl->hWndOutput, "");

	CComBSTR pathbuf(_MAX_PATH);
	LPTSTR path = (LPTSTR)(BSTR)pathbuf;
	int pos = 0;
	for (LPCTSTR pEnd = NULL; (pEnd = strstr(&buf[pos], "\r\n")) != NULL; )
	{
		int len = pEnd - &buf[pos];

		strncpy(path, &buf[pos], len);
		path[len] = 0;

		pos += len + 2;

		if (strncmp(base, path, min(strlen(base), strlen(path))) != 0)
		{
			error += path;
			error += "\r\n";
			continue;
		}

		len = strlen(&path[strlen(base)]);
		strncpy(path, &path[strlen(base)], len);
		path[len] = 0;
		for (int i = 0; path[i]; i++)
		{
			if (path[i] == '\\') path[i] = '/';
		}

		int ndx = GetWindowTextLength(pData->pwl->hWndServerEdit);
		CComBSTR textbuf(ndx + 1);
		LPTSTR text = (LPTSTR)(BSTR)textbuf;
		GetWindowText(pData->pwl->hWndServerEdit, text, ndx + 1);

		CComBSTR cmdbuf(_MAX_PATH * 3);
		LPSTR cmd = (LPSTR)(BSTR)cmdbuf;
		sprintf(cmd, "psync -avz --timeout=15 \"%s::psync/%s/\" \"/cygdrive/c/%s/\"", text, path, path);

		SECURITY_ATTRIBUTES sa = { sizeof SECURITY_ATTRIBUTES };

		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		HANDLE hWrite = NULL;

		if (!CreatePipe(&pData->hRead, &hWrite, &sa, 0))
		{
			// MessageBox(hWnd, "パイプの作成に失敗しました", "Error", MB_OK);
			error += path;
			error += "\r\n";
			continue;
		}

		BOOL bRes = DuplicateHandle(GetCurrentProcess(), //ソースプロセス
					pData->hRead, //duplicateするハンドル(オリジナルハンドル)
					GetCurrentProcess(), //ターゲットプロセス(行先)
					NULL, //複製ハンドルへのポインタ(コピーハンドル)
					0, //アクセス権
					FALSE, //子供がハンドルを継承するかどうか
					DUPLICATE_SAME_ACCESS);
		if (!bRes) //オプション
		{
			// MessageBox(hWnd, "DuplicateHandle Error", "Error", MB_OK);
			CloseHandle(hWrite);
			CloseHandle(pData->hRead);
			pData->hRead = NULL;
			error += path;
			error += "\r\n";
			continue;
		}

		PROCESS_INFORMATION pi = { 0 };
		STARTUPINFO si = { sizeof STARTUPINFO };

		si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE; // SW_SHOWNORMAL;
		si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		si.hStdOutput = hWrite;
		si.hStdError = GetStdHandle( STD_ERROR_HANDLE );

		if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
		{
			error += path;
			error += "\r\n";
			continue;
		}
		CloseHandle(pi.hThread);

		HANDLE hThread = CreateThread(NULL, 0, ReadThread, pData, 0, NULL);
		CloseHandle(hThread);

		DWORD res = WaitForMultipleObjects(1, &pi.hProcess, TRUE, INFINITE);

		DWORD exitCode = 0;
		GetExitCodeProcess(pi.hProcess, &exitCode);
		if (exitCode != 0)
		{
			/*
				   0      Success
				   1      Syntax or usage error
				   2      Protocol incompatibility
				   3      Errors selecting input/output files, dirs
				   4      Requested  action not supported: an attempt was made to manipulate 64-bit files on a platform 
						  that cannot support them; or an option was specified that is supported by the client and not by the server.
				   5      Error starting client-server protocol
				   6      Daemon unable to append to log-file
				   10     Error in socket I/O
				   11     Error in file I/O
				   12     Error in rsync protocol data stream
				   13     Errors with program diagnostics
				   14     Error in IPC code
				   20     Received SIGUSR1 or SIGINT
				   21     Some error returned by waitpid()
				   22     Error allocating core memory buffers
				   23     Partial transfer due to error
				   24     Partial transfer due to vanished source files
				   25     The --max-delete limit stopped deletions
				   30     Timeout in data send/receive
				   35     Timeout waiting for daemon connectio
			*/
			error += path;
			CComBSTR ewbuf(_MAX_PATH + 1);
			LPTSTR ew = (LPSTR)(BSTR)ewbuf;
			sprintf(ew, " ... (%d) \r\n", exitCode);
			error += ew;
		}

		CloseHandle(pi.hProcess);

	    CloseHandle(hWrite);
	}

	if (error.Length())
	{
		error += "\r\n上記の情報は同期に失敗しました。";
		CW2A msg(error);
		MessageBox(pData->hWnd, msg, "警告", MB_ICONWARNING | MB_OK);
	}
	else
	{
		MessageBox(pData->hWnd, "処理が終了しました。", "情報", MB_ICONINFORMATION | MB_OK);
	}

	PostMessage(pData->hWnd, WM_COMMAND, IDM_RSYNC_END, 0);

	return 0;
}

DWORD Rsync(LPVOID pParam)
{
	HANDLE hThread = CreateThread(NULL, 0, RsyncThread, pParam, 0, NULL);
	CloseHandle(hThread);

	return 0;
}

LRESULT EnableAction(WND_DATA* pData, BOOL nEnable = TRUE)
{
	HMENU hMenu = GetMenu(pData->hWnd);
	for (int i = 0, count = GetMenuItemCount(hMenu); i < count; i++)
	{
		EnableMenuItem(hMenu, i, MF_BYPOSITION | (nEnable ? MF_ENABLED : MF_GRAYED));
	}
	EnableWindow(pData->pwl->hWndServerEdit, nEnable);
	EnableWindow(pData->pwl->hWndEdit, nEnable);
	EnableWindow(pData->pwl->hWndRsync, nEnable);

	return 0;
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

	static WND_LIST wl = { 0 };

	static CComBSTR ipbuf(_MAX_PATH);

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

		case IDM_OPEN:
			{
				CComBSTR path(_MAX_PATH);
				LPTSTR fPath = (LPTSTR)(BSTR)path;
				fPath[0] = 0;

				OPENFILENAME ofn = { sizeof OPENFILENAME };
				ofn.hInstance = hInst;
				ofn.hwndOwner = hWnd;
				ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLESIZING;
				ofn.lpstrFilter = "pSyncScope files (*.pss)\0*.pss\0\0";
				ofn.lpstrFile = fPath;
				ofn.nMaxFile = _MAX_PATH;
				ofn.lpstrTitle = "読み込むファイルを指定してください";

				BOOL bRet = GetOpenFileName(&ofn);
				if (!bRet)
				{
					break;
				}

				HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile == INVALID_HANDLE_VALUE)
				{
					MessageBox(hWnd, "オープンに失敗しました。", "エラー", MB_ICONERROR | MB_OK);
					break;
				}

				DWORD readied = 0;
				DWORD dwReadHigh = 0;
				DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
				CComBSTR buf(dwRead + 1);
				LPTSTR buffer = (LPTSTR)(BSTR)buf;
				if (!ReadFile(hFile, buffer, dwRead, &readied, NULL))
				{
					MessageBox(hWnd, "読込に失敗しました。", "エラー", MB_ICONERROR | MB_OK);
					CloseHandle(hFile);
					break;
				}
				CloseHandle(hFile);

				buffer[readied] = 0;
				SetWindowText(wl.hWndEdit, buffer);
			}
			break;

		case IDM_SAVE:
			{
				CComBSTR path(_MAX_PATH);
				LPTSTR fPath = (LPTSTR)(BSTR)path;
				fPath[0] = 0;

				OPENFILENAME ofn = { sizeof OPENFILENAME };
				ofn.hInstance = hInst;
				ofn.hwndOwner = hWnd;
				ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLESIZING;
				ofn.lpstrFilter = "pSyncScope files (*.pss)\0*.pss\0\0";
				ofn.lpstrFile = fPath;
				ofn.nMaxFile = _MAX_PATH;
				ofn.lpstrTitle = "保存するファイルを指定してください";

				BOOL bRet = GetSaveFileName(&ofn);
				if (!bRet)
				{
					break;
				}

				if (strcmp(".pss", &ofn.lpstrFile[strlen(ofn.lpstrFile) - 4]) != 0)
				{
					strcat(ofn.lpstrFile, ".pss");
				}

				HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile == INVALID_HANDLE_VALUE)
				{
					MessageBox(hWnd, "保存に失敗しました。", "エラー", MB_ICONERROR | MB_OK);
					break;
				}

				int ndx = GetWindowTextLength(wl.hWndEdit);
				CComBSTR text(ndx + 1);
				LPTSTR buf = (LPTSTR)(BSTR)text;
				GetWindowText(wl.hWndEdit, buf, ndx + 1);
				DWORD dataSize = 0;
				WriteFile(hFile, buf, ndx, &dataSize, NULL);
				SetEndOfFile(hFile);
				CloseHandle(hFile);
			}
			break;

		case IDM_RSYNC:
			{
				static WND_DATA data = { 0 };
				data.pwl = &wl;
				data.hWnd = hWnd;

				EnableAction(&data, FALSE);
				Rsync(&data);
			}
			break;

		case IDM_RSYNC_END:
			{
				static WND_DATA data = { 0 };
				data.pwl = &wl;
				data.hWnd = hWnd;

				EnableAction(&data, TRUE);
			}
			break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_CREATE:
		{
			LPCTSTR text = "同期を行うサーバーを選択してください";
			wl.hWndServer = CreateWindow("Static", text, WS_CHILD | WS_VISIBLE, 10, 10, 350, 30, hWnd, NULL, hInst, NULL);
			wl.hWndServerEdit = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | WS_VISIBLE, 10, 40, 350, 30, hWnd, NULL, hInst, NULL);

			wl.hWndRsync = CreateWindow("BUTTON", "同期の実行", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 370, 10, 240, 60, hWnd, (HMENU)IDM_RSYNC, hInst, NULL);

			text = "同期を行うフォルダを選択してください（ドラッグ＆ドロップ機能あり）";
			wl.hWndStatic = CreateWindow("Static", text, WS_CHILD | WS_VISIBLE, 10, 80, 600, 30, hWnd, NULL, hInst, NULL);
			wl.hWndEdit = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL, 10, 110, 600, 200, hWnd, NULL, hInst, NULL);
			wl.hWndOutput = CreateWindow("EDIT", "進行状況：待機\r\n", WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL, 10, 320, 600, 100, hWnd, NULL, hInst, NULL);

			CComBSTR pathbuf(GetModuleFolder(1));
			pathbuf += ".pss";
			CW2A path(pathbuf);

			HANDLE hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD readied = 0;
				DWORD dwReadHigh = 0;
				DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
				CComBSTR buf(dwRead + 1);
				LPTSTR buffer = (LPTSTR)(BSTR)buf;
				if (ReadFile(hFile, buffer, dwRead, &readied, NULL))
				{
					buffer[readied] = 0;

					SetWindowText(wl.hWndEdit, buffer);
				}
				CloseHandle(hFile);
			}

			CComBSTR confbuf(GetModuleFolder(1));
			confbuf += ".conf";
			CW2A conf(confbuf);

			hFile = CreateFile(conf, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD readied = 0;
				DWORD dwReadHigh = 0;
				DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
				CComBSTR buf(dwRead + 1);
				LPTSTR buffer = (LPTSTR)(BSTR)buf;
				if (ReadFile(hFile, buffer, dwRead, &readied, NULL))
				{
					buffer[readied] = 0;

					SetWindowText(wl.hWndServerEdit, buffer);
				}
				CloseHandle(hFile);
			}
		}
		break;

	case WM_DESTROY:
		{
			CComBSTR pathbuf(GetModuleFolder(1));
			pathbuf += ".pss";
			CW2A path(pathbuf);

			HANDLE hFile = CreateFile(path, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				int ndx = GetWindowTextLength(wl.hWndEdit);
				CComBSTR textbuf(ndx + 1);
				LPTSTR text = (LPTSTR)(BSTR)textbuf;
				GetWindowText(wl.hWndEdit, text, ndx + 1);

				DWORD dataSize = 0;
				WriteFile(hFile, text, ndx, &dataSize, NULL);
				SetEndOfFile(hFile);
				CloseHandle(hFile);
			}

			CComBSTR confbuf(GetModuleFolder(1));
			confbuf += ".conf";
			CW2A conf(confbuf);

			hFile = CreateFile(conf, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				int ndx = GetWindowTextLength(wl.hWndServerEdit);
				CComBSTR textbuf(ndx + 1);
				LPTSTR text = (LPTSTR)(BSTR)textbuf;
				GetWindowText(wl.hWndServerEdit, text, ndx + 1);

				DWORD dataSize = 0;
				WriteFile(hFile, text, ndx, &dataSize, NULL);
				SetEndOfFile(hFile);
				CloseHandle(hFile);
			}
		}
		PostQuitMessage(0);
		break;

	case WM_DROPFILES:
		{
			HDROP hDropInfo = (HDROP)wParam;

			CComBSTR strbuf(_MAX_PATH);
			LPTSTR target = (LPTSTR)(BSTR)strbuf;
			for (UINT i = 0, count = DragQueryFile(hDropInfo, UINT_MAX, NULL, NULL); i < count; i++)
			{
				DragQueryFile(hDropInfo, i, target, _MAX_PATH);

				int ndx = GetWindowTextLength(wl.hWndEdit);
				::SendMessage(wl.hWndEdit, EM_SETSEL, ndx, ndx);
				::SendMessage(wl.hWndEdit, EM_REPLACESEL, (WPARAM)0, (LPARAM)target);
				ndx = GetWindowTextLength(wl.hWndEdit);
				::SendMessage(wl.hWndEdit, EM_SETSEL, ndx, ndx);
				::SendMessage(wl.hWndEdit, EM_REPLACESEL, (WPARAM)0, (LPARAM)"\r\n");
			}
		}
		break;

	case WM_SIZE:
		{
			RECT rect = { 0 };
			GetClientRect(hWnd, &rect);
			rect.left += 10;
			rect.top += 80;
			rect.right -= 10;
			rect.bottom -= 10;
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top - 30;
			int height_0 = height * 75 / 100;
			int height_1 = height - height_0 - 10;

			SetWindowPos(wl.hWndStatic, NULL, rect.left, rect.top, width, 30, SWP_NOZORDER);
			SetWindowPos(wl.hWndEdit, NULL, rect.left, rect.top + 30, width, height_0, SWP_NOZORDER);
			SetWindowPos(wl.hWndOutput, NULL, rect.left, rect.top + 30 + height_0 + 10, width, height_1, SWP_NOZORDER);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
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
