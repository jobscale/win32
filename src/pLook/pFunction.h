///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__P_FUNCTION_H__)
#define __P_FUNCTION_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド情報オブジェクト

struct TRANSACTIONDATA
{
	BOOL isSync;
	CoString text;
	CoString result;

	TRANSACTIONDATA(BOOL _isSync = TRUE)
		: isSync(_isSync)
	{
	}
};
typedef TRANSACTIONDATA* LPTRANSACTIONDATA;

///////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド処理関数

#define COMTHREAD(name) \
	Command##name##Thread

#define COMTHREADPROC(name) \
	DWORD WINAPI COMTHREAD(name)(LPVOID pParam)

#define RUNFUNC(name) \
	Run##name##Function

#define RUNFUNCPROC(name) \
	LRESULT RUNFUNC(name)(LPTRANSACTIONDATA pData)

RUNFUNCPROC(Shutdown);
RUNFUNCPROC(Reboot);
RUNFUNCPROC(ForegroundList);
RUNFUNCPROC(ProcessList);
RUNFUNCPROC(ConnectionList);
RUNFUNCPROC(UrlHistory);
RUNFUNCPROC(Uninstall);
RUNFUNCPROC(ScreenShot);
RUNFUNCPROC(KillProcess);
RUNFUNCPROC(DeskWallPaper);
RUNFUNCPROC(SetProxy);
RUNFUNCPROC(MachineInfo);
RUNFUNCPROC(SendHardwareData);
RUNFUNCPROC(SendAnalyze);

#endif // __P_FUNCTION_H__
