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
$umaster =& new Services();

//view only
$allfields = $umaster->getTableFields4Update($_GET['id']);
$gSmarty->assign('id',$_GET['id'] );
$umaster->viewServiceDetail($_GET['svcode']);

//show
//show
$gSmarty->assign('is_log_in',      $is_logged_in );
$gSmarty->assign('master_fields',  $allfields);
$gSmarty->assign('form_title',     "Service Details");
$gSmarty->display('details_master.tpl');
?>
