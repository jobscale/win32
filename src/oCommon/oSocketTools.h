///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#ifndef __O_SOCKETTOOLS_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// �C���N���[�h

#include <winsock2.h>
#include <atlstr.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// ��`

#define RECV_SIZE_BIG (256)

///////////////////////////////////////////////////////////////////////////////////////////////////
// �I�u�W�F�N�g

class CWSAInterface
{
private:
	WSADATA wsaData;
public:
	CWSAInterface();
	virtual ~CWSAInterface();
};

class CoAutoSocket
{
protected:
	SOCKET m_socket;
public:
	CoAutoSocket(SOCKET socket);
	virtual ~CoAutoSocket();

	SOCKET Dispatch();
};

struct OSOCKET_DATA
{
	LPTHREAD_START_ROUTINE func;
	SOCKET socket;
	sockaddr_in sockaddr;
	OSOCKET_DATA(LPTHREAD_START_ROUTINE _func, SOCKET _socket, sockaddr_in _sockaddr)
		: func(_func)
		, socket(_socket)
		, sockaddr(_sockaddr)
	{
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// �֐�

int CheckNetwork(const char* phost, SOCKET* _sock = 0, hostent** _host = 0);
int GetLocalInfo(const char* phost, CString& ip_address, CString& mac_address);

int SendToTcpSocket(LPCTSTR deststr, WORD portno, LPCTSTR text, CString* pReceive = NULL);
int SendToUdpSocket(LPCTSTR deststr, WORD portno, LPCTSTR text);

LRESULT TransRecvUDP(SOCKET sock, CString& recvData);
LRESULT TransRecvTCP(SOCKET sock, CString& recvData);
LRESULT TransRecvBrowser(SOCKET sock, CString& recvData);

LRESULT DataSendConsole(LPCTSTR deststr, WORD portno, LPCTSTR text, CString* pReceive = NULL);
int SendLineData(LPCTSTR text);

#endif // __O_SOCKETTOOLS_H__
