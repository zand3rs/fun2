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
$umaster =& new ACL();
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
				$gSmarty->assign('error_msg', "Add ACL Failed.");
			   }
			   else
			   {
				$soption =& new Select_Options_Master();
				$srole_codes = $soption->getRoleMaster2();
				$smod_codes  = $soption->getModuleMaster2();
				$gSmarty->assign('is_log_in' , $is_logged_in);
				$gSmarty->assign('is_searched', 0 );
				$gSmarty->assign('srole_code', $srole_codes);
				$gSmarty->assign('smod_code', $smod_codes);
				$gSmarty->assign('error_msg', "Add ACL Successfully Completed.");
				$gSmarty->display('list_acl_master.tpl');
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
$gSmarty->assign('form_title',    'Web ACL Maintenance');
$gSmarty->assign('form_action',   'add_acl_master.php');
$gSmarty->assign('form_submit',   'Save');
$gSmarty->display('add_master.tpl');

?>
