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
$umaster =& new User();
$umaster->Open($_SESSION["session.smartygold.profile.user"]);


//set sorter
$buid           = get_uid();
$whatsort       = ($whatsort==1) ? (0) : (1);
$sorter_href    = "$PHP_SELF?whatsort=$whatsort&nextpg=$nextpg&pguid=$buid";
$href_name      = $sorter_href."&sortedby=lastname&btnSubmit=Search";
$href_user      = $sorter_href."&sortedby=user&btnSubmit=Search";
$href_role      = $sorter_href."&sortedby=role&btnSubmit=Search";
$href_email     = $sorter_href."&sortedby=email&btnSubmit=Search";
$href_position  = $sorter_href."&sortedby=position&btnSubmit=Search";
$href_expiry    = $sorter_href."&sortedby=expiry&btnSubmit=Search";

$gSmarty->assign('href_sort_name'        , $href_name);
$gSmarty->assign('href_sort_user'        , $href_user);
$gSmarty->assign('href_sort_role'        , $href_role);
$gSmarty->assign('href_sort_email'       , $href_email);
$gSmarty->assign('href_sort_position'    , $href_position);
$gSmarty->assign('href_sort_expiry'      , $href_expiry);


//get fields
$allfields = $umaster->getTableFields();
$soption =& new Select_Options_Master();
$srole_codes = $soption->getRoleMaster2();

//extra
$extravars = array(
		'sortedby'   => $sortedby,
		'nextpg'     => $nextpg,
		'whatsort'   => $whatsort,
		'pguid'      => $pguid,
		'btnSubmit'  => 'Search',
                );

//get list
if ($btnSubmit=='Search')
	$listdata  = $umaster->getList($extravars);

//init
$gSmarty->assign('is_log_in' , is_logged_in() );
$gSmarty->assign('list_total', $listdata['total']);
$gSmarty->assign('list_data' , $listdata['data'] );
$gSmarty->assign('list_nav'  , $listdata['links'] );
$gSmarty->assign('srole_code', $srole_codes);
$gSmarty->assign('is_searched',($btnSubmit == 'Search' ? 1 : 0) );

//CSV Filename
$gSmarty->assign('q_csv'         , trim($listdata['csv']) );

//show-it
$gSmarty->display('list_user_master.tpl');
?>
