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
#include "../oCommon/oSocketTools.h"

#include <psapi.h>

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "oWndFind.h"

#include "pUtility.h"

#include "oMyJson.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

CCriticalSection& GetForeCriticalSection()
{
	static CCriticalSection s_section;

	return s_section;
}

#define MAX_TITLE 1024
#define MAX_CNAME 1024
#define MAX_LINE 4096
#define MAX_TIME 24

COLUMN_WND* CoWndFind::Add()
{
	CoCriticalSectionCtrl critical(GetForeCriticalSection());

	HWND hWnd = GetForegroundWindow();

	if (!hWnd)
	{
		CoString receive;
		SendToTcpSocket(P("127.0.0.2"), 12087, P("GET FOREGROUND WINDOW."), &receive);
		if (receive.Find(P("RESULT ")) == -1)
		{
			return NULL;
		}

		INT pos = 0;
		HWND hWnd = (HWND)strtoul(receive.AbstractSearchSubstance(pos), NULL, 16);
		DWORD pid = strtoul(receive.AbstractSearchSubstance(pos), NULL, 16);
		HMODULE hModule = (HMODULE)strtoul(receive.AbstractSearchSubstance(pos), NULL, 16);
		CString title = receive.AbstractSearchSubstance(pos);
		CString className = receive.AbstractSearchSubstance(pos);
		CString path = receive.AbstractSearchSubstance(pos);

		title.Replace("\"", " ");
		title.Replace("%%20", " ");

		path.Replace("%%20", " ");
		path.Replace("%%", " ");

		return Add(hWnd, pid, hModule, title, className, path);
	}

	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);

	CoString path;
	HMODULE hModule = NULL;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
	if (hProcess)
	{
		FILETIME CreationTime, ExitTime, KernelTime, UserTime;
		GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime);

		DWORD mSize = 0;
		EnumProcessModules(hProcess, &hModule, sizeof HMODULE, &mSize);

		GetModuleBaseName(hProcess, hModule, path.GetBuffer(_MAX_PATH + 1), _MAX_PATH);
		path.ReleaseBuffer();

		CloseHandle(hProcess);

		CoString drive, dir, fname, ext;
		_splitpath(path, drive.GetBuffer(_MAX_DRIVE + 1), dir.GetBuffer(_MAX_DIR + 1), fname.GetBuffer(_MAX_FNAME + 1), ext.GetBuffer(_MAX_EXT + 1));
		drive.ReleaseBuffer(); dir.ReleaseBuffer(); fname.ReleaseBuffer(); ext.ReleaseBuffer();

		path = fname + ext;
	}

	CoString title;
	GetWindowText(hWnd, title.GetBuffer(MAX_TITLE + 1), MAX_TITLE);
	title.ReleaseBuffer();

	CoString className;
	GetClassName(hWnd, className.GetBuffer(MAX_CNAME + 1), MAX_CNAME);
	className.ReleaseBuffer();

	title.Replace("\"", " ");
	title.Replace("%%20", " ");

	path.Replace("%%20", " ");
	path.Replace("%%", " ");

	return Add(hWnd, pid, hModule, title, className, path);
}

COLUMN_WND* CoWndFind::Add(HWND hWnd, DWORD pid, HMODULE hModule, CString& title, const CString& className, const CString& path)
{
	CoCriticalSectionCtrl critical(GetForeCriticalSection());

	BOOL IsIgnoreAnalizeProcess(const CoString& szProcess);
	if (IsIgnoreAnalizeProcess(path))
	{
		return NULL;
	}

	COLUMN_WND* base = column_wnd_table_finding;
	COLUMN_WND* terminal = NULL;
	for (; base; base = base->next)
	{
		if (hWnd == base->hWnd)
		{
			break;
		}
		terminal = base;
	}

	if (!base)
	{
		base = new COLUMN_WND;

		base->begin = now - COleDateTimeSpan(0, 0, 0, 10);

		if (!terminal)
		{
			column_wnd_table_finding = base;
		}
		else
		{
			terminal->next = base;
			base->prev = terminal;
		}
	}

	base->pid = pid;
	base->hModule = hModule;
	base->hWnd = hWnd;
	base->path = path;
	base->title = title;
	base->className = className;
	base->last = now;
	base->span = base->last - base->begin;

	return base;
}

LRESULT CoWndFind::GetCopyData(COLUMN_WND** pCopy)
{
	CoCriticalSectionCtrl critical(GetForeCriticalSection());

	_asm // TODO:
	{
		int 3
	}

	COLUMN_WND* base = column_wnd_table_finding;
	COLUMN_WND* prev = NULL;

	for (; base; base = base->next)
	{
		*pCopy = new COLUMN_WND(*base);
		(*pCopy)->prev = prev;
		prev = *pCopy;
		(*pCopy)->next = NULL;

		pCopy = &(*pCopy)->next;
	}

	return 0;
}

LRESULT CoWndFind::GetStringData(CString& str)
{
	CoCriticalSectionCtrl critical(GetForeCriticalSection());

	COLUMN_WND* base = column_wnd_table_finding;
#ifdef USE_JSONCPP
	Json::Value *jVal = create_AValue();
	Json::Value *jWork = create_OValue();
#else
	str = "[ ";
#endif

#ifdef USE_JSONCPP
	for (int i = 0; base; base = base->next, i++)
#else
	for (; base; base = base->next)
#endif
	{
#ifdef USE_JSONCPP
		jWork->clear();

		CoString tmpstr;
		tmpstr.Format("%d", base->pid);
		(*jWork)["pid"] = tmpstr;
		tmpstr.Format("%X", base->hModule);
		(*jWork)["hModule"] = tmpstr;
		tmpstr.Format("%X", base->hWnd);
		(*jWork)["hWnd"] = tmpstr;
		(*jWork)["path"] = (LPCTSTR)base->path;
		(*jWork)["title"] = (LPCTSTR)base->title;
		(*jWork)["className"] = (LPCTSTR)base->className;
		(*jWork)["begin"] = (LPCTSTR)base->begin.Format("%Y/%m/%d %H:%M:%S");
		(*jWork)["span"] = (LPCTSTR)base->span.Format("%H:%M:%S");

		(*jVal)[i] = *jWork;
#else
		CString work;
		work.Format("{ \"pid\":\"%d\", \"hModule\":\"%X\", \"hWnd\":\"%X\", \"path\":\"%s\", \"title\":\"%s\", \"className\":\"%s\", \"begin\":\"%s\", \"span\":\"%s\" }, \r\n",
			/*DWORD */base->pid,
			/*HMODULE */base->hModule,
			/*HWND */base->hWnd,
			/*CoString */(LPCTSTR)base->path,
			/*CoString */(LPCTSTR)base->title,
			/*CoString */(LPCTSTR)base->className,
			/*COleDateTime */(LPCTSTR)base->begin.Format("%Y/%m/%d %H:%M:%S"),
			/*COleDateTimeSpan */(LPCTSTR)base->span.Format("%H:%M:%S"));

		str += work;
#endif
	}
#ifdef USE_JSONCPP
	str = jVal->toFastString().c_str();

	jWork->release();
	jVal->release();
#else
	if (str.GetLength() >= 4)
	{
		str = str.Left(str.GetLength() - 4);
	}
	str += " ]";
#endif

	return 0;
}

LRESULT CoWndFind::Finalize(BOOL bFinish/* = FALSE*/)
{
	CoCriticalSectionCtrl critical(GetForeCriticalSection());

	COLUMN_WND* base = column_wnd_table_finding;
	CoString receive;
	if (!bFinish)
	{
		CString check = P("GET CLASS NAME. ");
		for (; base; base = base->next)
		{
			CString work;
			work.Format("\"0x%08X\" ", base->hWnd);
			check += work;
		}

		SendToTcpSocket(P("127.0.0.2"), 12087, check, &receive);
		if (receive.Find(P("RESULT ")) == -1)
		{
			return -1;
		}
	}

	int pos = 0;
	base = column_wnd_table_finding;
	for (; base; )
	{
		COLUMN_WND* next = base->next;

		if (!bFinish)
		{
			if (base->last != now)
			{
				HWND hWnd = (HWND)strtoul(receive.AbstractSearchSubstance(pos), NULL, 16);
				if (base->hWnd != hWnd)
				{
					break;
				}
				CString className = receive.AbstractSearchSubstance(pos);
				if (!className.IsEmpty())
				{
					base->last = now;
				}
			}
		}

		if (bFinish || base->last != now)
		{
			if (base->prev)
			{
				base->prev->next = base->next;
			}
			if (base->next)
			{
				base->next->prev = base->prev;
			}
			if (base == column_wnd_table_finding)
			{
				column_wnd_table_finding = base->next;
			}
			base->prev = NULL;
			base->next = NULL;

			if (!column_wnd_table_fixed)
			{
				column_wnd_table_fixed = base;
			}
			else
			{
				COLUMN_WND* terminal = column_wnd_table_fixed;
				for (; terminal->next; terminal = terminal->next)
				{
				}
				terminal->next = base;
				base->prev = terminal;
			}
		}

		base = next;
	}

	return 0;
}

LRESULT CoWndFind::FinalizeBase(BOOL bFinish/* = FALSE*/)
{
	CoCriticalSectionCtrl critical(GetForeCriticalSection());

	COLUMN_WND* base = column_wnd_table_finding;
	for (; base; )
	{
		COLUMN_WND* next = base->next;

		if (base->last != now)
		{
			CoString className;
			GetClassName(base->hWnd, className.GetBuffer(MAX_CNAME + 1), MAX_CNAME);
			className.ReleaseBuffer();
			if (!className.IsEmpty())
			{
				base->last = now;
			}
		}

		if (bFinish || base->last != now)
		{
			if (base->prev)
			{
				base->prev->next = base->next;
			}
			if (base->next)
			{
				base->next->prev = base->prev;
			}
			if (base == column_wnd_table_finding)
			{
				column_wnd_table_finding = base->next;
			}
			base->prev = NULL;
			base->next = NULL;

			if (!column_wnd_table_fixed)
			{
				column_wnd_table_fixed = base;
			}
			else
			{
				COLUMN_WND* terminal = column_wnd_table_fixed;
				for (; terminal->next; terminal = terminal->next)
				{
				}
				terminal->next = base;
				base->prev = terminal;
			}
		}

		base = next;
	}

	return 0;
}

void CoWndFind::TestOut()
{
	CRITICALTRACE(CoWndFind, TestOut);

	COLUMN_WND* base = column_wnd_table_finding;
	for (; base; base = base->next)
	{
		OutputViewer("[%s] (%s) %s", (LPCTSTR)base->path, (LPCTSTR)base->className, (LPCTSTR)base->title);
	}

	OutputViewer("----- %s -----", (LPCTSTR)COleDateTime::GetCurrentTime().Format("%Y/%m/%d %H:%M:%S"));
}

CoWndFind _cwf;

CoWndFind& GetWndFindCtrl()
{
	return _cwf;
}

DWORD WINAPI WinFindThread(LPVOID pParam)
{
	for (INT i = 0; ; i++)
	{
		if (1) // クリティカルセクション
		{
			_cwf.GetNow();

			_cwf.Add();

			if (i % 6 == 0)
			{
				_cwf.Finalize();
				i = 0;
			}

			// _cwf.TestOut();
		}

		Sleep(9 * 1000);
	}

	return 0;
}
