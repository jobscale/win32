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

#include "oWndFind.h"

#include "pProcess.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "pUtility.h"

#include "oMyJson.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

struct ANALYZE_FORE_DATA
{
	CoString key;
	CoString date_text;
	CoString pname_text;
	CoString window_text;
	CoString class_name;
	DWORD count;
	COleDateTimeSpan span;
	ANALYZE_FORE_DATA()
		: count(0)
	{
	}
	BOOL operator==(const ANALYZE_FORE_DATA& other) const
	{
		return key.CompareNoCase(other.key) == 0;
	}
};

class CoSummaryAnalyzeForeDataArray : public CoArray<ANALYZE_FORE_DATA>
{
public:
	INT Append(ANALYZE_FORE_DATA& target)
	{
		int find = Find(target);
		if (find == -1)
		{
			Add(target);
		}
		else
		{
			ElementAt(find).count += target.count;
			ElementAt(find).span += target.span;
		}
		return 0;
	}
};

// フォアグラウンドアナライズ情報の保存
LRESULT SaveAnalyzeForeData(CoAutoFile& file, const COleDateTime& now, const ANALYZE_FORE_DATA& data)
{
	CoString text;
	text.Format("console fore data ver100 \"%s\",\"%s\",\"%s\",\"%s\",\"%d\",\"%s\"\n",
		(LPCTSTR)now.Format("%Y/%m/%d"), (LPCTSTR)data.pname_text, (LPCTSTR)data.window_text, (LPCTSTR)data.class_name, 1, data.span.Format("%H:%M:%S"));

	CoString base64data;
	base64_encode(text, base64data, 5);

	file.Write(base64data);
	file.Write(RETERCODE);

	return 0;
}

// フォアグラウンドアナライズ情報の受け取り
HRESULT CatchAnalyzeForeData(const CoString& recvData)
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	LPCTSTR GetAnalyzeForeDataPath(const COleDateTime& now);
	CoString analyzePath = GetAnalyzeForeDataPath(now);
	CoAutoFile file(analyzePath, GENERIC_ALL);
	if (!file.IsOpen())
	{
		return -1;
	}
	file.SeekEnd();

	// 最後のページ
	INT lastPage = 0;
	// プロセスリストのアナライズ情報
	INT pos = 0;

	for (DWORD i = 0; ; i++)
	{
		CoString countData = recvData.AbstractSearchSubstance(pos);
		LPCTSTR textPos = countData;
		DWORD row = strtoul(&textPos[0], NULL, 10);
		DWORD rowTotal = strtoul(&textPos[7], NULL, 10);
		if (lastPage == 0)
		{
			lastPage = rowTotal;
		}
		else if (lastPage != rowTotal)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}

		if (row != i)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		if (i >= rowTotal)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}

		ANALYZE_FORE_DATA cmdData;
		CoString macaddr = recvData.AbstractSearchSubstance(pos);
		CoString ipaddr = recvData.AbstractSearchSubstance(pos);
		CoString hostname = recvData.AbstractSearchSubstance(pos);
		cmdData.pname_text = recvData.AbstractSearchSubstance(pos);
		cmdData.window_text = recvData.AbstractSearchSubstance(pos);
		cmdData.class_name = recvData.AbstractSearchSubstance(pos);
		CoString beginText = recvData.AbstractSearchSubstance(pos);
		CoString spanText = recvData.AbstractSearchSubstance(pos);
		if (spanText.GetLength() != 8)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		cmdData.span.SetDateTimeSpan(
			0,
			strtoul(&((LPCTSTR)spanText)[0], NULL, 10),
			strtoul(&((LPCTSTR)spanText)[3], NULL, 10),
			strtoul(&((LPCTSTR)spanText)[6], NULL, 10)
			);

		SaveAnalyzeForeData(file, now, cmdData);

		// 終わり判定
		if (row == (rowTotal - 1))
		{
			// 一応確認
			CoString checkTect = recvData.AbstractSearchSubstance(pos);
			if (!checkTect.IsEmpty())
			{
				DEBUG_OUT_DEFAULT("failed.");
				return -1;
			}
			break;
		}
	}

	return 0;
}

// フォアグラウンドアナライズ情報を管理端末に送信
LRESULT SendAnalyzeForeData(const CoString& sendData)
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
		return CatchAnalyzeForeData(sendData);
	}

	CoString recvData;
	DataSendConsole(masterConsoleIP, 12084, sendData, &recvData);
	if (recvData.Find("TRANSLATE 14 DATA SUCCEEDED ") == -1)
	{
		return -1;
	}

	return 0;
}

// フォアグラウンドアナライズ情報のロード
LRESULT LoadClientAnalyzeForeData()
{
	LPCTSTR GetAnalyzeDataDir();
	CoString analyzeDir = GetAnalyzeDataDir();

	LPCTSTR GetClientAnalyzeForeDataBasePath();
	CoString clientAnalyzePathList = GetClientAnalyzeForeDataBasePath();

	clientAnalyzePathList += "*.fan";

	WIN32_FIND_DATA wfd = { 0 };
	HANDLE hFind = NULL;
	BOOL bNext = TRUE;
	for (hFind = FindFirstFile(clientAnalyzePathList, &wfd); hFind != INVALID_HANDLE_VALUE && bNext; bNext = FindNextFile(hFind, &wfd))
	{
		CoString target = analyzeDir + "\\";
		target += wfd.cFileName;

		HANDLE hFile = CreateFile(target, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwReadHigh = 0;
			DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
			CoString base64;
			ReadFile(hFile, base64.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
			CloseHandle(hFile);

			int result = 0;
			if (dwRead != 0)
			{
				CoString text;
				base64_decode(base64, text);
				result = SendAnalyzeForeData(text);
			}

			if (result == 0)
			{
				DeleteFile(target);
			}
		}
	}
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
	}

	return 0;
}

// フォアグラウンドアナライズ情報のサマリー
LRESULT SummaryAnalyzeFore(const CoStringArray& value, CoSummaryAnalyzeForeDataArray& dest)
{
	ANALYZE_FORE_DATA anadata;
	CoString buffer;
	for (DWORD i = 0, count = value.GetSize(); i < count; i++)
	{
		base64_decode(value.ElementAt(i), buffer, 5);
		int pos = 0;
		anadata.date_text = buffer.AbstractSearchSubstance(pos);
		anadata.pname_text = buffer.AbstractSearchSubstance(pos);
		anadata.window_text = buffer.AbstractSearchSubstance(pos);
		anadata.class_name = buffer.AbstractSearchSubstance(pos);
		if (anadata.pname_text.CompareNoCase(P("iexplore.exe")) == 0)
		{
			anadata.key.Format("%s %s %s", (LPCTSTR)anadata.date_text, (LPCTSTR)anadata.pname_text, (LPCTSTR)anadata.class_name);
		}
		else
		{
			anadata.key.Format("%s %s %s %s", (LPCTSTR)anadata.date_text, (LPCTSTR)anadata.pname_text, (LPCTSTR)anadata.window_text, (LPCTSTR)anadata.class_name);
		}
		CoString count_text = buffer.AbstractSearchSubstance(pos);
		anadata.count = strtoul(count_text, NULL, 10);
		CoString spanText = buffer.AbstractSearchSubstance(pos);
		anadata.span.SetDateTimeSpan(
			0,
			strtoul(&((LPCTSTR)spanText)[0], NULL, 10),
			strtoul(&((LPCTSTR)spanText)[3], NULL, 10),
			strtoul(&((LPCTSTR)spanText)[6], NULL, 10)
			);

		dest.Append(anadata);
	}

	return 0;
}

// フォアグラウンドアナライズ情報のアップロード
int TransactionAnalyzeFore(const CoString& text)
{
	CoStringArray tmpdata;
	GetArrayValue(tmpdata, text, "\n", TRUE);

	CoSummaryAnalyzeForeDataArray value;
	SummaryAnalyzeFore(tmpdata, value);

	if (value.IsEmpty())
	{
		return 0;
	}

	COMMAND_CTRL_DATA cmdData;
	GetCommandCtrlData(cmdData);

	CoString uniq_text = GetShopUnique();
#ifdef USE_JSONCPP
	Json::Value *jVal = create_AValue();
	CoString tmpstr;
#else
	CoString sendData = "[";
#endif
	for (DWORD i = 0, count = value.GetCount(); i < count; i++)
	{
		value.ElementAt(i).date_text.Replace("/", "-");

		CoString spanText = value.ElementAt(i).span.Format("%H:%M:%S");
		DWORD span = strtoul(&((LPCTSTR)spanText)[3], NULL, 10) + strtoul(&((LPCTSTR)spanText)[0], NULL, 10) * 60;

		CoString& processName = value.ElementAt(i).pname_text;
		CoString& className = value.ElementAt(i).class_name;
		CoString& windowText = value.ElementAt(i).window_text;
		windowText.Replace("\\", "/");

#ifdef USE_JSONCPP
		(*jVal)[i]["i"] = (const char *)cmdData.ipaddr;
		(*jVal)[i]["d"] = (const char *)value.ElementAt(i).date_text;
		(*jVal)[i]["p"] = (const char *)processName;
		(*jVal)[i]["n"] = (const char *)className;
		(*jVal)[i]["t"] = (const char *)windowText;
		tmpstr.Format("%d", value.ElementAt(i).count);
		(*jVal)[i]["c"] = tmpstr;
		tmpstr.Format("%d", span);
		(*jVal)[i]["s"] = tmpstr;
#else
		CoString data_text;
		data_text.Format("{\"i\": \"%s\", \"d\": \"%s\", \"p\": \"%s\", \"n\": \"%s\", \"t\": \"%s\", \"c\": \"%d\", \"s\": \"%d\"}, ",
			(LPCTSTR)cmdData.ipaddr,
			(LPCTSTR)value.ElementAt(i).date_text,
			(LPCTSTR)processName,
			(LPCTSTR)className,
			(LPCTSTR)windowText,
			value.ElementAt(i).count,
			span);

		sendData += data_text;
#endif
	}
#ifdef USE_JSONCPP
#else
	if (value.GetCount())
	{
		sendData.GetBufferSetLength(sendData.GetLength() - 2);
	}
	sendData += "]";
#endif

	CoString shopName = GetShopLicense();
	SJIStoUTF8(shopName, shopName);

#ifdef USE_JSONCPP
	CoString sendData = jVal->toFastString().c_str();

	Json::Value *jHead = create_AValue();
	LPCTSTR GetThisVersion();
	(*jHead)[0u]["v"]        = "0001";
	(*jHead)[0u]["sv"]       = GetThisVersion();
	(*jHead)[0u]["e"]        = "";
	(*jHead)[0u]["u"]        = (const char *)uniq_text;
	(*jHead)[0u]["shopName"] = (const char *)shopName;
	tmpstr.Format("%d", value.GetCount());
	(*jHead)[0u]["c"]        = tmpstr;
	CoString headData = jHead->toFastString().c_str();

	jVal->release();
	jHead->release();
#else
	CoString headData;
	LPCTSTR GetThisVersion();
	headData.Format("[{\"v\": \"0001\",\"sv\":\"%s\", \"e\": \"\", \"u\": \"%s\", \"shopName\": \"%s\", \"c\": \"%d\"}]",
		GetThisVersion(), (LPCTSTR)uniq_text, (LPCTSTR)shopName, value.GetCount());
#endif

	CoString base64Data;
	base64_encode(sendData, base64Data, 5);

	CoString base64Head;
	base64_encode(headData, base64Head, 5);

	int encodeUrl(char* outBuf, const char* str);

	CoString urlText;
	encodeUrl(urlText.GetBuffer(base64Data.GetLength() * 3 + 1), base64Data);
	urlText.ReleaseBuffer();

	CoString headText;
	encodeUrl(headText.GetBuffer(base64Head.GetLength() * 3 + 1), base64Head);
	headText.ReleaseBuffer();

	OHTTP_DATA httpData;
	httpData.target = GetAnaForeURL();
	httpData.sendData.Format(P("head=%s&&data=%s"), (LPCTSTR)headText, (LPCTSTR)urlText);

	HttpPost(&httpData);
	if (httpData.recvData.Find("TRANSLATE ANALYZE DATA SUCCEEDED ") == -1)
	{
		DEBUG_OUT_DEFAULT((LPCTSTR)httpData.recvData);
		return -1;
	}

	return 0;
}

// フォアグラウンドアナライズ情報のアップロード
LRESULT UploadAnalyzeForeData()
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	LPCTSTR GetAnalyzeForeDataPath(const COleDateTime& now);
	CoString analyzePath = GetAnalyzeForeDataPath(now);

	LPCTSTR GetAnalyzeDataDir();
	CoString analyzeDir = GetAnalyzeDataDir();

	LPCTSTR GetAnalyzeForeDataBasePath();
	CoString analyzePathList = GetAnalyzeForeDataBasePath();

	analyzePathList += "*.fan";

	// 前日までのデータをサーバに送信
	WIN32_FIND_DATA wfd = { 0 };
	HANDLE hFind = NULL;
	BOOL bNext = TRUE;
	for (hFind = FindFirstFile(analyzePathList, &wfd); hFind != INVALID_HANDLE_VALUE && bNext; bNext = FindNextFile(hFind, &wfd))
	{
		CoString target = analyzeDir + "\\";
		target += wfd.cFileName;

		if (analyzePath.CompareNoCase(target) == 0)
		{
			// 本日分は除外
			continue;
		}
		HANDLE hFile = CreateFile(target, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwReadHigh = 0;
			DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
			CoString text;
			ReadFile(hFile, text.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);
			int result = TransactionAnalyzeFore(text);
			CloseHandle(hFile);
			if (result == 0)
			{
				DeleteFile(target);
			}
			Sleep(9 * 1000); // サーバの負荷分散
		}
	}
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
	}

	return 0;
}

// フォアグラウンドアナライズ情報の作成
HRESULT GetForeAnalyze(CoString& data)
{
	CRITICALTRACE(oReceiveBCast, GetForeAnalyze);

	CCriticalSection& GetForeCriticalSection();
	CoCriticalSectionCtrl critical(GetForeCriticalSection());

	CoWndFind& GetWndFindCtrl();
	CoWndFind& cwf = GetWndFindCtrl();

	cwf.FinalizeAll();

	if (cwf.GetSize() == 0)
	{
		return 0;
	}

	COMMAND_CTRL_DATA cmdData;
	GetCommandCtrlData(cmdData);

	COLUMN_WND* pcw = cwf.GetFirst();

	CoString work;

	data.Format("TCP 14 DATA ");
	for (INT i = 0, count = cwf.GetSize(); i < count; i++)
	{
		work.Format(" data \"%06d/%06d\"", i, count);
		data += work;
		work.Format(" macaddr \"%s\"", (LPCTSTR)cmdData.macaddr);
		data += work;
		work.Format(" ipaddr \"%s\"", (LPCTSTR)cmdData.ipaddr);
		data += work;
		work.Format(" hostname \"%s\"", (LPCTSTR)cmdData.hostname);
		data += work;
		work.Format(" process \"%s\"", pcw->path);
		data += work;
		work.Format(" title \"%s\"", pcw->title);
		data += work;
		work.Format(" classname \"%s\"", pcw->className);
		data += work;
		work = pcw->begin.Format(" begin \"%Y/%m/%d %H:%M:%S\"");
		data += work;
		work = pcw->span.Format(" span \"%H:%M:%S\"");
		data += work;

		pcw = cwf.GetNext(pcw);
	}

	cwf.RemoveAll();

	return 0;
}

// フォアグラウンドアナライズ情報のバックアップ
LRESULT BackupAnalyzeForeData(const CoString& data)
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	LPCTSTR GetClientAnalyzeForeDataPath(const COleDateTime& now, const DWORD random);
	CoString path = GetClientAnalyzeForeDataPath(now, (DWORD)&data);

	CoAutoFile file(path, GENERIC_ALL);
	if (!file.IsOpen())
	{
		DEBUG_OUT_DEFAULT("BackupAnalyzeForeData failed.");
		return -1;
	}

	CoString base64;
	base64_encode(data, base64);
	file.Write(base64);

	return 0;
}

// フォアグラウンドアナライズ情報を管理端末に送信
LRESULT SendAnalyzeForeData()
{
	CRITICALTRACE(oReceiveBCast, SendAnalyzeForeData);

	CoString sendData;
	GetForeAnalyze(sendData);

	if (sendData.IsEmpty())
	{
		return 0;
	}

	SJIStoUTF8(sendData, sendData);

	if (SendAnalyzeForeData(sendData) != 0)
	{
		return BackupAnalyzeForeData(sendData);
	}

	return 0;
}
