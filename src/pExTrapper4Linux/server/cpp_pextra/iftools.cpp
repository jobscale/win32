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

int GetAddrInfo(int sock, char* ipaddr, char* macaddr, char* ipmask, char* bcastaddr, char* ifname)
{
	hostent* host = gethostbyname("dl.plustar.jp");
	if (host == NULL)
	{
		printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
		return -1;
	}

	unsigned int** addrptr = (unsigned int **)host->h_addr_list;

	sockaddr_in hints = { 0 };
	hints.sin_family = AF_INET;
	hints.sin_port = htons(80);
	hints.sin_addr.s_addr = **addrptr; // INADDR_ANY

	if (connect(sock, (sockaddr*)&hints, sizeof hints) == -1)
	{
		printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
		return -1;
	}

	sockaddr_in local = hints;
	socklen_t length = sizeof local;
	if (getsockname(sock, (sockaddr*)&local, &length))
	{
		printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
		return -1;
	}

	char servaddr[NI_MAXHOST + 1] = { 0 };
	int retval = getnameinfo((sockaddr*)&local, length, servaddr, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
	if (retval)
	{
		printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
		return -1;
	}

	for (int i = 1; i < 5; i++)
	{
		if_indextoname(i, ifname);

		ifreq stcIFRreg = { 0 };
		strcpy(stcIFRreg.ifr_name, ifname);

		if (ioctl(sock, SIOCGIFFLAGS, &stcIFRreg) == -1)
		{
			printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
			return -1;
		}

		int iDevIdx = if_nametoindex(stcIFRreg.ifr_name);

		if (ioctl(sock, SIOCGIFADDR, &stcIFRreg) == -1)
		{
			printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
			return -1;
		}

		sockaddr_in stcAddr = { 0 };
		memcpy(&stcAddr, &stcIFRreg.ifr_addr, sizeof stcAddr);
		strcpy(ipaddr, inet_ntoa(stcAddr.sin_addr));

		if (ioctl(sock, SIOCGIFNETMASK, &stcIFRreg) == -1)
		{
			printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
			return -1;
		}
		memcpy(&stcAddr, &stcIFRreg.ifr_netmask, sizeof stcIFRreg.ifr_netmask); 
		strcpy(ipmask, inet_ntoa(stcAddr.sin_addr));

		if (ioctl(sock, SIOCGIFHWADDR, &stcIFRreg) == -1)
		{
			printf("ERROR: %s(%d): \n", __FILE__, __LINE__);
			return -1;
		}
		unsigned char* p = (unsigned char*)&stcIFRreg.ifr_hwaddr.sa_data;
		sprintf(macaddr, "%02x:%02x:%02x:%02x:%02x:%02x", *p, *(p+1), *(p+2), *(p+3), *(p+4), *(p+5));

		unsigned long dwIpAddr = inet_addr(ipaddr);
		unsigned long dwIpMask = inet_addr(ipmask);
		dwIpAddr |= ~dwIpMask;
		in_addr ina = { 0 };
		ina.s_addr = dwIpAddr;
		const char* _bcastaddr = inet_ntoa(ina);
		strcpy(bcastaddr, _bcastaddr);

		if (strcmp(servaddr, ipaddr) == 0)
		{
			break;
		}
	}

	return 0;
}
