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
$umaster =& new HlrIMSIMapping();
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
				$gSmarty->assign('error_msg', "Add HLR IMSI Mapping Failed.");
			   }
			   else
			   {
				//lookup
				$soption  =& new Select_Options_Master();
				$soptstat = $soption->getStatusSearch();
				$sopthlrs = $soption->getHlrSrvr();
				$gSmarty->assign('soptstatus', $soptstat );
				$gSmarty->assign('sopthlrsvr', $sopthlrs );

				//init
				$gSmarty->assign('is_log_in' , $is_logged_in);
				$gSmarty->assign('is_searched', 0 );
				$gSmarty->assign('error_msg', "Add HLR IMSI Mapping Successfully Completed.");
				$gSmarty->display('list_hlrimsi_mapping.tpl');
				exit;
			   }
			}
		}
		
	}
	
}

//show
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('master_fields', $allfields);
$gSmarty->assign('form_title',    'HLR Server IMSI Mapping Maintenance');
$gSmarty->assign('form_action',   'add_hlrimsi_mapping.php');
$gSmarty->assign('form_submit',   'Save');
$gSmarty->display('add_master.tpl');
?>
