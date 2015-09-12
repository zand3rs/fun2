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
$umaster =& new SsetMapping();

//set sorter
$buid           = get_uid();
$cur_wsort = $_GET['wsort'];
if ((strlen($cur_wsort)>0) and ($cur_wsort==1))
  $whatsort=1;
else
  $whatsort=0;

//$whatsort       = ($whatsort==1) ? (0) : (1);
$sorter_href    = "$PHP_SELF?whatsort=$whatsort&nextpg=$nextpg&pguid=$buid";
$href_code      = $sorter_href."&sortedby=msisdn_fr&btnSubmit=Search";
$href_desc      = $sorter_href."&sortedby=msisdn_to&btnSubmit=Search";
$href_name      = $sorter_href."&sortedby=sset_id&btnSubmit=Search";
$href_irse      = $sorter_href."&sortedby=irsset&btnSubmit=Search";
$href_norm      = $sorter_href."&sortedby=normalsset&btnSubmit=Search";
$href_status    = $sorter_href."&sortedby=status&btnSubmit=Search";

$gSmarty->assign('href_sort_code'        , $href_code);
$gSmarty->assign('href_sort_desc'        , $href_desc);
$gSmarty->assign('href_sort_name'        , $href_name);
$gSmarty->assign('href_sort_irse'        , $href_irse);
$gSmarty->assign('href_sort_norm'        , $href_norm);
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
      echo "<br>";
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
		else
			$listdata  = $umaster->getList($extravars);
	}
}
//lookup
$soption    =& new Select_Options_Master();
$soptstat   = $soption->getStatusSearch();
$gSmarty->assign('soptstatus', $soptstat );

//init
$gSmarty->assign('is_log_in' ,   $is_logged_in      );
$gSmarty->assign('list_total',   $listdata['total'] );
$gSmarty->assign('list_data' ,   $listdata['data']  );
$gSmarty->assign('list_nav'  ,   $listdata['links'] );
$gSmarty->assign('q_csv'     , trim($listdata['csv']) );
$gSmarty->assign('is_searched',($btnSubmit == 'Search' ? 1 : 0) );

$gSmarty->assign('q_scode' ,   $listdata['scode']   );
$gSmarty->assign('q_sname' ,   $listdata['sname']   );
$gSmarty->assign('q_sname1',   $listdata['sname1']  );
$gSmarty->assign('q_sname2',   $listdata['sname2']  );
$gSmarty->assign('q_sstat' ,   $listdata['sstat']   );

if ((strlen($cur_wsort)>0) && ($cur_wsort==1))
        $gSmarty->assign('q_ssort'   ,   "0"  );
else
        $gSmarty->assign('q_ssort'   ,   "1"  );

//show-it
$gSmarty->display('list_sset_mapping.tpl');
?>
