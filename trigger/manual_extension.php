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
if($btnSubmit == "EXTEND")
{
	//valid
	//ok 2 save
	if(isset($_POST))
	{
		if(!@preg_match("/^639[0-9]{9}$/i", $_POST['q_msisdn']) or (strlen($_POST['q_msisdn']) == 0) )
		{
			$gSmarty->assign('error_msg', "EXTEND ERROR: Invalid MSISDN.");
		} 
                elseif($umaster->isValidGlobe($_POST['q_msisdn']) == 0)
                {
                        $gSmarty->assign('error_msg', "ACTIVATION ERROR: Invalid MSISDN.");
                }
		elseif(!@preg_match("/^[0-9]{1,4}$/i", $_POST['q_duration']) and (strlen($_POST['q_duration']) > 0) )
		{
			$gSmarty->assign('error_msg', "EXTEND ERROR: Invalid duration.");
		} 
		elseif($umaster->isBlacklisted($_POST['q_msisdn']) == 1)
		{
			$gSmarty->assign('error_msg', "EXTEND ERROR: MSISDN is blacklisted.");
		} 
		elseif($umaster->chkRoamer($_POST['q_msisdn']) == 0)
		{
			$gSmarty->assign('error_msg', "EXTEND ERROR: Subscriber is not an active roamer.");
		} 
		elseif($umaster->chkExtensionAllowedDuration($_POST['q_msisdn'],$_POST['q_duration']) == 0)
		{
			$gSmarty->assign('error_msg', "EXTEND ERROR: Should be less than defined max duration.");
		} 
		elseif($umaster->chkPendingRequest($_POST['q_msisdn'],'EXTENSION') >= 1)
		{
			$gSmarty->assign('error_msg', "EXTEND ERROR: Already has a pending extend request.");
		} 
		else
		{
                   $nBalChk  = (isset($_POST['q_balchk'])) ? 1 : 0;
                   $nBPeriod = (isset($_POST['q_bperiod'])) ? 1 : 0;
		   $ret = $umaster->Extension($_POST['q_msisdn'], $_POST['q_duration'], $_POST['q_name'], $_POST['q_contact'], $_POST['q_remarks'], $nBalChk, $nBPeriod);	
		   if($ret['error'])
		   {
			$errwords = split(" ", $ret['error_msg']);
			$ukerr = 0;
			foreach ($errwords as $UkMsg){
				if ($UkMsg=="ORA-00001:")
					$ukerr = 1;
			}
			if ($ukerr == 1)
				$gSmarty->assign('error_msg', "EXTEND ERROR: Already has a pending extend request.");
			else
			     $gSmarty->assign('error_msg', "EXTEND ERROR");
		   }
		   else
		   {
			$gSmarty->assign('error_msg', SAVING_EXTENSION_SUCCESS);
		   }
		}
	}
	
}

//show
$gSmarty->assign('is_searched',($btnSubmit == 'EXTEND' ? 1 : 0) );
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('form_title',    'Manual Extension');
$gSmarty->assign('form_action',   'manual_extension.php');

//show-it
$gSmarty->display('manual_extension.tpl');

?>
