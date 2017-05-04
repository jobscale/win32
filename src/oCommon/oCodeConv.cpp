///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2011 Plustar Corporation. All rights reserved. プラスター
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#define _IS_SJIS_1(c) ((UINT)((BYTE)(c) ^ 0x20) - 0xa1 < 0x3c)
#define _IS_SJIS_2(c) (0x40 <= (BYTE)(c) && (BYTE)(c) <= 0xfc && (BYTE)(c) != 0x7f)

static WORD _mbcjmstojisex(const BYTE *pszSrc)
{
    WORD w;
    if (_IS_SJIS_1(*pszSrc) && _IS_SJIS_2(pszSrc[1]))
    {
        w = MAKEWORD(pszSrc[1], *pszSrc);
        if (*pszSrc == 0x0fa || *pszSrc == 0x0fb || 
            (*pszSrc == 0x0fc && pszSrc[1] <= 0x04b))
        {
            if      (w <= 0xfa49) w -= 0x0b51;
            else if (w <= 0xfa53) w -= 0x72f6;
            else if (w <= 0xfa57) w -= 0x0b5b;
            else if (w == 0xfa58) w  = 0x878a;
            else if (w == 0xfa59) w  = 0x8782;
            else if (w == 0xfa5a) w  = 0x8784;
            else if (w == 0xfa5b) w  = 0x879a;
            else if (w <= 0xfa7e) w -= 0x0d1c;
            else if (w <= 0xfa9b) w -= 0x0d1d;
            else if (w <= 0xfafc) w -= 0x0d1c;
            else if (w <= 0xfb5b) w -= 0x0d5f;
            else if (w <= 0xfb7e) w -= 0x0d1c;
            else if (w <= 0xfb9b) w -= 0x0d1d;
            else if (w <= 0xfbfc) w -= 0x0d1c;
            else                  w -= 0x0d5f;
        }
        return (WORD)_mbcjmstojis(w);
    }
    return 0;
}


INT SJIS2EUCSize(LPCSTR pszSrc, INT cchSrc)
{
    INT i, cchDest;
    WORD  sCode;

    cchDest = 0;
    for(i = 0; i < cchSrc; i++)
    {
        if (_IS_SJIS_1(pszSrc[i]) && _IS_SJIS_2(pszSrc[i + 1]))
        {
            sCode = (WORD)_mbcjmstojisex((const BYTE*)pszSrc + i);
            if (sCode != 0)
            {
                cchDest += 2;
                i++;
            }
            else
            {
                cchDest += 2;
                i++;
            }
        }
        else
        {
            if ((BYTE)pszSrc[i] >= (BYTE)0x80)
                cchDest += 2;
            else
                cchDest++;
        }
    }
    return cchDest;
}

INT SJIS2EUC(LPSTR pszDest, LPCSTR pszSrc, INT cchSrc)
{
    INT i, cchDest;
    WORD  sCode;

    cchDest = 0;
    for(i = 0; i < cchSrc; i++)
    {
        if (_IS_SJIS_1(pszSrc[i]) && _IS_SJIS_2(pszSrc[i + 1]))
        {
            sCode = (WORD)_mbcjmstojisex((const BYTE*)pszSrc + i);
            if (sCode != 0)
            {
                pszDest[cchDest]     = (BYTE)0x80 | (BYTE)(sCode >> 8);
                pszDest[cchDest + 1] = (BYTE)0x80 | (BYTE)sCode;
                cchDest += 2;
                i++;
            }
            else
            {
                pszDest[cchDest]     = pszSrc[i];
                pszDest[cchDest + 1] = pszSrc[i + 1];
                cchDest += 2;
                i++;
            }
        }
        else
        {
            if ((BYTE)pszSrc[i] >= (BYTE)0x80)
            {
                pszDest[cchDest]     = (BYTE)0x8e;
                pszDest[cchDest + 1] = pszSrc[i];
                cchDest += 2;
            }
            else
            {
                pszDest[cchDest] = pszSrc[i];
                cchDest++;
            }
        }
    }
    pszDest[cchDest] = '\0';
    return cchDest;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 文字列処理関数

INT SJIStoUTF8(const LPCSTR sjis, CComBSTR& utf8)
{
	CComBSTR bstr = sjis;
	INT size = WideCharToMultiByte(CP_UTF8, 0, bstr, -1, NULL, 0, NULL, NULL);
	utf8.AppendBytes("", size);
	WideCharToMultiByte(CP_UTF8, 0, bstr, -1, (LPTSTR)utf8.m_str, size, NULL, NULL);

	return 0;
}

INT UTF8toSJIS(const LPCSTR utf8, CComBSTR& sjis)
{
	INT size = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	CComBSTR bstr(size);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, bstr, size);
	sjis.AppendBytes("", size * 2 + 1);
	USES_CONVERSION; memcpy((LPSTR)sjis.m_str, W2A(bstr), size * 2 + 1);

	return 0;
}

INT SJIStoEUC(const LPCSTR sjis, CComBSTR& euc)
{
	int len = SJIS2EUCSize(sjis, strlen(sjis));
	euc.AppendBytes("", len);
	SJIS2EUC((LPSTR)euc.m_str, sjis, len);

	return 0;
}
