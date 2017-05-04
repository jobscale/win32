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

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "pProcess.h"
#include "pUtility.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

PNPDATA::PNPDATA()
	: dwPID(0)
	, wTask(0)
	, begin(COleDateTime::GetCurrentTime())
{
}

BOOL PNPDATA::operator==(const PNPDATA& other) const
{
	CoString text;
	text.Format("\0 %d|%d|%s" + 2, dwPID, wTask, (LPCTSTR)szProcess);

	CoString textOther;
	textOther.Format("\0 %d|%d|%s" + 2, other.dwPID, other.wTask, (LPCTSTR)other.szProcess);

	return text.CompareNoCase(textOther) == 0;
}

typedef BOOL (CALLBACK *PROCENUMPROC)(DWORD, WORD, LPCSTR, LPARAM);

// 列挙情報構造体
struct EnumInfoStruct
{
	DWORD dwPID;
	PROCENUMPROC lpProc;
	DWORD lParam;
	BOOL bEnd;
};

// プロセス強制終了
DWORD DieProcess(PNPDATA& pndata, ReportUseActionProc ReportUseAction)
{
	if (ReportUseAction == NULL)
	{
		return -1;
	}

	int killed = IsSuppressionRun(pndata.szProcess);
	// 期限切れのときは制御をおこなわない
	if (killed && IsExpireLicenseDateTime())
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pndata.dwPID);
		if (hProcess)
		{
			TerminateProcess(hProcess, -1);
			CloseHandle(hProcess);

			// 強制終了したプロセスの通知
			if (ReportUseAction)
			{
				// "pc_stop" 個別PCの自動停止 (pc)
				// "ad_stop" 管理設定の自動停止 (admin)
				// "se_stop" サーバ設定の自動停止 (server)

				ReportUseAction(killed == 1 ? "pc_stop" : killed == 2 ? "ad_stop" : "se_stop", pndata.szProcess);
			}
		}
	}

	return 0;
}

// プロセス情報の取得
BOOL CALLBACK ProcessEnumerator32(DWORD dwPID, WORD wTask, LPCSTR szProcess, LPARAM lParam)
{
	CRITICALTRACE(pProcess, ProcessEnumerator32);

	CString path = szProcess;
	if (path[path.GetLength() - 1] == 0x10)
	{
		((LPSTR)(LPCTSTR)path)[path.GetLength() - 1] = 0x00;
		path.ReleaseBuffer();
	}

	LPVOID* pDataList = (LPVOID*)lParam;
	CoProcessArray* process = (CoProcessArray*)pDataList[0];
	ReportUseActionProc ReportUseAction = (ReportUseActionProc)pDataList[1];

	PNPDATA pndata;
	pndata.dwPID = dwPID;
	pndata.wTask = wTask;
	pndata.szProcess = path;
	process->Add(pndata);

	// DIE process
	DieProcess(pndata, ReportUseAction);

	return TRUE;
}

CCriticalSection& GetProcessCriticalSection()
{
	static CCriticalSection s_section;

	return s_section;
}

CoProcessArray& GetProcessCtrl()
{
	static CoProcessArray s_process;

	return s_process;
}

BOOL IsIgnoreAnalizeProcess(const CoString& szProcess)
{
	static const LPCTSTR ignoreList[] =
	{
		PP(pExTrapper.exe), PP(pTools.exe), PP(pCheckWindow.exe), PP(pControler.exe),

			"pLauncher.exe",
			"pPower.exe",
			"explorer.exe",
			"MSASCui.exe",
			"SLsvc.exe",
			"SearchFilterHost.exe",
			"SearchIndexer.exe",
			"SearchProtocolHost.exe",
			"System",
			"TSVNCache.exe",
			"[System Process]",
			"alg.exe",
			"audiodg.exe",
			"csrss.exe",
			"ctfmon.exe",
			"dwm.exe",
			"hqtray.exe",
			"ieuser.exe",
			"logonui.exe",
			"lsass.exe",
			"lsm.exe",
			"msiexec.exe",
			"services.exe",
			"smss.exe",
			"spoolsv.exe",
			"svchost.exe",
			"taskeng.exe",
			"tp4serv.exe",
			"updater.exe",
			"userinit.exe",
			"wininit.exe",
			"winlogon.exe",
			"wscntfy.exe",
			"wuauclt.exe",
			"AppleMobileDeviceHelper.exe",
			"AppleMobileDeviceService.exe",
			"hkcmd.exe",
			"dllhost.exe",
			NULL,
	};

	for (DWORD i = 0; ignoreList[i]; i++)
	{
		if (szProcess.CompareNoCase(ignoreList[i]) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

DWORD MergeProcess(CoProcessArray& new_process)
{
	CRITICALTRACE(pProcess, MergeProcess);

	static CoProcessArray s_base_process;

	CoCriticalSectionCtrl critical(GetProcessCriticalSection());

	CoProcessArray& analyze = GetProcessCtrl();

	COleDateTime now = COleDateTime::GetCurrentTime();

	for (DWORD i = 0, count = new_process.GetSize(); i < count; i++)
	{
		PNPDATA& pnpdata = new_process.ElementAt(i);

		if (pnpdata.szProcess.FindNoCase(".tmp") != -1)
		{
			continue;
		}

		pnpdata.szProcess.Replace("[1]", "");
		pnpdata.szProcess.Replace("[2]", "");
		pnpdata.szProcess.Replace("[3]", "");
		pnpdata.szProcess.Replace("[4]", "");
		pnpdata.szProcess.Replace(" (1)", "");
		pnpdata.szProcess.Replace(" (2)", "");
		pnpdata.szProcess.Replace(" (3)", "");
		pnpdata.szProcess.Replace(" (4)", "");
		pnpdata.szProcess.Replace("(1)", "");
		pnpdata.szProcess.Replace("(2)", "");
		pnpdata.szProcess.Replace("(3)", "");
		pnpdata.szProcess.Replace("(4)", "");
		pnpdata.szProcess.Replace("%%20", " ");
		pnpdata.szProcess.Replace("%%", "");

		INT pos = s_base_process.Find(pnpdata);
		if (pos != -1)
		{
			pnpdata.begin = s_base_process.ElementAt(pos).begin;
		}

		if (!IsIgnoreAnalizeProcess(pnpdata.szProcess))
		{
			pos = analyze.Find(pnpdata);
			if (pos == -1)
			{
				analyze.Add(pnpdata);
			}
			else
			{
				PNPDATA& anael = analyze.ElementAt(pos);
				anael.span = now - anael.begin;
			}
		}
	}

	s_base_process = new_process;

	return 0;
}

INT GetEnumProcess(CoProcessArray& process, ReportUseActionProc ReportUseAction/* = NULL*/)
{
	CRITICALTRACE(pProcess, GetEnumProcess);

	CoCriticalSectionCtrl critical(GetProcessCriticalSection());

	LPVOID param[] = { &process, ReportUseAction };

	BOOL WINAPI EnumProcs(PROCENUMPROC lpProc, LPARAM lParam);
	EnumProcs(ProcessEnumerator32, (LPARAM)param);

	MergeProcess(process);

	return 0;
}

// 起動制御、条件指定
int IsSuppressionRun(LPCTSTR lpCommandLine)
{
	BOOL IsIgnoreAnalizeProcess(const CoString& szProcess);
	CoString target = lpCommandLine;
	if (IsIgnoreAnalizeProcess(target))
	{
		return 0;
	}

	CoString resData;
	CoStringArray value;

	// サーバ設定の自動停止
	DWORD resVer = 0;
	GetSuppresAutoList(resData, resVer);
	GetArrayValue(value, resData, "\\,", TRUE);
	if (IsFindArray(lpCommandLine, value))
	{
		return 3;
	}

	value.RemoveAll();

	// 管理設定の自動停止
	GetSuppressionList(resData);
	GetArrayValue(value, resData, "\\,", TRUE);
	if (IsFindArray(lpCommandLine, value))
	{
		return 2;
	}

	value.RemoveAll();

	// 個別PCの自動停止
	GetKillingList(resData);
	GetArrayValue(value, resData, "\\,", TRUE);
	if (IsFindArray(lpCommandLine, value))
	{
		return 1;
	}

	return 0;
}

// 起動制御、条件指定
BOOL IsRequiredRun(LPCTSTR lpCommandLine)
{
	CString work;
	MyGetProfileString(P("Setting"), P("target_process_list"), work.GetBuffer(2048), 2048);
	work.ReleaseBuffer();
	work.Replace("\r\n", "\\,");

	CoStringArray value;
	GetArrayValue(value, work, "\\,", TRUE);
	if (!IsFindArray(lpCommandLine, value))
	{
		return TRUE;
	}

	CoProcessArray process;
	GetEnumProcess(process);

	CoStringArray enumlist;
	for (INT i = 0, count = process.GetSize(); i < count; i++)
	{
		enumlist.Add(process[i].szProcess);
	}

	MyGetProfileString(P("Setting"), P("process_required_list"), work.GetBuffer(2048), 2048);
	work.ReleaseBuffer();
	work.Replace("\r\n", "\\,");

	GetArrayValue(value, work, "\\,", TRUE);

	if (!IsFindArrayAsArray(enumlist, value, 2))
	{
		return FALSE;
	}

	return TRUE;
}
