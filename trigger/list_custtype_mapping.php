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


//new
$umaster =& new CustomersMapping();


//set sorter
$buid           = get_uid();

$cur_wsort = $_GET['wsort'];
if ((strlen($cur_wsort)>0) && ($cur_wsort==1)){
  $whatsort=1;
  $gSmarty->assign('q_ssort',"0");
} else
  $whatsort=0;
  
$sorter_href    = "$PHP_SELF?whatsort=$whatsort&nextpg=$nextpg&pguid=$buid";
$href_name      = $sorter_href."&sortedby=customer_type&btnSubmit=Search";
$href_code      = $sorter_href."&sortedby=msisdn_fr&btnSubmit=Search";
$href_desc      = $sorter_href."&sortedby=msisdn_to&btnSubmit=Search";
$href_status    = $sorter_href."&sortedby=status&btnSubmit=Search";

$gSmarty->assign('href_sort_name'        , $href_name);
$gSmarty->assign('href_sort_code'        , $href_code);
$gSmarty->assign('href_sort_desc'        , $href_desc);
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
$checkbox=$_GET['check_rec'];
if (($btnSubmit=='Delete Multiple') || (($btnSubmit=="") && (count($checkbox) > 0))){
   $delCtr=null;
   for($i=0;$i<count($checkbox);$i++){
      $del_id = $checkbox[$i];
      $ret = $umaster->Delete($del_id);
      if($ret)
         $delCtr = $delCtr + 1;
   }
   if ($delCtr>0)
      $gSmarty->assign('error_msg', "$delCtr record(s) deleted.");
   else
      $gSmarty->assign('error_msg', "No deleted records.");
}
else
{
	$btnSubmit='Search';
	if ($btnSubmit=='Search') {
		if((strlen($_GET['scode']) > 0) && (!@preg_match("/^639[0-9]{0,9}$/i", $_GET['scode'])) )
			$gSmarty->assign('error_msg', "ERROR: Invalid MSISDN.");
		else {
			$listdata  = $umaster->getList($extravars);
		}
	}
}

//init
$soption    =& new Select_Options_Master();
$soptstat   = $soption->getStatusSearch();
$soptcust   = $soption->getCustomerTypes();
$gSmarty->assign('is_log_in' ,   $is_logged_in      );
$gSmarty->assign('list_total',   $listdata['total'] );
$gSmarty->assign('list_data' ,   $listdata['data']  );
$gSmarty->assign('list_nav'  ,   $listdata['links'] );
$gSmarty->assign('q_csv'     ,   trim($listdata['csv']) );
$gSmarty->assign('soptstatus',   $soptstat          );
$gSmarty->assign('soptcustyp',   $soptcust          );
$gSmarty->assign('is_searched',($btnSubmit == 'Search' ? 1 : 0) );

$gSmarty->assign('q_scode'   ,   $listdata['scode']  );
$gSmarty->assign('q_sname_l' ,   $listdata['sname']);
$gSmarty->assign('q_sstat'   ,   $listdata['sstat']  );
$gSmarty->assign('q_sdall'   ,   $listdata['sdall']  );

if ((strlen($cur_wsort)>0) && ($cur_wsort==1))
	$gSmarty->assign('q_ssort'   ,   "0"  );
else
	$gSmarty->assign('q_ssort'   ,   "1"  );


//show-it
$gSmarty->display('list_custtype_mapping.tpl');
?>
