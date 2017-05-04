///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
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

#include "../oCommon/oTools.h"

#include <iphlpapi.h>
#pragma comment (lib, "iphlpapi")
#include <ws2tcpip.h>

#pragma warning(disable: 4996)

///////////////////////////////////////////////////////////////////////////////////////////////////
// �}�V�����擾�֐�


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

const char *getString_Lifetime(char *szBuf, size_t size, ULONG lt)
{
    if (lt == ULONG_MAX) {
        if (_snprintf(szBuf, size, "%s", "infinite") < 0)
            return NULL;
    } else {
        if (_snprintf(szBuf, size, "%u", lt) < 0)
            return NULL;
    }

    return szBuf;
}

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

char *getString_AdapterFlags(char *szBuf, size_t size, DWORD dwFlags)
{
    char *p = szBuf;
    int len;
    struct _FlagList {
        DWORD Flag;
        const char* szFlagName;
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
            if ((len = _snprintf(p, size, "%s", FlagList[i].szFlagName)) < 0)
                return NULL;
            p+= len;
            size -= len;

            break;
        }
    }

    for (i++; i < count; ++i) {
        if (dwFlags & FlagList[i].Flag) {
            if ((len = _snprintf(p, size, ",%s", FlagList[i].szFlagName)) < 0)
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
        len = WideCharToMultiByte(
							CP_ACP, 0,
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

	return 0;
}

INT GetHostInfoCore(CString& hostname, CString& ipaddr, CString& ipmask, CString& username, CString& macaddr, CString& adapterName)
{
	if (gethostname(hostname.GetBuffer(_MAX_PATH),  _MAX_PATH) != NO_ERROR)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	hostname.ReleaseBuffer();
	LPHOSTENT pEnt = gethostbyname(hostname);
	if (!pEnt)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	HOSTENT ent = *pEnt;

	hostname = ent.h_name;
	ipaddr.Format("\0 %d.%d.%d.%d" + 2, (BYTE)ent.h_addr_list[0][0], (BYTE)ent.h_addr_list[0][1], (BYTE)ent.h_addr_list[0][2], (BYTE)ent.h_addr_list[0][3]);

	DWORD dwSize = UNLEN + 1;
	GetUserName(username.GetBuffer(dwSize), &dwSize);
	username.ReleaseBuffer();

	GetIpMask(ipaddr, ipmask);

	GetMac(ipaddr, macaddr, adapterName);

	return 0;
}

INT GetHostInfo(CString& hostname, CString& ipaddr, CString& ipmask, CString& username, CString& macaddr, CString& adapterName, BOOL isUpdate/* = FALSE*/)
{
	static CString s_hostname;
	static CString s_ipaddr;
	static CString s_ipmask;
	static CString s_username;
	static CString s_macaddr;
	static CString s_adapterName;

	if (isUpdate || s_hostname.IsEmpty() || s_ipaddr.IsEmpty() || s_ipmask.IsEmpty() || s_username.IsEmpty() || s_macaddr.IsEmpty() || s_adapterName.IsEmpty())
	{
		GetHostInfoCore(s_hostname, s_ipaddr, s_ipmask, s_username, s_macaddr, s_adapterName);
	}

	hostname = s_hostname;
	ipaddr = s_ipaddr;
	ipmask = s_ipmask;
	username = s_username;
	macaddr = s_macaddr;
	adapterName = s_adapterName;

	return 0;
}
