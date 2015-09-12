<?php
//include
include_once('includes/init.php');



//show
$gSmarty->assign('is_log_in',is_logged_in());


$gSmarty->display("index.tpl");
?>
