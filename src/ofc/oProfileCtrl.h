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

#ifndef __OPROFILECTRL_H__
#define __OPROFILECTRL_H__

#ifndef __AFXWIN_H__

// 2004.3.2 非MFCに対応

#include <tchar.h>

typedef char* LPTSTR;
typedef const char* LPCTSTR;
typedef int INT;
typedef unsigned int UINT;
typedef int BOOL;
typedef unsigned short WORD;
typedef unsigned long DWORD;

// メモリ操作に関する機能を提供するクラス
class CoEfficiencyMemory
{
protected:
	LPTSTR m_buffer;
	INT m_length;
public:
	CoEfficiencyMemory()
		: m_buffer(new CHAR[8])
		, m_length(0)
	{
		m_buffer[m_length] = NULL;
	}
	CoEfficiencyMemory(const CoEfficiencyMemory& other)
		: m_buffer(NULL)
		, m_length(0)
	{
		INT length = other.GetLength();
		m_buffer = new CHAR[length + 1];
		memcpy(m_buffer, other, length);
		m_buffer[length] = NULL;
		m_length = length;
	}
	CoEfficiencyMemory(LPCTSTR lpszText)
		: m_buffer(NULL)
		, m_length(0)
	{
		INT length = strlen(lpszText);
		m_buffer = new CHAR[length + 1];
		memcpy(m_buffer, lpszText, length);
		m_buffer[length] = NULL;
		m_length = length;
	}
	virtual ~CoEfficiencyMemory()
	{
		delete []m_buffer;
	}
	LPTSTR GetBuffer(INT nSize)
	{
		if (m_length < nSize)
		{
			delete []m_buffer;
			m_buffer = new CHAR[nSize + 1];
			m_length = 0;
			m_buffer[m_length] = NULL;
		}
		return m_buffer;
	}
	INT GetLength() const
	{
		return m_length;
	}
	BOOL IsEmpty() const
	{
		return m_length ? FALSE : TRUE;
	}
	void ReleaseBuffer(INT nNewLength = -1)
	{
		m_length = nNewLength == -1 ? strlen(m_buffer) : nNewLength;
	}
	operator LPCTSTR() const
	{
		return m_buffer;
	}
	CoEfficiencyMemory operator+(LPCTSTR lpszAppend)
	{
		CoEfficiencyMemory other(*this);
		other += lpszAppend;
		return other;
	}
	CoEfficiencyMemory& operator+=(LPCTSTR lpszAppend)
	{
		INT oldLength = GetLength();
		INT addLength = strlen(lpszAppend);
		INT newLength = oldLength + addLength;
		LPTSTR buffer = new CHAR[newLength + 1];
		memcpy(buffer, m_buffer, oldLength);
		memcpy(&buffer[oldLength], lpszAppend, addLength);
		buffer[newLength] = NULL;
		delete []m_buffer;
		m_buffer = buffer;
		m_length = newLength;
		return *this;
	}
	CoEfficiencyMemory& operator=(LPCTSTR lpszAppend)
	{
		delete []m_buffer;
		m_length = strlen(lpszAppend);
		m_buffer = new CHAR[m_length + 1];
		strcpy(m_buffer, lpszAppend);
		return *this;
	}
	CoEfficiencyMemory& operator=(CoEfficiencyMemory& other)
	{
		delete []m_buffer;
		m_length = other.GetLength();
		m_buffer = new CHAR[m_length + 1];
		strcpy(m_buffer, other);
		return *this;
	}
	CoEfficiencyMemory Right(INT nCount) const
	{
		if (m_length <= nCount)
		{
			return *this;
		}

		return &m_buffer[m_length - nCount];
	}
	INT CompareNoCase(LPCTSTR lpsz) const
	{
		return _tcsicmp(m_buffer, lpsz);
	}
};

typedef CoEfficiencyMemory CoString;

#else

#include "oString.h"

#endif // __AFXWIN_H__

// グローバル変数を使わずにメモリの管理を行うクラス
class CExitInstance
{
public:
	// コンストラクタ
	CExitInstance()
	{
	}

	// デストラクタ
	virtual ~CExitInstance()
	{
		Empty();
	}

	// メモリの開放
	static void Empty()
	{
		GetFolder(FALSE);
		GetKeyPos(FALSE);
	}

	// フォルダ用メモリの取得
	static CoString& GetFolder(BOOL bAllocate = TRUE)
	{
		static CoString* pFolder = NULL;
		if (!pFolder && bAllocate)
		{
			pFolder = new CoString;
		}
		else if (pFolder && !bAllocate)
		{
			delete pFolder;
			pFolder = NULL;
		}
		return *pFolder;
	}

	// キー用メモリの取得
	static CoString& GetKeyPos(BOOL bAllocate = TRUE)
	{
		static CoString* pKeyPos = NULL;
		if (!pKeyPos && bAllocate)
		{
			pKeyPos = new CoString;
		}
		else if (pKeyPos && !bAllocate)
		{
			delete pKeyPos;
			pKeyPos = NULL;
		}
		return *pKeyPos;
	}

	// アプリケーション用メモリの取得
	static CoString& GetApplicationPos(BOOL bAllocate = TRUE)
	{
		static CoString* pKeyPos = NULL;
		if (!pKeyPos && bAllocate)
		{
			pKeyPos = new CoString;
		}
		else if (pKeyPos && !bAllocate)
		{
			delete pKeyPos;
			pKeyPos = NULL;
		}
		return *pKeyPos;
	}
};
static CExitInstance theoProfileCtrl;

// プロファイルに関する機能を提供するクラス
class CoProfile
{
public:
	// プロファイルコントロール
	static CExitInstance& GetProfileCtrl()
	{
		return theoProfileCtrl;
	}

	// メモリの開放
	static void Empty()
	{
		GetProfileCtrl().Empty();
	}

	// フォルダの取得
	static CoString GetModuleDir(LPCTSTR lpPath = NULL)
	{
		CoString& folder = GetProfileCtrl().GetFolder();
		if (folder.IsEmpty())
		{
			DWORD dwRet = SearchPath(lpPath, "mainexe", NULL, _MAX_PATH, folder.GetBuffer(_MAX_PATH), NULL);
			folder.ReleaseBuffer();
			if (dwRet)
			{
				CoString drive;
				CoString directory;
				_splitpath(folder, drive.GetBuffer(_MAX_DRIVE), directory.GetBuffer(_MAX_DIR), NULL, NULL);
				drive.ReleaseBuffer();
				directory.ReleaseBuffer();
				folder = drive + directory;
			}
			else if (!lpPath)
			{
				// 実行ファイルのパスを取得
				CoString folder;
				GetModuleFileName(NULL, folder.GetBuffer(_MAX_PATH), _MAX_PATH);
				folder.ReleaseBuffer();
				CoString drive, directory;
				_splitpath(folder, drive.GetBuffer(_MAX_DRIVE), directory.GetBuffer(_MAX_DIR), NULL, NULL);
				drive.ReleaseBuffer(); directory.ReleaseBuffer();
				folder = drive + directory;

				return GetModuleDir(folder);
			}
			else
			{
				folder = lpPath;
			}
		}
		return folder;
	}

	// 設定ファイルの取得
	static CoString GetPrivateProfileKey()
	{
		CoString& keyPos = GetProfileCtrl().GetKeyPos();
		if (keyPos.IsEmpty())
		{
			GetPrivateProfileString(
				"\0 RegistryKey" + 2, "\0 Position" + 2,
				"\0 Software\\Plustar\\pLook" + 2,
				keyPos.GetBuffer(2048), 2048,
				GetModuleDir() + ("\0 regenv.ini" + 2)
				);
			keyPos.ReleaseBuffer();
		}
		return keyPos;
	}

	// 設定ファイルの取得
	static CoString GetPrivateProfileApplication()
	{
		CoString& keyPos = GetProfileCtrl().GetApplicationPos();
		if (keyPos.IsEmpty())
		{
			GetPrivateProfileString(
				"\0 RegistryKey" + 2, "\0 Application" + 2,
				"\0 Software\\Plustar\\pLook" + 2,
				keyPos.GetBuffer(2048), 2048,
				GetModuleDir() + ("\0 regenv.ini" + 2)
				);
			keyPos.ReleaseBuffer();
		}
		return keyPos;
	}

	// レジストリの取得
	static UINT GetProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpKeyPos = NULL)
	{
		CoString csKeyPos = lpKeyPos ? lpKeyPos : GetPrivateProfileKey();

		CoString csBuf=lpKeyName;
		CoString csKey;
		csKey=csKeyPos+"\\"+CoString(lpAppName);

		DWORD wType = REG_SZ;
		LONG lRet;
		HKEY hKeyResult=NULL;
		lRet=RegOpenKeyEx(HKEY_LOCAL_MACHINE,csKey,0,KEY_QUERY_VALUE ,&hKeyResult);
		if (lRet != ERROR_SUCCESS){
			return nDefault;
		}

		DWORD dwValue;
		wType = REG_DWORD;
		DWORD dwSize = sizeof DWORD;
		lRet = RegQueryValueEx(hKeyResult, lpKeyName, NULL, &wType, (BYTE*)&dwValue, &dwSize);
		RegCloseKey(hKeyResult);
		if (ERROR_SUCCESS != lRet){
			return nDefault;
		}

		return dwValue;
	}

	// レジストリの更新
	static BOOL WriteProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nValue, WORD wProtect = 0, LPCTSTR lpKeyPos = NULL)
	{
		CoString csKeyPos = lpKeyPos ? lpKeyPos : GetPrivateProfileKey();

		CoString csKey;
		HKEY hKeyResult;
		DWORD dwDisposition;
		csKey=csKeyPos+"\\"+CoString(lpAppName);

		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, csKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
			KEY_READ|KEY_WRITE,NULL,&hKeyResult,&dwDisposition)){
			// キーが作成された。ないしはキーがオープンされた。
			CoString lpBuf2 = lpKeyName;
			DWORD dwValue = nValue;
			if (ERROR_SUCCESS == RegSetValueEx(hKeyResult, lpBuf2, 0, REG_DWORD, (BYTE*)&dwValue, sizeof DWORD)){
				RegCloseKey(hKeyResult);
				return TRUE;
			}
			return FALSE;
		}
		return FALSE;
	}

	// レジストリの取得
	static CoString GetProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpszDefault, LPCTSTR pKeyPos = NULL)
	{
		CoString keyPos = pKeyPos ? pKeyPos : GetPrivateProfileKey();

		CoString csBuf = lpKeyName;
		CoString csKey = keyPos + "\\" + lpAppName;

		DWORD wType = REG_SZ;
		HKEY hKeyResult = NULL;
		LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, csKey, 0, KEY_QUERY_VALUE, &hKeyResult);
		if (lRet != ERROR_SUCCESS)
		{
			return lpszDefault;
		}
		CoString buffer;
		DWORD dwSize = 2048;
		lRet = RegQueryValueEx(hKeyResult, csBuf, NULL, &wType, (LPBYTE)(LPCTSTR)buffer.GetBuffer(dwSize), &dwSize);
		RegCloseKey(hKeyResult);
		if (ERROR_SUCCESS != lRet)
		{
			return lpszDefault;
		}

		buffer.ReleaseBuffer();
		return buffer;
	}

	// レジストリの更新
	static BOOL WriteProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpoString, WORD wProtect = 0, LPCTSTR pKeyPos = NULL)
	{
		CoString keyPos = pKeyPos ? pKeyPos : GetPrivateProfileKey();

		HKEY hKeyResult;
		DWORD dwDisposition;
		CoString csKey = keyPos + "\\" + lpAppName;
		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, csKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
			KEY_READ|KEY_WRITE,NULL,&hKeyResult,&dwDisposition)){
			// キーが作成された。ないしはキーがオープンされた。
			if (ERROR_SUCCESS == RegSetValueEx(hKeyResult, lpKeyName, 0, REG_SZ, (BYTE*)lpoString, lstrlen(lpoString)+1)){
				RegCloseKey(hKeyResult);
				return TRUE;
			}
			return FALSE;
		}
		return FALSE;
	}
};

#endif // __OPROFILECTRL_H__
