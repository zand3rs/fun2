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
$umaster =& new User();
$umaster->Open($_SESSION["session.smartygold.profile.user"]);
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
				$gSmarty->assign('error_msg', "Add Account Failed.");
			   }
			   else
			   {
				$soption =& new Select_Options_Master();
				$srole_codes = $soption->getRoleMaster2();
				$gSmarty->assign('is_log_in' , is_logged_in() );
				$gSmarty->assign('srole_code', $srole_codes);
				$gSmarty->assign('is_searched', 0 );
				$gSmarty->assign('error_msg', "Add Account Successfully Completed.");
				$gSmarty->display('list_user_master.tpl');
				exit;
			   }
			}
		}
		
	}
	
}

//show
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('master_fields', $allfields);
$gSmarty->assign('master_ctr', 0);
$gSmarty->assign('form_title',    'Web User Maintenance');
$gSmarty->assign('form_action',   'add_user_master.php');
$gSmarty->assign('form_submit',   'Save');
$gSmarty->display('add_master.tpl');
?>
