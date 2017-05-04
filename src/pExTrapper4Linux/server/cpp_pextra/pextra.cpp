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
	char* nbuf = (char*)malloc(nlen + 1);
	memcpy(nbuf, targ.buf, targ.len);
	memcpy(nbuf + targ.len, src.buf, src.len);
	nbuf[targ.len + src.len] = 0;

	if (targ.buf)
	{
		free(targ.buf);
	}
	targ.buf = nbuf;
	targ.len = nlen;

	return targ;
}

#define RECV_SIZE_BIG (256)

void* socket_thread(void* thdata)
{
	int* ppara = (int*)thdata;

	int sock = ((int*)ppara)[0];
	sockaddr_in* client = ((sockaddr_in**)ppara)[1];

	static int no = 0;
	no++;
	printf("No. %d\n", no);
	printf("Connected from %s\n", inet_ntoa(client->sin_addr));

	_RECV_DATA recvData;
	_RECV_DATA pTarg;
	pTarg.buf = (char*)malloc(RECV_SIZE_BIG);

	for (; ; )
	{
		int bytesRecv = recv(sock, pTarg.buf, RECV_SIZE_BIG, 0/*, timeout*/);
		if (bytesRecv == 0 || bytesRecv == -1)
		{
			return 0;
		}

		pTarg.len = bytesRecv;
		recvData += pTarg;

		if (bytesRecv < RECV_SIZE_BIG)
		{
			break;
		}
	}

	char tern[] = "OK!";
	send(sock, tern, strlen(tern), 0);

	close(sock);

	free(pTarg.buf);

	printf("%s", recvData.buf);
	free(recvData.buf);

	return 0;
}

int AcceptSocketSpecial()
{
	int sock0 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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

	unsigned short portNo = 12055;

	close(sock0);

	sock0 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	CAutoSock autoclose(sock0);

	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portNo);
	addr.sin_addr.s_addr = INADDR_ANY; // inet_addr(ipaddr);

	sockaddr_in* client = new sockaddr_in;

	if (bind(sock0, (sockaddr*)&addr, sizeof addr) == -1)
	{
		printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
		return -1;
	}

	if (listen(sock0, 5) == -1)
	{
		printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
		return -1;
	}

	for (; ; )
	{
		socklen_t len = sizeof(sockaddr_in);
		int sock = accept(sock0, (sockaddr*)client, &len);
		if (sock == -1)
		{
			break;
		}

		void** socketData = new void*[2];
		socketData[0] = (void*)sock;
		socketData[1] = (void*)client;
		pthread_t thread = 0;
		int res = pthread_create(&thread, 0, socket_thread, socketData);

		if (res == 0)
		{
			pthread_detach(thread);
		}
		else
		{
			printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
		}
	}

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
