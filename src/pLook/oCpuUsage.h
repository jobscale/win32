///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

// CpuUsageForNT.h: CCpuUsageForNT クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#ifndef _CCPUUSAGEFORNT_H_INCLUDED_
#define _CCPUUSAGEFORNT_H_INCLUDED_

#include <windows.h>
#include <conio.h>
#include <stdio.h>

#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

typedef struct
{
    DWORD   dwUnknown1;
    ULONG   uKeMaximumIncrement;
    ULONG   uPageSize;
    ULONG   uMmNumberOfPhysicalPages;
    ULONG   uMmLowestPhysicalPage;
    ULONG   uMmHighestPhysicalPage;
    ULONG   uAllocationGranularity;
    PVOID   pLowestUserAddress;
    PVOID   pMmHighestUserAddress;
    ULONG   uKeActiveProcessors;
    BYTE    bKeNumberProcessors;
    BYTE    bUnknown2;
    WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
    LARGE_INTEGER   liIdleTime;
    DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

// ntdll!NtQuerySystemInformation (NT specific!)
//
// The function copies the system information of the
// specified type into a buffer
//
// NTSYSAPI
// NTSTATUS
// NTAPI
// NtQuerySystemInformation(
//    IN UINT SystemInformationClass,    // information type
//    OUT PVOID SystemInformation,       // pointer to buffer
//    IN ULONG SystemInformationLength,  // buffer size in bytes
//    OUT PULONG ReturnLength OPTIONAL   // pointer to a 32-bit
//                                       // variable that receives
//                                       // the number of bytes
//                                       // written to the buffer 
// );
typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);

class CCpuUsageForNT  
{
public:
	BOOL IsOpenDLL(void);
	CCpuUsageForNT();
	virtual ~CCpuUsageForNT();

	int GetCpuUsageForNT(void);
	BYTE GetNumberOfProcessors(void);

protected:
	void ReleaseProc(void);
	PROCNTQSI GetNTQSIProcAdrs(void);

	PROCNTQSI m_pProcNTQSI;		// NtQuerySystemInformation への関数ポインタ
	HMODULE m_hModuleForNTDLL;	// NTDLL.DLL への module handler

	int m_NumberOfProcessors;	// システムに搭載されているCPUの個数

private:
	LARGE_INTEGER liOldIdleTime;
	LARGE_INTEGER liOldSystemTime;
};

#endif /* #ifdef _CCPUUSAGEFORNT_H_INCLUDED_ */
