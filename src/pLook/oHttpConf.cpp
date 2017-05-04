///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
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
//	P("http://anaj.plustar.jp/ciap.cgi"),            //  x: テスト用(c言語バージョン)
	P("http://anaj.plustar.jp/ciap.php"),            //  0: 起動アプリケーション統計
	P("http://anaj.plustar.jp/pafp.php"),            //  1: フォアグラウンドアプリケーション統計
	P("http://anaj.plustar.jp/phsa.php"),            //  2: 禁止プロセスURL(サーバー同期)
	P("http://anaj.plustar.jp/phha.php"),            //  3: hostsURL(サーバ同期)
	P("http://anaj.plustar.jp/phtp.php"),            //  4: 停止プロセスの通知URL
	P("                               "),            //  5: 
	P("http://ext.plustar.jp/history.json"),         //  6: アップデート履歴
	P("http://dl.plustar.jp/trapper/update/newst/"), //  7: アップデート置き場
	P("http://ext.plustar.jp/store/lic_res.php"),    //  8: ライセンス更新
	P("http://www.plustar.jp/trapper/"),             //  9: ExTrapper HOME
	P("http://anaj.plustar.jp/phpu.php"),            // 10: ポップアップが上がった場合に通知
	P("http://anaj.plustar.jp/phhw.php"),            // 11: ハードウェア情報のアップロード
	P("http://anaj.plustar.jp/phst.php"),            // 12: 可動統計データのアップロード
	P("http://anaj.plustar.jp/phpr.php"),            // 13: 印刷統計データのアップロード
	P("http://anaj.plustar.jp/pspl.php"),            // 14: 管理設定の禁止リストをアップロード
};
