

#include <atlbase.h>
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
