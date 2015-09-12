<?php
//include
include_once('includes/init.php');

//chk
//chk
if(! is_logged_in())
{
    redir_page("index.php");
    exit;
}

//allowed?
include_once('is_allowed.php');


//new
$umaster =& new User();
$umaster->Open($_SESSION["session.smartygold.profile.user"]);
$allfields = $umaster->getTableFields();
//chk submit
if($_GET['uid'] and $_GET['id']>0)
{
	$ret = $umaster->Delete($_GET['id']);	
	if(!$ret)
	{
		$gSmarty->assign('error_msg', "Delete Account Failed.");
	}
	else
	{
		$soption =& new Select_Options_Master();
		$srole_codes = $soption->getRoleMaster2();
		$gSmarty->assign('is_log_in' , is_logged_in() );
		$gSmarty->assign('srole_code', $srole_codes);
		$gSmarty->assign('is_searched', 0 );
		$gSmarty->assign('error_msg', "Delete Account Successfully Completed.");
		$gSmarty->display('list_user_master.tpl');
		exit;
	}
	
}
//show list
$listdata  = $umaster->getList();
$gSmarty->assign('is_log_in',is_logged_in() );
$gSmarty->assign('list_total',$listdata['total']);
$gSmarty->assign('list_data', $listdata['data'] );
$gSmarty->assign('list_nav', $listdata['links'] );
$gSmarty->display('list_user_master.tpl');
?>
