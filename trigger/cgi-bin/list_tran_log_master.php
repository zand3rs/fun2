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
$umaster =& new View_Log_Master();

//set sorter
$buid           = get_uid();
$whatsort       = ($whatsort==1) ? (0) : (1);
$sorter_href    = "$PHP_SELF?whatsort=$whatsort&nextpg=$nextpg&pguid=$buid";
$href_tran_id   = $sorter_href."&sortedby=tran_id";
$href_tran_dt   = $sorter_href."&sortedby=tran_dt";
$href_tran_amt  = $sorter_href."&sortedby=tran_amt";
$href_gsmnum    = $sorter_href."&sortedby=gsmnum";

$gSmarty->assign('href_sort_tran_id'    , $href_tran_id);
$gSmarty->assign('href_sort_tran_dt'    , $href_tran_dt);
$gSmarty->assign('href_sort_tran_amt'   , $href_tran_amt);
$gSmarty->assign('href_sort_gsmnum'     , $href_gsmnum);


//extra
$extravars = array(
		'sortedby'=> $sortedby,
		'nextpg'  => $nextpg,
		'whatsort'=> $whatsort,
		'pguid'   => $pguid,
		);

//get list
$listdata  = $umaster->getListTranLog($extravars);

//init
$gSmarty->assign('is_log_in' , $is_logged_in);
$gSmarty->assign('list_total', $listdata['total']);
$gSmarty->assign('list_data' , $listdata['data'] );
$gSmarty->assign('list_nav'  , $listdata['links'] );


//show-it
$gSmarty->display('list_tran_log_master.tpl');
?>
