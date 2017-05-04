///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

// pExTrapper.cpp : アプリケーションのクラス動作を定義します。
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#include <atlbase.h>

#include "pExTrapper.h"
#include "BaseService.h"

#include "../oCommon/oTools.h"

#define _OFC_EXPORT_
#include "../ofc/oFoundationClassLibrary.h"
#include "../ofc/oString.h"

#include "../pLook/pUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CpExTrapperApp

BEGIN_MESSAGE_MAP(CpExTrapperApp, CWinApp)
END_MESSAGE_MAP()


// CpExTrapperApp コンストラクション

CpExTrapperApp::CpExTrapperApp()
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, "\0 pExTrapperMutex" + 2);
	if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		DEBUG_OUT_DEFAULT("duplicate process failed.");
		TerminateProcess(GetCurrentProcess(), 0);
	}
}

// 唯一の CpExTrapperApp オブジェクトです。

CpExTrapperApp theApp;

DWORD SetupFireWall(BOOL bInst)
{
	DWORD nRet = 0;

	TCHAR pModuleName[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, pModuleName, _MAX_PATH);

	HKEY hKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "\0 SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile\\AuthorizedApplications\\List" + 2, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		if (bInst)
		{
			TCHAR value[_MAX_PATH + 100] = { 0 };
			strcat_s(value, pModuleName);
			strcat_s(value, ":*:Enabled:PLUSTAR_GK");

			if (RegSetValueEx(hKey, pModuleName, NULL, REG_SZ, (LPBYTE)(LPCTSTR)value, (DWORD)strlen(value)) != ERROR_SUCCESS)
			{
				OutputViewer("ERROR InstSender RegSetValueEx Firewall failed.");
				nRet = -1;
			}
		}
		else
		{
			if (RegDeleteValue(hKey, pModuleName) != ERROR_SUCCESS)
			{
				OutputViewer("ERROR InstSender RegDeleteKey Firewall failed.");
				nRet = -1;
			}
		}
		RegCloseKey(hKey);
	}
	else
	{
		OutputViewer("ERROR InstSender RegOpenKeyEx Firewall failed.");
		nRet = -1;
	}

	return nRet;
}

INT SetupTools(LPCTSTR lpszName, LPCTSTR lpszTarget, BOOL bInst = TRUE)
{
	INT nRet = 0;

	CHAR pValue[_MAX_PATH + 2] = { 0 };
	strcat_s(pValue, _MAX_PATH + 2, "\"");
	strcat_s(pValue, _MAX_PATH + 2, lpszTarget);
	strcat_s(pValue, _MAX_PATH + 2, "\"");

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "\0 SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run" + 2, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		if (bInst)
		{
			if (RegSetValueEx(hKey, lpszName, NULL, REG_SZ, (LPBYTE)pValue, (DWORD)strlen(pValue)) != ERROR_SUCCESS)
			{
				OutputViewer("ERROR InstSender RegSetValueEx failed.");
				nRet = -1;
			}
		}
		else
		{
			if (RegDeleteValue(hKey, lpszName) != ERROR_SUCCESS)
			{
				OutputViewer("ERROR InstSender RegDeleteKey failed.");
				nRet = -1;
			}
		}
		RegCloseKey(hKey);
	}
	else
	{
		OutputViewer("ERROR InstSender RegOpenKeyEx failed.");
		nRet = -1;
	}

	return nRet;
}

// CpExTrapperApp 初期化

BOOL CpExTrapperApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof InitCtrls;
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	// SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	CBaseService _service("Plustar eXtreme tRapper");

	LPCTSTR pCmdLine = GetCommandLine();

	if (strstr(pCmdLine, "uninstall") != 0)
	{
		SetupFireWall(FALSE);

		CoString name = "pTools.exe";
		CoString path = GetModuleFolder() + name;
		SetupTools(name, path, FALSE);

		//name = "pPower.exe";
		//path = GetModuleFolder() + name;
		//SetupTools(name, path, FALSE);

		_service.Uninstall();

		// pTools を強制終了
		HWND hWnd = FindWindow("\0 plus32 wndClass pTools" + 2, "\0 goode's checker pTools" + 2);
		if (hWnd)
		{
			DWORD pid = 0;
			GetWindowThreadProcessId(hWnd, &pid);
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
			if (hProcess)
			{
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}
		}

		// pTools をシャットダウン
		if (IsWindow(hWnd))
		{
			PostMessage(hWnd, WM_USER + 202, *LPDWORD("PTLS"), *LPDWORD("KILL"));
		}

		TerminateProcess(GetCurrentProcess(), 0);
	}
	else if (strstr(pCmdLine, "install") != 0)
	{
		_asm
		{
			int 3
		}
		SetupFireWall(TRUE);

		CoString name = "pTools.exe";
		CoString path = GetModuleFolder() + name;
		SetupTools(name, path, TRUE);

		//name = "pPower.exe";
		//path = GetModuleFolder() + name;
		//SetupTools(name, path, TRUE);

		_service.Install();

		TerminateProcess(GetCurrentProcess(), 0);
	}

	HMODULE hModule = LoadLibrary("pLook");

	if (IsSuperDebug())
	{
		if (_service.OnInit())
		{
			_service.StartRun(); // nothing more to do
		}
	}
    else if (1 || _service.IsInstalled())
	{
		_service.StartService();
    }

	TerminateProcess(GetCurrentProcess(), 0);

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}
