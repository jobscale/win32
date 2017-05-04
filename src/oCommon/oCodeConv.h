///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2011 Plustar Corporation. All rights reserved. プラスター
//

#ifndef _O_CODE_CONV_H_
#define _O_CODE_CONV_H_

INT SJIStoUTF8(const LPCSTR sjis, CComBSTR& utf8);
INT UTF8toSJIS(const LPCSTR utf8, CComBSTR& sjis);
INT SJIStoEUC(const LPCSTR sjis, CComBSTR& euc);

#endif
