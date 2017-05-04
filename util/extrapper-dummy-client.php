<?php
//error_reporting(E_ALL | E_STRICT);
$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($socket, '0.0.0.0', 12083);

while(1)
{
  $from = '';
	$port = 0;
	socket_recvfrom($socket, $buf, 19, 0, $from, $port);
	echo "ExTrapperサーバ[$from(Port：$port)]から[$buf]を受信" . PHP_EOL;

	if($buf == 'GET CONSOLE COMMAND')
	{
		echo "Send Data Sending …";
    for($i = 0; $i < 25; $i++)
    {
			$fp1  = fsockopen ($from, 12084, $errno, $errstr, 30);

			if($i < 100)
			{
				$uniq_num = sprintf("%02d",$i);

				$mc = '87:77:67:f7:67:'.$uniq_num;
			}
			else
			{
				$i_tmp = $i -99;
				$uniq_num = sprintf("%02d",$i_tmp);

				$mc = '87:77:67:f7:'.$uniq_num.':'.$uniq_num;
			}

			$uniq_num = sprintf("%03d",$i);
			$hn = 'PLUSTAR'.$uniq_num;
			$un = 'PLUSTAR'.$uniq_num;
			$ip = '172.16.1.'.$uniq_num;

			$data1 = "TCP 01 DATA  macaddr \"".$mc."\" ipaddr \"".$ip."\" hostname \"".$hn."\"";

			fputs ($fp1, $data1);
			while (!feof($fp1))
			{
				echo fgets ($fp1,128);
			}
			fclose ($fp1);

			if($i == 0)
			{
				$total_packet  = 6000000 * rand(1,10);
				$in_packet  = $total_packet * (rand(10,90) * 0.01);
				$out_packet = $total_packet - $in_packet;
			}
			else
			{
				$total_packet  = rand(100,300000000);
				$in_packet  = $total_packet * (rand(10,90) * 0.01);
				$out_packet = $total_packet - $in_packet;
			}

			$connections = rand(0,30);
			if($connections > 20)
				$connections = rand(0,30);

			// 不要なパタメータ(？)
			$dwTotalPhys = 0;//rand(10000,1000000);
			$dwAvailPhys = 0;//$dwTotalPhys * (rand(1,99) * 0.01);

			$dwTotalVirtual = 0;//1000000;
			$dwAvailVirtual = 0;//rand(10000,1000000);

			$ulFreeBytesAvailable = 0;//10;

			// ディスク使用率(下9桁の削除が発生するので)
			$ulBytes                   = rand(10,2000);
			$ulTotalNumberOfBytes      = $ulBytes . "000000000";
			$ulTotalNumberOfFreeBytes  = ($ulBytes * (rand(5,90)*0.01)) . "000000000";

			$target_time = mktime(date("H") - rand(0,24), rand(0,59), rand(0,59), date("m")  , date("d"), date("Y"));
			$boot_time = date("Y/m/d H:i:s", $target_time);

			$time_span_tmp = time() - $target_time;

			/*
			 * http://q.hatena.ne.jp/1176883168
			 * [Q]----------------------------------
			 * PHPで秒数の数値があり、それを
			 * 時：分：秒みたいに変換するには
			 * どうしたいいのでしょうか？
			 * 
			 * $var = 90;
			 * これを
			 * $time = 00:01:30という風に
			 * 変換したいです。
			 * [A]----------------------------------
			 * gmdate だと
			 * $var = 3600*25 ;
			 * のときに「01:00:00」になります。「25:00:00」が欲しいときには
			 * $time = sprintf("%d:%s", $var/3600, gmdate("i:s", $var%3600)) ;
			 * とかにするということです。
			 * 日数を出すのは
			 * $time = sprintf("%d %s", $var/(3600*24), gmdate("H:i:s", $var)) ;
			 * とか。
			 * 月や年は日付と連動しないと無理ですが。
			 */
			$time_span = sprintf("%02d:%s", $time_span_tmp/3600, gmdate("i:s", $time_span_tmp%3600)) ;

			$fp2  = fsockopen ($from, 12084, $errno, $errstr, 30);
			$data2  = "TCP 02 DATA  macaddr \"".$mc."\" username \"".$un."\" ";
			$data2 .= "boot_time \"".$boot_time."\" time_span \"".$time_span."\" ";
			$data2 .= "cpu_usage \"".rand(0,100)."\" process \"".rand(3,200)."\" dwMemoryLoad \"".rand(0,100)."\" ";
			$data2 .= "dwTotalPhys \"".$dwTotalPhys."\" dwAvailPhys \"".$dwAvailPhys."\" ";
			$data2 .= "dwTotalVirtual \"".$dwTotalVirtual."\" dwAvailVirtual \"".$dwAvailVirtual."\" ";
			$data2 .= "ulFreeBytesAvailable \"".$ulFreeBytesAvailable."\" ulTotalNumberOfBytes \"".$ulTotalNumberOfBytes."\" ";
			$data2 .= "ulTotalNumberOfFreeBytes \"".$ulTotalNumberOfFreeBytes."\" ";
			$data2 .= "in_packet \"".$in_packet."\" out_packet \"".$out_packet."\" connections \"".$connections."\"";
			fputs ($fp2, $data2);
			while (!feof($fp2))
			{
				echo fgets ($fp2,128);
			}
			fclose ($fp2);
		}
		echo "… ". $i."Client(s) done!\n\r";
	}

}
?>
