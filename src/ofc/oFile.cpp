///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
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

#define _OFC_EXPORT_
#include "oFile.h"

CoAutoFile::CoAutoFile(LPCTSTR filePath/* = NULL*/, DWORD dwShareMode/* = GENERIC_READ*/)
	: m_hFile(INVALID_HANDLE_VALUE)
{
	if (filePath)
	{
		Open(filePath, dwShareMode);
	}
}

CoAutoFile::~CoAutoFile()
{
	Close();
}

HANDLE CoAutoFile::Open(LPCTSTR filePath, DWORD dwShareMode/* = GENERIC_READ*/)
{
	DWORD dwCreationDisposition = dwShareMode == GENERIC_READ ? OPEN_EXISTING : OPEN_ALWAYS;
	m_filePath = filePath;
	m_hFile = CreateFile(m_filePath, dwShareMode, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	return m_hFile;
}

BOOL CoAutoFile::IsOpen()
{
	return m_hFile != INVALID_HANDLE_VALUE;
}

BOOL CoAutoFile::Close()
{
	if (!IsOpen())
	{
		return FALSE;
	}
	BOOL result = CloseHandle(m_hFile);
	if (result)
	{
		m_hFile = INVALID_HANDLE_VALUE;
	}
	return result;
}

HANDLE CoAutoFile::FromHandle(const HANDLE hFile)
{
	return m_hFile = hFile;
}

DWORD CoAutoFile::GetSize()
{
	if (!IsOpen())
	{
		return 0;
	}
	DWORD dwReadHigh = 0;
	DWORD dwRead = GetFileSize(m_hFile, &dwReadHigh);
	return dwRead;
}

BOOL CoAutoFile::SeekEnd()
{
	LARGE_INTEGER num = { 0 };
	return SetFilePointerEx(m_hFile, num, NULL, FILE_END);
}

BOOL CoAutoFile::Read(CoString& value, DWORD dwRead)
{
	if (!IsOpen())
	{
		return FALSE;
	}
	return ReadFile(m_hFile, value.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
}

BOOL CoAutoFile::Read(CoString& value)
{
	if (!IsOpen())
	{
		return FALSE;
	}
	DWORD dwRead = GetSize();
	return Read(value, dwRead);
}

BOOL CoAutoFile::Write(LPCTSTR value)
{
	if (!IsOpen())
	{
		return FALSE;
	}
	DWORD dwSize = 0;
	BOOL result = WriteFile(m_hFile, value, (DWORD)strlen(value), &dwSize, NULL);
	SetEndOfFile(m_hFile);
	return result;
}

BOOL CoAutoFile::Write(const CString& value)
{
	if (!IsOpen())
	{
		return FALSE;
	}
	DWORD dwSize = 0;
	BOOL result = WriteFile(m_hFile, value, value.GetLength(), &dwSize, NULL);
	SetEndOfFile(m_hFile);
	return result;
}
