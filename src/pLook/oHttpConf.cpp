///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
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

#include "oHttpConf.h"

#define P(x) ("\0 " x + 2)

const char* HTTP_PLUSTAR[] = {
//	P("http://anaj.plustar.jp/ciap.cgi"),            //  x: �e�X�g�p(c����o�[�W����)
	P("http://anaj.plustar.jp/ciap.php"),            //  0: �N���A�v���P�[�V�������v
	P("http://anaj.plustar.jp/pafp.php"),            //  1: �t�H�A�O���E���h�A�v���P�[�V�������v
	P("http://anaj.plustar.jp/phsa.php"),            //  2: �֎~�v���Z�XURL(�T�[�o�[����)
	P("http://anaj.plustar.jp/phha.php"),            //  3: hostsURL(�T�[�o����)
	P("http://anaj.plustar.jp/phtp.php"),            //  4: ��~�v���Z�X�̒ʒmURL
	P("                               "),            //  5: 
	P("http://ext.plustar.jp/history.json"),         //  6: �A�b�v�f�[�g����
	P("http://dl.plustar.jp/trapper/update/newst/"), //  7: �A�b�v�f�[�g�u����
	P("http://ext.plustar.jp/store/lic_res.php"),    //  8: ���C�Z���X�X�V
	P("http://www.plustar.jp/trapper/"),             //  9: ExTrapper HOME
	P("http://anaj.plustar.jp/phpu.php"),            // 10: �|�b�v�A�b�v���オ�����ꍇ�ɒʒm
	P("http://anaj.plustar.jp/phhw.php"),            // 11: �n�[�h�E�F�A���̃A�b�v���[�h
	P("http://anaj.plustar.jp/phst.php"),            // 12: �����v�f�[�^�̃A�b�v���[�h
	P("http://anaj.plustar.jp/phpr.php"),            // 13: ������v�f�[�^�̃A�b�v���[�h
	P("http://anaj.plustar.jp/pspl.php"),            // 14: �Ǘ��ݒ�̋֎~���X�g���A�b�v���[�h
};
