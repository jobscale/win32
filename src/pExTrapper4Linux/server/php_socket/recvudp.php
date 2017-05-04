<?php

$ifc = exec("ifconfig |grep ブロードキャスト:");
$ifcd = preg_split("/\s*[ :]\s*/", $ifc, 10);
$from = $ifcd[2];
echo $from;
$port = 12055;

$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
if (!socket_set_option($socket, SOL_SOCKET, SO_BROADCAST, 1))
{
   echo 'ソケットのオプションを設定できません: '
      . socket_strerror(socket_last_error()) . PHP_EOL;
}

socket_bind($socket, $from, $port);
for (;;)
{
   socket_recvfrom($socket, $buf, 5000, 0, $from, $port);
   echo $buf."\n";
}
socket_close($socket);


?>

