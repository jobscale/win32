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

// pPopMaker.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "pPopMaker.h"

const LPCTSTR GetModuleFolder()
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
	}
	return moduleFolder;
}

#define MAX_LOADSTRING 100

struct RESOUCE_DATA
{
	HINSTANCE hInst;
	TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
	TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��
	HWND hWnd;

	HWND hWndEdTimeLimit;
	HWND hWndEdStatus;
	HWND hWndEdColor;
	HWND hWndEdWidth;
	HWND hWndEdHeight;
	HWND hWndEdLink;
	HWND hWndEdText;
	HWND hWndBtTest;
	HWND hWndBtCommit;
	HWND hWndBtUpdate;
	HWND hWndBtEmergencyQuake;
	HWND hWndBtEmergencyFire;
	HWND hWndBtEmergencyTraining;
};

RESOUCE_DATA& GetResouceData()
{
	static RESOUCE_DATA resData = { 0 };
	return resData;
}

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���ł��B
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void RenderingLimits(DWORD& targ, DWORD min, DWORD max)
{
	if (targ < min)
	{
		targ = min;
	}
	else if (targ > max)
	{
		targ = max;
	}
}

HRESULT MakeSendData(CComBSTR& sendData, DWORD isBroadcast = 0)
{
	RESOUCE_DATA& resData = GetResouceData();

	CComBSTR _buf(11, "");
	LPTSTR buf = (LPTSTR)_buf.m_str;

	DWORD sizeBuf = GetWindowTextLength(resData.hWndEdStatus);
	GetWindowText(resData.hWndEdStatus, buf, 11);
	DWORD status = strtoul(buf, NULL, 10);

	sizeBuf = GetWindowTextLength(resData.hWndEdColor);
	GetWindowText(resData.hWndEdColor, buf, 11);
	DWORD color = strtoul(buf, NULL, 10);
	if(color < 0 || color > 8)
	{
		color = 1;
	}
	sizeBuf = GetWindowTextLength(resData.hWndEdWidth);
	GetWindowText(resData.hWndEdWidth, buf, 11);
	DWORD width = strtoul(buf, NULL, 10);
	
	sizeBuf = GetWindowTextLength(resData.hWndEdHeight);
	GetWindowText(resData.hWndEdHeight, buf, 11);
	DWORD height = strtoul(buf, NULL, 10);

	RenderingLimits(width, 150, 700);
	RenderingLimits(height, 100, 500);

	DWORD sizeLink = GetWindowTextLength(resData.hWndEdLink);
	CComBSTR _lpszLink(sizeLink + 1);
	LPTSTR lpszLink = (LPTSTR)_lpszLink.m_str;
	GetWindowText(resData.hWndEdLink, lpszLink, sizeLink + 1);

	DWORD sizeText = GetWindowTextLength(resData.hWndEdText);
	CComBSTR _lpszText(sizeText + 1);
	LPTSTR lpszText = (LPTSTR)_lpszText.m_str;
	GetWindowText(resData.hWndEdText, lpszText, sizeText + 1);

	sizeBuf = GetWindowTextLength(resData.hWndEdTimeLimit);
	GetWindowText(resData.hWndEdTimeLimit, buf, 11);
	DWORD showSeconds = strtoul(buf, NULL, 10);

	INT size = sizeLink + sizeText + 1024;
	CComBSTR _cmd(size, "");
	LPTSTR cmd = (LPTSTR)_cmd.m_str;
	sprintf(cmd, "%s%d:\"%d\" \"%d\" \"%d\" \"%s\" \"%s\" \"%d\"",
		isBroadcast ? "popupmessage:" : "popuptestmessage:", status, color, width, height, lpszLink, lpszText, showSeconds);

	sendData = _cmd;

	return 0;
}

LRESULT WriteCommander(LPCTSTR fName, LPCTSTR pCmdData)
{
	RESOUCE_DATA& resData = GetResouceData();

	CComBSTR _path(_MAX_PATH, "");
	LPTSTR path = (LPTSTR)_path.m_str;
	strcpy(path, GetModuleFolder());
	strcat(path, fName);

	HANDLE hFile = CreateFile(path, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxEx(resData.hWnd, "�K�v�ȃt�@�C���̃I�[�v���Ɏ��s���܂����B", "���f", MB_ICONSTOP, LANG_NEUTRAL);
		return -1;
	}
	else
	{
		DWORD dwWrite = 0;
		WriteFile(hFile, pCmdData, strlen(pCmdData), &dwWrite, NULL);
		CloseHandle(hFile);
	}

	return 0;
}

struct COLOR_USER
{
	COLORREF color[5];
};

static const COLOR_USER colorTable[] = {
	{ RGB(255, 255, 255),  RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255) },
	{ RGB(255, 220, 220),  RGB(255, 190, 190), RGB(255, 160, 160), RGB(255, 130, 130), RGB(255, 100, 100) },
	{ RGB(220, 255, 220),  RGB(190, 255, 190), RGB(160, 255, 160), RGB(130, 255, 130), RGB(100, 255, 100) },
	{ RGB(220, 220, 255),  RGB(190, 190, 255), RGB(160, 160, 255), RGB(130, 130, 255), RGB(100, 100, 255) },
	{ RGB(220, 255, 255),  RGB(190, 255, 255), RGB(160, 255, 255), RGB(130, 255, 255), RGB(100, 255, 255) },
	{ RGB(255, 255, 220),  RGB(255, 255, 190), RGB(255, 255, 160), RGB(255, 255, 130), RGB(255, 255, 100) },
	{ RGB(255, 220, 255),  RGB(255, 190, 255), RGB(255, 160, 255), RGB(255, 130, 255), RGB(255, 100, 255) },
	{ RGB(180, 180, 180),  RGB(180, 180, 180), RGB(180, 180, 180), RGB(180, 180, 180), RGB(180, 180, 180) },
	{ RGB(220, 220, 220),  RGB(220, 220, 220), RGB(220, 220, 220), RGB(220, 220, 220), RGB(220, 220, 220) },
};

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
	RESOUCE_DATA& resData = GetResouceData();

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static DWORD _color = 3;

	switch (message)
	{
	case WM_CTLCOLORMSGBOX   : //           0x0132
	case WM_CTLCOLOREDIT     : //           0x0133
	case WM_CTLCOLORLISTBOX  : //           0x0134
	case WM_CTLCOLORBTN      : //           0x0135
	case WM_CTLCOLORDLG      : //           0x0136
	case WM_CTLCOLORSCROLLBAR: //           0x0137
	case WM_CTLCOLORSTATIC   : //           0x0138
		SetBkMode((HDC)wParam, TRANSPARENT);
		if ((HWND)lParam == resData.hWndEdText)
		{
			DWORD sizeCount = sizeof colorTable / sizeof COLOR_USER;
			if (_color > sizeCount - 1)
			{
				_color = 0;
			}
			HBRUSH hBrush = CreateSolidBrush(colorTable[_color].color[0]);
			return (INT_PTR)hBrush;
		}
		else
		{
			return (INT_PTR)GetStockObject(WHITE_BRUSH);
		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		if (wmEvent == EN_CHANGE)
		{
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wmEvent == EN_CHANGE && (resData.hWndEdWidth == (HWND)lParam || resData.hWndEdHeight == (HWND)lParam))
		{
			CComBSTR _buf(11, "");
			LPTSTR buf = (LPTSTR)_buf.m_str;
			GetWindowText(resData.hWndEdWidth, buf, 10);
			DWORD width = strtoul(buf, NULL, 10);
			GetWindowText(resData.hWndEdHeight, buf, 10);
			DWORD height = strtoul(buf, NULL, 10);
			MoveWindow(resData.hWndEdText, 120, 110, width, height, TRUE);
		}
		else if (wmEvent == EN_CHANGE && resData.hWndEdColor == (HWND)lParam)
		{
			CComBSTR _buf(11, "");
			LPTSTR buf = (LPTSTR)_buf.m_str;
			DWORD sizeBuf = GetWindowTextLength(resData.hWndEdColor);
			GetWindowText(resData.hWndEdColor, buf, 11);

			_color = strtoul(buf, NULL, 10);
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtTest == (HWND)lParam)
		{
			CComBSTR sendData;
			MakeSendData(sendData);

			WriteCommander("pLook.commander", (LPTSTR)sendData.m_str);
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtCommit == (HWND)lParam)
		{
			if (MessageBoxEx(hWnd, "�N�����Ă��邷�ׂĂ̒[���Ƀ|�b�v�A�b�v���b�Z�[�W��\�����܂��B\r\n"
				"�\������郁�b�Z�[�W���m�F���܂������H\r\n"
				"�����𒆒f����ꍇ�̓L�����Z����I��ł��������B", "�x��", MB_OKCANCEL | MB_ICONWARNING, LANG_NEUTRAL) == IDOK)
			{
				CComBSTR sendData;
				MakeSendData(sendData, 1);

				WriteCommander("pLook.commander", (LPTSTR)sendData.m_str);

				// URL�����ɖ߂�
				SetWindowText(resData.hWndEdLink,"http://clip.plustar.jp/");
			}
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtUpdate == (HWND)lParam)
		{
			WriteCommander("pLook.commander", "setupupdatenow:");
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtEmergencyQuake == (HWND)lParam)
		{
			SetWindowText(resData.hWndEdText,
				"�������A�n�k���������܂����B\r\n"
				"���̌����͑ϐk�\���ɂȂ��Ă���܂��̂ň��S�ł��B\r\n\r\n"
				"�Ȃ��|��Ղ����◎���̊댯�̂�����̂�����A\r\n"
				"���̃A�i�E���X������܂ł��΂炭���҂��������B\r\n"
				"�G���x�[�^�[�͍Ŋ��̃t���A�Œ�~���܂��̂�\r\n"
				"�^�s�ĊJ�̘A��������܂ł��҂��������B\r\n\r\n"

				"Attention please!\r\n"
				"An earthquake has just occurred.\r\n"
				"This building is earthquake-proof.\r\n"
				"Please keep cool and look out for anything that has fallen or dropped.\r\n"
				"An announcement will follow.\r\n"
			);

			// �l�F0 �����\��
			SetWindowText(resData.hWndEdTimeLimit, "0");
			// �l�F6(��)
			SetWindowText(resData.hWndEdColor, "6");
			// �l�F400
			SetWindowText(resData.hWndEdWidth, "400");
			// �l�F200
			SetWindowText(resData.hWndEdHeight, "300");
			// �l�F��
			SetWindowText(resData.hWndEdLink,"");
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtEmergencyFire == (HWND)lParam)
		{
			SetWindowText(resData.hWndEdText,
				"�΍Ђ��������܂����B�΍Ђ��������܂����B\r\n\r\n"
				"�A�i�E���X�܂��͌W���̎w���ɏ]���āA\r\n"
				"�����A���K�i\r\n"
				"��������J�n���ĉ������B\r\n"
				"�Ȃ��A�G���x�[�^�[�͉^�s�𒆎~���܂��̂�\r\n"
				"��΂ɂ����ɂȂ�Ȃ��ŉ������B\r\n\r\n"

				"Emergency! Emergency!\r\n"
				"Fire alert! Fire alert!\r\n"
				"Evacuate from here to the exit immediately. \r\n"
				"It is NOT a fire-drill.It is NOT a fire-drill. \r\n"
			);

			// �l�F0 �����\��
			SetWindowText(resData.hWndEdTimeLimit, "0");
			// �l�F6(��)
			SetWindowText(resData.hWndEdColor, "6");
			// �l�F400
			SetWindowText(resData.hWndEdWidth, "400");
			// �l�F200
			SetWindowText(resData.hWndEdHeight, "300");
			// �l�F��
			SetWindowText(resData.hWndEdLink,"");
		}
		else if (wmEvent == BN_CLICKED && resData.hWndBtEmergencyTraining == (HWND)lParam)
		{
			SetWindowText(resData.hWndEdText,
				"����������A�΍Ђ�z�肵�����P�����J�n���܂��B\r\n\r\n"
				"�A�i�E���X���тɌW���̎w���ɏ]���āA\r\n"
				"�����A���K�i\r\n"
				"��������J�n���ĉ������B\r\n"
				"�F�l���̗�ÁA�����ȍs�����т�\r\n"
				"�����͂����肢�\���グ�܂��B\r\n\r\n"

				"Attention please!\r\n"
				"We will begin a fire drill�Cnow.\r\n"
				"An announcement will follow.\r\n"
			);

			// �l�F0 �����\��
			SetWindowText(resData.hWndEdTimeLimit, "0");
			// �l�F6(��)
			SetWindowText(resData.hWndEdColor, "2");
			// �l�F400
			SetWindowText(resData.hWndEdWidth, "400");
			// �l�F200
			SetWindowText(resData.hWndEdHeight, "300");
			// �l�F��
			SetWindowText(resData.hWndEdLink,"");
		}
		
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(resData.hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
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

	case WM_CREATE:
		CreateWindow("STATIC", "�X�e�[�^�X[0-7]:", WS_CHILD | SS_RIGHT, 13, 23, 130, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdStatus = CreateWindow("EDIT", "0", WS_CHILD | WS_BORDER | ES_NUMBER, 150, 20, 60, 25, hWnd, 0, resData.hInst, NULL);
		SendMessage(resData.hWndEdStatus, EM_SETLIMITTEXT, 10, 0);
		// �����l�F20�b�\��
		CreateWindow("STATIC", "�\������[�b��]:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 13, 23, 130, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdTimeLimit = CreateWindow("EDIT", "20", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 150, 20, 60, 25, hWnd, 0, resData.hInst, NULL);
		SendMessage(resData.hWndEdTimeLimit, EM_SETLIMITTEXT, 10, 0);
		// �����l�F3(��)
		CreateWindow("STATIC", "�F[0-8]:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 223, 23, 130, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdColor = CreateWindow("EDIT", "3", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 360, 20, 60, 25, hWnd, 0, resData.hInst, NULL);
		SendMessage(resData.hWndEdColor, EM_SETLIMITTEXT, 10, 0);
		// �����l�F300
		CreateWindow("STATIC", "��[150-700]:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 13, 53, 130, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdWidth = CreateWindow("EDIT", "300", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 150, 50, 60, 25, hWnd, 0, resData.hInst, NULL);
		SendMessage(resData.hWndEdWidth, EM_SETLIMITTEXT, 10, 0);
		// �����l�F250
		CreateWindow("STATIC", "����[100-500]:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 223, 53, 130, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdHeight = CreateWindow("EDIT", "250", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 360, 50, 60, 25, hWnd, 0, resData.hInst, NULL);
		SendMessage(resData.hWndEdHeight, EM_SETLIMITTEXT, 10, 0);

		CreateWindow("STATIC", "�����N:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 13, 83, 100, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdLink = CreateWindow("EDIT", "http://clip.plustar.jp/", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 120, 80, 400, 25, hWnd, 0, resData.hInst, NULL);
		CreateWindow("STATIC", "�e�L�X�g:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 13, 113, 100, 20, hWnd, 0, resData.hInst, NULL);
		resData.hWndEdText = CreateWindow("EDIT", "�l�b�g���[�N��Q�ɂ�����̕�\r\n"
			"�p�\�R���̕s���g�p�ɂ�����̕�\r\n\r\n"
			"�v���X�^�[�������ł��邩���m��܂���\r\n"
			"�������N���b�N����\r\n"
			"�v���X�^�[���L���Ă݂Ă�������\r\n\r\n"
			"http://www.plustar.jp/", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_MULTILINE, 120, 110, 300, 300, hWnd, 0, resData.hInst, NULL);

		resData.hWndBtTest = CreateWindow("BUTTON", "�e�X�g", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 140, 100, 25, hWnd, 0, resData.hInst, NULL);
		resData.hWndBtCommit = CreateWindow("BUTTON", "���M", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 170, 100, 25, hWnd, 0, resData.hInst, NULL);
#ifdef _DEBUG
		resData.hWndBtUpdate = CreateWindow("BUTTON", "�����X�V", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 200, 100, 25, hWnd, 0, resData.hInst, NULL);
#endif
		resData.hWndBtEmergencyFire     = CreateWindow("BUTTON", "�΍Д���", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 230, 100, 25, hWnd, 0, resData.hInst, NULL);
		resData.hWndBtEmergencyQuake    = CreateWindow("BUTTON", "�n�k����", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 260, 100, 25, hWnd, 0, resData.hInst, NULL);
		resData.hWndBtEmergencyTraining = CreateWindow("BUTTON", "�P��"    , WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER, 13, 290, 100, 25, hWnd, 0, resData.hInst, NULL);

		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
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
	RESOUCE_DATA& resData = GetResouceData();

	WNDCLASSEX wcex = { sizeof WNDCLASSEX };

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PPOPMAKER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PPOPMAKER);
	wcex.lpszClassName	= resData.szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PPOPMAKER));

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
	RESOUCE_DATA& resData = GetResouceData();

   resData.hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

   resData.hWnd = CreateWindow(resData.szWindowClass, resData.szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!resData.hWnd)
   {
      return FALSE;
   }

   ShowWindow(resData.hWnd, nCmdShow);
   UpdateWindow(resData.hWnd);

   return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	RESOUCE_DATA& resData = GetResouceData();

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, resData.szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PPOPMAKER, resData.szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PPOPMAKER));

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
