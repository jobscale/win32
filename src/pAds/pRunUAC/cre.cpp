//#include <atlcomtime.h>
#include <windows.h>
#include <stdio.h>

int Run(unsigned short sw, char* runProcess, char* param = NULL, unsigned long wait = INFINITE)
{
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { sizeof(STARTUPINFO) };

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = sw;

	char arg[1024 + 1];
	sprintf(arg, "\"%s\" %s", runProcess, param ? param : "");
	if (!CreateProcess(NULL, arg, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
	{
		return -1;
	}

	CloseHandle(pi.hThread);
	WaitForMultipleObjects(1, &pi.hProcess, TRUE, wait);
	CloseHandle(pi.hProcess);

	return 0;
}

int Start()
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hSCManager == NULL)
	{
		MessageBox(NULL, TEXT("SCMデータベースのオープンに失敗しました。"), NULL, MB_ICONWARNING);
		return -1;
	}
		
	SC_HANDLE hService = OpenService(hSCManager, "pads", SERVICE_START | SERVICE_STOP | SERVICE_QUERY_STATUS);
	if (hService == NULL)
	{
		MessageBox(NULL, TEXT("サービスのオープンに失敗しました。"), NULL, MB_ICONWARNING);	
		CloseServiceHandle(hSCManager);
		return -1;
	}
	
	SERVICE_STATUS ss = { 0 };
	QueryServiceStatus(hService, &ss);

	if (ss.dwCurrentState == SERVICE_STOPPED)
	{
		StartService(hService, 0, NULL);
	}

	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hService);

	return 0;
}

int main(int argc, char** argv)
{
	Start();

	Sleep(100);

	Run(SW_HIDE, "C:\\Program Files\\Plustar\\pads\\pWebBP.exe", 0, 0);

	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	return main(0, 0);
}
