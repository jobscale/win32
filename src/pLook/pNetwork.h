///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__P_NETWORK_H__)
#define __P_NETWORK_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// �l�b�g���[�N�����֐�

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

// �l�b�g���[�N�������ݒ�
struct NETWORK_PEJORATION_DATA
{
	DWORD percent;
	DWORD pejoration;
	DWORD offline;
};
DWORD GetNetworkPejoration(NETWORK_PEJORATION_DATA& resData, const BOOL isSet = FALSE);
DWORD SetNetworkPejoration(const NETWORK_PEJORATION_DATA& data);

// �ʒm�ݒ�
struct NOTICE_PEJORATION_DATA
{
	DWORD alert_type;
	DWORD notice_mail;
	DWORD notice_pop;
	CoString notice_mailadd;
};

// �l�b�g���[�N�f�f
DWORD GetNoticePejoration(NOTICE_PEJORATION_DATA& resData, const BOOL isSet = FALSE);
DWORD SetNoticePejoration(const NOTICE_PEJORATION_DATA& data);


#endif // __P_NETWORK_H__
