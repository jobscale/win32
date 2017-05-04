<?php

//global $_LIB_DIR_;
class Base64
{
	public static function encode($data, $isStandard = false)
	{
		$cvt = self::$customizeCvt;
		$ret = array();
		$len = is_array($data) ? count($data) : strlen($data);
		for ($i = 0; $i < $len; ++$i)
		{
			$c = (ord($data[$i]) >> 2) & 0x3f;
			$ret[] = $cvt[$c];
			$c = (ord($data[$i]) << 4) & 0x3f;
			if (++$i < $len)
			{
				$c |= (ord($data[$i]) >> 4) & 0x0f;
			}
			$ret[] = $cvt[$c];
			if ($i < $len)
			{
				$c = (ord($data[$i]) << 2) & 0x3f;
				if (++$i < $len)
				{
					$c |= (ord($data[$i]) >> 6) & 0x03;
				}
				$ret[] = $cvt[$c];
			}
			else
			{
				++$i;
				$ret[] = self::$fillchar;
			}

			if ($i < $len)
			{
				$c = ord($data[$i]) & 0x3f;
				$ret[] = $cvt[$c];
			}
			else
			{
				$ret[] = self::$fillchar;
			}
		}
		return is_array($data) ? $ret : implode('', $ret);
	}

	public static function decode($data, $isStandard = false)
	{
		$cvt = self::$customizeCvt;
		$ret = array();
		$len = is_array($data) ? count($data) : strlen($data);
		for ($i = 0; $i < $len; ++$i)
		{
			$c = strpos($cvt, $data[$i]);
			++$i;
			$c1 = strpos($cvt, $data[$i]);
			$c = (($c << 2) | (($c1 >> 4) & 0x3));
			$ret[] = chr($c);
			if (++$i < $len)
			{
				$c = ord($data[$i]);
				if (self::$fillchar == chr($c))
					break;

				$c = strpos($cvt, chr($c));
				$c1 = (($c1 << 4) & 0xf0) | (($c >> 2) & 0xf);
				$ret[] = chr($c1);
			}

			if (++$i < $len)
			{
				$c1 = ord($data[$i]);
				if (self::$fillchar == chr($c1))
				{
					break;
				}

				$c1 = strpos($cvt, chr($c1));
				$c = (($c << 6) & 0xc0) | $c1;
				$ret[] = chr($c);
			}
		}
		return is_array($data) ? $ret : implode('', $ret);
	}

	private static $fillchar = "!";

	private static $customizeCvt =
	"/+lJEjU6zQd1MghIB9RkpnDSeTuao2WKy0CwP7vZq85A4HLONfimcGb3trFsXVYx";
}

$ana_dir = 'C:\\Documents and Settings\\All Users\\Application Data\\pLook\pAnalyze\\';

if($argv[1] != '' && $argv[2] != '')
{
	$target_data = $argv[1];
	$target_date = $argv[2];

	if($target_data == 'a')
	{
		$ext  = ".pan";
		$head = "anapro_";
	}
	else
	{
		$ext  = ".fan";
		$head = "anafore_";
	}

	$ana_file = $ana_dir . $head.$target_date.$ext;
}
else
{
	echo <<< _HELP_
使用方法
$argv[0] [対象データ(a|an)] [対象日(YYYYMMDD)] [送信済みデータ(b)←未対応]
　対象データ：
　　旧版anaデータ：a
　　新版anaデータ：an
　対象日：
　　YYYYMMDD形式
　分析可能ファイル一覧\r\n
_HELP_;
//　送信済みデータ：
//　　送信済みデータを参照の場合は「b」を指定
//　分析可能ファイル一覧\r\n
//_HELP_;

	$d = dir($ana_dir);
	while (false !== ($entry = $d->read()))
	{
		if(count(split ("_", $entry)) == 2)
			echo "　　".$entry."\n";
	}
	$d->close();
/*
	echo "　送信済みデータ一覧：\n";
	$d = dir($ana_dir);
	while (false !== ($entry = $d->read()))
	{
		if(count(split ("_", $entry)) == 5)
			echo "　　".$entry."\n";
	}
	$d->close();
*/
	exit;
}

if(file_exists($ana_file))
{
  $ana_list = file($ana_file);
  $ana_list_count = count($ana_list);
  for( $i = 0; $i < $ana_list_count; $i++ )
  {
    $line = $ana_list[$i];
    $line = str_replace ("\r" , "" ,$line );
    $line = str_replace ("\n" , "" ,$line );

		$head = Base64::decode($line);
		echo $head;
  }
}

exit;
?>
