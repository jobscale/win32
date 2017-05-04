///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#ifndef __OSOUND_H__
#define __OSOUND_H__

#include <mmsystem.h>

struct WAVE_DATA
{
	LPBYTE lpData;
	WAVEHDR wh;
	HWAVEOUT hwo;
	DWORD times;
};

struct POPWND_DATA
{
	HWND hWnd;
	HINSTANCE hInst;
	DWORD status;
	DWORD width;
	DWORD height;
	DWORD color;
	CString link;
	CString text;
	DWORD showSeconds;
	WAVE_DATA wd;

	HWND hWndBtClose;
};

HRESULT PlaySound(HWND hWnd, LPTSTR wavName);
BOOL CloseSound(HWND hWnd);

#endif // __OSOUND_H__

