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

#include "shook.h"
#include "shirent.h"

#include <stdlib.h>

HANDLE chProcess;	// Process 監視用
PEPROCESS cProcess;	// 
PMDL	g_pmdlSystemCall;
__declspec(dllimport) ServiceDescriptorTableEntry_t KeServiceDescriptorTable;

/*/////////////////////////////////////////////////////////////////////
//　PsSetCreateProcessNotifyRoutine & PsLookupProcessByProcessid で
//　何かつくるかー？
*//////////////////////////////////////////////////////////////////////
NTSTATUS Get_Current_Process()
{
	cProcess = PsGetCurrentProcess();
	chProcess = PsGetCurrentProcessId();
	
//	if( PROC_LIST_FLG == 1 )
//	{
//		DbgPrint("DEGBUG : Process ID %d (%x) が稼動中・・・\n",  chProcess,cProcess );
//		return STATUS_SUCCESS;	
//	}
	return STATUS_SUCCESS;
}

PWCH WinDevName();
PWCH LinkDevName();

#define	WIN_DEVICE_NAME L"\\Device\\shirent"
#define	LINK_DEVICE_NAME L"\\DosDevices\\shirent"

////////////////////////////////////////////////////////////////
//  Hidden Process Restore	& Drive Unload
////////////////////////////////////////////////////////////////
VOID SDT_HIDE_RESTOR()
{
	UNHOOK_SYSCALL( ZwQuerySystemInformation, OldZwQuerySystemInformation, NewZwQuerySystemInformation );
	// Unlock and Free MDL
	if(g_pmdlSystemCall)
	{
		MmUnmapLockedPages(MappedSystemCallTable, g_pmdlSystemCall);
		IoFreeMdl(g_pmdlSystemCall);
	}
}
VOID Driver_Unload_Func
(IN PDRIVER_OBJECT pdriver_object)
{
	UNICODE_STRING usLinkDeviceName;
	RtlInitUnicodeString ( &usLinkDeviceName, LINK_DEVICE_NAME);

	 IoDeleteSymbolicLink( &usLinkDeviceName );
	 IoDeleteDevice( pdriver_object -> DeviceObject);

	if (SDT_FLG == 0)
	{
		DbgPrint("//\n//　プロセスリストの初期化\n");
		SDT_HIDE_RESTOR();
	 	SDT_FLG =1;
	}else DbgPrint("//　\nプロセステーブルの不整合回避のため一度のみの実行です\n");
	
	DbgPrint("//=========================================//\n");
	DbgPrint("//  Driver UnInstall :: See You !!\n");
	DbgPrint("//=========================================//\n");
	//return STATUS_SUCCESS;
}
////////////////////////////////////////////////////////////////
//  Calling IRP_MJ_DEVICE_CONTROL 
////////////////////////////////////////////////////////////////
NTSTATUS Device_Control
(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	char	str[1024];
	PCHAR	oBuffer, inBuffer;

	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
	if(!irpStack)
	{
		DbgPrint("// \n //  Nothing IRP STACK LOCATION !! //\n");
	}

	switch (irpStack ->Parameters.DeviceIoControl.IoControlCode)
	{

		//  SDT QueryInfo 初期化
		//
		case IOCTRL_SDT_HIDE_PROC_REST:
			if (SDT_FLG == 0)
			{
				DbgPrint("//\n//　プロセスリストの初期化\n");
				SDT_HIDE_RESTOR();
			 	SDT_FLG =1;
			}else DbgPrint("//　\nプロセステーブルの不整合回避のため一度のみの実行できません\n");
		break;
		return 0;
	}

	// IRP STACK LOCATION -> 0
	Irp->IoStatus.Information = 0;

	// IRP 完了
	Irp->IoStatus.Status = STATUS_SUCCESS;

	// IRP ->  I/Oマネージャへ通知
	IoCompleteRequest( Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


/**************************************************************
/*	Driver Entry :: Main()
/**************************************************************/
NTSTATUS DriverEntry
(IN PDRIVER_OBJECT  pDriverObject,
 IN PUNICODE_STRING pRegistryPath)
{
	ULONG	i, con;
	PDEVICE_OBJECT	Device_Object;
	PDRIVER_OBJECT	DriverObject;
	NTSTATUS	rc, ntstatus;
	UNICODE_STRING	usDriverName;
	UNICODE_STRING	usLinkDeviceName;


	RtlInitUnicodeString (&usDriverName, 	 WIN_DEVICE_NAME);
	RtlInitUnicodeString (&usLinkDeviceName, LINK_DEVICE_NAME);

	// Driver on Virtual Device
	ntstatus = IoCreateDevice(  pDriverObject,
								0, 
								&usDriverName,
								FILE_DEVICE_UNKNOWN,
								0,
								TRUE,
								&Device_Object );	
	if (! NT_SUCCESS(ntstatus) )
	{
		 DbgPrint("///  IoCreatDevice Failed !!\n///");
		 return ntstatus;
	}
	
	
	//	SymbolickLink 
	ntstatus = IoCreateSymbolicLink ( &usLinkDeviceName,
									  &usDriverName);
	if (! NT_SUCCESS(ntstatus) )
	{
		 DbgPrint("///  IoCreateSymbolicLink Failed !!\n///");
		 return ntstatus;
	}
	
	for(i=0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriverObject -> MajorFunction[i] = TEST_FUNC;
	}

	// 
	pDriverObject -> MajorFunction[IRP_MJ_CREATE] = MJ_CREAT_FUNC;	
	pDriverObject -> MajorFunction[IRP_MJ_CLOSE]  = MJ_CLOSE_FUNC;
	pDriverObject -> MajorFunction[IRP_MJ_DEVICE_CONTROL] = Device_Control; 
	pDriverObject -> DriverUnload = Driver_Unload_Func; 
	
	m_UserTime.QuadPart = m_KernelTime.QuadPart = 0;

	// SSDT -> オリジナル保存
	OldZwQuerySystemInformation =(ZWQUERYSYSTEMINFORMATION)(SYSTEMSERVICE(ZwQuerySystemInformation));

	// MDL マップ
	g_pmdlSystemCall = MmCreateMdl(NULL, KeServiceDescriptorTable.ServiceTableBase, KeServiceDescriptorTable.NumberOfServices*4);
	if(!g_pmdlSystemCall)
		return STATUS_UNSUCCESSFUL;
	MmBuildMdlForNonPagedPool(g_pmdlSystemCall);
	
	// MDL フラグ
	g_pmdlSystemCall->MdlFlags = g_pmdlSystemCall->MdlFlags | MDL_MAPPED_TO_SYSTEM_VA;
	MappedSystemCallTable = MmMapLockedPages(g_pmdlSystemCall, KernelMode);

	// Hook
	HOOK_SYSCALL( ZwQuerySystemInformation, NewZwQuerySystemInformation, OldZwQuerySystemInformation );
	return STATUS_SUCCESS;
}

int compare(PWCH target)
{
	PWCH tgl[] = {
		L"\0 pCheckWindow.exe" + 2,
		L"\0 pTools.exe" + 2,
		L"\0 pExTrapper.exe" + 2,
	};

	int i = 0, size = sizeof(tgl) / sizeof(LPSTR);
	for (i = 0; i < size; i++)
	{
		if (0 == memcmp(target, tgl[i], 6))
		{
			return 0;
		}
	}

	return 1;
}
