///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#ifndef __OPROCESS_H__
#define __OPROCESS_H__

#include <wininet.h>
#include <afxdisp.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// 宣言

struct PNPDATA
{
	DWORD dwPID;
	WORD wTask;
	CoString szProcess;
	COleDateTime begin;
	COleDateTimeSpan span;

	PNPDATA();
	BOOL operator==(const PNPDATA& other) const;
};

class CoProcessArray : public CoArray<PNPDATA>
{
};

int IsSuppressionRun(LPCTSTR lpCommandLine);
BOOL IsRequiredRun(LPCTSTR lpCommandLine);
typedef HRESULT (* ReportUseActionProc)(LPCTSTR kind, LPCTSTR data);
HRESULT ReportUseAction(LPCTSTR kind, LPCTSTR data);
INT GetEnumProcess(CoProcessArray& process, ReportUseActionProc ReportUseAction = NULL);

#endif // __OPROCESS_H__

