<?php
//The Server
error_reporting(E_ALL);

$ifc = exec("ifconfig |grep ブロードキャスト:");
$ifcd = preg_split("/\s*[ :]\s*/", $ifc, 10);
$address = $ifcd[2];
$port = 12055;

$sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

if (!socket_bind($sock, $address, $port))
{
    echo 'バインドできません: '
       . socket_strerror(socket_last_error()) . PHP_EOL;
    die();
}

socket_listen($sock, 5);

for (;;)
{
    $client = socket_accept($sock);

    $input = socket_read($client, 2048);
    $value = json_decode($input);
    print_r($value);

    $output = "OK";
    socket_write($client, $output, strlen($output));
    
    socket_close($client);

    sleep(5);
}

socket_close($sock);

?>

