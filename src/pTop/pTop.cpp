///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
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

// pTop.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "pTop.h"

#include "../oCommon/oBase64.h"

#include <Winuser.h>
#include <shlobj.h>

#define MAX_LOADSTRING 100

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
bool DoesFileExist( LPCTSTR lpFileName );

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PTOP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance (hInstance, SW_HIDE))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PTOP));

	// ���C�� ���b�Z�[�W ���[�v:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
//  �R�����g:
//
//    ���̊֐�����юg�����́A'RegisterClassEx' �֐����ǉ����ꂽ
//    Windows 95 ���O�� Win32 �V�X�e���ƌ݊�������ꍇ�ɂ̂ݕK�v�ł��B
//    �A�v���P�[�V�������A�֘A�t����ꂽ
//    �������`���̏������A�C�R�����擾�ł���悤�ɂ���ɂ́A
//    ���̊֐����Ăяo���Ă��������B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_CLASSDC;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PTOP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; // MAKEINTRESOURCE(IDC_PTOP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PTOP));

	return RegisterClassEx(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

	hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

	hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 128, LWA_ALPHA);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

#pragma warning(disable:4996)

const LPCTSTR GetModuleFolder(DWORD flg = 0)
{
	static CComBSTR _moduleFolder(_MAX_PATH, "");
	static LPTSTR moduleFolder = (LPTSTR)_moduleFolder.m_str;
	if (strlen(moduleFolder) == 0)
	{
		CComBSTR _drive(_MAX_DRIVE, "");
		LPTSTR drive = (LPTSTR)_drive.m_str;
		CComBSTR _dir(_MAX_DIR, "");
		LPTSTR dir = (LPTSTR)_dir.m_str;
		CComBSTR _fname(_MAX_FNAME, "");
		LPTSTR fname = (LPTSTR)_fname.m_str;
		CComBSTR _ext(_MAX_EXT, "");
		LPTSTR ext = (LPTSTR)_ext.m_str;

		GetModuleFileName(NULL, moduleFolder, _MAX_PATH);
		_splitpath_s(moduleFolder, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
		strcpy(moduleFolder, drive);
		strcat(moduleFolder, dir);
		if (flg & 1)
		{
			strcat(moduleFolder, fname);
		}
	}
	return moduleFolder;
}

// ��� �n���h���ł��B
INT_PTR CALLBACK Monitor(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
			SetWindowLong(hDlg, GWL_STYLE, 0);

			SetLayeredWindowAttributes(hDlg, RGB(0, 0, 0), 172, LWA_ALPHA);

			int cx = GetSystemMetrics(SM_CXSCREEN);
			int cy = GetSystemMetrics(SM_CYSCREEN);
			MoveWindow(hDlg, 0, 0, cx, cy, FALSE);

			CString store_name   = "�v���X�^�[�E�f���X";
			CString company_name = "�i���j�v���X�^�[";

			// �t�@�C������X�ܖ��A��Ж���E�o
			CString path = GetModuleFolder(1);
			path += ".evs"; // Encription Verify System

			CString readData;

			HANDLE hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				CString buffer;
				DWORD readied = 0;

				DWORD dwReadHigh = 0;
				DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
				for (; ReadFile(hFile, buffer.GetBuffer(dwRead), dwRead, &readied, NULL) && readied; )
				{
					buffer.GetBufferSetLength(readied);
					readData += buffer;
				}

				CloseHandle(hFile);
			}

			if (!readData.IsEmpty())
			{
				CString buffer;
				base64_decode(readData, buffer, 1);
				INT left = buffer.Find("\r\n");
				if (left != -1)
				{
					INT right = buffer.GetLength() - left - strlen("\r\n");
					store_name = buffer.Left(left);
					company_name = buffer.Right(right);
				}
			}

			// Static 1/6-5/6 Align 1/10-5/6 Center
			SetWindowPos(GetDlgItem(hDlg, IDC_TEXT), 0, cx / 6, cy / 12, cx * 4 / 6, cy * 9 / 12, SWP_NOZORDER);
			SetWindowText(GetDlgItem(hDlg, IDC_TEXT), 
				"�[���g�p����\r\n" 
				"\r\n" 
				+ store_name + "�ł̓C���^�[�l�b�g�A�p�\�R���A�v�����^�A���̑��@�킪����ɓ��삷�邱�Ƃ̕ۏ�͂��������˂܂��B"
				"��Q�̕����A�s��̉����ɂ͑S�͊��őP��s�����܂��B�������Ƃ����͂��肢�\���グ�܂��B \r\n" 
				"\r\n" 
				+ store_name + "�̃p�\�R���͌l���ی�̊ϓ_����V���b�g�_�E���E�ċN���ł����p�O�̏�ԂɃp�\�R���𕜌����܂��B" 
				"�ۑ������f�[�^����������܂��̂ł��q�l�̐ӔC�ő�؂ȃf�[�^�͊O�����f�B�A�ɕۑ����Ă��������B\r\n" 
				"�l���̎�舵���͂ɏ\���ɒ��ӂ��Ă��������B\r\n" 
				"\r\n" 
				+ store_name + "�ł͑��̂��q�l�ɂ����f��������s�ׂ��������ƃX�^�b�t�����f�����ꍇ�A�����ɂ����p�𒆎~�����Ă��������ꍇ���������܂��B \r\n" 
				"\r\n" 
				+ store_name + "�ł̓C���^�[�l�b�g�ƍ߂̖o�łɓw�߂Ă���܂��B" 
				"���ׂ̈ɁA�@�߂Ɋ�Â��[���g�p�󋵂̊J���v��������΁A���L�����J���������܂��B\r\n" 
				"�@�E�X���J�����̘^��f��\r\n" 
				"�@�E���X����\r\n" 
				"�@�E�[�����p����\r\n" 
				"\r\n" 
				"�ȏ�������ӂ̏�A�[���������p�����܂��悤���肢�\���グ�܂��B\r\n" 
				+ company_name + " " + store_name + " �X��"
			);

			// Buttom 5/6 Align Center
			int buttonWidth = cx / 8;
			SetWindowPos(GetDlgItem(hDlg, IDC_AGREE)   , 0, (cx / 2) - buttonWidth - 30, cy * 5 / 6 + 20, buttonWidth, 40, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hDlg, IDC_NOTAGREE), 0, (cx / 2) + 30, cy * 5 / 6 + 20, buttonWidth, 40, SWP_NOZORDER);
		}
		return (INT_PTR)TRUE;

	case WM_CTLCOLORMSGBOX   : //           0x0132
	case WM_CTLCOLOREDIT     : //           0x0133
	case WM_CTLCOLORLISTBOX  : //           0x0134
	case WM_CTLCOLORBTN      : //           0x0135
	case WM_CTLCOLORDLG      : //           0x0136
	case WM_CTLCOLORSCROLLBAR: //           0x0137
	case WM_CTLCOLORSTATIC   : //           0x0138
		SetBkColor((HDC)wParam, RGB(0, 0, 0));
		SetBkMode((HDC)wParam, TRANSPARENT);
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (INT_PTR)GetStockObject(BLACK_BRUSH);

	case WM_MOVE:
		{
			RECT rect = { 0 };
			GetWindowRect(hDlg, &rect);
			if (rect.left != 0 || rect.top != 0)
			{
				SetWindowPos(hDlg, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
		}
		return 1;

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			{
				if (LOWORD(wParam) == IDC_AGREE)
				{
					EndDialog(hDlg, LOWORD(wParam));
					PostQuitMessage(0);

					// ����Ă݂�
					return (INT_PTR)TRUE;
				}
				else if (LOWORD(wParam) == IDC_NOTAGREE)
				{
					if (MessageBox(hDlg, "�[���g�p�����ɓ��ӂ��������Ȃ��ꍇ�͒[���̂����p�͂ł��܂���B"
							"\r\n\r\n�V�X�e�����V���b�g�_�E�����܂��B\r\n��낵���ł����H",
							"�x��", MB_YESNO|MB_ICONQUESTION) == IDYES)
					{
						HANDLE hToken; 
						TOKEN_PRIVILEGES tkp;

						// Get a token for this process.
						OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES |TOKEN_QUERY, &hToken);
						// Get the LUID for the shutdown privilege.
						LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
						tkp.PrivilegeCount = 1;  // oneprivilege to set    
						tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
						// Get the shutdown privilege for this process.
						AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 

						UINT uFlags = EWX_SHUTDOWN | EWX_POWEROFF| EWX_FORCE;

						// shutdown
						ExitWindowsEx(uFlags, 0);
						return (INT_PTR)TRUE;
					}
					return (INT_PTR)TRUE;
				}
			}
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	INT_PTR resDlg = 0;

	switch (message)
	{
	case WM_CREATE:
		resDlg = DialogBox(hInst, MAKEINTRESOURCE(IDD_MONITOR), hWnd, Monitor);
		if (resDlg == IDC_AGREE)
		{
			PostQuitMessage(0);
			return 0;
		}
		else if (resDlg == IDC_NOTAGREE)
		{
			*LPDWORD(0) = 0;
			// 
		}
		break;

	case WM_SYSKEYDOWN:
		return 1;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/*
 * �t�@�C���������true��Ԃ�
 */
bool DoesFileExist( LPCTSTR lpFileName )
{
	bool bRet = false;
	WIN32_FIND_DATA	FindData;

	HANDLE hFile = FindFirstFile( lpFileName, &FindData );
	if ( hFile != INVALID_HANDLE_VALUE )
		bRet = true;
	FindClose( hFile );
	return bRet;
}
