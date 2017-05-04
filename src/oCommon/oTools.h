///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#ifndef __O_TOOLS_H__
#define __O_TOOLS_H__

#include <atlstr.h>

#ifndef P
#define P(x) ("\0 " x + 2)
#endif

#ifndef PP
#define PP(x) ("\0 " #x + 2)
#endif

BOOL IsWindows98SE();
BOOL IsWindowsME();
BOOL IsWindows2000();
BOOL IsWindowsXP();
BOOL IsWindows2003();
BOOL IsWindowsVista();

#define DEBUG_OUT_DEFAULT(x) OutputViewer("%s %s(%d): [0x%08X] %s", NowTime(), RightEight("\0 " __FILE__ + 2), __LINE__, GetLastError(), x)
#define KB *(1024)
#define MB *(1024*1024)
#define GB *(1024*1024*1024)
#define TB *(1024*1024*1024*1024)

// ファイルの存在が確認されるまで待機する
DWORD PregnancyFile(LPCTSTR filePath, DWORD tenacity);
// ファイルの保存が完了されるまで待機する
INT NativityFile(LPCTSTR filePath, INT tenacity, BOOL bExisting = TRUE, LPHANDLE phFile = NULL);

const LPCTSTR GetModuleFolder(DWORD flg = 0);
const LPCTSTR GetApplicationDataPath();
void GetFormatMessage(const DWORD error, CString& msg);
const BOOL IsSuperDebug(BOOL bDebug = FALSE);
void DebugStopper();
void OutputViewer(LPCTSTR lpszFormat, ...);
void OutputViewerEx(DWORD sw, LPCTSTR lpszFormat, ...);
const LPCTSTR NowTime();
const LPCTSTR RightEight(LPCTSTR value);
// プロセスの実行
LRESULT RunProcess(LPCTSTR runProcess, LPCTSTR param = NULL, DWORD wait = INFINITE);
// 権限
BOOL EnablePrivilege(LPTSTR lpszPrivilege);

// 文字列操作
CString oAbstractSearchSubstance(const CString& targ, INT& start, const CString ce = "\"", CString limit = "", BOOL bCSV = FALSE);

/////////////////////////////////////////////////////////////////////////////
// 共通オブジェクト

// 文字列クラス

class CText
{
private:
	char* _str;
	int _len;
public:
	CText()
		: _str(NULL)
		, _len(0)
	{
	}
	CText(const char* str, int len = -1)
	{
		if (len == -1)
		{
			_len = strlen(str);
		}
		else
		{
			_len = len;
		}
		_str = (char*)malloc(_len + 1);
		if (!_str)
		{
			_len = 0;
			return;
		}
		memcpy(_str, str, _len);
		_str[_len] = 0;
	}
	virtual ~CText()
	{
		Release();
	}
	void Release()
	{
		if (_str)
		{
			free(_str);
			_len = 0;
		}
	}
	char* StrCpy(const char* str, int len = -1)
	{
		Release();

		if (len == -1)
		{
			_len = strlen(str);
		}
		else
		{
			_len = len;
		}
		_str = (char*)malloc(_len + 1);
		if (!_str)
		{
			_len = 0;
			return _str;
		}
		memcpy(_str, str, _len);
		_str[_len] = 0;

		return _str;
	}
	char* operator=(const char* str)
	{
		Release();

		_len = strlen(str);
		_str = (char*)malloc(_len + 1);
		if (!_str)
		{
			_len = 0;
			return _str;
		}
		memcpy(_str, str, _len);
		_str[_len] = 0;

		return _str;
	}
	char* ToLower()
	{
		for (char* p = _str; *p; p++) *p = tolower(*p);
		return _str;
	}
	static char* ToLower(char* str)
	{
		for (char* p = str; *p; p++) *p = tolower(*p);
		return str;
	}
	char* ToUpper()
	{
		for (char* p = _str; *p; p++) *p = toupper(*p);
		return _str;
	}
	static char* ToUpper(char* str)
	{
		for (char* p = str; *p; p++) *p = toupper(*p);
		return str;
	}
	operator char*() const
	{
		return _str;
	}
	int GetLength()
	{
		return _len;
	}
};

class CMemStr
{
private:
	LPCTSTR _str;
	INT _length;
	INT _buflen;
	CComBSTR _buffer;
public:
	CMemStr() : _buflen(0) { GetBufferSetLength(0); }
	virtual ~CMemStr() {}
	CMemStr(LPCTSTR pszStr, int nLength = -1) : _buflen(0) {
		if (nLength == -1) nLength = strlen(pszStr);
		GetBufferSetLength(nLength);
		memcpy(operator LPTSTR(), pszStr, nLength);
		ReleaseBuffer(_length);
	}
	operator LPCTSTR() const { return (LPCTSTR)_buffer.m_str; }
	operator LPTSTR() { return (LPTSTR)_buffer.m_str; }
	CMemStr& operator=(LPCTSTR pszStr) {
		if (!pszStr) return *this;
		_length = strlen(pszStr);
		GetBuffer(_length);
		memcpy(operator LPTSTR(), pszStr, _length);
		ReleaseBuffer(_length);
		return *this;
	}
	CMemStr& operator+=(LPCTSTR pszStr) {
		if (!pszStr) return *this;
		int extLen = GetLength();
		int nLength = strlen(pszStr);
		GetBuffer(nLength);
		_length += nLength;
		memcpy(operator LPTSTR() + extLen, pszStr, nLength);
		ReleaseBuffer(_length);
		return *this;
	}
	CMemStr& operator=(const CMemStr& src) {
		Empty();
		operator+=(src);
		return *this;
	}
	CMemStr& operator+=(const CMemStr& src) {
		INT length = _length;
		INT nLength = src.GetLength();
		GetBufferSetLength(length + nLength);
		memcpy(operator LPTSTR() + length, (LPCTSTR)src._buffer.m_str, nLength);
		ReleaseBuffer(_length);
		return *this;
	}
	int GetLength() const { return _length; }
	void Empty() {
		_length = 0;
		ReleaseBuffer(_length);
	}
	int IsEmpty() { return !GetLength(); }
	LPTSTR GetBufferSetLength(int nLength) { _length = nLength; return GetBuffer(nLength); }
	LPTSTR GetBuffer(int nLength) {
		INT addByte = nLength + 2;
		if (addByte > _buflen) {
			INT nByte = (addByte - _buflen) + 1027;
			_buflen += nByte;
			AppendBuffer(nByte);
			_str = (LPCTSTR)_buffer.m_str;
			operator LPTSTR()[_length] = 0;
			operator LPTSTR()[_length + 1] = 0;
		}
		return operator LPTSTR();
	}
	HRESULT AppendBuffer(__in int nLen) throw()
	{
		if (nLen == 0)
			return S_OK;
		int n1 = _buffer.ByteLength();
		if (n1+nLen < n1)
			return E_OUTOFMEMORY;
		BSTR b;
		b = ::SysAllocStringByteLen(NULL, n1+nLen);
		if (b == NULL)
        {
			return E_OUTOFMEMORY;
        }
		Checked::memcpy_s(b, n1+nLen, _buffer.m_str, n1);
		*((OLECHAR*)(((char*)b)+n1+nLen)) = NULL;
		SysFreeString(_buffer.m_str);
		_buffer.m_str = b;
		return S_OK;
	}
	void ReleaseBufferSetLength(INT nNewLength) { ReleaseBuffer(nNewLength); }
	void ReleaseBuffer(INT nNewLength = -1) {
		if (nNewLength < 0) nNewLength = strlen(operator LPCTSTR());
		_length = nNewLength;
		operator LPTSTR()[_length] = 0;
		operator LPTSTR()[_length + 1] = 0;
	}
	int Find(LPCTSTR pszSub, int iStart = 0) {
		LPCTSTR pFind = strstr(operator LPCTSTR() + iStart, pszSub);
		return pFind ? pFind - operator LPCTSTR() : -1;
	}
	CMemStr Right(int nCount) const {
		if (nCount < 0) nCount = 0;
		int nLength = GetLength();
		if(nCount >= nLength) return *this;
		return CMemStr(operator LPCTSTR() + nLength - nCount);
	}
	CMemStr Left(int nCount) const {
		if (nCount < 0) nCount = 0;
		int nLength = GetLength();
		if (nCount >= nLength) return *this;
		return CMemStr(operator LPCTSTR(), nCount);
	}
	CMemStr Mid(int iFirst, int nCount) const {
		if (nCount < 0) nCount = 0;
		if (iFirst < 0) nCount = 0;
		if (iFirst > GetLength()) nCount = 0;
		CMemStr val(operator LPCTSTR() + iFirst);
		return val.Left(nCount);
	}
	INT GetAbsData(INT& pos, CMemStr& value, LPCTSTR sep = "\"") const {
		LPCTSTR fin = strstr(operator LPCTSTR() + pos, sep);
		if (fin == NULL) {
			value.Empty();
			return -1;
		}
		pos = fin - operator LPCTSTR() + strlen(sep);
		fin = strstr(operator LPCTSTR() + pos, sep);
		if (fin == NULL) {
			value.Empty();
			return -1;
		}
		INT srt = pos;
		pos = fin - operator LPCTSTR() + strlen(sep);
		INT end = pos - 1;

		value = Mid(srt, end - srt);
		return 0;
	}
};

#endif // __O_TOOLS_H__
