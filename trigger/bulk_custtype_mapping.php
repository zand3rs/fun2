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
			$data = split(',',trim($info));
			if (!((sizeof($data)==1) && (strlen($data[0])==0)))
			{
				$umaster->SaveList($data, $vUniqueKey);
                                $nCounter = $nCounter + 1;
			}
		}
		fclose ($fh);
		$listdata = $umaster->getBulkList($extravars, $vUniqueKey);
	}			
	$gSmarty->assign('is_searched',1);
        $gSmarty->assign('is_log_in' , is_logged_in() );
        $gSmarty->assign('list_data' , $listdata['data'] );
        $gSmarty->assign('list_nav'  , $listdata['links'] );
	$gSmarty->assign('list_total', $listdata['total']);
        $gSmarty->assign('list_all', $nCounter);
	$gSmarty->display('list_bulk_custtype_mapping.tpl');
}
else
{
        $gSmarty->assign('list_total', '-1');
	$gSmarty->assign('is_log_in' , is_logged_in() );
	$gSmarty->assign('list_data' , $listdata['data'] );
	$gSmarty->assign('list_nav'  , $listdata['links'] );
	$gSmarty->assign('srole_code', $srole_codes);

	//show-it
	$gSmarty->display('bulk_custtype_mapping.tpl');
}

?>
<script>
	function lockMenu()
	{
		var newDiv = new Element('div');
		var pos = $('nav').getCoordinates();
		newDiv.inject($('nav'));
		newDiv.id = 'lock_menu';
		newDiv.setStyles({
			'width' : pos.width,
			'height' : pos.height,
			'position' : 'absolute',
			'top' : pos.top,
			'left' : pos.left,
			'background' : '#FFFAF0'
		});
		
		var pos2 = $('upload_tr').getCoordinates();
		var newDiv2 = new Element('div');
		newDiv2.inject($('wrapper'));
		newDiv2.id = 'lock_bar';
	
		newDiv2.setStyles({
			'width' : pos2.width,
			'height' : pos2.height,
			'top' : pos2.top,
			'left': pos2.left,
			'background': '#FFFAF0',
			'position': 'absolute'
		});
	}
	
	function unlockMenu()
	{
		if ($('lock_menu'))
		{
			var elem = new Element($('lock_menu'));
			elem.dispose();
		}
		
		if ($('lock_bar'))
		{
			var elem = new Element($('lock_bar'));
			elem.dispose();
		}
	}
</script>
