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
	}
	else
	{
		//ok 2 save
		if(isset($_POST))
		{

		
			//get fields
		        $allfields = $umaster->getTableFields();
		        $allfields[] = array('name' => 'id', 'value'=> $_POST['id']);
			if(null != $allfields)
			{
			   $ret = $umaster->Update($allfields);	
			   if($ret['error'])
			   {
				$gSmarty->assign('error_msg', "Update HLR IMSI Mapping Failed.");
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
				$gSmarty->assign('error_msg', "Update HLR IMSI Mapping Successfully Completed.");
				$gSmarty->display('list_hlrimsi_mapping.tpl');
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
	   $gSmarty->assign('error_msg', UPDATE_HLR_IMSI_MAPPING_FAILED_NO_RECORD_FOUND); 
	}

}

//show
//$_SESSION["timeout"] = time();
$gSmarty->assign('is_log_in',     $is_logged_in );
$gSmarty->assign('master_fields', $allfields);
$gSmarty->assign('form_title',    "HLR Server IMSI Mapping Maintenance");
$gSmarty->assign('form_action',   "update_hlrimsi_mapping.php");
$gSmarty->assign('form_submit',   "Update");
$gSmarty->assign('form_primary',  "id");
$gSmarty->display('update_master.tpl');
?>
