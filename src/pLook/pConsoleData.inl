///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#include "oMyJson.h"

struct UPDATE_COMSOLE_DATA
{
	SYSTEMTIME latest_time;
	CoString macaddr;
	CoString ipaddr;
	CoString ipmask;
	CoString hostname;
	CoString status;
	CoString username;
	CoString boot_time;
	CoString time_span;
	CoString cpu_usage;
	CoString process;
	CoString dwMemoryLoad;
	CoString dwTotalPhys;
	CoString dwAvailPhys;
	CoString dwTotalVirtual;
	CoString dwAvailVirtual;
	CoString ulFreeBytesAvailable;
	CoString ulTotalNumberOfBytes;
	CoString ulTotalNumberOfFreeBytes;
	CoString in_packet;
	CoString out_packet;
	CoString connections;
	DWORD in_net;
	DWORD out_net;

	UPDATE_COMSOLE_DATA()
		: in_net(0)
		, out_net(0)
	{
		GetSystemTime(&latest_time);
		SystemTimeToTzSpecificLocalTime(NULL, &latest_time, &latest_time);
	}

	BOOL operator<(const UPDATE_COMSOLE_DATA& other) const
	{
		return macaddr < other.macaddr;
	}
	BOOL operator>(const UPDATE_COMSOLE_DATA& other) const
	{
		return macaddr > other.macaddr;
	}
	BOOL operator==(const UPDATE_COMSOLE_DATA& other) const
	{
		return macaddr == other.macaddr;
	}
};

class CoConsoleDataArray : public CoConstSortArray<UPDATE_COMSOLE_DATA>
{
public:
	INT Find(CoString& macaddr)
	{
		for (INT i = 0, count = GetSize(); i < count; i++)
		{
			if ((*this)[i].macaddr == macaddr)
			{
				return i;
			}
		}
		return -1;
	}
};

CoConsoleDataArray& GetConsoleData(CoCriticalSectionCtrl& _critical)
{
	static CoConsoleDataArray data;
	return data;
}

CCriticalSection& GetConsoleCritical()
{
	static CCriticalSection section;
	return section;
}

UPDATE_COMSOLE_DATA& GetConsoleData(CoString& macaddr, CoCriticalSectionCtrl& _critical)
{
	// クリティカルセクション
	CoCriticalSectionCtrl critical(GetConsoleCritical());

	CoConsoleDataArray& console_data = GetConsoleData(critical);
	INT current = console_data.Find(macaddr);
	if (current == -1)
	{
		UPDATE_COMSOLE_DATA data;
		data.macaddr = macaddr;
		DWORD mass = GetMassLicense();
		DWORD count = console_data.GetSize();

		// ライセンス数を超えたら管理しない
		if (count >= mass)
		{
			return *((UPDATE_COMSOLE_DATA*)NULL);
		}

		current = console_data.Add(data);
	}

	return console_data[current];
}

const UPDATE_COMSOLE_DATA GetConsoleData(CoString& macaddr)
{
	// クリティカルセクション
	CoCriticalSectionCtrl critical(GetConsoleCritical());

	return GetConsoleData(macaddr, critical);
}

DWORD GetConsoleData(DWORD pos, UPDATE_COMSOLE_DATA& data)
{
	// クリティカルセクション
	CoCriticalSectionCtrl critical(GetConsoleCritical());

	CoConsoleDataArray& console_data = GetConsoleData(critical);
	data = console_data.ElementAt(pos);

	return 0;
}

LPCTSTR ConsoleDataPath()
{
	static CoString modulePath = GetModuleFolder();
	static CoString path = modulePath + ("\0 pConsole.dat" + 2);

	return path;
}

DWORD UpdateConsoleData(UPDATE_COMSOLE_DATA& data)
{
	// クリティカルセクション
	CoCriticalSectionCtrl critical(GetConsoleCritical());

	UPDATE_COMSOLE_DATA& console_data = GetConsoleData(data.macaddr, critical);
	if (&console_data == NULL)
	{
		return -1;
	}

	if (strcmp((LPCTSTR)console_data.status, "OFFDUTY") == 0)
	{
		console_data.ipaddr.Empty();
		console_data.ipmask.Empty();

		return 0;
	}

	// 最終更新
	GetSystemTime(&console_data.latest_time);
	SystemTimeToTzSpecificLocalTime(NULL, &console_data.latest_time, &console_data.latest_time);

	if (!data.ipaddr.IsEmpty())
	{
		console_data.ipaddr = data.ipaddr;
	}
	if (!data.ipmask.IsEmpty())
	{
		console_data.ipmask = data.ipmask;
	}
	if (!data.hostname.IsEmpty())
	{
		console_data.hostname = data.hostname;
	}
	if (!data.username.IsEmpty())
	{
		console_data.username = data.username;
	}
	if (!data.boot_time.IsEmpty())
	{
		console_data.boot_time = data.boot_time;
	}
	if (!data.time_span.IsEmpty())
	{
		console_data.time_span = data.time_span;
	}
	if (!data.cpu_usage.IsEmpty())
	{
		console_data.cpu_usage = data.cpu_usage;
	}
	if (!data.process.IsEmpty())
	{
		console_data.process = data.process;
	}
	if (!data.dwMemoryLoad.IsEmpty())
	{
		console_data.dwMemoryLoad = data.dwMemoryLoad;
	}
	if (!data.dwTotalPhys.IsEmpty())
	{
		console_data.dwTotalPhys = data.dwTotalPhys;
	}
	if (!data.dwAvailPhys.IsEmpty())
	{
		console_data.dwAvailPhys = data.dwAvailPhys;
	}
	if (!data.dwTotalVirtual.IsEmpty())
	{
		console_data.dwTotalVirtual = data.dwTotalVirtual;
	}
	if (!data.dwAvailVirtual.IsEmpty())
	{
		console_data.dwAvailVirtual = data.dwAvailVirtual;
	}
	if (!data.ulFreeBytesAvailable.IsEmpty())
	{
		console_data.ulFreeBytesAvailable = data.ulFreeBytesAvailable;
	}
	if (!data.ulTotalNumberOfBytes.IsEmpty())
	{
		console_data.ulTotalNumberOfBytes = data.ulTotalNumberOfBytes;
	}
	if (!data.ulTotalNumberOfFreeBytes.IsEmpty())
	{
		console_data.ulTotalNumberOfFreeBytes = data.ulTotalNumberOfFreeBytes;
	}
	if (!data.in_packet.IsEmpty())
	{
		console_data.in_packet = data.in_packet;
	}
	if (!data.out_packet.IsEmpty())
	{
		console_data.out_packet = data.out_packet;
	}
	if (!data.connections.IsEmpty())
	{
		console_data.connections = data.connections;
	}

	return 0;
}

DWORD LoadConsoleData()
{
	// クリティカルセクション
	CoCriticalSectionCtrl critical(GetConsoleCritical());

	HANDLE hFile = CreateFile(ConsoleDataPath(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	CoString readData;
	CoString buffer;
	DWORD readied = 0;

	DWORD dwReadHigh = 0;
	DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	for (; ReadFile(hFile, buffer.GetBuffer(dwRead), dwRead, &readied, NULL) && readied; )
	{
		buffer.GetBufferSetLength(readied);
		readData += buffer;
	}

	CloseHandle(hFile);

	INT pos = 0;
	for (; ; )
	{
		CoString macaddr = readData.AbstractSearchSubstance(pos);
		if (macaddr.IsEmpty())
		{
			break;
		}
		UPDATE_COMSOLE_DATA& console_data = GetConsoleData(macaddr, critical);
		if (&console_data == NULL)
		{
			break;
		}
		console_data.hostname = readData.AbstractSearchSubstance(pos);
	}

	return 0;
}

DWORD SaveConsoleData()
{
	// クリティカルセクション
	CoCriticalSectionCtrl critical(GetConsoleCritical());

	CoConsoleDataArray& console_data = GetConsoleData(critical);

	CoString saveData;
	CoString work;

	for (DWORD i = 0, count = console_data.GetSize(); i < count; i++)
	{
		UPDATE_COMSOLE_DATA& data = console_data.ElementAt(i);

		work.Format("macaddr \"%s\" hostname \"%s\"\n", (LPCTSTR)data.macaddr, (LPCTSTR)data.hostname);
		saveData += work;
	}

	HANDLE hFile = CreateFile(ConsoleDataPath(), GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwWrite;
		WriteFile(hFile, saveData, saveData.GetLength(), &dwWrite, NULL);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
	}

	return 0;
}

DWORD GetConsoleData(CoConsoleDataArray& console_data)
{
	// クリティカルセクション
	CoCriticalSectionCtrl critical(GetConsoleCritical());

	console_data = GetConsoleData(critical);

	// 更新されていないものはネットワークに接続されていない
	COleDateTime now_time = COleDateTime::GetCurrentTime();
	COleDateTimeSpan max_span(0, 0, 2, 0); // 2分以上通信が途絶えた端末は除外する
	for (DWORD i = 0, count = console_data.GetSize(); i < count; i++)
	{
		UPDATE_COMSOLE_DATA& data = console_data.ElementAt(i);

		COleDateTimeSpan time_span = now_time - data.latest_time;
		if (time_span > max_span)
		{
			data.ipaddr.Empty();
			data.ipmask.Empty();
		}
	}

	return 0;
}

LRESULT GetMachineList(CoString& jData, DWORD flag = 0)
{
	// flag
	//  0: オンライン
	//  1: オフライン
	//  2: すべて
	// 戻り
	//  [ { "hn": "ZZZ", "mc": "x.x.x" } ]

	CoConsoleDataArray console_data;
	GetConsoleData(console_data);

#ifdef USE_JSONCPP
	Json::Value *jVal = create_AValue();
	int jCount = 0;
#else
	CoString work;
	jData = "[ ";
#endif
	for (DWORD i = 0, count = console_data.GetSize(); i < count; i++)
	{
		UPDATE_COMSOLE_DATA& data = console_data.ElementAt(i);

		if (flag == 2 || (flag == 0 && !data.ipaddr.IsEmpty()) || (flag == 1 && data.ipaddr.IsEmpty()))
		{
#ifdef USE_JSONCPP
			(*jVal)[jCount]["hn"] = (const char *)data.hostname;
			(*jVal)[jCount]["mc"] = (const char *)data.macaddr;
			jCount++;
#else
			work.Format("{ \"hn\": \"%s\", \"mc\": \"%s\" }, \r\n", (LPCTSTR)data.hostname, (LPCTSTR)data.macaddr);
			jData += work;
#endif
		}
	}
#ifdef USE_JSONCPP
	jData = jVal->toFastString().c_str();
	jVal->release();
#else
	INT jLength = jData.GetLength();
	if (jLength >= 4)
	{
		jLength -= 4;
	}
	jData.GetBufferSetLength(jLength);
	jData += " ]";
#endif

	return 0;
}
