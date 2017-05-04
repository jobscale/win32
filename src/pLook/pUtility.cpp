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

#include "../oCommon/oTools.h"
#include "../oCommon/oSocketTools.h"
#include "../oCommon/oEthernet.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "pProcess.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "pUtility.h"

#include <IPTypes.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

const WCHAR* COPYRIGHT = L"Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター\0";
const CHAR* RETERCODE = P("\r\n\0");
const CHAR* DELIMITER = P("\t\0\0");

LPCTSTR GetAnalyzeDataDir()
{
	static CoString localpath;
	localpath = GetApplicationDataPath();

	localpath += (P("\\pAnalyze"));
	CreateDirectory(localpath, NULL);

	return localpath;
}

LPCTSTR GetAnalyzeProcessDataBasePath()
{
	static CoString localpath;
	localpath = GetAnalyzeDataDir();

	localpath += (P("\\anapro_"));

	return localpath;
}

LPCTSTR GetAnalyzeForeDataBasePath()
{
	static CoString localpath;
	localpath = GetAnalyzeDataDir();

	localpath += (P("\\anafore_"));

	return localpath;
}

LPCTSTR GetAnalyzeProcessDataPath(const COleDateTime& now)
{
	static CoString localpath;
	localpath = GetAnalyzeProcessDataBasePath();

	localpath += now.Format("%Y%m%d");
	localpath += (P(".pan"));

	return localpath;
}

LPCTSTR GetAnalyzeForeDataPath(const COleDateTime& now)
{
	static CoString localpath;
	localpath = GetAnalyzeForeDataBasePath();

	localpath += now.Format("%Y%m%d");
	localpath += (P(".fan"));

	return localpath;
}

LPCTSTR GetClientAnalyzeProcessDataBasePath()
{
	static CoString localpath;
	localpath = GetAnalyzeDataDir();

	localpath += (P("\\backup_anapro_"));

	return localpath;
}

LPCTSTR GetClientAnalyzeForeDataBasePath()
{
	static CoString localpath;
	localpath = GetAnalyzeDataDir();

	localpath += (P("\\backup_anafore_"));

	return localpath;
}

LPCTSTR GetClientAnalyzeProcessDataPath(const COleDateTime& now, const DWORD random)
{
	static CoString localpath;
	localpath = GetClientAnalyzeProcessDataBasePath();

	CoString work;
	work.Format("%s_%X", (LPCTSTR)now.Format("%Y%m%d_%H%M%S"), random);
	localpath += work;
	localpath += (P(".pan"));

	return localpath;
}

LPCTSTR GetClientAnalyzeForeDataPath(const COleDateTime& now, const DWORD random)
{
	static CoString localpath;
	localpath = GetClientAnalyzeForeDataBasePath();

	CoString work;
	work.Format("%s_%X", (LPCTSTR)now.Format("%Y%m%d_%H%M%S"), random);
	localpath += work;
	localpath += (P(".fan"));

	return localpath;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ディレクトリ処理関数

// Program Files/Common Files の取得
INT GetProgramFilesCommon(LPTSTR szPath)
{
	CRITICALTRACE(pFunction, GetProgramFilesCommon);

	szPath[0] = NULL;
	BOOL bRet = SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAM_FILES_COMMON, 0);
	if (!bRet)
	{
		HKEY  hKey;
		DWORD dwType = REG_SZ;
		DWORD dwData = MAX_PATH;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, ("\0 SOFTWARE\\Microsoft\\Windows\\CurrentVersion") + 2, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey, ("\0 CommonFilesDir") + 2 , NULL, &dwType, (LPBYTE)(szPath), &dwData);
			RegCloseKey(hKey);
		}
		else
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		if (szPath[0] == NULL)
		{
			DEBUG_OUT_DEFAULT("failed.");
			_tcscpy(szPath, ("\0 C:\\Program Files\\Common Files") + 2);
		}
	}

	return 0;
}

// ファイル名の生成
INT PleaseTradeName(CoString& localpath, LPCTSTR type, LPCTSTR ext)
{
	CRITICALTRACE(pFunction, PleaseTradeName);

	CoString hostname, ipaddr, ipmask, username, macaddr, adapterName;
	GetHostInfo(hostname, ipaddr, ipmask, username, macaddr, adapterName);

	CoString fname = CoString(type) + ("\0 ." + 2) + ipaddr + ("\0 ." + 2) + COleDateTime::GetCurrentTime().Format("\0 %Y%m%d%H%M%S" + 2) + ext;

	BOOL bRet = SHGetSpecialFolderPath(NULL, localpath.GetBuffer(MAX_PATH), CSIDL_COMMON_APPDATA, 0);
	localpath.ReleaseBuffer();
	if (!bRet || localpath.IsEmpty())
	{
		DEBUG_OUT_DEFAULT("failed.");
		localpath = ("\0 C:\\Documents and Settings\\All Users\\Application Data") + 2;
	}

	localpath += ("\0 \\pLook" + 2);
	CreateDirectory(localpath, NULL);
	localpath += ("\0 \\pLog" + 2);
	CreateDirectory(localpath, NULL);
	localpath += ("\0 \\" + 2) + fname;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 文字列処理関数

INT SJIStoUTF8(const CoString& sjis, CoString& utf8)
{
	CComBSTR bstr = sjis;
	INT size = WideCharToMultiByte(CP_UTF8, 0, bstr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, bstr, -1, utf8.GetBufferSetLength(size), size, NULL, NULL);
	utf8.ReleaseBuffer();

	return 0;
}

INT UTF8toSJIS(const CoString& utf8, CoString& sjis)
{
	INT size = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	CComBSTR bstr(size);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, bstr, size);
	sjis = bstr;

	return 0;
}

INT SJIStoEUC(const CComBSTR& sjis, CComBSTR& euc)
{
	LPOLESTR ole = L"";
	int len = sjis.ByteLength() * 2;
	euc.Append(ole, len);
	void sjistoeuc(char *outbuf, int outlen, const char *inbuf);
	sjistoeuc((char*)euc.m_str, len, (const char*)sjis.m_str);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// レジストリ処理関数

// SettingのHKEY
HKEY GetSettingReg()
{
	// Open the registry key for ALL access. 
	HKEY hKey = NULL;
	DWORD dwDisposition = 0;
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Plustar\\pLook\\Setting", 0, REG_NONE, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
	{
		// The registry key was unable to be created. Return.
		return NULL;
	}

	return hKey;
}

// Setting情報の保存
INT RegSettingSetValue(LPCTSTR pKey, const CoString& recvData)
{
	HKEY hKey = GetSettingReg();
	if (!hKey)
	{
		return -1;
	}

	if (RegSetValueEx(hKey, pKey, NULL, REG_SZ, (LPBYTE)(LPCTSTR)recvData, recvData.GetLength()) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return -1;
	}

	RegCloseKey(hKey);

	return 0;
}

// Setting情報の取得
INT RegSettingGetValue(LPCTSTR pKey, CoString& recvData)
{
	HKEY hKey = GetSettingReg();
	if (!hKey)
	{
		return -1;
	}

	DWORD dwSize = 0;
	DWORD dwType = REG_SZ;
	if (RegQueryValueEx(hKey, pKey, NULL, &dwType, NULL, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return -1;
	}
	if (RegQueryValueEx(hKey, pKey, NULL, &dwType, (LPBYTE)recvData.GetBufferSetLength(dwSize), &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return -1;
	}

	RegCloseKey(hKey);

	return 0;
}

// Setting情報の保存
INT RegSettingSetValue(LPCTSTR pKey, const DWORD& recvData)
{
	HKEY hKey = GetSettingReg();
	if (!hKey)
	{
		return -1;
	}

	if (RegSetValueEx(hKey, pKey, NULL, REG_DWORD, (LPBYTE)&recvData, sizeof DWORD) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return -1;
	}

	RegCloseKey(hKey);

	return 0;
}

// Setting情報の取得
INT RegSettingGetValue(LPCTSTR pKey, DWORD& recvData)
{
	HKEY hKey = GetSettingReg();
	if (!hKey)
	{
		return -1;
	}

	DWORD dwType = REG_SZ;
	DWORD dwSize = sizeof DWORD;
	if (RegQueryValueEx(hKey, pKey, NULL, &dwType, (LPBYTE)&recvData, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return -1;
	}

	RegCloseKey(hKey);

	return 0;
}

// Get the setion registry key
HKEY MyGetSectionKey(LPCTSTR lpszSection)
{
	HKEY hSectionKey = NULL;

	// Store the information within the Plustar high level key.
	char service_reg_key[2048] = { 0 };
	strncat(service_reg_key, "SOFTWARE\\Plustar\\pLook\\", sizeof service_reg_key - strlen(service_reg_key));
	strncat(service_reg_key, lpszSection, sizeof service_reg_key - strlen(service_reg_key));

	DWORD dw = 0;
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, service_reg_key, 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL,
		&hSectionKey, &dw);

	return hSectionKey;
}

// Get a string from the registry
BOOL MyGetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszString, DWORD dwStringBuffer)
{
	HKEY hSecKey = MyGetSectionKey(lpszSection);
	if (hSecKey == NULL)
	{
		return TRUE;
	}
	DWORD dwSize = dwStringBuffer;
	DWORD dwType;
	LONG lResult = RegQueryValueEx(hSecKey, lpszEntry, NULL, &dwType, (LPBYTE)lpszString, &dwSize);
	RegCloseKey(hSecKey);
	return lResult == ERROR_SUCCESS;
}

// Write a string to the registry
BOOL MyWriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszString)
{
	HKEY hSecKey = MyGetSectionKey(lpszSection);
	if (hSecKey == NULL)
		return TRUE;
	LONG lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_SZ,
		(unsigned char *)lpszString, (DWORD)(strlen(lpszString)+1));
	RegCloseKey(hSecKey);
	return lResult == ERROR_SUCCESS;
}

// Get a DWORD from the registry
DWORD MyGetProfileDWORD(LPCTSTR lpszSection, LPCTSTR lpszEntry, DWORD nDefault)
{
	HKEY hSecKey = MyGetSectionKey(lpszSection);
	if (hSecKey == NULL)
		return nDefault;
	DWORD dwValue;
	DWORD dwType;
	DWORD dwCount = sizeof DWORD;
	LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
		(LPBYTE)&dwValue, &dwCount);
	RegCloseKey(hSecKey);
	if (lResult == ERROR_SUCCESS)
	{
		return dwValue;
	}
	return nDefault;
}

// Write a DWORD to the registry
BOOL MyWriteProfileDWORD(LPCTSTR lpszSection, LPCTSTR lpszEntry, DWORD nValue)
{
	HKEY hSecKey = MyGetSectionKey(lpszSection);
	if (hSecKey == NULL)
		return TRUE;
	LONG lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_DWORD,
		(LPBYTE)&nValue, sizeof nValue);
	RegCloseKey(hSecKey);
	return lResult == ERROR_SUCCESS;
}

// DateTime From "2008/02/22 22:22:22"
HRESULT DateFromString(SYSTEMTIME* sysTime, LPCTSTR sysTimeText, size_t len/* = 0*/)
{
	if (len == 0)
	{
		len = strlen(sysTimeText);
	}

	// 0123456789012345678
	// 2008/02/22 22:22:22

	if (len != 10 && len != 16 && len != 19)
	{
		return -1;
	}

	sysTime->wYear = (WORD)strtoul(&sysTimeText[0], NULL, 10);
	sysTime->wMonth = (WORD)strtoul(&sysTimeText[5], NULL, 10);
	sysTime->wDay = (WORD)strtoul(&sysTimeText[8], NULL, 10);

	if (len == 16 || len == 19)
	{
		sysTime->wHour = (WORD)strtoul(&sysTimeText[11], NULL, 10);
		sysTime->wMinute = (WORD)strtoul(&sysTimeText[14], NULL, 10);
	}

	if (len == 19)
	{
		sysTime->wSecond = (WORD)strtoul(&sysTimeText[17], NULL, 10);
	}

	return 0;
}

HRESULT DateFromString(COleDateTime& datetime, LPCTSTR sysTimeText, size_t len/* = 0*/)
{
	SYSTEMTIME sysTime = { 0 };
	HRESULT result = DateFromString(&sysTime, sysTimeText, len);
	datetime = sysTime;
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ライセンス管理

// ライセンス店舗UNIQUEの取得
LPCTSTR GetShopUnique(LPCTSTR unique/* = NULL*/)
{
	static CoString s_unique;
	if (unique != NULL)
	{
		s_unique = unique;
	}
	return s_unique;
}

// ライセンス店舗名の取得
LPCTSTR GetShopLicense(LPCTSTR shopLic/* = NULL*/)
{
	static CoString shopLicense;
	if (shopLic != NULL)
	{
		shopLicense = shopLic;
	}
	return shopLicense;
}

// ライセンス数の取得
DWORD GetMassLicense(DWORD massLic/* = -1*/)
{
	static DWORD massLicense = 0;
	if (massLic != -1)
	{
		massLicense = massLic;
	}
	return massLicense;
}

// ライセンス期限の取得
COleDateTime GetExpireLicense(COleDateTime* expireLic/* = NULL*/)
{
	static COleDateTime expireLicense;
	if (expireLic != NULL)
	{
		expireLicense = *expireLic;
	}
	return expireLicense;
}

// ライセンス有効期限の判定
BOOL IsExpireLicenseDateTime()
{
#ifdef _DEBUG
	return TRUE;
#endif

	COleDateTime expireLicense = GetExpireLicense();
	COleDateTime now = COleDateTime::GetCurrentTime();
	return expireLicense > now;
}

// プラチナライセンス
BOOL IsPlatinum()
{
	CString license = GetAsessNameLicense();
	return (license == "Platinum" || license == "Trial");
}

// ライセンス名の取得
LPCTSTR GetAsessNameLicense(LPCTSTR assessment_name/* = NULL*/)
{
	static CoString asessName;
	if (assessment_name != NULL)
	{
		asessName = assessment_name;
	}
	return asessName;
}

// アップグレード文字列の取得
DWORD GetTrapperUpGrade(DWORD gradeversion/* = 1*/)
{
	static DWORD s_gradeversion = 0;
	if (gradeversion != -1)
	{
		s_gradeversion = gradeversion;
	}
	return s_gradeversion;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// 禁止リスト

// 起動禁止リストの読込
LPCTSTR LoadSuppressionList()
{
	// 読込
	static CoString resData;
	RegSettingGetValue("suppression_list", resData);
	return resData;
}

// 起動禁止リストの保存
void SaveSuppressionList(const CoString& resData)
{
	// 保存
	RegSettingSetValue("suppression_list", resData);
}

// 起動禁止リストの取得
DWORD GetSuppressionList(CoString& resData, BOOL isSet/* = FALSE*/)
{
	static CoString s_taData = LoadSuppressionList();
	if (isSet)
	{
		s_taData = resData;

		SaveSuppressionList(resData);
	}
	else
	{
		resData = s_taData;

		if (!IsExpireLicenseDateTime())
		{
			resData.Empty();
		}
	}

	return 0;
}

// 起動禁止リストの設定
DWORD SetSuppressionList(const CoString& resData)
{
	// 重複削除
	CoStringArray value;
	GetArrayValue(value, resData, "\\,");

	value.Uniq();

	// 文字列サイズ1バイトの登録をさせない
	for (size_t i = 0, count = value.GetSize(); i < count; i++)
	{
		if (value.ElementAt(i).GetLength() <= 1)
		{
			value.RemoveAt(i);
			i--;
			count--;
		}
	}

	CoString setValue;
	GetStringValue(setValue, value, "\\,");

	GetSuppressionList(*(CoString*)&setValue, TRUE);

	return 0;
}

// 個別禁止リストの読込
LPCTSTR LoadKillingList()
{
	// 読込
	static CoString resData;
	RegSettingGetValue("killing_list", resData);
	return resData;
}

// 個別禁止リストの保存
void SaveKillingList(const CoString& resData)
{
	// 保存
	RegSettingSetValue("killing_list", resData);
}

// 個別禁止リストの取得
DWORD GetKillingList(CoString& resData, BOOL isSet/* = FALSE*/)
{
	static CoString s_taData = LoadKillingList();
	if (isSet)
	{
		s_taData = resData;

		SaveKillingList(resData);
	}
	else
	{
		resData = s_taData;
	}

	return 0;
}

// 個別禁止リストの設定
DWORD SetKillingList(const CoString& resData)
{
	// 重複削除
	CoStringArray value;
	GetArrayValue(value, resData, "\\,");

	value.Uniq();

	// 文字列サイズ1バイトの登録をさせない
	for (size_t i = 0, count = value.GetSize(); i < count; i++)
	{
		if (value.ElementAt(i).GetLength() <= 1)
		{
			value.RemoveAt(i);
			i--;
			count--;
		}
	}

	CoString setValue;
	GetStringValue(setValue, value, "\\,");

	GetKillingList(*(CoString*)&setValue, TRUE);

	return 0;
}

// 自動禁止リストの読込
LPCTSTR LoadSuppresAutoList(DWORD& resVer)
{
	// 読込
	RegSettingGetValue("suppres_auto_ver", resVer);
	static CoString s_resData;
	RegSettingGetValue("suppres_auto_list", s_resData);
	return s_resData;
}

// 自動禁止リストの保存
void SaveSuppresAutoList(const CoString& resData, const DWORD& resVer = *LPDWORD(NULL))
{
	// 保存
	if (&resVer) RegSettingSetValue("suppres_auto_ver", resVer);
	RegSettingSetValue("suppres_auto_list", resData);
}

// 自動禁止リストの取得
DWORD GetSuppresAutoList(CoString& resData, DWORD& resVer, BOOL isSet/* = FALSE*/)
{
	static DWORD s_taVer = 0;
	static CoString s_taData = LoadSuppresAutoList(s_taVer);
	if (isSet)
	{
		if (&resVer) s_taVer = resVer;
		s_taData = resData;

		SaveSuppresAutoList(resData, resVer);
	}
	else
	{
		if (&resVer) resVer = s_taVer;
		resData = s_taData;

		if (!IsExpireLicenseDateTime())
		{
			if (&resVer) resVer = 0;
			resData.Empty();
		}
	}

	return 0;
}

// 自動禁止リストの設定
DWORD SetSuppresAutoList(const CoString& resData, const DWORD& resVer)
{
	// 重複削除
	CoStringArray value;
	GetArrayValue(value, resData, "\\,");

	value.Uniq();

	// 文字列サイズ1バイトの登録をさせない
	for (size_t i = 0, count = value.GetSize(); i < count; i++)
	{
		if (value.ElementAt(i).GetLength() <= 1)
		{
			value.RemoveAt(i);
			i--;
			count--;
		}
	}

	CoString setValue;
	GetStringValue(setValue, value, "\\,");

	GetSuppresAutoList(*(CoString*)&setValue, *LPDWORD(&resVer), TRUE);

	return 0;
}

// 自動hostsの読込
LPCTSTR LoadHostsAutoList(DWORD& resVer)
{
	// 読込
	RegSettingGetValue("hosts_auto_ver", resVer);
	static CoString s_resData;
	RegSettingGetValue("hosts_auto_list", s_resData);
	return s_resData;
}

// 自動hostsの保存
LRESULT SaveHostsAutoList(const CoString& resData, const DWORD& resVer)
{
#if defined(_DEBUG) && 0
	CoString null;
	RegSettingSetValue("hosts_auto_list", null);
	return 0;
#endif

	// 保存
	if (&resVer) RegSettingSetValue("hosts_auto_ver", resVer);
	RegSettingSetValue("hosts_auto_list", resData);

	// C:\Windows\System32\drivers\etc\hosts を更新する
	CoString path;
	BOOL bRet = SHGetSpecialFolderPath(NULL, path.GetBuffer(MAX_PATH), CSIDL_SYSTEM, 0);
	path.ReleaseBuffer();
	if (!bRet || path.IsEmpty())
	{
		DEBUG_OUT_DEFAULT("failed.");
		path = ("\0 C:\\Windows\\System32") + 2;
	}
	path += ("\0 \\drivers\\etc\\hosts" + 2);

	WIN32_FILE_ATTRIBUTE_DATA wfad = { 0 };
	if (GetFileAttributesEx(path, GetFileExInfoStandard, &wfad))
	{
		wfad.dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
		SetFileAttributes(path, wfad.dwFileAttributes);
	}

	HANDLE hFile = CreateFile(path, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	DWORD sizeRead;
	DWORD dwReadHigh = 0;
	DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	CoString readData;
	ReadFile(hFile, readData.GetBufferSetLength(dwRead), dwRead, &sizeRead, NULL);

	INT oldPos = readData.Find("\r\n\r\n## version: ");

	LARGE_INTEGER num = { 0 };
	num.QuadPart = oldPos != -1 ? (LONGLONG)oldPos - readData.GetLength() : 0;
	SetFilePointerEx(hFile, num, NULL, FILE_END);

	CoString hostsData = resData;
	hostsData.Replace("\\,", "\r\n");

	CoString writeVersion;
	writeVersion.Format("\r\n\r\n## version: %d ##\r\n\r\n", resVer);
	DWORD dwWrite;
	WriteFile(hFile, writeVersion, writeVersion.GetLength(), &dwWrite, NULL);

	WriteFile(hFile, hostsData, hostsData.GetLength(), &dwWrite, NULL);
	SetEndOfFile(hFile);
	CloseHandle(hFile);

	return 0;
}

// 自動hostsの取得
DWORD GetHostsAutoList(CoString& resData, DWORD& resVer, BOOL isSet/* = FALSE*/)
{
	static DWORD s_taVer = 0;
	static CoString s_taData = LoadHostsAutoList(s_taVer);
	if (isSet)
	{
		if (&resVer == NULL)
		{
			return -1;
		}

		if (s_taVer != resVer)
		{
			s_taVer = resVer;
			s_taData = resData;

			SaveHostsAutoList(resData, resVer);
		}
	}
	else
	{
		if (&resVer) resVer = s_taVer;
		resData = s_taData;

		if (!IsExpireLicenseDateTime())
		{
			if (&resVer) resVer = 0;
			resData.Empty();
		}
	}

	return 0;
}

// 自動hostsの設定
DWORD SetHostsAutoList(const CoString& resData, const DWORD& resVer)
{
	// 重複削除
	CoStringArray value;
	GetArrayValue(value, resData, "\\,");

	value.Uniq();

	// 文字列サイズ1バイトの登録をさせない
	for (size_t i = 0, count = value.GetSize(); i < count; i++)
	{
		if (value.ElementAt(i).GetLength() <= 1)
		{
			value.RemoveAt(i);
			i--;
			count--;
		}
	}

	CoString setValue;
	GetStringValue(setValue, value, "\\,");

	if (!setValue.IsEmpty())
	{
		GetHostsAutoList(*(CoString*)&setValue, *LPDWORD(&resVer), TRUE);
	}

	return 0;
}
