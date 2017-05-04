///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__OFILE_H__)
#define __OFILE_H__

#include "oString.h"

#undef OFCL_API
#ifdef _OFC_EXPORT_
#define OFCL_API __declspec(dllexport)
#else
#define OFCL_API __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// 

class OFCL_API CoAutoFile
{
protected:
	CoString m_filePath;
	HANDLE m_hFile;
public:
	CoAutoFile(LPCTSTR filePath = NULL, DWORD dwShareMode = GENERIC_READ);
	virtual ~CoAutoFile();
	HANDLE Open(LPCTSTR filePath, DWORD dwShareMode = GENERIC_READ);
	BOOL IsOpen();
	BOOL Close();
	HANDLE FromHandle(const HANDLE hFile);
	DWORD GetSize();
	BOOL SeekEnd();
	BOOL Read(CoString& value, DWORD dwRead);
	BOOL Read(CoString& value);
	BOOL Write(LPCTSTR value);
	BOOL Write(const CString& value);
};

#endif // __OFILE_H__
