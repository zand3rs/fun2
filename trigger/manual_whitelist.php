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
if($btnSubmit == "WHITELIST")
{
	//valid
	//ok 2 save
	if(isset($_POST))
	{
		if(!@preg_match("/^639[0-9]{9}$/i", $_POST['q_msisdn']) or (strlen($_POST['q_msisdn']) == 0) )
		{
			$gSmarty->assign('error_msg', "WHITELISTING ERROR: Invalid MSISDN.");
		} 
		else
		{
		   $ret = $umaster->Whitelist($_POST['q_msisdn'], $_POST['q_remarks']);	
		   if($ret['error'])
		   {
			$errwords = split(" ", $ret['error_msg']);
			$ukerr = 0;
			foreach ($errwords as $UkMsg){
				if ($UkMsg=="ORA-00001:")
					$ukerr = 1;
			}
			if ($ukerr == 1)
				$gSmarty->assign('error_msg', "MSISDN is already whitelisted.");
			else
			     $gSmarty->assign('error_msg', "WHITELISTING ERROR");
		   }
		   else
		   {
			$gSmarty->assign('error_msg', "WHITELISTING Successfully entered.");
		   }
		}
	}
	
}

//show
$gSmarty->assign('is_searched',($btnSubmit == 'WHITELIST' ? 1 : 0) );
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('form_title',    'Manual Whitelist');
$gSmarty->assign('form_action',   'manual_whitelist.php');

//show-it
$gSmarty->display('manual_whitelist.tpl');

?>
