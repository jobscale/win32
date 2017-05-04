///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__OHTTPSOCKET_H__)
#define __OHTTPSOCKET_H__

#include <atlstr.h>

/////////////////////////////////////////////////////////////////////////////
// 共通関数

struct OHTTP_DATA
{
	CMemStr headerData;
	DWORD contentsLength;
	CMemStr recvData;

	CMemStr target;
	CMemStr proxy;
	WORD proxyPort;
	CMemStr sendData;
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
