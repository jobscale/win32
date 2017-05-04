<?php
/*
 * usage:gethtml.sh > gethtml.log 2>&1
 */

mb_regex_encoding('UTF-8');
mb_internal_encoding('UTF-8');

////////////////////////////////////////////////////////////////////////////
// 同時実行数 //////////////////////////////////////////////////////////////
$EXEC_PROC_NUM = 2;
////////////////////////////////////////////////////////////////////////////

$fp = fopen("html.list",'r');
$lc = 0; // 対象行数
while (!feof($fp))
{
	$url = fgets($fp, 1024);
	$lc++;
}
fclose($fp);

// 実行数分のファイルポインタの作成
$arr_fp = array();
for($i = 0; $i < $EXEC_PROC_NUM; $i++)
{
	$arr_fp[$i] = @fopen("html-".$i.".list", 'wb');
  @flock($arr_fp[$i], LOCK_EX);
}

$lc_t   = 0;
$fp_sp = $lc/$EXEC_PROC_NUM;
$fp = fopen("html.list",'r');
while (!feof($fp))
{
	$url = fgets($fp, 1024);

	$fp_name = floor($lc_t/$fp_sp);
	fwrite($arr_fp[$fp_name], $url);

	$lc_t++;
}
fclose($fp);

for($i = 0; $i < $EXEC_PROC_NUM; $i++)
{
	@flock($arr_fp[$i], LOCK_UN);
	fclose($arr_fp[$i]);
}

// 取得部分本体
for($i = 0; $i < $EXEC_PROC_NUM; $i++)
{
	$abstract = @fopen("abstract-".$i.".sh", 'wb');
	@flock($abstract, LOCK_EX);
	fwrite($abstract, "#!/bin/bash\n");
	fwrite($abstract, "\n");
	fwrite($abstract, "while read targ\n");
	fwrite($abstract, "do\n");
	fwrite($abstract, "  RET=`wget \$targ -O- 2>&1 |grep pad.plustar.jp |while read line; do expr \"\$line\" : '.*plustar-anal?\([^ \"]*\\)'; done`\n");
	fwrite($abstract, "  echo \$targ?\$RET\n");
	fwrite($abstract, "  #if [ \$RET = \"\" ]\n");
	fwrite($abstract, "  #then\n");
	fwrite($abstract, "  #  echo \"?a=0&b=0&t=0&c=0.000\"\n");
	fwrite($abstract, "  #fi\n");
	fwrite($abstract, "done\n");
	@flock($abstract, LOCK_UN);
	fclose($abstract);
}

// 上部シェル
$gethtml = @fopen("gethtml.sh", 'wb');
@flock($gethtml, LOCK_EX);

fwrite($gethtml, "#!/bin/bash\n");
fwrite($gethtml, "echo  http_proxy = http://localhost:8118/> ~/.wgetrc\n");
for($i = 0; $i < $EXEC_PROC_NUM; $i++)
{
	fwrite($gethtml, "./abstract-".$i.".sh <html-".$i.".list &\n");
}
@flock($gethtml, LOCK_UN);
fclose($gethtml);
?>
