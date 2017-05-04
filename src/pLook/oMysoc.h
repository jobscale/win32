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

#ifndef __OMYSOC_H__
#define __OMYSOC_H__

#define INCL_WINSOCK_API_TYPEDEFS TRUE
#include <Winsock2.h>

namespace omysoc
{
	extern LPFN_WSACLEANUP WSACleanup;
	extern LPFN_WSASTARTUP WSAStartup;
	extern LPFN_WSAGETLASTERROR WSAGetLastError;
	extern LPFN_RECV recv;
	extern LPFN_SEND send;
	extern LPFN_CONNECT connect;
	extern LPFN_HTONS htons;
	extern LPFN_GETHOSTBYNAME gethostbyname;
	extern LPFN_GETHOSTNAME gethostname;
	extern LPFN_SOCKET socket;
	extern LPFN_CLOSESOCKET closesocket;
	extern LPFN_ACCEPT accept;
	extern LPFN_LISTEN listen;
	extern LPFN_BIND bind;
}

#ifndef __OMYSOC_CPP__
#define WSACleanup omysoc::WSACleanup
#define WSAStartup omysoc::WSAStartup
#define WSAGetLastError omysoc::WSAGetLastError
#define recv omysoc::recv
#define send omysoc::send
#define connect omysoc::connect
#define htons omysoc::htons
#define gethostbyname omysoc::gethostbyname
#define gethostname omysoc::gethostname
#define socket omysoc::socket
#define closesocket omysoc::closesocket
#define accept omysoc::accept
#define listen omysoc::listen
#define bind omysoc::bind
#endif // __OMYSOC_CPP__

#endif // __OMYSOC_H__
