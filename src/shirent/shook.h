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

#ifndef __SHOOK_H__
#define __SHOOK_H__

#include <stdio.h>
#include <ntddk.h>

extern LARGE_INTEGER	m_UserTime;
extern LARGE_INTEGER	m_KernelTime;

extern PVOID	*MappedSystemCallTable;

/*////////////////////////////////////////////////////////////////////
//	KeServiceDescriptorTable 
////////////////////////////////////////////////////////////////////*/
#pragma pack(1)	
typedef struct ServiceDescriptorEntry {
        unsigned int *ServiceTableBase;			// SSDT Base Addr
        unsigned int *ServiceCounterTableBase;	// Used only in checked build
        unsigned int NumberOfServices;			// SystemCall Index No
        unsigned char *ParamTableBase;			// SSDP 
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;
#pragma pack()

struct _SYSTEM_THREADS
{
        LARGE_INTEGER				KernelTime;
        LARGE_INTEGER				UserTime;
        LARGE_INTEGER				CreateTime;
        ULONG						WaitTime;
        PVOID						StartAddress;
        CLIENT_ID					ClientIs;
        KPRIORITY					Priority;
        KPRIORITY					BasePriority;
        ULONG						ContextSwitchCount;
        ULONG						ThreadState;
        KWAIT_REASON				WaitReason;
};

struct _SYSTEM_PROCESSES
{
        ULONG						NextEntryDelta;
        ULONG						ThreadCount;
        ULONG						Reserved[6];
        LARGE_INTEGER				CreateTime;
        LARGE_INTEGER				UserTime;
        LARGE_INTEGER				KernelTime;
        UNICODE_STRING				ProcessName;
        KPRIORITY					BasePriority;
        ULONG						ProcessId;
        ULONG						InheritedFromProcessId;
        ULONG						HandleCount;
        ULONG						Reserved2[2];
        VM_COUNTERS					VmCounters;
        IO_COUNTERS					IoCounters; //windows 2000 only
        struct _SYSTEM_THREADS		Threads[1];
};

struct _SYSTEM_PROCESSOR_TIMES
{
		LARGE_INTEGER				IdleTime;
		LARGE_INTEGER				KernelTime;
		LARGE_INTEGER				UserTime;
		LARGE_INTEGER				DpcTime;
		LARGE_INTEGER				InterruptTime;
		ULONG						InterruptCount;
};

#define SYSTEMSERVICE(_function) KeServiceDescriptorTable.ServiceTableBase[ *(PULONG)((PUCHAR)_function+1)]
#define SYSCALL_INDEX(_Function) *(PULONG)((PUCHAR)_Function+1)

#define HOOK_SYSCALL(_Function, _Hook, _Orig )  \
       _Orig = (PVOID) InterlockedExchange( (PLONG) &MappedSystemCallTable[SYSCALL_INDEX(_Function)], (LONG) _Hook)
#define UNHOOK_SYSCALL(_Function, _Hook, _Orig )  \
       InterlockedExchange( (PLONG) &MappedSystemCallTable[SYSCALL_INDEX(_Function)], (LONG) _Hook)

/*////////////////////////////////////////////////////////////////////
//  DEFINE API Function 
////////////////////////////////////////////////////////////////////*/
NTSYSAPI // Hide Process 
NTSTATUS
NTAPI 
ZwQuerySystemInformation
(
	IN ULONG				SystemInformationClass,
	IN PVOID				SystemInformation,
	IN ULONG				SystemInformationLength,
	OUT PULONG				ReturnLength
);

typedef NTSTATUS (NTAPI *ZWQUERYSYSTEMINFORMATION)(
	IN ULONG				SystemInformationClass,
	IN PVOID				SystemInformation,
	IN ULONG				SystemInformationLength,
	OUT PULONG				ReturnLength
);

ZWQUERYSYSTEMINFORMATION	OldZwQuerySystemInformation;

/*////////////////////////////////////////////////////////////////////
//	Hide Current Process
////////////////////////////////////////////////////////////////////*/
NTSTATUS NewZwQuerySystemInformation(
            IN ULONG SystemInformationClass,
            IN PVOID SystemInformation,
            IN ULONG SystemInformationLength,
            OUT PULONG ReturnLength);

#endif // __SHOOK_H__
