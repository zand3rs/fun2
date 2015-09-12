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
$umaster =& new Enrollment();

//view only
$postf     = $umaster->RetryFailedTran($_GET['tran_id']);
$allfields = $umaster->getTableFields4Update($_GET['tran_id']);
if($postf == null)
{
   $gSmarty->assign('error_msg', NO_RECORD_FOUND); 
}
else
{
   $gSmarty->assign('error_msg', TRANSACTION_IS_SET_FOR_RETRY); 
}

//show
//show
$gSmarty->assign('is_log_in',      $is_logged_in );
$gSmarty->assign('master_fields',  $allfields);
$gSmarty->assign('form_title',     "Transaction Details");
$gSmarty->display('details_master.tpl');
?>
