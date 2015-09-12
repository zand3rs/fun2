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
include_once('is_allowed.php');


//new
$umaster =& new Zones();
$allfields = $umaster->getTableFields();

//chk submit
if(strtoupper($_POST['btnSubmit']) == "SAVE")
{
	//valid
	$chkfields = $umaster->CheckFields($_POST);	
	if($chkfields['error'])
	{
	    $gSmarty->assign('error_msg', $chkfields['msg']);
	}
	else
	{
		//ok 2 save
		if(isset($_POST))
		{
			if(null != $allfields)
			{
			   $ret = $umaster->Save($allfields);	
			   if($ret['error'])
			   {
			     $gSmarty->assign('error_msg', SAVING_ZONE_FAILED);
			   }
			   else
			   {
			     $gSmarty->assign('error_msg', SAVING_ZONE_SUCCESS);
			     redir_page('list_zones.php');
			     exit;

			   }
			}
		}
		
	}
	
}

//show
//$_SESSION["timeout"] = time();
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('master_fields', $allfields);
$gSmarty->assign('form_title',    'Zone Maintenance');
$gSmarty->assign('form_action',   'add_zone.php');
$gSmarty->assign('form_submit',   'Save');
$gSmarty->display('add_master.tpl');
?>
