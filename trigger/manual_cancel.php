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
		if(!@preg_match("/^639[0-9]{9}$/i", $_POST['q_msisdn']) or (strlen($_POST['q_msisdn']) == 0) )
		{
			$gSmarty->assign('error_msg', "FUN DEACTIVATION ERROR: Invalid OFW SIM.");
		} 
                elseif($umaster->isValidGlobe($_POST['q_msisdn']) == 0)
                {
                        $gSmarty->assign('error_msg', "FUN DEACTIVATION ERROR: Invalid OFW SIM.");
                }
		elseif(!@preg_match("/^639[0-9]{9}$/i", $_POST['q_linkto']) or (strlen($_POST['q_linkto']) == 0) )
		{
			$gSmarty->assign('error_msg', "FUN DEACTIVATION ERROR: Invalid LOCAL SIM.");
		} 
                elseif($umaster->isValidGlobe($_POST['q_linkto']) == 0)
                {
                        $gSmarty->assign('error_msg', "FUN DEACTIVATION ERROR: Invalid LOCAL SIM.");
                }
		elseif($umaster->isBlacklisted($_POST['q_msisdn']) == 1)
		{
			$gSmarty->assign('error_msg', "FUN DEACTIVATION ERROR: OFW SIM is blacklisted.");
		} 
		elseif($umaster->chkFunLocal($_POST['q_msisdn']) > 0)
		{
			$gSmarty->assign('error_msg', "FUN DEACTIVATION ERROR: The OFW SIM you entered is a LOCAL SIM.");
		} 
		elseif($umaster->chkFun($_POST['q_linkto']) > 0)
		{
			$gSmarty->assign('error_msg', "FUN DEACTIVATION ERROR: The LOCAL SIM you entered is an OFW SIM.");
		} 
		elseif($umaster->chkFun($_POST['q_msisdn']) == 0)
		{
			$gSmarty->assign('error_msg', "FUN DEACTIVATION ERROR: Subscriber is not an active fun member.");
		} 
		else
		{
		   $ret = $umaster->CancelSvc($_POST['q_msisdn'], $_POST['q_linkto'], $_POST['q_profile'], $_POST['q_remarks']);	
		   if($ret['error'])
		   {
			$errwords = split(" ", $ret['error_msg']);
			$ukerr = 0;
			foreach ($errwords as $UkMsg){
				if ($UkMsg=="ORA-00001:")
					$ukerr = 1;
			}
			if ($ukerr == 1)
				$gSmarty->assign('error_msg', "FUN DEACTIVATION ERROR: Duplicate error");
			else
			     $gSmarty->assign('error_msg', "FUN DEACTIVATION ERROR");
		   }
		   else
		   {
			$gSmarty->assign('error_msg', "FUN DEACTIVATION Successfully entered.");
		   }
		}
	}
	
}

//show
$gSmarty->assign('is_searched',($btnSubmit == 'SAVE' ? 1 : 0) );
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('form_title',    'Manual FUN Deactivation');
$gSmarty->assign('form_action',   'manual_cancel.php');

//show-it
$gSmarty->display('manual_cancel.tpl');

?>
