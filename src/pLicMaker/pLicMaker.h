///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
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

// pLicMaker.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��

#define SEC_PLM "pLicMaker"
#define KEY_BEFORE "before"
#define KEY_AFTER "after"

// CpLicMakerApp:
// ���̃N���X�̎����ɂ��ẮApLicMaker.cpp ���Q�Ƃ��Ă��������B
//

class CpLicMakerApp : public CWinApp
{
public:
	CpLicMakerApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CpLicMakerApp theApp;