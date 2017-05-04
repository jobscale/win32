////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2011 Plustar Corporation. All rights reserved. プラスター
//
// ┌──┐┌┐    ┌┐┌┐┌──┐┌──┐┌──┐┌──┐
// └─┐│││    │││││┌─┘└┐┌┘│┌┐││┌┐│
// ┌─┘│││    │││││└─┐  ││  │└┘││└┴┘
// │┌─┘││    ││││└─┐│  ││  │┌┐││┌┬┐
// ││    │└─┐│└┘│┌─┘│  ││  ││││││││
// └┘    └──┘└──┘└──┘  └┘  └┘└┘└┘└┘
//
// Powered by jobscale
// Since 22 July 1973
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

COleDateTimeSpan _span(0, 0, 1, 10);
COleDateTime _last = COleDateTime::GetCurrentTime() + _span;

int CheckHookTime()
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	if (now > _last)
	{
		return 1;
	}

	return 0;
}

POINT _point = { 0 };

int UpdateHookTime()
{
	POINT point = { 0 };
	GetCursorPos(&point);

	if (MAKELONG(_point.x, _point.y) != MAKELONG(point.x, point.y))
	{
		_point = point;
		_last = COleDateTime::GetCurrentTime() + _span;
	}

	return 0;
}
