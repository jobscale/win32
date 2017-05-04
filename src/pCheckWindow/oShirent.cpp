#include <winsock2.h>
#include <atlcomtime.h>

#pragma warning(disable: 4996)

///////  Original Device Code   
#define IO_DEVICE_TYPE				0x00001000
#define DRIVER_IO(_code)			CTL_CODE(IO_DEVICE_TYPE, _code, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_TEST					DRIVER_IO(0x1)
///////  CTL_CODE(IO_DEVICE_TYPE) : 32768 -  65535

#define IOCTRL_MUTEX				DRIVER_IO(0x2)
#define IOCTRL_SDT_HIDE_PROC_REST	DRIVER_IO(0x3)
#define	IOCTRL_GET_PROCESS_ID		DRIVER_IO(0x4)

HANDLE drvhFile = INVALID_HANDLE_VALUE;

LPCTSTR DriverBase()
{
	return "\0 shirent" + 2;
}

LPCTSTR DriverName()
{
	CString fpath;
	GetModuleFileName(NULL, fpath.GetBuffer(_MAX_PATH), _MAX_PATH);

	CString drive, dir, fname, ext;
	_splitpath(fpath, drive.GetBuffer(_MAX_DRIVE), dir.GetBuffer(_MAX_DIR), fname.GetBuffer(_MAX_FNAME), ext.GetBuffer(_MAX_EXT));

	static CString path;
	sprintf(path.GetBuffer(_MAX_PATH + 1), "\0 %s%s%s.sys" + 2, (LPCTSTR)drive, (LPCTSTR)dir, DriverBase());

	return path;
}

LPCTSTR DriverFileName()
{
	static CString dfn;
	dfn.Format("\0 \\\\.\\%s" + 2, DriverBase());
	return dfn;
}

LPCTSTR	DriverServiceName()
{
	return DriverBase();
}

//// Driver check
int DriverCreate(){
	drvhFile = CreateFile(	DriverFileName(), 
							GENERIC_READ | GENERIC_WRITE,
							0,	0,	OPEN_EXISTING,
							0,	0 );
	if( drvhFile == INVALID_HANDLE_VALUE  )
	{
		MessageBox(0,"CreateFile Function Err", "warning", 0);
		PostQuitMessage(0);
	}
	return 0;
}

int DriverClose()
{
	if (drvhFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(drvhFile);
	}
	return 0;
}

//// Driver Install
BOOL SCMInstall()
{
	BOOL  ret = true;
	SC_HANDLE	hSCM;
	SC_HANDLE	hSrv;
	SERVICE_STATUS	srvstatus;

		char DriverFullPath[MAX_PATH];
		//char dbgmsg[MAX_PATH];
		
		hSCM= OpenSCManager ( NULL, NULL, 
							  SC_MANAGER_ALL_ACCESS );
		if(hSCM)
		{
			hSrv = OpenService ( hSCM, DriverServiceName(), 
							     SERVICE_ALL_ACCESS );
			if( hSrv )
			{
				ret = ControlService ( hSrv, SERVICE_CONTROL_STOP, 
										&srvstatus);

				ret = DeleteService (hSrv);
				CloseServiceHandle  (hSrv);
			}

			DriverFullPath[0] = 0;
			lstrcat	(DriverFullPath, DriverName());
			
			hSrv = CreateService ( hSCM, DriverServiceName(), DriverServiceName(),
								   SERVICE_ALL_ACCESS,	SERVICE_KERNEL_DRIVER, 
								   SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
								   DriverFullPath,
								   NULL, NULL, NULL, NULL, NULL );
			
			if(hSrv)
			{
				ret = StartService (hSrv, 0, NULL);
				CloseServiceHandle (hSrv);
			}
			CloseServiceHandle(hSCM);
		}
	
	return ret;
}

//// Driver Uninstall 
BOOL SCMUninstall()
{
	BOOL  ret = false;
	SC_HANDLE	hSCM;
	SC_HANDLE	hSrv;
	SERVICE_STATUS	srvstatus;

	hSCM = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
	hSrv = OpenService (hSCM, DriverServiceName(), 
						SERVICE_ALL_ACCESS);
	if(hSrv)
	{
		ret = ControlService(hSrv, SERVICE_CONTROL_STOP, &srvstatus);
		if(TRUE==ret)
		{
			ret = DeleteService(hSrv);
			CloseServiceHandle(hSCM);
		}
	}

	return ret;
}

int StartDriver()
{
	HANDLE chandle = CreateMutex(NULL, NULL, DriverBase());
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(0, "running multiple failed.", "warning", 0);
		PostQuitMessage(0);
	}

	SCMInstall(); 
	DriverCreate();

	return 0;
}

int StopDriver()
{
	DriverClose();
	SCMUninstall();

	return 0;
}
