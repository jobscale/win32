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

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

// CPU情報
#define SDFGH \
	CoString regProcessorNameString; \
	if (1) \
	{ \
		LPCTSTR p = NULL; \
		p = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"; \
		HKEY hKey; \
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, p, 0, KEY_READ, &hKey) != ERROR_SUCCESS) \
		{ \
			DEBUG_OUT_DEFAULT("failed."); \
			return -1; \
		} \
		DWORD dwType; \
		DWORD dwSize = _MAX_PATH; \
		if (RegQueryValueEx(hKey, "ProcessorNameString", 0, &dwType, (LPBYTE)regProcessorNameString.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS) \
		{ \
			RegCloseKey(hKey); \
			DEBUG_OUT_DEFAULT("failed."); \
			return -1; \
		} \
		regProcessorNameString.ReleaseBuffer(); \

#define SDFGH_END \
		RegCloseKey(hKey); \
	} \

// グラフィック情報
#define ASDFG \
	CoString Display_DriverDesc; \
	if (1) \
	{ \
		LPCTSTR p = "SYSTEM\\ControlSet001\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\0001"; \
		LPCTSTR p2 = "SYSTEM\\ControlSet001\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\0000"; \
		HKEY hKey; \
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, p, 0, KEY_READ, &hKey) != ERROR_SUCCESS) \
		{ \
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, p2, 0, KEY_READ, &hKey) != ERROR_SUCCESS) \
			{ \
				DEBUG_OUT_DEFAULT("failed."); \
				return -1; \
			} \
		} \
		DWORD dwType; \
		DWORD dwSize = _MAX_PATH; \
		if (RegQueryValueEx(hKey, "DriverDesc", 0, &dwType, (LPBYTE)Display_DriverDesc.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS) \
		{ \
			RegCloseKey(hKey); \
			DEBUG_OUT_DEFAULT("failed."); \
			return -1; \
		} \
		Display_DriverDesc.ReleaseBuffer(); \

#define ASDFG_END \
		RegCloseKey(hKey); \
	} \

// CPU名、グラフィックボード名を取得
INT GetHardwareInformation(CoString& textData)
{
	CRITICALTRACE(systeminformation, GetHardwareInformation);

	OSVERSIONINFO info_ver = { sizeof OSVERSIONINFO };
	if (!GetVersionEx(&info_ver))
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	// レジストリからCPU情報を取得する
	SDFGH
	SDFGH_END

	textData = "{\"name\": \"CPU種別\",\"val\":\""+ regProcessorNameString +"\"},";

	// レジストリからグラボ情報を取得する
	ASDFG
	ASDFG_END

	textData += "{\"name\": \"グラフィックドライバー名\",\"val\":\""+ Display_DriverDesc +"\"},";

	return 0;
}

// システム情報を取得する
INT GetSystemInformation(CoString& textData)
{
	CRITICALTRACE(systeminformation, GetSystemInformation);

	OSVERSIONINFO info_ver = { sizeof OSVERSIONINFO };
	if (!GetVersionEx(&info_ver))
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	// レジストリからCPU情報を取得する
	CoString regProcessorIdentifier;
	CoString regProcessorVendor;

	SDFGH
		// CPUファミリーを取得する
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "Identifier", 0, &dwType, (LPBYTE)regProcessorIdentifier.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		regProcessorIdentifier.ReleaseBuffer();

		// プロダクトキーを取得する
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "VendorIdentifier", 0, &dwType, (LPBYTE)regProcessorVendor.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		regProcessorVendor.ReleaseBuffer();
	SDFGH_END

	textData = "{\"name\": \"CPUベンダーコード\",\"val\":\""+ regProcessorVendor +"\"},";
	textData += "{\"name\": \"CPUファミリー\",\"val\":\""+ regProcessorIdentifier +"\"},";
	textData += "{\"name\": \"CPU種別\",\"val\":\""+ regProcessorNameString +"\"},";

	//ＣＰＵの情報 
	//ＣＰＵの情報を得るには Windows-API である GetSystemInfo を呼び出します。
	//これによって得られる情報には次のような物があります。
	//プロセッサーアーキテクチャ
	//  ＣＰＵが Intel / MIPS / Alpha / PowerPC 等の何れかが分かります。
	//プロセッサータイプ
	//  ＣＰＵの具体的な型番がある程度が分かります。
	//    例）Intel Pentium / Alpha 21064 / PowerPC 602 等
	//プロセッサー数
	//  システムに実装されているＣＰＵの数。
	//--------------------------------------------------------------------------------
	CoString pwork;
	CoString processor;
	{
		SYSTEM_INFO info_sys;
		GetSystemInfo(&info_sys);
		switch (info_sys.dwProcessorType)
		{
		case PROCESSOR_INTEL_386 : processor = _T("PROCESSOR_INTEL_386"); break;
		case PROCESSOR_INTEL_486 : processor = _T("PROCESSOR_INTEL_486"); break;
		case PROCESSOR_INTEL_PENTIUM : processor = _T("PROCESSOR_INTEL_PENTIUM"); break;
		case PROCESSOR_MIPS_R4000 : processor = _T("PROCESSOR_MIPS_R4000"); break;
		case PROCESSOR_ALPHA_21064 : processor = _T("PROCESSOR_ALPHA_21064"); break;
		case PROCESSOR_PPC_601 : processor = _T("PROCESSOR_PPC_601"); break;
		case PROCESSOR_PPC_603 : processor = _T("PROCESSOR_PPC_603"); break;
		case PROCESSOR_PPC_604 : processor = _T("PROCESSOR_PPC_604"); break;
		case PROCESSOR_PPC_620 : processor = _T("PROCESSOR_PPC_620"); break;
		case PROCESSOR_HITACHI_SH3 : processor = _T("PROCESSOR_HITACHI_SH3"); break;
		case PROCESSOR_HITACHI_SH3E : processor = _T("PROCESSOR_HITACHI_SH3E"); break;
		case PROCESSOR_HITACHI_SH4 : processor = _T("PROCESSOR_HITACHI_SH4"); break;
		case PROCESSOR_MOTOROLA_821 : processor = _T("PROCESSOR_MOTOROLA_821"); break;
		case PROCESSOR_SHx_SH3 : processor = _T("PROCESSOR_SHx_SH3"); break;
		case PROCESSOR_SHx_SH4 : processor = _T("PROCESSOR_SHx_SH4"); break;
		case PROCESSOR_STRONGARM : processor = _T("PROCESSOR_STRONGARM"); break;
		case PROCESSOR_ARM720 : processor = _T("PROCESSOR_ARM720"); break;
		case PROCESSOR_ARM820 : processor = _T("PROCESSOR_ARM820"); break;
		case PROCESSOR_ARM920 : processor = _T("PROCESSOR_ARM920"); break;
		case PROCESSOR_ARM_7TDMI : processor = _T("PROCESSOR_ARM_7TDMI"); break;
		default : processor = _T("unknown"); break;
		}

		pwork.Format("{\"name\":\"CPUリビジョン\", \"val\":\"%d\"},"
			"{\"name\":\"CPUコア数\", \"val\":\"%d\"},"
			"{\"name\":\"ページサイズ\", \"val\":\"%d\"},",
			info_sys.wProcessorRevision,info_sys.dwNumberOfProcessors, info_sys.dwPageSize);
		//processor += work;
	}
	textData += "{\"name\": \"CPUコード\",\"val\": \""+ processor +"\"},";
	textData += pwork;
 
	//メモリの情報
	//メモリの情報を得るには Windows-API であるGlobalMemoryStatusを呼び出します。
	//これによって得られる情報には次のような物があります。
	//物理メモリの量（空き／全体）
	//仮想メモリの量（空き／全体）
	//スワップファイルの大きさ（ページングサイズ／全体）
	//--------------------------------------------------------------------------------
	CoString memory;
	{
		MEMORYSTATUS info_mem = { sizeof MEMORYSTATUS };
		GlobalMemoryStatus(&info_mem);
		memory.Format("{\"name\": \"物理メモリ\",\"val\": \"%d MB\"},{\"name\": \"仮想メモリ\",\"val\": \"%d MB\"},",
			(int)(info_mem.dwTotalPhys / 1024 / 1024), (int)(info_mem.dwTotalVirtual / 1024 / 1024));
	}
	textData += memory;
 
	//Windows のバージョン 
	//--------------------------------------------------------------------------------
	CoString windows;
	{
		DWORD dwBuild = info_ver.dwBuildNumber;
		switch(info_ver.dwPlatformId)
		{
		case VER_PLATFORM_WIN32s :
			windows = _T("Windows 3.1");
			break;
		case VER_PLATFORM_WIN32_WINDOWS :
			windows = _T("Windows 95/98/ME");
			dwBuild = LOWORD(info_ver.dwBuildNumber);
			break;
		case VER_PLATFORM_WIN32_NT :
			windows = _T("Windows NT");
			break;
		default :
			windows = _T("unknown");
			break;
		}

		//  メジャーバージョン・マイナーバージョン
		if (info_ver.dwMajorVersion == 5 && info_ver.dwMinorVersion == 0)
		{
			windows = "Windows 2000";
		}
		else if (info_ver.dwMajorVersion == 5 && info_ver.dwMinorVersion == 1)
		{
			windows = "Windows XP";
		}
		else if (info_ver.dwMajorVersion == 5 && info_ver.dwMinorVersion == 2)
		{
			windows = "Windows 2003";
		}
		else if (info_ver.dwMajorVersion == 6 && info_ver.dwMinorVersion == 0)
		{
			windows = "Windows VISTA";
		}
		else if (info_ver.dwMajorVersion == 7 && info_ver.dwMinorVersion == 0)
		{
			windows = "Windows 7";
		}
		else
		{
			CoString work;
			work.Format(" (%ld.%ld.%ld)", info_ver.dwMajorVersion, info_ver.dwMinorVersion, dwBuild);
			windows += work;
		}
	}
	textData += "{\"name\": \"OS\",\"val\": \""+ windows +"\"},";
 
	//Windows がインストールされているフォルダ 
	//Windows がインストールされているフォルダを取得するには Windows-API であるGetWindowsDirectory を呼び出します。
	//またシステムフォルダを取得するには、同じくGetSystemDirectory を呼び出します。
 	//--------------------------------------------------------------------------------
	CoString windowsFolder;
	CoString systemFolder;
	{
		// Windowsﾃﾞｨﾚｸﾄﾘの取得
		if (GetWindowsDirectory(windowsFolder.GetBuffer(_MAX_PATH), _MAX_PATH) > _MAX_PATH)
		{
			windowsFolder = _T("取得できません");
		}
		else
		{
			windowsFolder.ReleaseBuffer();
			windowsFolder.Replace("\\", "\\\\");
		}

		// ｼｽﾃﾑﾃﾞｨﾚｸﾄﾘの取得
		if (GetSystemDirectory(systemFolder.GetBuffer(_MAX_PATH), _MAX_PATH) > _MAX_PATH)
		{
			systemFolder = _T("取得できません");
		}
		else
		{
			systemFolder.ReleaseBuffer();
			systemFolder.Replace("\\", "\\\\");
		}
	}
	textData += "{\"name\": \"WINDOWSフォルダ\",\"val\": \""+ windowsFolder +"\"},";
	textData += "{\"name\": \"SYSTEMフォルダ\",\"val\": \""+ systemFolder +"\"},";
	//textData += "WINDOWSフォルダ: ";
	//textData += windowsFolder + "\n";
	//textData += "SYSTEMフォルダ: ";
	//textData += systemFolder + "\n";

 	//ログインしているユーザーの情報 
	//ログインしているユーザーの情報はレジストリから取得します。
	//レジストリの情報は Windows 3.1/95/98 と Windows NT ではレジストリキーが違うため、上記のような単純な処理ではユーザー情報を得ることは出来ません。 
	//Windows 3.1/95/98 のレジストリキー
	//SOFTWARE\MICROSOFT\Windows\CurrentVersion 
	//Windows NT のレジストリキー
	//Software\Microsoft\Windows NT\CurrentVersion 
	CoString registeredOrganization;
	CoString registeredOwner;
	CoString registeredProductId;
	//--------------------------------------------------------------------------------
	{
		// ﾚｼﾞｽﾄﾘ情報を検索するためにWindowsのﾊﾞｰｼﾞｮﾝ情報を取得する
		if (!GetVersionEx(&info_ver))
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		// Windowsの種別によって開くサブキーを変える
		LPCTSTR p = NULL;
		if (info_ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			p = "Software\\Microsoft\\Windows NT\\CurrentVersion"; // WindowsNT
		}
		else
		{
			p = "SOFTWARE\\MICROSOFT\\Windows\\CurrentVersion"; // Windows3.1/95/98
		}
		// ﾚｼﾞｽﾄﾘ情報をｵｰﾌﾟﾝする
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, p, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		// 会社名を取得する
		DWORD dwType;
		DWORD dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "RegisteredOrganization", 0, &dwType, (LPBYTE)registeredOrganization.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		registeredOrganization.ReleaseBuffer();
		// ﾕｰｻﾞｰ名を取得する
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "RegisteredOwner", 0, &dwType, (LPBYTE)registeredOwner.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		registeredOwner.ReleaseBuffer();

		// プロダクトキーを取得する
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "ProductId", 0, &dwType, (LPBYTE)registeredProductId.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		registeredProductId.ReleaseBuffer();
		RegCloseKey(hKey);
	}
	textData += "{\"name\": \"OS登録者名\",\"val\":\""+ registeredOwner +"\"},";
	textData += "{\"name\": \"OS登録企業名\",\"val\":\""+ registeredOrganization +"\"},";
	textData += "{\"name\": \"プロダクト番号\",\"val\":\""+ registeredProductId +"\"},";

	//textData += "使用者: ";
	//textData += registeredOwner + "\n";
	//textData += "会社: ";
	//textData += registeredOrganization + "\n";

	//接続されている機器の情報 
	//パソコンに接続されている機器の情報を得るには Windows-API であるGetSystemMetrics を呼び出します。
	//GetSystemMetrics からは様々な情報を得ることが出来、その主な物としては次のものがあります。
	//SM_CXSCREEN スクリーンのＸサイズ（ドット） 
	//SM_CYSCREEN スクリーンのＹサイズ（ドット） 
	CoString screen;
	screen.Format("{\"name\": \"スクリーンサイズ\", \"val\": \"%d x %d\"},", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	textData += screen;

	// レジストリからグラボ情報を取得する
	CoString Display_DriverVersion;
	CoString Display_ProviderName;

	ASDFG
		// CPUファミリーを取得する
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "DriverVersion", 0, &dwType, (LPBYTE)Display_DriverVersion.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		Display_DriverVersion.ReleaseBuffer();

		// プロダクトキーを取得する
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "ProviderName", 0, &dwType, (LPBYTE)Display_ProviderName.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		Display_ProviderName.ReleaseBuffer();
	ASDFG_END

	textData += "{\"name\": \"グラフィックドライバーベンダー\",\"val\":\""+ Display_ProviderName +"\"},";
	textData += "{\"name\": \"グラフィックドライバー名\",\"val\":\""+ Display_DriverDesc +"\"},";
	textData += "{\"name\": \"グラフィックドライバーバージョン\",\"val\":\""+ Display_DriverVersion +"\"},";

	//SM_MOUSEPRESENT マウスの接続状態 
	//SM_CMOUSEBUTTONS マウスボタンの数 
	//CoString mouse;
	//BOOL isMouse = GetSystemMetrics(SM_MOUSEPRESENT);
	//if (isMouse)
	//{
	//	mouse.Format("%d ボタンマウス Enable", GetSystemMetrics(SM_CMOUSEBUTTONS));
	//}
	//else
	//{
	//	mouse.Format("接続されていません");
	//}
	//textData += "マウス: ";
	//textData += mouse + "\n";

	//SM_NETWORK ネットワークが存在するかどうか 
	CoString network = GetSystemMetrics(SM_NETWORK) ? "有効" : "無効";
	textData += "{\"name\": \"ネットワーク\",\"val\":\""+ network +"\"},";

	//textData += "ネットワーク: ";
	//textData += network + "\n";

	//ドライブの情報
	//--------------------------------------------------------------------------------
	CoString drives;
	{
		//=====	Windows が認識しているドライブを取得する ========
		DWORD dwLogicalDrives = GetLogicalDrives();		// 認識できているドライブ群を取得する

		//=====	Ｃドライブより順番に検索する(Ａ，Ｂはフロッピーなので省く) =======
		CoString strDrive;
		for (UINT nDrive = 0; nDrive < 26; nDrive++)
		{
			strDrive.Format("%c", nDrive + 'A');

			// ドライブとして認識していないドライブは検索対象からはずす
			if ((dwLogicalDrives & (1 << nDrive)) == 0)
			{
				continue;
			}

			// ルートフォルダ
			CoString work;
			CoString rootDrive = strDrive + ":\\";
			UINT uiVal = GetDriveType(rootDrive);
			switch (uiVal)
			{
			case DRIVE_UNKNOWN : work = _T("不明なドライブ"); break;
			case DRIVE_REMOVABLE : work = _T("リムーバブルドライブ"); break;
			case DRIVE_FIXED : work = _T("固定ドライブ"); break;
			case DRIVE_REMOTE : work = _T("リモートドライブ"); break;
			case DRIVE_CDROM : work = _T("CD-ROMドライブ"); break;
			case DRIVE_RAMDISK : work = _T("RAMドライブ"); break;
			default : work = _T("未接続ドライブ"); break;
			}

			drives += "{\"name\": \"ドライブ(" + strDrive + ")\",";
			drives += "\"val\": \"" + work + "\"},";

			{
				// ファイルシステムチェック
				char volumeName[256] = { 0 };		// ボリュームの名前
				DWORD volumeNameSize = 256;			// ボリューム名のサイズ
				DWORD volumeSerialMunber;			// ボリュームの通し番号
				DWORD maxNameLength;				// ファイル名の最大長
				DWORD fileSystemFlags;				// ファイルシステムの情報フラグ
				char fileSystemName[256] = { 0 };	// ファイルシステムの名前（FAT,NTFS等）
				DWORD fileSystemNameSize = 256;		// ファイルシステムの名前のサイズ

				// ボリュームラベルが無い場合は文字列の更新が行われないので予め初期化しておく
				wsprintf(volumeName, ""); // ☆ボリュームラベルの初期化
				if (GetVolumeInformation(rootDrive, volumeName, volumeNameSize, &volumeSerialMunber,
									&maxNameLength, &fileSystemFlags, (LPTSTR)fileSystemName, fileSystemNameSize))
				{
					work.Format("{\"name\" : \"ボリュームの名前(%s)\", \"val\": \"%s\"},", strDrive,(LPCTSTR)volumeName);
					drives += work;
					work.Format("{\"name\" : \"ボリュームのシリアル番号(%s)\", \"val\" :\"%X\"},",strDrive, volumeSerialMunber);
					drives += work;
					work.Format("{\"name\" : \"ファイルシステムの名前(%s)\", \"val\" :\"%s\"},", strDrive,fileSystemName);
					drives += work;

					ULARGE_INTEGER ulFreeBytesAvailable = { 0 };
					ULARGE_INTEGER ulTotalNumberOfBytes = { 0 };
					ULARGE_INTEGER ulTotalNumberOfFreeBytes = { 0 };
					GetDiskFreeSpaceEx(rootDrive, &ulFreeBytesAvailable, &ulTotalNumberOfBytes, &ulTotalNumberOfFreeBytes);
					work.Format("{\"name\" : \"メディア容量(%s)\", \"val\" :\"%d MB\"}, {\"name\": \"空きスペース(%s)\", \"val\" :\"%d MB\"},", 
						strDrive,
						(int)(ulTotalNumberOfBytes.QuadPart / 1024 / 1024),
						strDrive,
						(int)(ulTotalNumberOfFreeBytes.QuadPart / 1024 / 1024)
					);
					drives += work;
				}
				else
				{
					drives += "";
				}
			}
		}
	}
	textData += drives;

	return 0;
}
