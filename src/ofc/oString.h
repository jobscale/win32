///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__OSTRING_H__)
#define __OSTRING_H__

#pragma warning(disable : 4996)

#include "oArray.h"
#include <atlbase.h>
#include <atlstr.h>

#ifndef P
#define P(x) ("\0 " x + 2)
#define PP(x) ("\0 " #x + 2)
#endif

#undef OFCL_API
#ifdef _OFC_EXPORT_
#define OFCL_API __declspec(dllexport)
#else
#define OFCL_API __declspec(dllimport)
#endif

class OFCL_API CoString : public CString
{
public:
	CoString();
	CoString(const CoString& other);
	CoString(const CString& other);
	CoString(LPCTSTR other);
	CoString(const CComBSTR& other);
	CoString(LPCWSTR other);

public:
	const CoString& operator=(const CoString& other);
	const CoString& operator=(const CString& other);
	const CoString& operator=(LPCTSTR other);
	const CoString& operator=(const CComBSTR& other);
	const CoString& operator=(LPCWSTR other);

protected:
	static CoString Fix(CoString targetText);
	static CoString FirstCut(CoString targetText, DWORD cutLength);

public:
	CoString Mid(INT nFirst, INT nCount) const;
	CoString Left(INT nCount) const;
	CoString Right(INT nCount) const;
	INT Replace(INT nFirst, INT nCount, LPCTSTR lpszNew);
	INT Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);
	INT Find(TCHAR ch) const;
	INT Find(LPCTSTR lpszSub, INT nStart = 0, CoString limit = "") const;
	INT FindPlus(LPCTSTR lpszSub, LPCTSTR lpszSubPlus, INT nStart = 0, CoString limit = "") const;
	INT FindWord(LPCTSTR lpszSub, INT nStart = 0, CoString limit = "") const;
	INT FindOneOf(LPCTSTR lpszCharSet, INT nStart = 0) const;
	INT FindOneOfNot(LPCTSTR lpszCharSet, INT nStart = 0) const;
	INT ReverseFind(TCHAR ch) const;
	INT ReverseFind(LPCTSTR lpszSub, INT nStart = -1) const;
	INT FindNoCase(LPCTSTR lpszSub, INT nStart = 0) const;
	INT AbstractReversePosition(INT nStart = -1) const;
	CoString AbstractSearchSubstance(INT& start, const CoString ce = "\"", CoString limit = "", BOOL bCSV = FALSE) const;

};

/////////////////////////////////////////////////////////////////////////////
// CoStringArray オブジェクト

class CoStringArray : public CoArray<CoString>
{
public:
	int Add(LPCTSTR element);
	int Find(LPCTSTR element) const;

};

BOOL OFCL_API IsFindArray(const CoString& text, const CoStringArray& ignore_list, const BOOL ignoreNoCase = TRUE);
BOOL OFCL_API IsFindArrayAsArray(const CoStringArray& target, const CoStringArray& source, const INT flag, const BOOL ignoreNoCase = TRUE);
BOOL OFCL_API GetArrayValue(CoStringArray& value, const CoString text, LPCTSTR separator, BOOL isFast = FALSE);
BOOL OFCL_API GetStringValue(CoString& text, const CoStringArray& value, LPCTSTR separator);


#endif // !defined(__OSTRING_H__)
