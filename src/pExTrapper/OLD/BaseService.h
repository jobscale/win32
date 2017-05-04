///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//
// $Date: $
// $Rev: $
// $Author: $
// $HeadURL: $
//
// $Id: $
//

// ntservice.h
//
// Definitions for CBaseService
//

#ifndef _NTSERVICE_H_
#define _NTSERVICE_H_

#include <winsvc.h>
#include <setupapi.h>
#include <Dbt.h>
#include <time.h>

#include "ntservmsg.h" // Event message ids

#define SERVICE_CONTROL_USER 128

class CBaseService
{
public:
    CBaseService(const char* szServiceName);
    virtual ~CBaseService();

	BOOL IsInstalled();
    BOOL Install();
    BOOL Uninstall();
    BOOL StartService();
	virtual BOOL OnInit();
    virtual void StartRun();

    // static member functions
    static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
	static DWORD WINAPI HandlerEx(DWORD dwOpcode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

    // static member functions
    void ServiceMainCore(DWORD dwArgc, LPTSTR* lpszArgv);
	DWORD HandlerExCore(DWORD dwOpcode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

protected:
    BOOL Initialize();
    virtual void Run();
    BOOL ParseStandardArgs(int argc, char* argv[]);
    void LogEvent(WORD wType, DWORD dwID, const char* pszS1 = NULL, const char* pszS2 = NULL, const char* pszS3 = NULL);
    void SetStatus(DWORD dwState);
    virtual void OnStop();
    virtual void OnInterrogate();
    virtual void OnPause();
    virtual void OnContinue();
    virtual void OnShutdown();
	virtual void OnSignal();
	virtual BOOL OnUserControl(DWORD dwOpcode);
    
    // data members
    char m_szServiceName[64];
    SERVICE_STATUS_HANDLE m_hServiceStatus;
    SERVICE_STATUS m_Status;
    BOOL m_bIsRunning;

private:
    HANDLE m_hEventSource;

};

#endif // _NTSERVICE_H_
