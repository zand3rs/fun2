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
$umaster =& new Keyword_Msgs();


//set sorter
$buid           = get_uid();
$cur_wsort = $_GET['wsort'];
if ((strlen($cur_wsort)>0) and ($cur_wsort==1))
  $whatsort=1;
else
  $whatsort=0;

//$whatsort       = ($whatsort==1) ? (0) : (1);
$sorter_href    = "$PHP_SELF?whatsort=$whatsort&nextpg=$nextpg&pguid=$buid";
$href_code      = $sorter_href."&sortedby=keyword&btnSubmit=Search";
$href_name      = $sorter_href."&sortedby=sub_keyword&btnSubmit=Search";
$href_cust      = $sorter_href."&sortedby=customer_type&btnSubmit=Search";
$href_mesg      = $sorter_href."&sortedby=message_id&btnSubmit=Search";
$href_desc      = $sorter_href."&sortedby=keyword_desc&btnSubmit=Search";
$href_status    = $sorter_href."&sortedby=status&btnSubmit=Search";

$gSmarty->assign('href_sort_name'        , $href_name);
$gSmarty->assign('href_sort_desc'        , $href_desc);
$gSmarty->assign('href_sort_code'        , $href_code);
$gSmarty->assign('href_sort_cust'        , $href_cust);
$gSmarty->assign('href_sort_mesg'        , $href_mesg);
$gSmarty->assign('href_sort_status'      , $href_status);


//extra
$extravars = array(
		'sortedby'=> $sortedby,
		'nextpg'  => $nextpg,
		'whatsort'=> $whatsort,
		'pguid'   => $pguid,
		'btnSubmit'  => 'Search',
                );
//get fields
$allfields  = $umaster->getTableFields();

//get list
$btnSubmit='Search';
if ($btnSubmit=='Search')
	$listdata  = $umaster->getList($extravars);

//lookup
$soption    =& new Select_Options_Master();
$soptstat   = $soption->getStatusSearch();
$soptcust   = $soption->getCustomerTypesSearch();
$soptmesg   = $soption->getMessageTypeSearch();
$soptkeyw   = $soption->getKeywords();
$soptsubk   = $soption->getSubKeywords();
$gSmarty->assign('soptstatus',   $soptstat);
$gSmarty->assign('soptcustomer', $soptcust);
$gSmarty->assign('soptmessage',  $soptmesg);
$gSmarty->assign('soptkeyword',  $soptkeyw);
$gSmarty->assign('soptsubkeyword',$soptsubk);

//init
$gSmarty->assign('is_log_in' ,   $is_logged_in);
$gSmarty->assign('list_total',   $listdata['total']);
$gSmarty->assign('list_data' ,   $listdata['data'] );
$gSmarty->assign('list_nav'  ,   $listdata['links'] );
$gSmarty->assign('q_csv'     ,   trim($listdata['csv']) );
$gSmarty->assign('is_searched',($btnSubmit == 'Search' ? 1 : 0) );

$gSmarty->assign('q_scode' ,    $listdata['scode'] );
$gSmarty->assign('q_ssubcode',  $listdata['ssubcode'] );
$gSmarty->assign('q_scust' ,    $listdata['scust'] );
$gSmarty->assign('q_smesg' ,    $listdata['smesg'] );
$gSmarty->assign('q_sname' ,    $listdata['sname'] );
$gSmarty->assign('q_sstat' ,    $listdata['sstat'] );

if ((strlen($cur_wsort)>0) && ($cur_wsort==1))
        $gSmarty->assign('q_ssort'   ,   "0"  );
else
        $gSmarty->assign('q_ssort'   ,   "1"  );

//show-it
$gSmarty->display('list_keyword_msgs.tpl');
?>
