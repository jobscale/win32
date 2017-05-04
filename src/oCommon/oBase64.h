///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#ifndef __BASE64_H__
#define __BASE64_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Base64処理関数

int base64_encode(CString& data, CString& ret, int tbl = 1);
int base64_decode(CString& data, CString& ret, int tbl = 1);
int base64_encode(LPCTSTR data, CString& ret, int tbl = 1);
int base64_decode(LPCTSTR data, CString& ret, int tbl = 1);

#endif // __BASE64_H__
