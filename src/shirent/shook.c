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

#include "sHook.h"

LARGE_INTEGER	m_UserTime;
LARGE_INTEGER	m_KernelTime;

PVOID	*MappedSystemCallTable;

/////// SYSENTER HOOK
ULONG	SYSENTER_CS_MSR  = 0x174;
ULONG	SYSENTER_ESP_MSR = 0x175;
ULONG	SYSENTER_EIP_MSR  = 0x176;
ULONG 	O_KiFastCallEntry;		// Original KiFastCallEntry , SYSENTER Hook
ULONG 	SYSENTER_EIP_MSR_H;	// 
ULONG 	SYSENTER_CS_MSR_L;	//
ULONG 	GET_SYSTEMCALL_NO;

/*////////////////////////////////////////////////////////////////////
//　プロテクトモード & リアルモード
////////////////////////////////////////////////////////////////////*/
/*
VOID Set_Protect()
{
	_asm{
		CLI
		MOV	EAX,	CR0
		AND	EAX,	NOT 10000H
		MOV	CR0,	EAX
	}
}

VOID Set_Real()
{
	_asm{
		MOV	EAX,	CR0
		OR	EAX,	100000H
		MOV	CR0,	EAX
		STI
	}
}

__declspec(naked) HOOK_SYSTEMCALL()
{
	_asm {
		pushad
		pushfd
		push fs
		mov GET_SYSTEMCALL_NO, eax
	}

	if( GET_SYSTEMCALL_NO == 0x1211 )
	{
		DbgPrint("// \n//　SetWindowsHookAW 呼び出し！！！\n");
	}

	if( GET_SYSTEMCALL_NO == 0x1212 )
	{
		DbgPrint("// \n//　SetWindowsHookEx 呼び出し！！！\n");
	}

	if( GET_SYSTEMCALL_NO == 0x1225 ) 
	{
		DbgPrint("// \n//　UnSetWindowsHOOK 呼び出し！！！\n");
	}

	_asm{
		pop fs
		popfd
		popad
		jmp [O_KiFastCallEntry]
	}
}

VOID SYSENTER_HOOK()
{
	_asm{
		cli
		mov ecx, SYSENTER_EIP_MSR 
		rdmsr
		mov O_KiFastCallEntry, eax
		mov eax, HOOK_SYSTEMCALL
		wrmsr
		sti
	}
}

VOID SYSENTER_HOOK_REST()
{
	_asm{
		cli
		mov ecx, SYSENTER_EIP_MSR 
		rdmsr
		mov eax, O_KiFastCallEntry
		wrmsr
		sti
	}
}
*/
NTSTATUS NewZwQuerySystemInformation(
            IN ULONG SystemInformationClass,
            IN PVOID SystemInformation,
            IN ULONG SystemInformationLength,
            OUT PULONG ReturnLength)
{
   NTSTATUS ntStatus;

   //  Get_Current_Process();
   ntStatus = ((ZWQUERYSYSTEMINFORMATION)(OldZwQuerySystemInformation)) (
											SystemInformationClass,
											SystemInformation,
											SystemInformationLength,
											ReturnLength );
   if( NT_SUCCESS(ntStatus)) 
   {
      // SDT クラス -> 5
      if(SystemInformationClass == 5)
      {					
		 struct _SYSTEM_PROCESSES *curr = (struct _SYSTEM_PROCESSES *)SystemInformation;
         struct _SYSTEM_PROCESSES *prev = NULL;
		 
		 while(curr)
		 {
            if (curr->ProcessName.Buffer != NULL)
			{
				int compare(PWCH target);
				if (0 == compare(curr->ProcessName.Buffer))
				{
					// 同期
					m_UserTime.QuadPart += curr->UserTime.QuadPart;
					m_KernelTime.QuadPart += curr->KernelTime.QuadPart;

					// 以下プロセスリンク
					if(prev) 
					{
						if(curr->NextEntryDelta)
							prev->NextEntryDelta += curr->NextEntryDelta;
						else
							prev->NextEntryDelta = 0;
					}
					else
					{
						if(curr->NextEntryDelta)
						{
							(char *)SystemInformation += curr->NextEntryDelta;
						}
						else 
							SystemInformation = NULL;
					}
				}
			}
			else
			{
			   curr->UserTime.QuadPart += m_UserTime.QuadPart;
			   curr->KernelTime.QuadPart += m_KernelTime.QuadPart;
				m_UserTime.QuadPart = m_KernelTime.QuadPart = 0;
			}
			prev = curr;
		    if(curr->NextEntryDelta) ((char *)curr += curr->NextEntryDelta);
		    else curr = NULL;
	     }
	  }
	  else if (SystemInformationClass == 8) 
	  {
         struct _SYSTEM_PROCESSOR_TIMES * times = (struct _SYSTEM_PROCESSOR_TIMES *)SystemInformation;
         times->IdleTime.QuadPart += m_UserTime.QuadPart + m_KernelTime.QuadPart;
	  }

   }
   return ntStatus;
}
