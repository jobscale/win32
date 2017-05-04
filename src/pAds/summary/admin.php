<?php
session_start();

if(isset($_POST['logout'])){
 
// 一時ファイルの消去
system("rm /tmp/acce-dl.txt");
system("rm /tmp/acce-dlout.txt");

// セッション変数を全て解除する
$_SESSION = array();
 
// セッションを切断するにはセッションクッキーも削除する。
// Note: セッション情報だけでなくセッションを破壊する。
if (ini_get("session.use_cookies")) {
    $params = session_get_cookie_params();
    setcookie(session_name(), '', time() - 42000,
        $params["path"], $params["domain"],
        $params["secure"], $params["httponly"]
    );
}
 
// セッションを破壊してリダイレクト
session_destroy();
header("Location:login.php");
}

// ログイン処理
define("USERID", "psuser");
define("PASSWORD", "pextrapads");

if(isset($_SESSION["SUM_USERID"]) && $_SESSION["SUM_PASSWORD"] && md5(USERID) === $_SESSION["SUM_USERID"] && md5(PASSWORD) === $_SESSION["SUM_PASSWORD"]){
    headhtml();
    bodyhtml();
       
}else{
    // セッション破棄
    session_destroy();
    headhtml();
    print "<font color=red>ログインしてください</font><br />";
    print "<a href=login.php>ログイン</a><br />";
    exit;
}

if(isset($_POST['acc_login'])){
    logprt1();
}else if(isset($_POST['acc_ver'])){
    logprt2();
}else if(isset($_POST['acc_update'])){
    logprt3();
}else if(isset($_POST['thislog'])){
    logprt4();
}else if(isset($_POST['machines'])){
    logprt5();
}else if(isset($_POST['shops'])){
    logprt6();
}

function headhtml(){
print <<<ENDOFPRT
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>pads summary</title>
ENDOFPRT;
$ip = $_SERVER["REMOTE_ADDR"];
$host = gethostbyaddr($ip);
printf("%s (%s)<br />\n", $ip, $host);
}

function bodyhtml(){
print <<<ENDOFPRT
<br />
<form action="" method="post">
<button type="submit" name="acc_login">起動ログ</button>
<button type="submit" name="acc_ver">チェックログ</button>
<button type="submit" name="acc_update">アップログ</button>
<button type="submit" name="logout">ログアウト</button>
&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp<button type="submit" name="thislog">詳細</button>
&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp<button type="submit" name="machines">machines</button>
<button type="submit" name="shops">shops</button>
</form>
ENDOFPRT;
}

function logprt1(){
system("cat /var/log/apache2/access-dl.log.1 >/tmp/acce-dl.txt");
system("cat /var/log/apache2/access-dl.log >> /tmp/acce-dl.txt");
system('grep check.php /tmp/acce-dl.txt | grep -v 00:21:6A:15:C8:1A | awk \'{ split(substr($7, 37, 88), a, /[=&]/); printf("%16s %16s %s %s\n", $1, a[2], a[4], a[6]) }\' | sort -nk1 | uniq -c > /tmp/acce-dlout.txt');
printf("first check log&nbsp&nbsp&nbsp&nbsp&nbsp\n\n");
system("cat /tmp/acce-dlout.txt | awk '{sum=sum+$1}END{print sum}'");
printf("count / machine ");
system("cat /tmp/acce-dlout.txt | awk '{print $2, $3, $4}' | uniq -c | wc -l");
printf("<br /><pre>\n");
system("cat /tmp/acce-dlout.txt");
printf("</pre>\n");
}

function logprt2(){
system("cat /var/log/apache2/access-dl.log.1 >/tmp/acce-dl.txt");
system("cat /var/log/apache2/access-dl.log >>/tmp/acce-dl.txt");
system('grep update.php /tmp/acce-dl.txt | grep -v 00:21:6A:15:C8:1A | awk \'{ split(substr($7, 76, 88), a, /[=&]/); printf("%16s %16s %s %s\n", $1, a[2], a[4], a[6]) }\' | sort -nk1 | uniq -c > /tmp/acce-dlout.txt');
printf("version check log &nbsp&nbsp&nbsp&nbsp&nbsp\n\n");
system("cat /tmp/acce-dlout.txt | awk '{sum=sum+$1}END{print sum}'");
printf("count / machine ");
system("cat /tmp/acce-dlout.txt | awk '{print $2, $3, $4}' | uniq -c | wc -l");
printf("<br /><pre>\n");
system("cat /tmp/acce-dlout.txt");
printf("</pre>\n");
}

function logprt3(){
system("cat /var/log/apache2/access-dl.log.1 >/tmp/acce-dl.txt");
system("cat /var/log/apache2/access-dl.log >>/tmp/acce-dl.txt");
system('grep All.exe /tmp/acce-dl.txt | grep -v 00:21:6A:15:C8:1A | awk \'{ split(substr($7, 50, 88), a, /[=&]/); printf("%16s %16s %s %s\n", $1, a[2], a[4], a[6]) }\' | sort -nk1 | uniq -c > /tmp/acce-dlout.txt');
printf("update log &nbsp&nbsp&nbsp&nbsp&nbsp\n\n");
system("cat /tmp/acce-dlout.txt | awk '{sum=sum+$1}END{print sum}'");
printf("count / machine ");
system("cat /tmp/acce-dlout.txt | awk '{print $2, $3, $4}' | uniq -c | wc -l");
printf("<br /><pre>\n");
system("cat /tmp/acce-dlout.txt");
printf("</pre>\n");
}

function logprt4(){
system("cat /var/log/apache2/access-dl.log.1 >/tmp/acce-dl.txt");
system("cat /var/log/apache2/access-dl.log >>/tmp/acce-dl.txt");
system('grep /pads/summary/ /tmp/acce-dl.txt | grep -v -e Safari/534.55.3 -e Safari/534.52.7 > /tmp/acce-dlout.txt');
printf("this log&nbsp&nbsp&nbsp&nbsp&nbsp\n\n");
printf("<br /><pre>\n");
system("cat /tmp/acce-dlout.txt");
system("echo");
system("echo");
system("tail -20 /var/log/apache2/access-dl.log");
printf("</pre>\n");
}

function logprt5(){
system("cat /var/log/apache2/access-dl.log.1 >/tmp/acce-dl.txt");
system("cat /var/log/apache2/access-dl.log >>/tmp/acce-dl.txt");
system('grep check.php /tmp/acce-dl.txt | grep -v 00:21:6A:15:C8:1A | awk \'{ split(substr($7, 37, 88), a, /[=&]/); if (a[2] != "" && a[4] != ""){printf("%16s %16s %s\n", $1, a[2], a[4])} }\' | sort -nk1 | uniq > /tmp/acce-dlout.txt');
printf("machines&nbsp&nbsp&nbsp&nbsp&nbsp\n\n");
printf("machine ");
system("cat /tmp/acce-dlout.txt | wc -l");
printf("<br /><pre>\n");
system("cat /tmp/acce-dlout.txt");
printf("</pre>\n");
}

function logprt6(){
system("cat /var/log/apache2/access-dl.log.1 >/tmp/acce-dl.txt");
system("cat /var/log/apache2/access-dl.log >>/tmp/acce-dl.txt");
system('grep check.php /tmp/acce-dl.txt | grep -v 00:21:6A:15:C8:1A | awk \'{ printf("%16s\n", $1) }\' | sort -nk1 | uniq -c | sort -nrk1 > /tmp/acce-dlout.txt');
printf("shops&nbsp&nbsp&nbsp&nbsp&nbsp\n\n");
printf("shop ");
system("cat /tmp/acce-dlout.txt | wc -l");
printf("<br /><pre>\n");
system("cat /tmp/acce-dlout.txt");
printf("</pre>\n");
}

?>

