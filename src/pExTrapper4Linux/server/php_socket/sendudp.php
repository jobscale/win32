<?php

$ifc = exec("ifconfig |grep ブロードキャスト:");
$ifcd = preg_split("/\s*[ :]\s*/", $ifc, 10);
$from = $ifcd[4];
$port = 12055;

$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
if (!socket_set_option($socket, SOL_SOCKET, SO_BROADCAST, 1))
{
   echo 'ソケットのオプションを設定できません: '
      . socket_strerror(socket_last_error()) . PHP_EOL;
}

$msg = date("r");
socket_sendto($socket, $msg, strlen($msg), 0, $from, $port);

socket_close($socket);

echo "OK";

?>

