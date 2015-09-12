<?php
//include
include_once('includes/init.php');

$is_logged_in = is_logged_in() ;
//chk
if(! $is_logged_in)
{
    redir_page("index.php");
    exit;
}

//allowed?
include_once('is_allowed.php');


//new
//$umaster =& new User_Master();
$umaster =& new User();

//view only
$allfields = $umaster->getTableFields4Update($_GET['id']);
$postf     = $umaster->Get($_GET['id']);
$gSmarty->assign('id',$_GET['id'] );
if($postf == null)
{
   $gSmarty->assign('error_msg', UPDATE_USER_FAILED_NO_RECORD_FOUND); 
}


//show
//show
$gSmarty->assign('is_log_in',      $is_logged_in );
$gSmarty->assign('master_fields',  $allfields);
$gSmarty->assign('form_title',     "User Details");
$gSmarty->display('details_master.tpl');
?>
