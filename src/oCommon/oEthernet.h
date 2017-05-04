///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__P_ETHERNET_H__)
#define __P_ETHERNET_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// ネットワーク処理関数

INT GetHostInfo(CString& hostname, CString& ipaddr, CString& mask, CString& username, CString& macaddr, CString& adapterName, BOOL isUpdate = FALSE);


#endif // __P_ETHERNET_H__
