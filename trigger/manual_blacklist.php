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
if($btnSubmit == "BLACKLIST")
{
	//valid
	//ok 2 save
	if(isset($_POST))
	{
		if(!@preg_match("/^639[0-9]{9}$/i", $_POST['q_msisdn']) or (strlen($_POST['q_msisdn']) == 0) )
		{
			$gSmarty->assign('error_msg', "BLACKLISTING ERROR: Invalid MSISDN.");
		} 
		elseif($umaster->isBlacklisted($_POST['q_msisdn']) == 1)
		{
			$gSmarty->assign('error_msg', "BLACKLISTING ERROR: MSISDN is already blacklisted.");
		} 
		else
		{
		   $ret = $umaster->Blacklist($_POST['q_msisdn'], $_POST['q_remarks']);	
		   if($ret['error'])
		   {
			$errwords = split(" ", $ret['error_msg']);
			$ukerr = 0;
			foreach ($errwords as $UkMsg){
				if ($UkMsg=="ORA-00001:")
					$ukerr = 1;
			}
			if ($ukerr == 1)
				$gSmarty->assign('error_msg', "BLACKLISTING ERROR: Duplicate entry.");
			else
			     $gSmarty->assign('error_msg', "BLACKLISTING ERROR");
		   }
		   else
		   {
			$gSmarty->assign('error_msg', "BLACKLISTING Successfully entered.");
		   }
		}
	}
	
}

//show
$gSmarty->assign('is_searched',($btnSubmit == 'BLACKLIST' ? 1 : 0) );
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('form_title',    'Manual Blacklist');
$gSmarty->assign('form_action',   'manual_blacklist.php');

//show-it
$gSmarty->display('manual_blacklist.tpl');

?>
