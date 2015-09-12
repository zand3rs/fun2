<?php
//include
include_once('includes/init.php');

//chk
$is_logged_in = is_logged_in();
if(! $is_logged_in)
{
	redir_page("index.php");
	exit;
}

//allowed?
//include_once('is_allowed.php');

$csv      = trim($csv);
//get csv file here
$csv_file = basename("./log/$csv");


debug("FILE: $csv ====> $csv_file ");


@header("Content-Disposition: attachment; filename=" . urlencode($csv_file));    
@header("Content-Type: application/force-download");
@header("Content-Type: application/octet-stream");
@header("Content-Type: application/download");
@header("Content-Description: File Transfer");             
@header("Content-Length: " . @filesize("log/".$csv));
@flush(); // this doesn't really matter.

$fp = @fopen("./log/$csv", "r"); 
while (!feof($fp))
{
	    echo fread($fp, 2048576); 
                 flush(); // this is essential for large downloads
}  
@fclose($fp); 
?>
