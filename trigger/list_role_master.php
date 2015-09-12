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
$umaster =& new Role();


//set sorter
$buid           = get_uid();
$whatsort       = ($whatsort==1) ? (0) : (1);
$sorter_href    = "$PHP_SELF?whatsort=$whatsort&nextpg=$nextpg&pguid=$buid";
$href_name      = $sorter_href."&sortedby=name&btnSubmit=Search";
$href_desc      = $sorter_href."&sortedby=desc&btnSubmit=Search";

$gSmarty->assign('href_sort_name'        , $href_name);
$gSmarty->assign('href_sort_desc'        , $href_desc);


//extra
$extravars = array(
		'sortedby'=> $sortedby,
		'nextpg'  => $nextpg,
		'whatsort'=> $whatsort,
		'pguid'   => $pguid,
                );
//get fields
$allfields = $umaster->getTableFields();
//get list
$listdata  = $umaster->getList($extravars);
//init
$gSmarty->assign('is_log_in' , $is_logged_in);
$gSmarty->assign('list_total', $listdata['total']);
$gSmarty->assign('list_data' , $listdata['data'] );
$gSmarty->assign('list_nav'  , $listdata['links'] );



//show-it
$gSmarty->display('list_role_master.tpl');
?>
