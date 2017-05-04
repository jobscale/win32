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

#include "pLogging.h"

#include "pUtility.h"

#include <winspool.h>

#include "oMyJson.h"

#define MAX_PRINTERS 256
#define dwFlags PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL

///////////////////////////////////////////////////////////////////////////////////////////////////
// 印刷ジョブの監視

DWORD WINAPI PrintJobThread(LPVOID pParam)
{
	/***********************************************************************
	Call EnumPrinters with a PRINTER_INFO_4 to enumerate all the printers 
	installed on the machine locally or networked.
	PRINTER_INFO_4 queries the registry to find all Prns.
	PRINTER_INFO_4 : Name=NULL; function will query on the Local machine;
	Also, because Level=4, you can only use the above two flags (dwFlags).
	***********************************************************************/ 

	DWORD pcbNeeded = 0, pcReturned = 0, dwBytesNeeded = 0, dwReturned = 0;
	JOB_INFO_2* pJobInfo = NULL;
	PRINTER_INFO_4* pinfo4 = NULL;

	int previous[MAX_PRINTERS] = { 0 };
	HANDLE hPrinter[MAX_PRINTERS] = { 0 };

	while ( !EnumPrinters (dwFlags, NULL, 4, (LPBYTE) pinfo4, dwBytesNeeded, &dwBytesNeeded, &dwReturned) )
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			if (pinfo4) LocalFree(pinfo4);
			pinfo4 = (PRINTER_INFO_4 *) LocalAlloc(LPTR, dwBytesNeeded);
		}
		else
		{
			DWORD error = GetLastError();
			CoString msg;
			GetFormatMessage(error, msg);
			TRACE("EnumPrinters failed: [%X]: %s\n", error, (LPCTSTR)msg);
			return 1;
		}
	}

	TRACE("PRINTERS (Local/Networked) INSTALLED ON YOUR MACHINE = %lu\n", dwReturned);
	for (int i = 0; i < (int)dwReturned; i++)
	{
		TRACE("Printer #%d: %s \n", i + 1, pinfo4[i].pPrinterName);
	}

	/**********************************************************************/ 

	for (int i = 0; i < (int)dwReturned; i++)
	{
		if ( !OpenPrinter( pinfo4[i].pPrinterName, (LPHANDLE)&hPrinter[i], (LPPRINTER_DEFAULTS)NULL) )
		{
			TRACE("OpenPrinter failed: %d\n", GetLastError()); 
			return 1;
		}
	}

	/**********************************************************************/ 

	// Keep polling forever and dump the info whenever a job is submitted to any printer.
	for (; ; )
	{
		Sleep(1);
		for (int i = 0; i < (int)dwReturned; i++) // Number of printers installed.
		{
			/**************************************************************/ 
			while ( !EnumJobs((HANDLE)hPrinter[i], 0, 1, 2, (LPBYTE)pJobInfo, 
					pcbNeeded, (LPDWORD)&pcbNeeded, (LPDWORD)&pcReturned) )
			{ 
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					if (pJobInfo) LocalFree(pJobInfo);
					pJobInfo = (JOB_INFO_2 *) LocalAlloc(LPTR, pcbNeeded); 
				}
				else
				{
					TRACE("EnumJobs on printer %d failed: %d\n", i + 1, GetLastError());
					goto Cleanup;
				}
			}

			if (pcReturned > 0) // There is a JOB to print from printer[i]
			{ 
				if ((int)pJobInfo->JobId != previous[i])
				{
					CoString strText, columnName, workStr;

					// next job
					TRACE("PRINTER #%d: IS PRINTING\n",i+1);

					/******************************************************/ 

					if (pJobInfo->pDevMode->dmFields & DM_YRESOLUTION)
					{
						// Y-Resolution (dpi)
						columnName = P("Y-Resolution");
						workStr.Format("%d", pJobInfo->pDevMode->dmYResolution);
						strText += workStr;
					}

					if (pJobInfo->pDevMode->dmPrintQuality & (DMRES_HIGH | DMRES_MEDIUM | DMRES_LOW | DMRES_DRAFT))
					{
						// Print Quality (dpi)
						columnName = P("PrintQuality");
						workStr.Format("%d", pJobInfo->pDevMode->dmPrintQuality);
						strText += workStr;
					}

					/******************************************************/ 
					/* Check if Orientation is Portrait or Landscape */ 
					/******************************************************/ 

					if (pJobInfo->pDevMode->dmOrientation == DMORIENT_PORTRAIT)
					{
						workStr.Format("Orientation:Portrait\n");
						strText += workStr;
					}

					if (pJobInfo->pDevMode->dmOrientation == DMORIENT_LANDSCAPE)
					{
						workStr.Format("Orientation:Landscape\n");
						strText += workStr;
					}

					/******************************************************/ 

					if (pJobInfo->pDevMode->dmFields & DM_COPIES)
					{
						workStr.Format("Num of Copies: %d\n", pJobInfo->pDevMode->dmCopies);
						strText += workStr;
					}

					if (pJobInfo->pDevMode->dmFields & DM_PAPERLENGTH)
					{
						workStr.Format("Paper Length: %d\n", pJobInfo->pDevMode->dmPaperLength);
						strText += workStr;
					}

					if (pJobInfo->pDevMode->dmFields & DM_PAPERWIDTH)
					{
						workStr.Format("Paper Width: %d\n", pJobInfo->pDevMode->dmPaperWidth);
						strText += workStr;
					}

					/******************************************************/ 

					if (pJobInfo->pDevMode->dmDuplex & DM_DUPLEX)
					{
						workStr.Format("Duplex Mode ON \n");
						strText += workStr;
					}

					if (pJobInfo->pDevMode->dmColor == DMCOLOR_COLOR)
					{
						workStr.Format("Color Image\n"); // COLOR
					}
					else
					{
						workStr.Format("Monochrome Image\n"); // MONOCHROME
					}
					strText += workStr;

					if (pJobInfo->pDevMode->dmFields & DM_BITSPERPEL)
					{
						workStr.Format("Bits Per Pel: %d\n", pJobInfo->pDevMode->dmBitsPerPel);
						strText += workStr;
					}

					///////////////////////////////////////////////////////////////////////////////
					previous[i] = pJobInfo->JobId;

					///////////////////////////////////////////////////////////////////////////////
					// DateTime
					SYSTEMTIME sysTime;
					COleDateTime dateTime = COleDateTime::GetCurrentTime();
					dateTime.GetAsSystemTime(sysTime);

					///////////////////////////////////////////////////////////////////////////////
					// Job information
					workStr.Format("\nJOB INFO-2 JobId : %lu\n", pJobInfo->JobId);
					strText += workStr;
					workStr.Format("Printer Name : %s\n",pJobInfo->pPrinterName);
					strText += workStr;
					workStr.Format("Machine Name : %s\n",pJobInfo->pMachineName);
					strText += workStr;
					workStr.Format("User Name : %s\n",pJobInfo->pUserName);
					strText += workStr;
					workStr.Format("Datatype : %s\n",pJobInfo->pDatatype);
					strText += workStr;
					workStr.Format("Print Processor : %s\n",pJobInfo->pPrintProcessor);
					strText += workStr;
					workStr.Format("Parameters : %s\n",pJobInfo->pParameters);
					strText += workStr;
					workStr.Format("Driver Name : %s\n",pJobInfo->pDriverName);
					strText += workStr;
					workStr.Format("Size : %lu\n",pJobInfo->Size);
					strText += workStr;
					workStr.Format("NotifyName : %s\n",pJobInfo->pNotifyName);
					strText += workStr;

					// 以下のデータをアップロード
					strText.Empty();
#ifdef USE_JSONCPP
					Json::Value *jVal = create_AValue();
					int idx = 0;

					workStr.Format("%s", pJobInfo->pDocument);
					workStr.Replace("\"", "");
					(*jVal)[idx++][P("Document")] = workStr;

					workStr.Format("%lu", pJobInfo->TotalPages);
					(*jVal)[idx++][P("TotalPages")] = workStr;

					(*jVal)[idx++][P("DateTime")] = dateTime.Format("%Y/%m/%d %H:%M:%S");

					strText = jVal->toFastString().c_str();
					jVal->release();
#else

					columnName = P("Document");
					workStr.Format("%s", pJobInfo->pDocument);
					workStr.Replace("\"", "");
					strText += "[ { \"" + columnName + "\": \"" + workStr + "\" }, ";

					columnName = P("TotalPages");
					workStr.Format("%lu", pJobInfo->TotalPages);
					strText += "{ \"" + columnName + "\": \"" + workStr + "\" }, ";

					columnName = P("DateTime");
					workStr.Format("%s", dateTime.Format("%Y/%m/%d %H:%M:%S"));
					strText += "{ \"" + columnName + "\": \"" + workStr + "\" } ]";
#endif

					// 印刷ジョブの中身

					// 印刷情報のアップロード
					LRESULT UploadPrintingData(CoString& jData);
					UploadPrintingData(strText);
				}
			}
		}
	}

Cleanup:
	LocalFree (pinfo4); LocalFree (pJobInfo); //cleanup memory!

	for (int i = 0; i < (int)dwReturned; i++)
	{
		if (!ClosePrinter( (HANDLE)hPrinter[i]))
		{
			TRACE("ClosePrinter failed: %d\n", GetLastError());
			return 1;
		}
	}
	return (0);
}
