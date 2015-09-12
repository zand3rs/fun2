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
$btnSubmit   = strtoupper($_POST['btnSubmit']);

//chk submit
if($btnSubmit == "ACTIVATE")
{
	//valid
	//ok 2 save
	if(isset($_POST))
	{
		if(!@preg_match("/^639[0-9]{9}$/i", $_POST['q_msisdn']) or (strlen($_POST['q_msisdn']) == 0) )
		{
			$gSmarty->assign('error_msg', "FUN ACTIVATION ERROR: Invalid OFW SIM.");
		} 
                elseif($umaster->isValidGlobe($_POST['q_msisdn']) == 0)
                {
                        $gSmarty->assign('error_msg', "FUN ACTIVATION ERROR: Invalid OFW SIM.");
                }
		elseif(!@preg_match("/^639[0-9]{9}$/i", $_POST['q_linkto']) or (strlen($_POST['q_linkto']) == 0) )
		{
			$gSmarty->assign('error_msg', "FUN ACTIVATION ERROR: Invalid LOCAL SIM.");
		} 
                elseif($umaster->isValidGlobe($_POST['q_linkto']) == 0)
                {
                        $gSmarty->assign('error_msg', "FUN ACTIVATION ERROR: Invalid LOCAL SIM.");
                }
		elseif($umaster->isBlacklisted($_POST['q_msisdn']) == 1)
		{
			$gSmarty->assign('error_msg', "FUN ACTIVATION ERROR: OFW SIM is blacklisted.");
		} 
		elseif($umaster->isBlacklisted($_POST['q_linkto']) == 1)
		{
			$gSmarty->assign('error_msg', "FUN ACTIVATION ERROR: LOCAL SIM is blacklisted.");
		} 
		elseif($umaster->chkFunMaxLinkReached($_POST['q_msisdn']) == 1)
		{
			$gSmarty->assign('error_msg', "FUN ACTIVATION ERROR: Subscriber Exceed Max Number of links.");
		} 
		elseif($umaster->chkPendingRequest($_POST['q_msisdn'],'LINK') >= 1)
		{
			$gSmarty->assign('error_msg', "FUN ACTIVATION ERROR: Already has a pending request.");
		} 
		else
		{
                   $nBalChk  = 0; //(isset($_POST['q_balchk'])) ? 1 : 0;
		   $ret = $umaster->FunActivation($_POST['q_msisdn'], $_POST['q_linkto'], $_POST['q_remarks'], $nBalChk);	
		   if($ret['error'])
		   {
			$errwords = split(" ", $ret['error_msg']);
			$ukerr = 0;
			foreach ($errwords as $UkMsg){
				if ($UkMsg=="ORA-00001:")
					$ukerr = 1;
			}
			if ($ukerr == 1)
				$gSmarty->assign('error_msg', "FUN ACTIVATION ERROR: Already has a pending request.");
			else
			     $gSmarty->assign('error_msg', "FUN ACTIVATION ERROR");
		   }
		   else
		   {
			$gSmarty->assign('error_msg', "FUN ACTIVATION successfully entered.");
		   }
		}
	}
	
}

//show
$gSmarty->assign('is_searched',($btnSubmit == 'ACTIVATE' ? 1 : 0) );
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('form_title',    'Manual Fun Activation');
$gSmarty->assign('form_action',   'manual_fun_activation.php');

//show-it
$gSmarty->display('manual_fun_activation.tpl');

?>
