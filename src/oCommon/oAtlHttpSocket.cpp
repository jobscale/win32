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

#include "oTools.h"
#include "oSocketTools.h"

#include "oAtlHttpSocket.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

INT CopyStringRight(CMemStr& value, CMemStr& target, INT length)
{
	CMemStr buffer;
	memcpy(buffer.GetBufferSetLength(length), &((LPCTSTR)target)[target.GetLength() - length], length);
	value.Empty();
	value += buffer;

	return length;
}

INT HttpRecvData(SOCKET sock, OHTTP_DATA* pData)
{
	// initialize
	pData->recvData.Empty();
	pData->contentsLength = 0;
	pData->headerData.Empty();

	// read
	INT bytesRecv = 0;
	CMemStr recvbuf;
	CMemStr oddLot;

	for (; ; )
	{
		bytesRecv = recv(sock, recvbuf.GetBuffer(RECV_SIZE_BIG), RECV_SIZE_BIG, 0);
		if (bytesRecv == SOCKET_ERROR || bytesRecv == WSAECONNRESET)
		{
			DEBUG_OUT_DEFAULT("http recv failed.");
			return -1;
		}

		if (bytesRecv == 0 && pData->contentsLength == 0)
		{
			if (oddLot.GetLength() == 0)
			{
				DEBUG_OUT_DEFAULT("no data recv failed.");
				return -1;
			}
			else
			{
				pData->contentsLength = oddLot.GetLength();
			}
		}
		else if (bytesRecv == 0 && pData->contentsLength == 3 MB)
		{
			break;
		}

		recvbuf.GetBufferSetLength(bytesRecv);
		oddLot += recvbuf;

		if (!pData->contentsLength)
		{
			int header_end = (int)oddLot.Find("\r\n\r\n");
			if (header_end == -1)
			{
				continue;
			}
			header_end += (int)strlen("\r\n\r\n");

			static const char* conlen = "Content-Length: ";
			static const size_t conlenlen = strlen(conlen);

			INT posLength = oddLot.Find(conlen);
			if (posLength != -1)
			{
				pData->contentsLength = strtol(&((LPCTSTR)oddLot)[posLength + conlenlen], NULL, 10);
				memcpy(pData->headerData.GetBufferSetLength(header_end), (LPCTSTR)oddLot, header_end);
				CopyStringRight(oddLot, oddLot, oddLot.GetLength() - header_end);

				pData->recvData.GetBuffer(pData->contentsLength);
			}
			else
			{
				int header_end2 = (int)oddLot.Find("\r\n", header_end);
				if (header_end2 == -1)
				{
					continue;
				}

				header_end2 += (int)strlen("\r\n");
				LPCTSTR lenPos = &((LPCTSTR)oddLot)[header_end];
				pData->contentsLength = strtol(lenPos, NULL, 16);
				if (pData->contentsLength == 0 && !(lenPos[0] == '0' && lenPos[1] == '\r' && lenPos[2] == '\n'))
				{
					OutputViewer("ERROR: http format failed.");
					pData->contentsLength = 3 MB;
					memcpy(pData->headerData.GetBufferSetLength(header_end), (LPCTSTR)oddLot, header_end);
					CopyStringRight(oddLot, oddLot, oddLot.GetLength() - header_end);
				}
				else
				{
					memcpy(pData->headerData.GetBufferSetLength(header_end2), (LPCTSTR)oddLot, header_end2);
					CopyStringRight(oddLot, oddLot, oddLot.GetLength() - header_end2);
				}
			}
		}

		pData->recvData += oddLot;
		oddLot.Empty();

		DWORD recvLength = pData->recvData.GetLength();
		if (recvLength >= pData->contentsLength)
		{
			pData->recvData.GetBufferSetLength(pData->contentsLength);
			break;
		}
	}

	return 0;
}

INT HttpSendRecv(SOCKET sock, const CMemStr& sendData, OHTTP_DATA* pData)
{
	// send
	int len = sendData.GetLength();
	int bytesSent = send(sock, sendData, len, 0);

	if (bytesSent != len)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	// read
	return HttpRecvData(sock, pData);
}

INT HttpSocket(OHTTP_DATA* pData, LPCTSTR type)
{
	if (pData->proxy.IsEmpty())
	{
		pData->proxy = pData->target.Right(pData->target.GetLength() - 7);
		INT pos = pData->proxy.Find("/");
		if (pos > 0)
		{
			pData->proxy = pData->proxy.Left(pos);
		}
		pData->proxyPort = 80;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) 
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	CoAutoSocket autoSocket(sock);

	hostent* host = gethostbyname(pData->proxy);
	if (host == NULL)
	{
		DEBUG_OUT_DEFAULT("failed.");
		if (WSAGetLastError() == WSAHOST_NOT_FOUND)
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		return -1;
	}

	unsigned int** addrptr = (unsigned int **)host->h_addr_list;

	SOCKADDR_IN clientService = { 0 };
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = *(*addrptr);
	clientService.sin_port = htons(pData->proxyPort);

	if (connect(sock, (SOCKADDR*)&clientService, sizeof clientService) == SOCKET_ERROR) 
	{
		DEBUG_OUT_DEFAULT("http connect failed.");
		return -1;
	}

	CMemStr bufSend;
	sprintf_s(bufSend.GetBuffer(5000), 5000, P(
		"%s %s HTTP/1.1\r\n"
		"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
		"Host: %s\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: %d\r\n"
		"Proxy-Connection: Keep-Alive\r\n\r\n"),
		type,
		(LPCTSTR)pData->target,
		(LPCTSTR)pData->proxy,
		pData->sendData.GetLength()
	);

	bufSend.ReleaseBuffer();
	bufSend += pData->sendData;

	int result = HttpSendRecv(sock, bufSend, pData);

	return result;
}

INT HttpPost(OHTTP_DATA* pData)
{
	INT resPost = HttpSocket(pData, "POST");

	return resPost;
}

INT HttpGet(OHTTP_DATA* pData)
{
	INT resPost = HttpSocket(pData, "GET");

	return resPost;
}
