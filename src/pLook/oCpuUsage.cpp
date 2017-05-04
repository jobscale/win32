///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#include "oCpuUsage.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CCpuUsageForNT::CCpuUsageForNT()
{
	liOldIdleTime.LowPart = 0;
	liOldIdleTime.HighPart = 0;
	liOldIdleTime.u.LowPart = 0;
	liOldIdleTime.u.HighPart = 0;
	liOldIdleTime.QuadPart = 0;

	liOldSystemTime = liOldIdleTime;

	m_pProcNTQSI			= (PROCNTQSI)GetNTQSIProcAdrs();
	m_NumberOfProcessors	= GetNumberOfProcessors();
}

CCpuUsageForNT::~CCpuUsageForNT()
{
	ReleaseProc();
}

///////////////////////////////////////////////////////////////////////////////
//　NtQuerySystemInformation への関数ポインタを NTDLL.DLL から得る
///////////////////////////////////////////////////////////////////////////////
// 引数:
//	なし
//
// 戻り値:
//	PROCNTQSI	NtQuerySystemInformation への関数ポインタ
//
PROCNTQSI CCpuUsageForNT::GetNTQSIProcAdrs()
{
	PROCNTQSI NtQuerySystemInformation;
	HMODULE hModule;

	if ((hModule = GetModuleHandle("ntdll")) == NULL)
	{
		return (PROCNTQSI)NULL;
	}

	m_hModuleForNTDLL = hModule; // メンバ変数へDLLのハンドラをコピーする

    NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(hModule, "NtQuerySystemInformation");

    if (!NtQuerySystemInformation)
        return (PROCNTQSI)NULL;
	else
		return (PROCNTQSI)NtQuerySystemInformation;
}


///////////////////////////////////////////////////////////////////////////////
//　NtQuerySystemInformation への関数ポインタを解放する
///////////////////////////////////////////////////////////////////////////////
// 引数:
//	なし
//
// 戻り値:
//	なし
//
void CCpuUsageForNT::ReleaseProc()
{
	if (m_hModuleForNTDLL != NULL)
	{
		FreeLibrary(m_hModuleForNTDLL);

		// 使い終わった変数は一応 NULL にセットしておく
		m_hModuleForNTDLL = (HMODULE)NULL;
		m_pProcNTQSI = (PROCNTQSI)NULL;
		m_NumberOfProcessors = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
//　システムに搭載されている CPU の数を得ます。
///////////////////////////////////////////////////////////////////////////////
// 引数:
//	なし
//
// 戻り値:
//	int					0 エラー
//				
//			CPU の個数	1 - single processor
//						2 - dual processor
//
BYTE CCpuUsageForNT::GetNumberOfProcessors()
{
    SYSTEM_BASIC_INFORMATION       SysBaseInfo;
    LONG                           status;

    // get number of processors in the system
    status = m_pProcNTQSI(SystemBasicInformation, &SysBaseInfo, sizeof SysBaseInfo, NULL);

    if (status != NO_ERROR)
	{
        return 0;
	}

	return SysBaseInfo.bKeNumberProcessors;
}


///////////////////////////////////////////////////////////////////////////////
//　現在のCPU利用率を得ます
///////////////////////////////////////////////////////////////////////////////
// 引数:
//	なし
//
// 戻り値:
//	int		0-100 [%] のCPU利用率 (single processor のみ対応している)
//
//			0 が連続するのはエラーの可能性が高い
//
int CCpuUsageForNT::GetCpuUsageForNT()
{
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo = { 0 };
	SYSTEM_TIME_INFORMATION SysTimeInfo = { 0 };
    double dbIdleTime = 0.0;
    double dbSystemTime = 0.0;

	int cpuUsage = 0; // CPU 使用率 [%]

	// get new system time
	LONG status = m_pProcNTQSI(SystemTimeInformation, &SysTimeInfo, sizeof SysTimeInfo, 0);
	if (status != NO_ERROR)
	{
		return 0;
	}

	// get new CPU's idle time
	status = m_pProcNTQSI(SystemPerformanceInformation, &SysPerfInfo, sizeof SysPerfInfo, NULL);
	if (status != NO_ERROR)
	{
		return 0;
	}

	// if it's a first call - skip it
	if (liOldIdleTime.QuadPart != 0)
	{
		// CurrentValue = NewValue - OldValue
		dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
		dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);

		// CurrentCpuIdle = IdleTime / SystemTime
		dbIdleTime = dbIdleTime / dbSystemTime;

		// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
		dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)m_NumberOfProcessors + 0.5;
		
		//printf("\b\b\b\b%3d%%",(UINT)dbIdleTime);
		cpuUsage = (int)dbIdleTime;
	}

	// store new CPU's idle and system time
	liOldIdleTime = SysPerfInfo.liIdleTime;
	liOldSystemTime = SysTimeInfo.liKeSystemTime;

	return cpuUsage;
}


///////////////////////////////////////////////////////////////////////////////
//　NTDLL.DLL が開けたかどうか調べます。外部からの動作確認にお使いください
///////////////////////////////////////////////////////////////////////////////
// 引数:
//	なし
//
// 戻り値:
//	BOOL	TRUE	成功している。CPU の利用率を取得できる。
//			FALSE	失敗している。CPU の利用率を取得できない。
//
BOOL CCpuUsageForNT::IsOpenDLL()
{
	return (m_hModuleForNTDLL != NULL) ? TRUE: FALSE;
}
