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
			$gSmarty->assign('error_msg', "ACTIVATION ERROR: Invalid MSISDN.");
		} 
                elseif($umaster->isValidGlobe($_POST['q_msisdn']) == 0)
                {
                        $gSmarty->assign('error_msg', "ACTIVATION ERROR: Invalid MSISDN.");
                }
       		elseif(!@preg_match("/^(\d{4}-?\d{2}-?\d{2})$/i", $_POST['q_sdate']) or (strlen($_POST['q_sdate']) == 0) )
		{
			$gSmarty->assign('error_msg', "ACTIVATION ERROR: Invalid activation date.");
	        } 
         	elseif($umaster->isValidActivationDt($_POST['q_sdate']) == 0)
                {
                        $gSmarty->assign('error_msg', "ACTIVATION ERROR: Invalid activation date.");
                }
		elseif($umaster->isBlacklisted($_POST['q_msisdn']) == 1)
		{
			$gSmarty->assign('error_msg', "ACTIVATION ERROR: MSISDN is blacklisted.");
		} 
		elseif($umaster->chkRoamer($_POST['q_msisdn']) == 1)
		{
			$gSmarty->assign('error_msg', "ACTIVATION ERROR: Subscriber is still active roamer.");
		} 
		elseif($umaster->chkPendingRequest($_POST['q_msisdn'],'ACTIVATION') >= 1)
		{
			$gSmarty->assign('error_msg', "ACTIVATION ERROR: Already has a pending roaming request.");
		} 
		else
		{
                   $nBalChk  = (isset($_POST['q_balchk'])) ? 1 : 0;
                   $nBPeriod = (isset($_POST['q_bperiod'])) ? 1 : 0;
		   $ret = $umaster->Activation($_POST['q_msisdn'], $_POST['q_sdate'], $_POST['q_duration'], $_POST['q_remarks'], $nBalChk, $nBPeriod);	
		   if($ret['error'])
		   {
			$errwords = split(" ", $ret['error_msg']);
			$ukerr = 0;
			foreach ($errwords as $UkMsg){
				if ($UkMsg=="ORA-00001:")
					$ukerr = 1;
			}
			if ($ukerr == 1)
				$gSmarty->assign('error_msg', "ACTIVATION ERROR: Already has a pending roaming request.");
			else
			     $gSmarty->assign('error_msg', "ACTIVATION ERROR");
		   }
		   else
		   {
			$gSmarty->assign('error_msg', "ACTIVATION successfully entered.");
		   }
		}
	}
	
}

//show
$gSmarty->assign('is_searched',($btnSubmit == 'ACTIVATE' ? 1 : 0) );
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('form_title',    'Manual Activation');
$gSmarty->assign('form_action',   'manual_activation.php');

//show-it
$gSmarty->display('manual_activation.tpl');

?>
