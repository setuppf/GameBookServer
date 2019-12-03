<?php
include "./mysql_config.php";
header("Content-type: text/html; charset=utf-8");

/*
传3个参数
$_POST['account'] = 帐号
$_POST['password'] = 密码

返回1个参数
0=正确
1=数据库操作错误
2=用户名错误
3=密码错误
*/

$sql = "SELECT password
				FROM
					".$dbsource['dbname'].".account
				WHERE
					account = '".trim($_POST['account'])."'
";
#echo $sql;
$rs = mysql_query($sql,$db);
if(mysql_error()){
	echo '{"returncode":1}';
	exit;
}

$row = mysql_fetch_array($rs);
if(mysql_num_rows($rs) >= 1){
	if($row['password'] == $_POST['password']){
		echo '{"returncode":0}';
	}else{
		echo '{"returncode":3}';
	}
}else{
	echo '{"returncode":2}';
}
?>
