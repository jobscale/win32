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

#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

#include <WS2tcpip.h>

#include "oTools.h"
#include "oSocketTools.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// オブジェクト

CWSAInterface::CWSAInterface()
{
	ZeroMemory(&wsaData, sizeof wsaData);

	if (WSAStartup(WINSOCK_VERSION, &wsaData) != NO_ERROR)
	{
		DEBUG_OUT_DEFAULT("failed.");
	}
}

CWSAInterface::~CWSAInterface()
{
	WSACleanup();
}

CoAutoSocket::CoAutoSocket(SOCKET socket)
	: m_socket(socket)
{
}

CoAutoSocket::~CoAutoSocket()
{
	if (m_socket)
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
	}
}

SOCKET CoAutoSocket::Dispatch()
{
	SOCKET sock = m_socket;
	m_socket = 0;
	return sock;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 関数

LRESULT TransRecvUDP(SOCKET sock, CString& recvData)
{
	INT bytesRecv = recvfrom(sock, recvData.GetBuffer(2 MB), 2 MB, 0, NULL, NULL);
	if (bytesRecv == SOCKET_ERROR || bytesRecv == WSAECONNRESET)
	{
		DEBUG_OUT_DEFAULT("trans udp recvfrom failed.");
		return -1;
	}
	recvData.GetBufferSetLength(bytesRecv);

	return 0;
}

LRESULT TransRecvTCP(SOCKET sock, CString& recvData)
{
	recvData.Empty();
	CString recvBuf;
	for (; ; )
	{
		INT bytesRecv = recv(sock, recvBuf.GetBuffer(RECV_SIZE_BIG), RECV_SIZE_BIG, 0/*, timeout*/);
		Sleep(1);
		if (bytesRecv == SOCKET_ERROR || bytesRecv == WSAECONNRESET)
		{
			if (!recvData.IsEmpty())
			{
				return 0;
			}
			DEBUG_OUT_DEFAULT("trans tcp recv failed.");
			return -1;
		}

		recvBuf.GetBufferSetLength(bytesRecv);
		recvData += recvBuf;

		INT pos = recvData.Find("User-Agent: exTrapper");
		if (pos != -1)
		{
			pos = recvData.Find("Content-Length: ");
			if (pos != -1)
			{
				pos += (INT)strlen("Content-Length: ");
				INT ContentLength = strtol(&((LPCTSTR)recvData)[pos], NULL, 10);
				pos = recvData.Find("\r\n\r\n");
				if (pos != -1)
				{
					recvData = recvData.Right(recvData.GetLength() - pos - 2);

					for (; ; )
					{
						INT bytesRecv = recv(sock, recvBuf.GetBuffer(RECV_SIZE_BIG), RECV_SIZE_BIG, 0/*, timeout*/);
						if (bytesRecv == SOCKET_ERROR || bytesRecv == WSAECONNRESET)
						{
							if (!recvData.IsEmpty())
							{
								return 0;
							}
							DEBUG_OUT_DEFAULT("trans tcp recv failed.");
							return -1;
						}

						recvBuf.GetBufferSetLength(bytesRecv);
						recvData += recvBuf;

						if (recvData.GetLength() >= ContentLength)
						{
							return 0;
						}
					}
				}
			}
		}

		if (bytesRecv < RECV_SIZE_BIG)
		{
			break;
		}
	}

	return 0;
}

LRESULT TransRecvBrowser(SOCKET sock, CString& recvData)
{
	recvData.Empty();
	CString recvBuf;
	for (; ; )
	{
		INT bytesRecv = recv(sock, recvBuf.GetBuffer(RECV_SIZE_BIG), RECV_SIZE_BIG, 0/*, timeout*/);
		Sleep(1);
		if (bytesRecv == SOCKET_ERROR || bytesRecv == WSAECONNRESET)
		{
			if (!recvData.IsEmpty())
			{
				return 0;
			}
			// DEBUG_OUT_DEFAULT("trans tcp recv failed.");
			return /*-1*/0; // browserから0バイトを受信することがある
		}

		recvBuf.GetBufferSetLength(bytesRecv);
		recvData += recvBuf;

		if (bytesRecv < RECV_SIZE_BIG)
		{
			break;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

LRESULT DataSendConsole(LPCTSTR deststr, WORD portno, LPCTSTR text, CString* pReceive/* = NULL*/)
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		DEBUG_OUT_DEFAULT("oReceiveBCast socket faile.");
		return -1;
	}

	CoAutoSocket autoSocket(sock);

	struct sockaddr_in server = { 0 };
	server.sin_family = AF_INET;
	server.sin_port = htons(portno);
	server.sin_addr.s_addr = inet_addr(deststr);
	if (server.sin_addr.s_addr == 0xffffffff)
	{
		struct hostent *host;

		host = gethostbyname(deststr);
		if (host == NULL)
		{
			if (WSAGetLastError() == WSAHOST_NOT_FOUND)
			{
				DEBUG_OUT_DEFAULT("oReceiveBCast host not found failed.");
			}
			return -1;
		}

		unsigned int **addrptr = (unsigned int **)host->h_addr_list;

		while (*addrptr != NULL)
		{
			server.sin_addr.s_addr = *(*addrptr);

			// connect()が成功したらloopを抜けます
			if (connect(sock, (struct sockaddr *)&server, sizeof server) == 0)
			{
				break;
			}

			addrptr++;
			// connectが失敗したら次のアドレスで試します
		}

		// connectが全て失敗した場合
		if (*addrptr == NULL)
		{
			DEBUG_OUT_DEFAULT("ERROR: oReceiveBCast connect ALL failed.");
			return -1;
		}
	}
	else
	{
		// inet_addr()が成功したとき

		// connectが失敗したらエラーを表示して終了
		if (connect(sock, (SOCKADDR*)&server, sizeof server) != 0)
		{
			DEBUG_OUT_DEFAULT("ERROR: oReceiveBCast connect failed.");
			return -1;
		}
	}

	int n = send(sock, text, (int)strlen(text), 0);

	CString receive;
	if (!pReceive)
	{
		pReceive = &receive;
	}

	pReceive->Empty();

	return TransRecvTCP(sock, *pReceive);
}

int CheckNetwork(const char* phost, SOCKET* _sock/* = 0*/, hostent** _host/* = 0*/)
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	CoAutoSocket autoSock(sock);

	hostent* host = gethostbyname(phost);
	if (host == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		if (WSAGetLastError() == WSAHOST_NOT_FOUND)
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		return -1;
	}

	if (_sock)
	{
		*_sock = autoSock.Dispatch();
	}

	if (_host)
	{
		*_host = host;
	}

	return 0;
}

int GetLocalInfo(const char* phost, CString& ip_address, CString& mac_address)
{
	SOCKET sock = 0;
	hostent* host = 0;
	int result = CheckNetwork(phost, &sock, &host);
	if (result != 0)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	CoAutoSocket autoSock(sock);

	unsigned int** addrptr = (unsigned int **)host->h_addr_list;

	SOCKADDR_IN clientService = { 0 };
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = *(*addrptr);
	clientService.sin_port = htons(80);
	if (connect(sock, (SOCKADDR*)&clientService, sizeof clientService) == SOCKET_ERROR) 
	{
		DEBUG_OUT_DEFAULT("http connect failed.");
		return -1;
	}

	SOCKADDR_IN local = clientService;
	int length = sizeof local;
	if (getsockname(sock, (struct sockaddr *) &local, &length))
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	int retval = getnameinfo((struct sockaddr *) &local, length,
		ip_address.GetBuffer(NI_MAXHOST), NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
	ip_address.ReleaseBuffer();
	if (retval)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	IP_ADAPTER_INFO AdapterInfo[16] = { 0 };
	DWORD dwBufLen = sizeof AdapterInfo;
	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	for (PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; pAdapterInfo; pAdapterInfo = pAdapterInfo->Next)
	{
		if (ip_address.Compare(pAdapterInfo->IpAddressList.IpAddress.String) == 0)
		{
			BYTE* MACData = pAdapterInfo->Address;
			sprintf_s(mac_address.GetBuffer(36), 36, "%02X:%02X:%02X:%02X:%02X:%02X", 
				MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
			mac_address.ReleaseBuffer();
			break;
		}
	}

	return 0;
}

int SendToTcpSocket(LPCTSTR deststr, WORD portno, LPCTSTR text, CString* pReceive/* = NULL*/)
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		OutputViewer("SendToTcpSocket socket : %d", WSAGetLastError());
		return -1;
	}

	CoAutoSocket autoSocket(sock);

	struct sockaddr_in server = { 0 };
	server.sin_family = AF_INET;
	server.sin_port = htons(portno);
	server.sin_addr.s_addr = inet_addr(deststr);
	if (server.sin_addr.s_addr == 0xffffffff)
	{
		struct hostent *host;

		host = gethostbyname(deststr);
		if (host == NULL)
		{
			if (WSAGetLastError() == WSAHOST_NOT_FOUND)
			{
				OutputViewer("SendToTcpSocket host not found : %s", deststr);
			}
			return -1;
		}

		unsigned int **addrptr = (unsigned int **)host->h_addr_list;

		while (*addrptr != NULL)
		{
			server.sin_addr.s_addr = *(*addrptr);

			// connect()が成功したらloopを抜けます
			if (connect(sock, (struct sockaddr *)&server, sizeof server) == 0)
			{
				break;
			}

			addrptr++;
			// connectが失敗したら次のアドレスで試します
		}

		// connectが全て失敗した場合
		if (*addrptr == NULL)
		{
			OutputViewer("SendToTcpSocket connect : %d", WSAGetLastError());
			return -1;
		}
	}
	else
	{
		// inet_addr()が成功したとき

		// connectが失敗したらエラーを表示して終了
		if (connect(sock, (SOCKADDR*)&server, sizeof server) != 0)
		{
			// TODO: これは ERROR の場合があるが問題ない
			// OutputViewer("SendToTcpSocket connect : %d", WSAGetLastError());
			return -1;
		}
	}

	int n = send(sock, text, (int)strlen(text), 0);

	CString receive;
	if (!pReceive)
	{
		pReceive = &receive;
	}

	return (int)TransRecvTCP(sock, *pReceive);
}

int SendToUdpSocket(LPCTSTR deststr, WORD portno, LPCTSTR text)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		DEBUG_OUT_DEFAULT("oReceiveBCast socket faile.");
		return -1;
	}

	CoAutoSocket autoSocket(sock);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portno);
	addr.sin_addr.s_addr = inet_addr(deststr);

	int n = sendto(sock, text, (int)strlen(text), 0, (struct sockaddr *)&addr, sizeof addr);

	return 0;
}

BOOL SendToSocket(DWORD ipproto, LPCTSTR deststr, WORD portno, LPCTSTR text, CString* pReceive = NULL)
{
	if (ipproto == IPPROTO_TCP)
	{
		return SendToTcpSocket(deststr, portno, text, pReceive) == 0;
	}

	return SendToUdpSocket(deststr, portno, text) == 0;
}

int SendLineData(LPCTSTR text)
{
	SendToSocket(IPPROTO_TCP, P("http://exsample.jp/uniuni.php"), 6161, text);

	return 0;
}
