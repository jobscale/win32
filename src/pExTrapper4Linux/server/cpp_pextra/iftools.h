////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2012 Plustar Corporation. All rights reserved.

#ifndef __IFTOOLS_H__
#define __IFTOOLS_H__

class CAutoSock
{
protected:
	int _sock;
public:
	CAutoSock(int sock)
		: _sock(sock)
	{
	}
	virtual ~CAutoSock()
	{
		close(_sock);
	}
};

int GetAddrInfo(int sock, char* ipaddr, char* macaddr, char* ipmask, char* bcastaddr, char* ifname);

#endif // __IFTOOLS_H__
