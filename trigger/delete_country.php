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
$umaster =& new Countries();

$allfields = $umaster->getTableFields();
//chk submit
if($_GET['uid'] and $_GET['id']>0)
{
	$ret = $umaster->Delete($_GET['id']);	
	if(!$ret)
	{
		$gSmarty->assign('error_msg', "Delete Country Failed.");
	}
	else
	{
		//lookup
		$soption    =& new Select_Options_Master();
		$soptstat   = $soption->getStatusSearch();
		$soptzone   = $soption->getZones();
		$gSmarty->assign('soptstatus', $soptstat);
		$gSmarty->assign('soptzones', $soptzone);
		//init
		$gSmarty->assign('is_log_in' , is_logged_in() );
		$gSmarty->assign('is_searched', 0 );
		$gSmarty->assign('error_msg', "Delete Country Successfully Completed.");
		$gSmarty->display('list_countries.tpl');
		exit;
	}
	
}
//set if update buttons
$edit_delete_enable  = (true) ? (1) : (0);
$gSmarty->assign('edit_delete_enable'    , $edit_delete_enable);

//lookup
$soption    =& new Select_Options_Master();
$soptstat   = $soption->getStatusSearch();
$soptzone   = $soption->getZones();
$gSmarty->assign('soptstatus', $soptstat);
$gSmarty->assign('soptzones', $soptzone);

//show list
//$listdata  = $umaster->getList();
$gSmarty->assign('is_log_in',is_logged_in() );
$gSmarty->assign('list_total',$listdata['total']);
$gSmarty->assign('list_data', $listdata['data'] );
$gSmarty->assign('list_nav', $listdata['links'] );
$gSmarty->display("list_countries.tpl");
?>
