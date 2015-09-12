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
$umaster =& new Services();

$allfields = $umaster->getTableFields();
//chk submit
if($_GET['uid'] and $_GET['id']>0)
{
	$ret = $umaster->Delete($_GET['id']);	
        if($ret['error'])
        {
                $errwords = split(" ", $ret['error_msg']);
                $ukerr = 0;
                foreach ($errwords as $UkMsg){
                        if ($UkMsg=="ORA-02292:")
                                $ukerr = 1;
                }
                if ($ukerr == 1)
                        $gSmarty->assign('error_msg', DELETE_FAILED_INTEGRITY_CONSTRAINT_VIOLATED);
                else
			$gSmarty->assign('error_msg', DELETE_SERVICE_FAILED);
	}
	else
	{
		$gSmarty->assign('error_msg', DELETE_SERVICE_SUCCESS);
	}
	
}
//set if update buttons
$edit_delete_enable  = (true) ? (1) : (0);
$gSmarty->assign('edit_delete_enable'    , $edit_delete_enable);
$soption    =& new Select_Options_Master();
$soptstat   = $soption->getStatusSearch();
$gSmarty->assign('soptstatus', $soptstat);

//show list
//$listdata  = $umaster->getList();
$gSmarty->assign('is_log_in',is_logged_in() );
$gSmarty->assign('list_total',$listdata['total']);
$gSmarty->assign('list_data', $listdata['data'] );
$gSmarty->assign('list_nav', $listdata['links'] );
$gSmarty->display("list_services.tpl");
?>
