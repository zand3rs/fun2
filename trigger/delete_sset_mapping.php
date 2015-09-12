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
$umaster =& new SsetMapping();

$allfields = $umaster->getTableFields();
//chk submit
if($_GET['uid'] and $_GET['id']>0)
{
	$ret = $umaster->Delete($_GET['id']);	
	if(!$ret)
	{
		$gSmarty->assign('error_msg', "Delete SSET Mapping Failed.");
	}
	else
	{
		//lookup
		$soption    =& new Select_Options_Master();
		$soptstat   = $soption->getStatusSearch();
		$gSmarty->assign('soptstatus', $soptstat );
		
		//init
		$gSmarty->assign('is_log_in' , is_logged_in());
		$gSmarty->assign('is_searched', 0 );
		$gSmarty->assign('error_msg', "Delete SSET Mapping Successfully Completed.");
		$gSmarty->display('list_sset_mapping.tpl');
		exit;
	}
	
}
//set if update buttons
$edit_delete_enable  = (true) ? (1) : (0);
$gSmarty->assign('edit_delete_enable'    , $edit_delete_enable);

//get fields
$allfields  = $umaster->getTableFields();

//lookup
$soption    =& new Select_Options_Master();
$soptstat   = $soption->getStatusSearch();
$gSmarty->assign('soptstatus', $soptstat);

//show list
$gSmarty->assign('is_log_in',is_logged_in() );
$gSmarty->assign('list_total',$listdata['total']);
$gSmarty->assign('list_data', $listdata['data'] );
$gSmarty->assign('list_nav', $listdata['links'] );
$gSmarty->display("list_sset_mapping.tpl");
?>
