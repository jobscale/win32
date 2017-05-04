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

#include <tlhelp32.h>
#include <psapi.h>
#pragma comment(lib, "psapi")

BOOL CALLBACK DefaultProcessEnumerator(DWORD dwPID, WORD wTask, LPCSTR szProcess, LPARAM lParam)
{
#ifndef _DEBUG
	return TRUE;
#endif
	int size = 4096;
	CComBSTR memory(size);
	size_t length = size * sizeof OLECHAR / sizeof TCHAR;
	LPTSTR strWork = (LPTSTR)(BSTR)memory;
	if (wTask == 0)
	{
		sprintf_s(strWork, length, "%5u   %s\n", dwPID, szProcess);
	}
	else
	{
		sprintf_s(strWork, length, " [Task] %5u %s\n", wTask, szProcess);
	}
	OutputDebugString(strWork);
	return TRUE;
}

typedef BOOL (CALLBACK* PROCENUMPROC)(DWORD dwPID, WORD wTask, LPCSTR szProcess, LPARAM lParam);
BOOL WINAPI EnumProcs(PROCENUMPROC lpProc, LPARAM lParam)
{
	// Retrieve the OS version
	OSVERSIONINFO osver = { sizeof osver };
	if (!GetVersionEx(&osver))
	{
		return FALSE;
	}

	// Get a handle to a Toolhelp snapshot of all processes.
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if (!lpProc)
	{
		lpProc = DefaultProcessEnumerator;
	}

	// Get the first process' information.
	PROCESSENTRY32 procEntry = { sizeof PROCESSENTRY32 };
	// While there are processes, keep looping.
	for (BOOL bFlag = Process32First(hSnapShot, &procEntry); bFlag; bFlag = Process32Next(hSnapShot, &procEntry))
	{
		// Call the enum func with the filename and ProcID.
		if (!lpProc(procEntry.th32ProcessID, 0, procEntry.szExeFile, lParam))
		{
			// Escape enumerate use.
			break;
		}
	}

	CloseHandle(hSnapShot);

	return TRUE;
}

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>

//  Forward declarations:
BOOL GetProcessList( );
BOOL ListProcessModules( DWORD dwPID );
BOOL ListProcessThreads( DWORD dwOwnerPID );
void printError( TCHAR* msg );

void main_GetProcessList( )
{
  GetProcessList( );
}

BOOL GetProcessList( )
{
  HANDLE hProcessSnap;
  HANDLE hProcess;
  PROCESSENTRY32 pe32;
  DWORD dwPriorityClass;

  // Take a snapshot of all processes in the system.
  hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
  if( hProcessSnap == INVALID_HANDLE_VALUE )
  {
    printError( TEXT("CreateToolhelp32Snapshot (of processes)") );
    return( FALSE );
  }

  // Set the size of the structure before using it.
  pe32.dwSize = sizeof( PROCESSENTRY32 );

  // Retrieve information about the first process,
  // and exit if unsuccessful
  if( !Process32First( hProcessSnap, &pe32 ) )
  {
    printError( TEXT("Process32First") ); // show cause of failure
    CloseHandle( hProcessSnap );          // clean the snapshot object
    return( FALSE );
  }

  // Now walk the snapshot of processes, and
  // display information about each process in turn
  do
  {
    printf( "\n\n=====================================================" );
    _tprintf( TEXT("\nPROCESS NAME:  %s"), pe32.szExeFile );
    printf( "\n-----------------------------------------------------" );

    // Retrieve the priority class.
    dwPriorityClass = 0;
    hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
    if( hProcess == NULL )
      printError( TEXT("OpenProcess") );
    else
    {
      dwPriorityClass = GetPriorityClass( hProcess );
      if( !dwPriorityClass )
        printError( TEXT("GetPriorityClass") );
      CloseHandle( hProcess );
    }

    printf( "\n  Process ID        = 0x%08X", pe32.th32ProcessID );
    printf( "\n  Thread count      = %d",   pe32.cntThreads );
    printf( "\n  Parent process ID = 0x%08X", pe32.th32ParentProcessID );
    printf( "\n  Priority base     = %d", pe32.pcPriClassBase );
    if( dwPriorityClass )
      printf( "\n  Priority class    = %d", dwPriorityClass );

    // List the modules and threads associated with this process
    ListProcessModules( pe32.th32ProcessID );
    ListProcessThreads( pe32.th32ProcessID );

  } while( Process32Next( hProcessSnap, &pe32 ) );

  CloseHandle( hProcessSnap );
  return( TRUE );
}


BOOL ListProcessModules( DWORD dwPID )
{
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
  MODULEENTRY32 me32;

  // Take a snapshot of all modules in the specified process.
  hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
  if( hModuleSnap == INVALID_HANDLE_VALUE )
  {
    printError( TEXT("CreateToolhelp32Snapshot (of modules)") );
    return( FALSE );
  }

  // Set the size of the structure before using it.
  me32.dwSize = sizeof( MODULEENTRY32 );

  // Retrieve information about the first module,
  // and exit if unsuccessful
  if( !Module32First( hModuleSnap, &me32 ) )
  {
    printError( TEXT("Module32First") );  // show cause of failure
    CloseHandle( hModuleSnap );           // clean the snapshot object
    return( FALSE );
  }

  // Now walk the module list of the process,
  // and display information about each module
  do
  {
    _tprintf( TEXT("\n\n     MODULE NAME:     %s"),   me32.szModule );
    _tprintf( TEXT("\n     Executable     = %s"),     me32.szExePath );
    printf( "\n     Process ID     = 0x%08X",         me32.th32ProcessID );
    printf( "\n     Ref count (g)  = 0x%04X",     me32.GlblcntUsage );
    printf( "\n     Ref count (p)  = 0x%04X",     me32.ProccntUsage );
    printf( "\n     Base address   = 0x%08X", (DWORD) me32.modBaseAddr );
    printf( "\n     Base size      = %d",             me32.modBaseSize );

  } while( Module32Next( hModuleSnap, &me32 ) );

  CloseHandle( hModuleSnap );
  return( TRUE );
}

BOOL ListProcessThreads( DWORD dwOwnerPID ) 
{ 
  HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
  THREADENTRY32 te32; 
 
  // Take a snapshot of all running threads  
  hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
  if( hThreadSnap == INVALID_HANDLE_VALUE ) 
    return( FALSE ); 
 
  // Fill in the size of the structure before using it. 
  te32.dwSize = sizeof(THREADENTRY32 ); 
 
  // Retrieve information about the first thread,
  // and exit if unsuccessful
  if( !Thread32First( hThreadSnap, &te32 ) ) 
  {
    printError( TEXT("Thread32First") ); // show cause of failure
    CloseHandle( hThreadSnap );          // clean the snapshot object
    return( FALSE );
  }

  // Now walk the thread list of the system,
  // and display information about each thread
  // associated with the specified process
  do 
  { 
    if( te32.th32OwnerProcessID == dwOwnerPID )
    {
      printf( "\n\n     THREAD ID      = 0x%08X", te32.th32ThreadID ); 
      printf( "\n     Base priority  = %d", te32.tpBasePri ); 
      printf( "\n     Delta priority = %d", te32.tpDeltaPri ); 
    }
  } while( Thread32Next(hThreadSnap, &te32 ) ); 

  CloseHandle( hThreadSnap );
  return( TRUE );
}

void printError( TCHAR* msg )
{
  DWORD eNum;
  TCHAR sysMsg[256];
  TCHAR* p;

  eNum = GetLastError( );
  FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, eNum,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
         sysMsg, 256, NULL );

  // Trim the end of the line and terminate it with a null
  p = sysMsg;
  while( ( *p > 31 ) || ( *p == 9 ) )
    ++p;
  do { *p-- = 0; } while( ( p >= sysMsg ) &&
                          ( ( *p == '.' ) || ( *p < 33 ) ) );

  // Display the message
  _tprintf( TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg );
}
