<?php
include "mysql_config.php";
header("Content-type: text/html; charset=utf-8");

/*
传入参数说明:
account:用户登录名
password:用户登陆密码(MD5后的密码)

输出参数说明:
returnCode-返回状态
1 操作成功
2 用户名被占用
3 数据库操作出错
returnMessage-返回状态说明

json传入例子:'{"account":"登陆名","password":"fae0b27c451c728867a567e8c1bb4e53"}'
json输入例子:'{"returnCode":"2","returnMessage":"用户名被占用,请重新选择"}'
*/

//step 1:检查注册的用户名是否存在
$sql_check = "SELECT
								account
							FROM
								".$dbsource['dbname'].".account
							WHERE
								account = '".trim($_POST['username'])."'
";
$rs_check = mysql_query($sql_check,$db);
if(mysql_num_rows($rs_check) >= 1){
	//用户名已被占用
	$json_return['returnCode'] = '2';
	$json_return['returnMessage'] = '用户名已被占用';
}else{
	
	//step 2:插入数据库
	//生成guid
	$charid = strtoupper(md5(uniqid(mt_rand(), true)));
	$hyphen = chr(45);// "-"
	$uuid = substr($charid, 0, 8).$hyphen.substr($charid, 8, 4).$hyphen.substr($charid,12, 4).$hyphen.substr($charid,16, 4).$hyphen.substr($charid,20,12);
	
	$sql_insert = "INSERT INTO ".$dbsource['dbname'].".9z_account
								SET
									id = '".$uuid."'
									,account = '".trim($_POST['username'])."'
									,password = '".$_POST['password']."'
	";
	$rs_insert = mysql_query($sql_insert,$db);
		
	if(!mysql_error()){
		$json_return['returnCode'] = '1';
		$json_return['returnMessage'] = '';
	}else{
		$json_return['returnCode'] = '3';
		$json_return['returnMessage'] = '数据库操作出错';
	}
}
echo json_encode($json_return);
?>
