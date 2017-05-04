// pSpice.cpp
/////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します
#define _WIN32_WINNT 0x0500
#if !defined(WINVER) || (WINVER < 0x0500)
#undef WINVER
#define WINVER 0x0500
#endif

#include <afxwin.h>

#include <Winsock2.h>
#include <afxdisp.h>

#include "oApiHook.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "../pLook/pProcess.h"
#include "../pLook/pUtility.h"

/////////////////////////////////////////////////////////////////////
// 制御関数
/////////////////////////////////////////////////////////////////////

// ImageDirectoryEntryToDataを使用するために
#pragma comment(lib, "Dbghelp.lib")

///////////////////////////////////////////////////////////////////////////////////////////////////
// 各プロセス共有変数

// 最初にDLLをマッピングしたプロセスの情報
#pragma comment(linker, "/Section:Shared,rws")
#pragma data_seg("Shared")

namespace hook
{
	HHOOK hhook = NULL;
	DWORD processID = 0;
	HANDLE hProcess = NULL;
}
#pragma data_seg()

///////////////////////////////////////////////////////////////////////////////////////////////////
// SetWindowsHookExに渡すプロシージャ関数

static LRESULT WINAPI GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	// CallNextHookExの第1引数は2000/XP以降ならばNULLでもよい
	return CallNextHookEx(hook::hhook, code, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// SetWindowsHookExで全プロセスにフックを実行

extern "C" _declspec(dllexport) int pSpice_Start(DWORD dwThreadId)
{
	// フックハンドルを確認
	if (hook::hhook != NULL)
	{
		return -1;
	}

	// このDLLのハンドルを取得
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if (VirtualQuery(pSpice_Start, &mbi, sizeof mbi) == 0)
	{
		return -1;
	}
	HMODULE hModule = (HMODULE)mbi.AllocationBase;

	hook::processID = GetCurrentProcessId();
	hook::hProcess = GetCurrentProcess();

	// 全プロセスへフックを実行
	hook::hhook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, hModule, dwThreadId);
	if (hook::hhook == NULL)
	{
		return -1;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 全プロセスのフックを解除

extern "C" _declspec(dllexport) int pSpice_Stop()
{
	// フックハンドルを確認
	if (hook::hhook == NULL)
	{
		return -1;
	}

	// UnhookWindowsHookExにてフックを解除
	UnhookWindowsHookEx(hook::hhook);
	hook::hhook = NULL;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ファイル系の監視を行う拡張子を登録

BOOL IsCalling_File(LPCSTR lpFileName)
{
	static CoStringArray ignore_list;
	if (ignore_list.GetSize() == 0)
	{
		ignore_list.Add("\\Documents and Settings\\.*\\Temporary Internet Files\\");
		ignore_list.Add("\\Documents and Settings\\.*\\Local Settings\\Temp\\");
		ignore_list.Add("\\Documents and Settings\\.*\\Application Data\\");
		ignore_list.Add("\\Documents and Settings\\.*\\Cookies\\");
	}

	if (IsFindArray(lpFileName, ignore_list, TRUE))
	{
		return TRUE;
	}

	static CoStringArray not_ignore_list;
	if (not_ignore_list.GetSize() == 0)
	{
		not_ignore_list.Add(".*.doc");
		not_ignore_list.Add(".*.xls");
		not_ignore_list.Add(".*.ppt");
		not_ignore_list.Add(".*.pdf");
		not_ignore_list.Add(".*.txt");
	}

	return !IsFindArray(lpFileName, not_ignore_list, TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 監視ウインドウに情報を送信

static CHAR s_buffer[1000512];

DWORD WINAPI SendSpiceCallingThread(LPVOID pParam)
{
	// メインWindowを検索
	HWND hWnd = FindWindow(NULL, _T("\0 goode's checker pLook" + 2));
	if (hWnd)
	{
		// メインWindowへ通知するデータを作成
		COPYDATASTRUCT cds = { 0 };
		cds.lpData = &(((LPDWORD)pParam)[1]);
		cds.cbData = (((LPDWORD)pParam)[0]);
		SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);
	}

	return 0;
}

// フックしたときの処理
void SendSpiceCalling(DWORD evId, LPCTSTR pFuncName, LPCTSTR pText)
{
	static CoString module;
	if (module.IsEmpty())
	{
		module = CoAPIHook::GetCurrentModuleFileName();
	}
	SYSTEMTIME sysTime = { 0 };
	COleDateTime dt = COleDateTime::GetCurrentTime();
	dt.GetAsSystemTime(sysTime);

	DWORD submitTimeSize = sizeof sysTime;
	DWORD funcNameSize = strlen(pFuncName);
	DWORD moduleSize = module.GetLength();
	DWORD textSize = strlen(pText);
	if (textSize > 1000000) textSize = 1000000;
	DWORD totalSize = sizeof DWORD + submitTimeSize + funcNameSize + moduleSize + textSize + (sizeof DWORD * 5) + 4;

	LPVOID pParam = s_buffer;

	((LPDWORD)pParam)[0] = totalSize;

	LPSTR p = (LPSTR)&((LPDWORD)pParam)[1];
	((LPDWORD)p)[0] = evId;

	p = (LPSTR)&((LPDWORD)p)[1];
	((LPDWORD)p)[0] = submitTimeSize;
	p = (LPSTR)&((LPDWORD)p)[1];
	memcpy(p, &sysTime, submitTimeSize);
	p[submitTimeSize] = '\0';

	p = &p[submitTimeSize + 1];
	((LPDWORD)p)[0] = funcNameSize;
	p = (LPSTR)&((LPDWORD)p)[1];
	memcpy(p, pFuncName, funcNameSize);
	p[funcNameSize] = '\0';

	p = &p[funcNameSize + 1];
	((LPDWORD)p)[0] = moduleSize;
	p = (LPSTR)&((LPDWORD)p)[1];
	memcpy(p, module, moduleSize);
	p[moduleSize] = '\0';

	p = &p[moduleSize + 1];
	((LPDWORD)p)[0] = textSize;
	p = (LPSTR)&((LPDWORD)p)[1];
	memcpy(p, pText, textSize);
	p[textSize] = '\0';

	HANDLE hThread = CreateThread(NULL, 0, SendSpiceCallingThread, pParam, 0, NULL);
	CloseHandle(hThread);
}

// Winsockをフックしたときの処理
void SendWinsockSpiceCalling(LPCTSTR pFuncName, LPCTSTR pText, INT len)
{
	if (len > 512)
	{
		len = 512;
	}
	CoString subText;
	strncpy(subText.GetBufferSetLength(len), pText, len);

	char str[] = " ";
	for (int i = 0, count = 0x1f; i < count; i++)
	{
		*str = i + 1;
		subText.Replace(str, "");
	}

	// 監視ウインドウに通知
	SendSpiceCalling(99011, pFuncName, subText);
}

// 文字列をフックしたときの処理
void SendStringSpiceCalling(LPCTSTR pFuncName, LPCTSTR pText, INT len)
{
	if (len > 512)
	{
		len = 512;
	}
	CoString subText;
	strncpy(subText.GetBufferSetLength(len), pText, len);

	char str[] = " ";
	for (int i = 0, count = 0x1f; i < count; i++)
	{
		*str = i + 1;
		subText.Replace(str, "");
	}

	// 監視ウインドウに通知
	SendSpiceCalling(99012, pFuncName, subText);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 処理関数
///////////////////////////////////////////////////////////////////////////////////////////////////

DWORD GetStealth()
{
	DWORD value = 1;
	HKEY hKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "\0 SOFTWARE\\Plustar\\pLook\\Setting" + 2, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_DWORD;
		DWORD size = sizeof value;
		if (RegQueryValueEx(hKey, "\0 stealth" + 2, NULL, &dwType, (LPBYTE)&value, &size) != ERROR_SUCCESS)
		{
			printf("ERROR stealth RegQueryValueEx failed.\n");
		}
		RegCloseKey(hKey);
	}
	return value;
}

DWORD GetDisablePrint()
{
	DWORD value = 0;
	HKEY hKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "\0 SOFTWARE\\Plustar\\pLook\\Setting" + 2, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_DWORD;
		DWORD size = sizeof value;
		if (RegQueryValueEx(hKey, "\0 disable_print" + 2, NULL, &dwType, (LPBYTE)&value, &size) != ERROR_SUCCESS)
		{
			printf("ERROR disable_print RegQueryValueEx failed.\n");
		}
		RegCloseKey(hKey);
	}
	return value;
}

DWORD GetLoggingMessageBox()
{
	DWORD value = 1;
	HKEY hKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "\0 SOFTWARE\\Plustar\\pLook\\Setting" + 2, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_DWORD;
		DWORD size = sizeof value;
		if (RegQueryValueEx(hKey, "\0 logging_messagebox" + 2, NULL, &dwType, (LPBYTE)&value, &size) != ERROR_SUCCESS)
		{
			printf("ERROR logging_messagebox RegQueryValueEx failed.\n");
		}
		RegCloseKey(hKey);
	}
	return value;
}

DWORD GetSpecialProcessId()
{
	DWORD processId = 0;
	HKEY hKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "\0 SOFTWARE\\Plustar\\pLook\\Process\\pLookTrapper" + 2, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_DWORD;
		DWORD size = sizeof processId;
		if (RegQueryValueEx(hKey, "\0 Special" + 2, NULL, &dwType, (LPBYTE)&processId, &size) != ERROR_SUCCESS)
		{
			printf("ERROR GetSpecialProcessId RegQueryValueEx failed.\n");
		}
		RegCloseKey(hKey);
	}
	return processId;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//=================================================================================================
// 以降に新たにフックしたいAPIを宣言し、置き換える関数を加える
//=================================================================================================
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// DLL Ntdll.dll
//-------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <Winternl.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..NtQuerySystemInformation..

extern CoAPIHook g_Psapi_NtQuerySystemInformation;

typedef NTSTATUS (WINAPI* PFN_Psapi_NtQuerySystemInformation)(
    IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
    OUT PVOID SystemInformation,
    IN ULONG SystemInformationLength,
    OUT PULONG ReturnLength OPTIONAL
	);

NTSTATUS WINAPI Hook_Psapi_NtQuerySystemInformation(
    IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
    OUT PVOID SystemInformation,
    IN ULONG SystemInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    )
{
	NTSTATUS nResult = ((PFN_Psapi_NtQuerySystemInformation)(PROC)g_Psapi_NtQuerySystemInformation)(
		SystemInformationClass,
		SystemInformation,
		SystemInformationLength,
		ReturnLength
		);

	if (SystemInformationClass != SystemProcessInformation)
	{
		return nResult;
	}

	if (FAILED(nResult) || !SystemInformation)
	{
		return nResult;
	}

	if (!GetStealth())
	{
		return nResult;
	}

	DWORD processId = GetSpecialProcessId();

	ULONG count = ReturnLength ? *ReturnLength : SystemInformationLength;

	SYSTEM_PROCESS_INFORMATION* p = (SYSTEM_PROCESS_INFORMATION*)SystemInformation;
	SYSTEM_PROCESS_INFORMATION* pPrev = NULL;
	for (DWORD i = 0; i < count && p; i++)
	{
		if ((DWORD)p->UniqueProcessId == hook::processID || (DWORD)p->UniqueProcessId == processId)
		{
			if (pPrev)
			{
				if (p->NextEntryOffset)
				{
					pPrev->NextEntryOffset += p->NextEntryOffset;
					p->UniqueProcessId = 0;
					p->Reserved2[1] = NULL;
				}
				else
				{
					pPrev->NextEntryOffset = 0;
				}
			}
		}
		pPrev = p;
		p = (SYSTEM_PROCESS_INFORMATION*)(((ULONG_PTR)p) + p->NextEntryOffset);
	}

	return nResult;
}

CoAPIHook g_Psapi_NtQuerySystemInformation("Ntdll.dll", "NtQuerySystemInformation", (PROC)Hook_Psapi_NtQuerySystemInformation);

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// DLL Kernel32.dll
//-------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..CreateProcess..

// 変数定義
extern CoAPIHook g_Kernel32_CreateProcessA;
extern CoAPIHook g_Kernel32_CreateProcessW;

// フックする関数のプロトタイプを定義
typedef BOOL (WINAPI* PFN_Kernel32_CreateProcessA)(
    IN LPCSTR lpApplicationName,
    IN LPSTR lpCommandLine,
    IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN BOOL bInheritHandles,
    IN DWORD dwCreationFlags,
    IN LPVOID lpEnvironment,
    IN LPCSTR lpCurrentDirectory,
    IN LPSTARTUPINFOA lpStartupInfo,
    OUT LPPROCESS_INFORMATION lpProcessInformation
	);
typedef BOOL (WINAPI* PFN_Kernel32_CreateProcessW)(
    IN LPCWSTR lpApplicationName,
    IN LPWSTR lpCommandLine,
    IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN BOOL bInheritHandles,
    IN DWORD dwCreationFlags,
    IN LPVOID lpEnvironment,
    IN LPCWSTR lpCurrentDirectory,
    IN LPSTARTUPINFOW lpStartupInfo,
    OUT LPPROCESS_INFORMATION lpProcessInformation
	);

// 置き換わる関数定義
BOOL WINAPI Hook_Kernel32_CreateProcessA(
    IN LPCSTR lpApplicationName,
    IN LPSTR lpCommandLine,
    IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN BOOL bInheritHandles,
    IN DWORD dwCreationFlags,
    IN LPVOID lpEnvironment,
    IN LPCSTR lpCurrentDirectory,
    IN LPSTARTUPINFOA lpStartupInfo,
    OUT LPPROCESS_INFORMATION lpProcessInformation
	)
{
	if (!IsSuppressionRun(lpCommandLine))
	{
		MessageBox(NULL, "指定したプロセスの実行は禁止されています。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	if (!IsRequiredRun(lpCommandLine))
	{
		MessageBox(NULL, "動作環境が正常ではありません。\r\n指定したプロセスを起動することができません。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_CreateProcessA)(PROC)g_Kernel32_CreateProcessA)(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
		);

	return nResult;
}

BOOL WINAPI Hook_Kernel32_CreateProcessW(
    IN LPCWSTR lpApplicationName,
    IN LPWSTR lpCommandLine,
    IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN BOOL bInheritHandles,
    IN DWORD dwCreationFlags,
    IN LPVOID lpEnvironment,
    IN LPCWSTR lpCurrentDirectory,
    IN LPSTARTUPINFOW lpStartupInfo,
    OUT LPPROCESS_INFORMATION lpProcessInformation
	)
{
	USES_CONVERSION;

	if (!IsSuppressionRun(W2A(lpCommandLine)))
	{
		MessageBox(NULL, "指定したプロセスの実行は禁止されています。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	if (!IsRequiredRun(W2A(lpCommandLine)))
	{
		MessageBox(NULL, "動作環境が正常ではありません。\r\n指定したプロセスを起動することができません。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_CreateProcessW)(PROC)g_Kernel32_CreateProcessW)(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
		);

	return nResult;
}

CoAPIHook g_Kernel32_CreateProcessA("Kernel32.dll", "CreateProcessA", (PROC)Hook_Kernel32_CreateProcessA);
CoAPIHook g_Kernel32_CreateProcessW("Kernel32.dll", "CreateProcessW", (PROC)Hook_Kernel32_CreateProcessW);

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..CreateProcessAsUser..

// 変数定義
extern CoAPIHook g_Kernel32_CreateProcessAsUserA;
extern CoAPIHook g_Kernel32_CreateProcessAsUserW;

// フックする関数のプロトタイプを定義
typedef BOOL (WINAPI* PFN_Kernel32_CreateProcessAsUserA)(
	IN HANDLE hToken,
	IN LPCSTR lpApplicationName,
	IN LPSTR lpCommandLine,
	IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
	IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
	IN BOOL bInheritHandles,
	IN DWORD dwCreationFlags,
	IN LPVOID lpEnvironment,
	IN LPCSTR lpCurrentDirectory,
	IN LPSTARTUPINFOA lpStartupInfo,
	OUT LPPROCESS_INFORMATION lpProcessInformation
	);
typedef BOOL (WINAPI* PFN_Kernel32_CreateProcessAsUserW)(
	IN HANDLE hToken,
	IN LPCWSTR lpApplicationName,
	IN LPWSTR lpCommandLine,
	IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
	IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
	IN BOOL bInheritHandles,
	IN DWORD dwCreationFlags,
	IN LPVOID lpEnvironment,
	IN LPCWSTR lpCurrentDirectory,
	IN LPSTARTUPINFOW lpStartupInfo,
	OUT LPPROCESS_INFORMATION lpProcessInformation
	);

// 置き換わる関数定義
BOOL WINAPI Hook_Kernel32_CreateProcessAsUserA(
	IN HANDLE hToken,
	IN LPCSTR lpApplicationName,
	IN LPSTR lpCommandLine,
	IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
	IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
	IN BOOL bInheritHandles,
	IN DWORD dwCreationFlags,
	IN LPVOID lpEnvironment,
	IN LPCSTR lpCurrentDirectory,
	IN LPSTARTUPINFOA lpStartupInfo,
	OUT LPPROCESS_INFORMATION lpProcessInformation
	)
{
	if (!IsSuppressionRun(lpCommandLine))
	{
		MessageBox(NULL, "指定したプロセスの実行は禁止されています。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	if (!IsRequiredRun(lpCommandLine))
	{
		MessageBox(NULL, "動作環境が正常ではありません。\r\n指定したプロセスを起動することができません。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_CreateProcessAsUserA)(PROC)g_Kernel32_CreateProcessAsUserA)(
		hToken,
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
		);

	return nResult;
}

BOOL WINAPI Hook_Kernel32_CreateProcessAsUserW(
	IN HANDLE hToken,
	IN LPCWSTR lpApplicationName,
	IN LPWSTR lpCommandLine,
	IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
	IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
	IN BOOL bInheritHandles,
	IN DWORD dwCreationFlags,
	IN LPVOID lpEnvironment,
	IN LPCWSTR lpCurrentDirectory,
	IN LPSTARTUPINFOW lpStartupInfo,
	OUT LPPROCESS_INFORMATION lpProcessInformation
	)
{
	USES_CONVERSION;

	if (!IsSuppressionRun(W2A(lpCommandLine)))
	{
		MessageBox(NULL, "指定したプロセスの実行は禁止されています。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	if (!IsRequiredRun(W2A(lpCommandLine)))
	{
		MessageBox(NULL, "動作環境が正常ではありません。\r\n指定したプロセスを起動することができません。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_CreateProcessAsUserW)(PROC)g_Kernel32_CreateProcessAsUserW)(
		hToken,
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
		);

	return nResult;
}

CoAPIHook g_Kernel32_CreateProcessAsUserA("Kernel32.dll", "CreateProcessAsUserA", (PROC)Hook_Kernel32_CreateProcessAsUserA);
CoAPIHook g_Kernel32_CreateProcessAsUserW("Kernel32.dll", "CreateProcessAsUserW", (PROC)Hook_Kernel32_CreateProcessAsUserW);

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..CreateProcessWith..

// 変数定義
extern CoAPIHook g_Kernel32_CreateProcessWithLogonW;
extern CoAPIHook g_Kernel32_CreateProcessWithTokenW;

// フックする関数のプロトタイプを定義
typedef BOOL (WINAPI* PFN_Kernel32_CreateProcessWithLogonW)(
	__in        LPCWSTR lpUsername,
	__in_opt    LPCWSTR lpDomain,
	__in        LPCWSTR lpPassword,
	__in        DWORD dwLogonFlags,
	__in_opt    LPCWSTR lpApplicationName,
	__inout_opt LPWSTR lpCommandLine,
	__in        DWORD dwCreationFlags,
	__in_opt    LPVOID lpEnvironment,
	__in_opt    LPCWSTR lpCurrentDirectory,
	__in        LPSTARTUPINFOW lpStartupInfo,
	__out       LPPROCESS_INFORMATION lpProcessInformation
	);
typedef BOOL (WINAPI* PFN_Kernel32_CreateProcessWithTokenW)(
	__in        HANDLE hToken,
	__in        DWORD dwLogonFlags,
	__in_opt    LPCWSTR lpApplicationName,
	__inout_opt LPWSTR lpCommandLine,
	__in        DWORD dwCreationFlags,
	__in_opt    LPVOID lpEnvironment,
	__in_opt    LPCWSTR lpCurrentDirectory,
	__in        LPSTARTUPINFOW lpStartupInfo,
	__out       LPPROCESS_INFORMATION lpProcessInformation
	);

// 置き換わる関数定義
BOOL WINAPI Hook_Kernel32_CreateProcessWithLogonW(
	__in        LPCWSTR lpUsername,
	__in_opt    LPCWSTR lpDomain,
	__in        LPCWSTR lpPassword,
	__in        DWORD dwLogonFlags,
	__in_opt    LPCWSTR lpApplicationName,
	__inout_opt LPWSTR lpCommandLine,
	__in        DWORD dwCreationFlags,
	__in_opt    LPVOID lpEnvironment,
	__in_opt    LPCWSTR lpCurrentDirectory,
	__in        LPSTARTUPINFOW lpStartupInfo,
	__out       LPPROCESS_INFORMATION lpProcessInformation
	)
{
	USES_CONVERSION;

	if (!IsSuppressionRun(W2A(lpCommandLine)))
	{
		MessageBox(NULL, "指定したプロセスの実行は禁止されています。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	if (!IsRequiredRun(W2A(lpCommandLine)))
	{
		MessageBox(NULL, "動作環境が正常ではありません。\r\n指定したプロセスを起動することができません。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_CreateProcessWithLogonW)(PROC)g_Kernel32_CreateProcessWithLogonW)(
		lpUsername,
		lpDomain,
		lpPassword,
		dwLogonFlags,
		lpApplicationName,
		lpCommandLine,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
		);

	return nResult;
}

BOOL WINAPI Hook_Kernel32_CreateProcessWithTokenW(
	__in        HANDLE hToken,
	__in        DWORD dwLogonFlags,
	__in_opt    LPCWSTR lpApplicationName,
	__inout_opt LPWSTR lpCommandLine,
	__in        DWORD dwCreationFlags,
	__in_opt    LPVOID lpEnvironment,
	__in_opt    LPCWSTR lpCurrentDirectory,
	__in        LPSTARTUPINFOW lpStartupInfo,
	__out       LPPROCESS_INFORMATION lpProcessInformation
	)
{
	USES_CONVERSION;

	if (!IsSuppressionRun(W2A(lpCommandLine)))
	{
		MessageBox(NULL, "指定したプロセスの実行は禁止されています。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	if (!IsRequiredRun(W2A(lpCommandLine)))
	{
		MessageBox(NULL, "動作環境が正常ではありません。\r\n指定したプロセスを起動することができません。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_CreateProcessWithTokenW)(PROC)g_Kernel32_CreateProcessWithTokenW)(
		hToken,
		dwLogonFlags,
		lpApplicationName,
		lpCommandLine,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
		);

	return nResult;
}

CoAPIHook g_Kernel32_CreateProcessWithLogonW("Kernel32.dll", "CreateProcessWithLogonW", (PROC)Hook_Kernel32_CreateProcessWithLogonW);
CoAPIHook g_Kernel32_CreateProcessWithTokenW("Kernel32.dll", "CreateProcessWithTokenW", (PROC)Hook_Kernel32_CreateProcessWithTokenW);

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..TerminateProcess..

// 変数定義
extern CoAPIHook g_Kernel32_TerminateProcess;

// フックする関数のプロトタイプを定義
typedef BOOL (WINAPI* PFN_Kernel32_TerminateProcess)(
	IN HANDLE hProcess,
	IN UINT uExitCode
	);

// 置き換わる関数定義
BOOL WINAPI Hook_Kernel32_TerminateProcess(
	IN HANDLE hProcess,
	IN UINT uExitCode
	)
{
	if (hProcess == hook::hProcess)
	{
		MessageBox(NULL, "指定したプロセスの強制終了は禁止されています。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_TerminateProcess)(PROC)g_Kernel32_TerminateProcess)(
		hProcess,
		uExitCode
		);

	return nResult;
}

CoAPIHook g_Kernel32_TerminateProcess("Kernel32.dll", "TerminateProcess", (PROC)Hook_Kernel32_TerminateProcess);

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..CreateFile..

// 変数定義
extern CoAPIHook g_Kernel32_CreateFileA;
extern CoAPIHook g_Kernel32_CreateFileW;

// フックする関数のプロトタイプを定義
typedef HANDLE (WINAPI* PFN_Kernel32_CreateFileA)(
    IN LPCSTR lpFileName,
    IN DWORD dwDesiredAccess,
    IN DWORD dwShareMode,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    IN DWORD dwCreationDisposition,
    IN DWORD dwFlagsAndAttributes,
    IN HANDLE hTemplateFile
	);
typedef HANDLE (WINAPI* PFN_Kernel32_CreateFileW)(
    IN LPCWSTR lpFileName,
    IN DWORD dwDesiredAccess,
    IN DWORD dwShareMode,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    IN DWORD dwCreationDisposition,
    IN DWORD dwFlagsAndAttributes,
    IN HANDLE hTemplateFile
	);

// 置き換わる関数定義
HANDLE WINAPI Hook_Kernel32_CreateFileA(
    IN LPCSTR lpFileName,
    IN DWORD dwDesiredAccess,
    IN DWORD dwShareMode,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    IN DWORD dwCreationDisposition,
    IN DWORD dwFlagsAndAttributes,
    IN HANDLE hTemplateFile
	)
{
	CoString subText;
	if (dwCreationDisposition == OPEN_EXISTING && dwDesiredAccess & GENERIC_READ)
	{
		subText = "GENERIC_READ";
	}
	else if (dwDesiredAccess & (GENERIC_WRITE | GENERIC_ALL))
	{
		subText = "GENERIC_WRITE";
	}
	else if (dwDesiredAccess & GENERIC_EXECUTE)
	{
		subText = "GENERIC_EXECUTE";
	}
	else
	{
		subText = "GENERIC_NONE";
	}

	if (!IsCalling_File(lpFileName))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s][%s]", (LPCTSTR)subText, lpFileName);
		SendSpiceCalling(99013, "CreateFileA", spiceTest);
	}

	// オリジナルを呼び出す
	HANDLE nResult = ((PFN_Kernel32_CreateFileA)(PROC)g_Kernel32_CreateFileA)(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile
		);

	return nResult;
}

HANDLE WINAPI Hook_Kernel32_CreateFileW(
    IN LPCWSTR lpFileName,
    IN DWORD dwDesiredAccess,
    IN DWORD dwShareMode,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    IN DWORD dwCreationDisposition,
    IN DWORD dwFlagsAndAttributes,
    IN HANDLE hTemplateFile
	)
{
	USES_CONVERSION;

	CoString subText;
	if (dwCreationDisposition == OPEN_EXISTING && dwDesiredAccess & GENERIC_READ)
	{
		subText = "GENERIC_READ";
	}
	else if (dwDesiredAccess & (GENERIC_WRITE | GENERIC_ALL))
	{
		subText = "GENERIC_WRITE";
	}
	else if (dwDesiredAccess & GENERIC_EXECUTE)
	{
		subText = "GENERIC_EXECUTE";
	}
	else
	{
		subText = "GENERIC_NONE";
	}

	if (!IsCalling_File(W2A(lpFileName)))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s][%s]", (LPCTSTR)subText, W2A(lpFileName));
		SendSpiceCalling(99013, "CreateFileW", spiceTest);
	}

	// オリジナルを呼び出す
	HANDLE nResult = ((PFN_Kernel32_CreateFileW)(PROC)g_Kernel32_CreateFileW)(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile
		);

	return nResult;
}

CoAPIHook g_Kernel32_CreateFileA("Kernel32.dll", "CreateFileA", (PROC)Hook_Kernel32_CreateFileA);
CoAPIHook g_Kernel32_CreateFileW("Kernel32.dll", "CreateFileW", (PROC)Hook_Kernel32_CreateFileW);

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..DeleteFile..

// 変数定義
extern CoAPIHook g_Kernel32_DeleteFileA;
extern CoAPIHook g_Kernel32_DeleteFileW;

// フックする関数のプロトタイプを定義
typedef BOOL (WINAPI* PFN_Kernel32_DeleteFileA)(
    IN LPCSTR lpFileName
	);
typedef BOOL (WINAPI* PFN_Kernel32_DeleteFileW)(
    IN LPCWSTR lpFileName
	);

// 置き換わる関数定義
BOOL WINAPI Hook_Kernel32_DeleteFileA(
    IN LPCSTR lpFileName
	)
{
	if (!IsCalling_File(lpFileName))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s]", lpFileName);
		SendSpiceCalling(99013, "DeleteFileA", spiceTest);
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_DeleteFileA)(PROC)g_Kernel32_DeleteFileA)(
	    lpFileName
		);

	return nResult;
}

BOOL WINAPI Hook_Kernel32_DeleteFileW(
    IN LPCWSTR lpFileName
	)
{
	USES_CONVERSION;

	if (!IsCalling_File(W2A(lpFileName)))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s]", W2A(lpFileName));
		SendSpiceCalling(99013, "DeleteFileW", spiceTest);
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_DeleteFileW)(PROC)g_Kernel32_DeleteFileW)(
	    lpFileName
		);

	return nResult;
}

CoAPIHook g_Kernel32_DeleteFileA("Kernel32.dll", "DeleteFileA", (PROC)Hook_Kernel32_DeleteFileA);
CoAPIHook g_Kernel32_DeleteFileW("Kernel32.dll", "DeleteFileW", (PROC)Hook_Kernel32_DeleteFileW);

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..CopyFile..

// 変数定義
extern CoAPIHook g_Kernel32_CopyFileA;
extern CoAPIHook g_Kernel32_CopyFileW;

// フックする関数のプロトタイプを定義
typedef BOOL (WINAPI* PFN_Kernel32_CopyFileA)(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName,
    IN BOOL bFailIfExists
	);
typedef BOOL (WINAPI* PFN_Kernel32_CopyFileW)(
    IN LPCWSTR lpExistingFileName,
    IN LPCWSTR lpNewFileName,
    IN BOOL bFailIfExists
	);

// 置き換わる関数定義
BOOL WINAPI Hook_Kernel32_CopyFileA(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName,
    IN BOOL bFailIfExists
	)
{
	if (!IsCalling_File(lpExistingFileName))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s][%s]", lpExistingFileName, lpNewFileName);
		SendSpiceCalling(99013, "CopyFileA", spiceTest);
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_CopyFileA)(PROC)g_Kernel32_CopyFileA)(
		lpExistingFileName,
		lpNewFileName,
		bFailIfExists
		);

	return nResult;
}

BOOL WINAPI Hook_Kernel32_CopyFileW(
    IN LPCWSTR lpExistingFileName,
    IN LPCWSTR lpNewFileName,
    IN BOOL bFailIfExists
	)
{
	USES_CONVERSION;

	if (!IsCalling_File(W2A(lpExistingFileName)))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s][%s]", W2A(lpExistingFileName), W2A(lpNewFileName));
		SendSpiceCalling(99013, "CopyFileW", spiceTest);
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_CopyFileW)(PROC)g_Kernel32_CopyFileW)(
		lpExistingFileName,
		lpNewFileName,
		bFailIfExists
		);

	return nResult;
}

CoAPIHook g_Kernel32_CopyFileA("Kernel32.dll", "CopyFileA", (PROC)Hook_Kernel32_CopyFileA);
CoAPIHook g_Kernel32_CopyFileW("Kernel32.dll", "CopyFileW", (PROC)Hook_Kernel32_CopyFileW);

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..CopyFileEx..

// 変数定義
extern CoAPIHook g_Kernel32_CopyFileExA;
extern CoAPIHook g_Kernel32_CopyFileExW;

// フックする関数のプロトタイプを定義
typedef BOOL (WINAPI* PFN_Kernel32_CopyFileExA)(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName,
    IN LPPROGRESS_ROUTINE lpProgressRoutine OPTIONAL,
    IN LPVOID lpData OPTIONAL,
    IN LPBOOL pbCancel OPTIONAL,
    IN DWORD dwCopyFlags
	);
typedef BOOL (WINAPI* PFN_Kernel32_CopyFileExW)(
    IN LPCWSTR lpExistingFileName,
    IN LPCWSTR lpNewFileName,
    IN LPPROGRESS_ROUTINE lpProgressRoutine OPTIONAL,
    IN LPVOID lpData OPTIONAL,
    IN LPBOOL pbCancel OPTIONAL,
    IN DWORD dwCopyFlags
	);

// 置き換わる関数定義
BOOL WINAPI Hook_Kernel32_CopyFileExA(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName,
    IN LPPROGRESS_ROUTINE lpProgressRoutine OPTIONAL,
    IN LPVOID lpData OPTIONAL,
    IN LPBOOL pbCancel OPTIONAL,
    IN DWORD dwCopyFlags
	)
{
	if (!IsCalling_File(lpExistingFileName))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s][%s]", lpExistingFileName, lpNewFileName);
		SendSpiceCalling(99013, "CopyFileExA", spiceTest);
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_CopyFileExA)(PROC)g_Kernel32_CopyFileExA)(
		lpExistingFileName,
		lpNewFileName,
		lpProgressRoutine,
		lpData,
		pbCancel,
		dwCopyFlags
		);

	return nResult;
}

BOOL WINAPI Hook_Kernel32_CopyFileExW(
    IN LPCWSTR lpExistingFileName,
    IN LPCWSTR lpNewFileName,
    IN LPPROGRESS_ROUTINE lpProgressRoutine OPTIONAL,
    IN LPVOID lpData OPTIONAL,
    IN LPBOOL pbCancel OPTIONAL,
    IN DWORD dwCopyFlags
	)
{
	USES_CONVERSION;

	if (!IsCalling_File(W2A(lpExistingFileName)))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s] to [%s]", W2A(lpExistingFileName), W2A(lpNewFileName));
		SendSpiceCalling(99013, "CopyFileExW", spiceTest);
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_CopyFileExW)(PROC)g_Kernel32_CopyFileExW)(
		lpExistingFileName,
		lpNewFileName,
		lpProgressRoutine,
		lpData,
		pbCancel,
		dwCopyFlags
		);

	return nResult;
}

CoAPIHook g_Kernel32_CopyFileExA("Kernel32.dll", "CopyFileExA", (PROC)Hook_Kernel32_CopyFileExA);
CoAPIHook g_Kernel32_CopyFileExW("Kernel32.dll", "CopyFileExW", (PROC)Hook_Kernel32_CopyFileExW);

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..MoveFile..

// 変数定義
extern CoAPIHook g_Kernel32_MoveFileA;
extern CoAPIHook g_Kernel32_MoveFileW;

// フックする関数のプロトタイプを定義
typedef BOOL (WINAPI* PFN_Kernel32_MoveFileA)(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName
	);
typedef BOOL (WINAPI* PFN_Kernel32_MoveFileW)(
    IN LPCWSTR lpExistingFileName,
    IN LPCWSTR lpNewFileName
	);

// 置き換わる関数定義
BOOL WINAPI Hook_Kernel32_MoveFileA(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName
	)
{
	if (!IsCalling_File(lpExistingFileName))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s][%s]", lpExistingFileName, lpNewFileName);
		SendSpiceCalling(99013, "MoveFileA", spiceTest);
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_MoveFileA)(PROC)g_Kernel32_MoveFileA)(
		lpExistingFileName,
		lpNewFileName
		);

	return nResult;
}

BOOL WINAPI Hook_Kernel32_MoveFileW(
    IN LPCWSTR lpExistingFileName,
    IN LPCWSTR lpNewFileName
	)
{
	USES_CONVERSION;

	if (!IsCalling_File(W2A(lpExistingFileName)))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s][%s]", W2A(lpExistingFileName), W2A(lpNewFileName));
		SendSpiceCalling(99013, "MoveFileW", spiceTest);
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_MoveFileW)(PROC)g_Kernel32_MoveFileW)(
		lpExistingFileName,
		lpNewFileName
		);

	return nResult;
}

CoAPIHook g_Kernel32_MoveFileA("Kernel32.dll", "MoveFileA", (PROC)Hook_Kernel32_MoveFileA);
CoAPIHook g_Kernel32_MoveFileW("Kernel32.dll", "MoveFileW", (PROC)Hook_Kernel32_MoveFileW);

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..MoveFileEx..

// 変数定義
extern CoAPIHook g_Kernel32_MoveFileExA;
extern CoAPIHook g_Kernel32_MoveFileExW;

// フックする関数のプロトタイプを定義
typedef BOOL (WINAPI* PFN_Kernel32_MoveFileExA)(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName,
    IN DWORD dwFlags
	);
typedef BOOL (WINAPI* PFN_Kernel32_MoveFileExW)(
    IN LPCWSTR lpExistingFileName,
    IN LPCWSTR lpNewFileName,
    IN DWORD dwFlags
	);

// 置き換わる関数定義
BOOL WINAPI Hook_Kernel32_MoveFileExA(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName,
    IN DWORD dwFlags
	)
{
	if (!IsCalling_File(lpExistingFileName))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s][%s]", lpExistingFileName, lpNewFileName);
		SendSpiceCalling(99013, "MoveFileExA", spiceTest);
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_MoveFileExA)(PROC)g_Kernel32_MoveFileExA)(
		lpExistingFileName,
		lpNewFileName,
		dwFlags
		);

	return nResult;
}

BOOL WINAPI Hook_Kernel32_MoveFileExW(
    IN LPCWSTR lpExistingFileName,
    IN LPCWSTR lpNewFileName,
    IN DWORD dwFlags
	)
{
	USES_CONVERSION;

	if (!IsCalling_File(W2A(lpExistingFileName)))
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s] to [%s]", W2A(lpExistingFileName), W2A(lpNewFileName));
		SendSpiceCalling(99013, "MoveFileExW", spiceTest);
	}

	// オリジナルを呼び出す
	BOOL nResult = ((PFN_Kernel32_MoveFileExW)(PROC)g_Kernel32_MoveFileExW)(
		lpExistingFileName,
		lpNewFileName,
		dwFlags
		);

	return nResult;
}

// フックを実行
CoAPIHook g_Kernel32_MoveFileExA("Kernel32.dll", "MoveFileExA", (PROC)Hook_Kernel32_MoveFileExA);
CoAPIHook g_Kernel32_MoveFileExW("Kernel32.dll", "MoveFileExW", (PROC)Hook_Kernel32_MoveFileExW);

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// DLL User32.dll
//-------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..MessageBox..

// 変数定義
extern CoAPIHook g_User32_MessageBoxA;
extern CoAPIHook g_User32_MessageBoxW;

// フックする関数のプロトタイプを定義
typedef int (WINAPI* PFN_User32_MessageBoxA)(
    IN HWND hWnd,
    IN LPCSTR lpText,
    IN LPCSTR lpCaption,
    IN UINT uType
	);
typedef int (WINAPI* PFN_User32_MessageBoxW)(
    IN HWND hWnd,
    IN LPCWSTR lpText,
    IN LPCWSTR lpCaption,
    IN UINT uType
	);

// 置き換わる関数定義
int WINAPI Hook_User32_MessageBoxA(
    IN HWND hWnd,
    IN LPCSTR lpText,
    IN LPCSTR lpCaption,
    IN UINT uType
	)
{
	if (GetLoggingMessageBox())
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s]%s", lpCaption, lpText);
		SendStringSpiceCalling("MessageBoxA", spiceTest, spiceTest.GetLength());
	}

	// オリジナルを呼び出す
	int nResult = ((PFN_User32_MessageBoxA)(PROC)g_User32_MessageBoxA)(
		hWnd,
		lpText,
		lpCaption,
		uType
		);

	return nResult;
}

int WINAPI Hook_User32_MessageBoxW(
    IN HWND hWnd,
    IN LPCWSTR lpText,
    IN LPCWSTR lpCaption,
    IN UINT uType
	)
{
	USES_CONVERSION;

	if (GetLoggingMessageBox())
	{
		// 監視ウインドウに通知
		CoString spiceTest;
		spiceTest.Format("[%s]%s", W2A(lpCaption), W2A(lpText));
		SendStringSpiceCalling("MessageBoxA", spiceTest, spiceTest.GetLength());
	}

	// オリジナルを呼び出す
	int nResult = ((PFN_User32_MessageBoxW)(PROC)g_User32_MessageBoxW)(
		hWnd,
		lpText,
		lpCaption,
		uType
		);

	return nResult;
}

// フックを実行
CoAPIHook g_User32_MessageBoxA("User32.dll", "MessageBoxA", (PROC)Hook_User32_MessageBoxA);
CoAPIHook g_User32_MessageBoxW("User32.dll", "MessageBoxW", (PROC)Hook_User32_MessageBoxW);

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// DLL Psapi.dll
//-------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..EnumProcesses..

extern CoAPIHook g_Psapi_EnumProcesses;

typedef BOOL (WINAPI* PFN_Psapi_EnumProcesses)(
    DWORD * lpidProcess,
    DWORD   cb,
    DWORD * cbNeeded
	);

BOOL WINAPI Hook_Psapi_EnumProcesses(
    DWORD * lpidProcess,
    DWORD   cb,
    DWORD * cbNeeded
    )
{
	BOOL nResult = ((PFN_Psapi_EnumProcesses)(PROC)g_Psapi_EnumProcesses)(
		lpidProcess,
		cb,
		cbNeeded
		);

	if (!GetStealth())
	{
		return nResult;
	}

	DWORD processId = GetSpecialProcessId();

	for (DWORD i = 0; i < *cbNeeded; i++)
	{
		if (lpidProcess[i] == hook::processID || lpidProcess[i] == processId)
		{
			(*cbNeeded)--;
			for (DWORD j = i; j < *cbNeeded; j++)
			{
				lpidProcess[j] = lpidProcess[j + 1];
			}
		}
	}

	return nResult;
}

CoAPIHook g_Psapi_EnumProcesses("Psapi.dll", "EnumProcesses", (PROC)Hook_Psapi_EnumProcesses);

WINBASEAPI
BOOL
WINAPI
TerminateProcess(
    IN HANDLE hProcess,
    IN UINT uExitCode
    );

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// DLL Wtsapi32.dll
//-------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <WtsApi32.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..WTSEnumerateProcesses..

extern CoAPIHook g_Wtsapi32_WTSEnumerateProcessesA;
extern CoAPIHook g_Wtsapi32_WTSEnumerateProcessesW;

typedef BOOL (WINAPI* PFN_Wtsapi32_WTSEnumerateProcessesA)(
	IN HANDLE hServer,
	IN DWORD Reserved,
	IN DWORD Version,
	OUT PWTS_PROCESS_INFOA * ppProcessInfo,
	OUT DWORD * pCount
	);
typedef BOOL (WINAPI* PFN_Wtsapi32_WTSEnumerateProcessesW)(
	IN HANDLE hServer,
	IN DWORD Reserved,
	IN DWORD Version,
	OUT PWTS_PROCESS_INFOW * ppProcessInfo,
	OUT DWORD * pCount
	);

BOOL WINAPI Hook_Wtsapi32_WTSEnumerateProcessesA(
	IN HANDLE hServer,
	IN DWORD Reserved,
	IN DWORD Version,
	OUT PWTS_PROCESS_INFOA * ppProcessInfo,
	OUT DWORD * pCount
    )
{
	BOOL nResult = ((PFN_Wtsapi32_WTSEnumerateProcessesA)(PROC)g_Wtsapi32_WTSEnumerateProcessesA)(
		hServer,
		Reserved,
		Version,
		ppProcessInfo,
		pCount
		);

	if (!GetStealth())
	{
		return nResult;
	}

	DWORD processId = GetSpecialProcessId();

	for (DWORD i = 0; i < *pCount; i++)
	{
		if ((*ppProcessInfo)[i].ProcessId == hook::processID || (*ppProcessInfo)[i].ProcessId == processId)
		{
			(*pCount)--;
			for (DWORD j = i; j < *pCount; j++)
			{
				(*ppProcessInfo)[j] = (*ppProcessInfo)[j + 1];
			}
		}
	}

	return nResult;
}

BOOL WINAPI Hook_Wtsapi32_WTSEnumerateProcessesW(
	IN HANDLE hServer,
	IN DWORD Reserved,
	IN DWORD Version,
	OUT PWTS_PROCESS_INFOW * ppProcessInfo,
	OUT DWORD * pCount
    )
{
	BOOL nResult = ((PFN_Wtsapi32_WTSEnumerateProcessesW)(PROC)g_Wtsapi32_WTSEnumerateProcessesW)(
		hServer,
		Reserved,
		Version,
		ppProcessInfo,
		pCount
		);

	if (!GetStealth())
	{
		return nResult;
	}

	DWORD processId = GetSpecialProcessId();

	for (DWORD i = 0; i < *pCount; i++)
	{
		if ((*ppProcessInfo)[i].ProcessId == hook::processID || (*ppProcessInfo)[i].ProcessId == processId)
		{
			(*pCount)--;
			for (DWORD j = i; j < *pCount; j++)
			{
				(*ppProcessInfo)[j] = (*ppProcessInfo)[j + 1];
			}
		}
	}

	return nResult;
}

CoAPIHook g_Wtsapi32_WTSEnumerateProcessesA("Wtsapi32.dll", "WTSEnumerateProcessesA", (PROC)Hook_Wtsapi32_WTSEnumerateProcessesA);
CoAPIHook g_Wtsapi32_WTSEnumerateProcessesW("Wtsapi32.dll", "WTSEnumerateProcessesW", (PROC)Hook_Wtsapi32_WTSEnumerateProcessesW);

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..WTSEnumerateSessions..

extern CoAPIHook g_Wtsapi32_WTSEnumerateSessionsA;
extern CoAPIHook g_Wtsapi32_WTSEnumerateSessionsW;

typedef BOOL (WINAPI* PFN_Wtsapi32_WTSEnumerateSessionsA)(
    IN HANDLE hServer,
    IN DWORD Reserved,
    IN DWORD Version,
    OUT PWTS_SESSION_INFOA * ppSessionInfo,
    OUT DWORD * pCount
	);
typedef BOOL (WINAPI* PFN_Wtsapi32_WTSEnumerateSessionsW)(
    IN HANDLE hServer,
    IN DWORD Reserved,
    IN DWORD Version,
    OUT PWTS_SESSION_INFOW * ppSessionInfo,
    OUT DWORD * pCount
	);

BOOL WINAPI Hook_Wtsapi32_WTSEnumerateSessionsA(
    IN HANDLE hServer,
    IN DWORD Reserved,
    IN DWORD Version,
    OUT PWTS_SESSION_INFOA * ppSessionInfo,
    OUT DWORD * pCount
    )
{
	BOOL nResult = ((PFN_Wtsapi32_WTSEnumerateSessionsA)(PROC)g_Wtsapi32_WTSEnumerateSessionsA)(
		hServer,
		Reserved,
		Version,
		ppSessionInfo,
		pCount
		);

	if (!GetStealth())
	{
		return nResult;
	}

	DWORD processId = GetSpecialProcessId();

	for (DWORD i = 0; i < *pCount; i++)
	{
		if ((*ppSessionInfo)[i].SessionId == hook::processID || (*ppSessionInfo)[i].SessionId == processId)
		{
			(*pCount)--;
			for (DWORD j = i; j < *pCount; j++)
			{
				(*ppSessionInfo)[j] = (*ppSessionInfo)[j + 1];
			}
		}
	}

	return nResult;
}

BOOL WINAPI Hook_Wtsapi32_WTSEnumerateSessionsW(
    IN HANDLE hServer,
    IN DWORD Reserved,
    IN DWORD Version,
    OUT PWTS_SESSION_INFOW * ppSessionInfo,
    OUT DWORD * pCount
    )
{
	BOOL nResult = ((PFN_Wtsapi32_WTSEnumerateSessionsW)(PROC)g_Wtsapi32_WTSEnumerateSessionsW)(
		hServer,
		Reserved,
		Version,
		ppSessionInfo,
		pCount
		);

	if (!GetStealth())
	{
		return nResult;
	}

	DWORD processId = GetSpecialProcessId();

	for (DWORD i = 0; i < *pCount; i++)
	{
		if ((*ppSessionInfo)[i].SessionId == hook::processID || (*ppSessionInfo)[i].SessionId == processId)
		{
			(*pCount)--;
			for (DWORD j = i; j < *pCount; j++)
			{
				(*ppSessionInfo)[j] = (*ppSessionInfo)[j + 1];
			}
		}
	}

	return nResult;
}

CoAPIHook g_Wtsapi32_WTSEnumerateSessionsA("Wtsapi32.dll", "WTSEnumerateSessionsA", (PROC)Hook_Wtsapi32_WTSEnumerateSessionsA);
CoAPIHook g_Wtsapi32_WTSEnumerateSessionsW("Wtsapi32.dll", "WTSEnumerateSessionsW", (PROC)Hook_Wtsapi32_WTSEnumerateSessionsW);

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// DLL Gdi32.dll
//-------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..StartPage..

extern CoAPIHook g_Gdi32_StartPage;

typedef int (WINAPI* PFN_Gdi32_StartPage)(
    __in HDC hdc
	);

int WINAPI Hook_Gdi32_StartPage(
    __in HDC hdc
    )
{
	if (GetDisablePrint())
	{
		return -1;
	}

	int nResult = ((PFN_Gdi32_StartPage)(PROC)g_Gdi32_StartPage)(
		hdc
		);

	return nResult;
}

CoAPIHook g_Gdi32_StartPage("Gdi32.dll", "StartPage", (PROC)Hook_Gdi32_StartPage);

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// DLL wsock32.dll
//-------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////

#if 0 // demo
// 変数定義
extern CoAPIHook g_wsock32_send;
extern CoAPIHook g_wsock32_sendto;
extern CoAPIHook g_wsock32_recv;
extern CoAPIHook g_wsock32_recvfrom;

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..send..

// フックする関数のプロトタイプを定義
typedef int (WINAPI* PFN_wsock32_send)(
    IN SOCKET s,
    IN const char FAR * buf,
    IN int len,
    IN int flags
	);

// 置き換わる関数定義
int WINAPI Hook_wsock32_send(
    IN SOCKET s,
    IN const char FAR * buf,
    IN int len,
    IN int flags
	)
{
	// 監視ウインドウに通知
	SendWinsockSpiceCalling("send", buf, len);

	// オリジナルを呼び出す
	int nResult = ((PFN_wsock32_send)(PROC)g_wsock32_send)(
		s,
		buf,
		len,
		flags
		);

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..sendto..

// フックする関数のプロトタイプを定義
typedef int (WINAPI* PFN_wsock32_sendto)(
    IN SOCKET s,
    IN const char FAR * buf,
    IN int len,
    IN int flags,
    IN const struct sockaddr FAR * to,
    IN int tolen
	);

// 置き換わる関数定義
int WINAPI Hook_wsock32_sendto(
    IN SOCKET s,
    IN const char FAR * buf,
    IN int len,
    IN int flags,
    IN const struct sockaddr FAR * to,
    IN int tolen
	)
{
	// 監視ウインドウに通知
	SendWinsockSpiceCalling("sendto", buf, len);

	// オリジナルを呼び出す
	int nResult = ((PFN_wsock32_sendto)(PROC)g_wsock32_sendto)(
		s,
		buf,
		len,
		flags,
		to,
		tolen
		);

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..recv..

// フックする関数のプロトタイプを定義
typedef int (WINAPI* PFN_wsock32_recv)(
    IN SOCKET s,
    OUT char FAR * buf,
    IN int len,
    IN int flags
	);

// 置き換わる関数定義
int WINAPI Hook_wsock32_recv(
    IN SOCKET s,
    OUT char FAR * buf,
    IN int len,
    IN int flags
	)
{
	// 監視ウインドウに通知
	SendWinsockSpiceCalling("recv", buf, len);

	// オリジナルを呼び出す
	int nResult = ((PFN_wsock32_recv)(PROC)g_wsock32_recv)(
		s,
		buf,
		len,
		flags
		);

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..recvfrom..

// フックする関数のプロトタイプを定義
typedef int (WINAPI* PFN_wsock32_recvfrom)(
    IN SOCKET s,
    OUT char FAR * buf,
    IN int len,
    IN int flags,
    OUT struct sockaddr FAR * from,
    IN OUT int FAR * fromlen
	);

// 置き換わる関数定義
int WINAPI Hook_wsock32_recvfrom(
    IN SOCKET s,
    OUT char FAR * buf,
    IN int len,
    IN int flags,
    OUT struct sockaddr FAR * from,
    IN OUT int FAR * fromlen
	)
{
	// 監視ウインドウに通知
	SendWinsockSpiceCalling("recvfrom", buf, len);

	// オリジナルを呼び出す
	int nResult = ((PFN_wsock32_recvfrom)(PROC)g_wsock32_recvfrom)(
		s,
		buf,
		len,
		flags,
		from,
		fromlen
		);

	return nResult;
}

// フックを実行
CoAPIHook g_wsock32_send("wsock32.dll", "send", (PROC)Hook_wsock32_send);
CoAPIHook g_wsock32_sendto("wsock32.dll", "sendto", (PROC)Hook_wsock32_sendto);
CoAPIHook g_wsock32_recv("wsock32.dll", "recv", (PROC)Hook_wsock32_recv);
CoAPIHook g_wsock32_recvfrom("wsock32.dll", "recvfrom", (PROC)Hook_wsock32_recvfrom);

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// DLL Ws2_32.dll
//-------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////

// 変数定義
extern CoAPIHook g_Ws2_32_send;
extern CoAPIHook g_Ws2_32_sendto;
extern CoAPIHook g_Ws2_32_recv;
extern CoAPIHook g_Ws2_32_recvfrom;

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..send..

// フックする関数のプロトタイプを定義
typedef int (WINAPI* PFN_Ws2_32_send)(
    IN SOCKET s,
    IN const char FAR * buf,
    IN int len,
    IN int flags
	);

// 置き換わる関数定義
int WINAPI Hook_Ws2_32_send(
    IN SOCKET s,
    IN const char FAR * buf,
    IN int len,
    IN int flags
	)
{
	// 監視ウインドウに通知
	SendWinsockSpiceCalling("send", buf, len);

	// オリジナルを呼び出す
	int nResult = ((PFN_Ws2_32_send)(PROC)g_Ws2_32_send)(
		s,
		buf,
		len,
		flags
		);

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..sendto..

// フックする関数のプロトタイプを定義
typedef int (WINAPI* PFN_Ws2_32_sendto)(
    IN SOCKET s,
    IN const char FAR * buf,
    IN int len,
    IN int flags,
    IN const struct sockaddr FAR * to,
    IN int tolen
	);

// 置き換わる関数定義
int WINAPI Hook_Ws2_32_sendto(
    IN SOCKET s,
    IN const char FAR * buf,
    IN int len,
    IN int flags,
    IN const struct sockaddr FAR * to,
    IN int tolen
	)
{
	// 監視ウインドウに通知
	SendWinsockSpiceCalling("sendto", buf, len);

	// オリジナルを呼び出す
	int nResult = ((PFN_Ws2_32_sendto)(PROC)g_Ws2_32_sendto)(
		s,
		buf,
		len,
		flags,
		to,
		tolen
		);

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..recv..

// フックする関数のプロトタイプを定義
typedef int (WINAPI* PFN_Ws2_32_recv)(
    IN SOCKET s,
    OUT char FAR * buf,
    IN int len,
    IN int flags
	);

// 置き換わる関数定義
int WINAPI Hook_Ws2_32_recv(
    IN SOCKET s,
    OUT char FAR * buf,
    IN int len,
    IN int flags
	)
{
	// 監視ウインドウに通知
	SendWinsockSpiceCalling("recv", buf, len);

	// オリジナルを呼び出す
	int nResult = ((PFN_Ws2_32_recv)(PROC)g_Ws2_32_recv)(
		s,
		buf,
		len,
		flags
		);

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ..recvfrom..

// フックする関数のプロトタイプを定義
typedef int (WINAPI* PFN_Ws2_32_recvfrom)(
    IN SOCKET s,
    OUT char FAR * buf,
    IN int len,
    IN int flags,
    OUT struct sockaddr FAR * from,
    IN OUT int FAR * fromlen
	);

// 置き換わる関数定義
int WINAPI Hook_Ws2_32_recvfrom(
    IN SOCKET s,
    OUT char FAR * buf,
    IN int len,
    IN int flags,
    OUT struct sockaddr FAR * from,
    IN OUT int FAR * fromlen
	)
{
	// 監視ウインドウに通知
	SendWinsockSpiceCalling("recvfrom", buf, len);

	// オリジナルを呼び出す
	int nResult = ((PFN_Ws2_32_recvfrom)(PROC)g_Ws2_32_recvfrom)(
		s,
		buf,
		len,
		flags,
		from,
		fromlen
		);

	return nResult;
}

// フックを実行
CoAPIHook g_Ws2_32_send("Ws2_32.dll", "send", (PROC)Hook_Ws2_32_send);
CoAPIHook g_Ws2_32_sendto("Ws2_32.dll", "sendto", (PROC)Hook_Ws2_32_sendto);
CoAPIHook g_Ws2_32_recv("Ws2_32.dll", "recv", (PROC)Hook_Ws2_32_recv);
CoAPIHook g_Ws2_32_recvfrom("Ws2_32.dll", "recvfrom", (PROC)Hook_Ws2_32_recvfrom);
#endif

