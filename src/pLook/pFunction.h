///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__P_FUNCTION_H__)
#define __P_FUNCTION_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h���I�u�W�F�N�g

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
// �X���b�h�����֐�

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
