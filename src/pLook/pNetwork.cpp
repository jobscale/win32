///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
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

// �l�b�g���[�N�f�f
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
		// ����ؒf�̓f�t�H�g�p����
		resData.offline = MyGetProfileDWORD(P("Setting"), P("network_offline"), 1);

		// �l�b�g���[�N���׃��j�^�[�̓f�t�H�g�p����
		resData.pejoration = MyGetProfileDWORD(P("Setting"), P("network_pejoration"), 1);

		// �f�t�H300%(�ʏ��3�{)�̏ꍇ�͌x�����o��
		resData.percent = MyGetProfileDWORD(P("Setting"), P("network_percent"), 300);
	}

	return 0;
}

// �l�b�g���[�N�f�f
DWORD SetNetworkPejoration(const NETWORK_PEJORATION_DATA& data)
{
	GetNetworkPejoration(*(NETWORK_PEJORATION_DATA*)&data, TRUE);

	return 0;
}

// �l�b�g���[�N�f�f
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
		// �|�b�v�A�b�v�ʒm :: �f�t�H�g�p���Ȃ�
		resData.notice_pop = MyGetProfileDWORD(P("Setting"), P("nc_pop"), 0);

		// ���[���ʒm :: �f�t�H�g�p���Ȃ�
		resData.notice_mail = MyGetProfileDWORD(P("Setting"), P("nc_mail"), 0);

		// ���[�����M��A�h���X
		CoString nc_mailadd;
		MyGetProfileString(P("Setting"), P("nc_mailadd"), nc_mailadd.GetBuffer(2048), 2048);
		nc_mailadd.ReleaseBuffer();
		resData.notice_mailadd = nc_mailadd;

		// �A���[�g����
		resData.alert_type = MyGetProfileDWORD(P("Setting"), P("audio_sound"), 0);
	}

	return 0;
}

DWORD SetNoticePejoration(const NOTICE_PEJORATION_DATA& data)
{
	GetNoticePejoration(*(NOTICE_PEJORATION_DATA*)&data, TRUE);

	return 0;
}
