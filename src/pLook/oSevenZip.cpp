///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//
// $Date: $
// $Rev: $
// $Author: $
// $HeadURL: $
//
// $Id: $
//

#include "pLook.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

#include "oSevenZip.h"

SEVENZIPDATA::SEVENZIPDATA()
	: result(0)
	, hWnd(NULL)
{
}

typedef	HGLOBAL	HARC;
#define FNAME_MAX32		512
typedef struct {
	DWORD	dwOriginalSize;
	DWORD	dwCompressedSize;
	DWORD	dwCRC;
	UINT	uFlag;
	UINT	uOSType;
	WORD	wRatio;
	WORD	wDate;
	WORD	wTime;
	char	szFileName[FNAME_MAX32 + 1];
	char	dummy1[3];
	char	szAttribute[8];
	char	szMode[8];
} INDIVIDUALINFO, *LPINDIVIDUALINFO;

namespace seven_zip
{
	typedef BOOL (WINAPI* SevenZipGetRunningProp)();
	typedef int (WINAPI* SevenZipProp)(const HWND _hwnd, LPCSTR _szCmdLine, LPSTR _szOutput, const DWORD _dwSize);
	typedef HARC (WINAPI* OpenArchiveProp)(const HWND _hwnd, LPCSTR _szFileName, const DWORD _dwMode);
	typedef int (WINAPI* CloseArchiveProp)(HARC _harc);
	typedef int (WINAPI* FindFirstProp)(HARC _harc, LPCSTR _szWildName, INDIVIDUALINFO *_lpSubInfo);
	typedef int (WINAPI* FindNextProp)(HARC _harc, INDIVIDUALINFO *_lpSubInfo);

	SevenZipGetRunningProp SevenZipGetRunning = NULL;
	SevenZipProp SevenZip = NULL;
	OpenArchiveProp OpenArchive = NULL;
	CloseArchiveProp CloseArchive = NULL;
	FindFirstProp FindFirst = NULL;
	FindNextProp FindNext = NULL;

	INT InitSevenZip()
	{
		HMODULE hModule = LoadLibrary(CoString(GetModuleFolder()) + ("\0 7-zip32.dll" + 2));
		if (!hModule)
		{
			OutputViewer("\0 %s(%d): [0x%08X][0x%08X]\n" + 2, __FILE__, __LINE__, GetLastError(), WSAGetLastError());
			return -1;
		}

		SevenZipGetRunning = (SevenZipGetRunningProp)GetProcAddress(hModule, "\0 SevenZipGetRunning" + 2);
		SevenZip = (SevenZipProp)GetProcAddress(hModule, ("\0 SevenZip") + 2);
		OpenArchive = (OpenArchiveProp)GetProcAddress(hModule, ("\0 SevenZipOpenArchive") + 2);
		CloseArchive = (CloseArchiveProp)GetProcAddress(hModule, ("\0 SevenZipCloseArchive") + 2);
		FindFirst = (FindFirstProp)GetProcAddress(hModule, ("\0 SevenZipFindFirst") + 2);
		FindNext = (FindNextProp)GetProcAddress(hModule, ("\0 SevenZipFindNext") + 2);

		return 0;
	}
}

INT InitSevenZip()
{
	CRITICALTRACE(sevenzip, InitSevenZip);

	return seven_zip::InitSevenZip();
}

static INT CriticalSevenZip()
{
	static CCriticalSection criticalSection;

	CoCriticalSectionCtrl critical(criticalSection);

	Sleep(100); // 7z32.dll の access violation 対策 （どうしようもない!?）

	return 0;
}

BOOL SevenZipGetRunning()
{
	CRITICALTRACE(sevenzip, SevenZipGetRunning);

	CriticalSevenZip();

	return seven_zip::SevenZipGetRunning();
}

int SevenZip(const HWND _hwnd, LPCSTR _szCmdLine, LPSTR _szOutput, const DWORD _dwSize)
{
	CRITICALTRACE(sevenzip, SevenZip);

	CriticalSevenZip();

	return seven_zip::SevenZip(_hwnd, _szCmdLine, _szOutput, _dwSize);
}

HARC OpenArchive(const HWND _hwnd, LPCSTR _szFileName, const DWORD _dwMode)
{
	CRITICALTRACE(sevenzip, OpenArchive);

	CriticalSevenZip();

	return seven_zip::OpenArchive(_hwnd, _szFileName, _dwMode);
}

int CloseArchive(HARC _harc)
{
	CRITICALTRACE(sevenzip, CloseArchive);

	CriticalSevenZip();

	return seven_zip::CloseArchive(_harc);
}

int FindFirst(HARC _harc, LPCSTR _szWildName, INDIVIDUALINFO *_lpSubInfo)
{
	CRITICALTRACE(sevenzip, FindFirst);

	CriticalSevenZip();

	return seven_zip::FindFirst(_harc, _szWildName, _lpSubInfo);
}

int FindNext(HARC _harc, INDIVIDUALINFO *_lpSubInfo)
{
	CRITICALTRACE(sevenzip, FindNext);

	CriticalSevenZip();

	return seven_zip::FindNext(_harc, _lpSubInfo);
}

INT SevenZipWaitRunning()
{
	CRITICALTRACE(sevenzip, SevenZipWaitRunning);

	BOOL IsWait = FALSE;
	for (INT i = 0, count = 1000; i < count; i++)
	{
		IsWait = SevenZipGetRunning();
		if (!IsWait)
		{
			break;
		}
		Sleep(200);
	}
	if (IsWait)
	{
		OutputViewer("\0 %s(%d): [0x%08X][0x%08X]\n" + 2, __FILE__, __LINE__, GetLastError(), WSAGetLastError());
		return -1;
	}

	return 0;
}

INT SevenZipArchive(SEVENZIPDATA& data)
{
	CRITICALTRACE(sevenzip, SevenZipArchive);

	data.result = -1;
	for (INT i = 0, count = 100; i < count; i++)
	{
		if (SevenZipWaitRunning() != 0)
		{
			continue;
		}

		data.result = SevenZip(data.hWnd, data.cmdLine, data.output.GetBuffer(SHRT_MAX + 1), SHRT_MAX);
		data.output.ReleaseBuffer();
		if (data.result == 0)
		{
			break;
		}

		Sleep(200);
	}

	if (data.result != 0)
	{
		OutputViewer("\0 %s(%d): [0x%08X][0x%08X]\n" + 2, __FILE__, __LINE__, GetLastError(), WSAGetLastError());
		return -1;
	}

	return 0;
}

INT SevenZipGetList(SEVENZIPDATA& data)
{
	CRITICALTRACE(sevenzip, SevenZipGetList);

	data.arry.RemoveAll();

	HARC hArc = NULL;
	for (INT i = 0, count = 100; i < count; i++)
	{
		if (SevenZipWaitRunning() != 0)
		{
			continue;
		}

		hArc = OpenArchive(NULL, data.archive, 0);
		if (hArc)
		{
			break;
		}

		OutputViewer("\0 %s(%d): [0x%08X][0x%08X]\n" + 2, __FILE__, __LINE__, GetLastError(), WSAGetLastError());
		Sleep(200);
	}

	if (!hArc)
	{
		OutputViewer("\0 %s(%d): [0x%08X][0x%08X]\n" + 2, __FILE__, __LINE__, GetLastError(), WSAGetLastError());
		return -1;
	}

	INDIVIDUALINFO info = { 0 };
	if (FindFirst(hArc, ("\0 *") + 2, &info) != 0)
	{
		CloseArchive(hArc);

		OutputViewer("\0 %s(%d): [0x%08X][0x%08X]\n" + 2, __FILE__, __LINE__, GetLastError(), WSAGetLastError());
		return -1;
	}

	data.arry.Add(info.szFileName);

	for (; FindNext(hArc, &info) == 0; )
	{
		data.arry.Add(info.szFileName);
	}

	CloseArchive(hArc);

	return 0;
}
