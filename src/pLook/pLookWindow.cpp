// pLookWindow.cpp : アプリケーションのエントリ ポイントを定義します。
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

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "pLogging.h"

#include <atlcomtime.h>

#include <psapi.h>
#pragma comment(lib, "psapi")

BOOL InitApplication()
{
	CoString moduleFileName;
	GetModuleFileName(NULL, moduleFileName.GetBuffer(_MAX_PATH), _MAX_PATH);
	moduleFileName.ReleaseBuffer();
	CoString drive, dir, fname, ext;
	_splitpath(moduleFileName, drive.GetBuffer(_MAX_DRIVE), dir.GetBuffer(_MAX_DIR), fname.GetBuffer(_MAX_FNAME), ext.GetBuffer(_MAX_EXT));
	drive.ReleaseBuffer(); dir.ReleaseBuffer(); fname.ReleaseBuffer(); ext.ReleaseBuffer();

	if (fname.CompareNoCase("pExTrapper") != 0)
	{
		DEBUG_OUT_DEFAULT("unauthorized permission failed.");
		return TRUE;
	}

	if (!EnablePrivilege(SE_RESTORE_NAME))
	{
		DEBUG_OUT_DEFAULT("unauthorized permission failed.");
		MessageBox(NULL, "権限のない実行環境です。", "ERROR", MB_ICONERROR | MB_OK);
		OutputViewer("TerminateProcess:%s:(%d)", __FILE__, __LINE__);
		TerminateProcess(GetCurrentProcess(), 0);
	}

	HMODULE LoadIMGCTL();
	if (!LoadIMGCTL())
	{
		DEBUG_OUT_DEFAULT("imgctl unauthorized permission failed.");
		return TRUE;
	}

	HMODULE GetSpiceDLL();
	if (!GetSpiceDLL())
	{
		DEBUG_OUT_DEFAULT("spice unauthorized permission failed.");
		return TRUE;
	}

	DWORD WINAPI ProcessAttachThread(LPVOID pParam);
	HANDLE hThread = CreateThread(NULL, 0, ProcessAttachThread, NULL, 0, NULL);
	if (!hThread)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return FALSE;
	}
	CloseHandle(hThread);
	Sleep(1000);

	return TRUE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	if (!InitApplication())
	{
		return FALSE;
	}

	return TRUE;
}

DWORD WINAPI MainFunction(LPVOID pParam)
{
	CWSAInterface wsa;

	HANDLE hProcess = GetCurrentProcess();
	HMODULE hModule = NULL;
	DWORD mSize = 0;
	EnumProcessModules(hProcess, &hModule, sizeof HMODULE, &mSize);
	HINSTANCE hInstance = hModule;

	// アプリケーションの初期化を実行します:
	if (!InitInstance(hInstance, SW_SHOWNORMAL))
	{
		OutputViewer("TerminateProcess:%s:(%d)", __FILE__, __LINE__);
		TerminateProcess(GetCurrentProcess(), -1);
		return -1;
	}

	return 0;
}
