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
$umaster =& new Enrollment();
$soption    =& new Select_Options_Master();
$soptcust   = $soption->getCustomerTypesSearch();
$gSmarty->assign('soptcustomer',  $soptcust);
$btnSubmit   = strtoupper($_POST['btnSubmit']);


//chk submit
if($btnSubmit == "SAVE")
{
	//valid
	//ok 2 save
	if(isset($_POST))
	{

		if(
		  !@preg_match("/^[0-9]{12}$/i", $_POST['q_msisdn']) or
		  (strlen($_POST['q_msisdn']) == 0)
		  )
		{
			$gSmarty->assign('error_msg', INVALID_MSISDN_PARAMETER);
		} 
		elseif($umaster->isBlacklisted($_POST['q_msisdn']) == 1)
		{
			$gSmarty->assign('error_msg', MSISDN_IS_BLACKLISTED);
		} 
		else
		{
		   $ret = $umaster->RetrySvc($_POST['q_msisdn'], $_POST['q_profile'], $_POST['q_remarks']);	
		   if($ret['error'])
		   {
			$errwords = split(" ", $ret['error_msg']);
			$ukerr = 0;
			foreach ($errwords as $UkMsg){
				if ($UkMsg=="ORA-00001:")
					$ukerr = 1;
			}
			if ($ukerr == 1)
				$gSmarty->assign('error_msg', "RETRY Failed: Duplicate error");
			else
			     $gSmarty->assign('error_msg', "RETRY Failed");
		   }
		   else
		   {
			$gSmarty->assign('error_msg', SAVING_RETRY_SUCCESS);
		   }
		}
	}
	
}

//show
$gSmarty->assign('is_searched',($btnSubmit == 'SAVE' ? 1 : 0) );
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('form_title',    'Manual Retry of failed Transaction');
$gSmarty->assign('form_action',   'manual_retry.php');

//show-it
$gSmarty->display('manual_retry.tpl');

?>
