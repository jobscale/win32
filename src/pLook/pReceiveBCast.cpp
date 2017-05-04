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
#include "../oCommon/oHttpSocket.h"
#include "../oCommon/oBase64.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"
#include "../ofc/oFile.h"

#include "oHttpConf.h"

#include "pProcess.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "pUtility.h"

#include "oMyJson.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// インラインソース

	#include "pConsoleData.inl"
	#include "pConsoleHtml.inl"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

DWORD GetInternet(OHTTP_DATA* pHttpData);
DWORD PopupMessageWindow(LPCTSTR lpszText, LPCTSTR lpszLink = "", DWORD color = 2, DWORD width = 300, DWORD height = 300, DWORD status = 0, DWORD showSeconds = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
//
#define	MACSIZE				(6)
#define	MACLOOP				(16)
#define	PORTECHO			(7)

// 自動アップデート用バージョン
static LPCTSTR _version = "00106";

// 履歴
// 00104 外部送信データ形式をJSON化
//       ANAデータ送信後に自動再起動
//       IEのデータマージを行う(貴重なデータなので欲しいけどデータ量が膨大なので諦めた、、)
//       GUIで個別マシン利用状況アイコン
// 00103 pExTrapperとpToolsをサービスに登録
// 00102 通常実行
// 00101 Service で動作

// 自動アップデート用バージョン
LPCTSTR GetThisVersion()
{
	return _version;
}

LPCTSTR GetThisRev()
{
	return __DATE__ " " __TIME__;
}

int WakeUpConsole(LPCTSTR deststr, WORD portno, LPCTSTR text)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) 
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	CoAutoSocket autoSocket(sock);

	int yes = 1;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof yes);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portno);
	addr.sin_addr.s_addr = inet_addr(deststr);

	int n = sendto(sock, text, strlen(text), 0, (struct sockaddr *)&addr, sizeof addr);

	return 0;
}

DWORD IsCheckBroadcast(DWORD check = -1)
{
	static DWORD isCheckBroadcast = 0;

	if (check != -1)
	{
		isCheckBroadcast = check;
	}

	return isCheckBroadcast;
}

LRESULT GetBroadcastIP(CoString& broadcastIP)
{
	COMMAND_CTRL_DATA cmdData;
	GetCommandCtrlData(cmdData);

	DWORD dwIpAddr = inet_addr(cmdData.ipaddr);
	DWORD dwIpMask = inet_addr(cmdData.ipmask);

	in_addr ina = { 0 };
	dwIpAddr |= ~dwIpMask;
	ina.S_un.S_addr = dwIpAddr;
	broadcastIP = inet_ntoa(ina);

	return 0;
}

DWORD BroadcastSpecialData(LPCTSTR pCmdKind, LPCTSTR data)
{
	if (!IsThisMasterConsole())
	{
		// 管理ではない
		DEBUG_OUT_DEFAULT("ERROR: duplicate console failed.");
		return -1;
	}

	COMMAND_CTRL_DATA cmdData;
	GetCommandCtrlData(cmdData);

	CoString masterConsoleIP = cmdData.ipaddr;

	COleDateTime sysTime = cmdData.boot_time;

	CoString broadcastIP;
	GetBroadcastIP(broadcastIP);

	CoString sendData;
	sendData.Format("%s\"%s\" \"%s\" \"%s\" \"%s\"", pCmdKind, (LPCTSTR)sysTime.Format("%Y/%m/%d %H:%M:%S"), (LPCTSTR)masterConsoleIP, (LPCTSTR)data, (LPCTSTR)data);

	WakeUpConsole(broadcastIP, 12083, sendData);
	if (!IsCheckBroadcast())
	{
		WakeUpConsole("127.0.0.1", 12083, sendData);
	}

	return 0;
}

DWORD GetParameter(const LPTSTR REQUEST, CoString key, CoString & val)
{
	key += "=";
	val = REQUEST;
	INT start = val.Find(key);
	if (start >= 0)
	{
		start += strlen(key);
		INT pos = ((CString)val).Find("&", start);
		if (pos < 0) {
			val = val.Right(val.GetLength()-start);
		} else {
			val = val.Mid(start, pos-start);
		}
	}
	else
	{
		val = "";
	}

	return val.GetLength();
}

// sting から 16進数変換
BOOL atox( unsigned char *to, const char *lp ){
	BYTE ans=0;
	char *wp;

	wp = (char*)lp;
	while( *wp ){
		if(		'0' <= *wp && *wp <= '9' )	ans = ans * 0x10 + (*wp-'0');
		else if('A' <= *wp && *wp <= 'F' )	ans = ans * 0x10 + (*wp-'A'+10);
		else if('a' <= *wp && *wp <= 'f' )	ans = ans * 0x10 + (*wp-'a'+10);
		else return FALSE;
		wp++;
	}
	*to = ans;
	return TRUE;
}

int HandleWebPages(LPTSTR HTTPBuffer, LPTSTR REQUEST, SOCKET http_socket)
{
	if (strcmp(HTTPBuffer, "/") == 0 || strcmp(HTTPBuffer, "/index.html") == 0  ||
		strcmp(HTTPBuffer, "/index.shtml") == 0 || strcmp(HTTPBuffer, "/index.php") == 0 ||
		strcmp(HTTPBuffer, "/index.cgi") == 0 || strcmp(HTTPBuffer, "/index") == 0)
	{
		if (!IsExpireLicenseDateTime())
		{
			// output page
			DisplayExpirePage(http_socket);

			return 0;
		}

		// output index page
		DisplayIndexPage(http_socket);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/machinelist") == 0)
	{
		// machine list action
		DisplayMachineList(http_socket);
		return 0;
	}
	else if (strncmp(HTTPBuffer, "/lib/", strlen("/lib/")) == 0)
	{
		// output external resource
		CoString path = GetModuleFolder();
		path += HTTPBuffer;
		DisplayExternalResource(http_socket, path);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/updatehistory") == 0)
	{
		// アップデート履歴の取得
		OHTTP_DATA httpData;
		httpData.target = HTTP_PLUSTAR[6];

		DWORD result = GetInternet(&httpData);

		DisplayUpdateHistoryList(http_socket, httpData.recvData);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/stopmcheinelist") == 0)
	{
		// 停止しているマシンのリストを取得
		CoString data;
		DWORD result = GetMachineList(data,1);

		DisplayStopMcheineList(http_socket, data);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/wakeup") == 0)
	{
		// shutdown action
		CoString macaddr;
		(VOID)GetParameter(REQUEST, "macaddr", macaddr);
		//UPDATE_COMSOLE_DATA console = GetConsoleData(macaddr);
		//CoString cmd;
		//base64_encode("shutdown", cmd);
		//DataSendConsole(console.ipaddr, 12089, cmd);
		macaddr.Replace(":", "-");

		// MACﾁｪｯｸ(:)
		// FF:FF:FF:FF:FF:FF
		// 01234567890123456

		char	wbuf[18];
		BYTE lpmac[MACSIZE+1] = { 0 };
		ZeroMemory( wbuf, sizeof(wbuf) );
		lstrcpy( wbuf, macaddr );

		// MACアドレスのﾁｪｯｸ
		for( int ix=0 ; ix<MACSIZE ; ix++ ){
			wbuf[ix*2+ix+2] = NULL;
			atox( &lpmac[ix], &wbuf[ix*2+ix] );
		}

		int answer = 0;
		SOCKET fd = INVALID_SOCKET;
		int result;
		char sendbuf[MACLOOP * MACSIZE + MACSIZE + 1];
		ZeroMemory(sendbuf, MACLOOP * MACSIZE + MACSIZE + 1);
		//struct sockaddr_in	fromaddr;
		struct sockaddr_in	toaddr;
		char optValue = 1;

		//--------------------------------------------
		//	送信データ作成
		//--------------------------------------------
		FillMemory( &sendbuf[0], MACSIZE, 0xFF );
		for( int ix=0 ; ix<MACLOOP ; ix++ ){
			CopyMemory( &sendbuf[MACSIZE+ix*MACSIZE], lpmac, MACSIZE );
		}
		//--------------------------------------------
		//	ソケットの生成
		//--------------------------------------------
		if((fd = socket(AF_INET,SOCK_DGRAM,0)) == INVALID_SOCKET){
			answer = -1;
			goto ExitModule;
		}
		//--------------------------------------------
		//	Socket Option
		//--------------------------------------------
		result = setsockopt( fd, SOL_SOCKET, SO_BROADCAST, &optValue, sizeof(optValue) );
		if( result == SOCKET_ERROR ){
			answer = -3;
			goto ExitModule;
		}
		//--------------------------------------------
		//	送信先設定
		//--------------------------------------------
		ZeroMemory(&toaddr, sizeof(toaddr));
		toaddr.sin_port = htons(PORTECHO);
		toaddr.sin_family = AF_INET;
		ULONG bcad = 0;
		{
			CoString broadcastIP;
			GetBroadcastIP(broadcastIP);
			bcad = inet_addr(broadcastIP);
		}
		toaddr.sin_addr.S_un.S_addr = bcad; //INADDR_BROADCAST;
		//--------------------------------------------
		//	ﾃﾞｰﾀ送信
		//--------------------------------------------
		result = sendto( fd, sendbuf, MACLOOP * MACSIZE + MACSIZE, 0, (struct sockaddr*)&toaddr, sizeof(toaddr) );
		if( result != MACLOOP * MACSIZE + MACSIZE ){
			answer = -4;
			goto ExitModule;
		}

ExitModule:
	if( fd != INVALID_SOCKET )
		closesocket( fd );

		DisplayResult(http_socket, true);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/shutdown") == 0)
	{
		// shutdown action
		CoString macaddr;
		(VOID)GetParameter(REQUEST, "macaddr", macaddr);
		UPDATE_COMSOLE_DATA console = GetConsoleData(macaddr);
		CoString cmd;
		base64_encode("shutdown", cmd);
		DataSendConsole(console.ipaddr, 12089, cmd);
		DisplayResult(http_socket, true);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/reboot") == 0)
	{
		// reboot action
		CoString macaddr;
		(VOID)GetParameter(REQUEST, "macaddr", macaddr);
		UPDATE_COMSOLE_DATA console = GetConsoleData(macaddr);
		CoString cmd;
		base64_encode("reboot", cmd);
		DataSendConsole(console.ipaddr, 12089, cmd);
		DisplayResult(http_socket, true);
		return 0;
	}
	// 起動フォアグラウンドウインドウリスト
	else if (strcmp(HTTPBuffer, "/foreground") == 0)
	{
		// process list action
		CoString macaddr;
		(VOID)GetParameter(REQUEST, "macaddr", macaddr);
		UPDATE_COMSOLE_DATA console = GetConsoleData(macaddr);
		CoString cmd;

		CoString procname;
		if (GetParameter(REQUEST, "procname", procname) > 0)
		{
			// kill process
			base64_encode("killprocess "+procname, cmd);
			DataSendConsole(console.ipaddr, 12089, cmd);
		}

		base64_encode("foregroundlist", cmd);
		CoString data;
		DataSendConsole(console.ipaddr, 12089, cmd, &data);
		DisplayProcessList(http_socket, &data);

		// データフォーマット
		// [ { "pid":"123", "hModule":"ABC", "hWnd":"ABC", "path":"Path", "title":"Title", "className":"Name", "begin":"2010/10/10 22:22:22", "span":"01 22:22:22" } ]"

		return 0;
	}
	// 起動プロセスリスト
	else if (strcmp(HTTPBuffer, "/process") == 0)
	{
		// process list action
		CoString macaddr;
		(VOID)GetParameter(REQUEST, "macaddr", macaddr);
		UPDATE_COMSOLE_DATA console = GetConsoleData(macaddr);
		CoString cmd;

		CoString procname;
		if (GetParameter(REQUEST, "procname", procname) > 0)
		{
			// kill process
			base64_encode("killprocess "+procname, cmd);
			DataSendConsole(console.ipaddr, 12089, cmd);
		}

		base64_encode("processlist", cmd);
		CoString data;
		DataSendConsole(console.ipaddr, 12089, cmd, &data);
		DisplayProcessList(http_socket, &data);
		return 0;
	}
	// 接続先
	else if (strcmp(HTTPBuffer, "/connectionlist") == 0)
	{
		// connection list action
		CoString macaddr;
		(VOID)GetParameter(REQUEST, "macaddr", macaddr);
		UPDATE_COMSOLE_DATA console = GetConsoleData(macaddr);
		CoString cmd;
		base64_encode("connectionlist", cmd);
		CoString data;
		DataSendConsole(console.ipaddr, 12089, cmd, &data);
		DisplayConnectionList(http_socket, &data);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/screenshot") == 0)
	{
		static CoString screenshot_target_macaddr;

		// authentication
		if (strcmp(screenshot_target_macaddr, "") == 0)
		{
			CoString password;
			if (GetParameter(REQUEST, "password", password) <= 0)
			{
				return DisplayInternalServerError(http_socket);
			}

			CoString sshot_password;
			(VOID)GetScreenPassword(sshot_password);

			if (password.Compare(sshot_password) == 0)
			{
				(VOID)GetParameter(REQUEST, "macaddr", screenshot_target_macaddr);
				return DisplayResult(http_socket, true);
			}
			else
			{
				return DisplayInternalServerError(http_socket);
			}
		}

		// screenshot action
		UPDATE_COMSOLE_DATA console = GetConsoleData(screenshot_target_macaddr);
		screenshot_target_macaddr = "";

		CoString data;
		DataSendConsole(console.ipaddr, 12087, "screenshot:", &data);
		DisplayScreenshot(http_socket, &data);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/suppression") == 0)
	{
		// suppression list action
		CoString data;
		if (GetParameter(REQUEST, "data", data) > 0)
		{
			UTF8toSJIS(data, data);

			// save suppression list
			INT start = data.Find('[') + 1;
			INT pos = data.ReverseFind(']');
			data = data.Mid(start, pos - start);
			data.Replace("{\"procname\":\"", "");
			data.Replace("\"},", "\\,");
			data.Replace("\"}", "");

			SetSuppressionList(data);
			// 管理設定の自動停止リストの送信
			LRESULT UploadSuppressionListData();
			UploadSuppressionListData();

			BroadcastSpecialData("SET SUPPRESSION COMMAND ", data);
		}
		else
		{
			(VOID)GetSuppressionList(data);
		}

		DisplaySuppressionList(http_socket, data);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/password") == 0)
	{
		// change password action
		CoString type;
		(VOID)GetParameter(REQUEST, "password_type", type);
		CoString opwd;
		(VOID)GetParameter(REQUEST, "old_password", opwd);
		CoString npwd;
		(VOID)GetParameter(REQUEST, "new_password", npwd);
		CoString cpwd;
		(VOID)GetParameter(REQUEST, "cfm_password", cpwd);

		DisplayPassword(http_socket, type, opwd, npwd, cpwd);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/misc") == 0)
	{
		CoString tm_type;
		(VOID)GetParameter(REQUEST, "tm_type", tm_type);
		CoString re_type;
		(VOID)GetParameter(REQUEST, "re_type", re_type);
		CoString p2p_type;
		(VOID)GetParameter(REQUEST, "p2p_type", p2p_type);

		DisplayMisc(http_socket,tm_type,re_type,p2p_type);
		return 0;
	}
	// フィルター系
	else if (strcmp(HTTPBuffer, "/filter") == 0)
	{
		// POSTされることはないので取得を行わない
		CoString status = "0";
		//(VOID)GetParameter(REQUEST, "status", status);
		CoString server;
		(VOID)GetParameter(REQUEST, "filter_server", server);
		CoString password;
		(VOID)GetParameter(REQUEST, "filter_server_password", password);

		DisplayMiscFilter(http_socket,status,server, password);
		return 0;
	}
	// Sync系
	else if (strcmp(HTTPBuffer, "/sync") == 0)
	{
		CoString server;
		(VOID)GetParameter(REQUEST, "sync_server", server);

		DisplayMiscSync(http_socket,server);
		return 0;
	}
	// 回線状態
	else if (strcmp(HTTPBuffer, "/net") == 0)
	{
		CoString mon;
		(VOID)GetParameter(REQUEST, "net_mon", mon);
		CoString con;
		(VOID)GetParameter(REQUEST, "net_con", con);
		CoString alt;
		(VOID)GetParameter(REQUEST, "net_alt", alt);

		DisplayMiscNet(http_socket,mon,con,alt);
		return 0;
	}
	// 通知
	else if (strcmp(HTTPBuffer, "/notice") == 0)
	{
		CoString nc_pop;
		(VOID)GetParameter(REQUEST, "nc_pop", nc_pop);
		CoString nc_mail;
		(VOID)GetParameter(REQUEST, "nc_mail", nc_mail);
		CoString nc_mailadd;
		(VOID)GetParameter(REQUEST, "nc_mailadd", nc_mailadd);
		CoString alert_type;
		(VOID)GetParameter(REQUEST, "alert_type", alert_type);

		DisplayMiscNotice(http_socket,nc_pop,nc_mail,nc_mailadd, alert_type);
		return 0;
	}
	// マシン構成リスト
	else if (strcmp(HTTPBuffer, "/machineinfo") == 0)
	{
 		// machine list action
		CoString macaddr;
		(VOID)GetParameter(REQUEST, "macaddr", macaddr);
		UPDATE_COMSOLE_DATA console = GetConsoleData(macaddr);
		CoString cmd;
		base64_encode("machineinfo", cmd);
		CoString data;
		DataSendConsole(console.ipaddr, 12089, cmd, &data);
		DisplayMachineInfo(http_socket, &data);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/machineinfolist") == 0)
	{
		DisplayMachineInfoList(http_socket);
		return 0;
	}
	// インストール情報
	else if (strcmp(HTTPBuffer, "/install") == 0)
	{
		// install list action
		CoString macaddr;
		(VOID)GetParameter(REQUEST, "macaddr", macaddr);
		UPDATE_COMSOLE_DATA console = GetConsoleData(macaddr);
		CoString cmd;
		base64_encode("uninstall", cmd);
		CoString data;
		DataSendConsole(console.ipaddr, 12089, cmd, &data);
		DisplayInstall(http_socket, &data);
		return 0;
	}
	// URL履歴
	else if (strcmp(HTTPBuffer, "/url") == 0)
	{
		// url list action
		CoString macaddr;
		(VOID)GetParameter(REQUEST, "macaddr", macaddr);
		UPDATE_COMSOLE_DATA console = GetConsoleData(macaddr);
		CoString cmd;
		base64_encode("urlhistory", cmd);
		CoString data;
		DataSendConsole(console.ipaddr, 12089, cmd, &data);
		DisplayUrl(http_socket, &data);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/log") == 0)
	{
		DisplayClientLog(http_socket);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/extrapper.css") == 0)
	{
		// output stylesheet
		DisplayStylesheet(http_socket);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/extrapper.js") == 0)
	{
		// output javascript
		DisplayJavaScript(http_socket);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/extrapper3.css") == 0)
	{
		// output stylesheet
		DisplayStylesheet(http_socket,"3");
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/extrapper3.js") == 0)
	{
		// output javascript
		DisplayJavaScript(http_socket,"3");
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/logo.png") == 0)
	{
		// output logo.png
		DisplayImageLogo(http_socket);
		return 0;
	}
	else if (strcmp(HTTPBuffer, "/powered.png") == 0)
	{
		// output powered.png
		DisplayImagePowered(http_socket);
		return 0;
	}
	else
	{
		if (IsSuperDebug())
		{
			// 管理端末の切替
			if (strcmp(HTTPBuffer, "/superuser_force") == 0)
			{
				CCriticalSection& CommandCtrlDataCritical();
				CoCriticalSectionCtrl critical(CommandCtrlDataCritical());

				COMMAND_CTRL_DATA& GetCommandCtrlData();
				COMMAND_CTRL_DATA& cmdData = GetCommandCtrlData();

				COleDateTime old = COleDateTime::GetCurrentTime();
				COleDateTimeSpan day(60, 0, 0, 0);
				old -= day;
				old.GetAsSystemTime(cmdData.boot_time);

				// output index page
				DisplayIndexPage(http_socket);

				return 0;
			}
		}
	}

	// 404
	return DisplayNotFound(http_socket);
}

// 通信スレッド
DWORD WINAPI MultiSocketThread(LPVOID pParam)
{
	OSOCKET_DATA* pData = (OSOCKET_DATA*)pParam;

	CWSAInterface wsa;

	DWORD result = pData->func(pData);

	delete pData;

	return result;
}

// ブロードキャストコマンド送信スレッド
DWORD WINAPI SendBroadcastConsoleThread(LPVOID pParam)
{
	CWSAInterface wsa;

	BroadcastSpecialData("GET ANALYZE PROCESS COMMAND ", "");

	return 0;
}

// ブロードキャストコマンド送信スレッド
DWORD WINAPI SendBroadcastThread(LPVOID pParam)
{
	CWSAInterface wsa;

	DWORD& flagBroadcast = *LPDWORD(pParam);

	COleDateTime expire_date = GetExpireLicense();
	CoString str_date = expire_date.Format("%Y/%m/%d");
	BroadcastSpecialData("GET CONSOLE COMMAND ", str_date);

	Sleep(7200);

	SaveConsoleData();

	flagBroadcast = 0;

	return 0;
}

BOOL Authenticate(const CoString recvbuf)
{
#ifdef _DEBUG
	return TRUE;
#endif

	BOOL auth = TRUE;

	CoString recvData = recvbuf;
	LPTSTR buffer = recvData.GetBuffer(0);

	LPTSTR pos = strstr(buffer, "Authorization: Basic ");
	if (!pos)
	{
		auth = FALSE;
	}
	else
	{
		LPCTSTR pass = pos + strlen("Authorization: Basic ");
		CoString authorization;
		base64_decode(pass, authorization, 0);

		CoString passStr;
		if (GetConsolePassword(passStr) != 0)
		{
			DEBUG_OUT_DEFAULT("[ERROR]: GetConsolePassword failed.");
			auth = FALSE;
		}

		if (authorization.CompareNoCase(passStr) != 0)
		{
			DEBUG_OUT_DEFAULT("Requesting Auth: bad password");
			auth = FALSE;
		}
	}

	recvData.ReleaseBuffer();
	return auth;
}

DWORD GetContentLength(const CoString recvbuf)
{
	DWORD length = 0;

	CoString recvData = recvbuf;
	LPTSTR buffer = recvData.GetBuffer(0);

	LPTSTR pos = strstr(buffer, "Content-Length: ");
	if (pos)
	{
		buffer = pos + strlen("Content-Length: ");
		pos = strstr(buffer, "\n");
		if (pos)
		{
			*pos = '\0';
		}
		pos = strstr(buffer, "\r");
		if (pos)
		{
			*pos = '\0';
		}
		length = atol(buffer);
	}

	recvData.ReleaseBuffer();
	return length;
}

BOOL HandleHttpGET(LPTSTR HTTPBuffer,
				 LPTSTR REQUEST,
				 const CoString recvbuf,
				 const INT sizeBuf,
				 const SOCKET sock)
{
	LPTSTR pos = strstr(HTTPBuffer, "GET ");
	if (pos)
	{
		// url
		strncpy(HTTPBuffer, pos+strlen("GET "), sizeBuf);
		pos = strstr(HTTPBuffer, "?");
		if (pos)
		{
			*pos = '\0';
		}
		pos = strstr(HTTPBuffer, " HTTP/");
		if (pos)
		{
			*pos = '\0';
		}

		// parameter
		pos = strstr(REQUEST, "?");
		if (pos)
		{
			strncpy(REQUEST, pos+1, sizeBuf);
			pos = strstr(REQUEST, " HTTP/");
			if (pos)
			{
				*pos = '\0';
			}
		}
		else
		{
			*REQUEST = '\0';
		}
		return TRUE;
	}
	return FALSE;
}

BOOL HandleHttpPOST(LPTSTR HTTPBuffer,
					LPTSTR REQUEST,
					const CoString recvbuf,
					const INT sizeBuf,
					const SOCKET sock)
{
	LPTSTR pos = strstr(HTTPBuffer, "POST ");
	if (pos)
	{
		// url
		strncpy(HTTPBuffer, pos+strlen("POST "), sizeBuf);
		pos = strstr(HTTPBuffer, " HTTP/");
		if (pos)
		{
			*pos = '\0';
		}

		// content-length
		DWORD len = GetContentLength(recvbuf);
		if (len == 0)
		{
			*REQUEST = '\0';
			return TRUE;
		}

		// parameter
		pos = strstr(REQUEST, "\r\n\r\n");
		if (pos)
		{
			strncpy(REQUEST, pos+strlen("\r\n\r\n"), sizeBuf);

			DWORD rest = len - strlen(REQUEST);
			if (rest != 0)
			{
				CoString buf;
				INT bytesRecv = recv(sock, buf.GetBuffer(rest), rest, 0);
				if (bytesRecv == SOCKET_ERROR || bytesRecv == WSAECONNRESET)
				{
					return FALSE;
				}
				if (bytesRecv != rest)
				{
					DEBUG_OUT_DEFAULT("recv length error.");
					return FALSE;
				}
				strncat(REQUEST, buf, rest);
			}
		}
		return TRUE;
	}
	return FALSE;
}

LRESULT DefaultBroadcastCommand()
{
	static DWORD flagBroadcast = 0;
	if (flagBroadcast != 0)
	{
		return -1;
	}

	flagBroadcast = 1;
	HANDLE hThread = CreateThread(NULL, 0, SendBroadcastThread, &flagBroadcast, 0, NULL);
	CloseHandle(hThread);

	return 0;
}

// socket thread
DWORD WINAPI CommandThread(LPVOID pParam)
{
	CRITICALTRACE(oReceiveBCast, CommandThread);

	CWSAInterface wsa;

	static DWORD load_console = LoadConsoleData();

	DefaultBroadcastCommand();

	SOCKET sock = (*(OSOCKET_DATA*)pParam).socket;

	CoAutoSocket autoSocket(sock);

	CoString recvData;
	LRESULT resRecv = TransRecvBrowser(sock, recvData);
	if (resRecv != 0)
	{
		// 通信障害
		DEBUG_OUT_DEFAULT("recv from browser failed.");
		return -1;
	}

	// authentication
	if (!Authenticate(recvData))
	{
		DisplayRequestAuth(sock);
		return 0;
	}

	CoString bufData = recvData;
	CoString reqData = recvData;
	LPTSTR HTTPBuffer = bufData.GetBuffer(bufData.GetLength() * 3);
	LPTSTR REQUEST = reqData.GetBuffer(bufData.GetLength() * 3);

	if (!HandleHttpGET(HTTPBuffer, REQUEST, recvData, recvData.GetLength(), sock) &&
			!HandleHttpPOST(HTTPBuffer, REQUEST, recvData, recvData.GetLength(), sock))
	{
		return -1;
	}

	void decodeurl(char *pEncoded);
	decodeurl(HTTPBuffer);
	decodeurl(REQUEST);

	HandleWebPages(HTTPBuffer, REQUEST, sock);

	bufData.ReleaseBuffer();
	reqData.ReleaseBuffer();

	return 0;
}

INT AcceptSocketCommand()
{
	CRITICALTRACE(oReceiveBCast, AcceptSocketCommand);

	SetCurrentDirectory(GetModuleFolder());

	SOCKET sock0 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock0 == INVALID_SOCKET)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	WORD portNo = 12082; // 管理コンソールポート(HTML)
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portNo);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock0, (sockaddr*)&addr, sizeof addr) != 0)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	if (listen(sock0, 5) != 0)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	for (; ; )
	{
		sockaddr_in client = { 0 };
		int len = sizeof sockaddr_in;
		SOCKET sock = accept(sock0, (sockaddr*)&client, &len);
		if (sock == INVALID_SOCKET)
		{
			DEBUG_OUT_DEFAULT("failed.");
			break;
		}

		OSOCKET_DATA* socketData = new OSOCKET_DATA(CommandThread, sock, client);
		HANDLE hThread = CreateThread(NULL, 0, MultiSocketThread, socketData, 0, NULL);
		CloseHandle(hThread);
	}

	return 0;
}

// 自動アップデートのパス
LRESULT GetUpdatePath(CoString& updatePath)
{
	CoString modulePath = GetModuleFolder();
	updatePath = modulePath + "update\\";
	CreateDirectory(updatePath, NULL);
	updatePath += "newst\\";
	CreateDirectory(updatePath, NULL);

	return 0;
}

LRESULT GetUpdateTextPath(CoString& updateText)
{
	CoString updatePath;
	GetUpdatePath(updatePath);
	updateText = updatePath + "update.txt";

	return 0;
}

LRESULT GetUpdateExecPath(CoString& updateExec)
{
	CoString updatePath;
	GetUpdatePath(updatePath);
	updateExec = updatePath + "update.exe";

	return 0;
}

LRESULT GetUpdateVersion(CoString& update)
{
	CoString updateText;
	GetUpdateTextPath(updateText);

	HANDLE hFile = CreateFile(updateText, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	DWORD dwReadHigh = 0;
	DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	ReadFile(hFile, update.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
	CloseHandle(hFile);

	return 0;
}

// 自動アップデートの確認
HRESULT CheckSetupUpdate()
{
	CoString masterConsoleIP;
	for (int i = 0, count = 30; i < count; i++)
	{
		// 管理端末が確認できるまで最大30秒間待機する
		MasterConsoleIP(masterConsoleIP);
		if (!masterConsoleIP.IsEmpty())
		{
			break;
		}
		Sleep(1000);
	}
	if (masterConsoleIP.IsEmpty())
	{
		// 管理端末不在
		return -1;
	}

	if (IsThisMasterConsole())
	{
		// 管理は情報収集の必要なし
		return 1;
	}

	// 最新バージョンのインストール

	CoString sendData;
	sendData.Format("TCP 06 DATA ");

	CoString update;
	GetUpdateVersion(update);

	CoString work;
	work.Format(" version \"%s\" \"%s\"", (LPCTSTR)update, GetThisVersion());
	sendData += work;

	SJIStoUTF8(sendData, sendData);

	CoString recvData;
	INT resRecv = DataSendConsole(masterConsoleIP, 12084, sendData, &recvData);

	if (resRecv == 0 && !recvData.IsEmpty())
	{
		CoString verPath;
		GetUpdateTextPath(verPath);
		CoString binPath;
		GetUpdateExecPath(binPath);

		CoString saveData;
		memcpy(saveData.GetBufferSetLength(10), recvData, 10);

		CoAutoFile f(verPath, GENERIC_ALL);
		f.Write(saveData);
		f.Close();

		DWORD thisVersion = strtoul(GetThisVersion(), NULL, 10);
		DWORD newstVersion = strtoul(saveData, NULL, 10);
		if (newstVersion > thisVersion && recvData.GetLength() > 10)
		{
			DWORD binLength = recvData.GetLength() - 10;
			memcpy(saveData.GetBufferSetLength(binLength), &((LPCTSTR)recvData)[10], binLength);

			f.Open(binPath, GENERIC_ALL);
			f.Write(saveData);
			f.Close();

		#ifndef _DEBUG
			RunProcess(binPath, "/S", 1);
			OutputViewer("TerminateProcess:%s:(%d)", __FILE__, __LINE__);
			TerminateProcess(GetCurrentProcess(), 0);
			_asm
			{
				int 3
			}
		#endif
		}
	}

	// 端末制御情報の取得

	sendData.Format("TCP 03 DATA ");

	SJIStoUTF8(sendData, sendData);

	DataSendConsole(masterConsoleIP, 12084, sendData);

	return 0;
}

// 最初の通信処理
HRESULT InitializeInterface()
{
	// ブロードキャストIPの生成
	CoString broadcastIP;
	LRESULT GetBroadcastIP(CoString& broadcastIP);
	GetBroadcastIP(broadcastIP);

	// ブロードキャストのチェック（エコー確認）
	int WakeUpConsole(LPCTSTR deststr, WORD portno, LPCTSTR text);
	WakeUpConsole(broadcastIP, 12083, "CHECK BROADCAST COMMAND ");

	return 0;
}

// 起動時の情報収集
HRESULT FirstCallConsole()
{
	// 自動アップデートの確認
	CheckSetupUpdate();

	return 0;
}

// 最初のサーバ通信
LRESULT AvertissementLicense(CoString& newLicData)
{
	COleDateTime licDate = GetExpireLicense();

	OHTTP_DATA httpData;
	httpData.target = HTTP_PLUSTAR[8]; // ライセンス更新
	httpData.sendData.Format("data=avertissement license unique code.&unique=%s&date=%s&version=%s",
		GetShopUnique(), (LPCTSTR)licDate.Format("%Y/%m/%d"), GetThisVersion());

	HttpPost(&httpData);
	if (httpData.recvData.Find("AVERTISSEMENT SUCCEEDED ") == -1)
	{
		DEBUG_OUT_DEFAULT((LPCTSTR)httpData.recvData);
		return -1;
	}

	INT pos = httpData.recvData.Find("NEW LICENSE DATA ");
	if (pos != -1)
	{
		newLicData = oAbstractSearchSubstance(httpData.recvData, pos);
	}

	return 0;
}

struct VERSION_UPDATE_DATA
{
	CoString execute;
	CoString clientServer;
	CoString version;
};

#define VUD_SERVER (0)
#define VUD_CLIENT (1)
#define VUD_UTIL (2)

// 最新バージョンの取得
VERSION_UPDATE_DATA* GetVersionUpdateData()
{
	static VERSION_UPDATE_DATA versionUpdateData[3];

	return versionUpdateData;
}

// 最新モジュールの取得（管理のみ）
LRESULT GetUpdateModule()
{
	static const CoString versionUpdate = "version_update.php";

	CoString updatePath;
	GetUpdatePath(updatePath);

	CoString existVersion;
	CoAutoFile f(updatePath + versionUpdate);
	f.Read(existVersion);
	f.Close();

	VERSION_UPDATE_DATA* versionExsistData = GetVersionUpdateData();

	INT pos = 0;
	if (!existVersion.IsEmpty())
	{
		for (int i = 0; i < 3; i++)
		{
			versionExsistData[i].version = existVersion.AbstractSearchSubstance(pos);
			versionExsistData[i].clientServer = existVersion.AbstractSearchSubstance(pos);
			versionExsistData[i].execute = existVersion.AbstractSearchSubstance(pos);
		}
	}

	static const CoString urlPath = HTTP_PLUSTAR[7]; // アップデート置き場

	OHTTP_DATA httpData;
	httpData.target = urlPath + versionUpdate;
	HttpPost(&httpData);

	if (httpData.recvData.IsEmpty() || memcmp(httpData.headerData, "HTTP/1.1 404 Not Found", strlen("HTTP/1.1 404 Not Found")) == 0 || memcmp(httpData.headerData, "HTTP/1.0 404 Not Found", strlen("HTTP/1.0 404 Not Found")) == 0)
	{
		return 0;
	}

	CoString newstVersion = httpData.recvData;

	if (newstVersion == existVersion)
	{
		return 0;
	}

	VERSION_UPDATE_DATA newstData[3];

	pos = 0;
	for (int i = 0; i < 3; i++)
	{
		newstData[i].version = newstVersion.AbstractSearchSubstance(pos);
		newstData[i].clientServer = newstVersion.AbstractSearchSubstance(pos);
		newstData[i].execute = newstVersion.AbstractSearchSubstance(pos);
	}

	INT nError = 0;

	pos = 0;
	for (int i = 0; i < 3; i++)
	{
		if (newstData[i].version > versionExsistData[i].version)
		{
			httpData.target = urlPath + newstData[i].execute;
			HttpPost(&httpData);

			if (httpData.recvData.IsEmpty()
				|| memcmp(httpData.headerData, "HTTP/1.1 404 Not Found", strlen("HTTP/1.1 404 Not Found")) == 0
				|| memcmp(httpData.headerData, "HTTP/1.0 404 Not Found", strlen("HTTP/1.0 404 Not Found")) == 0)
			{
				nError++;
				continue;
			}

			f.Open(updatePath + newstData[i].execute, GENERIC_ALL);
			f.Write(httpData.recvData);
			f.Close();
		}
	}

	if (nError == 0)
	{
		f.Open(updatePath + versionUpdate, GENERIC_ALL);
		f.Write(newstVersion);
		f.Close();

		// 自動アップデート
		UINT existVer = strtoul(versionExsistData[VUD_SERVER].version, NULL, 10);
		UINT newstVer = strtoul(newstData[VUD_SERVER].version, NULL, 10);
		UINT thisVer = strtoul(GetThisVersion(), NULL, 10);

		if (existVer < newstVer && thisVer < newstVer)
		{
			CoString binPath = updatePath + newstData[VUD_SERVER].execute;

		#ifndef _DEBUG
			RunProcess(binPath, "/S", 1);
			OutputViewer("TerminateProcess:%s:(%d)", __FILE__, __LINE__);
			TerminateProcess(GetCurrentProcess(), 0);
			_asm
			{
				int 3
			}
		#endif
		}

		versionExsistData[VUD_SERVER] = newstData[VUD_SERVER];
		versionExsistData[VUD_CLIENT] = newstData[VUD_CLIENT];
		versionExsistData[VUD_UTIL] = newstData[VUD_UTIL];
	}

	return 0;
}

struct REPORT_USE_DATA
{
	CoString kind;
	CoString data;
};

// 管理画面への通知処理
DWORD WINAPI ReportUseActionThread(LPVOID pParam)
{
	CRITICALTRACE(oReceiveBCast, ReportUseAction);

	CWSAInterface wsa;

	REPORT_USE_DATA* pData = (REPORT_USE_DATA*)pParam;

	class CTrash
	{
	protected:
		REPORT_USE_DATA* m_pData;
	public:
		CTrash(REPORT_USE_DATA* pData)
			: m_pData(pData)
		{
		}
		virtual ~CTrash()
		{
			delete m_pData;
		}
	};

	CTrash trash(pData);

	// kind
	//   "pc_stop" 個別PCの自動停止 (pc)
	//   "ad_stop" 管理設定の自動停止 (admin)
	//   "se_stop" サーバ設定の自動停止 (server)

	COMMAND_CTRL_DATA cmdData;
	GetCommandCtrlData(cmdData);

	CoString masterConsoleIP;
	MasterConsoleIP(masterConsoleIP);
	if (masterConsoleIP.IsEmpty())
	{
		return -1;
	}

	COleDateTime now = COleDateTime::GetCurrentTime();

	CoString sendData;
	CoString work;

	sendData.Format("TCP 05 DATA ");

	// 現在のテキスト形式を
	// [2009/04/15 23:22:38] PS20 (192.168.0.30 ) Winny.exe
	// 変更後
	// [ {"dy": "2009/04/15 23:22:38", "hn": "PS34", "ip": "192.168.0.34", "sn": "Winny.exe", "kg": "se_stop" },
	//   {"dy": "2009/04/15 23:22:38", "hn": "PS55", "ip": "192.168.0.55", "sn": "Winny.exe", "kg": "ad_stop" }, ]

	work = now.Format("{ \"dy\": \"%Y/%m/%d %H:%M:%S\", ");
	sendData += work;
	work.Format("\"hn\": \"%-20s\", ", (LPCTSTR)cmdData.hostname);
	sendData += work;
	work.Format("\"ip\": \"%-15s\", ", (LPCTSTR)cmdData.ipaddr);
	sendData += work;
	work.Format("\"sn\": \"%s\", ", (LPCTSTR)pData->data);
	sendData += work;
	work.Format("\"kg\": \"%s\" }, ", (LPCTSTR)pData->kind);
	sendData += work;

	SJIStoUTF8(sendData, sendData);

	DataSendConsole(masterConsoleIP, 12084, sendData);

	return 0;
}

// 管理画面への通知処理
HRESULT ReportUseAction(LPCTSTR kind, LPCTSTR data)
{
	REPORT_USE_DATA* pData = new REPORT_USE_DATA;
	pData->kind = kind;
	pData->data = data;

	HANDLE hThread = CreateThread(NULL, 0, ReportUseActionThread, pData, 0, NULL);
	CloseHandle(hThread);

	return 0;
}

// 自動禁止リストの取得
LRESULT TransactionSuppresAutoList()
{
	// exsample "(POST) http://anaj.exsample.jp/phsa.php?sid=ZXZXZXZXZX&ver=00012"

	CoString uniq_text = GetShopUnique();

	CoString resData;
	DWORD resVer = 0;
	GetSuppresAutoList(resData, resVer);

	OHTTP_DATA httpData;
	httpData.target = GetSuppresAutoListURL();
	httpData.sendData.Format("data=suppres auto list ver100&sid=%s&ver=%010d",
			(LPCTSTR)uniq_text, resVer);

	HttpPost(&httpData);

	CoString data_ver = httpData.recvData.CString::Mid(0, 10);
	DWORD val_ver = strtoul(data_ver, NULL, 10);
	if (val_ver == 0)
	{
		DEBUG_OUT_DEFAULT((LPCTSTR)httpData.recvData);
		return -1;
	}

	CoString data_len = httpData.recvData.CString::Mid(12, 10);
	DWORD val_len = strtoul(data_len, NULL, 10);
	if (val_ver == resVer && val_len == 0)
	{
		return 0;
	}
	else if (val_len == 0)
	{
		DEBUG_OUT_DEFAULT((LPCTSTR)httpData.recvData);
		return 0;
	}
	CoString setData = httpData.recvData.CString::Mid(24);

	// プラチナライセンスのみ
	if (!IsPlatinum())
	{
		DWORD nulVer = 0;
		CoString nulData;
		SetSuppresAutoList(nulData, nulVer);
		return 0;
	}

	setData.Replace("\r\n", "\\,");
	SetSuppresAutoList(setData, val_ver);

	return 0;
}

// 自動hostsの取得
LRESULT TransactionHostsAutoList()
{
	// exsample "(POST) http://anaj.exsample.jp/phha.php?sid=ZXZXZXZXZX&ver=00012"

	CoString uniq_text = GetShopUnique();

	CoString resData;
	DWORD resVer = 0;
	GetHostsAutoList(resData, resVer);

	OHTTP_DATA httpData;
	httpData.target = GetHostsAutoListURL();
	httpData.sendData.Format("data=hosts auto list ver100&sid=%s&ver=%010d",
			(LPCTSTR)uniq_text, resVer);

	HttpPost(&httpData);

	CoString data_ver = httpData.recvData.CString::Mid(0, 10);
	DWORD val_ver = strtoul(data_ver, NULL, 10);
	if (val_ver == 0)
	{
		DEBUG_OUT_DEFAULT((LPCTSTR)httpData.recvData);
		return -1;
	}

	CoString data_len = httpData.recvData.CString::Mid(12, 10);
	DWORD val_len = strtoul(data_len, NULL, 10);
	if (val_ver == resVer && val_len == 0)
	{
		return 0;
	}
	else if (val_len == 0)
	{
		DEBUG_OUT_DEFAULT((LPCTSTR)httpData.recvData);
		return 0;
	}
	CoString setData = httpData.recvData.CString::Mid(24);

	// プラチナライセンスのみ
	if (!IsPlatinum())
	{
		DWORD nulVer = 0;
		CoString nulData;
		SetHostsAutoList(nulData, nulVer);
		return 0;
	}

	setData.Replace("\r\n", "\\,");
	SetHostsAutoList(setData, val_ver);

	return 0;
}

// 停止プロセスの通知
LRESULT TransactionHistoryTerminateProcess(LPCTSTR resData)
{
	// exsample "(POST) http://anaj.exsample.jp/phtp.php?sid=ZXZXZXZXZX&report=[ { "pn": "winny.exe" } ]"

	CoString uniq_text = GetShopUnique();
	CoString nc_mailadd; // メール送信先アドレス

	CoString shop = GetShopLicense(); // 店舗名
	SJIStoUTF8(shop, shop);

	// メールの送信有無
	DWORD notice_mail = MyGetProfileDWORD(P("Setting"), P("nc_mail"), 0);
	if (notice_mail == 1)
	{
		// メール送信先アドレス
		MyGetProfileString(P("Setting"), P("nc_mailadd"), nc_mailadd.GetBuffer(2048 + 1), 2048);
		nc_mailadd.ReleaseBuffer();
	}

	OHTTP_DATA httpData;
	httpData.target = GetHistoryTerminateProcessURL();

	httpData.sendData.Format("data=history terminate process ver100&sid=%s&report=%s&store=%s&mail=%s",
			(LPCTSTR)uniq_text, (LPCTSTR)resData, (LPCTSTR)shop, (LPCTSTR)nc_mailadd);

	HttpPost(&httpData);

	if (memcmp(httpData.recvData, "OK", strlen("OK") != 0))
	{
		DEBUG_OUT_DEFAULT((LPCTSTR)httpData.recvData);
		return -1;
	}

	return 0;
}

// 管理に通知
LRESULT PopupTerminateProcess(LPCTSTR resData)
{
	// プラチナライセンスのみ
	if (!IsPlatinum())
	{
		return 0;
	}

	// 警告ポプアップの表示
	DWORD notice_pop = MyGetProfileDWORD(P("Setting"), P("nc_pop"), 1); // デフォルトON
	if (notice_pop != 1)
	{
		return 0;
	}

	// 停止プロセス名
	INT pos = 0;
	CoString json = resData;
	CoString cl_time = json.AbstractSearchSubstance(pos);
	cl_time = json.AbstractSearchSubstance(pos);
	CoString cl_name = json.AbstractSearchSubstance(pos);
	cl_name = json.AbstractSearchSubstance(pos);
	CoString cl_ip = json.AbstractSearchSubstance(pos);
	cl_ip = json.AbstractSearchSubstance(pos);
	CoString cl_proc = json.AbstractSearchSubstance(pos);
	cl_proc = json.AbstractSearchSubstance(pos);

	UTF8toSJIS(cl_proc, cl_proc);
	UTF8toSJIS(cl_name, cl_name);

	CoString text;
	text.Format("プロセスを自動停止しました。\r\n\r\n"
		"停止日時：%s\r\n"
		"マシン名：%s\r\n"
		"IPアドレス：%s\r\n"
		"停止プロセス：%s\r\n\r\n"
		, cl_time, cl_name, cl_ip, cl_proc);

	// alertポップアップ表示
	//
	// status
	// 0 DEFAULT
	// 1 ATTENTION
	// 2 CAUTION
	// 3 ALERT
	// 4 EMERGENCY
	// 5 WARNING
	// 6 ERROR
	// 7 FAILED
	//
	//PopupMessageWindow(LPCTSTR lpszText, 
	//									 LPCTSTR lpszLink = "",
	//									 DWORD color = 2,
	//									 DWORD width = 300,
	//									 DWORD height = 300,
	//									 DWORD status = 0,
	//									 DWORD showSeconds = 0);
	PopupMessageWindow(text, P("http://localhost:12082/"), 1, 300, 200, 0, 120);

	return 0;
}

// 管理設定の自動停止リストのアップロード
LRESULT UploadSuppressionListData()
{
	// exsample "(POST) http://anaj.exsample.jp/pspl.php?sid=ZXZXZXZXZX&report=[ { "pn": "winny.exe" } ]"

	CoString resData;
	GetSuppressionList(resData);

	CoStringArray value;
	GetArrayValue(value, resData, "\\,");

#ifdef USE_JSONCPP
	Json::Value *jVal = create_AValue();
#else
	CoString work;
	CoString jData = "[ ";
#endif

	for (int i = 0, count = value.GetSize(); i < count; i++)
	{
#ifdef USE_JSONCPP
		(*jVal)[i]["pn"] = (const char *)value[i];
#else
		work.Format("{ \"pn\": \"%s\" }, \r\n", (LPCTSTR)value[i]);
		jData += work;
#endif
	}

#ifdef USE_JSONCPP
	CoString jData = jVal->toFastString().c_str();
	jVal->release();
#else
	int jLength = jData.GetLength();
	if (jLength >= 4)
	{
		jData.GetBufferSetLength(jLength - 4);
	}
	jData += " ]";
#endif

	CoString uniq_text = GetShopUnique();

	OHTTP_DATA httpData;
	httpData.target = GetStopProcessListURL();
	httpData.sendData.Format("data=stop process list ver100&sid=%s&report=%s",
			(LPCTSTR)uniq_text, (LPCTSTR)jData);

	HttpPost(&httpData);

	if (memcmp(httpData.recvData, "OK", strlen("OK") != 0))
	{
		DEBUG_OUT_DEFAULT((LPCTSTR)httpData.recvData);
		return -1;
	}

	return 0;
}

// 店内マシン情報のアップロード
LRESULT UploadHardInfoListData()
{
	CoString text;

	CoString modulePath = GetModuleFolder();
	CoString reportPath = modulePath + "pHardware.dat";
	HANDLE hFile = CreateFile(reportPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwReadHigh = 0;
		DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
		ReadFile(hFile, text.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);

		CloseHandle(hFile);
	}

	if (text.IsEmpty())
	{
		return 0;
	}

	CoString jData = "[ " + text.Left(text.GetLength() - 3) + " ]";

	CoString uniq_text = GetShopUnique();

	OHTTP_DATA httpData;
	httpData.target = GetHardwareURL();
	httpData.sendData.Format("data=hardware machine list ver100&sid=%s&report=%s",
			(LPCTSTR)uniq_text, (LPCTSTR)jData);

	HttpPost(&httpData);

	if (memcmp(httpData.recvData, "OK", strlen("OK") != 0))
	{
		DEBUG_OUT_DEFAULT((LPCTSTR)httpData.recvData);
		return -1;
	}

	return 0;
}

// 印刷情報のアップロード
LRESULT UploadPrintingData(CoString& jData)
{
	CoString uniq_text = GetShopUnique();

	OHTTP_DATA httpData;
	httpData.target = HTTP_PLUSTAR[13];
	httpData.sendData.Format("data=printing data ver100&sid=%s&report=%s",
			(LPCTSTR)uniq_text, (LPCTSTR)jData);

	HttpPost(&httpData);

	if (memcmp(httpData.recvData, "OK", strlen("OK") != 0))
	{
		DEBUG_OUT_DEFAULT((LPCTSTR)httpData.recvData);
		return -1;
	}

	return 0;
}

HRESULT CatchStandardData(const CoString& recvData)
{
	// PC基本情報の受信
	INT pos = 0;
	UPDATE_COMSOLE_DATA data;
	data.macaddr = recvData.AbstractSearchSubstance(pos);
	data.ipaddr = recvData.AbstractSearchSubstance(pos);
	data.hostname = recvData.AbstractSearchSubstance(pos);
	data.status = recvData.AbstractSearchSubstance(pos);

	UpdateConsoleData(data);

	return 0;
}

HRESULT CatchBasicData(const CoString& recvData)
{
	// PC追加情報の受信
	INT pos = 0;
	UPDATE_COMSOLE_DATA data;
	data.macaddr = recvData.AbstractSearchSubstance(pos);
	data.username = recvData.AbstractSearchSubstance(pos);
	data.boot_time = recvData.AbstractSearchSubstance(pos);
	data.time_span = recvData.AbstractSearchSubstance(pos);
	data.cpu_usage = recvData.AbstractSearchSubstance(pos);
	data.process = recvData.AbstractSearchSubstance(pos);
	data.dwMemoryLoad = recvData.AbstractSearchSubstance(pos);
	data.dwTotalPhys = recvData.AbstractSearchSubstance(pos);
	data.dwAvailPhys = recvData.AbstractSearchSubstance(pos);
	data.dwTotalVirtual = recvData.AbstractSearchSubstance(pos);
	data.dwAvailVirtual = recvData.AbstractSearchSubstance(pos);
	data.ulFreeBytesAvailable = recvData.AbstractSearchSubstance(pos);
	data.ulTotalNumberOfBytes = recvData.AbstractSearchSubstance(pos);
	data.ulTotalNumberOfFreeBytes = recvData.AbstractSearchSubstance(pos);
	data.in_packet = recvData.AbstractSearchSubstance(pos);
	data.out_packet = recvData.AbstractSearchSubstance(pos);
	data.connections = recvData.AbstractSearchSubstance(pos);

	UpdateConsoleData(data);

	return 0;
}

HRESULT CatchSuppressionData(const CoString& recvData)
{
	CoString data;

	// 起動禁止リスト配信
	GetSuppressionList(data);
	BroadcastSpecialData("SET SUPPRESSION COMMAND ", data);

	// 自動禁止リスト配信
	DWORD resVer = 0;
	GetSuppresAutoList(data, resVer);
	BroadcastSpecialData("SET SUPPRES AUTO COMMAND ", data);

	// 自動hosts配信
	resVer = 0;
	GetHostsAutoList(data, resVer);
	CoString sendData;
	sendData.Format("%010d%s", resVer, (LPCTSTR)data);
	BroadcastSpecialData("SET HOSTS AUTO COMMAND ", sendData);

	return 0;
}

// 受信した自動停止リポートの保存
HRESULT AddedTerminateProcess(const CoString& reportData)
{
	CoString modulePath = GetModuleFolder();
	CoString reportPath = modulePath + "pExTrapperReport.txt";
	HANDLE hFile = INVALID_HANDLE_VALUE; // CreateFile(reportPath, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	NativityFile(reportPath, 5, FALSE, &hFile);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwReadHigh = 0;
		DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
		CoString text;
		ReadFile(hFile, text.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);

		LARGE_INTEGER num = { 0 };
		SetFilePointerEx(hFile, num, NULL, FILE_BEGIN);

		int line = text.Replace(RETERCODE, RETERCODE);
		int prune = line - 500 + 1;
		if (prune > 0)
		{
			int pos = 0;
			for (; prune; prune--)
			{
				pos = text.Find(RETERCODE, pos);
				pos += strlen(RETERCODE);
			}
			text = text.Right(text.GetLength() - pos);
		}

		DWORD dwSize = 0;
		WriteFile(hFile, text, text.GetLength(), &dwSize, NULL);
		WriteFile(hFile, reportData, reportData.GetLength(), &dwSize, NULL);
		WriteFile(hFile, RETERCODE, (DWORD)strlen(RETERCODE), &dwSize, NULL);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
	}

	return 0;
}

// 自動停止リポートの受信
HRESULT CatchReportData(const CoString& recvData)
{
	CoString reportData = recvData.Right(recvData.GetLength() - strlen("TCP 05 DATA "));

	// 受信した自動停止リポートの保存
	AddedTerminateProcess(reportData);

	// JSON作成
	CoString jData = "[ ";
	jData += reportData;
	int jLength = jData.GetLength();
	if (jLength >= 2)
	{
		jData.GetBufferSetLength(jLength - 2);
	}
	jData += " ]";

	// サーバに送信
	TransactionHistoryTerminateProcess(jData);

	// 管理に通知
	PopupTerminateProcess(jData);

	return 0;
}

void CheckHardwareChange(const CoString &nowHard, const CoString &oldHard)
{
	CoStringArray now, old;
	enum {e_HOST, e_IP, e_MAC, e_GRABO, e_MEM, e_CPU};

	//{ "hostname":"xxx","ipaddr":"xxx.xxx.xxx.xxx","macaddr":"xx:xx:xx:xx:xx:xx","grabo":"xxx","mem":"xxx","cpu":"xxx"},
	for ( int pos1 = 0, pos2= 0, i = e_HOST; i <= e_CPU;i++ ) {
		nowHard.AbstractSearchSubstance(pos1);
		oldHard.AbstractSearchSubstance(pos2);

		now.Add(nowHard.AbstractSearchSubstance(pos1));
		old.Add(oldHard.AbstractSearchSubstance(pos2));
	}

	CoString msg, hardwareType;
	bool isChange = false;
	int  change_count = 0;

	for (int i = e_GRABO; i <= e_CPU; i++) {
		if ( now.GetAt(i) != old.GetAt(i) ) {
			if( isChange == false) {
				msg = "マシン構成が変更されています。\n\nマシン名: " + now.GetAt(e_HOST) + "\n";
			}

			switch (i) {
				case e_GRABO: hardwareType = "グラボ"; break;
				case e_MEM:   hardwareType = "メモリ"; break;
				case e_CPU:   hardwareType = "ＣＰＵ"; break;
			}

			msg += "種別: " + hardwareType + "\n";
			
			// 連続したスペースを半角スペース1つに
			// 気持ち頭の半角は取りたいな。。
			now.GetAt(i).Replace("  ", "");
			old.GetAt(i).Replace("  ", "");

			msg += "現在: " + now.GetAt(i) + "\n";
			msg += "以前: " + old.GetAt(i) + "\n";

			isChange = true;

			// エラー数に応じて高さを決める
			change_count++;
		}
	}

	if (isChange) {
		// 警告ポップアップの高さはデフォ300px
		int popup_height = 200;
		popup_height += 75*(change_count-1);
		PopupMessageWindow(msg, P("http://localhost:12082/"), 1, 300, popup_height, 4);
	}
}

// 受信したマシン構成の保存
HRESULT CatchHardwareData(const CoString& recvData)
{
	CoString hardwareData = recvData.Right(recvData.GetLength() - strlen("TCP 07 DATA "));

	CoString modulePath = GetModuleFolder();
	CoString reportPath = modulePath + "pHardware.dat";
	HANDLE hFile = CreateFile(reportPath, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwReadHigh = 0;
		DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
		CoString text;
		ReadFile(hFile, text.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);

		LARGE_INTEGER num = { 0 };
		SetFilePointerEx(hFile, num, NULL, FILE_BEGIN);

		//{ "hostname":"xxx","ipaddr":"xxx.xxx.xxx.xxx","macaddr":"xx:xx:xx:xx:xx:xx","grabo":"xxx","mem":"xxx","cpu":"xxx"},
		INT pos = 0;
		CoString macaddr;
		pos = hardwareData.Find(",\"macaddr\":\"");
		macaddr = hardwareData.AbstractSearchSubstance(pos, ",");

		INT head, tail;
		CoString oldHard;
		bool isNewPC = true;
		head = text.Find(macaddr);
		if ( head >= 0 ) {
			head = text.ReverseFind("\n" , head);
			tail = text.Find("\n" , head + 1);
			oldHard = text.Mid(head, tail - head);
			text.Delete(head, tail - head);
			isNewPC = false;
		}

		DWORD dwSize = 0;
		WriteFile(hFile, text, text.GetLength(), &dwSize, NULL);
		WriteFile(hFile, hardwareData, hardwareData.GetLength(), &dwSize, NULL);
		WriteFile(hFile, RETERCODE, (DWORD)strlen(RETERCODE), &dwSize, NULL);
		SetEndOfFile(hFile);
		CloseHandle(hFile);

		if (!isNewPC) {
			CheckHardwareChange(hardwareData, oldHard);
		}
	}

	return 0;
}

// 自動アップデート
HRESULT AutoUpdateData(const CoString& clientAddr, const CoString& recvData, CoString& moduleData)
{
	VERSION_UPDATE_DATA* versionUpdateData = GetVersionUpdateData();

	INT pos = 0;
	CoString clientExsistVersion = recvData.AbstractSearchSubstance(pos);
	CoString clientVersion = recvData.AbstractSearchSubstance(pos);

	UINT existVer = strtoul(clientExsistVersion, NULL, 10);
	UINT thisVer = strtoul(clientVersion, NULL, 10);

	UINT newstVer = strtoul(versionUpdateData[VUD_CLIENT].version, NULL, 10);

	moduleData = versionUpdateData[VUD_CLIENT].version;

	if (thisVer >= newstVer)
	{
		// 最新なので更新の必要なし
		return 1;
	}

	CoString updatePath;
	GetUpdatePath(updatePath);

	CoString binData;
	CoAutoFile f(updatePath + versionUpdateData[VUD_CLIENT].execute);
	if (!f.IsOpen())
	{
		return -1;
	}
	f.Read(binData);
	f.Close();

	moduleData += binData;

	return 0;
}

// PCシャットダウン時の処理
LRESULT CatchMachineShutdown()
{
	// TODO: ここで情報を管理に送信
	// OutputViewer("CatchMachineShutdown: send analyze");

	// PC電源を切ろうとしています

	// フォアグラウンドアナライズ情報を管理端末に送信
	LRESULT SendAnalyzeForeData();
	SendAnalyzeForeData();

	// プロセスアナライズ情報を管理端末に送信
	LRESULT SendAnalyzeProcessData();
	SendAnalyzeProcessData();

	return 0;
}

// socket thread
DWORD WINAPI IntegratedThread(LPVOID pParam)
{
	CRITICALTRACE(oReceiveBCast, IntegratedThread);

	CWSAInterface wsa;

	SOCKET sock = (*(OSOCKET_DATA*)pParam).socket;

	CoAutoSocket autoSocket(sock);

	CoString clientAddr = inet_ntoa((*(OSOCKET_DATA*)pParam).sockaddr.sin_addr);

	CoString msgstr, text, result;

	// read
	CoString recvData;
	LRESULT resRecv = TransRecvTCP(sock, recvData);

	if (resRecv != 0)
	{
		// 通信障害
		DEBUG_OUT_DEFAULT("ERROR: network trans failed.");
		return -1;
	}

	if (!IsThisMasterConsole())
	{
		// 管理ではない
		DEBUG_OUT_DEFAULT("ERROR: duplicate console failed.");
		return -1;
	}

	if (strncmp(recvData, P("UPLOAD ANALYZE."), strlen(P("UPLOAD ANALYZE."))) == 0)
	{
		// 情報の保存
		CatchMachineShutdown();

		Sleep(9 * 1000);

		// フォアグラウンドアナライズ情報のアップロード
		LRESULT UploadAnalyzeForeData();
		UploadAnalyzeForeData();

		// プロセスアナライズ情報のアップロード
		LRESULT UploadAnalyzeProcessData();
		UploadAnalyzeProcessData();

		send(sock, P("SO UPLOAD ANALYZE."), (int)strlen(P("SO UPLOAD ANALYZE.")), 0);
	}
	else if (strncmp(recvData, P("TERMINATE CHECKING."), strlen(P("TERMINATE CHECKING."))) == 0)
	{
		// 情報の保存
		CatchMachineShutdown();

		send(sock, P("SO FINALY."), (int)strlen(P("SO FINALY.")), 0);

		PostQuitMessage(0);
		OutputViewer("TerminateProcess:%s:(%d)", __FILE__, __LINE__);
		TerminateProcess(GetCurrentProcess(), 0);
		_asm
		{
			int 3
		}
	}
	else if (strncmp(recvData, P("LIFECHECK CHECKING."), strlen(P("LIFECHECK CHECKING."))) == 0)
	{
		send(sock, P("SO FUNNY LIFE."), (int)strlen(P("SO FUNNY LIFE.")), 0);
	}
	else if (strncmp(recvData, "TCP 01 DATA ", strlen("TCP 01 DATA ")) == 0)
	{
		CatchStandardData(recvData);
	}
	else if (strncmp(recvData, "TCP 02 DATA ", strlen("TCP 02 DATA ")) == 0)
	{
		CatchBasicData(recvData);
	}
	else if (strncmp(recvData, "TCP 03 DATA ", strlen("TCP 03 DATA ")) == 0)
	{
		CatchSuppressionData(recvData);
	}
	else if (strncmp(recvData, "TCP 04 DATA ", strlen("TCP 04 DATA ")) == 0)
	{
		CoString sendData;
		HRESULT CatchAnalyzeProcessData(const CoString& recvData);
		if (CatchAnalyzeProcessData(recvData) == 0)
		{
			sendData.Format("TRANSLATE 04 DATA SUCCEEDED (%s)", (LPCTSTR)clientAddr);
		}
		send(sock, sendData, sendData.GetLength(), 0);
	}
	else if (strncmp(recvData, "TCP 14 DATA ", strlen("TCP 14 DATA ")) == 0)
	{
		CoString sendData;
		HRESULT CatchAnalyzeForeData(const CoString& recvData);
		if (CatchAnalyzeForeData(recvData) == 0)
		{
			sendData.Format("TRANSLATE 14 DATA SUCCEEDED (%s)", (LPCTSTR)clientAddr);
		}
		send(sock, sendData, sendData.GetLength(), 0);
	}
	else if (strncmp(recvData, "TCP 05 DATA ", strlen("TCP 05 DATA ")) == 0)
	{
		CatchReportData(recvData);
	}
	else if (strncmp(recvData, "TCP 06 DATA ", strlen("TCP 06 DATA ")) == 0)
	{
		// 自動アップデート（クライアント）
		CoString sendData;
		LRESULT result = AutoUpdateData(clientAddr, recvData, sendData);
		if (result == 1)
		{
			P("最新です");
		}
		else if (result == -1)
		{
			P("エラー");
		}
		send(sock, sendData, sendData.GetLength(), 0);
	}
	else if (strncmp(recvData, "TCP 07 DATA ", strlen("TCP 07 DATA ")) == 0)
	{
		// ハードウェア構成の自動取得
		CatchHardwareData(recvData);
	}
	else
	{
		DEBUG_OUT_DEFAULT("ERROR: miss_match tcp data failed.");
	}

	return 0;
}

INT AcceptSocketIntegrated()
{
	CRITICALTRACE(oReceiveBCast, AcceptSocketIntegrated);

	SOCKET sock0 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock0 == INVALID_SOCKET)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	WORD portNo = 12084; // リソース情報ポート
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portNo);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock0, (sockaddr*)&addr, sizeof addr) != 0)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	if (listen(sock0, 5) != 0)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	for (; ; )
	{
		sockaddr_in client = { 0 };
		int len = sizeof sockaddr_in;
		SOCKET sock = accept(sock0, (sockaddr*)&client, &len);
		if (sock == INVALID_SOCKET)
		{
			DEBUG_OUT_DEFAULT("failed.");
			break;
		}

		OSOCKET_DATA* socketData = new OSOCKET_DATA(IntegratedThread, sock, client);
		HANDLE hThread = CreateThread(NULL, 0, MultiSocketThread, socketData, 0, NULL);
		CloseHandle(hThread);
	}

	return 0;
}

// PCの終了を管理端末に送信
LRESULT SendOffdutyPC()
{
	CoString masterConsoleIP;
	MasterConsoleIP(masterConsoleIP);
	if (masterConsoleIP.IsEmpty())
	{
		return -1;
	}

	COMMAND_CTRL_DATA cmdData;
	GetCommandCtrlData(cmdData);
	if (IsThisMasterConsole())
	{
		return -1;
	}

	CoString sendData;
	CoString work;
	sendData.Format("TCP 01 DATA "); // OFFDUTY
	work.Format(" macaddr \"%s\"", (LPCTSTR)cmdData.macaddr);
	sendData += work;
	work.Format(" ipaddr \"%s\"", (LPCTSTR)cmdData.ipaddr);
	sendData += work;
	work.Format(" hostname \"%s\"", (LPCTSTR)cmdData.hostname);
	sendData += work;
	work.Format(" status \"OFFDUTY\"");
	sendData += work;

	SJIStoUTF8(sendData, sendData);

	DataSendConsole(masterConsoleIP, 12084, sendData);

	return 0;
}

// マシン構成を管理コンソールに送信
LRESULT SendHardwareData()
{
	CoString masterConsoleIP;
	MasterConsoleIP(masterConsoleIP);
	if (masterConsoleIP.IsEmpty())
	{
		return -1;
	}

	COMMAND_CTRL_DATA cmdData;
	GetCommandCtrlData(cmdData);
	CoString sendData;
	CoString work;
	sendData.Format("TCP 07 DATA ");

	INT GetHardwareInformation(CoString& textData);
	CoString str;
	GetHardwareInformation(str);

	INT pos = 0;
	CoString cpuName, graboName;
	str.AbstractSearchSubstance(pos, ":");
	cpuName = str.AbstractSearchSubstance(pos);
	str.AbstractSearchSubstance(pos, ":");
	graboName = str.AbstractSearchSubstance(pos);

	sendData += "{";
	work.Format(" \"hostname\":\"%s\"", (LPCTSTR)cmdData.hostname);
	sendData += work;
	work.Format(",\"ipaddr\":\"%s\"", (LPCTSTR)cmdData.ipaddr);
	sendData += work;
	work.Format(",\"macaddr\":\"%s\"", (LPCTSTR)cmdData.macaddr);
	sendData += work;
	work.Format(",\"grabo\":\"%s\"", graboName);
	sendData += work;
	work.Format(",\"mem\":\"%u\"", cmdData.info_mem.dwTotalPhys);
	sendData += work;
	work.Format(",\"cpu\":\"%s\"", cpuName);
	sendData += work;
	sendData += "},";

	SJIStoUTF8(sendData, sendData);

	if (IsThisMasterConsole())
	{
		CatchHardwareData(sendData);
	}
    else
	{
		DataSendConsole(masterConsoleIP, 12084, sendData);
	}

	return 0;
}

// 拡張コマンド監視スレッド
DWORD WINAPI ListenExtentsCommandThread(LPVOID pParam)
{
	CoString modulePath = GetModuleFolder();
	CoString cmdPath = modulePath + "pLook.commander";

	for (; ; )
	{
		if (PregnancyFile(cmdPath, INFINITE) != 0)
		{
			continue;
		}

		HANDLE hFile = NULL;
		if (NativityFile(cmdPath, 1, TRUE, &hFile) == 0)
		{
			DWORD dwReadHigh = 0;
			DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
			CoString buffer;
			ReadFile(hFile, buffer.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
			CloseHandle(hFile);
			DeleteFile(cmdPath);

			if (strncmp(buffer, "popupmessage:", strlen("popupmessage:")) == 0)
			{
				CoString popMsg;
				base64_encode(buffer, popMsg);
				BroadcastSpecialData("SHOW POPUP MESSAGE COMMAND ", popMsg);

				Sleep(1000);
			}
			else if (strncmp(buffer, "popuptestmessage:", strlen("popuptestmessage:")) == 0)
			{
				CoString recvData;
				DataSendConsole("localhost", 12087, buffer, &recvData);
			}
			else if (strncmp(buffer, "setupupdatenow:", strlen("setupupdatenow:")) == 0)
			{
				BroadcastSpecialData("CHECK SETUP UPDATE COMMAND ", "");

				Sleep(1000);
			}
		}
		Sleep(1000);
	}

	return TRUE;
}

INT AcceptSocketBroadcast()
{
	SOCKET sock0 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock0 == INVALID_SOCKET)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	WORD portNo = 12083; // ブロードキャスト受信ポート(UDP)
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portNo);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock0, (sockaddr*)&addr, sizeof addr) != 0)
	{
		DEBUG_OUT_DEFAULT("ERROR: bind failed.");
		return -1;
	}

	for (; ; )
	{
		CoString recvData;
		if (TransRecvUDP(sock0, recvData) != 0)
		{
			continue;
		}

		if (recvData.IsEmpty())
		{
#ifdef _DEBUG
			_asm
			{
				int 3
			}
#endif
			continue;
		}

		// LINUXからのリソース情報の要求（管理を含む）
		if (strncmp(recvData, "GET CONSOLE LINUX ", strlen("GET CONSOLE LINUX ")) == 0)
		{
#ifdef USE_JSONCPP
			int pos = 0;
			CoString linuxIP = recvData.AbstractSearchSubstance(pos);

			static int update = 0;

			COMMAND_CTRL_DATA cmdData;
			GetCommandCtrlData(cmdData, !update);

			update = ++update % 10;

			GetSystemTime(&cmdData.time_span);
			SystemTimeToTzSpecificLocalTime(NULL, &cmdData.time_span, &cmdData.time_span);
			COleDateTime nowtime(cmdData.time_span);
			COleDateTime oletime(cmdData.boot_time);
			COleDateTimeSpan timespan = nowtime - oletime;

			CoString aa = cmdData.ipaddr.Right(2);
			INT pp = strtol(aa, NULL, 10);
#ifndef _DEBUG
			Sleep(pp * 30); // 0.03秒間隔で3秒間に全台を更新
#endif

			Json::Value *jVal = create_OValue();
			Json::Value *jWork = create_OValue();
			BOOL is_check_Console = FALSE;
			int machine_count = 0;

			(*jVal)["total"] = (unsigned int)1;

			jWork->clear();

			CString work;

			(*jWork)["macaddr"] = (LPCTSTR)cmdData.macaddr;
			(*jWork)["ipaddr"] = (LPCTSTR)cmdData.ipaddr;
			(*jWork)["hostname"] = (LPCTSTR)cmdData.hostname;
			(*jWork)["status"] = P("WAKEUP");

			int maru = (1024 * 1024);

			work = timespan.Format("%H:%M:%S");
			(*jWork)["time_span"] = work;
			(*jWork)["cpu_usage"] = (UINT)cmdData.cpu_usage;
			(*jWork)["cpu_usage"] = cmdData.nowProcess.GetSize();
			(*jWork)["dwMemoryLoad"] = (UINT)cmdData.info_mem.dwMemoryLoad;
			(*jWork)["dwTotalPhys"] = (UINT)(cmdData.info_mem.dwTotalPhys / maru);
			(*jWork)["dwAvailPhys"] = (UINT)(cmdData.info_mem.dwAvailPhys / maru);
			(*jWork)["dwTotalVirtual"] = (UINT)(cmdData.info_mem.dwTotalVirtual / maru);
			(*jWork)["dwAvailVirtual"] = (UINT)(cmdData.info_mem.dwAvailVirtual / maru);
			(*jWork)["ulFreeBytesAvailable"] = (UINT)(cmdData.ulFreeBytesAvailable / maru); // I64u
			(*jWork)["ulTotalNumberOfBytes"] = (UINT)(cmdData.ulTotalNumberOfBytes / maru); // I64u
			(*jWork)["ulTotalNumberOfFreeBytes"] = (UINT)(cmdData.ulTotalNumberOfFreeBytes / maru); // I64u
			(*jWork)["in_packet"] = (UINT)cmdData.in_packet;
			(*jWork)["out_packet"] = (UINT)cmdData.out_packet;
			(*jWork)["connections"] = cmdData.connections.GetSize();

			(*jVal)["results"][machine_count] = *jWork;
			machine_count++;

			CoString buffer = jVal->toFastString().c_str();

			jWork->release();
			jVal->release();

			DataSendConsole(linuxIP, 12055, buffer);
#endif
		}
		// リソース情報の要求（管理を含む）
		else if (strncmp(recvData, "GET CONSOLE COMMAND ", strlen("GET CONSOLE COMMAND ")) == 0)
		{
			INT pos = 0;
			CoString sysTimeText = recvData.AbstractSearchSubstance(pos);
			CoString masterConsoleIP = recvData.AbstractSearchSubstance(pos);

			CoString data0 = recvData.AbstractSearchSubstance(pos);
			CoString data1 = recvData.AbstractSearchSubstance(pos);

			if (data0 == data1)
			{
				COleDateTime expire_day;
				DateFromString(expire_day, data0);

				GetExpireLicense(&expire_day);
			}

			SYSTEMTIME sysTime = { 0 };
			if (DateFromString(&sysTime, sysTimeText) != 0)
			{
				continue;
			}

			MasterConsoleIP(masterConsoleIP, &sysTime);

			static int update = 0;

			COMMAND_CTRL_DATA cmdData;
			GetCommandCtrlData(cmdData, !update);

			update = ++update % 10;

			GetSystemTime(&cmdData.time_span);
			SystemTimeToTzSpecificLocalTime(NULL, &cmdData.time_span, &cmdData.time_span);
			COleDateTime nowtime(cmdData.time_span);
			COleDateTime oletime(cmdData.boot_time);
			COleDateTimeSpan timespan = nowtime - oletime;

			CoString aa = cmdData.ipaddr.Right(2);
			INT pp = strtol(aa, NULL, 10);
			Sleep(pp * 30); // 0.03秒間隔で3秒間に全台を更新

			CoString sendData;
			CoString work;
			sendData.Format("TCP 01 DATA "); // WAKEUP
			work.Format(" macaddr \"%s\"", (LPCTSTR)cmdData.macaddr);
			sendData += work;
			work.Format(" ipaddr \"%s\"", (LPCTSTR)cmdData.ipaddr);
			sendData += work;
			work.Format(" hostname \"%s\"", (LPCTSTR)cmdData.hostname);
			sendData += work;
			work.Format(" status \"WAKEUP\"");
			sendData += work;

			SJIStoUTF8(sendData, sendData);

			DataSendConsole(masterConsoleIP, 12084, sendData);

			Sleep(pp * 10); // 0.01秒間隔で1秒間に全台を更新

			sendData.Format("TCP 02 DATA ");
			work.Format(" macaddr \"%s\"", (LPCTSTR)cmdData.macaddr);
			sendData += work;
			work.Format(" username \"%s\"", (LPCTSTR)cmdData.username);
			sendData += work;
			work = oletime.Format(" boot_time \"%Y/%m/%d %H:%M:%S\"");
			sendData += work;

			work = timespan.Format(" time_span \"%H:%M:%S\"");
			sendData += work;
			work.Format(" cpu_usage \"%u\"", cmdData.cpu_usage);
			sendData += work;
			work.Format(" process \"%u\"", cmdData.nowProcess.GetSize());
			sendData += work;
			work.Format(" dwMemoryLoad \"%u\"", cmdData.info_mem.dwMemoryLoad);
			sendData += work;
			work.Format(" dwTotalPhys \"%u\"", cmdData.info_mem.dwTotalPhys);
			sendData += work;
			work.Format(" dwAvailPhys \"%u\"", cmdData.info_mem.dwAvailPhys);
			sendData += work;
			work.Format(" dwTotalVirtual \"%u\"", cmdData.info_mem.dwTotalVirtual);
			sendData += work;
			work.Format(" dwAvailVirtual \"%u\"", cmdData.info_mem.dwAvailVirtual);
			sendData += work;
			work.Format(" ulFreeBytesAvailable \"%I64u\"", cmdData.ulFreeBytesAvailable);
			sendData += work;
			work.Format(" ulTotalNumberOfBytes \"%I64u\"", cmdData.ulTotalNumberOfBytes);
			sendData += work;
			work.Format(" ulTotalNumberOfFreeBytes \"%I64u\"", cmdData.ulTotalNumberOfFreeBytes);
			sendData += work;
			work.Format(" in_packet \"%u\"", cmdData.in_packet);
			sendData += work;
			work.Format(" out_packet \"%u\"", cmdData.out_packet);
			sendData += work;
			work.Format(" connections \"%u\"", cmdData.connections.GetSize());
			sendData += work;

			SJIStoUTF8(sendData, sendData);

			DataSendConsole(masterConsoleIP, 12084, sendData);
		}
		// アナライズ情報の要求（クライアントのみ）
		else if (strncmp(recvData, "GET ANALYZE PROCESS COMMAND ", strlen("GET ANALYZE PROCESS COMMAND ")) == 0)
		{
			if (IsThisMasterConsole())
			{
				continue;
			}

			INT pos = 0;
			CoString sysTimeText = recvData.AbstractSearchSubstance(pos);
			CoString masterConsoleIP = recvData.AbstractSearchSubstance(pos);

			SYSTEMTIME sysTime = { 0 };
			if (DateFromString(&sysTime, sysTimeText) != 0)
			{
				continue;
			}

			MasterConsoleIP(masterConsoleIP, &sysTime);

			static BOOL bFirst = TRUE;
			if (bFirst)
			{
				bFirst = FALSE;

				// 通信に失敗した情報を再送信
				LRESULT LoadClientAnalyzeProcessData();
				LoadClientAnalyzeProcessData();

				// 通信に失敗した情報を再送信
				LRESULT LoadClientAnalyzeForeData();
				LoadClientAnalyzeForeData();
			}
		}
		// 起動禁止リストの設定（クライアントのみ）
		else if (strncmp(recvData, "SET SUPPRESSION COMMAND ", strlen("SET SUPPRESSION COMMAND ")) == 0)
		{
			if (IsThisMasterConsole())
			{
				continue;
			}

			INT pos = 0;
			CoString sysTimeText = recvData.AbstractSearchSubstance(pos);
			CoString masterConsoleIP = recvData.AbstractSearchSubstance(pos);
			CoString dataOne = recvData.AbstractSearchSubstance(pos);
			CoString dataTwo = recvData.AbstractSearchSubstance(pos);

			if (dataOne.Compare(dataTwo) == 0)
			{
				SYSTEMTIME sysTime = { 0 };
				if (DateFromString(&sysTime, sysTimeText) != 0)
				{
					continue;
				}

				MasterConsoleIP(masterConsoleIP, &sysTime);

				SetSuppressionList(dataOne);
			}
		}
		// 自動起動禁止リストの設定（クライアントのみ）
		else if (strncmp(recvData, "SET SUPPRES AUTO COMMAND ", strlen("SET SUPPRES AUTO COMMAND ")) == 0)
		{
			if (IsThisMasterConsole())
			{
				continue;
			}

			INT pos = 0;
			CoString sysTimeText = recvData.AbstractSearchSubstance(pos);
			CoString masterConsoleIP = recvData.AbstractSearchSubstance(pos);
			CoString dataOne = recvData.AbstractSearchSubstance(pos);
			CoString dataTwo = recvData.AbstractSearchSubstance(pos);

			if (dataOne.Compare(dataTwo) == 0)
			{
				SYSTEMTIME sysTime = { 0 };
				if (DateFromString(&sysTime, sysTimeText) != 0)
				{
					continue;
				}

				MasterConsoleIP(masterConsoleIP, &sysTime);

				DWORD resVer = 0;
				SetSuppresAutoList(dataOne, resVer);
			}
		}
		// ホストス情報の設定（クライアントのみ）
		else if (strncmp(recvData, "SET HOSTS AUTO COMMAND ", strlen("SET HOSTS AUTO COMMAND ")) == 0)
		{
			if (IsThisMasterConsole())
			{
				continue;
			}

			INT pos = 0;
			CoString sysTimeText = recvData.AbstractSearchSubstance(pos);
			CoString masterConsoleIP = recvData.AbstractSearchSubstance(pos);
			CoString dataOne = recvData.AbstractSearchSubstance(pos);
			CoString dataTwo = recvData.AbstractSearchSubstance(pos);

			if (dataOne.Compare(dataTwo) == 0)
			{
				SYSTEMTIME sysTime = { 0 };
				if (DateFromString(&sysTime, sysTimeText) != 0)
				{
					continue;
				}

				MasterConsoleIP(masterConsoleIP, &sysTime);

				CoString textVer = dataOne.Left(10);
				DWORD resVer = strtoul(textVer, NULL, 10);
				CoString resData = dataOne.Right(dataOne.GetLength() - 10);
				SetHostsAutoList(resData, resVer);
			}
		}
		// ポップアップの表示（管理を含む）
		else if (strncmp(recvData, "SHOW POPUP MESSAGE COMMAND ", strlen("SHOW POPUP MESSAGE COMMAND ")) == 0)
		{
			INT pos = 0;
			CoString sysTimeText = recvData.AbstractSearchSubstance(pos);
			CoString masterConsoleIP = recvData.AbstractSearchSubstance(pos);
			CoString dataOne = recvData.AbstractSearchSubstance(pos);
			CoString dataTwo = recvData.AbstractSearchSubstance(pos);

			if (dataOne.Compare(dataTwo) == 0)
			{
				SYSTEMTIME sysTime = { 0 };
				if (DateFromString(&sysTime, sysTimeText) != 0)
				{
					continue;
				}

				MasterConsoleIP(masterConsoleIP, &sysTime);

				CoString popMsg;
				base64_decode(dataOne, popMsg);

				if (memcmp(popMsg, "popupmessage:", strlen("popupmessage:")) == 0)
				{
					CoString recvData;
					DataSendConsole("localhost", 12087, popMsg, &recvData);
				}
			}
		}
		// アップデートの確認（クライアントのみ）
		else if (strncmp(recvData, "CHECK SETUP UPDATE COMMAND ", strlen("CHECK SETUP UPDATE COMMAND ")) == 0)
		{
			if (IsThisMasterConsole())
			{
				continue;
			}

			// アップデートの確認
			CheckSetupUpdate();
		}
		// ブロードキャストのチェック（管理のみ）
		else if (strncmp(recvData, "CHECK BROADCAST COMMAND ", strlen("CHECK BROADCAST COMMAND ")) == 0)
		{
			if (!IsThisMasterConsole())
			{
				continue;
			}

			IsCheckBroadcast(1);
		}
	}

	return 0;
}
