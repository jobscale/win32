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

// pLicMaker.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

#define SEC_PLM "pLicMaker"
#define KEY_BEFORE "before"
#define KEY_AFTER "after"

// CpLicMakerApp:
// このクラスの実装については、pLicMaker.cpp を参照してください。
//

class CpLicMakerApp : public CWinApp
{
public:
	CpLicMakerApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CpLicMakerApp theApp;