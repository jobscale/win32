<?php
/*
 * 
 *  CREATE TABLE `pads_res` (
 *   `id` int(10) unsigned DEFAULT '0',
 *   `rev` char(10) CHARACTER SET utf8 DEFAULT NULL,
 *   `a` int(10) unsigned DEFAULT '0',
 *   `o` int(10) unsigned DEFAULT '0',
 *   `b` int(10) unsigned DEFAULT '0',
 *   `t` int(10) unsigned DEFAULT '0',
 *   `c` float(10,4) unsigned DEFAULT '0.0000',
 *   `mt` int(10) unsigned DEFAULT '0',
 *   `md` int(10) unsigned DEFAULT '0',
 *   `mk` int(10) unsigned DEFAULT '0',
 *   UNIQUE KEY `idx_id_rev` (`id`,`rev`)
 * ) ENGINE=MyISAM DEFAULT CHARSET=binary
 *  
 * rev 1,リビジョン2817のもの
 * rev 2,リビジョン2817からdescription、keywordsの0.01計算を抜いたもの
 * rev 3,リビジョン2817から19,21を抜いたもの
 * rev 4,リビジョン2828からアダルトワードsexを抜いたもの
 * 
 */

mb_regex_encoding('UTF-8');
mb_internal_encoding('UTF-8');

////////////////////////////////////////////////////////////////////////////
//// Mysql
$con = mysql_connect("localhost", "root", "rootzero");
mysql_select_db("test", $con);
mysql_query("SET NAMES utf8" , $con);

$target_file = $argv[1];
if($target_file == '')
{
	echo "処理対象ファイルを指定して下さい。\r\n";
}

$target_rev = $argv[2];
if($target_rev == '')
{
	//echo "処理識別子(数字)を指定してください。\r\n";
	//"http://p.plustar.jp/repv.php?t=pads"

	exit;
}

// 現在取得可能なパラメータ
// http://einstein.plustar.jp/pAds/0/1.html
// ?a=10
// &o=14575
// &b=13368
// &t=460
// &c=0.034
// &h=100

if(!file_exists($target_file))
{
	echo "対象ファイルが存在しません\r\n";
	exit;
}

$fp = fopen($target_file,'r');
while (!feof($fp))
{
	$url = fgets($fp, 1024);

	$url = str_replace("\r","",$url);
	$url = str_replace("\n","",$url);
	$url = str_replace("\r\n","",$url);

	$arr_url = parse_url($url);
	//var_dump($arr_url);
	$host = $arr_url["host"];
	$path = $arr_url["path"];
	$query = $arr_url["query"];

	$id = str_replace(".html","",basename($path));

	$a  = 0;
	$o  = 0;
	$b  = 0;
	$t  = 0;
	$c  = 0;
	$mt = 0;
	$md = 0;
	$mk = 0;

	if($query != '')
	{
		parse_str($query,$amp);
		//var_dump($amp);

		$a  = $amp["a"];
		$o  = $amp["o"];
		$b  = $amp["b"];
		$t  = $amp["t"];
		$c  = $amp["c"];
		$mt = $amp["h"]{0};
		$md = $amp["h"]{1};
		$mk = $amp["h"]{2};
	}

	echo $a ."," . $o . "," . $b . "," . $t . "," . $c . "," . $mt . "," . $md . "," . $mk."\r\n";

	$sql2 = "insert into `pads_res` (`id`,`rev`,`a`,`o`,`b`,`t`,`c`,`mt`,`md`,`mk`) values ('".$id."','".$target_rev."','".$a."','".$o."','".$b."','".$t."','".$c."','".$mt."','".$md."','".$mk."');";
	$rs2 = mysql_query($sql2, $con);
}
fclose($fp);

?>
