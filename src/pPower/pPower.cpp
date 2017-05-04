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

// pPower.cpp : アプリケーション用のエントリ ポイントの定義
//

#include <afxwin.h>

#include <Winsock2.h>
#pragma comment(lib, "ws2_32")
#define BESTBUFFERSIZE 204800
#define BESTRECVSIZE 256

// グローバル変数:
HINSTANCE hInst;					// 現在のインスタンス
TCHAR szTitle[] = "pPower";				// タイトル バー テキスト
TCHAR szWindowClass[] = "pPowerClass";

// このコード モジュールに含まれる関数の前宣言:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

CStringArray g_ini;

BOOL GetArrayValue(CStringArray& value, const CString text, LPCTSTR separator, BOOL ignoreNoCase/* = TRUE*/)
{
	value.RemoveAll();

	INT sepLength = strlen(separator);
	INT length = text.GetLength();
	for (INT nBegin = 0; nBegin < length; )
	{
		INT nEnd = text.Find(separator, nBegin);
		if (nEnd == -1)
		{
			nEnd = length;
		}
		INT size = nEnd - nBegin;
		if (size)
		{
			CString result = text.Mid(nBegin, size);
			if (ignoreNoCase)
			{
				result.MakeLower();
			}
			value.Add(result);
		}
		nBegin = nEnd + sepLength;
	}

	return TRUE;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, "\0 pPowerMutex" + 2);
	if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// duplicate process failed.
		return -1;
	}

	CString folder;
	GetModuleFileName(NULL, folder.GetBuffer(_MAX_PATH), _MAX_PATH);
	folder.ReleaseBuffer();
	CString drive, dir, fname, ext;
	_splitpath_s(folder, drive.GetBuffer(_MAX_DRIVE), _MAX_DRIVE, dir.GetBuffer(_MAX_DIR), _MAX_DIR, fname.GetBuffer(_MAX_FNAME), _MAX_FNAME, ext.GetBuffer(_MAX_EXT), _MAX_EXT);
	drive.ReleaseBuffer(); dir.ReleaseBuffer(); fname.ReleaseBuffer();
	folder = drive + dir + fname;
	CString ini;
	HANDLE hFile = CreateFile(folder + ".ini", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile)
	{
		DWORD dwRead;
		ReadFile(hFile, ini.GetBuffer(204800), 204800, &dwRead, NULL);
		ini.GetBufferSetLength(dwRead);
		CloseHandle(hFile);
	}

	ini.Replace("\r\n", "");
	ini.Replace("\r", "");
	ini.Replace("\n", "");
	GetArrayValue(g_ini, ini, ",", FALSE);

	if (g_ini.GetSize() < 1)
	{
		//MessageBox(NULL, "error ini read failed.", "ERROR", MB_ICONERROR | MB_OK);
		g_ini.Add("http://gatekeeper.plustar.jp/pow.php");		   
		//return -1;
	}
	if (g_ini.GetSize() < 2)
	{
		g_ini.Add("0");
	}

	// グローバル ストリングを初期化します
	if (!MyRegisterClass(hInstance))
	{
		return FALSE;
	}

	// アプリケーションの初期化を行います:
	if (!InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	// メイン メッセージ ループ:
	MSG msg = { 0 };
	for (; GetMessage(&msg, NULL, 0, 0); )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof WNDCLASSEX; 

	wcex.style		= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // グローバル変数にインスタンス ハンドルを保存します

   hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, szWindowClass, szTitle, WS_POPUPWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd) 
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

INT GetHostInfo(CString& hostname, CString& ipaddr, CString& username)
{
	WSADATA wsaData;
	if (WSAStartup(WINSOCK_VERSION, &wsaData) != NO_ERROR)
	{
		return -1;
	}
	if (gethostname(hostname.GetBuffer(BESTBUFFERSIZE),  BESTBUFFERSIZE) != NO_ERROR)
	{
		WSACleanup();
		return -1;
	}
	hostname.ReleaseBuffer();
	LPHOSTENT pEnt = gethostbyname(hostname);
	if (!pEnt)
	{
		WSACleanup();
		return -1;
	}
	HOSTENT ent = *pEnt;
	WSACleanup();

	hostname = ent.h_name;
	ipaddr.Format("\0 %d.%d.%d.%d" + 2, (BYTE)ent.h_addr_list[0][0], (BYTE)ent.h_addr_list[0][1], (BYTE)ent.h_addr_list[0][2], (BYTE)ent.h_addr_list[0][3]);

	DWORD dwSize = 256 + 1; // UNLEN
	GetUserName(username.GetBuffer(dwSize), &dwSize);
	username.ReleaseBuffer();

	return 0;
}

INT GetRecvData(SOCKET sock, CString& recvData)
{
	// read
	recvData.Empty();
	INT contentsLength = 0;
	INT bytesRecv = BESTRECVSIZE;
	CString recvbuf;
	CString oddLot;

	for (; ;) 
	{
		bytesRecv = recv(sock, recvbuf.GetBuffer(BESTRECVSIZE), BESTRECVSIZE, 0);
		if (bytesRecv == 0)
		{
			break;
		}
		if (bytesRecv == WSAECONNRESET || bytesRecv == SOCKET_ERROR) 
		{
			break;
		}
		recvbuf.GetBufferSetLength(bytesRecv);
		oddLot += recvbuf;

		if (!contentsLength)
		{
			for (; ;)
			{
				CString cl = ("\0 Content-Length: ") + 2;
				INT posLength = oddLot.Find(cl);
				INT pos = oddLot.Find("\0 \r\n" + 2);
				if (pos == -1)
				{
					break;
				}
				if (posLength == 0)
				{
					contentsLength = strtol(&((LPCTSTR)oddLot)[cl.GetLength()], NULL, 10);
				}
				oddLot = oddLot.Right(oddLot.GetLength() - pos - 2);
				if (contentsLength && pos == 0)
				{
					break;
				}

				if (pos == 0)
				{
					INT posNext = oddLot.Find("\0 \n" + 2);
					if (posNext == -1)
					{
						break;
					}
					contentsLength = strtol(oddLot, NULL, 16);
					if (contentsLength == 0)
					{
						contentsLength = INT_MAX;
					}
					else
					{
						oddLot = oddLot.Right(oddLot.GetLength() - posNext - 1);
					}
				}
			}
			if (!contentsLength)
			{
				continue;
			}
		}

		recvData += oddLot;
		oddLot.Empty();

		if (recvData.GetLength() > contentsLength)
		{
			continue;
			ASSERT(bytesRecv != BESTRECVSIZE);
			recvData.GetBufferSetLength(contentsLength);
			break;
		}
	}

	return 0;
}

INT HttpSendRecv(SOCKET sock, const CString& sendData, CString* recvData)
{
	// send
	int size = sendData.GetLength();
	int bytesSent = send(sock, sendData, size, 0);

	if (bytesSent != size)
	{
		return -1;
	}

	if (!recvData)
	{
		return 0;
	}

	// read
	return GetRecvData(sock, *recvData);
}

int get_http(LPCTSTR _TargetIP, LPCTSTR _ProxyIP, WORD _ProxyPort, CString* bodyData)
{
	// Target
	CString TargetIP = _TargetIP;
	// Proxy
	CString ProxyIP = _ProxyIP;
	// Port
	WORD ProxyPort = _ProxyPort;

	if (ProxyIP.IsEmpty())
	{
		ProxyIP = TargetIP.Right(TargetIP.GetLength() - 7);
		INT pos = ProxyIP.Find("\0 /" + 2);
		if (pos > 0)
		{
			ProxyIP = ProxyIP.Left(pos);
		}
		ProxyPort = 80;
	}

	WSADATA wsaData;
	if (WSAStartup(WINSOCK_VERSION, &wsaData) != NO_ERROR)
	{
		return -1;
	}

	SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) 
	{
		WSACleanup();
		return -1;
	}

	hostent* host = gethostbyname(ProxyIP);
	if (host == NULL)
	{
		if (WSAGetLastError() == WSAHOST_NOT_FOUND)
		{
		}
		WSACleanup();
		return -1;
	}

	unsigned int** addrptr = (unsigned int **)host->h_addr_list;

	SOCKADDR_IN clientService = { 0 };
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = *(*addrptr);
	clientService.sin_port = htons(ProxyPort);

	if (connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof clientService) == SOCKET_ERROR) 
	{
		WSACleanup();
		return -1;
	}

	CString bufSend;
	bufSend.Format("\0 "
		"GET %s HTTP/1.1\r\n"
		"User-Agent: Plustar pLook browsing transaction.\r\n"
		"Host: localhost\r\n"
		"Proxy-Connection: Keep-Alive\r\n\r\n" + 2,
		TargetIP
	);

	int result = HttpSendRecv(ConnectSocket, bufSend, bodyData);

	closesocket(ConnectSocket);

	WSACleanup();

	return result;
}

DWORD PowerPosting(INT nPower)
{
	static INT power = -1;

	if (power == nPower)
	{
		// 重複させない
		return 0;
	}
	power = nPower;

	CString hostname, ipaddr, username;
	GetHostInfo(hostname, ipaddr, username);

	CString TargetIP;
	TargetIP.Format("%s?ip=%u&t=%d", (LPCTSTR)g_ini[0], ntohl(inet_addr(ipaddr)), nPower);
	CString ProxyIP;
	WORD ProxyPort = 0;

	int result = -1;
	for (INT i = 0, count = 1; i < count; i++)
	{
		result = get_http(TargetIP, ProxyIP, ProxyPort, NULL);
		if (result == 0)
		{
			break;
		}
		//if (g_ini[1] == "1") MessageBox(NULL, TargetIP + "\r\n\r\n通信失敗", "ERROR", MB_ICONERROR | MB_OK);
		Sleep(500);
	}
	if (result != 0)
	{
		return -1;
	}

	return 0;
}

LRESULT CALLBACK CatchPowerBroadcast(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case PBT_APMQUERYSUSPEND:

		PowerPosting(0);
		break;

	case PBT_APMQUERYSUSPENDFAILED:

		PowerPosting(1);
		break;

	case PBT_APMRESUMESUSPEND:

		PowerPosting(1);
		break;

	default:

		break;

	}

	return 0;
}

// ウインドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) 
    {
    case WM_CREATE:

		PowerPosting(1);
        break;

    case WM_DESTROY:

		PowerPosting(0);
        PostQuitMessage(0);
        break;

	case WM_POWERBROADCAST:

		CatchPowerBroadcast(hWnd, message, wParam, lParam);
		break;

	case WM_QUERYENDSESSION:

		PowerPosting(0);
		break;

	case WM_ENDSESSION:

		if (wParam == FALSE)
		{
			PowerPosting(1);
		}
		else
		{
			DestroyWindow(hWnd);
		}
		break;

    case WM_CLOSE:

		DestroyWindow(hWnd);
		break;

    default:

		break;

    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
