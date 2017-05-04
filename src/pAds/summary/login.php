<?php
session_start();
define("USERID", "psuser");
define("PASSWORD", "pextrapads");

if(isset($_POST["login"])){
   
    if(USERID === $_POST["userid"] && PASSWORD === $_POST["password"]){
        // 暗号化してセッションに保存
        $_SESSION["SUM_USERID"] = md5(USERID);
        $_SESSION["SUM_PASSWORD"] = md5(PASSWORD);
        header("Location:admin.php");
       
    }else{
        // セッション破棄
        session_destroy();
        headhtml();
        message();
    }
}else{
    headhtml();
}

bodyhtml();
 
function headhtml(){
print <<<ENDOFPRT
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>pads summary login</title>
ENDOFPRT;
$ip = $_SERVER["REMOTE_ADDR"];
$host = gethostbyaddr($ip);
printf("%s (%s)<br />\n", $ip, $host);
}

function message(){
    print "<font color=red>ユーザIDまたはパスワードが違います</font><br />";
}

function bodyhtml(){
print <<<ENDOFPRT
<br />
<table><tr><td>
<div align=right>
<form action="" method="post">
ユーザID <input name="userid" type="text" value="" /><br />
パスワード <input name="password" type="password" value="" /><br />
<button type="submit" name="login">ログイン</button>
</form>
</div>
</td></tr></table>
ENDOFPRT;
}

?>

