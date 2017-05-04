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

$target_rev = $argv[1];
if($target_rev == '')
{
	//echo "処理識別子(数字)を指定してください。\r\n";
	//"http://p.plustar.jp/repv.php?t=pads"

	exit;
}


// 現在取得可能なパラメータ
//$ cat index.html | ./treg.exe
//size_len=53024
//&body_len=51020
//&nood_len=49728
//&natu_len=359
//&take_len=516
//&count_a=13
//&bt=0.010114
//&nt=0.010376
//&count_href=104
//&count_img=28
//&size_len2=53024
//&tdk=111
//&result=16


$loop_count = 0;

$sql = "SELECT * FROM `pads_url` WHERE skip_tidy <> '1' limit 0,29000;";
$rs = mysql_query($sql, $con);
$num = mysql_numrows($rs);

for ($i = 0; $i < $num; $i++)
{
	$id   = mysql_result($rs, $i, "id"   );

	$dir_name = floor($id/1000);
	$file_name = $dir_name."/".$id.".html";

	//cat 0.html |./treg 2> 0.html.in
	$query = exec('cat ~/url/html/'.$file_name.' | treg.exe 2> tmp.html.in');
	parse_str($query,$amp);
	//$query_arr = explode("&",$query);
	//var_dump($amp);

	$a  = 0;
	$o  = 0;
	$b  = 0;
	$t  = 0;
	$c  = 0;
	$mt = 0;
	$md = 0;
	$mk = 0;
	$ac = 0;
	$ic = 0;

	if($query != '')
	{
		//var_dump($amp);

		$a  = $amp["result"];
		$o  = $amp["size_len"];
		$b  = $amp["body_len"];
		$t  = $amp["take_len"];
		$c  = $amp["bt"];
		$mt = $amp["tdk"]{0};
		$md = $amp["tdk"]{1};
		$mk = $amp["tdk"]{2};
		$ac = $amp["count_a"];
		$ic = $amp["count_img"];
	}

	echo "[".$loop_count."]:[".$id."]:".$a ."," . $o . "," . $b . "," . $t . "," . $c . "," . $mt . "," . $md . "," . $mk.",".$ac.",".$ic."\r\n";

	$sql2  = "insert into `pads_res` (`id`,`rev`,`a`,`o`,`b`,`t`,`c`,`mt`,`md`,`mk`,`ac`,`ic`) ";
	$sql2 .= "values ('".$id."','".$target_rev."','".$a."','".$o."','".$b."','".$t."','".$c."','".$mt."','".$md."','".$mk."','".$ac."','".$ic."');";
	$rs2 = mysql_query($sql2, $con);

	$loop_count++;
}

?>
