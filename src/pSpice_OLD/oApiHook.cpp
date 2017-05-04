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

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します
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

// 最初にDLLをマッピングしたプロセスの情報
#pragma comment(linker, "/Section:Shared,rws")
#pragma data_seg("Shared")
namespace hook
{
	DWORD firstProcessId = 0;
}
#pragma data_seg()

// 禁止されているプロセス
BOOL TerminateSuppressionProcess()
{
	int IsSuppressionRun(LPCTSTR lpCommandLine);
	BOOL IsRequiredRun(LPCTSTR lpCommandLine);

	if (!IsSuppressionRun(CoAPIHook::GetCurrentModuleFileName()))
	{
		TerminateProcess(GetCurrentProcess(), -1);
		MessageBox(NULL, "指定したプロセスの実行は禁止されています。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	if (!IsRequiredRun(CoAPIHook::GetCurrentModuleFileName()))
	{
		TerminateProcess(GetCurrentProcess(), -1);
		MessageBox(NULL, "動作環境が正常ではありません。\r\n指定したプロセスを起動することができません。", "WARNING", MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	return TRUE;
}

// ノードのトップを初期化
CoAPIHook* CoAPIHook::sm_pHead = NULL;

// コンストラクタ
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

	// ラップモジュールは処理しない
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

	// 関数を持つモジュールのハンドルを取得
	HMODULE hmodOrig = GetModuleHandleA(pszModuleName);
	if (!hmodOrig)
	{
		return;
	}

	m_pNext = sm_pHead; // 次のノードのアドレスを代入
	sm_pHead = this; // このノードのアドレスを代入

	// オリジナル関数のアドレスを取得
	PROC pfnOrig = GetProcAddress(hmodOrig, pszFuncName);

	// フックに関するデータを保存
	m_hmodOrig = hmodOrig;
	m_pszModuleName = pszModuleName;
	m_pszFuncName = pszFuncName;
	m_pfnOrig = pfnOrig;
	m_pfnHook = pfnHook;

	// プロセスIDが0ならDLLをマッピングする最初のプロセスと判断
	// そのプロセスIDを共有メモリに保存
	if (hook::firstProcessId == 0)
	{
		hook::firstProcessId = GetCurrentProcessId();
	}

	// 起動元と同じプロセスならフックを行わない
	if (hook::firstProcessId != GetCurrentProcessId())
	{
		ReplaceIATEntryInAllMods(m_pszModuleName, m_pfnOrig, m_pfnHook);
	}

	// 禁止されているプロセス
	if (!TerminateSuppressionProcess())
	{
		ExitProcess(-1);
	}
}

// デストラクタ
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

	// 起動元と同じプロセスはフックしない
	if (hook::firstProcessId != GetCurrentProcessId())
	{
		ReplaceIATEntryInAllMods(m_pszModuleName, m_pfnHook, m_pfnOrig);
	}

	// ノードのトップを取得
	CoAPIHook *p = sm_pHead;

	// ノードのトップが自分ならば、次のノードをトップに据えて終了
	if (p == this)
	{
		sm_pHead = p->m_pNext;
		return;
	}
	// もし自分ではないならば、ノードの中から検索して
	// 自分を連結から外す
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
// 競合アプリなど監視の必要がないものを排除

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
// 呼び出し元のモジュール名

/*static */LPCTSTR CoAPIHook::GetCurrentModuleFileName()
{
	// プロセスのファイル名を取得
	static CoString processPath;
	if (processPath.IsEmpty())
	{
		GetModuleFileNameA(NULL, processPath.GetBuffer(1024), 1024);
		processPath.ReleaseBuffer();
	}
	return processPath;
}

// すべてのモジュールに対してAPIフックを行う関数
void CoAPIHook::ReplaceIATEntryInAllMods(PCSTR pszModuleName, const PROC pfnCurrent, const PROC pfnNew)
{
	// 自分自身（pSpice.dll）のモジュールハンドルを取得
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery(ReplaceIATEntryInAllMods, &mbi, sizeof mbi) == 0)
	{
		return;
	}
	HMODULE hModThisMod = (HMODULE) mbi.AllocationBase;

	// モジュールリストを取得
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return;
	}

	MODULEENTRY32 me = { sizeof me };
	BOOL bModuleResult = Module32First(hModuleSnap, &me);
	// それぞれのモジュールに対してReplaceIATEntryInOneModを実行
	// ただし自分自身（pSpice.dll）には行わない
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

// ひとつのモジュールに対してAPIフックを行う関数
void CoAPIHook::ReplaceIATEntryInOneMod(PCSTR pszModuleName, const PROC pfnCurrent, const PROC pfnNew, const HMODULE hmodCaller)
{
	CoString moduleName = pszModuleName;

	// モジュールのインポートセクションのアドレスを取得
	ULONG ulSize;
	PIMAGE_IMPORT_DESCRIPTOR pFirstImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

	// インポートセクションを持っていない
	if (pFirstImportDesc == NULL)
	{
		return;
	}

	// インポートディスクリプタを検索
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

	// このモジュールは呼び出し先から関数をインポートしていない
	if (!pImportDesc || !pImportDesc->Name)
	{
		return;
	}

	// インポートアドレステーブルを取得
	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)hmodCaller + pImportDesc->FirstThunk);

	// 新しい関数アドレスに置き換える
	while (pThunk && pThunk->u1.Function)
	{
		// 関数アドレスのアドレスを取得
		PROC& pfnThunk = *(PROC*)&pThunk->u1.Function;

		// 該当関数であるならば発見！
		if (pfnThunk == pfnCurrent)
		{
			// アドレスが一致したので、インポートセクションのアドレスを書き換える
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
	// ここに処理が移った場合、インポートセクションに該当関数がなかったことになる
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// デフォルトでフックするAPIの処理（CoAPIHookクラス内部で管理する）

// デフォルトでフックする関数のプロトタイプを定義
typedef HMODULE (WINAPI *PFN_Kernel32_LoadLibraryA)(PCSTR);
typedef HMODULE (WINAPI *PFN_Kernel32_LoadLibraryW)(PCWSTR);
typedef HMODULE (WINAPI *PFN_Kernel32_LoadLibraryExA)(PCSTR, HANDLE, DWORD);
typedef HMODULE (WINAPI *PFN_Kernel32_LoadLibraryExW)(PCWSTR, HANDLE, DWORD);
typedef FARPROC (WINAPI *PFN_Kernel32_GetProcAddress)(HMODULE, PCSTR);

// モジュールを扱うAPIをあらかじめフックしておく
CoAPIHook CoAPIHook::sm_Kernel32_LoadLibraryA("Kernel32.dll", "LoadLibraryA", (PROC)CoAPIHook::Hook_Kernel32_LoadLibraryA);
CoAPIHook CoAPIHook::sm_Kernel32_LoadLibraryW("Kernel32.dll", "LoadLibraryW", (PROC)CoAPIHook::Hook_Kernel32_LoadLibraryW);
CoAPIHook CoAPIHook::sm_Kernel32_LoadLibraryExA("Kernel32.dll", "LoadLibraryExA", (PROC)CoAPIHook::Hook_Kernel32_LoadLibraryExA);
CoAPIHook CoAPIHook::sm_Kernel32_LoadLibraryExW("Kernel32.dll", "LoadLibraryExW", (PROC)CoAPIHook::Hook_Kernel32_LoadLibraryExW);
CoAPIHook CoAPIHook::sm_Kernel32_GetProcAddress("Kernel32.dll", "GetProcAddress", (PROC)CoAPIHook::Hook_Kernel32_GetProcAddress);

// 新たにロードされた関数をフックする関数
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

// 置き換わったLoadLibraryA関数
HMODULE WINAPI CoAPIHook::Hook_Kernel32_LoadLibraryA(PCSTR pszModulePath) 
{
	HMODULE hMod = ((PFN_Kernel32_LoadLibraryA)(PROC)CoAPIHook::sm_Kernel32_LoadLibraryA)(pszModulePath);
	FixupNewlyLoadedModule(hMod, 0);
	return hMod;
}

// 置き換わったLoadLibraryW関数
HMODULE WINAPI CoAPIHook::Hook_Kernel32_LoadLibraryW(PCWSTR pszModulePath) 
{
	HMODULE hMod = ((PFN_Kernel32_LoadLibraryW)(PROC)CoAPIHook::sm_Kernel32_LoadLibraryW)(pszModulePath);
	FixupNewlyLoadedModule(hMod, 0);
	return hMod;
}

// 置き換わったLoadLibraryExA関数
HMODULE WINAPI CoAPIHook::Hook_Kernel32_LoadLibraryExA(PCSTR pszModulePath, HANDLE hFile, DWORD dwFlags)
{
	HMODULE hMod = ((PFN_Kernel32_LoadLibraryExA)(PROC)CoAPIHook::sm_Kernel32_LoadLibraryExA)(pszModulePath, hFile, dwFlags);
	FixupNewlyLoadedModule(hMod, dwFlags);
	return hMod;
}

// 置き換わったLoadLibraryExW関数
HMODULE WINAPI CoAPIHook::Hook_Kernel32_LoadLibraryExW(PCWSTR pszModulePath, HANDLE hFile, DWORD dwFlags)
{
	HMODULE hMod = ((PFN_Kernel32_LoadLibraryExW)(PROC)CoAPIHook::sm_Kernel32_LoadLibraryExW)(pszModulePath, hFile, dwFlags);
	FixupNewlyLoadedModule(hMod, dwFlags);
	return hMod;
}

// 置き換わったGetProcAddress関数
FARPROC WINAPI CoAPIHook::Hook_Kernel32_GetProcAddress(HMODULE hMod, PCSTR pszProcName)
{
	// 本当の関数アドレスを取得
	FARPROC pfn = ((PFN_Kernel32_GetProcAddress)(PROC)CoAPIHook::sm_Kernel32_GetProcAddress)(hMod, pszProcName);

	// もしフックすべき関数であったならば、置き換わった関数のアドレスを渡す
	for (CoAPIHook *p = sm_pHead; (pfn != NULL) && (p != NULL); p = p->m_pNext)
	{
		// 同一モジュールで同一アドレス
		if (hMod == p->m_hmodOrig && pfn == p->m_pfnOrig)
		{
			return p->m_pfnHook;
		}
	}
   return pfn;
}
