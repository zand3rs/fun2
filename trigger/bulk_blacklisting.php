<?php
//include
include_once('includes/init.php');
set_time_limit(0);

//chk
$is_logged_in = is_logged_in();
if(! $is_logged_in)
{
    redir_page("index.php");
    exit;
}

//allowed?
include_once('is_allowed.php');

$umaster =& new BulkBlacklisting();
$vUniqueKey = ora_addslashes($_SESSION["session.smartygold.profile.user"])."-".date("YmdHis");
$nCounter = 0;
//process file
if (!empty($_FILES))
{
	if ($_FILES['uploaded_file']['name'] != '')
	{
		$upload_path = $upload_dir.'/'.$_FILES['uploaded_file']['name'];
		if (move_uploaded_file($_FILES['uploaded_file']['tmp_name'], $upload_path))
		{
			$fh = fopen ($upload_path,'r+');
			while (!feof($fh))
			{
				$info = fgets($fh);
				$data = split(',',trim($info));
				if (!((sizeof($data)==1) && (strlen($data[0])==0)))
				{
					$umaster->SaveList($data,$vUniqueKey);
					$nCounter = $nCounter + 1;
				}
			}
			fclose ($fh);
			$listdata = $umaster->getList($vUniqueKey);
		}
	}
	$gSmarty->assign('list_total', $listdata['total']);
	$gSmarty->assign('list_all', $nCounter);
}
else
{
	$gSmarty->assign('list_total', '-1');
}

$gSmarty->assign('is_log_in' , is_logged_in() );
$gSmarty->assign('list_data' , $listdata['data'] );
$gSmarty->assign('list_nav'  , $listdata['links'] );
$gSmarty->assign('srole_code', $srole_codes);


//show-it
$gSmarty->display('bulk_blacklisting.tpl');

?>
