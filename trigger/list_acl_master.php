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
$umaster =& new ACL();

//set sorter
$buid           = get_uid();
$whatsort       = ($whatsort==1) ? (0) : (1);
$sorter_href    = "$PHP_SELF?whatsort=$whatsort&nextpg=$nextpg&pguid=$buid";
$href_role      = $sorter_href."&sortedby=role&btnSubmit=Search";
$href_mod_name  = $sorter_href."&sortedby=mod&btnSubmit=Search";

$gSmarty->assign('href_sort_role'        , $href_role);
$gSmarty->assign('href_sort_mod_name'    , $href_mod_name);

//extra
$extravars = array(
		'sortedby'=> $sortedby,
		'nextpg'  => $nextpg,
		'whatsort'=> $whatsort,
		'pguid'   => $pguid,
		'btnSubmit'  => 'Search',
                );

$soption =& new Select_Options_Master();
$srole_codes = $soption->getRoleMaster2();
$smod_codes  = $soption->getModuleMaster2();

//get fields
$allfields = $umaster->getTableFields();

//get list
if ($btnSubmit=='Search')
	$listdata  = $umaster->getList($extravars);
//init
$gSmarty->assign('is_log_in' , $is_logged_in);
$gSmarty->assign('list_total', $listdata['total']);
$gSmarty->assign('list_data' , $listdata['data'] );
$gSmarty->assign('list_nav'  , $listdata['links'] );
$gSmarty->assign('master_fields', $allfields);
$gSmarty->assign('is_searched',($btnSubmit == 'Search' ? 1 : 0) );
$gSmarty->assign('srole_code', $srole_codes);
$gSmarty->assign('smod_code', $smod_codes);


//show-it
$gSmarty->display('list_acl_master.tpl');
?>
