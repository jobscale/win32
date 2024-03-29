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
#include "../oCommon/oEthernet.h"
#include "../oCommon/oBase64.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "oHttpConf.h"
#include "oCpuUsage.h"

#include "pProcess.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "pUtility.h"
#include "pLook.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

extern "C" _declspec(dllexport) int Update_SystemReg(SYSTEM_REG *pData)
{
	HKEY hKey = NULL;
	DWORD dwDisposition = 0;

	// UsbStor USBストレージのポリシー
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\UsbStor", 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
	{
		OutputViewer("ERROR RegCreateKeyEx UsbStor failed.");
	}
	else
	{
		// no_usb_storage
		DWORD value = pData->no_usb_storage ? 4 : 3;
		if (RegSetValueEx(hKey, "Start", 0, REG_DWORD, (LPBYTE)&value, sizeof value) != ERROR_SUCCESS)
		{
			OutputViewer("ERROR RegSetValueEx UsbStor Start failed.");
		}
		RegCloseKey(hKey);
	}

	// NoAddPrinter プリンタの追加を禁止する
	// NoDeletePrinter プリンタの削除を禁止する
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
	{
		OutputViewer("ERROR RegCreateKeyEx Policies\\Explorer failed.");
	}
	else
	{
		// no_add_printer
		if (pData->no_add_printer)
		{
			DWORD value = 1;
			if (RegSetValueEx(hKey, "NoAddPrinter", 0, REG_DWORD, (LPBYTE)&value, sizeof value) != ERROR_SUCCESS)
			{
				OutputViewer("ERROR RegSetValueEx Policies\\Explorer NoAddPrinter failed.");
			}
		}
		else
		{
			if (RegDeleteValue(hKey, "NoAddPrinter") != ERROR_SUCCESS)
			{
				OutputViewer("ERROR RegDeleteValue Policies\\Explorer NoAddPrinter failed.");
			}
		}
		// no_delete_printer
		if (pData->no_delete_printer)
		{
			DWORD value = 1;
			if (RegSetValueEx(hKey, "NoDeletePrinter", 0, REG_DWORD, (LPBYTE)&value, sizeof value) != ERROR_SUCCESS)
			{
				OutputViewer("ERROR RegSetValueEx Policies\\Explorer NoDeletePrinter failed.");
			}
		}
		else
		{
			if (RegDeleteValue(hKey, "NoDeletePrinter") != ERROR_SUCCESS)
			{
				OutputViewer("ERROR RegDeleteValue Policies\\Explorer NoDeletePrinter failed.");
			}
		}
		RegCloseKey(hKey);
	}

	return 0;
}

extern "C" _declspec(dllexport) int Read_SystemReg(SYSTEM_REG *pData)
{
	pData->no_usb_storage = MyGetProfileDWORD(P("Setting"), P("no_usb_storage"), 0);
	pData->no_add_printer = MyGetProfileDWORD(P("Setting"), P("no_add_printer"), 0);
	pData->no_delete_printer = MyGetProfileDWORD(P("Setting"), P("no_delete_printer"), 0);
	pData->disable_print = MyGetProfileDWORD(P("Setting"), P("disable_print"), 0);

	return 0;
}

extern "C" _declspec(dllexport) int Write_SystemReg(SYSTEM_REG *pData)
{
	MyWriteProfileDWORD(P("Setting"), P("no_usb_storage"), pData->no_usb_storage);
	MyWriteProfileDWORD(P("Setting"), P("no_add_printer"), pData->no_add_printer);
	MyWriteProfileDWORD(P("Setting"), P("no_delete_printer"), pData->no_delete_printer);
	MyWriteProfileDWORD(P("Setting"), P("disable_print"), pData->disable_print);

	Update_SystemReg(pData);

	return 0;
}

// インターネットページの取得
DWORD GetInternet(OHTTP_DATA* pHttpData)
{
	INT retryCount = 2;

	int result = -1;
	for (INT i = 0, count = retryCount; i < count; i++)
	{
		result = HttpGet(pHttpData);
		if (result >= 0)
		{
			break;
		}
		OutputViewer("ERROR: get internet failed. retry %d [%s].", count - i - 1, pHttpData->target.GetBuffer(0));
	}

	return result >= 0 ? 0 : -1;
}

// 過去のネットワーク速度平均から現在のネットワーク率を算出
class CoNetworkTime : public CoArray<DWORD>
{
public:
	DOUBLE GetInternetPercent(DWORD target)
	{
		DWORD total = 0;
		INT count = GetSize();
		for (INT i = 0; i < count; i++)
		{
			total += operator[](i);
		}
		// 少なくとも過去10回分の情報が必要
		if (count > 10 && total > 0)
		{
			return (DOUBLE)target / ((DOUBLE)total / count);
		}
		return 1.00;
	}
};

enum SITE_TYPE
{
	SITE_SJIS,
	SITE_UTF8,
	SITE_EUC,
};

struct URL_SITE_TABLE
{
	SITE_TYPE type;
	LPCTSTR site;
	LPCTSTR query;
};

static URL_SITE_TABLE _siteTable[] =
{
	// utf-8
	SITE_UTF8, P("www.google.co.jp/search"), P("?hl=ja&lr=lang_ja&q="),
	SITE_UTF8, P("www.bing.com/search"), P("?q="),
	SITE_UTF8, P("search.yahoo.com/search"), P("?n=40&ei=UTF-8&p="),
	SITE_UTF8, P("search.yahoo.co.jp/search"), P("?ei=UTF-8&n=40&va="),
	SITE_UTF8, P("search.www.infoseek.co.jp/Web"), P("?qt="),
	SITE_UTF8, P("www.youtube.com/results"), P("?search_query="),
	SITE_UTF8, P("blog-search.yahoo.co.jp/search"), P("?cop=&ei=UTF-8&p="),
	SITE_UTF8, P("blogsearch.google.co.jp/blogsearch"), P("?hl=ja&ie=UTF-8&lr=lang_ja&q="),
	SITE_UTF8, P("blogsearch.livedoor.com/search/article"), P("?q="),
	SITE_UTF8, P("music-search.yahoo.co.jp/search"), P("?ei=UTF-8&rkf=1&p="),
	SITE_UTF8, P("image-search.yahoo.co.jp/search"), P("?ei=UTF-8&p="),
	SITE_UTF8, P("nsearch.yahoo.co.jp/bin/query"), P("?ei=UTF-8&p="),

	// euc
	SITE_EUC, P("search.goo.ne.jp/web.jsp"), P("?DC=50&MT="),
	SITE_EUC, P("search.blogs.yahoo.co.jp/SEARCH/index.html"), P("?p="),
	SITE_EUC, P("cgi.search.biglobe.ne.jp/cgi-bin/search"), P("?num=20&op_q="),
	SITE_EUC, P("www.baidu.jp/s"), P("wd="),
	SITE_EUC, P("blog.baidu.jp/s"), P("?cl=3&ie=utf-8&bsm=1&tn=baidujpblog&meta=ja&wd="),
	SITE_EUC, P("image.baidu.jp/i"), P("?word="),
	SITE_EUC, P("kakaku.livedoor.com/keyword_search/"), P("?keyword="),

	// s-jis
	SITE_SJIS, P("www.excite.co.jp/search.gw"), P("?NumResults=20&search="),
	SITE_SJIS, P("www.bestprice.net/list.php"), P("?Keyword="),
	SITE_SJIS, P("www.amazon.co.jp/s/ref=nb_ss_"), P("?__mk_ja_JP=%83J%83%5E%83J%83i&field-keywords="),
};

struct SEARCH_DATA
{
	LPCTSTR search;
};

static SEARCH_DATA _searchTable[] =
{
	// フリーワード
	"freeline",
	"skates",
	"clip",
	"クリップ",
	"フリーライン",
	"スケート",
	"ネットカフェ",
	"フード",
	"座席",
	"空席",
	"検索",
	"注文",
	"料金",
	"最安",
	"安定",
	"統計",
	"ネカフェ",
	"ポス",
	"ポスシステム",
	"監視",
	"winny",
	"利用統計",
	"問い合わせ",
	"アプリケーション",
	"分析",
	"位置情報",

	// プラスター
	"plustar",
	"プラスター",

	// 製品
	"extrapper",
	"トラッパー",
	"posplus",
	"監視くん",
	"gatekeeper",

	// くにや
	"jobscale",
	"jsx.jp",
	"くにや",
	"焼肉",
	"ホルモン",
};

int MakeEncodeSearchKey(CoString& encsjis, CoString& encutf8, CoString& enceuc, const DWORD i)
{
	int encodeUrl(char* outBuf, const char* str);

	// sjis
	CoString sjis(_searchTable[i].search);
	encodeUrl(encsjis.GetBuffer(sjis.GetLength() * 3 + 1), sjis);
	encsjis.ReleaseBuffer();

	// utf8
	CoString utf8;
	SJIStoUTF8(sjis, utf8);
	encodeUrl(encutf8.GetBuffer(utf8.GetLength() * 3 + 1), utf8);
	encutf8.ReleaseBuffer();

	// euc
	CComBSTR euc;
	CComBSTR comsjis(sjis.GetLength() + 1);
	strcpy((LPTSTR)comsjis.m_str, sjis);
	SJIStoEUC(comsjis, euc);
	encodeUrl(enceuc.GetBuffer(euc.ByteLength() * 3 + 1), (LPCTSTR)euc.m_str);
	enceuc.ReleaseBuffer();

	return 0;
}

int __rand(){ srand((unsigned int)time(NULL)); return rand(); }

struct SEARCHKEY_DATA
{
	CoString str[3];
};

LRESULT MakeSearchKey(SEARCHKEY_DATA& searchKey)
{
	DWORD count = sizeof _searchTable / sizeof SEARCH_DATA;

	static const int _rand = __rand();
	DWORD i = rand();
	i %= count;

	SEARCHKEY_DATA data0;
	MakeEncodeSearchKey(data0.str[SITE_SJIS], data0.str[SITE_UTF8], data0.str[SITE_EUC], i);

	i = rand();
	i %= count;

	SEARCHKEY_DATA data1;
	MakeEncodeSearchKey(data1.str[SITE_SJIS], data1.str[SITE_UTF8], data1.str[SITE_EUC], i);

	searchKey.str[SITE_SJIS] = data0.str[SITE_SJIS] + "+" + data1.str[SITE_SJIS];
	searchKey.str[SITE_UTF8] = data0.str[SITE_UTF8] + "+" + data1.str[SITE_UTF8];
	searchKey.str[SITE_EUC] = data0.str[SITE_EUC] + "+" + data1.str[SITE_EUC];

	return 0;
}

LRESULT MakeUrl(CString& url, CString& query, const SEARCHKEY_DATA& data, const DWORD i)
{
	url = P("http://");
	url += _siteTable[i].site;

	query = _siteTable[i].query;
	query += data.str[_siteTable[i].type];

	return 0;
}

struct URL_DATA
{
	CoString url;
	HRESULT result;
	DWORD miliseconds;
	URL_DATA()
		: result(-1)
		, miliseconds(0)
	{
	}
	INT operator<(const URL_DATA& other)
	{
		return miliseconds < other.miliseconds;
	}
	INT operator>(const URL_DATA& other)
	{
		return miliseconds > other.miliseconds;
	}
};

class CoUrlArray : public CoSortArray<URL_DATA>
{
};

HRESULT FebInternetSite(LPDWORD pResponse)
{
	SEARCHKEY_DATA searchKey;
	MakeSearchKey(searchKey);

	URL_DATA urlData;
	CoUrlArray urlList;

	DWORD timeTotal = 0;
	DWORD successTotal = 0;
	DWORD timeStart = timeGetTime();
	DWORD count = sizeof _siteTable / sizeof URL_SITE_TABLE;
	for (DWORD i = 0; i < count; i++)
	{
		OHTTP_DATA httpData;

		MakeUrl(urlData.url, httpData.sendData, searchKey, i);
		httpData.target = urlData.url;
		urlData.result = GetInternet(&httpData);
		DWORD timeEnd = timeGetTime();

#ifdef _DEBUG
		if (0) // DEBUG: テスト用
		{
			void OutputViewerDirect(LPCTSTR lpszDirect);
			OutputViewer("Target=%s", (LPCTSTR)httpData.target);
			OutputViewer("Query=%s", (LPCTSTR)httpData.sendData);
			OutputViewerDirect((LPCTSTR)httpData.headerData);
			OutputViewer("ContentsLength=%d", httpData.contentsLength);
			OutputViewerDirect("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXURL\r\n");
			OutputViewerDirect((LPCTSTR)httpData.recvData);
			OutputViewerDirect("\r\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXURL\r\n");
		}
#endif

		if (urlData.result < 0)
		{
			timeStart = timeEnd;
			continue;
		}

		if (timeStart > timeEnd)
		{
			urlData.miliseconds = -1 - timeStart + timeEnd;
		}
		else
		{
			urlData.miliseconds = timeEnd - timeStart;
		}
		urlList.Add(urlData);
		timeStart = timeEnd;
	}

	urlList.Sort();

	DWORD total = 0;

	count = urlList.GetSize();

	if (count == 0)
	{
		return -1;
	}

	for (DWORD i = 0; i < count; i++)
	{
		total += urlList.ElementAt(i).miliseconds;
	}

	*pResponse = total / count;

	return 0;
}

DWORD GetInternetInformation(DOUBLE* pPercent = NULL)
{
	// 過去１時間のネットワーク時間
	static CoNetworkTime timeAverage;

	// 直近のネットワーク時間
	static DWORD timeData = 0;

	// 過去と比較した現在の使用率
	static DOUBLE percent = 1.00;

	// ネットワークエラーの保存
	static INT networkError = 0;

	if (pPercent)
	{
		*pPercent = percent;
		return networkError;
	}

	DWORD timeNow = 0;

	bool isAbnormal = false;
	bool isBusy     = false;
	static bool isAbnormalPrevious = false;
	static bool isBusyPrevious     = false;
	CCpuUsageForNT cpu;
	int cpuUsage;

	int nRetry = 2;
	for (int i = 0; i < nRetry; i++)
	{
		cpu.GetCpuUsageForNT(); //cpu利用率の計測開始
		networkError = FebInternetSite(&timeNow);
		cpuUsage = cpu.GetCpuUsageForNT(); //cpu利用率の計測終了
		isBusy = cpuUsage >= 98 ? true : false;

		if (networkError < 0)
		{
			continue;
		}

		// コメントアウトでログ出力を抑える
		//OutputViewer("i %d: milisec=[%d], cpu usage = %d%%", i, timeNow, cpuUsage);

		//busyの時は正常な値が取得できません。リトライせずに次の呼び出し時まで待つことにします
		//previous ->  FebInternetsite 中にbusyじゃなくなった時用
		if (isBusy || isBusyPrevious)
		{
			percent = 1.00; // 呼び出し元でポップアップを出さないためのダミー
			//OutputViewer("break by busy %s", isBusy ? "": "previous" );
			isAbnormal = true;
			break;
		}

		percent = timeAverage.GetInternetPercent(timeNow); // 異常な結果の場合はリトライ
		if (percent > 2.00)
		{
			if (i ==  nRetry - 1) { isAbnormal = true; }
			//OutputViewer("continue by network percent");
			continue;
		}
		//下のpercent の計算には、直前の結果も影響してくるので、直前に異常があれば無視
		//    (直前に異常で引っかかった場合に、今回正常でも引っかかってしまう場合がある)
		if (isAbnormalPrevious)
		{
			//OutputViewer("break by abnormal previous");
			break;
		}

		percent = timeAverage.GetInternetPercent((timeData + timeNow) / 2);
		break;
	}
	//OutputViewer("percent %.2f%%", percent );

	timeAverage.Add(timeNow);

	if (timeAverage.GetSize() > 60)
	{
		timeAverage.RemoveAt(0);
	}

	timeData = timeNow;
	isAbnormalPrevious = isAbnormal;
	isBusyPrevious = isBusy;

	return networkError;
}

// ポップアップメッセージと音声で情報を通知する
// status
// 0 DEFAULT
// 1 ATTENTION
// 2 CAUTION
// 3 ALERT
// 4 EMERGENCY
// 5 WARNING
// 6 ERROR
// 7 FAILED
DWORD PopupMessageWindow(LPCTSTR lpszText, LPCTSTR lpszLink = "", DWORD color = 2, DWORD width = 300, DWORD height = 300, DWORD status = 0, DWORD showSeconds = 0)
{
	CoString popMsg;
	popMsg.Format(P("popupmessage:%d:\"%d\" \"%d\" \"%d\" \"%s\" \"%s\" \"%d\""),
		status, color, width, height, lpszLink, lpszText, showSeconds);

	CoString recvData;
	DataSendConsole("localhost", 12087, popMsg, &recvData);

	return 0;
}

DWORD WINAPI GetInternetThread(LPVOID pParam)
{
	CWSAInterface wsa;

	// 2010-10-22
	for (; ; )
	{
		CoString hostname, ipaddr, ipmask, username, macaddr, adapterName;
		GetHostInfo(hostname, ipaddr, ipmask, username, macaddr, adapterName);

		if (macaddr.GetLength() > 10)
		{
			break;
		}

		Sleep(5 * 1000);
	}

	int nNetErr = 0;

	for (; ; )
	{
		GetInternetInformation();

		// ネットワーク悪化率
		NETWORK_PEJORATION_DATA pejorationData = { 0 };
		GetNetworkPejoration(pejorationData);
		DOUBLE pejorationPercent = pejorationData.percent;
		pejorationPercent /= 100;

		DOUBLE percent = 1.00;
		LRESULT result = GetInternetInformation(&percent);

		if (result < 0)
		{
			if (nNetErr == 0 && pejorationData.offline)
			{
				// 現在時刻
				COleDateTime now = COleDateTime::GetCurrentTime();
				CoString text;
				text.Format("ネットワークの接続が確認できなく\r\nなりましたのでお知らせします。\n\n\n");
				text += now.Format("%Y/%m/%d %H:%M");
				// alert
				PopupMessageWindow(text, HTTP_PLUSTAR[9], 1, 300, 200, 4);
			}
		}
		else if (percent >= pejorationPercent && pejorationData.pejoration)
		{
			if (nNetErr == 0)
			{
				// 現在時刻
				COleDateTime now = COleDateTime::GetCurrentTime();
				CoString text;
				text.Format("ネットワークの負荷が著しく\r\n悪化しましたのでお知らせします。\n\n\n");
				text += now.Format("%Y/%m/%d %H:%M");
				// alert
				PopupMessageWindow(text, "", 1, 300, 200, 5);
			}
		}
		else
		{
			nNetErr = 0;
		}

		Sleep(70 * 1000);
	}

	return 0;
}

BOOL GetSettingValue(const CoString& confData, LPCTSTR lpszKey, CoStringArray& value)
{
	CRITICALTRACE(conf, GetSettingValue);

	value.RemoveAll();

	INT nFind = confData.Find(lpszKey);
	if (nFind == -1)
	{
		return TRUE;
	}
	INT nBegin = confData.Find(P("%BEGIN%"), nFind);
	if (nBegin == -1)
	{
		return FALSE;
	}
	nBegin = confData.Find("\r\n", nBegin);
	if (nBegin == -1)
	{
		return FALSE;
	}
	nBegin += 2;
	INT nEnd = confData.Find(P("%END%"), nBegin);
	if (nEnd == -1)
	{
		return FALSE;
	}

	CoString body = (LPCTSTR)confData.Mid(nBegin, nEnd - nBegin);
	return GetArrayValue(value, body, "\r\n");
}

BOOL GetSettingValue(const CoString& confData, LPCTSTR lpszKey, LPTSTR value)
{
	CRITICALTRACE(conf, GetSettingValue);

	value[0] = NULL;

	LPCTSTR pos = strstr(confData, lpszKey);
	if (!pos)
	{
		return FALSE;
	}
	INT size = strlen(pos);
	if (size > SHRT_MAX)
	{
		size = SHRT_MAX;
	}
	CoString line;
	strncpy(line.GetBuffer(SHRT_MAX + 1), pos, size);
	line.GetBufferSetLength(size);
	size = strcspn(line, "\t ");
	LPTSTR pSel = &line.GetBuffer(0)[size];
	size = strspn(pSel, "\t ");
	if (pSel[size] != '=')
	{
		DEBUG_OUT_DEFAULT("failed.");
		return FALSE;
	}

	size++;
	size += strspn(&pSel[size], ("\0 \t ") + 2);
	LPTSTR begin = &pSel[size];
	size = strcspn(begin, ("\0 \r\n") + 2);
	begin[size] = NULL;
	strcpy(value, begin);

	return TRUE;
}

BOOL ReadConfFileData(HANDLE hFile, CoString& confData)
{
	CRITICALTRACE(conf, ReadConfFileData);

	CoString buffer;
	DWORD readied = 0;

	DWORD dwReadHigh = 0;
	DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	for (; ReadFile(hFile, buffer.GetBuffer(dwRead), dwRead, &readied, NULL) && readied; )
	{
		buffer.GetBufferSetLength(readied);
		confData += buffer;
	}

	// コメント行の削除
	LPTSTR p = confData.GetBuffer(0);
	for (; p && p[0] != NULL; )
	{
		INT n = strspn(p, ("\0 \t ") + 2);
		if (p[n] == '#')
		{
			LPTSTR pDel = &p[n];
			n = strcspn(pDel, ("\0 \n") + 2);
			if (n > 0)
			{
				memset(pDel, ' ', n);
			}
			p = &pDel[n + 1];
		}
		else
		{
			LPTSTR pCom = &p[n];
			n = strcspn(pCom, ("\0 \n") + 2);
			p = &pCom[n + 1];
		}
	}

	return TRUE;
};

// サーチエンジンの設定
INT SearchEngineSetting(LPCTSTR search_url)
{
	CoString keyword = search_url;
	keyword += ("\0 ?keyword=%s" + 2);

	HKEY  hKey;
	DWORD dwType = REG_SZ;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, ("\0 Software\\Microsoft\\Internet Explorer\\Main") + 2, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		if (RegSetValueEx(hKey, ("\0 Search Page") + 2 , NULL, dwType, (LPBYTE)(LPCTSTR)search_url, strlen(search_url)) != ERROR_SUCCESS )
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		if (RegSetValueEx(hKey, ("\0 Search Bar") + 2 , NULL, dwType, (LPBYTE)(LPCTSTR)search_url, strlen(search_url)) != ERROR_SUCCESS )
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		RegCloseKey(hKey);
	}
	else
	{
		DEBUG_OUT_DEFAULT("failed.");
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, ("\0 Software\\Microsoft\\Internet Explorer\\SearchURL") + 2, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		if (RegSetValueEx(hKey, ("\0 ") + 2 , NULL, dwType, (LPBYTE)(LPCTSTR)(keyword), keyword.GetLength()) != ERROR_SUCCESS )
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		RegCloseKey(hKey);
	}
	else
	{
		DEBUG_OUT_DEFAULT("failed.");
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, ("\0 Software\\Microsoft\\Internet Explorer\\Search") + 2, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		if (RegSetValueEx(hKey, ("\0 SearchAssistant") + 2 , NULL, dwType, (LPBYTE)(LPCTSTR)search_url, strlen(search_url)) != ERROR_SUCCESS )
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		RegCloseKey(hKey);
	}
	else
	{
		DEBUG_OUT_DEFAULT("failed.");
	}

	return 0;
}
