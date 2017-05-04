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

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

typedef HANDLE HDIB;
typedef const HANDLE HDIBC;

HMODULE LoadIMGCTL()
{
	static HMODULE hModule = NULL;
	if (!hModule)
	{
		CoString modulePath = GetModuleFolder();
		hModule = LoadLibrary(modulePath + PP(imgctl.dll));
	}
	return hModule;
}

HDIB WINAPI ToDIB(LPCSTR lpImageFile)
{
	CRITICALTRACE(imgctl, ToDIB);

	typedef HDIB (WINAPI *TODIB)(LPCSTR);
	HMODULE hModule = LoadIMGCTL();
	if (hModule)
	{
		TODIB _ToDIB = (TODIB)GetProcAddress(hModule, ("\0 ToDIB") + 2);
		if (_ToDIB)
		{
			return _ToDIB(lpImageFile);
		}
	}
	return NULL;
}

BOOL WINAPI DeleteDIB(HDIB hDIB)
{
	CRITICALTRACE(imgctl, DeleteDIB);

	typedef BOOL (WINAPI *DELETEDIB)(HDIB);
	HMODULE hModule = LoadIMGCTL();
	if (hModule)
	{
		DELETEDIB _DeleteDIB = (DELETEDIB)GetProcAddress(hModule, ("\0 DeleteDIB") + 2);
		if (_DeleteDIB)
		{
			return _DeleteDIB(hDIB);
		}
	}
	return FALSE;
}

BOOL WINAPI DIBtoJPG(LPCSTR lpJpegFile, HDIBC hDIB, int iQuality, BOOL bProgression)
{
	CRITICALTRACE(imgctl, DIBtoJPG);

	typedef BOOL (WINAPI *DIBTOJPG)(LPCSTR, HDIBC, int, BOOL);
	HMODULE hModule = LoadIMGCTL();
	if (hModule)
	{
		DIBTOJPG _DIBtoJPG = (DIBTOJPG)GetProcAddress(hModule, ("\0 DIBtoJPG") + 2);
		if (_DIBtoJPG)
		{
			return _DIBtoJPG(lpJpegFile, hDIB, iQuality, bProgression);
		}
	}
	return FALSE;
}

/* 画像ファイルをJPEGファイルにする */
BOOL ToJPG(LPCSTR lpJpegFile, LPCSTR lpImageFile, int iQuality, BOOL bProgression)
{
	CRITICALTRACE(imgctl, ToJPG);

	BOOL bRes = FALSE;
	HDIB hDIB = NULL;

	hDIB = ToDIB(lpImageFile);
	if (hDIB != NULL)
	{
		bRes = DIBtoJPG(lpJpegFile, hDIB, iQuality, bProgression);
		DeleteDIB(hDIB);
	}

	return bRes;
}
