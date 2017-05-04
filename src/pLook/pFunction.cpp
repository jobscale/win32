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
#include "../oCommon/oBase64.h"

#define _OFC_EXPORT_
#include "../ofc/oFoundationClassLibrary.h"
#include "../ofc/oCriticalTrace.h"

#include "pProcess.h"
#include "pLogging.h"
#include "pNetwork.h"
#include "pConsole.h"

#include "pFunction.h"
#include "pUtility.h"
#include "oWndFind.h"

#include "pLook.h"

#include <UrlHist.h>
#include <SrRestorePtApi.h>

#include "oMyjson.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// メインファンクション

void ShutDownAndPowerOffSystem(BOOL bForce)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp;
	
	// Get a token for this process.
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES |TOKEN_QUERY, &hToken);
	// Get the LUID for the shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;  // oneprivilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	// Get the shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 

	UINT uFlags = EWX_SHUTDOWN | EWX_POWEROFF;
	if (bForce == TRUE)
	{
		uFlags |= EWX_FORCE;
	}

	ExitWindowsEx(uFlags, 0);
}

void RebootSystem(BOOL bForce)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp;
	
	// Get a token for this process.
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES |TOKEN_QUERY, &hToken);
	// Get the LUID for the shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;  // oneprivilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	// Get the shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 

	UINT uFlags = EWX_REBOOT;
	if (bForce == TRUE)
	{
		uFlags |= EWX_FORCE;
	}

	ExitWindowsEx(uFlags, 0);
}

RUNFUNCPROC(Shutdown)
{
	CRITICALTRACE(pFunction, Shutdown);

	ShutDownAndPowerOffSystem(TRUE);

	return 0;
}

RUNFUNCPROC(Reboot)
{
	CRITICALTRACE(pFunction, Reboot);

	RebootSystem(TRUE);

	return 0;
}

RUNFUNCPROC(KillProcess)
{
	CRITICALTRACE(pFunction, KillProcess);

	CoString killing_list;
	GetKillingList(killing_list);

	if (!killing_list.IsEmpty())
	{
		killing_list += "\\,";
	}
	killing_list += "^";
	killing_list += pData->text;
	killing_list += "$";

	SetKillingList(killing_list);

	pData->result = "\0 Added killing list." + 2;

	return 0;
}

DWORD ScreenShot(LPCSTR localPath)
{
	CRect rect;
	GetClientRect(GetDesktopWindow(), rect);
	CoBitmapDC dc;
	dc.CreateBitmapDC(NULL, rect.TopLeft(), rect.Size());
	CoBitmapDC memDC;
	memDC.CreateBitmapDC(&dc, rect.TopLeft(), rect.Size());
	CBitmap& bmp = memDC.GetBitmap();
	BITMAP bm;
	bmp.GetBitmap(&bm);

	BITMAPINFO bInfo = { sizeof BITMAPINFOHEADER };
	BITMAPINFOHEADER& biHeader = bInfo.bmiHeader;
	biHeader.biWidth = bm.bmWidth;
	biHeader.biHeight = bm.bmHeight;
	biHeader.biPlanes = bm.bmPlanes;
	biHeader.biBitCount = 24;
	biHeader.biCompression = 0;
	biHeader.biSizeImage = 0;
	biHeader.biXPelsPerMeter = 0;
	biHeader.biYPelsPerMeter = 0;
	biHeader.biClrUsed = 0;
	biHeader.biClrImportant = 0;

	INT nWidthLength = biHeader.biWidth * 3;
	INT nPer = nWidthLength % 4;
	if (nPer)
	{
		nWidthLength += nPer;
	}
	biHeader.biSizeImage = nWidthLength * biHeader.biHeight;
	CoString buffer;
	LPVOID pBits = buffer.GetBuffer(biHeader.biSizeImage);
	GetDIBits(memDC.GetSafeHdc(), (HBITMAP)bmp.GetSafeHandle(), 0, bm.bmHeight, pBits, &bInfo, DIB_RGB_COLORS);

	BITMAPFILEHEADER bFile = { 0 };
	bFile.bfType = *LPWORD("\0 BM" + 2);
	bFile.bfSize = sizeof BITMAPFILEHEADER;
	bFile.bfReserved1 = 0;
	bFile.bfReserved2 = 0;
	bFile.bfOffBits = sizeof BITMAPFILEHEADER + sizeof BITMAPINFO;

	CoString added;
	CoString binary;
	memcpy(added.GetBufferSetLength(sizeof BITMAPFILEHEADER), &bFile, sizeof BITMAPFILEHEADER);
	binary += added;
	memcpy(added.GetBufferSetLength(sizeof BITMAPINFO), &bInfo, sizeof BITMAPINFO);
	binary += added;
	memcpy(added.GetBufferSetLength(biHeader.biSizeImage), pBits, biHeader.biSizeImage);
	binary += added;

	HANDLE hFile = CreateFile(localPath, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

    DWORD dwWrite = 0;
	WriteFile(hFile, binary, binary.GetLength(), &dwWrite, NULL);
	CloseHandle(hFile);

	return 0;
}

RUNFUNCPROC(ScreenShot)
{
	CRITICALTRACE(pFunction, ScreenShot);

	CoString jpgData;
	CoString sendData = "screenshot:";
	DataSendConsole("localhost", 12087, sendData, &jpgData);

	base64_encode(jpgData, pData->result);

	return 0;
}

RUNFUNCPROC(ForegroundList)
{
	CRITICALTRACE(pFunction, ForegroundList);

	CString work;
	MyGetProfileString(P("Setting"), P("ignore_proc_list"), work.GetBuffer(2048), 2048);
	work.ReleaseBuffer();
	if (work.IsEmpty())
	{
		work = P("pExTrapper.exe\\,pTools.exe\\,pControler.exe\\,pPower.exe");
	}
	else
	{
		work.Replace("\r\n", "\\,");
	}

	CoStringArray ignore_proc_list;
	GetArrayValue(ignore_proc_list, work, "\\,");

	// 既に取得している情報を取得する
	CoWndFind& GetWndFindCtrl();
	CoWndFind& cwf = GetWndFindCtrl();

	CoString str;
	cwf.GetStringData(str);

	if (pData && pData->isSync)
	{
		// Convert to UTF-8
		SJIStoUTF8(str, str);

		CoString result;
		base64_encode(str, result);
		pData->result = result;
	}

	return 0;
}

RUNFUNCPROC(ProcessList)
{
	CRITICALTRACE(pFunction, ProcessList);

	CString work;
	MyGetProfileString(P("Setting"), P("ignore_proc_list"), work.GetBuffer(2048), 2048);
	work.ReleaseBuffer();
	if (work.IsEmpty())
	{
		work = P("pExTrapper.exe\\,pTools.exe\\,pControler.exe\\,pPower.exe");
	}
	else
	{
		work.Replace("\r\n", "\\,");
	}

	CoStringArray ignore_proc_list;
	GetArrayValue(ignore_proc_list, work, "\\,");

	// 既に取得しているプロセス情報を取得する
	CCriticalSection& CommandCtrlDataCritical();
	CoCriticalSectionCtrl critical(CommandCtrlDataCritical());
	COMMAND_CTRL_DATA& GetCommandCtrlData();
	COMMAND_CTRL_DATA& com_data = GetCommandCtrlData();
	CoProcessArray& process = com_data.nowProcess;

#ifdef USE_JSONCPP
	Json::Value *jVal = create_AValue();
#endif
	CoString str;

	COleDateTime now = COleDateTime::GetCurrentTime();
#ifdef USE_JSONCPP
	for (INT i = 0, idx = 0, count = process.GetSize(); i < count; i++)
#else
	for (INT i = 0, count = process.GetSize(); i < count; i++)
#endif
	{
		// 処理しない
		if (IsFindArray((LPCTSTR)process[i].szProcess, ignore_proc_list))
		{
			continue;
		}

#ifdef USE_JSONCPP
		CoString tmpstr;
		tmpstr.Format("%d", process[i].dwPID);
		(*jVal)[idx]["pid"] = tmpstr;
		tmpstr.Format("%d", process[i].wTask);
		(*jVal)[idx]["sid"] = tmpstr;
		(*jVal)[idx]["procname"]  = (LPCTSTR)process[i].szProcess;
		(*jVal)[idx]["begin"]     = (LPCTSTR)process[i].begin.Format("%Y/%m/%d %H:%M:%S");
		(*jVal)[idx]["time_span"] = (LPCTSTR)(now-process[i].begin).Format("%H:%M:%S");
		idx++;
#else
		CoString text;
		text.Format("\0 {\"pid\": \"%d\", \"sid\": \"%d\", \"procname\": \"%s\", \"begin\": \"%s\", \"time_span\": \"%s\"}, " + 2,
			process[i].dwPID, process[i].wTask, (LPCTSTR)process[i].szProcess,
			(LPCTSTR)process[i].begin.Format("%Y/%m/%d %H:%M:%S"),
			(LPCTSTR)(now-process[i].begin).Format("%H:%M:%S"));

		str += text;
#endif
	}

	if (pData && pData->isSync)
	{
#ifdef USE_JSONCPP
		str = jVal->toFastString().c_str();
		jVal->release();
#else
		if (str.IsEmpty())
		{
      str = "[]";
		}
		else
		{
      str = "[" + str.Left(str.GetLength() - 2) + "]";
		}
#endif

		// Convert to UTF-8
		SJIStoUTF8(str, str);

		CoString result;
		base64_encode(str, result);
		pData->result = result;
	}

    return 0;
}

RUNFUNCPROC(ConnectionList)
{
	CRITICALTRACE(pFunction, ConnectionList);

	CoString str;

	DWORD TCPConnections(CoConnectionsArray& connectionsm, DWORD flag = 0);
	CoConnectionsArray connections;
	TCPConnections(connections, 1);

#ifdef USE_JSONCPP
	Json::Value *jVal = create_AValue();
#endif

	for (INT i = 0, count = connections.GetSize(); i < count; i++)
	{
#ifdef USE_JSONCPP
		(*jVal)[i]["local_addr"]  = (LPCTSTR)connections[i].localAddr;
		(*jVal)[i]["local_port"]  = (LPCTSTR)connections[i].localPort;
		(*jVal)[i]["remote_addr"] = (LPCTSTR)connections[i].remoteAddr;
		(*jVal)[i]["remote_port"] = (LPCTSTR)connections[i].remotePort;
		(*jVal)[i]["status"]      = (LPCTSTR)connections[i].status.text;
#else
		CoString text;
		text.Format("\0 {\"local_addr\": \"%s\", \"local_port\": \"%s\", \"remote_addr\": \"%s\", \"remote_port\": \"%s\", \"status\": \"%s\"}, " + 2,
			(LPCTSTR)connections[i].localAddr, (LPCTSTR)connections[i].localPort, (LPCTSTR)connections[i].remoteAddr, (LPCTSTR)connections[i].remotePort, (LPCTSTR)connections[i].status.text);

		str += text;
#endif
	}

	if (pData && pData->isSync)
	{
#ifdef USE_JSONCPP
		str = jVal->toFastString().c_str();
		jVal->release();
#else
		if (str.IsEmpty())
		{
			str = "[]";
		}
		else
		{
			str = "[" + str.Left(str.GetLength() - 2) + "]";
		}
#endif

		// Convert to UTF-8
		SJIStoUTF8(str, str);

		CoString result;
		base64_encode(str, result);
		pData->result = result;
	}

    return 0;
}

RUNFUNCPROC(UrlHistory)
{
	CRITICALTRACE(pFunction, UrlHistory);

	static COleDateTime antiDuplicate = COleDateTime::GetCurrentTime();

	CoStringArray ignore_url_list;
	GetArrayValue(ignore_url_list, "127.0.0.1\\,localhost\\,127.0.0.2", "\\,");

	CoString str;
#ifdef USE_JSONCPP
	Json::Value *jVal = create_AValue();
#endif

	CoInitialize(NULL);

	IUrlHistoryStg2* pHistory;
	HRESULT hr = CoCreateInstance(CLSID_CUrlHistory, NULL, CLSCTX_INPROC_SERVER, IID_IUrlHistoryStg2, reinterpret_cast<void **>(&pHistory));

	IEnumSTATURL* URLs;
	if (SUCCEEDED(hr) && SUCCEEDED(pHistory->EnumUrls(&URLs)))
	{
		USES_CONVERSION;

		_STATURL url = { 0 };
		DWORD celt = 1;
#ifdef USE_JSONCPP
		int i = 0;
#endif
		while (celt == 1)
		{
			HRESULT hRes = URLs->Next(1, &url, &celt);
			if (!celt || FAILED(hRes))
			{
				break;
			}

			COleDateTime dateTime = url.ftLastUpdated;
			//if (pData && pData->isSync)
			//{
				static COleDateTimeSpan dateSpan(1, 0, 0, 0);
				if ((antiDuplicate - dateSpan) >= dateTime)
				{
					continue;
				}
			//}
			//else if (antiDuplicate >= dateTime)
			//{
			//	continue;
			//}

			CComBSTR bstr = url.pwcsTitle;
			CoString title = bstr;
			bstr = url.pwcsUrl;
			CoString urlText = bstr;

			title = title.Left(254);
			urlText = urlText.Left(254);

#ifdef USE_JSONCPP
			(*jVal)[i]["url"]   = urlText;
			(*jVal)[i]["title"] = title;
			i++;
#else
			// 処理しない
			//if (IsFindArray((LPCTSTR)urlText, ignore_url_list))
			//{
			//	continue;
			//}

			//if (pData && pData->isSync)
			//{
				//urlText = dateTime.Format("%Y/%m/%d %H:%M:%S");
				//urlText += "DELIMITER<a href=\"" + urlText + "\">" + urlText + ("\0 </a>DELIMITER" + 2) + title + ("\0 \n" + 2);
				//urlText = dateTime.Format("%Y/%m/%d %H:%M:%S");
				//urlText += "{\"url\": \"" + urlText + "\", \"title\": \"" + title + "\"},";
			//}
			//else
			//{
			//	SYSTEMTIME sysTime = { 0 };
			//	dateTime.GetAsSystemTime(sysTime);

			//	CoString data;
			//	//data.Format("%s %s", urlText, title);
			//	data.Format("{\"url\": \"%s\", \"title\": \"%s\"},", urlText, title);

			//	//SendLoggingLook(99001, data, &sysTime, "InternetExplorer", "Information");
			//}

			//str += urlText;
			str += "{\"url\": \"" + urlText + "\", \"title\": \"" + title + "\"},";
#endif
		}

		URLs->Release();
	}

	pHistory->Release();
	CoUninitialize();

	antiDuplicate = COleDateTime::GetCurrentTime();

	if (pData && pData->isSync)
	{
#ifdef USE_JSONCPP
		str = jVal->toFastString().c_str();
		jVal->release();
#else
		if (str.IsEmpty())
		{
		//	str = "<div id=bg_bcmd_title align=left>INFORMATION</div><br>URL history data nothing";
			str = "[]";
		}
		else
		{
		//	str.Replace("DELIMITER", "</td><td bordercolor=\"#6688ff\">");
		//	str.Replace("\n", "</td></tr>\n<tr><td bordercolor=\"#6688ff\">");
		//	str = ("<div id=bg_bcmd_title align=left>WEBサイト閲覧履歴</div><br><table border=1 bordercolor=\"#6688ff\"><tr><td bordercolor=\"#6688ff\">" + str + "</td></tr></table>");
			str = "[" + str.Left(str.GetLength() - 2) + "]";
		}
#endif

		// Convert to UTF-8
		SJIStoUTF8(str, str);

		CoString result;
		base64_encode(str, result);
		//pData->result = "%binary%%html%" + result;
		pData->result = result;
	}

	return 0;
}

RUNFUNCPROC(Uninstall)
{
	CRITICALTRACE(pFunction, Uninstall);

	struct UNINSTALLDATA
	{
		CoStringArray string;
		CoStringArray version;
	};
	static UNINSTALLDATA uninstData;

	CString work;
	MyGetProfileString(P("Setting"), P("ignore_install_list"), work.GetBuffer(2048), 2048);
	work.ReleaseBuffer();
	if (work.IsEmpty())
	{
	}
	else
	{
		work.Replace("\r\n", "\\,");
	}

	CoStringArray ignore_install_list;
	GetArrayValue(ignore_install_list, work, "\\,");

	CoString str;

	HKEY key[] ={ HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, };

	for (INT i = 0, count = sizeof key / sizeof HKEY; i < count; i++)
	{
		HKEY hKey;
		if (RegOpenKeyEx(key[i], ("\0 SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall") + 2, NULL, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			CoString nameSubKey;
			for (DWORD dwIndex = 0, count = 2048; dwIndex < count; dwIndex++)
			{
				DWORD num = 2048;
				if (RegEnumKeyEx(hKey, dwIndex, nameSubKey.GetBuffer(2048), &num, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
				{
					break;
				}
				nameSubKey.ReleaseBuffer();

				HKEY hSubKey;
				if (RegOpenKeyEx(hKey, nameSubKey, NULL, KEY_READ, &hSubKey) != ERROR_SUCCESS)
				{
					continue;
				}

				CoString caption;
				DWORD regType = REG_SZ;
				num = _MAX_PATH;
				RegQueryValueEx(hSubKey, ("\0 DisplayName") + 2, NULL, &regType, (LPBYTE)caption.GetBuffer(_MAX_PATH), &num);
				caption.ReleaseBuffer();
				if (caption.IsEmpty())
				{
					RegCloseKey(hSubKey);
					continue;
				}

				// 処理しない
				if (IsFindArray((LPCTSTR)caption, ignore_install_list))
				{
					RegCloseKey(hSubKey);
					continue;
				}

				// versionの取得
				CoString caption_version;
				RegQueryValueEx(hSubKey, ("\0 DisplayVersion") + 2, NULL, &regType, (LPBYTE)caption_version.GetBuffer(_MAX_PATH), &num);
				RegCloseKey(hSubKey);
				caption_version.ReleaseBuffer();
				if (caption_version.IsEmpty())
				{
					caption_version = "-";
				}

				caption = caption.Left(254);
				caption_version = caption_version.Left(254);

				//caption += ("\0 \n" + 2);
				CoString captionData(caption);

				if (uninstData.string.Find(captionData) == -1)
				{
					uninstData.string.Add(captionData);
					uninstData.version.Add(caption_version);
				}
			}

			RegCloseKey(hKey);
		}
	}

#ifdef USE_JSONCPP
 	Json::Value *jVal = create_AValue();
#endif

	for (INT i = 0, count = uninstData.string.GetSize(); i < count; i++)
	{
#ifdef USE_JSONCPP
		(*jVal)[i]["val"] = uninstData.string[i];
		(*jVal)[i]["ver"] = uninstData.version[i];
#else
		str += "{\"val\":\"" + uninstData.string[i] + "\",\"ver\":\"" + uninstData.version[i] + "\"},";
#endif
	}

#ifdef USE_JSONCPP
	str = jVal->toFastString().c_str();
	jVal->release();
#else
	if (str.IsEmpty())
	{
		str = "[]";
	}
	else
	{
		str = "[" + str.Left(str.GetLength() - 1) + "]";
	}
#endif

	// Convert to UTF-8
	SJIStoUTF8(str, str);

	CoString result;
	base64_encode(str, result);
	pData->result = result;

	return 0;
}

RUNFUNCPROC(DeskWallPaper)
{
	CRITICALTRACE(pFunction, DeskWallPaper);

	// 壁紙の並び方を設定する
	// SystemParametersInfo(SPI_SETDESKPATTERN, 0, 0, 0);
	// 壁紙を設定する
	// SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, deskwallpaper, 0);

	int style = WPSTYLE_CENTER;
	//     WPSTYLE_TILE
	//     WPSTYLE_STRETCH
	//     WPSTYLE_CENTER

	CoInitialize(NULL);
	IActiveDesktop* pActiveDesktop;
	if (CoCreateInstance(CLSID_ActiveDesktop, NULL, CLSCTX_INPROC_SERVER, IID_IActiveDesktop,(void **)&pActiveDesktop) == S_OK)
	{
		COMPONENTSOPT comopt = { sizeof COMPONENTSOPT };
		pActiveDesktop->GetDesktopItemOptions(&comopt, 0);
		if (!comopt.fActiveDesktop || !comopt.fEnableComponents)
		{
			comopt.fEnableComponents = TRUE;
			comopt.fActiveDesktop = TRUE;
			if (pActiveDesktop->SetDesktopItemOptions(&comopt, 0) != 0)
			{
				DEBUG_OUT_DEFAULT("failed.");
			}
			pActiveDesktop->ApplyChanges(AD_APPLY_SAVE);
		}

		//Ajoutons un item a ActiveDesktop 
		//Add an item to a ActiveDesktop 
		COMPONENT comp = { sizeof COMPONENT };

		// remove all items
		INT count = 0;
		if (pActiveDesktop->GetDesktopItemCount(&count, 0) == 0 && count)
		{
			for (INT i = 0; i < count; i++)
			{
				if (pActiveDesktop->GetDesktopItem(count - i - 1, &comp, 0) == 0)
				{
					pActiveDesktop->RemoveDesktopItem(&comp, 0);
				}
			}
		}
		pActiveDesktop->ApplyChanges(AD_APPLY_SAVE);

		CComBSTR bstr = pData->text;

		WALLPAPEROPT wpopt = { sizeof WALLPAPEROPT, style };
		if (pActiveDesktop->SetWallpaperOptions(&wpopt, 0) != 0)
		{
			DEBUG_OUT_DEFAULT("failed.");
		}
		pActiveDesktop->ApplyChanges(AD_APPLY_SAVE);

		//iComponentType : Type de controle, peut etre une page web, un ActiveX etc. 
		//iComponentType : Control type, it could be a webpage, an ActiveX etc. 
		comp.iComponentType = COMP_TYPE_WEBSITE;//COMP_TYPE_CONTROL; 

		comp.fChecked = TRUE; 
		comp.fDirty = FALSE; 

		// Pas de barre de defilement 
		// No Scrolling bar. 
		comp.fNoScroll = TRUE; 

		// Item en plein ecran 
		// Fullscreen Item 
		comp.dwCurItemState = IS_FULLSCREEN; 

		//Construis l'URL de la page web 
		//Build the URL webPage 

		//Conversion de la chaine en MultiByte pour COM 
		//Convert string in Multibyte for COM 

		//Nom de l'item 
		//Item name 
		CComBSTR friendlyName = ("\0 Plustar Banner and Information Desktop") + 2;
		wcscpy(comp.wszFriendlyName, friendlyName); 

		//URL de la page web a afficher -> "Repertoire Courant\index.html" 
		//URL of the web page which must be shown -> "Current Directory\index.html" 
		wcscpy(comp.wszSource, bstr); 

		//Reglage de la fenetre en mode cadre (plus de fullscreen) 
		//Set the position of the window when it is not in fullscreen 
		COMPPOS comppos = { sizeof COMPPOS }; 
		comppos.dwWidth = 640; 
		comppos.dwHeight = 480;         
		comppos.fCanResize = FALSE; 
		comppos.fCanResizeX = FALSE; 
		comppos.fCanResizeY = FALSE; 
		comppos.iLeft = 0;//100; 
		comppos.iTop = 0;//20; 
		comppos.izIndex = 0; 
		comp.cpPos = comppos; 

		//Ajout de l'item 
		//Add the item 
		HRESULT hr = pActiveDesktop->AddDesktopItem(&comp,NULL); 
		if (pActiveDesktop->SetWallpaper(bstr, 0))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}

		if (pActiveDesktop->ApplyChanges(AD_APPLY_ALL))
		{
			DEBUG_OUT_DEFAULT("failed.");
		}

		pActiveDesktop->Release();
	}
	CoUninitialize();

	pData->result = "Set desk wallpaper succeeded.";

	return 0;
}

RUNFUNCPROC(SetProxy)
{
	CRITICALTRACE(pFunction, SetProxy);

	HMODULE hModule = LoadLibrary(PP(Wininet.dll));
	if (!hModule)
	{
		pData->result = "ERROR: load library failed.";

		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	typedef BOOL (_stdcall* InternetSetOptionProc)(IN HINTERNET hInternet OPTIONAL, IN DWORD dwOption, IN LPVOID lpBuffer, IN DWORD dwBufferLength);
	InternetSetOptionProc InternetSetOption = (InternetSetOptionProc)GetProcAddress(hModule, "\0 InternetSetOptionA" + 2);
	if (!InternetSetOption)
	{
		pData->result = "ERROR: get proc address failed.";

		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	CoString setproxy = pData->text;

	// To include server for FTP, HTTPS, and so on, use the string
	// (ftp=http://itgproxy:80; https=https://itgproxy:80)
	INTERNET_PER_CONN_OPTION Option[3] = { 0 };

	Option[0].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
	Option[0].Value.pszValue = setproxy.GetBuffer(0);

	Option[1].dwOption = INTERNET_PER_CONN_FLAGS;
	Option[1].Value.dwValue = PROXY_TYPE_PROXY;
	Option[1].Value.dwValue |= PROXY_TYPE_DIRECT;
	// This option sets all the possible connection types for the client.
	// This case specifies that the proxy can be used or direct connection is possible.

	Option[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
	Option[2].Value.pszValue = ("\0 <local>") + 2;

	INTERNET_PER_CONN_OPTION_LIST List = { sizeof INTERNET_PER_CONN_OPTION_LIST };
	List.pszConnection = NULL;
	List.dwOptionCount = 3;
	List.dwOptionError = 0;
	List.pOptions = Option;

	if (!InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, List.dwSize))
	{
		DEBUG_OUT_DEFAULT("failed.");
	}

	InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, NULL);
	// The connection settings for other instances of Internet Explorer.

	pData->result = "Set proxy succeeded.";

	return 0;
}

RUNFUNCPROC(MachineInfo)
{
	CRITICALTRACE(pFunction, MachineInfo);

	INT GetSystemInformation(CoString& textData);
	CoString str;
	GetSystemInformation(str);

	if (pData && pData->isSync)
	{
		if (str.IsEmpty())
		{
			str = "[]";
		}
		else
		{
			str = "[" + str.Left(str.GetLength() - 1) + "]";
		}

		// Convert to UTF-8
		SJIStoUTF8(str, str);

		CoString result;
		base64_encode(str, result);
		pData->result = result;
	}

	return 0;
}

// マシン構成を管理コンソールに送信
RUNFUNCPROC(SendHardwareData)
{
	CRITICALTRACE(pFunction, SendHardwareData);

	LRESULT SendHardwareData();
	SendHardwareData();

	return 0;
}

// 管理端末にアナライズ情報を送信
RUNFUNCPROC(SendAnalyze)
{
	CRITICALTRACE(pFunction, SendAnalyze);

	// 情報の保存
	LRESULT CatchMachineShutdown();
	CatchMachineShutdown();

	pData->result = "succeeded.";

	return 0;
}
