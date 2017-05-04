///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__OHTTPSOCKET_H__)
#define __OHTTPSOCKET_H__

#include <atlbase.h>
#include <afxmt.h>

/////////////////////////////////////////////////////////////////////////////
// 共通関数

struct OHTTP_DATA
{
	CString headerData;
	DWORD contentsLength;
	CString recvData;

	CString target;
	CString proxy;
	WORD proxyPort;
	CString referer;
	CString sendData;
	DWORD reserved;
	OHTTP_DATA()
		: proxyPort(0)
		, contentsLength(0)
		, reserved(0)
	{
	}
};

INT HttpPost(OHTTP_DATA* pData);
INT HttpGet(OHTTP_DATA* pData);

#endif // !defined(__OHTTPSOCKET_H__)
