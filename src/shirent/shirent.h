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

#ifndef __SHIRENT_H__
#define __SHIRENT_H__

#define BOOL unsigned long
#define	Dbgmsg			L" UNNOOK!!"

///////  Original Device Code   ///////
#define IO_DEVICE_TYPE				0x00001000
#define DRIVER_IO(_code)			CTL_CODE(IO_DEVICE_TYPE, _code, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_SDT_HIDE_PROC_REST	DRIVER_IO(0x3)

unsigned int SDT_FLG=0;
unsigned int PROC_LIST_FLG = 0; 

//  Startup Infomation Message  //
VOID InfoMSG()
{
	DbgPrint("//=========================================//\n");
	DbgPrint("//\n//　WinXP Driver :: 実装完了\n");
	DbgPrint("//　実行させたいボタンを押してください\n//\n");
	DbgPrint("//=========================================//\n");
}

//  IRP_MJ_**** Request Message  //

NTSTATUS Driver_Load_Func
(IN PDEVICE_OBJECT pdevice_object, IN PIRP Irp)
{
	DbgPrint("//=========================================//\n");
	DbgPrint("//  Loading Driver :: Start!!\n");
	DbgPrint("//=========================================//\n");
	return STATUS_SUCCESS;
}
NTSTATUS MJ_CREAT_FUNC
(IN PDEVICE_OBJECT pdevice_object, IN PIRP Irp)
{
//	DbgPrint("//=========================================//\n");
//	DbgPrint("// Calling IRP_MJ_CREATE\n");
//	DbgPrint("//=========================================//\n");
	return STATUS_SUCCESS;
}
NTSTATUS MJ_CLOSE_FUNC
(IN PDEVICE_OBJECT pdevice_object, IN PIRP Irp)
{
//	DbgPrint("//=========================================//\n");
//	DbgPrint("// Calling IRP_MJ_CLOSE\n");
//	DbgPrint("//=========================================//\n");
	return STATUS_SUCCESS;
}
NTSTATUS TEST_FUNC
(IN PDEVICE_OBJECT pdevice_object, IN PIRP Irp)
{
//	DbgPrint("//=========================================//\n");
//	DbgPrint("// TEST TEST TEST!!\n");
//	DbgPrint("//=========================================//\n");
	return STATUS_SUCCESS;
}

#endif // __SHIRENT_H__
