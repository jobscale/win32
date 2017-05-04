///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

// pExTrapper.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#include <atlbase.h>

#include "pExTrapper.h"
#include "BaseService.h"

#include "../oCommon/oTools.h"

#define _OFC_EXPORT_
#include "../ofc/oFoundationClassLibrary.h"
#include "../ofc/oString.h"

#include "../pLook/pUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CpExTrapperApp

BEGIN_MESSAGE_MAP(CpExTrapperApp, CWinApp)
END_MESSAGE_MAP()


// CpExTrapperApp �R���X�g���N�V����

CpExTrapperApp::CpExTrapperApp()
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, "\0 pExTrapperMutex" + 2);
	if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		DEBUG_OUT_DEFAULT("duplicate process failed.");
		TerminateProcess(GetCurrentProcess(), 0);
	}
}

// �B��� CpExTrapperApp �I�u�W�F�N�g�ł��B

CpExTrapperApp theApp;

DWORD SetupFireWall(BOOL bInst)
{
	DWORD nRet = 0;

	TCHAR pModuleName[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, pModuleName, _MAX_PATH);

	HKEY hKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "\0 SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile\\AuthorizedApplications\\List" + 2, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		if (bInst)
		{
			TCHAR value[_MAX_PATH + 100] = { 0 };
			strcat_s(value, pModuleName);
			strcat_s(value, ":*:Enabled:PLUSTAR_GK");

			if (RegSetValueEx(hKey, pModuleName, NULL, REG_SZ, (LPBYTE)(LPCTSTR)value, (DWORD)strlen(value)) != ERROR_SUCCESS)
			{
				OutputViewer("ERROR InstSender RegSetValueEx Firewall failed.");
				nRet = -1;
			}
		}
		else
		{
			if (RegDeleteValue(hKey, pModuleName) != ERROR_SUCCESS)
			{
				OutputViewer("ERROR InstSender RegDeleteKey Firewall failed.");
				nRet = -1;
			}
		}
		RegCloseKey(hKey);
	}
	else
	{
		OutputViewer("ERROR InstSender RegOpenKeyEx Firewall failed.");
		nRet = -1;
	}

	return nRet;
}

INT SetupTools(LPCTSTR lpszName, LPCTSTR lpszTarget, BOOL bInst = TRUE)
{
	INT nRet = 0;

	CHAR pValue[_MAX_PATH + 2] = { 0 };
	strcat_s(pValue, _MAX_PATH + 2, "\"");
	strcat_s(pValue, _MAX_PATH + 2, lpszTarget);
	strcat_s(pValue, _MAX_PATH + 2, "\"");

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "\0 SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run" + 2, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		if (bInst)
		{
			if (RegSetValueEx(hKey, lpszName, NULL, REG_SZ, (LPBYTE)pValue, (DWORD)strlen(pValue)) != ERROR_SUCCESS)
			{
				OutputViewer("ERROR InstSender RegSetValueEx failed.");
				nRet = -1;
			}
		}
		else
		{
			if (RegDeleteValue(hKey, lpszName) != ERROR_SUCCESS)
			{
				OutputViewer("ERROR InstSender RegDeleteKey failed.");
				nRet = -1;
			}
		}
		RegCloseKey(hKey);
	}
	else
	{
		OutputViewer("ERROR InstSender RegOpenKeyEx failed.");
		nRet = -1;
	}

	return nRet;
}

// CpExTrapperApp ������

BOOL CpExTrapperApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof InitCtrls;
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	// SetRegistryKey(_T("�A�v���P�[�V���� �E�B�U�[�h�Ő������ꂽ���[�J�� �A�v���P�[�V����"));

	CBaseService _service("Plustar eXtreme tRapper");

	LPCTSTR pCmdLine = GetCommandLine();

	if (strstr(pCmdLine, "uninstall") != 0)
	{
		SetupFireWall(FALSE);

		CoString name = "pTools.exe";
		CoString path = GetModuleFolder() + name;
		SetupTools(name, path, FALSE);

		//name = "pPower.exe";
		//path = GetModuleFolder() + name;
		//SetupTools(name, path, FALSE);

		_service.Uninstall();

		// pTools �������I��
		HWND hWnd = FindWindow("\0 plus32 wndClass pTools" + 2, "\0 goode's checker pTools" + 2);
		if (hWnd)
		{
			DWORD pid = 0;
			GetWindowThreadProcessId(hWnd, &pid);
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
			if (hProcess)
			{
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}
		}

		// pTools ���V���b�g�_�E��
		if (IsWindow(hWnd))
		{
			PostMessage(hWnd, WM_USER + 202, *LPDWORD("PTLS"), *LPDWORD("KILL"));
		}

		TerminateProcess(GetCurrentProcess(), 0);
	}
	else if (strstr(pCmdLine, "install") != 0)
	{
		_asm
		{
			int 3
		}
		SetupFireWall(TRUE);

		CoString name = "pTools.exe";
		CoString path = GetModuleFolder() + name;
		SetupTools(name, path, TRUE);

		//name = "pPower.exe";
		//path = GetModuleFolder() + name;
		//SetupTools(name, path, TRUE);

		_service.Install();

		TerminateProcess(GetCurrentProcess(), 0);
	}

	HMODULE hModule = LoadLibrary("pLook");

	if (IsSuperDebug())
	{
		if (_service.OnInit())
		{
			_service.StartRun(); // nothing more to do
		}
	}
    else if (1 || _service.IsInstalled())
	{
		_service.StartService();
    }

	TerminateProcess(GetCurrentProcess(), 0);

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}
