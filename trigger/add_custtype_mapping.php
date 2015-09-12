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
$umaster =& new CustomersMapping();
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
				$gSmarty->assign('error_msg', "Add Customer Type Mapping Successfully Failed.");
			   }
			   else
			   {
				$soption    =& new Select_Options_Master();
				$soptstat   = $soption->getStatusSearch();
				$soptcust   = $soption->getCustomerTypes();
				$gSmarty->assign('is_log_in' ,   $is_logged_in      );
				$gSmarty->assign('soptstatus',   $soptstat          );
				$gSmarty->assign('soptcustyp',   $soptcust          );
				$gSmarty->assign('is_searched', 0 );
				$gSmarty->assign('error_msg', "Add Customer Type Mapping Successfully Completed.");
				$gSmarty->display('list_custtype_mapping.tpl');
				exit;
			   }
			}
		}
		
	}
	
}

//show
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('master_fields', $allfields);
$gSmarty->assign('form_title',    'Customer Type Mapping Maintenance');
$gSmarty->assign('form_action',   'add_custtype_mapping.php');
$gSmarty->assign('form_submit',   'Save');
$gSmarty->display('add_master.tpl');
?>
