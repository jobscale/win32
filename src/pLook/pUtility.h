///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__OUTILITY_H__)
#define __OUTILITY_H__

#include <wininet.h>
#include <afxdisp.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// ��`

///////////////////////////////////////////////////////////////////////////////////////////////////
// ���L�ϐ�

extern const CHAR* RETERCODE;
extern const CHAR* DELIMITER;

///////////////////////////////////////////////////////////////////////////////////////////////////
// �f�B���N�g�������֐�

INT PleaseTradeName(CoString& localpath, LPCTSTR type, LPCTSTR ext);

///////////////////////////////////////////////////////////////////////////////////////////////////
// �����񏈗��֐�

INT SJIStoUTF8(const CoString& sjis, CoString& utf8);
INT UTF8toSJIS(const CoString& utf8, CoString& sjis);
INT SJIStoEUC(const CComBSTR& sjis, CComBSTR& euc);
HRESULT DateFromString(SYSTEMTIME* sysTime, LPCTSTR sysTimeText, size_t len = 0);
HRESULT DateFromString(COleDateTime& sysTime, LPCTSTR sysTimeText, size_t len = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
// ���W�X�g�������֐�

BOOL MyGetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszString, DWORD dwStringBuffer);
BOOL MyWriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszString);

DWORD MyGetProfileDWORD(LPCTSTR lpszSection, LPCTSTR lpszEntry, DWORD nDefault);
BOOL MyWriteProfileDWORD(LPCTSTR lpszSection, LPCTSTR lpszEntry, DWORD nValue);

INT RegSettingSetValue(LPCTSTR pKey, const CoString& recvData); // Setting���̕ۑ�
INT RegSettingGetValue(LPCTSTR pKey, CoString& recvData); // Setting���̎擾
INT RegSettingSetValue(LPCTSTR pKey, const DWORD& recvData); // Setting���̕ۑ�
INT RegSettingGetValue(LPCTSTR pKey, DWORD& recvData); // Setting���̎擾

///////////////////////////////////////////////////////////////////////////////////////////////////
// ���W�X�g�����

struct SYSTEM_REG
{
	DWORD no_usb_storage;
	DWORD no_add_printer;
	DWORD no_delete_printer;
	DWORD disable_print;

	SYSTEM_REG()
	{
		ZeroMemory(this, sizeof SYSTEM_REG);
	}
};

// ���C�Z���X�Ǘ�
COleDateTime GetExpireLicense(COleDateTime* expierLic = NULL); // ���C�Z���X�L�������̎擾
BOOL IsExpireLicenseDateTime(); // ���C�Z���X�L�������̔���
BOOL IsPlatinum(); // �v���`�i���C�Z���X�̊m�F
LPCTSTR GetAsessNameLicense(LPCTSTR assessment_name = NULL); // ���C�Z���X���̎擾
LPCTSTR GetShopLicense(LPCTSTR shopLic = NULL); // ���C�Z���X�X�ܖ��̎擾
LPCTSTR GetShopUnique(LPCTSTR unique = NULL); // ���C�Z���X�X��UNIQUE�̎擾
DWORD GetMassLicense(DWORD massLic = -1); // ���C�Z���X���̎擾
DWORD GetTrapperUpGrade(DWORD gradeversion = 1); // �A�b�v�O���[�h������̎擾

// �֎~���X�g
DWORD GetSuppressionList(CoString& resData, BOOL isSet = FALSE);
DWORD SetSuppressionList(const CoString& resData);
DWORD GetKillingList(CoString& resData, BOOL isSet = FALSE);
DWORD SetKillingList(const CoString& resData);
DWORD GetSuppresAutoList(CoString& resData, DWORD& resVer, BOOL isSet = FALSE);
DWORD SetSuppresAutoList(const CoString& resData, const DWORD& resVer);

// hosts
DWORD GetHostsAutoList(CoString& resData, DWORD& resVer, BOOL isSet = FALSE);
DWORD SetHostsAutoList(const CoString& resData, const DWORD& resVer);

#endif // __OUTILITY_H__
