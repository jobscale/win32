///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__P_NETWORK_H__)
#define __P_NETWORK_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// ネットワーク処理関数

struct STATUS_DATA
{
	DWORD flag;
	DWORD status;
	LPCTSTR text;
};

struct CONNECTIONS_DATA
{
	CoString localAddr;
	CoString localPort;
	CoString remoteAddr;
	CoString remotePort;
	STATUS_DATA status;
};

class CoConnectionsArray : public CoArray<CONNECTIONS_DATA>
{
};

DWORD IfInAverage(DWORD dwInAverage = -1);
DWORD IfOutAverage(DWORD dwOutAverage = -1);

// ネットワーク悪化率設定
struct NETWORK_PEJORATION_DATA
{
	DWORD percent;
	DWORD pejoration;
	DWORD offline;
};
DWORD GetNetworkPejoration(NETWORK_PEJORATION_DATA& resData, const BOOL isSet = FALSE);
DWORD SetNetworkPejoration(const NETWORK_PEJORATION_DATA& data);

// 通知設定
struct NOTICE_PEJORATION_DATA
{
	DWORD alert_type;
	DWORD notice_mail;
	DWORD notice_pop;
	CoString notice_mailadd;
};

// ネットワーク診断
DWORD GetNoticePejoration(NOTICE_PEJORATION_DATA& resData, const BOOL isSet = FALSE);
DWORD SetNoticePejoration(const NOTICE_PEJORATION_DATA& data);


#endif // __P_NETWORK_H__
