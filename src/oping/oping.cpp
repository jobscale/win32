// oping.cpp : アプリケーションのエントリ ポイントを定義します。
//

// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

// 下で指定された定義の前に対象プラットフォームを指定しなければならない場合、以下の定義を変更してください。
// 異なるプラットフォームに対応する値に関する最新情報については、MSDN を参照してください。
#ifndef WINVER				// Windows XP 以降のバージョンに固有の機能の使用を許可します。
#define WINVER 0x0501		// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_WINNT		// Windows XP 以降のバージョンに固有の機能の使用を許可します。                   
#define _WIN32_WINNT 0x0501	// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_WINDOWS 0x0410 // これを Windows Me またはそれ以降のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_IE			// IE 6.0 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_IE 0x0600	// これを IE. の他のバージョン向けに適切な値に変更してください。
#endif

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <winsock2.h>
#pragma comment(lib, "wsock32")

#include <stdio.h>

// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi")

int arp()
{
	DWORD dwSize = 0;
	DWORD dwResult = GetIpNetTable(NULL, &dwSize, 0);
	if (dwResult != ERROR_INSUFFICIENT_BUFFER || dwSize == 0)
	{
		return -1;
	}

	PMIB_IPNETTABLE pIpNetTable = (PMIB_IPNETTABLE)malloc(dwSize);
	DWORD dwRetVal = GetIpNetTable(pIpNetTable, &dwSize, 0);
	if (dwRetVal != NO_ERROR)
	{
		printf("GetIpNetTable failed.\n");
		LPVOID lpMsgBuf;
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
						 FORMAT_MESSAGE_FROM_SYSTEM | 
						 FORMAT_MESSAGE_IGNORE_INSERTS,
						 NULL,
						 dwRetVal,
						 MAKELANGID(LANG_NEUTRAL,
									SUBLANG_DEFAULT), //Default language
						 (LPTSTR) &lpMsgBuf,
						 0,
						 NULL ))
		{
			printf("\tError: %s", lpMsgBuf);
		}

		LocalFree(lpMsgBuf);

		return -1;
	}

	if (pIpNetTable->dwNumEntries <= 0)
	{
		printf("Noting\n");
		return -1;
	}

	for (DWORD i = 0; i < pIpNetTable->dwNumEntries; i++)
	{
		 if (pIpNetTable->table[i].dwType == 3)
		 {
			// printf("Phys Addr Len: %d\n", pIpNetTable->table[i].dwPhysAddrLen);
			printf("Phys: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x IP: %s\n",
				pIpNetTable->table[i].bPhysAddr[0],
				pIpNetTable->table[i].bPhysAddr[1],
				pIpNetTable->table[i].bPhysAddr[2],
				pIpNetTable->table[i].bPhysAddr[3],
				pIpNetTable->table[i].bPhysAddr[4],
				pIpNetTable->table[i].bPhysAddr[5],
				inet_ntoa(*(in_addr*)&pIpNetTable->table[i].dwAddr));

			// printf("Index:   %ld\n", pIpNetTable->table[i].dwIndex);
			// printf("Type:   %ld\n", pIpNetTable->table[i].dwType);
			// printf("\n");
		}
	}

	return 0;
}


struct ICMP_HDR
{
	unsigned char icmp_type;
	unsigned char icmp_code;
	unsigned short icmp_checksum;
	unsigned short icmp_id;
	unsigned short icmp_sequence;
	unsigned long icmp_timestamp;
	unsigned long icmp_timestampsum;
	unsigned long icmp_timestampsumsum;
};


#define ICMP_DATASIZE  (16) // min 8 - 999000 max


USHORT CheckSum(USHORT *buffer, int size)
{
    unsigned long cksum = 0;
    for (; size > 1; size -= sizeof USHORT)
    {
        cksum += *buffer;
		buffer++;
    }
    if (size) 
    {
#ifdef _DEBUG
		_asm
		{
			int 3
		}
#endif
        cksum += *(UCHAR*)buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);

    return (USHORT)(~cksum);
}

BOOL BuildICMPEchoPacket(char* pBuff)
{	
	static char chData = 'z';
	memset(pBuff + sizeof ICMP_HDR, chData++, ICMP_DATASIZE);

	ICMP_HDR* icmp_dhr = (ICMP_HDR*)pBuff;
	icmp_dhr->icmp_type = 8; // echo request.
	icmp_dhr->icmp_code = 0;
	icmp_dhr->icmp_id = (unsigned short)GetCurrentProcessId();
	icmp_dhr->icmp_sequence = 0;
	icmp_dhr->icmp_timestamp = GetTickCount();
	icmp_dhr->icmp_checksum = CheckSum((USHORT*)icmp_dhr, sizeof ICMP_HDR + ICMP_DATASIZE);
	icmp_dhr->icmp_timestampsum = icmp_dhr->icmp_timestamp;
	icmp_dhr->icmp_timestampsumsum = icmp_dhr->icmp_timestamp;

	return TRUE;
}


struct IP_HEADER
{
	unsigned char ip_vhl; // version and header length

#define ip_v ip_vhl>>4
#define ip_hl ip_vhl&0x0F

	unsigned char ip_tos; // type of service
	unsigned short ip_len; // total length
	unsigned short ip_id; // identification
	unsigned short ip_off; //fragment offset field

#define IP_DF 0x4000 //dont fragment flag
#define IP_MF 0x2000 //more fragment flag
#define IP_OFFMASK 0x1fff //mask for fragmenting bits

	unsigned char ip_ttl; // time to live
	unsigned char ip_p; //protocol
	unsigned short ip_sum; // checksum
	in_addr ip_src; // source address
	in_addr ip_dst; // dest address
};


class CAutoSock
{
protected:
	SOCKET _sock;
public:
	CAutoSock(SOCKET sock)
		: _sock(sock)
	{
	}
	virtual ~CAutoSock()
	{
		closesocket(_sock);
	}
};


int oping(SOCKET sock, char ipaddr[], int i)
{
	SOCKADDR_IN TargetAddr = { 0 };
	TargetAddr.sin_addr.S_un.S_addr = inet_addr(ipaddr);
	TargetAddr.sin_port = htons(80);
	TargetAddr.sin_family = AF_INET;

	char szBuff[sizeof ICMP_HDR + ICMP_DATASIZE] = { 0 };
	BuildICMPEchoPacket(szBuff);

	int nRet = sendto(sock, szBuff, sizeof ICMP_HDR + ICMP_DATASIZE, 0,
			(SOCKADDR*)&TargetAddr, sizeof TargetAddr);
	if (nRet == SOCKET_ERROR)
	{
		fprintf(stderr, "%s sendto Failed. (%d)\n", ipaddr, WSAGetLastError());
		return -1;
	}

	// Preparing for after use.
	fd_set readfds = { 0 };
	readfds.fd_count = 1;
	readfds.fd_array[0] = sock;
	struct timeval Timeout = { 0 };
	Timeout.tv_sec = 0; // timeout 5 sec.
	Timeout.tv_usec = 10;

	nRet = select(1, &readfds, NULL, NULL, &Timeout);
	if (nRet == 0) // Timed out.
	{
		fprintf(stderr, "Request timed out.\n");
		return 4;
	}

	SOCKADDR_IN AddrFrom = { 0 };
	int nRecv = sizeof AddrFrom;
	char szBuffRecv[sizeof ICMP_HDR + ICMP_DATASIZE + 1024] = { 0 };
	nRet = recvfrom(sock, szBuffRecv, sizeof szBuffRecv, 0, (SOCKADDR*)&AddrFrom, &nRecv);
	if (nRet == 0 || nRet == SOCKET_ERROR)
	{
		fprintf(stderr, "recvfrom Failed. (ret, gle) = (%d, %d)\n", nRet, WSAGetLastError());
		return -1;
	}

	IP_HEADER* ip_hdr = (IP_HEADER*)szBuffRecv;
	ICMP_HDR* icmp_dhr = (ICMP_HDR*)(szBuffRecv + (((int)ip_hdr->ip_hl) * 4));

	unsigned short cs = CheckSum((USHORT*)icmp_dhr, sizeof ICMP_HDR + ICMP_DATASIZE);
	if (cs != 0)
	{
		fprintf(stderr, "Data received corrupted.\n");
		return 2;
	}

	if (icmp_dhr->icmp_timestamp != icmp_dhr->icmp_timestampsum || icmp_dhr->icmp_timestampsum != icmp_dhr->icmp_timestampsumsum)
	{
		fprintf(stderr, "Data received corrupted.\n");
		return 2;
	}

	DWORD dwTimeTaken = GetTickCount() - (DWORD)icmp_dhr->icmp_timestamp;

	fprintf(stdout, "Reply from %s: bytes=%d time=%dms TTL=%d\n",
		inet_ntoa(ip_hdr->ip_src),
		nRet - ((int) ip_hdr->ip_hl) * 4 - sizeof ICMP_HDR,
		dwTimeTaken,
		ip_hdr->ip_ttl);

	return 0;
}

int oping_main()
{
	SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock == INVALID_SOCKET)
	{
		printf("socket Failed. (%d)\n", WSAGetLastError());
		return -1;
	}

	CAutoSock autosock(sock);

	char output[1024] = { 0 };
	char ipbuf[255] = { 0 };
	for (int i = 1; i < 255; i++)
	{
		sprintf_s(ipbuf, sizeof ipbuf, "172.16.6.%d", i);
		oping(sock, ipbuf, i);
	}

	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WSADATA wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	oping_main();

	arp();

	WSACleanup();

	return 0;
}
