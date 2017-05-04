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
#include "../oCommon/oSocketTools.h"
#include "../oCommon/oHttpSocket.h"
#include "../oCommon/oEthernet.h"
#include "../oCommon/oBase64.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "pProcess.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "pUtility.h"
#include "pLook.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

INT EncryptionHttpPostFile(LPCTSTR source, LPCTSTR target, LPCTSTR type, CoString* pRecvData = NULL, DWORD reserved = 0)
{
	CoString hostname, ipaddr, ipmask, username, macaddr, adapterName;
	GetHostInfo(hostname, ipaddr, ipmask, username, macaddr, adapterName);

	HANDLE hFile = CreateFile(source, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CoString textData, textTemp;
	DWORD store_code = 0;
	textData.Format("\0 ip=%s&store_code=%d&type=%s&data=" + 2, (LPCTSTR)ipaddr, store_code, type);

	DWORD sizeRead;
	DWORD dwReadHigh = 0;
	DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	for (; ReadFile(hFile, textTemp.GetBuffer(dwRead), dwRead, &sizeRead, NULL) && sizeRead != 0; )
	{
		textTemp.GetBufferSetLength(sizeRead);
		textData += textTemp;
	}

	CloseHandle(hFile);
	NativityFile(source, 2);

	OHTTP_DATA httpData;
	httpData.target = target;
	httpData.sendData = textData;
	httpData.reserved = reserved;
	INT result = HttpPost(&httpData);
	if (result == 0)
	{
		DeleteFile(source);
	}

	if (pRecvData)
	{
		_asm
		{
			int 3
		}
	}

	return result;
}

INT EncryptionSendScketFile(LPCTSTR source, LPCTSTR type)
{
	CoString hostname, ipaddr, ipmask, username, macaddr, adapterName;
	GetHostInfo(hostname, ipaddr, ipmask, username, macaddr, adapterName);

	HANDLE hFile = NULL;
	if (NativityFile(source, 5, TRUE, &hFile) != 0 || !hFile)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	int sendCount = 0;
	CoString textData, textTemp, lineData, fatalData;
	DWORD dataSize = 0;

	DWORD dwReadHigh = 0;
	DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	for (; ReadFile(hFile, textTemp.GetBuffer(dwRead), dwRead, &dataSize, NULL) && dataSize != 0; )
	{
		textTemp.GetBufferSetLength(dataSize);
		textData += textTemp;
		for (int find = -1; (find = textData.Find(RETERCODE)) != -1; )
		{
			lineData = textData.Left(find);
			textData = textData.Right(textData.GetLength() - find - strlen(RETERCODE));

			if (lineData.IsEmpty())
			{
				continue;
			}

			CoString base64 = lineData;
			base64_decode(base64, lineData);

			// syslog exsample
			// <priority> Dec 6 12:34:56 kei newsyslog[69183]: logfile turned over

			SYSTEMTIME systemTime = { 0 };
			DateFromString(&systemTime, lineData, 19);

			COleDateTime oleTime = systemTime;

			CoString syslogHeader;
			syslogHeader.Format("<%d> %s pLook", 0, (LPCTSTR)oleTime.Format("%b %d %H:%M:%S"));
			CoString sendData;
			DWORD store_code = 0;
			sendData.Format("\0 %s:%s%s%s%s%s%s%d%s%s" + 2
				, (LPCTSTR)syslogHeader, type
				, DELIMITER, (LPCTSTR)ipaddr
				, DELIMITER, (LPCTSTR)macaddr
				, DELIMITER, store_code
				, DELIMITER, lineData);

			if (SendLineData(sendData) == 0)
			{
				if (sendCount == 0)
				{
					break;
				}
				fatalData += base64 + RETERCODE;
			}
			sendCount++;
		}
		if (sendCount == 0)
		{
			break;
		}
	}

	if (!fatalData.IsEmpty())
	{
		LARGE_INTEGER num = {0};
		SetFilePointerEx(hFile, num, NULL, FILE_BEGIN);
		if (!WriteFile(hFile, (LPCTSTR)fatalData, fatalData.GetLength(), &dataSize, NULL))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		SetEndOfFile(hFile);
	}

	CloseHandle(hFile);

	if (sendCount != 0)
	{
		if (fatalData.IsEmpty())
		{
			NativityFile(source, 2);

			DeleteFile(source);
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

// 符号
INT EncryptionFile(LPCTSTR source)
{
	CRITICALTRACE(decryption, EncryptionFile);

	CoString drive, dir, fname, ext;
	_splitpath(source, drive.GetBuffer(_MAX_DRIVE), dir.GetBuffer(_MAX_DIR), fname.GetBuffer(_MAX_FNAME), ext.GetBuffer(_MAX_EXT));
	drive.ReleaseBuffer(); dir.ReleaseBuffer(); fname.ReleaseBuffer(); ext.ReleaseBuffer();

	CoString encSource = drive + dir + fname + ".enc";
	if (!MoveFile(source, encSource))
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	CoString folder = drive + dir;
	WIN32_FIND_DATA wfd = { 0 };
	HANDLE hFind = FindFirstFile(folder + "*.enc", &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	CoString target = ""; // http://exsample.com/get.php
	BOOL isHostNode = target.Left(7).Compare(P("http://")) != 0;

	char type[2] = { 0 };
	for (; ; )
	{
		type[0] = wfd.cFileName[0];
		if (isHostNode)
		{
			if (EncryptionSendScketFile(folder + wfd.cFileName, type) != 0)
			{
				break;
			}
		}
		else
		{
			if (EncryptionHttpPostFile(folder + wfd.cFileName, target, type) != 0)
			{
				break;
			}
		}

		if (!FindNextFile(hFind, &wfd))
		{
			break;
		}
	}

	FindClose(hFind);

	return 0;
}
