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

#include <atlstr.h>
#include <shlobj.h>
#include <atlcomtime.h>

#include "oTools.h"

#pragma warning(disable: 4996)

// Windows98SEである
BOOL IsWindows98SE()
{
	OSVERSIONINFO osvi = { sizeof OSVERSIONINFO };
	BOOL bSuceeded = GetVersionEx(&osvi);
	return (bSuceeded && osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10 && LOWORD(osvi.dwBuildNumber) >= 2222);
}

// WindowsMEである
BOOL IsWindowsME()
{
	OSVERSIONINFO osvi = { sizeof OSVERSIONINFO };
	BOOL bSuceeded = GetVersionEx(&osvi);
	return (bSuceeded && osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90);
}

// Windows2000である
BOOL IsWindows2000()
{
	OSVERSIONINFO osvi = { sizeof OSVERSIONINFO };
	BOOL bSuceeded = GetVersionEx(&osvi);
	return (bSuceeded && osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0);
}

// WindowsXPである
BOOL IsWindowsXP()
{
	OSVERSIONINFO osvi = { sizeof OSVERSIONINFO };
	BOOL bSuceeded = GetVersionEx(&osvi);
	return (bSuceeded && osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1);
}

// Windows2003である
BOOL IsWindows2003()
{
	OSVERSIONINFO osvi = { sizeof OSVERSIONINFO };
	BOOL bSuceeded = GetVersionEx(&osvi);
	return (bSuceeded && osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2);
}

// WindowsVistaである
BOOL IsWindowsVista()
{
	OSVERSIONINFO osvi = { sizeof OSVERSIONINFO };
	BOOL bSuceeded = GetVersionEx(&osvi);
	return (bSuceeded && osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0);
}

// ファイルの存在が確認されるまで待機する
DWORD PregnancyFile(LPCTSTR filePath, DWORD tenacity)
{
	for (DWORD i = 0, count = tenacity; i < count; i++)
	{
		WIN32_FIND_DATA wfd;
		HANDLE hFindFile = FindFirstFile(filePath, &wfd);
		if (hFindFile != INVALID_HANDLE_VALUE)
		{
			FindClose(hFindFile);

			return 0;
		}
		Sleep(500);
	}

	return -1; // time out
}

// ファイルの保存が完了されるまで待機する
INT NativityFile(LPCTSTR filePath, INT tenacity, BOOL bExisting/* = TRUE*/, LPHANDLE phFile/* = NULL*/)
{
	for (INT i = 0, count = tenacity; i < count; i++)
	{
		HANDLE hFile = CreateFile(filePath, GENERIC_ALL, 0, NULL, bExisting ? OPEN_EXISTING : OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			if (phFile)
			{
				*phFile = hFile;
			}
			else
			{
				CloseHandle(hFile);
			}

			return 0;
		}
		Sleep(500);
	}

	if (phFile)
	{
		*phFile = INVALID_HANDLE_VALUE;
	}

	return -1; // time out
}

const LPCTSTR GetModuleFolder(DWORD flg/* = 0*/)
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
		memcpy(moduleFolder, drive, strlen(drive) + 1);
		memcpy(moduleFolder + strlen(moduleFolder), dir, strlen(dir) + 1);
		if (flg & 1)
		{
			memcpy(moduleFolder + strlen(moduleFolder), fname, strlen(fname) + 1);
		}
	}
	return moduleFolder;
}

// extern "C" _declspec(dllexport)
const LPCTSTR GetApplicationDataPath()
{
	static CString localpath;
	if (!localpath.IsEmpty())
	{
		return localpath;
	}

	BOOL bRet = SHGetSpecialFolderPath(NULL, localpath.GetBuffer(MAX_PATH), CSIDL_COMMON_APPDATA, 0);
	localpath.ReleaseBuffer();
	if (!bRet || localpath.IsEmpty())
	{
		DEBUG_OUT_DEFAULT("failed.");
		localpath = ("\0 C:\\Documents and Settings\\All Users\\Application Data") + 2;
	}
	localpath += ("\0 \\pLook" + 2);
	CreateDirectory(localpath, NULL);

	return localpath;
}

void GetFormatMessage(const DWORD error, CString& msg)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

	msg = (LPCTSTR)lpMsgBuf;

	// バッファを解放する。
	LocalFree(lpMsgBuf);
}

// デバッグ用に動作する
const BOOL IsSuperDebug(BOOL bDebug/* = FALSE*/)
{
	static BOOL s_bDebug = FALSE;
	static BOOL s_first = TRUE;

	if (s_first)
	{
		s_first = FALSE;

	#ifdef _DEBUG // SuperDebug
		s_bDebug = TRUE;
	#endif
		CString modulePath = GetModuleFolder();
		if (strstr(modulePath, "\\Projects\\Plustar\\") != 0)
		{
			s_bDebug = TRUE;
		}
	}

	return s_bDebug;
}

void DebugStopper()
{
#ifdef _DEBUG
	_asm
	{
		int 3
	}
#endif
}

// extern "C" _declspec(dllexport)
const LPCTSTR GetDebugLogPath()
{
	static CString folder;
	static CString localpath;
	if (!localpath.IsEmpty())
	{
		return localpath;
	}

	folder = GetApplicationDataPath();
	folder += P("\\pDebug");
	CreateDirectory(localpath, NULL);

	localpath = folder + P("\\logdata_");

	WIN32_FIND_DATA wfd = { 0 };
	HANDLE hFind = FindFirstFile(localpath + "*", &wfd);
	for (BOOL bNext = hFind != INVALID_HANDLE_VALUE ? TRUE : FALSE; bNext; bNext = FindNextFile(hFind, &wfd))
	{
		COleDateTime now = COleDateTime::GetCurrentTime();
		now -= COleDateTimeSpan(7, 0, 0, 0);
		if (now > wfd.ftLastWriteTime)
		{
			DeleteFile(folder + "\\" + wfd.cFileName);
		}
	}

	COleDateTime time = COleDateTime::GetCurrentTime();
	localpath += time.Format("%Y%m%d");
	localpath += P(".log");

	return localpath;
}

const LPCTSTR NowTime()
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	static CString time;
	time = now.Format("%H:%M:%S");
	return time;
}

const LPCTSTR RightEight(LPCTSTR value)
{
	size_t length = strlen(value);
	if (length <= 8)
	{
		return value;
	}
	return value + length - 8;
}

void OutputViewer(LPCTSTR lpszFormat, ...)
{
	CString text;
	va_list argList;
	va_start(argList, lpszFormat);
	text.FormatV(lpszFormat, argList);
	va_end(argList);
	OutputDebugString(text);
	OutputDebugString("\r\n");

	static CString logPath = GetDebugLogPath();

	for (int i = 0, count = 1; i < count; i++)
	{
		HANDLE hFile = CreateFile(logPath, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER num = { 0 };
			SetFilePointerEx(hFile, num, NULL, FILE_END);
			DWORD dwSize = 0;
			WriteFile(hFile, text, text.GetLength(), &dwSize, NULL);
			WriteFile(hFile, "\r\n", (DWORD)strlen("\r\n"), &dwSize, NULL);
			SetEndOfFile(hFile);
			CloseHandle(hFile);
		}
	}
}

void OutputViewerEx(DWORD sw, LPCTSTR lpszFormat, ...)
{
	CString text;
	va_list argList;
	va_start(argList, lpszFormat);
	text.FormatV(lpszFormat, argList);
	va_end(argList);
	OutputDebugString(text);
	OutputDebugString("\r\n");

	static CString logPath = GetDebugLogPath();

	for (int i = 0, count = 1; i < count; i++)
	{
		HANDLE hFile = CreateFile(logPath, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER num = { 0 };
			SetFilePointerEx(hFile, num, NULL, FILE_END);
			DWORD dwSize = 0;
			WriteFile(hFile, text, text.GetLength(), &dwSize, NULL);
			WriteFile(hFile, "\r\n", (DWORD)strlen("\r\n"), &dwSize, NULL);
			SetEndOfFile(hFile);
			CloseHandle(hFile);
		}
	}
}

void OutputViewerDirect(LPCTSTR lpszDirect)
{
	static CString logPath = GetDebugLogPath();

	for (int i = 0, count = 1; i < count; i++)
	{
		HANDLE hFile = CreateFile(logPath, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER num = { 0 };
			SetFilePointerEx(hFile, num, NULL, FILE_END);
			DWORD dwSize = 0;
			WriteFile(hFile, lpszDirect, (DWORD)strlen(lpszDirect), &dwSize, NULL);
			SetEndOfFile(hFile);
			CloseHandle(hFile);
		}
	}

#if 0 // demo
	DebugStopper();
#endif
}

// プロセスの実行
LRESULT RunProcess(LPCTSTR runProcess, LPCTSTR param/* = NULL*/, DWORD wait/* = INFINITE*/)
{
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { sizeof STARTUPINFO };

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	CString arg;
	arg.Format("\"%s\" %s", runProcess, param);
	if (!CreateProcess(NULL, (LPSTR)(LPCTSTR)arg, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	CloseHandle(pi.hThread);
	WaitForMultipleObjects(1, &pi.hProcess, TRUE, wait);
	CloseHandle(pi.hProcess);

	return 0;
}

BOOL EnablePrivilege(LPTSTR lpszPrivilege)
{
	HANDLE htoken = 0;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &htoken))
		return FALSE;
	
	LUID luid = { 0 };
	if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
		CloseHandle(htoken);
		return FALSE;
	}

	TOKEN_PRIVILEGES tp = { 0 };
	tp.PrivilegeCount           = 1;
	tp.Privileges[0].Luid       = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	
	BOOL bRetCode = AdjustTokenPrivileges(htoken, FALSE, &tp, sizeof TOKEN_PRIVILEGES, NULL, NULL);

	CloseHandle(htoken);
	
	return bRetCode;
}

// 囲まれている文字列の取得（中身だけ）
CString oAbstractSearchSubstance(const CString& targ, INT& start, const CString ce/* = "\""*/, CString limit/* = ""*/, BOOL bCSV/* = FALSE*/)
{
	INT nStart = start;
	INT temp;
	INT limited = limit.IsEmpty() ? targ.GetLength() : (temp = targ.Find(limit, start)) == -1 ? targ.GetLength() : (temp + limit.GetLength());
	// 文字列の開始
	CString c = ce.Mid(0, 1);
	CString e = ce.Mid(1, 1);
	if (e.GetLength() == 0)
	{
		e = c;
	}
	start = targ.Find(c, start);
	if (start == -1 || !(start < limited))
	{
		if (!bCSV)
		{
			start = limited;
			// 検索の終了
			return CString();
		}

		start = nStart + 1;
		INT end = targ.Find("\r\n", start);
		if (end == -1 || !(start < limited))
		{
			start = limited;
			// 検索の終了
			return CString();
		}
		INT nFirst = start + 1;
		INT nCount = end - start - 2;
		start = end;

		// 発見した文字列
		return targ.Mid(nFirst, nCount);
	}

	// 文字列の終了
	for (INT end = start + 1; (end = targ.Find(e, end)) != -1; end++)
	{
		CHAR cCheck = targ[end + 1];
		if (cCheck == e[0])
		{
			end++;
			continue;
		}
		end++;
		INT nFirst = start + 1;
		INT nCount = end - start - 2;
		start = end;

		// 発見した文字列
		return targ.Mid(nFirst, nCount);
	}

	start = -1;

	// 検索の終了
	return CString();
}
