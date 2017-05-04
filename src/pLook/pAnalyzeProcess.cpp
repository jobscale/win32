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

#include "pProcess.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "pUtility.h"

#include "oMyJson.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

struct PCLDATA
{
	CoString macaddr;
	CoString ipaddr;
	CoString hostname;
};

struct PNPLOGGINGDATA : PCLDATA, PNPDATA
{
};

// プロセスアナライズ情報の保存
LRESULT SaveAnalyzeProcessData(CoAutoFile& file, const COleDateTime& now, const PNPLOGGINGDATA& data)
{
	CoString spanText = data.span.Format("%H:%M:%S");
	CoString text;
	text.Format("console pro data ver100 \"%s\",\"%s\",\"%s\",\"%d\",\"%s\"\n",
		data.ipaddr, (LPCTSTR)now.Format("%Y/%m/%d"), (LPCTSTR)data.szProcess, 1, (LPCTSTR)spanText);

	CoString base64data;
	base64_encode(text, base64data, 5);

	file.Write(base64data);
	file.Write(RETERCODE);

	return 0;
}

// プロセスアナライズ情報の受け取り
HRESULT CatchAnalyzeProcessData(const CoString& recvData)
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	LPCTSTR GetAnalyzeProcessDataPath(const COleDateTime& now);
	CoString analyzePath = GetAnalyzeProcessDataPath(now);
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

		PNPLOGGINGDATA cmdData;
		cmdData.macaddr = recvData.AbstractSearchSubstance(pos);
		cmdData.ipaddr = recvData.AbstractSearchSubstance(pos);
		cmdData.hostname = recvData.AbstractSearchSubstance(pos);
		cmdData.dwPID = strtoul(recvData.AbstractSearchSubstance(pos), NULL, 10);
		cmdData.wTask = (WORD)strtoul(recvData.AbstractSearchSubstance(pos), NULL, 10);
		cmdData.szProcess = recvData.AbstractSearchSubstance(pos);

		CoString beginText = recvData.AbstractSearchSubstance(pos);
		if (DateFromString(cmdData.begin, beginText, 19) != 0)
		{
			DEBUG_OUT_DEFAULT((LPCTSTR)beginText);
			return -1;
		}
		CoString spanText = recvData.AbstractSearchSubstance(pos);
		if (spanText.GetLength() != 8)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		textPos = spanText;
		SYSTEMTIME span = { 0 };
		span.wDay = 0;
		span.wHour = (WORD)strtoul(&textPos[0], NULL, 10);
		span.wMinute = (WORD)strtoul(&textPos[3], NULL, 10);
		span.wSecond = (WORD)strtoul(&textPos[6], NULL, 10);
		cmdData.span.SetDateTimeSpan(span.wDay, span.wHour, span.wMinute, span.wSecond);

		SaveAnalyzeProcessData(file, now, cmdData);

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

// プロセスアナライズ情報を管理端末に送信
LRESULT SendAnalyzeProcessData(const CoString& sendData)
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
		return CatchAnalyzeProcessData(sendData);
	}

	CoString recvData;
	DataSendConsole(masterConsoleIP, 12084, sendData, &recvData);
	if (recvData.Find("TRANSLATE 04 DATA SUCCEEDED ") == -1)
	{
		return -1;
	}

	return 0;
}

// プロセスアナライズ情報のロード
LRESULT LoadClientAnalyzeProcessData()
{
	LPCTSTR GetAnalyzeDataDir();
	CoString analyzeDir = GetAnalyzeDataDir();

	LPCTSTR GetClientAnalyzeProcessDataBasePath();
	CoString clientAnalyzePathList = GetClientAnalyzeProcessDataBasePath();

	clientAnalyzePathList += "*.pan";

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
				result = SendAnalyzeProcessData(text);
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

struct ANALYZE_PROCESS_DATA
{
	CoString key;
	CoString date_text;
	CoString pname_text;
	DWORD count;
	DWORD span;
	BOOL operator==(const ANALYZE_PROCESS_DATA& other) const
	{
		return key.CompareNoCase(other.key) == 0;
	}
};

class CoSummaryAnalyzeDataArray : public CoArray<ANALYZE_PROCESS_DATA>
{
public:
	INT Append(ANALYZE_PROCESS_DATA& target)
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

// プロセスアナライズ情報のサマリー
LRESULT SummaryAnalyzeProcess(const CoStringArray& value, CoSummaryAnalyzeDataArray& dest)
{
	ANALYZE_PROCESS_DATA anadata;
	CoString buffer;
	for (DWORD i = 0, count = value.GetSize(); i < count; i++)
	{
		base64_decode(value.ElementAt(i), buffer, 5);
		int pos = 0;
		CoString localip = buffer.AbstractSearchSubstance(pos);
		anadata.date_text = buffer.AbstractSearchSubstance(pos);
		anadata.pname_text = buffer.AbstractSearchSubstance(pos);
		anadata.key.Format("%s %s", (LPCTSTR)anadata.date_text, (LPCTSTR)anadata.pname_text);
		CoString count_text = buffer.AbstractSearchSubstance(pos);
		anadata.count = strtoul(count_text, NULL, 10);
		CoString span_text = buffer.AbstractSearchSubstance(pos);
		WORD hour = (WORD)strtoul(&((LPCTSTR)span_text)[0], NULL, 10);
		WORD minute = (WORD)strtoul(&((LPCTSTR)span_text)[3], NULL, 10);
		anadata.span = minute + hour * 60;

		dest.Append(anadata);
	}

	return 0;
}

// プロセスアナライズ情報のアップロード
int TransactionAnalyzeProcess(const CoString& text)
{
	CoStringArray tmpdata;
	GetArrayValue(tmpdata, text, "\n", TRUE);

	CoSummaryAnalyzeDataArray value;
	SummaryAnalyzeProcess(tmpdata, value);

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

#ifdef USE_JSONCPP
		(*jVal)[i]["i"] = (const char *)cmdData.ipaddr;
		(*jVal)[i]["d"] = (const char *)value.ElementAt(i).date_text;
		(*jVal)[i]["p"] = (const char *)value.ElementAt(i).pname_text;
		tmpstr.Format("%d", value.ElementAt(i).count);
		(*jVal)[i]["c"] = tmpstr;
		tmpstr.Format("%d", value.ElementAt(i).span);
		(*jVal)[i]["s"] = tmpstr;
#else
		CoString data_text;
		data_text.Format("{\"i\": \"%s\", \"d\": \"%s\", \"p\": \"%s\", \"c\": \"%d\", \"s\": \"%d\"}, ",
			(LPCTSTR)cmdData.ipaddr,
			(LPCTSTR)value.ElementAt(i).date_text,
			(LPCTSTR)value.ElementAt(i).pname_text,
			value.ElementAt(i).count,
			value.ElementAt(i).span);

		sendData += data_text;
#endif
	}
#ifdef USE_JSONCPP
	CoString sendData = jVal->toFastString().c_str();

	Json::Value *jHead = create_AValue();
	LPCTSTR GetThisVersion();
	(*jHead)[0u]["v"]  = "0001";
	(*jHead)[0u]["sv"] = GetThisVersion();
	(*jHead)[0u]["e"]  = "";
	(*jHead)[0u]["u"]  = (const char *)uniq_text;
	tmpstr.Format("%d", value.GetCount());
	(*jHead)[0u]["c"]  = tmpstr;
	CoString headData = jHead->toFastString().c_str();

	jVal->release();
	jHead->release();
#else
	if (value.GetCount())
	{
		sendData.GetBufferSetLength(sendData.GetLength() - 2);
	}
	sendData += "]";

	CoString headData;
	LPCTSTR GetThisVersion();
	headData.Format("[{\"v\": \"0001\", \"sv\": \"%s\", \"e\": \"\", \"u\": \"%s\", \"c\": \"%d\"}]",
		GetThisVersion(), (LPCTSTR)uniq_text, value.GetCount());
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
	httpData.target = GetAnaProURL();
	httpData.sendData.Format("\0 head=%s&&data=%s" + 2, (LPCTSTR)headText, (LPCTSTR)urlText);

	HttpPost(&httpData);
	if (httpData.recvData.Find("TRANSLATE ANALYZE DATA SUCCEEDED ") == -1)
	{
		DEBUG_OUT_DEFAULT((LPCTSTR)httpData.recvData);
		return -1;
	}

	return 0;
}

// プロセスアナライズ情報のアップロード
LRESULT UploadAnalyzeProcessData()
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	LPCTSTR GetAnalyzeProcessDataPath(const COleDateTime& now);
	CoString analyzePath = GetAnalyzeProcessDataPath(now);

	LPCTSTR GetAnalyzeDataDir();
	CoString analyzeDir = GetAnalyzeDataDir();

	LPCTSTR GetAnalyzeProcessDataBasePath();
	CoString analyzePathList = GetAnalyzeProcessDataBasePath();

	analyzePathList += "*.pan";

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
			int result = TransactionAnalyzeProcess(text);
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

// プロセスアナライズ情報の作成
HRESULT GetProcessAnalyze(CoString& data)
{
	CRITICALTRACE(oReceiveBCast, GetProcessAnalyze);

	CCriticalSection& GetProcessCriticalSection();
	CoCriticalSectionCtrl critical(GetProcessCriticalSection());

	CoProcessArray& GetProcessCtrl();
	CoProcessArray& analyze = GetProcessCtrl();

	if (analyze.GetSize() == 0)
	{
		return 0;
	}

	COMMAND_CTRL_DATA cmdData;
	GetCommandCtrlData(cmdData);

	CoString work;

	data.Format("TCP 04 DATA ");
	for (INT i = 0, count = analyze.GetSize(); i < count; i++)
	{
		work.Format(" data \"%06d/%06d\"", i, count);
		data += work;
		work.Format(" macaddr \"%s\"", (LPCTSTR)cmdData.macaddr);
		data += work;
		work.Format(" ipaddr \"%s\"", (LPCTSTR)cmdData.ipaddr);
		data += work;
		work.Format(" hostname \"%s\"", (LPCTSTR)cmdData.hostname);
		data += work;
		work.Format(" pid \"%u\"", analyze[i].dwPID);
		data += work;
		work.Format(" task \"%u\"", analyze[i].wTask);
		data += work;
		work.Format(" process \"%s\"", (LPCTSTR)analyze[i].szProcess);
		data += work;
		work = analyze[i].begin.Format(" begin \"%Y/%m/%d %H:%M:%S\"");
		data += work;
		work = analyze[i].span.Format(" span \"%H:%M:%S\"");
		data += work;
	}

	analyze.RemoveAll();

	return 0;
}

// プロセスアナライズ情報のバックアップ
LRESULT BackupAnalyzeProcessData(const CoString& data)
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	LPCTSTR GetClientAnalyzeProcessDataPath(const COleDateTime& now, const DWORD random);
	CoString path = GetClientAnalyzeProcessDataPath(now, (DWORD)&data);

	CoAutoFile file(path, GENERIC_ALL);
	if (!file.IsOpen())
	{
		DEBUG_OUT_DEFAULT("BackupAnalyzeProcessData failed.");
		return -1;
	}

	CoString base64;
	base64_encode(data, base64);
	file.Write(base64);

	return 0;
}

// プロセスアナライズ情報を管理端末に送信
LRESULT SendAnalyzeProcessData()
{
	CRITICALTRACE(oReceiveBCast, SendAnalyzeProcessData);

	CoString sendData;
	GetProcessAnalyze(sendData);

	if (sendData.IsEmpty())
	{
		return 0;
	}

	SJIStoUTF8(sendData, sendData);

	if (SendAnalyzeProcessData(sendData) != 0)
	{
		return BackupAnalyzeProcessData(sendData);
	}

	return 0;
}
