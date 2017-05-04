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

#define WIN32_LEAN_AND_MEAN		// Windows �w�b�_�[����w�ǎg�p����Ȃ��X�^�b�t�����O���܂�
#define _WIN32_WINNT 0x0500
#if !defined(WINVER) || (WINVER < 0x0500)
#undef WINVER
#define WINVER 0x0500
#endif

#include <afxwin.h>

#include <tlhelp32.h>
#include <imagehlp.h>
#pragma comment(lib, "imagehlp")

#define _OFC_EXPORT_
#include "../ofc/oString.h"

#include "oApiHook.h"

// �ŏ���DLL���}�b�s���O�����v���Z�X�̏��
#pragma comment(linker, "/Section:Shared,rws")
#pragma data_seg("Shared")
namespace hook
{
	DWORD firstProcessId = 0;
}
#pragma data_seg()

// �֎~����Ă���v���Z�X
BOOL TerminateSuppressionProcess()
{
	int IsSuppressionRun(LPCTSTR lpCommandLine);
	BOOL IsRequiredRun(LPCTSTR lpCommandLine);

	if (!IsSuppressionRun(CoAPIHook::GetCurrentModuleFileName()))
	{
		TerminateProcess(GetCurrentProcess(), -1);
		MessageBox(NULL, "�w�肵���v���Z�X�̎��s�͋֎~����Ă��܂��B", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	if (!IsRequiredRun(CoAPIHook::GetCurrentModuleFileName()))
	{
		TerminateProcess(GetCurrentProcess(), -1);
		MessageBox(NULL, "�����������ł͂���܂���B\r\n�w�肵���v���Z�X���N�����邱�Ƃ��ł��܂���B", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	return TRUE;
}

// �m�[�h�̃g�b�v��������
CoAPIHook* CoAPIHook::sm_pHead = NULL;

// �R���X�g���N�^
CoAPIHook::CoAPIHook(PSTR pszModuleName, PSTR pszFuncName, PROC pfnHook)
	: m_pNext(NULL)
	, m_hmodOrig(NULL)
	, m_pszModuleName(NULL)
	, m_pszFuncName(NULL)
	, m_pfnOrig(NULL)
	, m_pfnHook(NULL)
{
	if (IsIgnoreModule())
	{
		return;
	}

	// ���b�v���W���[���͏������Ȃ�
	if (CoString(pszModuleName).CompareNoCase("\0 Ws2_32.dll" + 2) == 0)
	{
		if (GetModuleHandleA("wsock32.dll"))
		{
			return;
		}
		if (GetModuleHandleA("mswsock.dll"))
		{
			return;
		}
	}

	// �֐��������W���[���̃n���h�����擾
	HMODULE hmodOrig = GetModuleHandleA(pszModuleName);
	if (!hmodOrig)
	{
		return;
	}

	m_pNext = sm_pHead; // ���̃m�[�h�̃A�h���X����
	sm_pHead = this; // ���̃m�[�h�̃A�h���X����

	// �I���W�i���֐��̃A�h���X���擾
	PROC pfnOrig = GetProcAddress(hmodOrig, pszFuncName);

	// �t�b�N�Ɋւ���f�[�^��ۑ�
	m_hmodOrig = hmodOrig;
	m_pszModuleName = pszModuleName;
	m_pszFuncName = pszFuncName;
	m_pfnOrig = pfnOrig;
	m_pfnHook = pfnHook;

	// �v���Z�XID��0�Ȃ�DLL���}�b�s���O����ŏ��̃v���Z�X�Ɣ��f
	// ���̃v���Z�XID�����L�������ɕۑ�
	if (hook::firstProcessId == 0)
	{
		hook::firstProcessId = GetCurrentProcessId();
	}

	// �N�����Ɠ����v���Z�X�Ȃ�t�b�N���s��Ȃ�
	if (hook::firstProcessId != GetCurrentProcessId())
	{
		ReplaceIATEntryInAllMods(m_pszModuleName, m_pfnOrig, m_pfnHook);
	}

	// �֎~����Ă���v���Z�X
	if (!TerminateSuppressionProcess())
	{
		ExitProcess(-1);
	}
}

// �f�X�g���N�^
CoAPIHook::~CoAPIHook() 
{
	if (IsIgnoreModule())
	{
		return;
	}

	if (!m_pszModuleName || !m_pfnHook || !m_pfnOrig)
	{
		return;
	}

	// �N�����Ɠ����v���Z�X�̓t�b�N���Ȃ�
	if (hook::firstProcessId != GetCurrentProcessId())
	{
		ReplaceIATEntryInAllMods(m_pszModuleName, m_pfnHook, m_pfnOrig);
	}

	// �m�[�h�̃g�b�v���擾
	CoAPIHook *p = sm_pHead;

	// �m�[�h�̃g�b�v�������Ȃ�΁A���̃m�[�h���g�b�v�ɐ����ďI��
	if (p == this)
	{
		sm_pHead = p->m_pNext;
		return;
	}
	// ���������ł͂Ȃ��Ȃ�΁A�m�[�h�̒����猟������
	// ������A������O��
	while (p->m_pNext)
	{
		if (p->m_pNext == this)
		{
			p->m_pNext = p->m_pNext->m_pNext;
			break;
		}
		p = p->m_pNext;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// �����A�v���ȂǊĎ��̕K�v���Ȃ����̂�r��

BOOL CoAPIHook::IsIgnoreModule() const
{
	static CoStringArray ignore_list;
	if (ignore_list.GetSize() == 0)
	{
		ignore_list.Add("\\Program Files\\.*avast.exe");
		ignore_list.Add("\\Program Files\\.*devenv.exe");
		ignore_list.Add("\\Program Files\\.*MSDEV.EXE");
		ignore_list.Add("\\Program Files\\.*depends.exe");
	}

	return IsFindArray(GetCurrentModuleFileName(), ignore_list);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// �Ăяo�����̃��W���[����

/*static */LPCTSTR CoAPIHook::GetCurrentModuleFileName()
{
	// �v���Z�X�̃t�@�C�������擾
	static CoString processPath;
	if (processPath.IsEmpty())
	{
		GetModuleFileNameA(NULL, processPath.GetBuffer(1024), 1024);
		processPath.ReleaseBuffer();
	}
	return processPath;
}

// ���ׂẴ��W���[���ɑ΂���API�t�b�N���s���֐�
void CoAPIHook::ReplaceIATEntryInAllMods(PCSTR pszModuleName, const PROC pfnCurrent, const PROC pfnNew)
{
	// �������g�ipSpice.dll�j�̃��W���[���n���h�����擾
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery(ReplaceIATEntryInAllMods, &mbi, sizeof mbi) == 0)
	{
		return;
	}
	HMODULE hModThisMod = (HMODULE) mbi.AllocationBase;

	// ���W���[�����X�g���擾
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return;
	}

	MODULEENTRY32 me = { sizeof me };
	BOOL bModuleResult = Module32First(hModuleSnap, &me);
	// ���ꂼ��̃��W���[���ɑ΂���ReplaceIATEntryInOneMod�����s
	// �������������g�ipSpice.dll�j�ɂ͍s��Ȃ�
	while (bModuleResult)
	{
		if (me.hModule != hModThisMod)
		{
			ReplaceIATEntryInOneMod(pszModuleName, pfnCurrent, pfnNew, me.hModule);
		}
		bModuleResult = Module32Next(hModuleSnap, &me);
	}
	CloseHandle(hModuleSnap);
}

// �ЂƂ̃��W���[���ɑ΂���API�t�b�N���s���֐�
void CoAPIHook::ReplaceIATEntryInOneMod(PCSTR pszModuleName, const PROC pfnCurrent, const PROC pfnNew, const HMODULE hmodCaller)
{
	CoString moduleName = pszModuleName;

	// ���W���[���̃C���|�[�g�Z�N�V�����̃A�h���X���擾
	ULONG ulSize;
	PIMAGE_IMPORT_DESCRIPTOR pFirstImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

	// �C���|�[�g�Z�N�V�����������Ă��Ȃ�
	if (pFirstImportDesc == NULL)
	{
		return;
	}

	// �C���|�[�g�f�B�X�N���v�^������
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = NULL;
	for (INT i = 0, count = 2; i < count; i++)
	{
		pImportDesc = pFirstImportDesc;
		while (pImportDesc && pImportDesc->Name)
		{
			PSTR pszModName = (PSTR)((PBYTE)hmodCaller + pImportDesc->Name);
			if (lstrcmpiA(pszModName, moduleName) == 0)
			{
				break;
			}
			pImportDesc++;
		}
		if (pImportDesc && pImportDesc->Name)
		{
			break;
		}

		if (moduleName.Right(4).CompareNoCase(".dll") == 0)
		{
			break;
		}
		moduleName += ".dll";
	}

	// ���̃��W���[���͌Ăяo���悩��֐����C���|�[�g���Ă��Ȃ�
	if (!pImportDesc || !pImportDesc->Name)
	{
		return;
	}

	// �C���|�[�g�A�h���X�e�[�u�����擾
	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)hmodCaller + pImportDesc->FirstThunk);

	// �V�����֐��A�h���X�ɒu��������
	while (pThunk && pThunk->u1.Function)
	{
		// �֐��A�h���X�̃A�h���X���擾
		PROC& pfnThunk = *(PROC*)&pThunk->u1.Function;

		// �Y���֐��ł���Ȃ�Δ����I
		if (pfnThunk == pfnCurrent)
		{
			// �A�h���X����v�����̂ŁA�C���|�[�g�Z�N�V�����̃A�h���X������������
			DWORD dwDummy;
			if (VirtualProtect(&pfnThunk, sizeof pfnThunk, PAGE_EXECUTE_READWRITE, &dwDummy) == 0)
			{
				return;
			}
			WriteProcessMemory(GetCurrentProcess(), &pfnThunk, &pfnNew, sizeof pfnNew, NULL);
			return;
		}
		pThunk++;
	}
	// �����ɏ������ڂ����ꍇ�A�C���|�[�g�Z�N�V�����ɊY���֐����Ȃ��������ƂɂȂ�
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// �f�t�H���g�Ńt�b�N����API�̏����iCoAPIHook�N���X�����ŊǗ�����j

// �f�t�H���g�Ńt�b�N����֐��̃v���g�^�C�v���`
typedef HMODULE (WINAPI *PFN_Kernel32_LoadLibraryA)(PCSTR);
typedef HMODULE (WINAPI *PFN_Kernel32_LoadLibraryW)(PCWSTR);
typedef HMODULE (WINAPI *PFN_Kernel32_LoadLibraryExA)(PCSTR, HANDLE, DWORD);
typedef HMODULE (WINAPI *PFN_Kernel32_LoadLibraryExW)(PCWSTR, HANDLE, DWORD);
typedef FARPROC (WINAPI *PFN_Kernel32_GetProcAddress)(HMODULE, PCSTR);

// ���W���[��������API�����炩���߃t�b�N���Ă���
CoAPIHook CoAPIHook::sm_Kernel32_LoadLibraryA("Kernel32.dll", "LoadLibraryA", (PROC)CoAPIHook::Hook_Kernel32_LoadLibraryA);
CoAPIHook CoAPIHook::sm_Kernel32_LoadLibraryW("Kernel32.dll", "LoadLibraryW", (PROC)CoAPIHook::Hook_Kernel32_LoadLibraryW);
CoAPIHook CoAPIHook::sm_Kernel32_LoadLibraryExA("Kernel32.dll", "LoadLibraryExA", (PROC)CoAPIHook::Hook_Kernel32_LoadLibraryExA);
CoAPIHook CoAPIHook::sm_Kernel32_LoadLibraryExW("Kernel32.dll", "LoadLibraryExW", (PROC)CoAPIHook::Hook_Kernel32_LoadLibraryExW);
CoAPIHook CoAPIHook::sm_Kernel32_GetProcAddress("Kernel32.dll", "GetProcAddress", (PROC)CoAPIHook::Hook_Kernel32_GetProcAddress);

// �V���Ƀ��[�h���ꂽ�֐����t�b�N����֐�
void CoAPIHook::FixupNewlyLoadedModule(HMODULE hMod, DWORD dwFlags)
{
	if ((hMod != NULL) && ((dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0))
	{
		for (CoAPIHook *p = sm_pHead; p != NULL; p = p->m_pNext)
		{
			ReplaceIATEntryInOneMod(p->m_pszModuleName, p->m_pfnOrig, p->m_pfnHook, hMod);
		}
	}
}

// �u���������LoadLibraryA�֐�
HMODULE WINAPI CoAPIHook::Hook_Kernel32_LoadLibraryA(PCSTR pszModulePath) 
{
	HMODULE hMod = ((PFN_Kernel32_LoadLibraryA)(PROC)CoAPIHook::sm_Kernel32_LoadLibraryA)(pszModulePath);
	FixupNewlyLoadedModule(hMod, 0);
	return hMod;
}

// �u���������LoadLibraryW�֐�
HMODULE WINAPI CoAPIHook::Hook_Kernel32_LoadLibraryW(PCWSTR pszModulePath) 
{
	HMODULE hMod = ((PFN_Kernel32_LoadLibraryW)(PROC)CoAPIHook::sm_Kernel32_LoadLibraryW)(pszModulePath);
	FixupNewlyLoadedModule(hMod, 0);
	return hMod;
}

// �u���������LoadLibraryExA�֐�
HMODULE WINAPI CoAPIHook::Hook_Kernel32_LoadLibraryExA(PCSTR pszModulePath, HANDLE hFile, DWORD dwFlags)
{
	HMODULE hMod = ((PFN_Kernel32_LoadLibraryExA)(PROC)CoAPIHook::sm_Kernel32_LoadLibraryExA)(pszModulePath, hFile, dwFlags);
	FixupNewlyLoadedModule(hMod, dwFlags);
	return hMod;
}

// �u���������LoadLibraryExW�֐�
HMODULE WINAPI CoAPIHook::Hook_Kernel32_LoadLibraryExW(PCWSTR pszModulePath, HANDLE hFile, DWORD dwFlags)
{
	HMODULE hMod = ((PFN_Kernel32_LoadLibraryExW)(PROC)CoAPIHook::sm_Kernel32_LoadLibraryExW)(pszModulePath, hFile, dwFlags);
	FixupNewlyLoadedModule(hMod, dwFlags);
	return hMod;
}

// �u���������GetProcAddress�֐�
FARPROC WINAPI CoAPIHook::Hook_Kernel32_GetProcAddress(HMODULE hMod, PCSTR pszProcName)
{
	// �{���̊֐��A�h���X���擾
	FARPROC pfn = ((PFN_Kernel32_GetProcAddress)(PROC)CoAPIHook::sm_Kernel32_GetProcAddress)(hMod, pszProcName);

	// �����t�b�N���ׂ��֐��ł������Ȃ�΁A�u����������֐��̃A�h���X��n��
	for (CoAPIHook *p = sm_pHead; (pfn != NULL) && (p != NULL); p = p->m_pNext)
	{
		// ���ꃂ�W���[���œ���A�h���X
		if (hMod == p->m_hmodOrig && pfn == p->m_pfnOrig)
		{
			return p->m_pfnHook;
		}
	}
   return pfn;
}
