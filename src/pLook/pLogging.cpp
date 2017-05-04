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

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "pUtility.h"

#include "pLook.h"

void StartLoggerTimer(HWND hWnd)
{
	CRITICALTRACE(pLook, StartLoggerTimer);

	SetTimer(hWnd, 1, 9 * 1000, NULL);
}

void EndLoggerTimer(HWND hWnd)
{
	CRITICALTRACE(pLook, EndLoggerTimer);

	KillTimer(hWnd, 1);
}
