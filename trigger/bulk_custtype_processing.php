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

$umaster =& new CustomersMapping();

//set sorter
$buid           = get_uid();
$whatsort       = ($whatsort==1) ? (0) : (1);

//get fields
$soption    =& new Select_Options_Master();
$soptstat   = $soption->getStatusSearch();
$soptcust   = $soption->getCustomerTypes();
$gSmarty->assign('soptstatus',   $soptstat          );
$gSmarty->assign('soptcustyp',   $soptcust          );

//extra
$extravars = array(
                'sortedby'=> $sortedby,
                'nextpg'  => $nextpg,
                'whatsort'=> $whatsort,
                'pguid'   => $pguid,
                'btnSubmit'  => 'Search',
                );

//Upload check
$vUniqueKey = ora_addslashes($_SESSION["session.smartygold.profile.user"])."-".date("YmdHis");
$nCounter = 0;

//process file
if (!empty($_FILES) && ($_FILES['uploaded_file']['name'] != ''))
{
	$upload_path = $upload_dir.'/'.$_FILES['uploaded_file']['name'];
	if (move_uploaded_file($_FILES['uploaded_file']['tmp_name'], $upload_path))
	{
		$fh = fopen ($upload_path,'r+');
		while (!feof($fh))
		{
			$info = fgets($fh);
			$data = split(',',$info);
			if (!((sizeof($data)==1) && (strlen($data[0])==0)))
			{
				$umaster->SaveList($data, $vUniqueKey);
                                $nCounter = $nCounter + 1;
			}
		}
		fclose ($fh);
		$listdata = $umaster->getBulkList($extravars, $vUniqueKey);
		echo "<script>parent.unlockMenu();</script>";
	}
	$gSmarty->assign('is_searched',1);
        $gSmarty->assign('is_log_in' , is_logged_in() );
        $gSmarty->assign('list_data' , $listdata['data'] );
        $gSmarty->assign('list_nav'  , $listdata['links'] );
	$gSmarty->assign('list_total', $listdata['total']);
        $gSmarty->assign('list_all', $nCounter);
        $gSmarty->display('list_custtype_mapping2.tpl');
}
else
{
	echo "<script>parent.unlockMenu();</script>";
        $gSmarty->assign('list_total', '-1');
	$gSmarty->assign('is_log_in' , is_logged_in() );
	$gSmarty->assign('list_data' , $listdata['data'] );
	$gSmarty->assign('list_nav'  , $listdata['links'] );
}

?>