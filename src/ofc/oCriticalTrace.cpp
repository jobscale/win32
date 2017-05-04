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
#include "oCriticalTrace.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

#include <afxdisp.h>

#include "oProfileCtrl.h"

// Speed Trace ////////////////////////////////////////////////////////////////////////////////////

// 所要時間計測クラスの配列管理クラス
class CoArraySpeedCounter : public CoArray<SPEEDCOUNTER>
{
protected:
	CoString m_filePath;
public:
	CoArraySpeedCounter()
		: m_filePath(CoCriticalTrace::GetLogFolder())
	{
	}
	virtual ~CoArraySpeedCounter()
	{
		CoString fileName;
		fileName.Format("%s%04X", m_filePath, INFINITE);
		CStdioFile f;
		if (!f.Open(fileName, CFile::modeReadWrite | CFile::shareDenyWrite | CFile::modeCreate | CFile::modeNoTruncate))
		{
			return;
		}

		CoString fstr;

		f.SeekToEnd();
		f.WriteString(COleDateTime::GetCurrentTime().Format("%Y/%m/%d %H:%M:%S") + "\n");
		for (INT i = 0, count = (INT)GetSize(); i < count; i++)
		{
			SPEEDCOUNTER& rTarget = GetAt(i);

			fstr.Format("speed counter %8d total %6.1f [%s]\n", rTarget.count, DOUBLE(rTarget.time) / 1000, rTarget.name);
			f.WriteString(fstr);
		}
		fstr.Format("speed counter --------------------------------------------------------------\n");
		f.WriteString(fstr);
	}
	void FindSet(LPSPEEDCOUNTER pSc, BOOL add = TRUE)
	{
		INT count = (INT)GetSize();
		for (INT i = 0; i < count; i++)
		{
			SPEEDCOUNTER& rTarget = operator[](i);
			if (!pSc->name.Compare(rTarget.name))
			{
				rTarget.count += add ? 1 : 0;
				rTarget.time += pSc->time;
				return;
			}
		}
		pSc->count += add ? 1 : 0;
		Add(*pSc);
	}
};

// 所要時間計測の開始
CoSpeedCounter::CoSpeedCounter(LPCTSTR name)
	: m_name(name)
	, m_in(timeGetTime())
{
	SPEEDCOUNTER sc(m_name, 0);
	GetArraySpeedCounter().FindSet(sc, FALSE);
}

// 所要時間計測の終了
CoSpeedCounter::~CoSpeedCounter()
{
	SPEEDCOUNTER sc(m_name, timeGetTime() - m_in);
	GetArraySpeedCounter().FindSet(sc);
}

/*static */CoArraySpeedCounter& CoSpeedCounter::GetArraySpeedCounter()
{
	static CoArraySpeedCounter arrayoSpeedCounter;
	return arrayoSpeedCounter;
}

// Speed Trace ////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

// クリティカルセクションのロック
CoCriticalSectionCtrl::CoCriticalSectionCtrl(CCriticalSection& critical)
	: m_critical(critical)
{
	m_critical.Lock();
}

// クリティカルセクションのアンロック
CoCriticalSectionCtrl::~CoCriticalSectionCtrl()
{
	m_critical.Unlock();
}

// イベントの非シグナル化
CoAutoEvent::CoAutoEvent(HANDLE hEvent, DWORD dwMilliseconds/* = INFINITE*/)
	: m_hEvent(hEvent)
	, m_dwResult(ERROR_SUCCESS)
{
	if (m_hEvent)
	{
		m_dwResult = WaitForMultipleObjects(1, &m_hEvent, TRUE, dwMilliseconds);
		if (m_dwResult != ERROR_SUCCESS)
		{
			return;
		}
		ResetEvent(m_hEvent);
	}
}

// イベントのシグナル化
CoAutoEvent::~CoAutoEvent()
{
	if (m_hEvent)
	{
		SetEvent(m_hEvent);
	}
}

DWORD CoAutoEvent::GetResult()
{
	return m_dwResult;
}

// コールスタック情報構造体
struct CALLSTACKDATA
{
	CoString className;
	CoString functionName;
	COleDateTime dateTime;
	CALLSTACKDATA(LPCTSTR _className = NULL, LPCTSTR _functionName = NULL, COleDateTime _dateTime = COleDateTime::GetCurrentTime())
		: className(_className)
		, functionName(_functionName)
		, dateTime(_dateTime)
	{
	}
};

// コールスタック情報配列
class CallStack : public CoArray<CALLSTACKDATA>
{
public:
	CallStack& operator=(const CallStack& other)
	{
		RemoveAll();
		for (INT i = 0, count = (INT)other.GetSize(); i < count; i++)
		{
			Add(CALLSTACKDATA(other[i]));
		}
		return *this;
	}
};

// スレッド管理構造体
struct THREADDATA
{
	DWORD nID;
	INT lineCount;
	CallStack callStack;
	THREADDATA(DWORD _nID = -1, INT _lineCount = 0)
		: nID(_nID)
		, lineCount(_lineCount)
	{
	}
	THREADDATA(const THREADDATA& other)
	{
		*this = other;
	}
	BOOL operator<(const THREADDATA& other) const
	{
		return nID < other.nID;
	}
	BOOL operator>(const THREADDATA& other) const
	{
		return nID > other.nID;
	}
	BOOL operator==(const THREADDATA& other) const
	{
		return nID == other.nID;
	}
	BOOL operator!=(const THREADDATA& other) const
	{
		return !(*this == other);
	}
	THREADDATA& operator=(const THREADDATA& other)
	{
		nID = other.nID;
		lineCount = other.lineCount;
		callStack = other.callStack;

		return *this;
	}
};

// スレッド制御クラス
class CoThreadCtrl : public CoConstSortArray<THREADDATA>
{
public:
	virtual ~CoThreadCtrl()
	{
		for (INT i = 0, count = (INT)GetSize(); i < count; i++)
		{
			if ((INT)operator[](i).nID < 0)
			{
				continue;
			}
			DeleteFile(CoCriticalTrace::GetLogfileName(operator[](i).nID));
		}
	}
	INT GetLineCount()
	{
		INT nIndex = FindCertainty(GetCurrentThreadId());

		return operator[](nIndex).lineCount;
	}
	void Inc()
	{
		INT nIndex = FindCertainty(GetCurrentThreadId());

		operator[](nIndex).lineCount++;
	}
	void Dec()
	{
		INT nIndex = FindCertainty(GetCurrentThreadId());

		operator[](nIndex).lineCount--;
	}
	void IncCallStack(LPCTSTR className, LPCTSTR functionName)
	{
		INT nIndex = FindCertainty(GetCurrentThreadId());

		operator[](nIndex).callStack.Add(CALLSTACKDATA(className, functionName, COleDateTime::GetCurrentTime()));
	}
	void DecCallStack(LPCTSTR className, LPCTSTR functionName)
	{
		INT nIndex = FindCertainty(GetCurrentThreadId());

		INT nStack = (INT)operator[](nIndex).callStack.GetSize() - 1;
		ASSERT(!operator[](nIndex).callStack[nStack].className.Compare(className));
		ASSERT(!operator[](nIndex).callStack[nStack].functionName.Compare(functionName));
		operator[](nIndex).callStack.RemoveAt(nStack);
	}
	INT GetCallStack()
	{
		INT nIndex = FindCertainty(GetCurrentThreadId());

		return (INT)operator[](nIndex).callStack.GetSize();
	}
	INT FindCertainty(DWORD nID)
	{
		THREADDATA threadData(nID);
		INT nIndex = Find(threadData);
		if (nIndex != -1) return nIndex;
		nIndex = Add(threadData);
		return nIndex;
	}
	void TraceCallStack(CoCriticalTrace& criticalTrace)
	{
		INT nCurrent = FindCertainty(GetCurrentThreadId());
		for (INT nIndex = 0, count = (INT)GetSize(); nIndex < count; nIndex++)
		{
			for (INT i = 0, count = (INT)operator[](nIndex).callStack.GetSize(); i < count; i++)
			{
				if (!i)
				{
					if (nIndex == nCurrent)
					{
						criticalTrace << "current thread ";
					}
					criticalTrace << "---------------------------------------------------------------------- CallStack before\n";
				}

				CALLSTACKDATA& trace = operator[](nIndex).callStack[i];
				CoString text;
				text.Format("[%s] %s::$s\n", trace.dateTime.Format("%Y/%m/%d %H:%M:%S"), trace.className, trace.functionName);
				criticalTrace << text;

				if (i == count - 1)
				{
					if (nIndex == nCurrent)
					{
						criticalTrace << "current thread ";
					}
					criticalTrace << "---------------------------------------------------------------------- CallStack after\n";
				}
			}
		}
	}
};

// flags information
//  0x001 Trace on.
//  0x002 Time required measurement.
//  0x004 
//  0x008 Trace callstack.
//  0x010 Trace program source information.
//  0x020 Remove traced file.
//  0x040 Trace OutputDebugString.
//  0x080 Trace Output file

// 関数の開始
CoCriticalTrace::CoCriticalTrace(LPCTSTR name, INT line, LPCTSTR clname/* = NULL*/, LPCTSTR funame/* = NULL*/, DWORD flag/* = 0x000*/, DWORD flagex/* = 0x000*/)
	: m_flag(0x000)
	, m_name(name)
	, m_line(line)
	, m_clname(clname)
	, m_funame(funame)
	, m_constructor(timeGetTime())
	, m_speedCounter(NULL)
	, m_pWnd(NULL)
{
	CoCriticalSectionCtrl critical(GetCriticalSection());

	m_flag = flagex ? flagex : (flag | ReadFlag());

	if (m_flag & 0x002)
	{
		m_speedCounter = new CoSpeedCounter(CoString(clname) + "::" + funame);
	}

	GetThreadCtrl().IncCallStack(m_clname, m_funame);

	Constructor();
}

// 関数の終了
CoCriticalTrace::~CoCriticalTrace()
{
	CoCriticalSectionCtrl critical(GetCriticalSection());

	if (m_speedCounter)
	{
		delete m_speedCounter;
	}

	GetThreadCtrl().DecCallStack(m_clname, m_funame);

	Destructor();

	if (m_flag & 0x020)
	{
		if (GetThreadCtrl().GetCallStack() == 0)
		{
			DeleteFile(GetLogfileName());
		}
	}
}

// フラグの取得
/*static */DWORD CoCriticalTrace::ReadFlag(BOOL bRefresh/* = FALSE*/)
{
	static DWORD flag = 0x001 | 0x008; // CoProfile::GetProfileInt("\0 Tools" + 2, "\0 ApplicationTrace" + 2, 0);

	if (bRefresh)
	{
		flag = CoProfile::GetProfileInt("\0 Tools" + 2, "\0 ApplicationTrace" + 2, 0);
	}

	return flag;
}

void CoCriticalTrace::Constructor()
{
	// 出力設定のない場合
	if (!(m_flag & 0x001)) return;

	if (m_flag & 0x008)
	{
		*this << m_clname + "::" + m_funame + "\n";
		*this << "{\n";
		GetThreadCtrl().Inc();
	}

	if (m_flag & 0x010)
	{
		CoString fstr;
		fstr.Format("%s(%d): [%s %.4f] ... in\n", m_name, m_line, COleDateTime::GetCurrentTime().Format("%Y/%m/%d %H:%M:%S"), (DOUBLE)m_constructor / 10000);
		*this << fstr;
	}
}

void CoCriticalTrace::Destructor()
{
	// 出力設定のない場合
	if (!(m_flag & 0x001)) return;

	DWORD destructor = timeGetTime();

	if (m_flag & 0x010)
	{
		CoString fstr;
		fstr.Format("%s(%d): [%s %.4f] ... out\n", m_name, m_line, COleDateTime::GetCurrentTime().Format("%Y/%m/%d %H:%M:%S"), (DOUBLE)destructor / 10000);
		*this << fstr;
	}

	if (m_flag & 0x008)
	{
		GetThreadCtrl().Dec();
		DWORD getTime = destructor;
		if (getTime >= m_constructor)
		{
			getTime -= m_constructor;
		}
		CoString fstr;
		fstr.Format("}\t\t\t[%.3f]\n", (DOUBLE)getTime / 1000);
		*this << fstr;
	}
}

// 標準トレース出力
CoCriticalTrace& CoCriticalTrace::operator<<(LPCTSTR src)
{
	CoCriticalSectionCtrl critical(GetCriticalSection());

	if (!(m_flag & 0x001)) return *this;

	CoString scope;
	if (m_flag & 0x008)
	{
		for (INT i = 0, lineCount = GetThreadCtrl().GetLineCount(); i < lineCount; i++)
		{
			scope += " ";
		}
	}
	scope += src;

	if (m_flag & 0x040)
	{
		OutputDebugString(scope);
	}

	if (m_flag & 0x080)
	{
		CStdioFile f;
		if (f.Open(GetLogfileName(), CFile::modeReadWrite | CFile::shareDenyWrite | CFile::modeCreate | CFile::modeNoTruncate))
		{
			f.SeekToEnd();

			f.WriteString(scope);

			f.Close();
		}
	}

	return *this;
}

// 日時トレース出力
CoCriticalTrace& CoCriticalTrace::operator<<(COleDateTime& src)
{
	return *this << src.Format("%Y/%m/%d %H:%M:%S");
}

// 数値トレース出力
CoCriticalTrace& CoCriticalTrace::operator<<(HANDLE src)
{
	CString work;
	work.Format("0x%08X", src);
	return *this << work;
}

// コールスタックの出力
CoCriticalTrace& CoCriticalTrace::TraceCallStack()
{
	CoCriticalSectionCtrl critical(GetCriticalSection());

	GetThreadCtrl().TraceCallStack(*this);

	return *this;
}

LPCTSTR GetDefaultLogFolder()
{
	static CoString localpath;
	if (localpath.IsEmpty())
	{
		BOOL bRet = SHGetSpecialFolderPath(NULL, localpath.GetBuffer(MAX_PATH), CSIDL_COMMON_APPDATA, 0);
		localpath.ReleaseBuffer();
		if (!bRet || localpath.IsEmpty())
		{
			localpath = "\0 C:\\Documents and Settings\\All Users\\Application Data" + 2;
		}
		localpath += "\0 \\pLook" + 2;
		CreateDirectory(localpath, NULL);
		localpath += "\0 \\pTrace" + 2;
		CreateDirectory(localpath, NULL);
		localpath += "\0 \\thre" + 2;
	}
	return localpath;
}

/*static */CoString CoCriticalTrace::GetLogFolder()
{
	static CoString path = CoProfile::GetProfileString("\0 Tools" + 2, "\0 ApplicationTracePath" + 2, GetDefaultLogFolder());
	return path;
}

// トレース出力するファイル名の取得
/*static */CoString CoCriticalTrace::GetLogfileName(DWORD nID/* = -1*/)
{
	if (nID == -1)
	{
		nID = GetCurrentThreadId();
	}

	CoString logname;
	logname.Format("%s%04X.txt", GetLogFolder(), nID);
	return logname;
}

// クリティカルセクションコントロールの取得
/*static */CCriticalSection& CoCriticalTrace::GetCriticalSection()
{
	static CCriticalSection critical;
	return critical;
}

// スレッドコントロールの取得
/*static */CoThreadCtrl& CoCriticalTrace::GetThreadCtrl()
{
	static CoThreadCtrl thread;
	return thread;
}

static LPCTSTR ConvertSimpleException(DWORD dwExcept)
{
	switch (dwExcept)
	{
	case STATUS_WAIT_0: return "Succeeded.";
	case EXCEPTION_ACCESS_VIOLATION: return "Access Violation";
 	case EXCEPTION_DATATYPE_MISALIGNMENT: return "Datatype Misalignment";
 	case EXCEPTION_BREAKPOINT: return "Breakpoint";
 	case EXCEPTION_SINGLE_STEP: return "Single Step";
 	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "Array Bounds Exceeded";
 	case EXCEPTION_FLT_DENORMAL_OPERAND: return "Flt Denormal Operand";
 	case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "Flt Divide By ZERO";
 	case EXCEPTION_FLT_INEXACT_RESULT: return "Flt Inexact Result";
 	case EXCEPTION_FLT_INVALID_OPERATION: return "Flt Invalid Operation";
 	case EXCEPTION_FLT_OVERFLOW: return "Flt Overflow";
 	case EXCEPTION_FLT_STACK_CHECK: return "Flt Stack Check";
 	case EXCEPTION_FLT_UNDERFLOW: return "Flt Underflow";
 	case EXCEPTION_INT_DIVIDE_BY_ZERO: return "Int Divide By Zero";
 	case EXCEPTION_INT_OVERFLOW: return "Int Overflow";
 	case EXCEPTION_PRIV_INSTRUCTION: return "Priv Instruction";
 	case EXCEPTION_IN_PAGE_ERROR: return "In Page Error";
 	case EXCEPTION_ILLEGAL_INSTRUCTION: return "Illegal Instruction";
 	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Noncontinuable Exception";
	case EXCEPTION_STACK_OVERFLOW: return "Stack Overflow";
 	case EXCEPTION_INVALID_DISPOSITION: return "Invalid Disposition";
 	case EXCEPTION_GUARD_PAGE: return "Guard Page";
 	case EXCEPTION_INVALID_HANDLE: return "Invalid Handle";
	default: break;
	}
	return "Unknown Exception";
}

void trans_func(unsigned int u, EXCEPTION_POINTERS* pExp);

#include <eh.h>

class CGlobal
{
protected:
	_se_translator_function m_pTrans_func;
public:
	CGlobal()
		: m_pTrans_func(_set_se_translator(trans_func))
	{
	}
	void Release()
	{
		VERIFY(_set_se_translator(m_pTrans_func) == trans_func);
		m_pTrans_func = NULL;
	}
};

CGlobal g_global;

void trans_func(unsigned int u, EXCEPTION_POINTERS* pExp)
{
	CRITICALTRACE_EX(global, trans_func, 0x001 | 0x008 | 0x010 | 0x080);

	g_global.Release();

	CoString text;
	text.Format("\0 [%s]Verification of Plustar pLook.\nEXCEPTION: [0x%08X] %s" + 2, COleDateTime::GetCurrentTime().Format("%Y/%m/%d %H:%M:%S"), u, ConvertSimpleException(u));

	criticalTrace << text << "\n";
	criticalTrace.TraceCallStack();
}
