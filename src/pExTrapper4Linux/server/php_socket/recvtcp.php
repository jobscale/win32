<?php
//The Server
error_reporting(E_ALL);

$ifc = exec("ifconfig |grep 255 |head -1");
$ifcd = preg_split("/\s*[ :]\s*/", $ifc, 10);
$address = $ifcd[2];
$port = 12055;

echo "wait for ".$address.":".$port."\n";

$sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

socket_bind($sock, $address, $port);

socket_listen($sock, 5);

$no=0;

for (;;)
{
    $client = socket_accept($sock);

    $input = socket_read($client, 2048);
    $no++;
    echo "No. ".$no."\n";
    echo "$input";

    $output = "OK";
    socket_write($client, $output, strlen($output));
    
    socket_close($client);

    sleep(5);
}

socket_close($sock);

?>

