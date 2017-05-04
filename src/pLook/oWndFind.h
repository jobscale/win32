///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#ifndef _OWND_FIND_H_
#define _OWND_FIND_H_

#include <afxmt.h>
#include <atlcomtime.h>

CCriticalSection& GetForeCriticalSection();

struct COLUMN_WND;
struct COLUMN_WND
{
	DWORD pid;
	HMODULE hModule;
	HWND hWnd;
	CoString path;
	CoString title;
	CoString className;
	COleDateTime begin;
	COleDateTime last;
	COleDateTimeSpan span;
	COLUMN_WND* prev;
	COLUMN_WND* next;

	COLUMN_WND()
		: pid(0)
		, hModule(NULL)
		, hWnd(NULL)
		, prev(NULL)
		, next(NULL)
	{
	}
};

class CoWndFind
{
private:
	COleDateTime now;
	COLUMN_WND* column_wnd_table_fixed;
	COLUMN_WND* column_wnd_table_finding;

public:
	CoWndFind()
		: now(COleDateTime::GetCurrentTime())
		, column_wnd_table_fixed(NULL)
		, column_wnd_table_finding(NULL)
	{
	}

	virtual ~CoWndFind()
	{
		Release();
	}

	LRESULT Release()
	{
		CoCriticalSectionCtrl critical(GetForeCriticalSection());

		COLUMN_WND* base = column_wnd_table_fixed;
		column_wnd_table_fixed = NULL;
		Release(base);

		base = column_wnd_table_finding;
		column_wnd_table_finding = NULL;
		Release(base);

		return 0;
	}

	LRESULT GetNow()
	{
		now = COleDateTime::GetCurrentTime();

		return 0;
	}

	LRESULT GetSize()
	{
		CoCriticalSectionCtrl critical(GetForeCriticalSection());

		LRESULT size = 0;
		COLUMN_WND* base = column_wnd_table_fixed;
		for (; base; )
		{
			size++;
			base = base->next;
		}
		return size;
	}

	COLUMN_WND* GetFirst()
	{
		return column_wnd_table_fixed;
	}

	COLUMN_WND* GetNext(COLUMN_WND* pcw)
	{
		return pcw->next;
	}

	LRESULT RemoveAll()
	{
		return Release();
	}

	LRESULT FinalizeAll()
	{
		CoCriticalSectionCtrl critical(GetForeCriticalSection());

		return Finalize(TRUE);
	}

private:
	LRESULT Release(COLUMN_WND* base)
	{
		CoCriticalSectionCtrl critical(GetForeCriticalSection());

		for (; base; )
		{
			COLUMN_WND* next = base->next;
			delete base;
			base = next;
		}

		return 0;
	}

public:
	COLUMN_WND* Add();
	COLUMN_WND* Add(HWND hWnd, DWORD pid, HMODULE hModule, CString& title, const CString& className, const CString& path);

	LRESULT GetCopyData(COLUMN_WND** pCopy);
	LRESULT GetStringData(CString& str);
	LRESULT Finalize(BOOL bFinish = FALSE);
	LRESULT FinalizeBase(BOOL bFinish = FALSE);
	void TestOut();

};

#endif // _OWND_FIND_H_
