<?php
$dbsource['account']['address'] = '127.0.0.1';
$dbsource['account']['username'] = 'root';
$dbsource['account']['password'] = '123456';

$dbsource['dbname'] = 'account';

$db = mysql_connect($dbsource['account']['address'],$dbsource['account']['username'],$dbsource['account']['password']);
mysql_select_db($dbsource['dbname'],$db);
mysql_query('SET NAMES UTF8');
?>
