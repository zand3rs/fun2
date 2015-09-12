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


//chk submit
if(strtoupper($_POST['btnSubmit']) == "UPDATE")
{
        //valid
	$gSmarty->assign('id',$_POST['id'] );
	$chkfields = $umaster->CheckFields($_POST,1);	
	
	
	if($chkfields['error'])
	{
	    $gSmarty->assign('error_msg', $chkfields['msg']);
		$allfields = $umaster->getTableFields4Update($_POST['id']);
		$postf     = $umaster->Get($_POST['id']);
		$gSmarty->assign('id',$_POST['id'] );
	}
	else
	{
		//ok 2 save
		if(isset($_POST))
		{

		
			//get fields
		        $allfields   = $umaster->getTableFields();
		        $allfields[] = array('name' => 'id', 'value'=> $_POST['id']);
			if(null != $allfields)
			{
			   $ret = $umaster->Update($allfields);	
			   if($ret['error'])
			   {
			     $gSmarty->assign('error_msg', "User Update Failed.");
			   }
			   else
			   {
				$soption =& new Select_Options_Master();
				$srole_codes = $soption->getRoleMaster2();
				$gSmarty->assign('is_log_in' , is_logged_in() );
				$gSmarty->assign('srole_code', $srole_codes);
				$gSmarty->assign('is_searched', 0 );
				$gSmarty->assign('error_msg', "Update Successfully Completed.");
				$gSmarty->display('list_user_master.tpl');
				exit;
			   }
			}
		}
		
	}
	
}
else
{
	//view only
	$allfields = $umaster->getTableFields4Update($_GET['id']);
	$postf     = $umaster->Get($_GET['id']);
	$gSmarty->assign('id',$_GET['id'] );
	if($postf == null)
	{
	   $gSmarty->assign('error_msg', UPDATE_USER_FAILED_NO_RECORD_FOUND); 
	}

}

//show
//$_SESSION["timeout"] = time();
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('master_fields', $allfields);
$gSmarty->assign('form_title',    "Web User Maintenance");
$gSmarty->assign('form_action',   "update_user_master.php");
$gSmarty->assign('form_submit',   "Update");
$gSmarty->assign('form_primary',  "id");
$gSmarty->display('update_master.tpl');
?>
