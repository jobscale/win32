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

#include "oSound.h"

BOOL LoadSoundFile(LPTSTR lpszFile, LPWAVEFORMATEX lpwf, LPBYTE *lplpData, LPDWORD lpdwSize)
{
	HMMIO hmmio = mmioOpen(lpszFile, NULL, MMIO_READ);
	if (hmmio == NULL)
	{
		OutputViewer("ERROR: sound mmioOpen failed. [%s]", lpszFile);
		return FALSE;
	}

	MMCKINFO mmckRiff = { 0 };

	mmckRiff.fccType = mmioStringToFOURCC(TEXT("WAVE"), 0);
	MMRESULT mmr = mmioDescend(hmmio, &mmckRiff, NULL, MMIO_FINDRIFF);
	if (mmr != MMSYSERR_NOERROR)
	{
		OutputViewer("ERROR: sound check audio failed. [%s]", lpszFile);
		mmioClose(hmmio, 0);
		return FALSE;
	}

	MMCKINFO mmckFmt = { 0 };

	mmckFmt.ckid = mmioStringToFOURCC(TEXT("fmt "), 0);
	mmioDescend(hmmio, &mmckFmt, &mmckRiff, MMIO_FINDCHUNK);
	mmioRead(hmmio, (HPSTR)lpwf, mmckFmt.cksize);
	mmioAscend(hmmio, &mmckFmt, 0);
	if (lpwf->wFormatTag != WAVE_FORMAT_PCM)
	{
		OutputViewer("ERROR: sound check pcm failed. [%s]", lpszFile);
		mmioClose(hmmio, 0);
		return FALSE;
	}

	MMCKINFO mmckData = { 0 };

	mmckData.ckid = mmioStringToFOURCC(TEXT("data"), 0);
	mmioDescend(hmmio, &mmckData, &mmckRiff, MMIO_FINDCHUNK);
	*lplpData = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, mmckData.cksize);
	mmioRead(hmmio, (HPSTR)*lplpData, mmckData.cksize);
	mmioAscend(hmmio, &mmckData, 0);

	mmioAscend(hmmio, &mmckRiff, 0);
	mmioClose(hmmio, 0);
	
	*lpdwSize = mmckData.cksize;

	return TRUE;
}

HRESULT PlaySound(HWND hWnd, LPTSTR wavName)
{
	POPWND_DATA& popwndData = *(POPWND_DATA*)GetWindowLong(hWnd, GWLP_USERDATA);

	DWORD dwSize = 0;
	WAVEFORMATEX wf = { 0 };

	if (!LoadSoundFile(wavName, &wf, &popwndData.wd.lpData, &dwSize))
	{
		OutputViewer("ERROR: sound load failed. [%s]", wavName);
		return -1;
	}

	if (waveOutOpen(&popwndData.wd.hwo, WAVE_MAPPER, &wf, (DWORD)hWnd, 0, CALLBACK_WINDOW) != MMSYSERR_NOERROR)
	{
		OutputViewer("ERROR: sound waveOutOpen failed. [%s]", wavName);
		return -1;
	}

	popwndData.wd.wh.lpData = (LPSTR)popwndData.wd.lpData;
	popwndData.wd.wh.dwFlags = 0;
	popwndData.wd.wh.dwBufferLength = dwSize;

	waveOutPrepareHeader(popwndData.wd.hwo, &popwndData.wd.wh, sizeof WAVEHDR);

	waveOutWrite(popwndData.wd.hwo, &popwndData.wd.wh, sizeof WAVEHDR);

	return 0;
}

BOOL CloseSound(HWND hWnd)
{
	POPWND_DATA& popwndData = *(POPWND_DATA*)GetWindowLong(hWnd, GWLP_USERDATA);

	if (popwndData.wd.hwo != NULL)
	{
		waveOutReset(popwndData.wd.hwo);
		waveOutUnprepareHeader(popwndData.wd.hwo, &popwndData.wd.wh, sizeof WAVEHDR);
		waveOutClose(popwndData.wd.hwo);

		popwndData.wd.hwo = NULL;
	}

	if (popwndData.wd.lpData != NULL)
	{
		HeapFree(GetProcessHeap(), 0, popwndData.wd.lpData);

		popwndData.wd.lpData = NULL;
	}

	return TRUE;
}
