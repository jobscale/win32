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

#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>

#include <atlcomtime.h>
#include <shlobj.h>

#include "oImage.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"
#include "../ofc/oFoundationClassLibrary.h"

#include "../pLook/pUtility.h"

HRESULT DIBtoJPG(CString& jpgData, const CString& bmpData)
{
	CoString localPath;
	PleaseTradeName(localPath, "\0 z_" + 2, "\0 .temp" + 2);

	HANDLE hFile = CreateFile(localPath, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

    DWORD dwWrite = 0;
	WriteFile(hFile, bmpData, bmpData.GetLength(), &dwWrite, NULL);
	CloseHandle(hFile);

	CString jpegFile = localPath + ("\0 .jpg" + 2);
	BOOL ToJPG(LPCSTR lpJpegFile, LPCSTR lpImageFile, int iQuality, BOOL bProgression);
	ToJPG(jpegFile, localPath, 80, TRUE);
	DeleteFile(localPath);

	hFile = CreateFile(jpegFile, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	DWORD dwReadHigh = 0;
    DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	ReadFile(hFile, jpgData.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
	CloseHandle(hFile);
	DeleteFile(jpegFile);

	return 0;
}

DWORD ScreenShot(CString* pData)
{
	CRect rect;
	GetClientRect(GetDesktopWindow(), rect);
	CoBitmapDC dc;
	dc.CreateBitmapDC(NULL, rect.TopLeft(), rect.Size());
	CoBitmapDC memDC;
	memDC.CreateBitmapDC(&dc, rect.TopLeft(), rect.Size());
	CBitmap& bmp = memDC.GetBitmap();
	BITMAP bm;
	bmp.GetBitmap(&bm);

	BITMAPINFO bInfo = { sizeof BITMAPINFOHEADER };
	BITMAPINFOHEADER& biHeader = bInfo.bmiHeader;
	biHeader.biWidth = bm.bmWidth;
	biHeader.biHeight = bm.bmHeight;
	biHeader.biPlanes = bm.bmPlanes;
	biHeader.biBitCount = 24;
	biHeader.biCompression = 0;
	biHeader.biSizeImage = 0;
	biHeader.biXPelsPerMeter = 0;
	biHeader.biYPelsPerMeter = 0;
	biHeader.biClrUsed = 0;
	biHeader.biClrImportant = 0;

	INT nWidthLength = biHeader.biWidth * 3;
	INT nPer = nWidthLength % 4;
	if (nPer)
	{
		nWidthLength += nPer;
	}
	biHeader.biSizeImage = nWidthLength * biHeader.biHeight;
	CString buffer;
	LPVOID pBits = buffer.GetBuffer(biHeader.biSizeImage);
	GetDIBits(memDC.GetSafeHdc(), (HBITMAP)bmp.GetSafeHandle(), 0, bm.bmHeight, pBits, &bInfo, DIB_RGB_COLORS);

	BITMAPFILEHEADER bFile = { 0 };
	bFile.bfType = *LPWORD("\0 BM" + 2);
	bFile.bfSize = sizeof BITMAPFILEHEADER;
	bFile.bfReserved1 = 0;
	bFile.bfReserved2 = 0;
	bFile.bfOffBits = sizeof BITMAPFILEHEADER + sizeof BITMAPINFO;

	CString added;
	CString binary;
	memcpy(added.GetBufferSetLength(sizeof BITMAPFILEHEADER), &bFile, sizeof BITMAPFILEHEADER);
	binary += added;
	memcpy(added.GetBufferSetLength(sizeof BITMAPINFO), &bInfo, sizeof BITMAPINFO);
	binary += added;
	memcpy(added.GetBufferSetLength(biHeader.biSizeImage), pBits, biHeader.biSizeImage);
	binary += added;

	DIBtoJPG(*pData, binary);

	return 0;
}
