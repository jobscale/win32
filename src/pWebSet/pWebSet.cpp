///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
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

#include "stdafx.h"
#include "pWebSet.h"

#include "../oCommon/oTools.h"

#define _OFC_EXPORT_
#include "../ofc/oString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CpWebSetApp

BEGIN_MESSAGE_MAP(CpWebSetApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CpWebSetApp �R���X�g���N�V����

CpWebSetApp::CpWebSetApp()
{
}

#include <iphlpapi.h>
#pragma comment (lib, "iphlpapi")
//#include <ws2tcpip.h>

/* �A�_�v�^�̃^�C�v�i���f�B�A��ʁj������ */
/* �l�idwIfType�j�𕶎���ɕϊ�����       */
const char *getString_IfType(DWORD dwIfType)
{
    switch(dwIfType) {
    case IF_TYPE_ETHERNET_CSMACD:   /* 6 */
        return "Ethernet";
    case IF_TYPE_SOFTWARE_LOOPBACK: /* 24 */
        return "Loopback";
    case IF_TYPE_TUNNEL:            /* 131 */
        return "Tunnel";
    default:
        return "Other Adapter Type";
    }
}

/* �A�_�v�^�̏�Ԃ�����                       */
/* IF_OPER_STATUS�^�̗񋓎q�𕶎���ɕϊ����� */
const char *getString_IF_OPER_STATUS(IF_OPER_STATUS ios)
{
    const char *szOperationalStatus[] = {
        "",                /* IF_OPER_STATUS�񋓎q�̒�`�ł�0�͌��� */
        "Up",
        "Down",
        "Testing",
        "Unknown",
        "Dormant",
        "NotPresent",
        "LowerLayerDown"
    };

    return szOperationalStatus[ios];
}

/* ������\��ULONG�l�ilt�j�𕶎���ɕϊ���szBuf�֏������ށB */
/* �����llt��ULONG_MAX�ɓ������ꍇ��"������\��������       */
/* "infinite"��ԋp����BszBuf�ɏ\���ȑ傫���isize�j���m��  */
/* ����Ă��Ȃ��ꍇ��NULL��ԋp����B                       */
const char *getString_Lifetime(char *szBuf, size_t size, ULONG lt)
{
    if (lt == ULONG_MAX) {
        if (_snprintf_s(szBuf, size, size, "%s", "infinite") < 0)
            return NULL;
    } else {
        if (_snprintf_s(szBuf, size, size, "%u", lt) < 0)
            return NULL;
    }

    return szBuf;
}

/* IP�A�h���X�̃v���t�B�b�N�X(�l�b�g���[�N�j���̕t�^�� */
/* ��\��IP_PREFIX_ORIGIN�^�̗񋓎q�𕶎���ɕϊ�����  */
const char *getString_IP_PREFIX_ORIGIN(IP_PREFIX_ORIGIN ipo)
{
    const char *szOriginString[] = {
        "Other",
        "Manual",
        "WellKnown",
        "Dhcp",
        "RouterAdvertisement"
    };

    return szOriginString[ipo];
}

/* IP�A�h���X�̃C���^�[�t�F�C�XID(�z�X�g���j�̕t�^�� */
/* ��\��IP_SUFFIX_ORIGIN�^�̗񋓎q�𕶎���ɕϊ�����*/
const char *getString_IP_SUFFIX_ORIGIN(IP_SUFFIX_ORIGIN iso)
{
    const char *szOriginString[] = {
        "Other",
        "Manual",
        "WellKnown",
        "Dhcp",
        "LinkLayerAddress",
        "Random"
    };

    return szOriginString[iso];
}

/* IPv6�A�h���X��DAD��Ԃ�����              */
/* IP_DAD_STATE�^�̗񋓎q�𕶎���ɕϊ����� */
const char *getString_IP_DAD_STATE(IP_DAD_STATE ids)
{
    const char *szDADstate[] = {
        "Invalid",
        "Tentative",
        "Duplicate",
        "Deprecated",
        "Preferred"
    };

    return szDADstate[ids];
} 

/* �A�_�v�^�̑����������t���O���𕶎���ɕϊ�����B�t���O�̖��O��    */
/* �J���}(,)�ŋ�؂��ďo�͂����                                       */
/* szBuf�ɏ\���ȑ傫���isize�j���m�ۂ���Ă��Ȃ��ꍇ��NULL��ԋp����B */
char *getString_AdapterFlags(char *szBuf, size_t size, DWORD dwFlags)
{
    char *p = szBuf;
    int len;
    struct _FlagList {
        DWORD Flag;
        const char *szFlagName;
    } FlagList[] = {
        { IP_ADAPTER_DDNS_ENABLED, "DDNS_ENABLED" },
        { IP_ADAPTER_REGISTER_ADAPTER_SUFFIX, "REGISTER_ADAPTER_SUFFIX" },
        { IP_ADAPTER_DHCP_ENABLED, "DHCP_ENABLED" },
        { IP_ADAPTER_RECEIVE_ONLY, "RECEIVE_ONLY" },
        { IP_ADAPTER_NO_MULTICAST, "NO_MULTICAST" },
        { IP_ADAPTER_IPV6_OTHER_STATEFUL_CONFIG, "OTHER_STATEFUL_CONFIG" }
    };

	int count = sizeof FlagList / sizeof _FlagList;
	int i = 0;
    for (; i < count; ++i) {
        if (dwFlags & FlagList[i].Flag) {
            if ((len = _snprintf_s(p, size, size, "%s", FlagList[i].szFlagName)) < 0)
                return NULL;
            p+= len;
            size -= len;

            break;
        }
    }

    for (i++; i < count; ++i) {
        if (dwFlags & FlagList[i].Flag) {
            if ((len = _snprintf_s(p, size, size, ",%s", FlagList[i].szFlagName)) < 0)
                return NULL;
            p+= len;
            size -= len;
        }
    }

    return szBuf;
}

INT GetMac(const CString& ipaddr, CString& mac, CString& adapterName)
{
    /*
     * WinSock�̏�����
     */
    WSADATA wsaData = { 0 };
    if (WSAStartup(WINSOCK_VERSION, &wsaData)) {
        fprintf(stderr, "cannot initilize WinSock\n");
        exit(1);
    }

    /*
     * �߂�l���i�[����̂ɕK�v�ȃo�b�t�@�̑傫�������߂�
     * dwSize�ɂ��̒l��������BFirstPrefix�����o�Ƀv���t�B�b�N�X
     * ���𓾂邽�߁B��2�����̃t���O��GAA_FLAG_INCLUDE_PREFIX�𗧂Ă�
     */
    DWORD dwSize = 0;
    DWORD dwRet = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &dwSize);
    if (dwRet != ERROR_BUFFER_OVERFLOW) {
        fprintf(stderr, "no enough buffer\n");
        exit(1);
    }

    /*
     * pAdapterAddresses��dwSize�̑傫���̃o�b�t�@���m�ۂ���
     */
    PIP_ADAPTER_ADDRESSES pAdapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(dwSize);
    if (pAdapterAddresses == NULL) {
        fprintf(stderr, "no enough buffer\n");
        exit(1);
    }

    /*
     * GetAdaptersAddresses()���ēx�Ăяo��pAdapterAddresses��
     * �A�_�v�^�����擾����BFirstPrefix�����o�Ƀv���t�B�b�N�X
     * ���𓾂邽�߁B��2�����̃t���O��GAA_FLAG_INCLUDE_PREFIX�𗧂Ă�
     */
    dwRet = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX,
                                 NULL, pAdapterAddresses, &dwSize);
    if (dwRet != ERROR_SUCCESS) {
        fprintf(stderr, "GetAdaptersAddresses() failed\n");
        exit(1);
    }

    /*
     * ���ׂẴA�_�v�^�ɂ��āA���ꂼ�ꂪ������\������
     * pAA�����ݒ��ڂ���A�_�v�^��IP_ADAPTER_ADDRESSES�\���̂�
     * �C���X�^���X
     */
	PIP_ADAPTER_ADDRESSES pAA = { 0 };
    for (pAA = pAdapterAddresses; pAA; pAA = pAA->Next) {
        char szAdapterName[BUFSIZ];
        char szAdapterDescription[BUFSIZ];
        char szDnsSuffix[BUFSIZ];
        int len;
        PIP_ADAPTER_UNICAST_ADDRESS pUnicastAddress;
        PIP_ADAPTER_ANYCAST_ADDRESS pAnycastAddress;
        PIP_ADAPTER_MULTICAST_ADDRESS pMulticastAddress;
        PIP_ADAPTER_DNS_SERVER_ADDRESS pDnsServerAddress;
        char szFlagsString[BUFSIZ];
        PIP_ADAPTER_PREFIX pIpAdapterPrefix;
        int i;

		CString ipAddress;

        /*
         * FriendlyName�����o�ɂ��ǂȃA�_�v�^����\������
         */
        /* FriendlyName�̓��j�R�[�h�̂��߃}���`�o�C�g */
        /* �����iShift JIS�j�ɕϊ�����                */
        len = WideCharToMultiByte(CP_ACP, 0,
                                  pAA->FriendlyName,
                                  (int)wcslen(pAA->FriendlyName),
                                  szAdapterName, sizeof szAdapterName,
                                  NULL, NULL);
// zero cast
		if (len == 0) {
            fprintf(stderr, "cannot convert adapter name\n");
            exit(1);
        }
        szAdapterName[len] = '\0';
        TRACE("Adapter Name : %s\n", szAdapterName);

        /*
         * AdapterName�����o�ɂ��A�_�v�^���iUID�`���j��\������
         */
        TRACE("  Adapter UID = %s\n", pAA->AdapterName);

        /*
         * Description�����o����A�_�v�^�̉����\������
         */
        /* Description�̓��j�R�[�h�̂��߃}���`�o�C�g  */
        /* �����iShift JIS�j�ɕϊ�����                */
        len = WideCharToMultiByte(CP_ACP, 0,
                                  pAA->Description,
                                  (int)wcslen(pAA->Description),
                                  szAdapterDescription,
                                  sizeof szAdapterDescription,
                                  NULL, NULL);
// zero cast
        if (len == 0) {
            fprintf(stderr, "cannot convert adapter description\n");
            exit(1);
        }
        szAdapterDescription[len] = '\0';
        TRACE("  Description : %s\n", szAdapterDescription);

        /*
         * IfType�����o�ɂ��A�_�v�^�̃��f�B�A�^�C�v��\������
         */
        TRACE("  Type : %s\n", getString_IfType(pAA->IfType));

        /*
         * OperStatus�����o�ɂ��A�_�v�^�̏�Ԃ�\������
         */
        TRACE("  Operational Status : %s\n",
               getString_IF_OPER_STATUS(pAA->OperStatus));

        /*
         * FirstUnicastAddress�����o�ɂ��A�_�v�^��
         * �t�^���ꂽ���j�L���X�g�A�h���X��\������
         */
        for (i = 0, pUnicastAddress = pAA->FirstUnicastAddress;
             pUnicastAddress;
             i++, pUnicastAddress = pUnicastAddress->Next) {
            char szAddress[NI_MAXHOST];
            char szValidLifetime[BUFSIZ];
            char szPreferredLifetime[BUFSIZ];
            char szLeaseLifetime[BUFSIZ];

            /*
             * ���j�L���X�g�A�h���X�̕\������
             */
            /* �l�b�g���[�N�t�H�[�}�b�g�i�o�C�i���j�Ŋi�[�����A�h���X�� */
            /* �ǂȕ�����ipresentation format�j�ɕϊ�����              */
            if (getnameinfo(pUnicastAddress->Address.lpSockaddr,
                            pUnicastAddress->Address.iSockaddrLength,
                            szAddress, sizeof szAddress, NULL, 0,
                            NI_NUMERICHOST)) {
                fprintf(stderr,
                        "can't convert network format to presentation format");
                exit(1);
            }
            /* ���j�L���X�g�A�h���X�̕������\������ */
            TRACE("  Unicast Address#%d\n", i);
            TRACE("    Family=%s, Addr=%s\n",
                   ((pUnicastAddress->Address.lpSockaddr)->sa_family
                       == AF_INET) ? "IPv4" : "IPv6",
                   szAddress);
			ipAddress = szAddress;

            /* �����𕶎���ɕϊ����\������ */
            if (getString_Lifetime(szValidLifetime,
                                   sizeof szValidLifetime,
                                   pUnicastAddress->ValidLifetime) == NULL) {
                fprintf(stderr, "cannot convert to string\n");
                exit(1);
            }
            if (getString_Lifetime(szPreferredLifetime,
                                   sizeof szPreferredLifetime,
                                   pUnicastAddress->PreferredLifetime)
                                                                == NULL) {
                fprintf(stderr, "cannot convert to string\n");
                exit(1);
            }
            if (getString_Lifetime(szLeaseLifetime,
                                   sizeof szLeaseLifetime,
                                   pUnicastAddress->LeaseLifetime) == NULL) {
                fprintf(stderr, "cannot convert to string\n");
                exit(1);
            }
            TRACE("    ValidLifetime=%s, "
                   "PreferredLifetime=%s, "
                   "LeaseLifetime=%s\n",
                   szValidLifetime, szPreferredLifetime, szLeaseLifetime);

            /*
             * IP�A�h���X�̎擾���iPREFIX, SUFFIX�j��\������
             */
            TRACE("    prefix origin : %s\n",
                   getString_IP_PREFIX_ORIGIN(pUnicastAddress->PrefixOrigin));
            TRACE("    suffix origin : %s\n",
                   getString_IP_SUFFIX_ORIGIN(pUnicastAddress->SuffixOrigin));

            /*
             * IPv6�A�h���X�̏ꍇDAD��Ԃ�\������
             */
            if ((pUnicastAddress->Address.lpSockaddr)->sa_family == AF_INET6) {
                TRACE("    DAD state : %s\n",
                       getString_IP_DAD_STATE(pUnicastAddress->DadState));
            }
        }

        /*
         * FirstAnycastAddress�����o�ɂ��G�j�[�L���X�g�A�h���X��\������
         */
        for (i = 0, pAnycastAddress = pAA->FirstAnycastAddress;
             pAnycastAddress;
             i++, pAnycastAddress = pAnycastAddress->Next) {
            char szAddress[NI_MAXHOST];

            /*
             * �G�j�[�L���X�g�A�h���X�̕\������
             */
            /* �l�b�g���[�N�t�H�[�}�b�g�i�o�C�i���j�Ŋi�[�����A�h���X�� */
            /* �ǂȕ�����ipresentation format�j�ɕϊ�����              */
            if (getnameinfo(pAnycastAddress->Address.lpSockaddr,
                            pAnycastAddress->Address.iSockaddrLength,
                            szAddress, sizeof szAddress, NULL, 0,
                            NI_NUMERICHOST)) {
                fprintf(stderr,
                        "can't convert network format to presentation format");
                exit(1);
            }
            /* �G�j�[�L���X�g�A�h���X�̕������\������ */
            TRACE("  Anycast Address#%d\n", i);
            TRACE("    Family=%s, Addr=%s\n",
                   ((pAnycastAddress->Address.lpSockaddr)->sa_family
                                                 == AF_INET) ? "IPv4" : "IPv6",
                   szAddress);
        }

        /*
         * FirstMulticastAddress�����o�ɂ��}���`�L���X�g�A�h���X��\������
         */
        for (i = 0, pMulticastAddress = pAA->FirstMulticastAddress;
             pMulticastAddress;
             i++, pMulticastAddress = pMulticastAddress->Next) {
            char szAddress[NI_MAXHOST];

            /*
             * �}���`�L���X�g�A�h���X�̕\������
             */
            /* �l�b�g���[�N�t�H�[�}�b�g�i�o�C�i���j�Ŋi�[�����A�h���X�� */
            /* �ǂȕ�����ipresentation format�j�ɕϊ�����              */
            if (getnameinfo(pMulticastAddress->Address.lpSockaddr,
                            pMulticastAddress->Address.iSockaddrLength,
                            szAddress, sizeof szAddress, NULL, 0,
                            NI_NUMERICHOST)) {
                fprintf(stderr,
                        "can't convert network format to presentation format");
                exit(1);
            }
            /* �}���`�L���X�g�A�h���X�̕������\������ */
            TRACE("  Multicast Address#%d\n", i);
            TRACE("    Family=%s, Addr=%s\n",
                   ((pMulticastAddress->Address.lpSockaddr)->sa_family
                                                == AF_INET) ? "IPv4" : "IPv6",
                   szAddress);
        }
        
        /*
         * FirstDnsServerAddress�����o�ɂ��DNS�T�[�o�̃A�h���X��\������
         */
        for (i = 0, pDnsServerAddress = pAA->FirstDnsServerAddress;
             pDnsServerAddress;
             i++, pDnsServerAddress = pDnsServerAddress->Next) {
            char szAddress[NI_MAXHOST];

            /*
             * DNS�T�[�o�̃A�h���X�̕\������
             */
            /* �l�b�g���[�N�t�H�[�}�b�g�i�o�C�i���j�Ŋi�[�����A�h���X�� */
            /* �ǂȕ�����ipresentation format�j�ɕϊ�����              */
            if (getnameinfo(pDnsServerAddress->Address.lpSockaddr,
                            pDnsServerAddress->Address.iSockaddrLength,
                            szAddress, sizeof szAddress, NULL, 0,
                            NI_NUMERICHOST)) {
                fprintf(stderr,
                        "can't convert network format to presentation format");
                exit(1);
            }
            /* DNS�T�[�o�A�h���X�̕������\������ */
            TRACE("  DNS Server Address#%d\n", i);
            TRACE("    Family=%s, Addr=%s\n",
                   ((pDnsServerAddress->Address.lpSockaddr)->sa_family
                                                == AF_INET) ? "IPv4" : "IPv6",
                   szAddress);
        }

        /*
         * DnsSuffix�����o�ɂ��DNS�T�t�B�b�N�X�i�h���C�����j��\������
         */
        /* DnsSuffix�̓��j�R�[�h�̂��߃}���`�o�C�g */
        /* �����iShift JIS�j�ɕϊ�����             */
        if (wcslen(pAA->DnsSuffix) != 0) {
            len = WideCharToMultiByte(CP_ACP, 0,
                                      pAA->DnsSuffix,
                                      (int)wcslen(pAA->DnsSuffix),
                                      szDnsSuffix,
                                      sizeof szDnsSuffix,
                                      NULL, NULL);
// zero cast
            if (len == 0) {
                fprintf(stderr, "cannot convert DNS suffix\n");
                exit(1);
            }
            szDnsSuffix[len] = '\0';
            TRACE("  DNS suffix : %s\n", szDnsSuffix);
        }

        /*
         * PhysicalAddress, PhysicalAddressLength�����o�ɂ��
         * �A�_�v�^�̕����A�h���X�iMAC�A�h���X�j��\������
         */
		CString physicalAddress;
        if (pAA->PhysicalAddressLength != 0) {
			CString hex;
			int count = pAA->PhysicalAddressLength - 1;
            for (i = 0; i < count; ++i)
			{
				hex.Format("%02X:", pAA->PhysicalAddress[i]);
				physicalAddress += hex;
			}
			hex.Format("%02X", pAA->PhysicalAddress[pAA->PhysicalAddressLength - 1]);
			physicalAddress += hex;
        }
        TRACE("  PhysicalAddress : %s\n", (LPCTSTR)physicalAddress);

        TRACE("  + AdapterName : %s\n", pAdapterAddresses->AdapterName);
		if (ipaddr == ipAddress)
		{
			adapterName = pAdapterAddresses->AdapterName;
			mac = physicalAddress;
		}

        /*
         * Mtu�����o�ɂ��A�_�v�^��MTU�l��\������
         */
        TRACE("  MTU : %d\n", pAA->Mtu);

        /*
         * Ipv6IfIndex�����o�ɂ��C���^�[�t�F�C�X�C���f�b�N�X��\������
         *   fe80::1%6 --- %�Ɍ��ɋL�q�����A�_�v�^�������ԍ��ƈ�v
         */
        TRACE("  IPv6 Interface Index : %d\n", pAA->Ipv6IfIndex);

        /*
         * Flags�����o�ɂ��A�_�v�^�̑�����\������
         */
        if (getString_AdapterFlags(szFlagsString, sizeof szFlagsString,
                                   pAA->Flags) == NULL) {
            fprintf(stderr, "cannot convert to string\n");
            exit(1);
        }
        TRACE("  Flags: %s\n", szFlagsString);

        /*
         * ZoneIndices�����o�ɂ��]�[������\������
         */
        TRACE("  Zone Info: if %d, link %d, subnet %d, admin %d, site %d, "
               "org %d, global %d\n",
               pAA->ZoneIndices[ScopeLevelInterface],
               pAA->ZoneIndices[ScopeLevelLink],
               pAA->ZoneIndices[ScopeLevelSubnet],
               pAA->ZoneIndices[ScopeLevelAdmin],
               pAA->ZoneIndices[ScopeLevelSite],
               pAA->ZoneIndices[ScopeLevelOrganization],
               pAA->ZoneIndices[ScopeLevelGlobal]);


        /*
         * FirstPrefix�����o�ɂ��A�_�v�^�̎��v���t�B�b�N�X��\������
         */
        for (i = 0, pIpAdapterPrefix= pAA->FirstPrefix;
             pIpAdapterPrefix;
             i++, pIpAdapterPrefix = pIpAdapterPrefix->Next) {
            char szAddress[NI_MAXHOST];

            if (getnameinfo(pIpAdapterPrefix->Address.lpSockaddr,
                            pIpAdapterPrefix->Address.iSockaddrLength,
                            szAddress, sizeof szAddress, NULL, 0,
                            NI_NUMERICHOST)) {
                fprintf(stderr,
                        "can't convert network format to presentation format");
                exit(1);
            }

            TRACE("  Prefix#%d\n", i);
            switch((pIpAdapterPrefix->Address.lpSockaddr)->sa_family) {
            case AF_INET:
                TRACE("    Family=IPv4, Addr=%s\n", szAddress);
                break;
            case AF_INET6:
                TRACE("    Family=IPv6, Addr=%s/64\n", szAddress);
                break;
            default:
                TRACE("    Family=Unknown, Addr=%s\n", szAddress);
            }
        }

        putchar('\n');
    }

    /*
     * �A�_�v�^�����i�[���Ă����̈���J������
     */
    free(pAdapterAddresses);

    /*
     * WinSock�̃N���[���i�b�v
     */
    WSACleanup();

	return 0;
}

DWORD GetIpMask(const CString& ipaddr, CString& ipmask)
{
	//��P�p�����[�^��NULL�������āA�K�v�T�C�Y�� DWORD d �Ɏ擾����
	DWORD size = 0;
	GetIpAddrTable(NULL, &size, FALSE);

	//MIB_IPADDRTABLE�ւ̃|�C���^�ɁA�K�v�T�C�Y���m�ۂ���
	PMIB_IPADDRTABLE IpAddrTable = (PMIB_IPADDRTABLE)new char[size];
	if (IpAddrTable)
	{
		//�A�h���X�e�[�u���̎擾
		if (GetIpAddrTable((PMIB_IPADDRTABLE)IpAddrTable, &size, FALSE) == NO_ERROR)
		{
			//�擾�����e�[�u�����́AdwNumEntries�ŕ�����
			for (DWORD i = 0 ; i < IpAddrTable->dwNumEntries ; i++)
			{
				in_addr ina = { 0 };
				ina.S_un.S_addr = IpAddrTable->table[i].dwAddr;
				if (ipaddr.Compare(inet_ntoa(ina)) == 0)
				{
					ina.S_un.S_addr = IpAddrTable->table[i].dwBCastAddr;
					CString BCastAddr = inet_ntoa(ina);

					ina.S_un.S_addr = IpAddrTable->table[i].dwMask;
					ipmask = inet_ntoa(ina);

					break;
				}
			}
		}
		//MIB_IPADDRTABLE�ւ̃|�C���^�ɁA�m�ۂ������������������
		delete [] IpAddrTable;
	}

	 return 0;
}

INT GetHostInfoCore(CString& hostname, CString& ipaddr, CString& ipmask, CString& username, CString& macaddr, CString& adapterName)
{
	WSADATA wsaData = { 0 };
	if (WSAStartup(WINSOCK_VERSION, &wsaData) != NO_ERROR)
	{
		return -1;
	}
	if (gethostname(hostname.GetBuffer(1024),  1024) != NO_ERROR)
	{
		WSACleanup();
		return -1;
	}
	hostname.ReleaseBuffer();
	LPHOSTENT pEnt = gethostbyname(hostname);
	if (!pEnt)
	{
		WSACleanup();
		return -1;
	}
	HOSTENT ent = *pEnt;
	WSACleanup();

	hostname = ent.h_name;
	ipaddr.Format("\0 %d.%d.%d.%d" + 2, (BYTE)ent.h_addr_list[0][0], (BYTE)ent.h_addr_list[0][1], (BYTE)ent.h_addr_list[0][2], (BYTE)ent.h_addr_list[0][3]);

	DWORD dwSize = UNLEN + 1;
	GetUserName(username.GetBuffer(dwSize), &dwSize);
	username.ReleaseBuffer();

	GetIpMask(ipaddr, ipmask);

	GetMac(ipaddr, macaddr, adapterName);

	return 0;
}

HKEY GetSettingReg()
{
	// Open the registry key for ALL access. 
	HKEY hKey = NULL;
	DWORD dwDisposition = 0;
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM", 0, REG_NONE, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
	{
		// The registry key was unable to be created. Return.
		return NULL;
	}

	CString hostname, ipaddr, ipmask, username, macaddr, adapterName;
	GetHostInfoCore(hostname, ipaddr, ipmask, username, macaddr, adapterName);

	CString dnsName;

	CString regName;
	DWORD regSize = 1024;
	FILETIME lastTime = { 0 };
	for (DWORD i = 0; RegEnumKeyEx(hKey, i, regName.GetBuffer(1024), &regSize, NULL, NULL, NULL, &lastTime) != ERROR_NO_MORE_ITEMS; i++)
	{
		regSize = 1024;
		regName.ReleaseBuffer();

		if (strncmp(regName, "ControlSet", strlen("ControlSet")) != 0)
		{
			continue;
		}
		if (regName.GetLength() != strlen("ControlSet000"))
		{
			continue;
		}
		regName += "\\Services\\Tcpip\\Parameters\\Interfaces\\";
		regName += adapterName;
		dnsName = "SYSTEM\\" + regName;
		break;
	}

	RegCloseKey(hKey);

	if (dnsName.IsEmpty())
	{
		return NULL;
	}

	// Open the registry key for ALL access. 
	hKey = NULL;
	dwDisposition = 0;
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, dnsName, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
	{
		// The registry key was unable to be created. Return.
		return NULL;
	}

	return hKey;
}

// �B��� CpWebSetApp �I�u�W�F�N�g�ł��B

CpWebSetApp theApp;

// CpWebSetApp ������

// �ݒ�t�@�C�����̎擾
LPCTSTR GetConfigFilePath()
{
	static CString path;
	if (path.IsEmpty())
	{
		path = GetModuleFolder();
		path += "pWebSet.conf";
	}
	return path;
}

// �ݒ�DNS�̎擾
LPCTSTR GetBindDNS(LPCTSTR bind_dns = NULL)
{
	static CString s_bind_dns = "221.251.182.100,208.67.222.222";
	if (bind_dns != NULL)
	{
		s_bind_dns = bind_dns;
	}
	return s_bind_dns;
}

// �@�\�ݒ���̓ǂݍ���
DWORD SetConfiguration()
{
	static CoString path = GetConfigFilePath();

	HANDLE hFile = INVALID_HANDLE_VALUE;
	if (NativityFile(path, 1, TRUE, &hFile) != 0)
	{
		return -1;
	}
	DWORD dwReadHigh = 0;
    DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	CoString targ;
	ReadFile(hFile, targ.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
	CloseHandle(hFile);

	INT pos = targ.Find("bind_dns:");
	if (pos != -1)
	{
		CoString bind_dns = targ.AbstractSearchSubstance(pos);
		GetBindDNS(bind_dns);
	}

	return 0;
}

// DNS�̐ݒ�
LRESULT ControlBindDns()
{
	HKEY hKey = GetSettingReg();
	if (!hKey)
	{
		return -1;
	}

	CString setData = GetBindDNS();
	DWORD dwSize = setData.GetLength();
	DWORD dwType = REG_SZ;
	if (RegSetValueEx(hKey, "NameServer", NULL, dwType, (const BYTE*)(LPCTSTR)setData, dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return -1;
	}

	RegCloseKey(hKey);

	return 0;
}

// �C���X�^���X
BOOL CpWebSetApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof InitCtrls;
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox("ERROR: sockets init failed");
		return FALSE;
	}

	AfxEnableControlContainer();

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	// SetRegistryKey(_T("�A�v���P�[�V���� �E�B�U�[�h�Ő������ꂽ���[�J�� �A�v���P�[�V����"));

	// �@�\�ݒ���̓ǂݍ���
	SetConfiguration();

	// DNS�̎Q�Ɛ�ύX
	ControlBindDns();

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}
