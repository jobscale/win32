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

#include "pProcess.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "pUtility.h"
#include "pLook.h"

// ネットワーク診断
DWORD GetNetworkPejoration(NETWORK_PEJORATION_DATA& resData, const BOOL isSet/* = FALSE*/)
{
	if (isSet)
	{
		if (!MyWriteProfileDWORD(P("Setting"), P("network_offline"), resData.offline))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		if (!MyWriteProfileDWORD(P("Setting"), P("network_pejoration"), resData.pejoration))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		if (!MyWriteProfileDWORD(P("Setting"), P("network_percent"), resData.percent))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
	}
	else
	{
		// 回線切断はデフォ使用する
		resData.offline = MyGetProfileDWORD(P("Setting"), P("network_offline"), 1);

		// ネットワーク負荷モニターはデフォ使用する
		resData.pejoration = MyGetProfileDWORD(P("Setting"), P("network_pejoration"), 1);

		// デフォ300%(通常の3倍)の場合は警告を出す
		resData.percent = MyGetProfileDWORD(P("Setting"), P("network_percent"), 300);
	}

	return 0;
}

// ネットワーク診断
DWORD SetNetworkPejoration(const NETWORK_PEJORATION_DATA& data)
{
	GetNetworkPejoration(*(NETWORK_PEJORATION_DATA*)&data, TRUE);

	return 0;
}

// ネットワーク診断
DWORD GetNoticePejoration(NOTICE_PEJORATION_DATA& resData, const BOOL isSet/* = FALSE*/)
{
	if (isSet)
	{
		if (!MyWriteProfileDWORD(P("Setting"), P("nc_pop"), resData.notice_pop))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		if (!MyWriteProfileDWORD(P("Setting"), P("nc_mail"), resData.notice_mail))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		if (!MyWriteProfileString(P("Setting"), P("nc_mailadd"), resData.notice_mailadd))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		if (!MyWriteProfileDWORD(P("Setting"), P("audio_sound"), resData.alert_type))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
	}
	else
	{
		// ポップアップ通知 :: デフォ使用しない
		resData.notice_pop = MyGetProfileDWORD(P("Setting"), P("nc_pop"), 0);

		// メール通知 :: デフォ使用しない
		resData.notice_mail = MyGetProfileDWORD(P("Setting"), P("nc_mail"), 0);

		// メール送信先アドレス
		CoString nc_mailadd;
		MyGetProfileString(P("Setting"), P("nc_mailadd"), nc_mailadd.GetBuffer(2048), 2048);
		nc_mailadd.ReleaseBuffer();
		resData.notice_mailadd = nc_mailadd;

		// アラート音源
		resData.alert_type = MyGetProfileDWORD(P("Setting"), P("audio_sound"), 0);
	}

	return 0;
}

DWORD SetNoticePejoration(const NOTICE_PEJORATION_DATA& data)
{
	GetNoticePejoration(*(NOTICE_PEJORATION_DATA*)&data, TRUE);

	return 0;
}
