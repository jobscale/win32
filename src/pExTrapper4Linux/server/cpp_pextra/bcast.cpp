////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2012 Plustar Corporation. All rights reserved.

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <netdb.h>
#include <sys/ioctl.h> 
#include <net/if.h>  

#include "iftools.h"

struct _RECV_DATA
{
	char* buf;
	int len;
	_RECV_DATA* next;
	_RECV_DATA()
		: buf(0)
		, len(0)
		, next(0)
	{
	}
};

_RECV_DATA& operator+=(_RECV_DATA& targ, _RECV_DATA& src)
{
	if (src.len == 0)
	{
		return targ;
	}

	int nlen = targ.len + src.len;
	char* nbuf = (char*)malloc(nlen);
	memcpy(nbuf, targ.buf, targ.len);
	memcpy(nbuf + targ.len, src.buf, src.len);

	if (targ.buf)
	{
		free(targ.buf);
	}
	targ.buf = nbuf;
	targ.len = nlen;

	return targ;
}


int AcceptSocketSpecial()
{
	int sock0 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	CAutoSock autoclose(sock0);

	char ifname [IF_NAMESIZE] = { 0 };
	char ipaddr[INET_ADDRSTRLEN] = { 0 };
    char ipmask[INET_ADDRSTRLEN] = { 0 };
    char bcastaddr[INET_ADDRSTRLEN] = { 0 };
    char macaddr[17 + 1] = { 0 };

	GetAddrInfo(sock0, ipaddr, macaddr, ipmask, bcastaddr, ifname);

	printf("I/F Name: %s\n", ifname);
	printf("Addr: %s\n", ipaddr);
	printf("Mask: %s\n", ipmask);
	printf("Bcas: %s\n", bcastaddr);
	printf("Mac: %s\n", macaddr);
	printf("---------------------------\n");

	unsigned short portNo = 12083;

	int yes = 1;
	if (setsockopt(sock0, SOL_SOCKET, SO_BROADCAST, &yes, sizeof yes) == -1)
	{
		printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
		return -1;
	}

	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portNo);
	addr.sin_addr.s_addr = inet_addr(bcastaddr); // INADDR_ANY

	printf("sendto %s:%d return %s\n", bcastaddr, portNo, ipaddr);
	char sdat[1024] = { 0 };
	sprintf(sdat, "GET CONSOLE LINUX \"%s\"", ipaddr);
	sendto(sock0, sdat, strlen(sdat), 0, (sockaddr*)&addr, sizeof addr);

	return 0;
}

int main(int argc, char* argv[])
{
	char dir[] = "/tmp/pextra";
	char dir_work[] = "/tmp/pextra/cons";

	mkdir(dir, 0777);
	mkdir(dir_work, 0777);
	chdir(dir_work);

	AcceptSocketSpecial();
	return 0;
}
