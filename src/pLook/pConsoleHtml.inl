///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#include "pConsolePage.inl"

#include "oMyJson.h"

int DisplayHttpHeader(SOCKET http_socket,
											CoString content_type="text/html; charset=utf-8",
											ULONGLONG content_length=0,
											CoString status="200 OK",
											CoString cache="no-cache, no-store, max-age=0")
{
	CoString buffer =
		"HTTP/1.1 " + status + "\r\n"
		"Cache-Control: " + cache + "\r\n"
		"Content-Type: " + content_type + "\r\n"
		"Server: Plustar Pl00k C0nS0le/1.0\r\n"
		"Connection: close\r\n";
	if (content_length > 0)
	{
		CoString work;
		work.Format("Content-Length: %d\r\n", content_length);
		buffer += work;
	}
	buffer += "WWW-Authenticate: Basic realm=\"ExTrapper By Plustar\"\r\n\r\n";

	SJIStoUTF8(buffer, buffer);

	int n = send(http_socket, buffer, buffer.GetLength(), 0);
	if (n < 1)
	{
		OutputViewer("\0 %s(%d): [0x%08X][0x%08X] n=%d" + 2, __FILE__, __LINE__, GetLastError(), WSAGetLastError(), n);
		return -1;
	}
	return n;
}

int DisplayRequestAuth(SOCKET http_socket)
{
	CoString buffer = js_err_401_html;

	SJIStoUTF8(buffer, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/html; charset=utf-8", len, "401 Unauthorized");
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayNotFound(SOCKET http_socket)
{
	CoString buffer = js_err_404_html;

	SJIStoUTF8(buffer, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/html; charset=utf-8", len, "404 Not Found");
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayInternalServerError(SOCKET http_socket)
{
	CoString buffer = js_err_500_html;

	SJIStoUTF8(buffer, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/html; charset=utf-8", len, "500 Internal Server Error");
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayResult(SOCKET http_socket, BOOL success, CoString errors="[]")
{
#ifdef USE_JSONCPP
	Json::Value *jVal = create_OValue();
	(*jVal)["success"] = (success ? true : false);

	Json::Reader *reader = create_Reader();
	reader->parse((const char *)errors, (*jVal)["errors"]);
	CoString buffer = jVal->toFastString().c_str();

	jVal->release();
	reader->release();

#else
	CoString buffer;
	buffer.Format("{\"success\": %s, \"errors\": %s}",
								success?"true":"false",
								errors);
#endif

	SJIStoUTF8(buffer, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return 0;
}

int DisplayExpirePage(SOCKET http_socket)
{
	CoString shop = GetShopLicense();

	COleDateTime expire = GetExpireLicense();
	CoString unique = GetShopUnique();

	CoString buffer;
//	buffer.Format("店舗:%s<br>管理コード:%s<br>\n有効期限:%s<br><br>\nライセンスの有効期限を確認してください<br><br>"
//		"<a href=\"http://plustar.jp\"><img src=\"powered.png\" width=\"146\" height=\"42\" alt=\"POWERED BY PLUSTAR\" /><br>plustar.jp</a>",
//		(LPCTSTR)shop, (LPCTSTR)unique, expire.Format("%Y/%m/%d"));

	buffer.Format(exp_lic_html,
		(LPCTSTR)shop,                // shop
		(LPCTSTR)unique,              // store code
		expire.Format("%Y/%m/%d"),    // expire date
		(LPCTSTR)shop,                // shop
		(LPCTSTR)unique,              // store code
		(LPCTSTR)shop);               // shop

	SJIStoUTF8(buffer, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/html; charset=utf-8", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayIndexPage(SOCKET http_socket)
{
	CoString shop = GetShopLicense();

	COleDateTime expire = GetExpireLicense();
	COleDateTime today = COleDateTime::GetCurrentTime();

	struct LIC_STR_DATA
	{
		LPCTSTR str;
		LPCTSTR text;
	};

	LIC_STR_DATA typeTable[] =
	{
		"Trial", "試用ライセンス",
		"Silver", "シルバーライセンス",
		"Gold", "ゴールドライセンス",
		"Platinum", "プラチナライセンス",
		NULL, NULL
	};

	CoString type = "不明なライセンス";
	CoString typeAsess = GetAsessNameLicense();
	for (DWORD i = 0; typeTable[i].str; i++)
	{
		if (typeAsess.CompareNoCase(typeTable[i].str) == 0)
		{
			type = typeTable[i].text;
			break;
		}
	}

	LPCTSTR GetThisRev();
	CoString rev = GetThisRev();

	LPCTSTR GetThisVersion();
	CoString ver = GetThisVersion();

	// プラチナとデモは全ての機能を有効にする
	// 今後ext1系はコアライブラリをext3系に変更して機能制限を行うバージョンにする
	LPCTSTR js_ext = js_index_html;
	if ( typeAsess == "Platinum" || typeAsess == "Trial" )
		js_ext = js_index_html3;

	CoString buffer;
	buffer.Format(js_ext,
		shop,                         // license shop for title
		(LPCTSTR)ver,                 // version number
		(LPCTSTR)rev,                 // revision number
		type,                         // license type
		shop,                         // license shop
		GetMassLicense(),             // license mass
		expire.Format("%Y/%m/%d"),    // expire date
		(expire-today).Format("%D"),  // remaining day
		GetTrapperUpGrade(),          // remaining day
		shop);                        // license shop for Google Analytics

	SJIStoUTF8(buffer, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/html; charset=utf-8", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayStylesheet(SOCKET http_socket, CoString version = "")
{
	//CoString buffer = js_extrapper_css + version;
	CoString buffer = version == "3" ? js_extrapper_css3 : js_extrapper_css;


	SJIStoUTF8(buffer, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/css; charset=utf-8", len, "200 OK", "private, max-age=31536000");
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayJavaScript(SOCKET http_socket, CoString version = "")
{
	//CoString buffer = js_extrapper_js + version;
	CoString buffer = version == "3" ? js_extrapper_js3 : js_extrapper_js;

	SJIStoUTF8(buffer, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8", len, "200 OK", "private, max-age=31536000");
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

DWORD DisplayImageLogo(SOCKET http_socket)
{
	CoString typeAsess = GetAsessNameLicense();
	CoString imageData = js_logo_png;

	// ライセンスによりロゴを使い分ける
	if ( typeAsess == "Platinum" )
		imageData = js_logo_png_platinum;
	else if(typeAsess == "Gold" )
		imageData = js_logo_png_gold;
	else if( typeAsess == "Silver" )
		imageData = js_logo_png_silver;
	else if(typeAsess == "Trial")
		imageData = js_logo_png_trial;

	CoString buffer;
	DWORD size = base64_decode(imageData, buffer, 0);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "image/png", len, "200 OK", "private, max-age=31536000");
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

DWORD DisplayImagePowered(SOCKET http_socket)
{
	CoString imageData = js_powered_png;

	CoString buffer;
	DWORD size = base64_decode(imageData, buffer, 0);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "image/png", len, "200 OK", "private, max-age=31536000");
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayExternalResource(SOCKET http_socket, CoString path)
{
	if (::GetFileAttributes(path) == 0xFFFFFFFF)
	{
		return DisplayNotFound(http_socket); // file not exist
	}

	HANDLE hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return DisplayNotFound(http_socket); // file open error
	}

	DWORD dwReadHigh = 0;
	DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	CoString buf;
	ReadFile(hFile, buf.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
	CloseHandle(hFile);

	if (path.ReverseFind(".css") >= 0)
	{
		DisplayHttpHeader(http_socket, "text/css; charset=utf-8",
											buf.GetLength(), "200 OK", "private, max-age=31536000");
	}
	else if (path.ReverseFind(".js") >= 0)
	{
		DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8",
											buf.GetLength(), "200 OK", "private, max-age=31536000");
	}
	else if (path.ReverseFind(".gif") >= 0)
	{
		DisplayHttpHeader(http_socket, "image/gif",
											buf.GetLength(), "200 OK", "private, max-age=31536000");
	}
	else if (path.ReverseFind(".png") >= 0)
	{
		DisplayHttpHeader(http_socket, "image/png",
											buf.GetLength(), "200 OK", "private, max-age=31536000");
	}
	else if (path.ReverseFind(".jpg") >= 0)
	{
		DisplayHttpHeader(http_socket, "image/jpg",
											buf.GetLength(), "200 OK", "private, max-age=31536000");
	}
	else if (path.ReverseFind(".txt") >= 0)
	{
		DisplayHttpHeader(http_socket, "text/plain",
											buf.GetLength(), "200 OK", "private, max-age=31536000");
	}
	else if (path.ReverseFind(".ico") >= 0)
	{
		DisplayHttpHeader(http_socket, "image/vnd.microsoft.icon",
											buf.GetLength(), "200 OK", "private, max-age=31536000");
	}
	else
	{
		return DisplayInternalServerError(http_socket); // unsupported file type
	}

	send(http_socket, buf, buf.GetLength(), 0);

	return buf.GetLength();
}

int DisplayMachineList(SOCKET http_socket)
{
	CoConsoleDataArray console_data;
	GetConsoleData(console_data);

	DWORD mass = GetMassLicense();
	DWORD count = console_data.GetSize();
	if (count > mass)
	{
		// ライセンス数を超えたPCは表示しない
		count = mass;
	}

	INT online = 0;
	unsigned long process_count = 0;

	UINT64 totalNetwork = 0;
	for (DWORD i = 0; i < count; i++)
	{
		UPDATE_COMSOLE_DATA& data = console_data.ElementAt(i);

		if (data.ipaddr.IsEmpty())
		{
			continue;	// 電源OFFのPCはスキップ
		}

		data.in_net = strtoul(data.in_packet, NULL, 10)/100;
		data.out_net = strtoul(data.out_packet, NULL, 10)/100;
		totalNetwork += (data.in_net + data.out_net);
		
		process_count += strtoul(data.process, NULL, 10);

		online++;
	}

	if (online == 0)
	{
		// DEBUG_OUT_DEFAULT("data failed."); // 時にはいいかも
		return -1;
	}
	
	// 起動平均プロセス数
	unsigned long process_ave = process_count / online;

#ifdef USE_JSONCPP
	Json::Value *jVal = create_OValue();
	Json::Value *jWork = create_OValue();
	BOOL is_check_Console = FALSE;
	int machine_count = 0;

	(*jVal)["total"] = (unsigned int)count;
#else
	BOOL is_first = TRUE;
	BOOL is_check_Console = FALSE;
	CoString buffer, work;
	work.Format("{\"total\": %u, \"results\": [", count);
	buffer = work;
#endif
	for (DWORD i = 0; i < count; i++)
	{
		UPDATE_COMSOLE_DATA& data = console_data.ElementAt(i);

		if (data.ipaddr.IsEmpty())
		{
			continue; // 電源OFFのPCはスキップ
		}

#ifdef USE_JSONCPP
#else
		if (!is_first) {
			buffer += ",";
		}else{
			is_first = FALSE;
		}
#endif

		// DISK容量1GB単位での計算処理
		DWORD ulTotalNumberOfFreeBytes = strtoul(data.ulTotalNumberOfFreeBytes.Left(data.ulTotalNumberOfFreeBytes.GetLength() - 9), NULL, 10);
		DWORD ulTotalNumberOfBytes = strtoul(data.ulTotalNumberOfBytes.Left(data.ulTotalNumberOfBytes.GetLength() - 9), NULL, 10);
		DWORD perNumberOfBytes = 100 - (ulTotalNumberOfFreeBytes * 100 / (ulTotalNumberOfBytes ? ulTotalNumberOfBytes : 1));

		DWORD perinpac = 0;
		DWORD peroutpac = 0;
		DWORD perNetwork = 0;
		UINT64 currentNetwork = data.in_net + data.out_net;
		if (currentNetwork != 0 && totalNetwork != 0)
		{
			perinpac = (DWORD)((UINT64)data.in_net * 100 / currentNetwork);
			peroutpac = 100 - perinpac;
			perNetwork = (DWORD)((UINT64)currentNetwork * 100 / totalNetwork);
		}


		unsigned long memoryLoad  = strtoul(data.dwMemoryLoad, NULL, 10);
		unsigned long cpu_usage   = strtoul(data.cpu_usage, NULL, 10);
		unsigned long connections = strtoul(data.connections, NULL, 10);

		// デフォステータス：PC起動
		DWORD user_job = 0;

		// パラメータ分析をして端末で何が行われているかアイコンで示す
		// Case1：管理サーバ
		// 関数あり
		if(!is_check_Console && IsConsole(data.ipaddr))
		{
			user_job = 1;
			is_check_Console = TRUE;
		}
		// Case6：オンラインゲームゲーム
		// CPU:20～30
		// 通信のINとOUTが共に50%
		else if((cpu_usage > 20 && cpu_usage < 30)
		&& (perinpac > 45 && perinpac < 55) && (peroutpac > 45 && peroutpac < 55))
		{
			user_job = 6;
		}
		// Case2：ストリーミング
		// 通信のINが70% / CPU:70% / コネクション:1
		else if( (perinpac > 60 && perinpac < 75)
		 && (cpu_usage > 65 && cpu_usage < 75))
		{
			user_job = 2;
		}
		// Case7：ウェブサーフィン
		// CPU:50
		// 通信のIN20-30%
		else if((cpu_usage > 45 && cpu_usage < 55)
		&& (perinpac > 20 && perinpac < 30))
		{
			user_job = 7;
		}
		//  Case3：動画
		// CPU:70%
		else if((cpu_usage > 65 && cpu_usage < 75))
		{
			user_job = 3;
		}
		// Case4：ダウンロード,P2P
		// 通信のINが100
		// ネットワーク使用率が全体の1%以上
		else if(perNetwork >= 1 && perinpac > 95)
		{
			user_job = 4;
		}
		// Case5：データ送信
		// 通信のOUTが100
		// ネットワーク使用率が全体の1%以上
		else if(perNetwork >= 1 && peroutpac > 95)
		{
			user_job = 5;
		}
		// Case8：PC使用中
		// 平均プロセス数よりも多い場合は使用中
		else if(process_ave < strtoul(data.process, NULL, 10) )
		{
			user_job = 8;
		}
		
		// Case9：・・・
		// Case10：・・・
		
		// CaseX：起動のみ
		// 全部0

#ifdef USE_JSONCPP
		jWork->clear();
		(*jWork)["macaddr"]     = (const char *)data.macaddr;
		(*jWork)["hostname"]    = (const char *)data.hostname;
		(*jWork)["ipaddr"]      = (const char *)data.ipaddr;
		(*jWork)["boot_time"]   = (const char *)data.boot_time;
		(*jWork)["time_span"]   = (const char *)data.time_span;
		(*jWork)["proc"]        = atoi( (const char *)data.process );
		(*jWork)["mem"]         = (unsigned int)strtoul(data.dwMemoryLoad, NULL, 10);
		(*jWork)["disk"]        = (unsigned int)perNumberOfBytes;
		(*jWork)["cpu"]         = (unsigned int)strtoul(data.cpu_usage, NULL, 10);
		(*jWork)["traffic"]     = (unsigned int)perNetwork;
		(*jWork)["net_in"]      = (unsigned int)perinpac;
		(*jWork)["net_out"]     = (unsigned int)peroutpac;
		(*jWork)["connections"] = (unsigned int)strtoul(data.connections, NULL, 10);
		(*jWork)["user_job"]    = (unsigned int)user_job;

		(*jVal)["results"][machine_count] = *jWork;
		machine_count++;
#else
		work.Format("{"
								"\"macaddr\": \"%s\", "
								"\"hostname\": \"%s\", "
								"\"ipaddr\": \"%s\", "
								"\"boot_time\": \"%s\", "
								"\"time_span\": \"%s\", "
								"\"proc\": %s, "
								"\"mem\": %u, "
								"\"disk\": %u, "
								"\"cpu\": %u, "
								"\"traffic\": %u, "
								"\"net_in\": %u, "
								"\"net_out\": %u, "
								"\"connections\": %u, "
								"\"user_job\": %u "
								"}",
								data.macaddr,
								data.hostname,
								data.ipaddr,
								data.boot_time,
								data.time_span,
								data.process,
								memoryLoad,
								perNumberOfBytes,
								cpu_usage,
								perNetwork,
								perinpac,
								peroutpac,
								connections,
								user_job
								);
		buffer += work;
#endif
	}
#ifdef USE_JSONCPP
	CoString buffer = jVal->toFastString().c_str();

	jWork->release();
	jVal->release();

#else
	buffer += "]}";
#endif

	SJIStoUTF8(buffer, buffer);

	int len = buffer.GetLength();

	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8",len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return 0;
}

int DisplayProcessList(SOCKET http_socket, CoString* data)
{
	CoString buffer;
	DWORD size = base64_decode(*data, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayConnectionList(SOCKET http_socket, CoString* data)
{
	CoString buffer;
	DWORD size = base64_decode(*data, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayMachineInfo(SOCKET http_socket, CoString* data)
{
	CoString buffer;
	DWORD size = base64_decode(*data, buffer);

	// マシン構成
	// SystemInformation
	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayMachineInfoList(SOCKET http_socket)
{
	CoString data;
	CoString modulePath = GetModuleFolder();
	CoString reportPath = modulePath + "pHardware.dat";

	HANDLE hFile = CreateFile(reportPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwReadHigh = 0;
		DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
		ReadFile(hFile, data.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
		CloseHandle(hFile);
	}

	data.Insert(0, "[");
	data.TrimRight(",\r\n");
	data += "]";

	int len = data.GetLength();
	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8", len);
	int n = send(http_socket, data, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayInstall(SOCKET http_socket, CoString* data)
{
	CoString buffer;
	DWORD size = base64_decode(*data, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayUrl(SOCKET http_socket, CoString* data)
{
	CoString buffer;
	DWORD size = base64_decode(*data, buffer);

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayScreenshot(SOCKET http_socket, CoString* data)
{
	CoString& buffer = *data;
//	base64_decode(*data, buffer);
	if (buffer.Compare("[ERROR]\nMaking infomation failed.") == 0)
	{
		return DisplayInternalServerError(http_socket); // failed get screenshot
	}

	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "image/jpg", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplaySuppressionList(SOCKET http_socket, CoString& data)
{
	CoString buffer = data;
	if (buffer.GetLength() == 0)
	{
		buffer = "[]";
	}
	else
	{
		buffer.Replace("\\,", "\"}, {\"procname\": \"");
		buffer = "[{\"procname\": \"" + buffer + "\"}]";
	}
	SJIStoUTF8(buffer, buffer);
	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayPassword(SOCKET http_socket,
										CoString& type,
										CoString& opwd,
										CoString& npwd,
										CoString& cpwd)
{
	BOOL success = TRUE;
#ifdef USE_JSONCPP
	Json::Value *jErrors = create_AValue();
	int count = 0;
#else
	CoString errors = "[";
#endif

	if (npwd.Compare("") == 0)
	{
#ifdef USE_JSONCPP
		(*jErrors)[count]["id"]  = "new_password";
		(*jErrors)[count]["msg"] = "必須項目です。";
		count++;
#else
		errors += "{\"id\": \"new_password\","
							" \"msg\": \"必須項目です。\"}, ";
#endif
		success = FALSE;
	}

	if (npwd.Compare(cpwd) != 0)
	{
#ifdef USE_JSONCPP
		(*jErrors)[count]["id"]  = "cfm_password";
		(*jErrors)[count]["msg"] = "新しいパスワードと一致しません。";
		count++;
#else
		errors += "{\"id\": \"cfm_password\","
							" \"msg\": \"新しいパスワードと一致しません。\"}, ";
#endif
		success = FALSE;
	}

	if (type.Compare("login") == 0)
	{
		CoString nowpwd;
		(VOID)GetConsolePassword(nowpwd);
		if (nowpwd.Compare("pLook:"+opwd) != 0)
		{
#ifdef USE_JSONCPP
			(*jErrors)[count]["id"]  = "old_password";
			(*jErrors)[count]["msg"] = "パスワードが違います。";
			count++;
#else
			errors += "{\"id\": \"old_password\","
				" \"msg\": \"パスワードが違います。\"}, ";
#endif
			success = FALSE;
		}

		if (success)
		{
			(VOID)SetConsolePassword(npwd);
		}
	}
	else if (type.Compare("sshot") == 0)
	{
		CoString nowpwd;
		(VOID)GetScreenPassword(nowpwd);
		if (nowpwd.Compare(opwd) != 0)
		{
#ifdef USE_JSONCPP
			(*jErrors)[count]["id"]  = "old_password";
			(*jErrors)[count]["msg"] = "パスワードが違います。";
			count++;
#else
			errors += "{\"id\": \"old_password\","
				" \"msg\": \"パスワードが違います。\"}, ";
#endif
			success = FALSE;
		}

		if (success)
		{
			(VOID)SetScreenPassword(npwd);
		}
	}
	else {
		return DisplayInternalServerError(http_socket);	// Not happen
	}

#ifdef USE_JSONCPP
	CoString errors = jErrors->toFastString().c_str();
	jErrors->release();
#else
	errors.TrimRight(", ");
	errors += "]";
#endif

	return DisplayResult(http_socket, success, errors);
}

int DisplayMiscFilter(SOCKET http_socket,
								CoString& status,
								CoString& server,
								CoString& password)
{
	BOOL success = TRUE;
#ifdef USE_JSONCPP
#else
	CoString errors = "[";
#endif
	// TODO
	if (/*status == "" ||*/ server == "" && password == "") {
		NETWORK_PEJORATION_DATA pejorationData = { 0 };
		GetNetworkPejoration(pejorationData);
#ifdef USE_JSONCPP
		Json::Value *jVal = create_AValue();
		(*jVal)[0u]["status"]   = "";
		(*jVal)[0u]["server"]   = "";
		(*jVal)[0u]["password"] = "";
		CoString buffer = jVal->toFastString().c_str();

		jVal->release();
#else
		CoString buffer;
		buffer.Format("[{\"status\":\"\",\"server\":\"\",\"password\":\"\"}]");
#endif
		int len = buffer.GetLength();
		DisplayHttpHeader(http_socket, "text/plain; charset=utf-8",
											len, "200 OK", "private");
		int n = send(http_socket, buffer, len, 0);
		return len;
	}

	// TODO
	success = FALSE;
#ifdef USE_JSONCPP
	Json::Value *jErrors = create_AValue();
	(*jErrors)[0u]["id"]  = "filter_server";
	(*jErrors)[0u]["msg"] = "未契約です。";
	(*jErrors)[1]["id"]   = "filter_server_password";
	(*jErrors)[1]["msg"]  = "未契約です。";

	CoString errors = jErrors->toFastString().c_str();
	jErrors->release();
#else
	errors += "{\"id\": \"filter_server\","
		" \"msg\": \"未契約です。\"},"
		"{\"id\": \"filter_server_password\","
		" \"msg\": \"未契約です。\"}";
	errors += "]";
#endif

	return DisplayResult(http_socket, success, errors);
}

int DisplayMiscSync(SOCKET http_socket,
								CoString& server)
{
	BOOL success = TRUE;
#ifdef USE_JSONCPP
#else
	CoString errors = "[";
#endif
	if (server == "") {
#ifdef USE_JSONCPP
		Json::Value *jVal = create_AValue();
		(*jVal)[0u]["server"] = "";
		CoString buffer = jVal->toFastString().c_str();

		jVal->release();
#else
		CoString buffer;
		buffer.Format("[{\"server\":\"\"}]");
#endif
		int len = buffer.GetLength();
		DisplayHttpHeader(http_socket, "text/plain; charset=utf-8",
											len, "200 OK", "private");
		int n = send(http_socket, buffer, len, 0);
		return len;
	}

	// TODO
	success = FALSE;
#ifdef USE_JSONCPP
	Json::Value *jErrors = create_AValue();
	(*jErrors)[0u]["id"]  = "sync_server";
	(*jErrors)[0u]["msg"] = "未契約です。";

	CoString errors = jErrors->toFastString().c_str();
	jErrors->release();
#else
	errors += "{\"id\": \"sync_server\","
		" \"msg\": \"未契約です。\"}";
	errors += "]";
#endif

	return DisplayResult(http_socket, success, errors);
}

int DisplayMiscNet(SOCKET http_socket,
								CoString& mon,
								CoString& con,
								CoString& alt)
{
	BOOL success = TRUE;
#ifdef USE_JSONCPP
#else
	CoString errors = "[";
#endif

	// POSTではない場合はレジストリから設定値を取得
	if (mon == "" || con == "" || alt == "") {
		NETWORK_PEJORATION_DATA pejorationData = { 0 };
		GetNetworkPejoration(pejorationData);
#ifdef USE_JSONCPP
		Json::Value *jVal = create_AValue();
		(*jVal)[0u]["mon"] = (unsigned int)pejorationData.offline;
		(*jVal)[0u]["con"] = (unsigned int)pejorationData.pejoration;
		(*jVal)[0u]["alt"] = (unsigned int)pejorationData.percent;
		CoString buffer = jVal->toFastString().c_str();

		jVal->release();
#else
		CoString buffer;
		buffer.Format("[{\"mon\":%d,\"con\":%d,\"alt\":%d}]",
									pejorationData.offline, pejorationData.pejoration, pejorationData.percent);
#endif
		int len = buffer.GetLength();
		DisplayHttpHeader(http_socket, "text/plain; charset=utf-8",
											len, "200 OK", "private");
		int n = send(http_socket, buffer, len, 0);
		return len;
	}

	NETWORK_PEJORATION_DATA pejorationData = { 0 };
	pejorationData.offline = strtoul(mon, NULL, 10);
	if(pejorationData.offline != 0 && pejorationData.offline != 1)
	{
		// 分からん値が返ってきた場合はONとする
		pejorationData.offline = 1;
	}

	pejorationData.pejoration = strtoul(con, NULL, 10);
	if(pejorationData.pejoration != 0 && pejorationData.pejoration != 1)
	{
		// 分からん値が返ってきた場合はONとする
		pejorationData.pejoration = 1;
	}

#ifdef USE_JSONCPP
	Json::Value *jErrors = create_AValue();
	int count = 0;
#endif

	// 念のためにチェックを行う
	pejorationData.percent = strtoul(alt, NULL, 10);
	if(pejorationData.percent > 999)
	{
#ifdef USE_JSONCPP
		(*jErrors)[count]["id"] = "net_alt";
		(*jErrors)[count]["msg"] = "負荷設定値が大きいです。<br>999%(通常の約10倍)以上は設定できません。";
		count++;
#else
		errors += "{\"id\": \"net_alt\","
			" \"msg\": \"負荷設定値が大きいです。<br>999%(通常の約10倍)以上は設定できません。\"}";
#endif
		success = FALSE;
	}
	else if(pejorationData.percent < 150)
	{
#ifdef USE_JSONCPP
		(*jErrors)[count]["id"] = "net_alt";
		(*jErrors)[count]["msg"] = "負荷設定値が小さいです。<br>150%(通常レスポンスの1.5倍)未満は設定できません。";
		count++;
#else
		errors += "{\"id\": \"net_alt\","
			" \"msg\": \"負荷設定値が小さいです。<br>150%(通常レスポンスの1.5倍)未満は設定できません。\"}";
#endif
		success = FALSE;
	}

	if(success == TRUE)
	{
		SetNetworkPejoration(pejorationData);
	}
#ifdef USE_JSONCPP
	CoString errors = jErrors->toFastString().c_str();
	jErrors->release();
#else
	errors += "]";
#endif

	return DisplayResult(http_socket, success, errors);
}

int DisplayMiscNotice(SOCKET http_socket,
								CoString& nc_pop,
								CoString& nc_mail,
								CoString& nc_mailadd,
								CoString& alert_type)
{
	BOOL success = TRUE;
#ifdef USE_JSONCPP
#else
	CoString errors = "[";
#endif

	// POSTではない場合はレジストリから設定値を取得
	if (nc_pop == "" && nc_mail == "" && nc_mailadd == "" && alert_type == "") {
		NOTICE_PEJORATION_DATA pejorationData;
		GetNoticePejoration(pejorationData);
#ifdef USE_JSONCPP
		Json::Value *jVal = create_AValue();
		(*jVal)[0u]["nc_pop"]     = (unsigned int)pejorationData.notice_pop;
		(*jVal)[0u]["nc_mail"]    = (unsigned int)pejorationData.notice_mail;
		(*jVal)[0u]["nc_mailadd"] = (const char *)pejorationData.notice_mailadd;
		(*jVal)[0u]["alert_type"] = (unsigned int)pejorationData.alert_type;
		CoString buffer = jVal->toFastString().c_str();

		jVal->release();
#else
		CoString buffer;
		buffer.Format("[{\"nc_pop\":%d,\"nc_mail\":%d,\"nc_mailadd\":\"%s\",\"alert_type\":%d }]",
									pejorationData.notice_pop, pejorationData.notice_mail, 
									pejorationData.notice_mailadd, pejorationData.alert_type);
#endif
		int len = buffer.GetLength();
		DisplayHttpHeader(http_socket, "text/plain; charset=utf-8",
											len, "200 OK", "private");
		int n = send(http_socket, buffer, len, 0);
		return len;
	}

#ifdef USE_JSONCPP
	Json::Value *jErrors = create_AValue();
#endif
	NOTICE_PEJORATION_DATA pejorationData;
	pejorationData.notice_pop = strtoul(nc_pop, NULL, 10);
	if(pejorationData.notice_pop != 0 && pejorationData.notice_pop != 1)
	{
		// 分からん値が返ってきた場合はOFFとする
		pejorationData.notice_pop = 0;
	}

	pejorationData.notice_mail = strtoul(nc_mail, NULL, 10);
	if(pejorationData.notice_mail != 0 && pejorationData.notice_mail != 1)
	{
		// 分からん値が返ってきた場合はOFFとする
		pejorationData.notice_mail = 0;
	}

	//nc_mailadd.Replace("送信先メールアドレス", "");
	pejorationData.notice_mailadd = nc_mailadd;
	if(pejorationData.notice_mail == 1 && nc_mailadd.Compare("") == 0)
	{
#ifdef USE_JSONCPP
		(*jErrors)[0u]["id"]  = "nc_mailadd";
		(*jErrors)[0u]["msg"] = "メールの送信を行う場合は必須項目です。";
#else
		errors += "{\"id\": \"nc_mailadd\","
							" \"msg\": \"メールの送信を行う場合は必須項目です。\"}, ";
#endif
		success = FALSE;
	}

	//SetAudioSound(alert_type == "0" ? 0 : 1);
	pejorationData.alert_type = strtoul(alert_type, NULL, 10);
	if(pejorationData.alert_type != 0 && pejorationData.alert_type != 1)
	{
		// 分からん値が返ってきた場合はWAV音とする
		pejorationData.alert_type = 0;
	}

	if(success == TRUE)
	{
		SetNoticePejoration(pejorationData);
	}

#ifdef USE_JSONCPP
	CoString errors = jErrors->toFastString().c_str();
	jErrors->release();
#else
	errors.TrimRight(", ");
	errors += "]";
#endif

	return DisplayResult(http_socket, success, errors);
}

int DisplayUpdateHistoryList(SOCKET http_socket, CoString data)
{
	CoString buffer = data;
	SJIStoUTF8(buffer, buffer);
	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayStopMcheineList(SOCKET http_socket, CoString data)
{
	CoString buffer = data;
	SJIStoUTF8(buffer, buffer);
	int len = buffer.GetLength();
	DisplayHttpHeader(http_socket, "text/javascript; charset=utf-8", len);
	int n = send(http_socket, buffer, len, 0);
	if (n < 1)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}
	return n;
}

int DisplayClientLog(SOCKET http_socket)
{
	CoString path = GetModuleFolder();
	path += "pExTrapperReport.txt";
	CFile file;
	try
	{
		if ((::GetFileAttributes(path) == 0xFFFFFFFF) ||
				(file.Open(path, CFile::modeRead
											 | CFile::shareDenyNone
											 | CFile::typeBinary) == 0))
		{
			// ファイルが無かったら、空テキストを表示
			DisplayHttpHeader(http_socket, "text/plain; charset=utf-8",
												0, "200 OK", "private");
			return 0;
		}

		// ファイルの読み出し
		CoString readBuffer;
		UINT readLength = (UINT)file.GetLength();
		if (readLength >= 4)
		{
			readLength -= 4;
		}
		if (file.Read(readBuffer.GetBufferSetLength(readLength), readLength) != readLength)
		{
			file.Close();
			return DisplayInternalServerError(http_socket); // failed get screenshot
		}

		// 出力情報の成型
		CoString outBuffer;
		outBuffer.Format("[ %s ]", (LPCTSTR)readBuffer);

		DisplayHttpHeader(http_socket, "text/plain; charset=utf-8",
										outBuffer.GetLength(), "200 OK", "private");

		int n = send(http_socket, outBuffer, outBuffer.GetLength(), 0);
		file.Close();
		return n;
	}
	catch(CFileException e)
	{
		DEBUG_OUT_DEFAULT("file read failed.");
		if (file != NULL) {
			file.Close();
		}
		return DisplayInternalServerError(http_socket); // file read error
	}
}

int GetKillTaskManagerType()
{
	int ret = 1;
	CoString data;
	GetSuppressionList(data);
	if(( data.Find("taskmgr*exe") != -1) && ( data.Find("procexp*exe") != -1))
		ret = 0;

	return ret;
}

int GetKillRegEditType()
{
	CoString data;
	GetSuppressionList(data);
	return data.Find("regedit*exe") == -1;
}

int GetKillP2PType()
{
	int ret = 1;
	CoString data;
	GetSuppressionList(data);

	if ((data.Find("winny*exe") != -1)
		&& (data.Find("share*exe") != -1)
		&& (data.Find("cabos*exe") != -1)
		&& (data.Find("bittorrent*exe") != -1)
		&& (data.Find("bitcomet*exe") != -1)
		&& (data.Find("shareaza*exe") != -1)
		&& (data.Find("torrent") != -1)
		&& (data.Find("limewire*exe") != -1)
		&& (data.Find("perfect*dark*exe") != -1)
		&& (data.Find("kazaa*exe") != -1)
		&& (data.Find("guntella*exe") != -1)
		&& (data.Find("rufus*exe") != -1)
		&& (data.Find("wxdfast*exe") != -1)
		&& (data.Find("^webdownload.exe$") != -1)
		&& (data.Find("^sdp.exe$") != -1)
		&& (data.Find("^dep.exe$") != -1)
		&& (data.Find("flvdownloader*exe") != -1)
		&& (data.Find("flashget*exe") != -1)
		&& (data.Find("^dsdl.exe$") != -1)
		&& (data.Find("gigaget*exe") != -1)
		&& (data.Find("^bukster.exe$") != -1)
		&& (data.Find("^daman.exe$") != -1)
		&& (data.Find("downup2u*exe") != -1)
		&& (data.Find("^fdm.exe$") != -1)
		&& (data.Find("^hidownload*exe$") != -1)
		&& (data.Find("leechget*exe") != -1)
		&& (data.Find("neodownloader*exe") != -1)
		&& (data.Find("^orbitnet.exe$") != -1)
		&& (data.Find("^webripper.exe$") != -1)
		&& (data.Find("^stardown.exe$") != -1)
		&& (data.Find("truedownloader*exe") != -1)
		&& (data.Find("^ultraget.exe$") != -1)
		&& (data.Find("^wellget.exe$") != -1)
		&& (data.Find("^grab.exe$") != -1)
		&& (data.Find("^irvine.exe$") != -1)
		&& (data.Find("^UPnPCJ.exe$") != -1)
		// 久々に以下追加 2009/10/23
		&& (data.Find("^Getter1.exe$") != -1)
		&& (data.Find("^NetAnts.exe$") != -1)
		&& (data.Find("^Phex.exe$") != -1)
		&& (data.Find("^FrostWire.exe$") != -1)
		&& (data.Find("^Azureus.exe$") != -1)
		&& (data.Find("^kazaalite.kpp$") != -1)
		&& (data.Find("^emule.exe$") != -1)
		&& (data.Find("^btdna.exe$") != -1)
		)
	{
		ret = 0;
	}

	return ret;
}

void SaveKillTaskManagerType(CoString type) {
	CoString data;
	GetSuppressionList(data);
	if (type == "1") {
		CoStringArray value;
		GetArrayValue(value, data, "\\,");

		// ポリシーとしてTrapperで追加したものに関してはメンテする
		int idx = -1;
		if ((idx = value.Find("taskmgr*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("procexp*exe")) != -1) {
			value.RemoveAt(idx);
		}

		data.Empty();
		GetStringValue(data, value, "\\,");

	} else {
		if (data != "") {
			data += "\\,";
		}
		data += "taskmgr*exe\\,procexp*exe";
	}
	SetSuppressionList(data);
}

void SaveKillRegEditType(CoString type) {
	CoString data;
	GetSuppressionList(data);
	if (type == "1") {
		CoStringArray value;
		GetArrayValue(value, data, "\\,");

		int idx = -1;
		if ((idx = value.Find("regedit*exe")) != -1) {
			value.RemoveAt(idx);
		}

		data.Empty();
		GetStringValue(data, value, "\\,");

	} else {
		if (data != "") {
			data += "\\,";
		}
		data += "regedit*exe";
	}
	SetSuppressionList(data);
}

void SaveKillP2PType(CoString type) {
	CoString data;
	GetSuppressionList(data);
	if (type == "1") {
		CoStringArray value;
		GetArrayValue(value, data, "\\,");

		int idx = -1;
		if ((idx = value.Find("winny*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("share*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("cabos*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("bittorrent*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("bitcomet*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("shareaza*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("torrent")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("limewire*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("perfect*dark*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("kazaa*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("guntella*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("rufus*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("wxdfast*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^webdownload.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^sdp.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^dep.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("flvdownloader*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("flashget*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^dsdl.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("gigaget*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^bukster.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^daman.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("downup2u*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^fdm.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^hidownload*exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("leechget*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("neodownloader*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^orbitnet.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^webripper.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^stardown.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("truedownloader*exe")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^ultraget.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^wellget.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^grab.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^irvine.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^UPnPCJ.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^Getter1.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^NetAnts.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^Phex.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^FrostWire.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^Azureus.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^kazaalite.kpp$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^emule.exe$")) != -1) {
			value.RemoveAt(idx);
		}
		if ((idx = value.Find("^btdna.exe$")) != -1) {
			value.RemoveAt(idx);
		}

		data.Empty();
		GetStringValue(data, value, "\\,");

	} else {
		if (data != "") {
			data += "\\,";
		}
		data += "winny*exe\\,share*exe\\,cabos*exe\\,bittorrent*exe\\,bitcomet*exe\\,shareaza*exe\\,torrent\\,limewire*exe\\,perfect*dark*exe\\,kazaa*exe\\,guntella*exe\\,rufus*exe\\,";
		data += "wxdfast*exe\\,^webdownload.exe$\\,^sdp.exe$\\,^dep.exe$\\,flvdownloader*exe\\,flashget*exe\\,^dsdl.exe$\\,gigaget*exe\\,^bukster.exe$\\,^daman.exe$\\,downup2u*exe\\,^fdm.exe$\\,^hidownload*exe$\\,";
		data += "leechget*exe\\,neodownloader*exe\\,^orbitnet.exe$\\,^webripper.exe$\\,^stardown.exe$\\,truedownloader*exe\\,^ultraget.exe$\\,^wellget.exe$\\,^grab.exe$\\,^irvine.exe$\\,^UPnPCJ.exe$\\,^Getter1.exe$\\,";
		data += "^NetAnts.exe$\\,^Phex.exe$\\,^FrostWire.exe$\\,^Azureus.exe$\\,^kazaalite.kpp$\\,^emule.exe$\\,^btdna.exe$";
	}
	SetSuppressionList(data);
}


int DisplayMisc(SOCKET http_socket, CoString& tm_type,CoString& re_type, CoString& p2p_type)
{
	if (tm_type == ""  || re_type == "" || p2p_type == "") {
#ifdef USE_JSONCPP
		Json::Value *jVal = create_AValue();
		(*jVal)[0u]["tm_type"]  = GetKillTaskManagerType();
		(*jVal)[0u]["re_type"]  = GetKillRegEditType();
		(*jVal)[0u]["p2p_type"] = GetKillP2PType();
		CoString buffer = jVal->toFastString().c_str();

		jVal->release();
#else
		CoString buffer;
		buffer.Format("[{\"tm_type\":%d,\"re_type\":%d,\"p2p_type\":%d}]",
			GetKillTaskManagerType(), GetKillRegEditType(), GetKillP2PType());
#endif
		int len = buffer.GetLength();
		DisplayHttpHeader(http_socket, "text/plain; charset=utf-8",
											len, "200 OK", "private");
		int n = send(http_socket, buffer, len, 0);
		return len;
	}

	SaveKillTaskManagerType(tm_type);
	SaveKillRegEditType(re_type);
	SaveKillP2PType(p2p_type);
	// 管理設定の自動停止リストの送信
	LRESULT UploadSuppressionListData();
	UploadSuppressionListData();

	return DisplayResult(http_socket, TRUE);
}

