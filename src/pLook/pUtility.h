///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__OUTILITY_H__)
#define __OUTILITY_H__

#include <wininet.h>
#include <afxdisp.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// 定義

///////////////////////////////////////////////////////////////////////////////////////////////////
// 共有変数

extern const CHAR* RETERCODE;
extern const CHAR* DELIMITER;

///////////////////////////////////////////////////////////////////////////////////////////////////
// ディレクトリ処理関数

INT PleaseTradeName(CoString& localpath, LPCTSTR type, LPCTSTR ext);

///////////////////////////////////////////////////////////////////////////////////////////////////
// 文字列処理関数

INT SJIStoUTF8(const CoString& sjis, CoString& utf8);
INT UTF8toSJIS(const CoString& utf8, CoString& sjis);
INT SJIStoEUC(const CComBSTR& sjis, CComBSTR& euc);
HRESULT DateFromString(SYSTEMTIME* sysTime, LPCTSTR sysTimeText, size_t len = 0);
HRESULT DateFromString(COleDateTime& sysTime, LPCTSTR sysTimeText, size_t len = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
// レジストリ処理関数

BOOL MyGetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszString, DWORD dwStringBuffer);
BOOL MyWriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszString);

DWORD MyGetProfileDWORD(LPCTSTR lpszSection, LPCTSTR lpszEntry, DWORD nDefault);
BOOL MyWriteProfileDWORD(LPCTSTR lpszSection, LPCTSTR lpszEntry, DWORD nValue);

INT RegSettingSetValue(LPCTSTR pKey, const CoString& recvData); // Setting情報の保存
INT RegSettingGetValue(LPCTSTR pKey, CoString& recvData); // Setting情報の取得
INT RegSettingSetValue(LPCTSTR pKey, const DWORD& recvData); // Setting情報の保存
INT RegSettingGetValue(LPCTSTR pKey, DWORD& recvData); // Setting情報の取得

///////////////////////////////////////////////////////////////////////////////////////////////////
// レジストリ情報

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

// ライセンス管理
COleDateTime GetExpireLicense(COleDateTime* expierLic = NULL); // ライセンス有効期限の取得
BOOL IsExpireLicenseDateTime(); // ライセンス有効期限の判定
BOOL IsPlatinum(); // プラチナライセンスの確認
LPCTSTR GetAsessNameLicense(LPCTSTR assessment_name = NULL); // ライセンス名の取得
LPCTSTR GetShopLicense(LPCTSTR shopLic = NULL); // ライセンス店舗名の取得
LPCTSTR GetShopUnique(LPCTSTR unique = NULL); // ライセンス店舗UNIQUEの取得
DWORD GetMassLicense(DWORD massLic = -1); // ライセンス数の取得
DWORD GetTrapperUpGrade(DWORD gradeversion = 1); // アップグレード文字列の取得

// 禁止リスト
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
