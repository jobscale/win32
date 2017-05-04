#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#include <winsock2.h>
#include <afxsock.h>		// MFC のソケット拡張機能
#include <afxinet.h>

#include "../oCommon/oTools.h"
#include "../oCommon/oEthernet.h"
#include "../oCommon/oHttpSocket.h"
#include "../oCommon/oSocketTools.h"

// pWebBP.conf
// d:"xxx.xxx.xxx.xxx,yyy.yyy.yyy.yyy"    // dns
// r:"xxx.xxx.xxx.xxx:8080"               // proxy
// b:"xxx.xxx.xxx.xxx;yyy.yyy.yyy.yyy"    // bypass

HKEY GetSystemReg()
{
	// Open the registry key for ALL access. 
	HKEY hKey = NULL;
	DWORD dwDisposition = 0;
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM", 0, REG_NONE, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
	{
		// The registry key was unable to be created. Return.
		return NULL;
	}

	CString hostname, ipaddr, ipmask, username, macaddr, adapterName;
	GetHostInfo(hostname, ipaddr, ipmask, username, macaddr, adapterName);

	CString dnsName;

	CString regName;
	DWORD regSize = 1024;
	FILETIME lastTime = { 0 };
	for (DWORD i = 0; RegEnumKeyEx(hKey, i, regName.GetBuffer(1024), &regSize, NULL, NULL, NULL, &lastTime) != ERROR_NO_MORE_ITEMS; i++)
	{
		regSize = 1024;
		regName.ReleaseBuffer();

		if (strncmp(regName, "ControlSet", strlen("ControlSet")) != 0)
		{
			continue;
		}
		if (regName.GetLength() != strlen("ControlSet000"))
		{
			continue;
		}
		regName += "\\Services\\Tcpip\\Parameters\\Interfaces\\";
		regName += adapterName;
		dnsName = "SYSTEM\\" + regName;
		break;
	}

	RegCloseKey(hKey);

	if (dnsName.IsEmpty())
	{
		return NULL;
	}

	// Open the registry key for ALL access. 
	hKey = NULL;
	dwDisposition = 0;
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, dnsName, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
	{
		// The registry key was unable to be created. Return.
		return NULL;
	}

	return hKey;
}

// 設定ファイル名の取得
LPCTSTR GetConfigFilePath()
{
	static CString path;
	if (path.IsEmpty())
	{
		path = GetModuleFolder(1);
		path += ".conf";
	}
	return path;
}

// 設定DNSの取得
LPCTSTR GetBindDNS(LPCTSTR bind = NULL)
{
	static CString s_bind = ""; // "208.67.222.222,208.67.222.220";
	if (bind != NULL)
	{
		s_bind = bind;
	}
	return s_bind;
}

// 設定Proxyの取得
LPCTSTR GetBindProxy(LPCTSTR bind = NULL)
{
	// default pads.exe settings
	static CString s_bind = "http=127.0.0.1:8118\0; https=; ftp="; // "localhost:80";
	if (bind != NULL)
	{
		s_bind = bind;
	}
	return s_bind;
}

// 設定Bypassの取得
LPCTSTR GetBindBypass(LPCTSTR bind = NULL)
{
	static CString s_bind = ""; // "192.168.0.254;192.168.0.253";
	if (bind != NULL)
	{
		s_bind = bind;
	}
	return s_bind;
}

// 機能設定情報の読み込み
DWORD SetConfiguration()
{
	static CString path = GetConfigFilePath();

	HANDLE hFile = INVALID_HANDLE_VALUE;
	if (NativityFile(path, 1, TRUE, &hFile) != 0)
	{
		return -1;
	}
	DWORD dwReadHigh = 0;
    DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	CString targ;
	ReadFile(hFile, targ.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
	CloseHandle(hFile);

	// DNS Server
	INT pos = targ.Find("d:");
	if (pos != -1)
	{
		CString bind = oAbstractSearchSubstance(targ, pos);
		GetBindDNS(bind);
	}

	// Proxy
	pos = targ.Find("r:");
	if (pos != -1)
	{
		CString bind = oAbstractSearchSubstance(targ, pos);
		GetBindProxy(bind);
	}

	// Bypass
	pos = targ.Find("b:");
	if (pos != -1)
	{
		CString bind = oAbstractSearchSubstance(targ, pos);
		GetBindBypass(bind);
	}

	return 0;
}

LRESULT CheckBindDns()
{
	HKEY hKey = GetSystemReg();
	if (!hKey)
	{
		return -1;
	}

	CString getData;
	getData.GetBufferSetLength(_MAX_PATH);
	DWORD dwSize = getData.GetLength();
	DWORD dwType = REG_SZ;
	if (RegQueryValueEx(hKey, "NameServer", NULL, &dwType, (LPBYTE)(LPCTSTR)getData, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return -1;
	}

	RegCloseKey(hKey);

	getData.ReleaseBuffer();
	CString setData = GetBindDNS();
	if (getData != setData)
	{
		return 1;
	}

	return 0;
}

// DNSの設定
LRESULT ControlBindDns()
{
	HKEY hKey = GetSystemReg();
	if (!hKey)
	{
		return -1;
	}

	CString setData = GetBindDNS();
	DWORD dwSize = setData.GetLength();
	DWORD dwType = REG_SZ;
	if (RegSetValueEx(hKey, "NameServer", NULL, dwType, (const BYTE*)(LPCTSTR)setData, dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return -1;
	}

	RegCloseKey(hKey);

	return 0;
}

#include <WinInet.h>

LRESULT CheckIEProxy()
{
	CString setproxy = GetBindProxy();
	CString setbypass = GetBindBypass();
	if (!setbypass.IsEmpty())
	{
		setbypass += ";";
	}
	setbypass += "<local>";

	// To include server for FTP, HTTPS, and so on, use the string
	// (ftp=http://itgproxy:80; https=https://itgproxy:80)
	INTERNET_PER_CONN_OPTION Option[3] = { 0 };

	Option[0].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
	Option[0].Value.pszValue = setproxy.GetBuffer(0);

	Option[1].dwOption = INTERNET_PER_CONN_FLAGS;
	Option[1].Value.dwValue = PROXY_TYPE_PROXY | PROXY_TYPE_DIRECT;
	// This option sets all the possible connection types for the client.
	// This case specifies that the proxy can be used or direct connection is possible.

	Option[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
	Option[2].Value.pszValue = setbypass.GetBuffer(0);

	INTERNET_PER_CONN_OPTION_LIST List = { sizeof INTERNET_PER_CONN_OPTION_LIST };
	List.pszConnection = NULL;
	List.dwOptionCount = 3;
	List.dwOptionError = 0;
	List.pOptions = Option;

	if (!InternetQueryOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &List.dwSize))
	{
		// failed.
		OutputDebugString("InternetQueryOption faild.");
	}

	if (List.dwSize != sizeof INTERNET_PER_CONN_OPTION_LIST)
	{
		return 1;
	}

	if (Option[0].dwOption != INTERNET_PER_CONN_PROXY_SERVER)
	{
		return 1;
	}

	if (!Option[0].Value.pszValue || strcmp(Option[0].Value.pszValue, setproxy.GetBuffer(0)) != 0)
	{
		return 1;
	}

	if (Option[1].dwOption != INTERNET_PER_CONN_FLAGS)
	{
		return 1;
	}

	if (Option[1].Value.dwValue != (PROXY_TYPE_PROXY | PROXY_TYPE_DIRECT))
	{
		return 1;
	}

	if (Option[2].dwOption != INTERNET_PER_CONN_PROXY_BYPASS)
	{
		return 1;
	}

	if (!Option[2].Value.pszValue || strcmp(Option[2].Value.pszValue, setbypass) != 0)
	{
		return 1;
	}

	return 0;
}

int GetHttp(LPCTSTR target, CString& sContent)
{
	OHTTP_DATA data;
	data.target = target;
	INT result = HttpGet(&data);

	sContent = data.recvData;

	return data.headerData.Find("200 OK\r\n") ? 0 : 1;
}

int GetHttpProxy(LPCTSTR target, CString& sContent)
{
	CString setproxy = GetBindProxy();

	int pos = setproxy.Find(':');
	if (pos == -1)
	{
		return -1;
	}
	CString proxy = setproxy.Mid(5, pos - 5);
	int port = strtoul((((LPCTSTR)setproxy) + pos + 1), NULL, 10);
	if (port == 0)
	{
		port = 80;
	}
	OHTTP_DATA data;
	data.target = target;
	data.proxy = proxy;
	data.proxyPort = port;
	INT result = HttpGet(&data);

	sContent = data.recvData;

	if (result == 0)
	{
		result = data.headerData.Find("200 OK\r\n") ? 0 : -1;
	}
	return result;
}

int GetHttpAtl(LPCTSTR target, CString& sContent)
{
	CString m_sStatus;
	CString m_sURL = target;

	// HTTP ステータスコードを取得
	DWORD dwStatus = 0;

	// CInternetSession オブジェクトを作成
	CInternetSession cSession("HTTP Get Test Application ver-1.00");

	try
	{
		// OpenURL を実行して CHttpFile へのポインタを取得する
		CHttpFile* pFile = (CHttpFile*)cSession.OpenURL(m_sURL);

		// HTTP ステータスコードを取得
		pFile->QueryInfoStatusCode(dwStatus);
		m_sStatus.Format("%d", dwStatus);

		// ファイル読み取り用バッファを用意
		char pBuffer[1024];
		UINT iReadBytes;
		sContent = "";

		// 終端に達するまで読み取る
		while(true)
		{
			iReadBytes = pFile->Read(pBuffer, 1024);
			
			CString str((LPCTSTR)pBuffer, iReadBytes);
			sContent += str;

			if (iReadBytes < 1024) break;
		}

		// CHttpFile オブジェクトを削除
		delete pFile;
	}
	catch (CInternetException *pEx)
	{
		// 例外処理
		CString sErr;
		char pBuff[512];

		pEx->GetErrorMessage(pBuff, 512);
		
//		sErr.Format("例外が発生しました。\r\n"
//			"エラー: %d\r\n"
//			"%s",
//			pEx->m_dwError, pBuff);
//
//		::AfxMessageBox(sErr, MB_OK | MB_ICONEXCLAMATION);
	}

	return dwStatus == 200 ? 0 : -1;
}

LRESULT ControlIEProxy(int flag = 0)
{
	CString setproxy = GetBindProxy();
	CString setbypass = GetBindBypass();
	if (!setbypass.IsEmpty())
	{
		setbypass += ";";
	}
	setbypass += "<local>";

	// To include server for FTP, HTTPS, and so on, use the string
	// (ftp=http://itgproxy:80; https=https://itgproxy:80)
	INTERNET_PER_CONN_OPTION Option[3] = { 0 };

	Option[0].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
	Option[0].Value.pszValue = setproxy.GetBuffer(0);

	Option[1].dwOption = INTERNET_PER_CONN_FLAGS;
	Option[1].Value.dwValue = flag ? PROXY_TYPE_PROXY | PROXY_TYPE_DIRECT : PROXY_TYPE_DIRECT;
	// This option sets all the possible connection types for the client.
	// This case specifies that the proxy can be used or direct connection is possible.

	Option[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
	Option[2].Value.pszValue = setbypass.GetBuffer(0);

	INTERNET_PER_CONN_OPTION_LIST List = { sizeof INTERNET_PER_CONN_OPTION_LIST };
	List.pszConnection = NULL;
	List.dwOptionCount = 3;
	List.dwOptionError = 0;
	List.pOptions = Option;

	if (!InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, List.dwSize))
	{
		// failed.
	}

	InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);
	// The connection settings for other instances of Internet Explorer.

	InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
	SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, (WPARAM)(0), (LPARAM)(0));

	// Set proxy succeeded.
	return 0;
}

int SetInternetSettings()
{
	DWORD dwDisposition = 0;
	HKEY hkResult = NULL;
	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, P("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),
			NULL, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition))
	{
			return -1;
	}

	DWORD on = 1;

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("EnableHttp1_1"),
		NULL, REG_DWORD, (LPBYTE)&on, sizeof(on)))
	{
	}

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("ProxyHttp1.1"),
		NULL, REG_DWORD, (LPBYTE)&on, sizeof(on)))
	{
	}

	RegCloseKey(hkResult);

	return 0;
}

int SetInternetExplorerMain()
{
	DWORD dwDisposition = 0;
	HKEY hkResult = NULL;
	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, P("Software\\Microsoft\\Internet Explorer\\Main"),
			NULL, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition))
	{
			return -1;
	}

	LPCTSTR yes = P("yes");
	LPCTSTR no = P("no");

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("Disable Script Debugger"),
		NULL, REG_SZ, (LPBYTE)yes, strlen(yes)))
	{
	}

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("DisableScriptDebuggerIE"),
		NULL, REG_SZ, (LPBYTE)yes, strlen(yes)))
	{
	}

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("Error Dlg Displayed On Every Error"),
		NULL, REG_SZ, (LPBYTE)no, strlen(no)))
	{
	}

	RegCloseKey(hkResult);

	return 0;
}

int SetInternetSecurity()
{
	DWORD dwDisposition = 0;
	HKEY hkResult = NULL;
	DWORD security = 2;

	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, P("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Domains\\plustar.jp\\pad"),
			NULL, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition))
	{
			return -1;
	}

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("http"),
		NULL, REG_DWORD, (LPBYTE)&security, sizeof(security)))
	{
	}

	RegCloseKey(hkResult);

	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, P("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Domains\\plustar.jp\\x"),
			NULL, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition))
	{
			return -1;
	}

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("http"),
		NULL, REG_DWORD, (LPBYTE)&security, sizeof(security)))
	{
	}

	RegCloseKey(hkResult);

	return 0;
}

int InitializeInternet()
{
	SetInternetSettings();
	SetInternetExplorerMain();

#if 0
	SetInternetSecurity();
#endif

	InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);
	// The connection settings for other instances of Internet Explorer.

	InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
	SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, (WPARAM)(0), (LPARAM)(0));

	return 0;
}

#pragma comment(lib, "Version")

int GetFileVersion(LPCTSTR path, CString& ver)
{
	DWORD dwZero = 0;
	DWORD dwVerInfoSize = GetFileVersionInfoSize(path, &dwZero);
	if (dwVerInfoSize == 0)
	{
		return -1;
	}

	unsigned char *pBlock;
	pBlock = new unsigned char[dwVerInfoSize];
	if (pBlock == NULL)
	{
		return -1;
	}
	GetFileVersionInfo(path, dwZero, dwVerInfoSize, pBlock);

	//バージョンを取得する為のバッファ
	void *pvVersion;
	UINT VersionLen;

	//全言語のプロダクトバージョンを表示する例
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;
	UINT TranslateLen;
	VerQueryValue(pBlock, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &TranslateLen);
	for (UINT i = 0; i < TranslateLen / sizeof(*lpTranslate); i++)
	{
		//コードページを指定
		char name[256];
		wsprintf(name, TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
		if (VerQueryValue(pBlock, name, &pvVersion, &VersionLen))
		{
			ver.Format("%s\n", pvVersion);
		}
	}

	delete [] pBlock;
	return TRUE;
}

#define SECOND * (1000)
#define MINUTE * (60 SECOND)

#include <atlcomtime.h>

int MakeParam(CString& param, const CString ip_address, const CString mac_address)
{
	CString aTarg = "pAds.exe";
	CString aVer;
	GetFileVersion(aTarg, aVer);
	CString uTarg = "pUtils.dll";
	CString uVer;
	GetFileVersion(uTarg, uVer);
	CString tTarg = "pTemp.dll";
	CString tVer;
	GetFileVersion(tTarg, tVer);

	const char* datetime = __DATE__ " " __TIME__;
	COleDateTime dt;
	dt.ParseDateTime(datetime);

	param.Format("ip=%s&mac=%s&v=%d.%d.%d-%d.%d.%d", (LPCTSTR)ip_address, (LPCTSTR)mac_address, dt.GetMonth(), dt.GetDay(), dt.GetHour(),
			strtol(aVer, NULL, 10), strtol(uVer, NULL, 10), strtol(tVer, NULL, 10));

	return 0;
}

int LocalProxyCheck(CString& sContent, const CString param)
{
	CString target = P("http://pad.plustar.jp/");
	target += "?";
	target += param;

	return GetHttpProxy(target, sContent);
}

int ConnectFirst(CString& sContent, const CString param)
{
	CString target = P("http://dl.plustar.jp/pads/check.php");
	target += "?";
	target += param;

	return GetHttp(target, sContent);
}

int ConnectFinish(CString& sContent, const CString param)
{
	CString target = P("http://dl.plustar.jp/pads/endofpc.php");
	target += "?";
	target += param;

	return GetHttp(target, sContent);
}

// ネットワーク接続確認
int CheckNetworkAndProxy()
{
	// DNSとのリンク確認
	if (CheckNetwork(PP(dl.plustar.jp)) != 0)
	{
		return -1;
	}

	// Proxy生存確認
	CString sContent;
	CString param;
	if (LocalProxyCheck(sContent, param) != 0)
	{
		return -1;
	}

	return 0;
}

CMemStr GetCurrentVersionData(const char* fname)
{
	HANDLE hFile = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return "";
	}

	DWORD dwReadHigh = 0;
	DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	CMemStr inside;
	ReadFile(hFile, inside.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);

	CloseHandle(hFile);

	return inside;
}

int SaveCurrentVersionData(const char* fname, const INT line, const CMemStr n_version, const CMemStr n_instname, const CMemStr n_fname)
{
	CMemStr inside = GetCurrentVersionData(fname);

	HANDLE hFile = CreateFile(fname, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	// overwrite
	LARGE_INTEGER num = { 0 };
	SetFilePointerEx(hFile, num, NULL, FILE_BEGIN);

	int saveFlag = 0;

	INT pos = 0;
	for (INT i = 0; ; i++)
	{
		CMemStr o_version;
		CMemStr o_instname;
		CMemStr o_fname;

		inside.GetAbsData(pos, o_version);
		inside.GetAbsData(pos, o_instname);
		inside.GetAbsData(pos, o_fname);

		CMemStr wdata;
		if (i == line)
		{
			sprintf_s(wdata.GetBuffer(1024), 1024, "\"%s\" \"%s\" \"%s\"\n", (LPCTSTR)n_version, (LPCTSTR)n_instname, (LPCTSTR)n_fname);
			saveFlag++;
		}
		else
		{
			if (saveFlag && o_fname.IsEmpty())
			{
				break;
			}
			LPCTSTR w_version = strlen((LPCTSTR)o_version) ? (LPCTSTR)o_version : P("0000000000");
			LPCTSTR w_instname = strlen((LPCTSTR)o_instname) ? (LPCTSTR)o_instname : P("dummy");
			LPCTSTR w_fname = strlen((LPCTSTR)o_fname) ? (LPCTSTR)o_fname : P("dummy.exe");
			sprintf_s(wdata.GetBuffer(1024), 1024, "\"%s\" \"%s\" \"%s\"\n", w_version, w_instname, w_fname);
		}
		wdata.ReleaseBuffer();

		DWORD dwWriteHigh = 0;
		WriteFile(hFile, (LPCTSTR)wdata, wdata.GetLength(), &dwWriteHigh, NULL);
	}

	SetEndOfFile(hFile);

	CloseHandle(hFile);

	return 0;
}

int SaveCurrentExecData(const char* fname, const OHTTP_DATA& data)
{
	HANDLE hFile = CreateFile(fname, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	DWORD dwWriteHigh = 0;
	WriteFile(hFile, (LPCTSTR)data.recvData, data.recvData.GetLength(), &dwWriteHigh, NULL);
	SetEndOfFile(hFile);

	CloseHandle(hFile);

	return 0;
}

#ifdef _DEBUG

#define UPDATE_URL P("http://dl.plustar.jp/pads/update/make_version/")
#define UPDATE_VER P("version_update.php")

#else

#define UPDATE_URL P("http://dl.plustar.jp/pads/update/")
#define UPDATE_VER P("version_update.php")

#endif

#define UPD_NAME P("upd")
#define EXE_NAME P("inst.exe")

int UpdatePads(const INT line, const OHTTP_DATA& dataUp, INT& n_pos, const CMemStr& inside, INT& e_pos)
{
	CMemStr n_version;
	CMemStr n_instname;
	CMemStr n_fname;

#if 1
	n_version = oAbstractSearchSubstance(dataUp.recvData, n_pos);
	n_instname = oAbstractSearchSubstance(dataUp.recvData, n_pos);
	n_fname = oAbstractSearchSubstance(dataUp.recvData, n_pos);
#else
	dataUp.recvData.GetAbsData(n_pos, n_version);
	dataUp.recvData.GetAbsData(n_pos, n_instname);
	dataUp.recvData.GetAbsData(n_pos, n_fname);
#endif

	if (n_fname.IsEmpty())
	{
		return -1; // end of update
	}

	CMemStr e_version;
	CMemStr e_instname;
	CMemStr e_fname;

	inside.GetAbsData(e_pos, e_version);
	inside.GetAbsData(e_pos, e_instname);
	inside.GetAbsData(e_pos, e_fname);

	INT n_ver = strtol(n_version, NULL, 10);
	if (n_ver == 0)
	{
		return 0;
	}

	INT e_ver = strtol(e_version, NULL, 10);
	if (n_ver <= e_ver)
	{
		return 0;
	}

	OHTTP_DATA dataExe;
	dataExe.target = UPDATE_URL;
	dataExe.target += n_fname;

	CString ip_address;
	CString mac_address;
	for (; GetLocalInfo(P("dl.plustar.jp"), ip_address, mac_address) != 0; Sleep(2 SECOND))
	{
	}
	CString param;
	MakeParam(param, ip_address, mac_address);

	dataExe.target += "?";
	dataExe.target += param;

	dataExe.referer = P("http://dl.plustar.jp/index.html");
	INT res = HttpGet(&dataExe);
	if (res != 0)
	{
		return 0;
	}

	int instSize = strtol(n_instname, NULL, 10);
	if (instSize != dataExe.contentsLength)
	{
		return 0;
	}

	// CRCの情報
	CString vcrd = n_instname.Right(8);
	// CRCの生成
	int fpos = instSize / 5;
	DWORD jent = 0;
	char* pJent = (char*)&jent;
	pJent[3] = dataExe.recvData[fpos];
	pJent[2] = dataExe.recvData[fpos * 2];
	pJent[1] = dataExe.recvData[fpos * 3];
	pJent[0] = dataExe.recvData[fpos * 4];
	CString crdat;
	crdat.Format("%2x", jent);
	if (vcrd != crdat)
	{
		return 0;
	}

	// CRC情報の除去
	n_instname = n_instname.Left(n_instname.GetLength() - 9);

	SaveCurrentExecData(EXE_NAME, dataExe);

	SaveCurrentVersionData(UPD_NAME, line, n_version, n_instname, n_fname);

#ifdef _DEBUG
	return 1;
#endif

	LPCTSTR path = GetModuleFolder();
	if (strstr(path, P("Projects")) != 0) // development
	{
		return 1;
	}

	RunProcess(EXE_NAME);

	TerminateProcess(NULL, 0);

	return 1; // "run Setup.exe"
}

int UpdatePadsRun()
{
	// 集中アクセス防止のためランダム対応
	static CString ip_address;
	static CString mac_address;
	if (ip_address.IsEmpty() || mac_address.IsEmpty())
	{
		for (; GetLocalInfo(P("dl.plustar.jp"), ip_address, mac_address) != 0; Sleep(2 SECOND))
		{
		}
	}

	const char* sup = strrchr(ip_address, '.');
	int nsup = sup ? strtoul(sup + 1, NULL, 10) : 0;
	srand(nsup);
	DWORD dwMiliseconds = rand();
	dwMiliseconds %= (15 SECOND); // n秒間でバージョン情報取得
	Sleep(dwMiliseconds);

	OHTTP_DATA dataUp;
	dataUp.target = UPDATE_URL;
	dataUp.target += UPDATE_VER;

	char crc[] = "CRC:" __DATE__ " " __TIME__;
	int crclen = strlen(crc);
	for (int i = 0; crc[i]; i++) if (crc[i] == ' ') crc[i] = '-';
	dataUp.target += "?t=";
	dataUp.target += crc + 4;

	CString param;
	MakeParam(param, ip_address, mac_address);
	dataUp.target += "&";
	dataUp.target += param;

	INT res = HttpGet(&dataUp);
	if (res != 0)
	{
		return 0;
	}

	char* pVer = (LPTSTR)(LPCTSTR)dataUp.recvData;
	int lenVer = dataUp.recvData.GetLength();
	if (pVer[0] != '"')
	{
		return 0;
	}
	if (memcmp(&pVer[lenVer - crclen], crc, crclen + 1))
	{
		return 0;
	}

	Sleep(5 SECOND);
	Sleep(dwMiliseconds * 2); // n*2秒間でアップデート取得

	const CMemStr inside = GetCurrentVersionData(UPD_NAME);

	INT n_pos = 0;
	INT e_pos = 0;

	for (INT i = 0; UpdatePads(i, dataUp, n_pos, inside, e_pos) != -1; i++)
	{
	}

	return 0;
}

DWORD WINAPI UpdatePadsThread(LPVOID pData)
{
	CWSAInterface cwsainterface;

#ifdef _DEBUG
	UpdatePadsRun();
	return 0;
#endif

	for (; ; )
	{
		Sleep(1000);

		int UpdateHookTime();
		UpdateHookTime();

		int CheckHookTime();
		if (CheckHookTime())
		{
			UpdatePadsRun();
			break;
		}
	}

	return 0;
}

int update_pads()
{
	HANDLE hThread = CreateThread(NULL, 0, UpdatePadsThread, NULL, 0, NULL);
	CloseHandle(hThread);

	return 0;
}

DWORD WINAPI main_loop(LPVOID pVoid)
{
	CWSAInterface cwsainterface;

	// ネットワーク接続確認
	if (CheckNetworkAndProxy() != 0)
	{
		// ネットワーク未接続
		ControlIEProxy(0);
	}

	// ネットワークが繋がるまで待機
	CString ip_address;
	CString mac_address;
	if (ip_address.IsEmpty() || mac_address.IsEmpty())
	{
		for (; GetLocalInfo(P("dl.plustar.jp"), ip_address, mac_address) != 0; Sleep(2 SECOND))
		{
		}
	}

	CString param;
	MakeParam(param, ip_address, mac_address);

	CString sContent;
	ConnectFirst(sContent, param);

	// 抑止されていない場合は常にProxy制御を行う
	if (((LPCTSTR)sContent)[0] /*!= '1'*/== '0')
	{
		// Proxy の制御が許可されていない
		ControlIEProxy(0);
		return 0;
	}

	DWORD waitTime = 1 MINUTE;
	for (; ; )
	{
#if 0 // ランシステム対応: 書き換えなし
		// DNSの参照先のチェック
		if (CheckBindDns() != 0)
		{
			// DNSの参照先の変更
			ControlBindDns();
		}
#endif

		// IEのProxyのチェック
		if (strlen(GetBindProxy()) != 0)
		{
			// ネットワーク接続確認
			if (CheckNetworkAndProxy() == 0)
			{
				// 繋がったので次のチェックは1分後
				waitTime = 1 MINUTE;

				if (CheckIEProxy() != 0)
				{
					// システムのProxyの設定
					ControlIEProxy(1);
				}
			}
			else
			{
				// 繋がらなかったので次のチェックは10秒後
				waitTime = 10 SECOND;

				// Disable Proxy
				ControlIEProxy(0);
			}
		}

		Sleep(waitTime);
	}

	return 0;
}

int OnEndSession()
{
	CWSAInterface cwsainterface;

	CString target = P("http://pad.plustar.jp/ps_getend");

	CString sContent;
	GetHttpProxy(target, sContent);

	CString ip_address;
	CString mac_address;
	for (; GetLocalInfo(P("dl.plustar.jp"), ip_address, mac_address) != 0; Sleep(2 SECOND))
	{
	}

	CString param;
	MakeParam(param, ip_address, mac_address);

	param += "&d=";
	param += sContent;

	ConnectFinish(sContent, param);

	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HANDLE hMutex = CreateMutex(NULL, TRUE, P("pWebBPMutex"));
	if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		OutputDebugString("duplicate process failed. pWebBP");
		return 9;
	}

	if (strstr(lpCmdLine, "--uninstall") != 0)
	{
		ControlIEProxy(0);
		return 0;
	}

	if (0)
	{
		for (; ; )
		{
			ControlIEProxy(0);
			ControlIEProxy(1);
		}
	}

	LPCTSTR path = GetModuleFolder();
	SetCurrentDirectory(path);

	InitializeInternet();

	update_pads();

	SetConfiguration();

	HANDLE hThread = CreateThread(NULL, 0, main_loop, NULL, 0, NULL);
	CloseHandle(hThread);

	BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
	InitInstance(hInstance, SW_HIDE);

	// メイン メッセージ ループ
	MSG msg = { 0 };
	for (; GetMessage(&msg, NULL, 0, 0); )
	{
		if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
