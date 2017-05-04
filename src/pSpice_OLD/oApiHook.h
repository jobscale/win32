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

#ifndef __O_APIHOOK_H__
#define __O_APIHOOK_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// APIフックを実行するメインルーチン

class CoAPIHook 
{
public:

	// コンストラクタ
	CoAPIHook(PSTR, PSTR, PROC);

	// デストラクタ
	~CoAPIHook();

	// オリジナルアドレス取得用
	operator PROC() { return m_pfnOrig; }

protected:

	static CoAPIHook *sm_pHead;  // ノードのトップのデータ
	CoAPIHook *m_pNext;          // 次のノードのデータ

	// 保存領域
	HMODULE m_hmodOrig; // 関数を持つモジュールのハンドル
	PCSTR m_pszModuleName; // 関数を持つモジュール名（ANSI）
	PCSTR m_pszFuncName; // 関数名（ANSI）
	PROC m_pfnOrig; // オリジナル関数のアドレス
	PROC m_pfnHook; // 置き換わる関数のアドレス

public:

	// サービス関数
	static LPCTSTR GetCurrentModuleFileName(); // モジュール名の取得

protected:

	// 自給関数
	virtual BOOL IsIgnoreModule() const;

	// APIフックを適応させるメイン関数
	static void WINAPI ReplaceIATEntryInAllMods(PCSTR, const PROC, const PROC);
	static void WINAPI ReplaceIATEntryInOneMod(PCSTR, const PROC, const PROC, const HMODULE);

	// 新たにロードされたモジュールにAPIフックを対応させる関数
	static void WINAPI FixupNewlyLoadedModule(HMODULE, DWORD);

	// それぞれのAPIに置き換わるフック関数
	static HMODULE WINAPI Hook_Kernel32_LoadLibraryA(PCSTR);
	static HMODULE WINAPI Hook_Kernel32_LoadLibraryW(PCWSTR);
	static HMODULE WINAPI Hook_Kernel32_LoadLibraryExA(PCSTR, HANDLE, DWORD);
	static HMODULE WINAPI Hook_Kernel32_LoadLibraryExW(PCWSTR, HANDLE, DWORD);
	static FARPROC WINAPI Hook_Kernel32_GetProcAddress(HMODULE, PCSTR);

	// それぞれのAPIをフックすることを宣言
	static CoAPIHook sm_Kernel32_LoadLibraryA;
	static CoAPIHook sm_Kernel32_LoadLibraryW;
	static CoAPIHook sm_Kernel32_LoadLibraryExA;
	static CoAPIHook sm_Kernel32_LoadLibraryExW;
	static CoAPIHook sm_Kernel32_GetProcAddress;

};

#endif // __O_APIHOOK_H__
