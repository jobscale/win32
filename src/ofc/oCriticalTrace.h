///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__OCRITICALTRACE_H__)
#define __OCRITICALTRACE_H__

#include <atlbase.h>
#include <afxmt.h>

#include "oString.h"
#include "oArray.h"

#undef OFCL_API
#ifdef _OFC_EXPORT_
#define OFCL_API __declspec(dllexport)
#else
#define OFCL_API __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// トレース制御クラス

class OFCL_API CoCriticalSectionCtrl
{
protected:
	CCriticalSection& m_critical;

public:
	CoCriticalSectionCtrl(CCriticalSection& critical);
	virtual ~CoCriticalSectionCtrl();

};

/////////////////////////////////////////////////////////////////////////////
// イベント制御クラス

class OFCL_API CoAutoEvent
{
protected:
	HANDLE m_hEvent;
	DWORD m_dwResult;

public:
	CoAutoEvent(HANDLE hEvent, DWORD dwMilliseconds = INFINITE);
	virtual ~CoAutoEvent();
	DWORD GetResult();

};

/////////////////////////////////////////////////////////////////////////////
// 測定情報構造体

struct SPEEDCOUNTER
{
	CoString name;
	DWORD time;
	DWORD count;

	SPEEDCOUNTER(LPCTSTR _name = NULL, DWORD _time = 0)
		: name(_name)
		, time(_time)
		, count(0)
	{
	}

	operator SPEEDCOUNTER*()
	{
		return this;
	}

};
typedef SPEEDCOUNTER* LPSPEEDCOUNTER;

class CoArraySpeedCounter;

/////////////////////////////////////////////////////////////////////////////
// 測定制御クラス

class CoSpeedCounter
{
protected:
	CoString m_name;
	DWORD m_in;

public:
	CoSpeedCounter(LPCTSTR name);
	virtual ~CoSpeedCounter();

protected:
	static CoArraySpeedCounter& GetArraySpeedCounter();

};

class CoThreadCtrl;

class OFCL_API CoCriticalTrace
{
protected:
	DWORD m_flag;
	CoString m_name;
	INT m_line;
	CoString m_clname;
	CoString m_funame;
	DWORD m_constructor;
	CoSpeedCounter* m_speedCounter;
	CWnd* m_pWnd;

public:
	CoCriticalTrace(LPCTSTR name, INT line, LPCTSTR clname = NULL, LPCTSTR funame = NULL, DWORD flag = 0x000, DWORD flagex = 0x000);
	virtual ~CoCriticalTrace();

protected:
	void Constructor();
	void Destructor();

public:
	static DWORD ReadFlag(BOOL bRefresh = FALSE);
	static CoString GetLogFolder();
	static CoString GetLogfileName(DWORD nID = -1);
	CoCriticalTrace& operator<<(LPCTSTR src);
	CoCriticalTrace& operator<<(COleDateTime& src);
	CoCriticalTrace& operator<<(HANDLE src);
	CoCriticalTrace& TraceCallStack();

protected:
	static CCriticalSection& GetCriticalSection();
	static CoThreadCtrl& GetThreadCtrl();

};

#if defined(_DEBUG) || 0
#define CRITICALTRACE(clname, funame)
#else
#define CRITICALTRACE(clname, funame) \
	CoCriticalTrace criticalTrace(P(""), __LINE__, P(#clname), P(#funame), 0x001 | 0x008)
#endif

#define CRITICALTRACE_EX(clname, funame, flagex) \
	CoCriticalTrace criticalTrace(P(""), __LINE__, P(#clname), P(#funame), 0x000, flagex)

#endif // !defined(__OCRITICALTRACE_H__)
