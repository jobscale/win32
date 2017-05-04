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

// コンパイル時に警告を無視する
//#define _CRT_SECURE_NO_DEPRECATE 1
//#define _CRT_NONSTDC_NO_DEPRECATE 1

#include "BaseService.h"

#define _OFC_EXPORT_
#include "../ofc/oString.h"

#include "../pLook/pUtility.h"

// NOTE: FOR DFAT TESTING ONLY
//#define DEBUG_TO_FILE 1

CBaseService& BaseService(CBaseService* _pService = NULL)
{
	static CBaseService* pService = _pService;
	if (!pService)
	{
		_asm
		{
			int 3
		}
	}
	return *pService;
}

CBaseService& GetBaseService()
{
	return BaseService();
}

CBaseService::CBaseService(const char* szServiceName)
{
	BaseService(this);

    // Set the default service name and version
    strncpy(m_szServiceName, szServiceName, sizeof m_szServiceName - 1);
    m_hEventSource = NULL;

    // set up the initial service status 
    m_hServiceStatus = NULL;
    m_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_Status.dwCurrentState = SERVICE_STOPPED;
    m_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_Status.dwWin32ExitCode = 0;
    m_Status.dwServiceSpecificExitCode = 0;
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 0;
    m_bIsRunning = FALSE;
}

CBaseService::~CBaseService()
{
    if (m_hEventSource) {
        ::DeregisterEventSource(m_hEventSource);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// Default command line argument parsing

// Returns TRUE if it found an arg it recognised, FALSE if not
// Note: processing some arguments causes output to stdout to be generated.
BOOL CBaseService::ParseStandardArgs(int argc, char* argv[])
{
    // See if we have any command line args we recognise
    if (argc <= 1) return FALSE;

    if (_stricmp(argv[1], "-i") == 0) {

        // Request to install.
        if (IsInstalled()) {
            printf("%s is already installed\n", m_szServiceName);
        } else {
            // Try and install the copy that's running
            if (Install()) {
                printf("%s installed\n", m_szServiceName);
            } else {
                printf("%s failed to install. Error %d\n", m_szServiceName, GetLastError());
            }
        }
        return TRUE; // say we processed the argument

    } else if (_stricmp(argv[1], "-u") == 0) {

        // Request to uninstall.
        if (!IsInstalled()) {
            printf("%s is not installed\n", m_szServiceName);
        } else {
            // Try and remove the copy that's installed
            if (Uninstall()) {
                // Get the executable file path
                char szFilePath[_MAX_PATH];
                ::GetModuleFileName(NULL, szFilePath, sizeof szFilePath);
                printf("%s removed.\n",
                       m_szServiceName);
            } else {
                printf("Could not remove %s. Error %d\n", m_szServiceName, GetLastError());
            }
        }
        return TRUE; // say we processed the argument
    }
         
    // Don't recognise the args
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////
// Install/uninstall routines

// Test if the service is currently installed
BOOL CBaseService::IsInstalled()
{
    BOOL bResult = FALSE;

    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (hSCM) {

        // Try to open the service
        SC_HANDLE hService = ::OpenService(hSCM,
                                           m_szServiceName,
                                           SERVICE_QUERY_CONFIG);
        if (hService) {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }

        ::CloseServiceHandle(hSCM);
    }
    
    return bResult;
}

BOOL CBaseService::Install()
{
    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (!hSCM) return FALSE;

    // Get the executable file path
    char szFilePath[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, sizeof szFilePath);

    // Create the service
    // SC_HANDLE CreateService(
    //   SC_HANDLE hSCManager,       // SCM データベースのハンドル
    //   LPCTSTR lpServiceName,      // 開始したいサービスの名前
    //   LPCTSTR lpDisplayName,      // 表示名
    //   DWORD dwDesiredAccess,      // サービスのアクセス権のタイプ
    //   DWORD dwServiceType,        // サービスのタイプ
    //   DWORD dwStartType,          // サービスを開始する時期
    //   DWORD dwErrorControl,       // サービスに失敗したときの深刻さ
    //   LPCTSTR lpBinaryPathName,   // バイナリファイル名
    //   LPCTSTR lpLoadOrderGroup,   // ロード順序を決定するグループ名
    //   LPDWORD lpdwTagId,          // タグ識別子
    //   LPCTSTR lpDependencies,     // 複数の依存名からなる配列
    //   LPCTSTR lpServiceStartName, // アカウント名
    //   LPCTSTR lpPassword          // アカウントのパスワード
    // );
    SC_HANDLE hService = ::CreateService(hSCM,
                                         m_szServiceName,
                                         m_szServiceName,
                                         SERVICE_ALL_ACCESS,
                                         SERVICE_INTERACTIVE_PROCESS | SERVICE_WIN32_OWN_PROCESS,
                                         SERVICE_AUTO_START,       
                                         SERVICE_ERROR_NORMAL,
                                         szFilePath,
                                         NULL,
                                         NULL,
                                         "\0\0",
                                         NULL,
                                         NULL);
    if (!hService) {
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }

    // make registry entries to support logging messages
    // Add the source name as a subkey under the Application
    // key in the EventLog service portion of the registry.
    char szKey[256];
    HKEY hKey = NULL;
	_snprintf(szKey, sizeof szKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s", m_szServiceName);

    if (::RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS) {
        ::CloseServiceHandle(hService);
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }

    // Add the Event ID message-file name to the 'EventMessageFile' subkey.
    ::RegSetValueEx(hKey, "EventMessageFile", 0, REG_EXPAND_SZ, (CONST BYTE*)szFilePath, (int)strlen(szFilePath) + 1);

    // Set the supported types flags.
    DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
    ::RegSetValueEx(hKey, "TypesSupported", 0, REG_DWORD, (CONST BYTE*)&dwData, sizeof DWORD);
    ::RegCloseKey(hKey);

    LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_INSTALLED, m_szServiceName);

    // tidy up
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

BOOL CBaseService::Uninstall()
{
    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (!hSCM) return FALSE;

    BOOL bResult = FALSE;
    SC_HANDLE hService = ::OpenService(hSCM,
                                       m_szServiceName,
                                       DELETE);
    if (hService) {
        if (::DeleteService(hService)) {
            LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_REMOVED, m_szServiceName);
            bResult = TRUE;
        } else {
            LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_NOTREMOVED, m_szServiceName);
        }
        ::CloseServiceHandle(hService);
    }
    
    ::CloseServiceHandle(hSCM);
    return bResult;
}

///////////////////////////////////////////////////////////////////////////////////////
// Logging functions

// This function makes an entry into the application event log
void CBaseService::LogEvent(WORD wType, DWORD dwID,
                          const char* pszS1,
                          const char* pszS2,
                          const char* pszS3)
{
    const char* ps[3];
    ps[0] = pszS1;
    ps[1] = pszS2;
    ps[2] = pszS3;

    int iStr = 0;
    for (int i = 0; i < 3; i++) {
        if (ps[i] != NULL) iStr++;
    }
        
    // Check the event source has been registered and if
    // not then register it now
    if (!m_hEventSource) {
        m_hEventSource = ::RegisterEventSource(NULL,  // local machine
                                               m_szServiceName); // source name
    }

    if (m_hEventSource) {
        ::ReportEvent(m_hEventSource,
                      wType,
                      0,
                      dwID,
                      NULL, // sid
                      iStr,
                      0,
                      ps,
                      NULL);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Service startup and registration

BOOL CBaseService::StartService()
{
    SERVICE_TABLE_ENTRY st[] = {
        {m_szServiceName, ServiceMain},
        {NULL, NULL}
    };

    BOOL b = ::StartServiceCtrlDispatcher(st);
    return b;
}

// static member function (callback)
void WINAPI CBaseService::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	return GetBaseService().ServiceMainCore(dwArgc, lpszArgv);
}

void CBaseService::ServiceMainCore(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Register the control request handler
    m_Status.dwCurrentState = SERVICE_START_PENDING;
	//HandlerEx function not supported under NT4
    m_hServiceStatus = RegisterServiceCtrlHandlerEx(m_szServiceName, (LPHANDLER_FUNCTION_EX)HandlerEx, 0);
    if (m_hServiceStatus == NULL) {
        LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_CTRLHANDLERNOTINSTALLED);
        return;
    }

	// Start the initialisation
    if (Initialize()) {

        // Do the real work. 
        // When the Run function returns, the service has stopped.
        m_bIsRunning = TRUE;
        m_Status.dwWin32ExitCode = 0;
        m_Status.dwCheckPoint = 0;
        m_Status.dwWaitHint = 0;
        Run();
    }

    // Tell the service manager we are stopped
    SetStatus(SERVICE_STOPPED);
}

///////////////////////////////////////////////////////////////////////////////////////////
// status functions

void CBaseService::SetStatus(DWORD dwState)
{
    m_Status.dwCurrentState = dwState;
    ::SetServiceStatus(m_hServiceStatus, &m_Status);
}

///////////////////////////////////////////////////////////////////////////////////////////
// Service initialization

BOOL CBaseService::Initialize()
{
    // Start the initialization
    SetStatus(SERVICE_START_PENDING);
    
    // Perform the actual initialization
    BOOL bResult = OnInit(); 
    
    // Set final state
    m_Status.dwWin32ExitCode = GetLastError();
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 0;
    if (!bResult) {
        LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_FAILEDINIT);
        SetStatus(SERVICE_STOPPED);
        return FALSE;    
    }
    
    LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STARTED);
    SetStatus(SERVICE_RUNNING);

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// main function to do the real work of the service

// This function performs the main work of the service. 
// When this function returns the service has stopped.
void CBaseService::Run()
{
    while (m_bIsRunning) {
        // OutputViewer("Sleeping...");
        Sleep(5000);
    }

    // nothing more to do
}

void CBaseService::StartRun()
{
	m_bIsRunning = TRUE;
	Run();
}

//////////////////////////////////////////////////////////////////////////////////////
// Control request handlers

DWORD WINAPI CBaseService::HandlerEx(DWORD dwOpcode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	return GetBaseService().HandlerExCore(dwOpcode, dwEventType, lpEventData, lpContext);
}

DWORD CBaseService::HandlerExCore(DWORD dwOpcode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
    switch (dwOpcode) {
    case SERVICE_CONTROL_STOP: // 1
        SetStatus(SERVICE_STOP_PENDING);
        OnStop();
        m_bIsRunning = FALSE;
        LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED);
		SetStatus(SERVICE_STOPPED);
        break;

    case SERVICE_CONTROL_PAUSE: // 2
        OnSignal();
		OnPause();
        break;

    case SERVICE_CONTROL_CONTINUE: // 3
        OnSignal();
		OnContinue();
        break;

    case SERVICE_CONTROL_INTERROGATE: // 4
        OnInterrogate();
		OnSignal();
        break;

    case SERVICE_CONTROL_SHUTDOWN: // 5
        SetStatus(SERVICE_STOP_PENDING);
        OnShutdown();
        m_bIsRunning = FALSE;
        LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED);
		SetStatus(SERVICE_STOPPED);
		m_bIsRunning = FALSE;
        break;

	case SERVICE_CONTROL_DEVICEEVENT: // B
		::SetServiceStatus(m_hServiceStatus, &m_Status);

		break;

    default:
		OnSignal();
        if (dwOpcode >= SERVICE_CONTROL_USER) {
            if (!OnUserControl(dwOpcode)) {
                LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
            }
        } else {
            LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
        }
        break;
    }

    // Report current status
    ::SetServiceStatus(m_hServiceStatus, &m_Status);

	return NO_ERROR;
}

// Called when the service is first initialized
BOOL CBaseService::OnInit()
{
	return TRUE;
}

// Called when the service control manager wants to stop the service
void CBaseService::OnStop()
{
}

// called when the service is interrogated
void CBaseService::OnInterrogate()
{
}

// called when the service is paused
void CBaseService::OnPause()
{
}

// called when the service is continued
void CBaseService::OnContinue()
{
}

// called when the service is shut down
void CBaseService::OnShutdown()
{
}

// called for any other service signal
void CBaseService::OnSignal()
{
}

// called when the service gets a user control message
BOOL CBaseService::OnUserControl(DWORD dwOpcode)
{
    return FALSE; // say not handled
}
