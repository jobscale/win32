<?php
//The Client
error_reporting(E_ALL);

$ifc = exec("ifconfig |grep ブロードキャスト:");
$ifcd = preg_split("/\s*[ :]\s*/", $ifc, 10);
$address = $ifcd[2];
$port = 12055;

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket === false) {
    echo "socket_create() failed: reason: "
        . socket_strerror(socket_last_error()) . "\n";
}

$result = socket_connect($socket, $address, $port);
if ($result === false) {
    echo "socket_connect() failed.\nReason: ($result) "
        . socket_strerror(socket_last_error($socket)) . "\n";
}

$msg = date("r");
socket_write($socket, $msg, strlen($msg));
    
$input = socket_read($socket, 2048);

echo "$input\n";

socket_close($socket);

?>

