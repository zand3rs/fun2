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
$umaster =& new Customers();


//set sorter
$buid           = get_uid();
$cur_wsort = $_GET['wsort'];
if ((strlen($cur_wsort)>0) and ($cur_wsort==1))
  $whatsort=1;
else
  $whatsort=0;

//$whatsort       = ($whatsort==1) ? (0) : (1);
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
		'btnSubmit'  => 'Search',
                );
//get fields
$allfields = $umaster->getTableFields();

//get list
$btnSubmit='Search';
if ($btnSubmit=='Search')
	$listdata  = $umaster->getList($extravars);

//init
$soption    =& new Select_Options_Master();
$soptstat   = $soption->getStatusSearch();
$gSmarty->assign('is_log_in' , $is_logged_in);
$gSmarty->assign('list_total', $listdata['total']);
$gSmarty->assign('list_data' , $listdata['data'] );
$gSmarty->assign('list_nav'  , $listdata['links'] );
$gSmarty->assign('q_csv'     , trim($listdata['csv']) );
$gSmarty->assign('soptstatus', $soptstat);
$gSmarty->assign('is_searched',($btnSubmit == 'Search' ? 1 : 0) );

$gSmarty->assign('q_scode' ,   $listdata['scode']  );
$gSmarty->assign('q_sstat' ,   $listdata['sstat']  );

if ((strlen($cur_wsort)>0) && ($cur_wsort==1))
        $gSmarty->assign('q_ssort'   ,   "0"  );
else
        $gSmarty->assign('q_ssort'   ,   "1"  );

//show-it
$gSmarty->display('list_customers.tpl');
?>
