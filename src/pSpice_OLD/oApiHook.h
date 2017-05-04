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

#ifndef __O_APIHOOK_H__
#define __O_APIHOOK_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// API�t�b�N�����s���郁�C�����[�`��

class CoAPIHook 
{
public:

	// �R���X�g���N�^
	CoAPIHook(PSTR, PSTR, PROC);

	// �f�X�g���N�^
	~CoAPIHook();

	// �I���W�i���A�h���X�擾�p
	operator PROC() { return m_pfnOrig; }

protected:

	static CoAPIHook *sm_pHead;  // �m�[�h�̃g�b�v�̃f�[�^
	CoAPIHook *m_pNext;          // ���̃m�[�h�̃f�[�^

	// �ۑ��̈�
	HMODULE m_hmodOrig; // �֐��������W���[���̃n���h��
	PCSTR m_pszModuleName; // �֐��������W���[�����iANSI�j
	PCSTR m_pszFuncName; // �֐����iANSI�j
	PROC m_pfnOrig; // �I���W�i���֐��̃A�h���X
	PROC m_pfnHook; // �u�������֐��̃A�h���X

public:

	// �T�[�r�X�֐�
	static LPCTSTR GetCurrentModuleFileName(); // ���W���[�����̎擾

protected:

	// �����֐�
	virtual BOOL IsIgnoreModule() const;

	// API�t�b�N��K�������郁�C���֐�
	static void WINAPI ReplaceIATEntryInAllMods(PCSTR, const PROC, const PROC);
	static void WINAPI ReplaceIATEntryInOneMod(PCSTR, const PROC, const PROC, const HMODULE);

	// �V���Ƀ��[�h���ꂽ���W���[����API�t�b�N��Ή�������֐�
	static void WINAPI FixupNewlyLoadedModule(HMODULE, DWORD);

	// ���ꂼ���API�ɒu�������t�b�N�֐�
	static HMODULE WINAPI Hook_Kernel32_LoadLibraryA(PCSTR);
	static HMODULE WINAPI Hook_Kernel32_LoadLibraryW(PCWSTR);
	static HMODULE WINAPI Hook_Kernel32_LoadLibraryExA(PCSTR, HANDLE, DWORD);
	static HMODULE WINAPI Hook_Kernel32_LoadLibraryExW(PCWSTR, HANDLE, DWORD);
	static FARPROC WINAPI Hook_Kernel32_GetProcAddress(HMODULE, PCSTR);

	// ���ꂼ���API���t�b�N���邱�Ƃ�錾
	static CoAPIHook sm_Kernel32_LoadLibraryA;
	static CoAPIHook sm_Kernel32_LoadLibraryW;
	static CoAPIHook sm_Kernel32_LoadLibraryExA;
	static CoAPIHook sm_Kernel32_LoadLibraryExW;
	static CoAPIHook sm_Kernel32_GetProcAddress;

};

#endif // __O_APIHOOK_H__
