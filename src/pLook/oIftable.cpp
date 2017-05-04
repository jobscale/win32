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
#include "../oCommon/oEthernet.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "pProcess.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "pUtility.h"

#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi")
#include <iprtrmib.h>

DWORD GetArchiTable(PMIB_IFTABLE IfTable, const CoString& physAddr)
{
    for (DWORD i = 0; i < IfTable->dwNumEntries; i++)
	{
		MIB_IFROW& table = IfTable->table[i];

		CoString curPhysAddr;
		CoString work;
		for (DWORD phi = 0, count = table.dwPhysAddrLen; phi < count; phi++)
		{
			if (phi != 0)
			{
				curPhysAddr += ":";
			}
			work.Format("%02X", table.bPhysAddr[phi]);
			curPhysAddr += work;
		}
		if (curPhysAddr.Compare(physAddr) == 0)
		{
			return i;
		}
	}

	return -1;
}

struct OCTETS_DATA
{
	SYSTEMTIME systime;
	DWORD dwInOctets;
	DWORD dwOutOctets;
};

struct IFINFORMATION_DATA
{
	PMIB_IFTABLE IfTable;
	DWORD size;
	CoArray<OCTETS_DATA> octets;

	IFINFORMATION_DATA()
		: IfTable(0)
		, size(0)
	{
	}
};

DWORD GetIfInformationCore(IFINFORMATION_DATA& data, const CoString& physAddr)
{
	if (GetIfTable((PMIB_IFTABLE)data.IfTable, &data.size, FALSE) != NO_ERROR)
	{
		OutputViewer("ERROR: GetIfTable failed.");
		return -1;
	}

	DWORD archiTable = GetArchiTable(data.IfTable, physAddr);
	if (archiTable == -1)
	{
		return -1;
	}
	MIB_IFROW& table = data.IfTable->table[archiTable];

#if 0
	DWORD inNetLen = table.dwInUcastPkts + table.dwInNUcastPkts;
	DWORD outNetLen = table.dwOutUcastPkts + table.dwOutNUcastPkts;
#else
	DWORD inNetLen = table.dwInOctets;
	DWORD outNetLen = table.dwOutOctets;
#endif

	static DWORD beforeInOctets = 0;
	static DWORD beforeOutOctets = 0;
	if (beforeInOctets == 0 && beforeOutOctets == 0)
	{
		beforeInOctets = inNetLen;
		beforeOutOctets = outNetLen;
		return 0;
	}

	OCTETS_DATA odata;

	GetSystemTime(&odata.systime);
	SystemTimeToTzSpecificLocalTime(NULL, &odata.systime, &odata.systime);

	// オーバーフロー処理
	if (inNetLen < beforeInOctets)
	{
		odata.dwInOctets = ULONG_MAX - beforeInOctets + inNetLen;
	}
	else
	{
		odata.dwInOctets = inNetLen - beforeInOctets;
	}
	if (outNetLen < beforeOutOctets)
	{
		odata.dwOutOctets = ULONG_MAX - beforeOutOctets + outNetLen;
	}
	else
	{
		odata.dwOutOctets = outNetLen - beforeOutOctets;
	}
	beforeInOctets = inNetLen;
	beforeOutOctets = outNetLen;
	data.octets.Add(odata);
	if (data.octets.GetSize() > 24)
	{
		data.octets.RemoveAt(0);
	}

	return 0;
}

DWORD GetIfInformation(DWORD& dwInAverage, DWORD& dwOutAverage, const CoString& physAddr)
{
    DWORD size = 0;
    GetIfTable(NULL, &size, FALSE);

	PMIB_IFTABLE IfTable = (PMIB_IFTABLE)new char[size];
	if (!IfTable)
	{
		OutputViewer("ERROR: get memory failed.");
		return -1;
	}

	static IFINFORMATION_DATA data;
	data.IfTable = IfTable;
	data.size = size;
	GetIfInformationCore(data, physAddr);

	delete[] IfTable;

	UINT64 dwInOctets = 0;
	UINT64 dwOutOctets = 0;
	DWORD count = data.octets.GetSize();
	if (count < 1)
	{
		return -1;
	}
	for (DWORD i = 0; i < count; i++)
	{
		dwInOctets += data.octets.GetAt(i).dwInOctets;
		dwOutOctets += data.octets.GetAt(i).dwOutOctets;
	}

	// count seconds average
	dwInAverage = (DWORD)(dwInOctets / count);
	dwOutAverage = (DWORD)(dwOutOctets / count);

	return 0;
}

DWORD IfInAverage(DWORD dwInAverage/* = -1*/)
{
	static CCriticalSection section;
	CoCriticalSectionCtrl critical(section);

	static DWORD _dwInAverage = 0;
	if (dwInAverage != -1)
	{
		_dwInAverage = dwInAverage;
	}
	return _dwInAverage;
}

DWORD IfOutAverage(DWORD dwOutAverage/* = -1*/)
{
	static CCriticalSection section;
	CoCriticalSectionCtrl critical(section);

	static DWORD _dwOutAverage = 0;
	if (dwOutAverage != -1)
	{
		_dwOutAverage = dwOutAverage;
	}
	return _dwOutAverage;
}

DWORD WINAPI GetIfInformationThread(LPVOID pParam)
{
	CWSAInterface wsa;

	DWORD dwInAverage = 0;
	DWORD dwOutAverage = 0;
	for (; ; )
	{
		CoString hostname, ipaddr, ipmask, username, macaddr, adapterName;
		GetHostInfo(hostname, ipaddr, ipmask, username, macaddr, adapterName);

		GetIfInformation(dwInAverage, dwOutAverage, macaddr);
		IfInAverage(dwInAverage);
		IfOutAverage(dwOutAverage);
		Sleep(5000);
	}

	return 0;
}

const STATUS_DATA* GetTcpStatusText(const DWORD status)
{
	static const STATUS_DATA statusData[] =
	{
		0, MIB_TCP_STATE_CLOSED, "Closed",
		1, MIB_TCP_STATE_LISTEN, "Listening",
		1, MIB_TCP_STATE_SYN_SENT, "SYN Sent",
		1, MIB_TCP_STATE_SYN_RCVD, "SYN Received",
		1, MIB_TCP_STATE_ESTAB, "Established",
		0, MIB_TCP_STATE_FIN_WAIT1, "Waiting for FIN",
		0, MIB_TCP_STATE_FIN_WAIT2, "Waiting for FIN",
		0, MIB_TCP_STATE_CLOSE_WAIT, "Waiting for Close",
		0, MIB_TCP_STATE_CLOSING, "Closing",
		0, MIB_TCP_STATE_LAST_ACK, "Last ACK",
		0, MIB_TCP_STATE_TIME_WAIT, "Time Wait",
		0, MIB_TCP_STATE_DELETE_TCB, "TCB deleted",
		0, -1, "Unknown status",
	};

	for (DWORD i = 0; statusData[i].status != -1; i++)
	{
		if (status == statusData[i].status)
		{
			return &statusData[i];
		}
	}

	return &statusData[(sizeof statusData / sizeof STATUS_DATA) - 1];
}

DWORD TCPConnections(CoConnectionsArray& connections, DWORD flag = 0)
{
	connections.RemoveAll();

	CoString buffer;
	DWORD dwTableSize = 100 * 20 + 4;
	PMIB_TCPTABLE pTcpTable = (PMIB_TCPTABLE)buffer.GetBuffer(dwTableSize);
	DWORD dwRet = GetTcpTable(pTcpTable, &dwTableSize, TRUE);
	if (dwRet != NO_ERROR)
	{
		return -1;
	}

	in_addr	addrLoc = { 0 };
	in_addr	addrRem = { 0 };

	for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++)
	{
		CONNECTIONS_DATA data;

		DWORD dwRemIP = htonl(pTcpTable->table[i].dwRemoteAddr);
		WORD nRemPort = htons((WORD)pTcpTable->table[i].dwRemotePort);

		DWORD dwLocIP = htonl(pTcpTable->table[i].dwLocalAddr);
		WORD nLocPort = htons((WORD)pTcpTable->table[i].dwLocalPort);

		addrLoc.S_un.S_addr = ntohl(dwLocIP);
		addrRem.S_un.S_addr = ntohl(dwRemIP);

		data.localAddr = inet_ntoa(addrLoc);
		data.remoteAddr = inet_ntoa(addrRem);

		data.localPort.Format("%u", nLocPort);
		data.remotePort.Format("%u", nRemPort);

		data.status = *GetTcpStatusText(pTcpTable->table[i].dwState);

		if (data.status.flag == 0)
		{
			continue;
		}

		if (data.localAddr == "0.0.0.0")
		{
			continue;
		}

		if (data.remoteAddr == "0.0.0.0")
		{
			continue;
		}

		if (data.localAddr == "127.0.0.1")
		{
			continue;
		}

		if (data.remoteAddr == "127.0.0.1")
		{
			continue;
		}

		if (data.localPort.GetLength() == 5 && data.localPort.Left(3) == "120")
		{
			continue;
		}

		if (data.remotePort.GetLength() == 5 && data.remotePort.Left(3) == "120")
		{
			continue;
		}

		connections.Add(data);
	}

	return 0;
}
