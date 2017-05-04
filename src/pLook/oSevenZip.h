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

#ifndef __SEVENZIP_H__
#define __SEVENZIP_H__

struct SEVENZIPDATA
{
	INT result;
	CoString cmdLine;
	CoString output;
	HWND hWnd;
	CoString archive;
	CoStringArray arry;
	SEVENZIPDATA();
};
typedef SEVENZIPDATA* LPSEVENZIPDATA;

INT SevenZipArchive(SEVENZIPDATA& data);
INT SevenZipGetList(SEVENZIPDATA& data);

#endif // __SEVENZIP_H__
