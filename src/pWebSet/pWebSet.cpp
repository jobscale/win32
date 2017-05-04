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

// CpWebSetApp コンストラクション

CpWebSetApp::CpWebSetApp()
{
}

#include <iphlpapi.h>
#pragma comment (lib, "iphlpapi")
//#include <ws2tcpip.h>

/* アダプタのタイプ（メディア種別）を示す */
/* 値（dwIfType）を文字列に変換する       */
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

/* アダプタの状態を示す                       */
/* IF_OPER_STATUS型の列挙子を文字列に変換する */
const char *getString_IF_OPER_STATUS(IF_OPER_STATUS ios)
{
    const char *szOperationalStatus[] = {
        "",                /* IF_OPER_STATUS列挙子の定義では0は欠番 */
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

/* 寿命を表すULONG値（lt）を文字列に変換しszBufへ書き込む。 */
/* 寿命値ltがULONG_MAXに等しい場合は"無限を表す文字列       */
/* "infinite"を返却する。szBufに十分な大きさ（size）が確保  */
/* されていない場合はNULLを返却する。                       */
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

/* IPアドレスのプレフィックス(ネットワーク）部の付与元 */
/* を表すIP_PREFIX_ORIGIN型の列挙子を文字列に変換する  */
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

/* IPアドレスのインターフェイスID(ホスト部）の付与元 */
/* を表すIP_SUFFIX_ORIGIN型の列挙子を文字列に変換する*/
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

/* IPv6アドレスのDAD状態を示す              */
/* IP_DAD_STATE型の列挙子を文字列に変換する */
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

/* アダプタの属性を示すフラグ情報を文字列に変換する。フラグの名前は    */
/* カンマ(,)で区切って出力ｓれる                                       */
/* szBufに十分な大きさ（size）が確保されていない場合はNULLを返却する。 */
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
     * WinSockの初期化
     */
    WSADATA wsaData = { 0 };
    if (WSAStartup(WINSOCK_VERSION, &wsaData)) {
        fprintf(stderr, "cannot initilize WinSock\n");
        exit(1);
    }

    /*
     * 戻り値を格納するのに必要なバッファの大きさを求める
     * dwSizeにその値が得られる。FirstPrefixメンバにプリフィックス
     * 情報を得るため。第2引数のフラグにGAA_FLAG_INCLUDE_PREFIXを立てる
     */
    DWORD dwSize = 0;
    DWORD dwRet = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &dwSize);
    if (dwRet != ERROR_BUFFER_OVERFLOW) {
        fprintf(stderr, "no enough buffer\n");
        exit(1);
    }

    /*
     * pAdapterAddressesにdwSizeの大きさのバッファを確保する
     */
    PIP_ADAPTER_ADDRESSES pAdapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(dwSize);
    if (pAdapterAddresses == NULL) {
        fprintf(stderr, "no enough buffer\n");
        exit(1);
    }

    /*
     * GetAdaptersAddresses()を再度呼び出しpAdapterAddressesに
     * アダプタ情報を取得する。FirstPrefixメンバにプリフィックス
     * 情報を得るため。第2引数のフラグにGAA_FLAG_INCLUDE_PREFIXを立てる
     */
    dwRet = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX,
                                 NULL, pAdapterAddresses, &dwSize);
    if (dwRet != ERROR_SUCCESS) {
        fprintf(stderr, "GetAdaptersAddresses() failed\n");
        exit(1);
    }

    /*
     * すべてのアダプタについて、それぞれがもつ情報を表示する
     * pAAが現在注目するアダプタのIP_ADAPTER_ADDRESSES構造体の
     * インスタンス
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
         * FriendlyNameメンバにより可読なアダプタ名を表示する
         */
        /* FriendlyNameはユニコードのためマルチバイト */
        /* 文字（Shift JIS）に変換する                */
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
         * AdapterNameメンバによりアダプタ名（UID形式）を表示する
         */
        TRACE("  Adapter UID = %s\n", pAA->AdapterName);

        /*
         * Descriptionメンバからアダプタの解説を表示する
         */
        /* Descriptionはユニコードのためマルチバイト  */
        /* 文字（Shift JIS）に変換する                */
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
         * IfTypeメンバによりアダプタのメディアタイプを表示する
         */
        TRACE("  Type : %s\n", getString_IfType(pAA->IfType));

        /*
         * OperStatusメンバによりアダプタの状態を表示する
         */
        TRACE("  Operational Status : %s\n",
               getString_IF_OPER_STATUS(pAA->OperStatus));

        /*
         * FirstUnicastAddressメンバによりアダプタに
         * 付与されたユニキャストアドレスを表示する
         */
        for (i = 0, pUnicastAddress = pAA->FirstUnicastAddress;
             pUnicastAddress;
             i++, pUnicastAddress = pUnicastAddress->Next) {
            char szAddress[NI_MAXHOST];
            char szValidLifetime[BUFSIZ];
            char szPreferredLifetime[BUFSIZ];
            char szLeaseLifetime[BUFSIZ];

            /*
             * ユニキャストアドレスの表示処理
             */
            /* ネットワークフォーマット（バイナリ）で格納されるアドレスを */
            /* 可読な文字列（presentation format）に変換する              */
            if (getnameinfo(pUnicastAddress->Address.lpSockaddr,
                            pUnicastAddress->Address.iSockaddrLength,
                            szAddress, sizeof szAddress, NULL, 0,
                            NI_NUMERICHOST)) {
                fprintf(stderr,
                        "can't convert network format to presentation format");
                exit(1);
            }
            /* ユニキャストアドレスの文字列を表示する */
            TRACE("  Unicast Address#%d\n", i);
            TRACE("    Family=%s, Addr=%s\n",
                   ((pUnicastAddress->Address.lpSockaddr)->sa_family
                       == AF_INET) ? "IPv4" : "IPv6",
                   szAddress);
			ipAddress = szAddress;

            /* 寿命を文字列に変換し表示する */
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
             * IPアドレスの取得元（PREFIX, SUFFIX）を表示する
             */
            TRACE("    prefix origin : %s\n",
                   getString_IP_PREFIX_ORIGIN(pUnicastAddress->PrefixOrigin));
            TRACE("    suffix origin : %s\n",
                   getString_IP_SUFFIX_ORIGIN(pUnicastAddress->SuffixOrigin));

            /*
             * IPv6アドレスの場合DAD状態を表示する
             */
            if ((pUnicastAddress->Address.lpSockaddr)->sa_family == AF_INET6) {
                TRACE("    DAD state : %s\n",
                       getString_IP_DAD_STATE(pUnicastAddress->DadState));
            }
        }

        /*
         * FirstAnycastAddressメンバによりエニーキャストアドレスを表示する
         */
        for (i = 0, pAnycastAddress = pAA->FirstAnycastAddress;
             pAnycastAddress;
             i++, pAnycastAddress = pAnycastAddress->Next) {
            char szAddress[NI_MAXHOST];

            /*
             * エニーキャストアドレスの表示処理
             */
            /* ネットワークフォーマット（バイナリ）で格納されるアドレスを */
            /* 可読な文字列（presentation format）に変換する              */
            if (getnameinfo(pAnycastAddress->Address.lpSockaddr,
                            pAnycastAddress->Address.iSockaddrLength,
                            szAddress, sizeof szAddress, NULL, 0,
                            NI_NUMERICHOST)) {
                fprintf(stderr,
                        "can't convert network format to presentation format");
                exit(1);
            }
            /* エニーキャストアドレスの文字列を表示する */
            TRACE("  Anycast Address#%d\n", i);
            TRACE("    Family=%s, Addr=%s\n",
                   ((pAnycastAddress->Address.lpSockaddr)->sa_family
                                                 == AF_INET) ? "IPv4" : "IPv6",
                   szAddress);
        }

        /*
         * FirstMulticastAddressメンバによりマルチキャストアドレスを表示する
         */
        for (i = 0, pMulticastAddress = pAA->FirstMulticastAddress;
             pMulticastAddress;
             i++, pMulticastAddress = pMulticastAddress->Next) {
            char szAddress[NI_MAXHOST];

            /*
             * マルチキャストアドレスの表示処理
             */
            /* ネットワークフォーマット（バイナリ）で格納されるアドレスを */
            /* 可読な文字列（presentation format）に変換する              */
            if (getnameinfo(pMulticastAddress->Address.lpSockaddr,
                            pMulticastAddress->Address.iSockaddrLength,
                            szAddress, sizeof szAddress, NULL, 0,
                            NI_NUMERICHOST)) {
                fprintf(stderr,
                        "can't convert network format to presentation format");
                exit(1);
            }
            /* マルチキャストアドレスの文字列を表示する */
            TRACE("  Multicast Address#%d\n", i);
            TRACE("    Family=%s, Addr=%s\n",
                   ((pMulticastAddress->Address.lpSockaddr)->sa_family
                                                == AF_INET) ? "IPv4" : "IPv6",
                   szAddress);
        }
        
        /*
         * FirstDnsServerAddressメンバによりDNSサーバのアドレスを表示する
         */
        for (i = 0, pDnsServerAddress = pAA->FirstDnsServerAddress;
             pDnsServerAddress;
             i++, pDnsServerAddress = pDnsServerAddress->Next) {
            char szAddress[NI_MAXHOST];

            /*
             * DNSサーバのアドレスの表示処理
             */
            /* ネットワークフォーマット（バイナリ）で格納されるアドレスを */
            /* 可読な文字列（presentation format）に変換する              */
            if (getnameinfo(pDnsServerAddress->Address.lpSockaddr,
                            pDnsServerAddress->Address.iSockaddrLength,
                            szAddress, sizeof szAddress, NULL, 0,
                            NI_NUMERICHOST)) {
                fprintf(stderr,
                        "can't convert network format to presentation format");
                exit(1);
            }
            /* DNSサーバアドレスの文字列を表示する */
            TRACE("  DNS Server Address#%d\n", i);
            TRACE("    Family=%s, Addr=%s\n",
                   ((pDnsServerAddress->Address.lpSockaddr)->sa_family
                                                == AF_INET) ? "IPv4" : "IPv6",
                   szAddress);
        }

        /*
         * DnsSuffixメンバによりDNSサフィックス（ドメイン名）を表示する
         */
        /* DnsSuffixはユニコードのためマルチバイト */
        /* 文字（Shift JIS）に変換する             */
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
         * PhysicalAddress, PhysicalAddressLengthメンバにより
         * アダプタの物理アドレス（MACアドレス）を表示する
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
         * MtuメンバによりアダプタのMTU値を表示する
         */
        TRACE("  MTU : %d\n", pAA->Mtu);

        /*
         * Ipv6IfIndexメンバによりインターフェイスインデックスを表示する
         *   fe80::1%6 --- %に後ろに記述されるアダプタを示す番号と一致
         */
        TRACE("  IPv6 Interface Index : %d\n", pAA->Ipv6IfIndex);

        /*
         * Flagsメンバによりアダプタの属性を表示する
         */
        if (getString_AdapterFlags(szFlagsString, sizeof szFlagsString,
                                   pAA->Flags) == NULL) {
            fprintf(stderr, "cannot convert to string\n");
            exit(1);
        }
        TRACE("  Flags: %s\n", szFlagsString);

        /*
         * ZoneIndicesメンバによりゾーン情報を表示する
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
         * FirstPrefixメンバによりアダプタの持つプレフィックスを表示する
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
     * アダプタ情報を格納していた領域を開放する
     */
    free(pAdapterAddresses);

    /*
     * WinSockのクリーンナップ
     */
    WSACleanup();

	return 0;
}

DWORD GetIpMask(const CString& ipaddr, CString& ipmask)
{
	//第１パラメータにNULLをおいて、必要サイズを DWORD d に取得する
	DWORD size = 0;
	GetIpAddrTable(NULL, &size, FALSE);

	//MIB_IPADDRTABLEへのポインタに、必要サイズを確保する
	PMIB_IPADDRTABLE IpAddrTable = (PMIB_IPADDRTABLE)new char[size];
	if (IpAddrTable)
	{
		//アドレステーブルの取得
		if (GetIpAddrTable((PMIB_IPADDRTABLE)IpAddrTable, &size, FALSE) == NO_ERROR)
		{
			//取得したテーブル数は、dwNumEntriesで分かる
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
		//MIB_IPADDRTABLEへのポインタに、確保したメモリを解放する
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

// 唯一の CpWebSetApp オブジェクトです。

CpWebSetApp theApp;

// CpWebSetApp 初期化

// 設定ファイル名の取得
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

// 設定DNSの取得
LPCTSTR GetBindDNS(LPCTSTR bind_dns = NULL)
{
	static CString s_bind_dns = "221.251.182.100,208.67.222.222";
	if (bind_dns != NULL)
	{
		s_bind_dns = bind_dns;
	}
	return s_bind_dns;
}

// 機能設定情報の読み込み
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

// DNSの設定
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

// インスタンス
BOOL CpWebSetApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof InitCtrls;
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox("ERROR: sockets init failed");
		return FALSE;
	}

	AfxEnableControlContainer();

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	// SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	// 機能設定情報の読み込み
	SetConfiguration();

	// DNSの参照先変更
	ControlBindDns();

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}
