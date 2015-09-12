<?php
//include
include_once('includes/init.php');

define("ERROR_SESSION_EXPIRED",    "Sorry, your session was timed-out.");

//show
$gSmarty->assign('is_log_in',is_logged_in());
$gSmarty->assign('error_msg', ERROR_SESSION_EXPIRED);


$gSmarty->display("index.tpl");
?>
