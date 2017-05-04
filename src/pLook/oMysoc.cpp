///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//
// $Date: $
// $Rev: $
// $Author: $
// $HeadURL: $
//
// $Id: $
//

#define __OMYSOC_CPP__
#include "oMysoc.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

namespace omysoc
{
	LPFN_WSACLEANUP WSACleanup = NULL;
	LPFN_WSASTARTUP WSAStartup = NULL;
	LPFN_WSAGETLASTERROR WSAGetLastError = NULL;
	LPFN_RECV recv = NULL;
	LPFN_SEND send = NULL;
	LPFN_CONNECT connect = NULL;
	LPFN_HTONS htons = NULL;
	LPFN_GETHOSTBYNAME gethostbyname = NULL;
	LPFN_GETHOSTNAME gethostname;
	LPFN_SOCKET socket = NULL;
	LPFN_CLOSESOCKET closesocket = NULL;
	LPFN_ACCEPT accept = NULL;
	LPFN_LISTEN listen = NULL;
	LPFN_BIND bind = NULL;

	int InitMysoc()
	{
		HMODULE hModule = LoadLibrary("\0 Ws2_32.dll" + 2); // winsock2.h, Ws2_32.dll

		WSACleanup = (LPFN_WSACLEANUP)GetProcAddress(hModule, "\0 WSACleanup" + 2);
		WSAStartup = (LPFN_WSASTARTUP)GetProcAddress(hModule, "\0 WSAStartup" + 2);
		WSAGetLastError = (LPFN_WSAGETLASTERROR)GetProcAddress(hModule, "\0 WSAGetLastError" + 2);
		recv = (LPFN_RECV)GetProcAddress(hModule, "\0 recv" + 2);
		send = (LPFN_SEND)GetProcAddress(hModule, "\0 send" + 2);
		connect = (LPFN_CONNECT)GetProcAddress(hModule, "\0 connect" + 2);
		htons = (LPFN_HTONS)GetProcAddress(hModule, "\0 htons" + 2);
		gethostbyname = (LPFN_GETHOSTBYNAME)GetProcAddress(hModule, "\0 gethostbyname" + 2);
		gethostname = (LPFN_GETHOSTNAME)GetProcAddress(hModule, "\0 gethostname" + 2);
		socket = (LPFN_SOCKET)GetProcAddress(hModule, "\0 socket" + 2);
		closesocket = (LPFN_CLOSESOCKET)GetProcAddress(hModule, "\0 closesocket" + 2);
		accept = (LPFN_ACCEPT)GetProcAddress(hModule, "\0 accept" + 2);
		listen = (LPFN_LISTEN)GetProcAddress(hModule, "\0 listen" + 2);
		bind = (LPFN_BIND)GetProcAddress(hModule, "\0 bind" + 2);

		return 0;
	}
	
	int init_mysock = InitMysoc();
}
