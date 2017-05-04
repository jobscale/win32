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

#include <winsock2.h>

#include <wchar.h>
#include <stdlib.h>

PWCH DriverBase()
{
	return L"shirent";

	static wchar_t base[_MAX_PATH];
	wchar_t fpath[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];

	GetModuleFileNameW(NULL, fpath, _MAX_PATH);

//	_wsplitpath(fpath, drive, dir, fname, ext);

	wsprintfW(base, L"shirent%s", fname);

	return base;
}

extern "C"
{

PWCH WinDevName()
{
	static wchar_t wcstring[101] = { 0 };

	wsprintfW(wcstring, L"\\Device\\%s", DriverBase());

	return wcstring;
}

PWCH LinkDevName()
{
	static wchar_t wcstring[101] = { 0 };

	wsprintfW(wcstring, L"\\DosDevices\\%s", DriverBase());

	return wcstring;
}

}
