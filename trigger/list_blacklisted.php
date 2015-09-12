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

define("DATE_PARAMETER_IS_INVALID_S",        "Invalid start date parameter - should be in YYYY-MM-DD format.");
define("DATE_PARAMETER_IS_INVALID_E",        "Invalid end date parameter - should be in YYYY-MM-DD format.");
define("DATE_PARAMETER_IS_INVALID_S_ISNULL", "Please enter start date.");
define("DATE_PARAMETER_IS_INVALID_E_ISNULL", "Please enter end date.");
define("DATE_PARAMETER_IS_INVALID_G",        "Start date should be less than End date.");
//new
$master =& new View_Blacklisted();

//extra
$extravars = array(
		'sortedby'   => $sortedby,
		'nextpg'     => $nextpg,
		'whatsort'   => $whatsort,
		'pguid'      => $pguid,
		'q_sdate'    => $q_sdate,
		'q_edate'    => $q_edate,
		'q_msisdn'   => $q_msisdn,
		'btnSubmit'  => 'Search',
);
                
if(
  !@preg_match("/^(\d{4}-?\d{2}-?\d{2})$/i", $q_sdate) and
  (strlen($q_sdate) > 0)
  )
{
	$gSmarty->assign('error_msg', DATE_PARAMETER_IS_INVALID_S);
} 
else if(
  !@preg_match("/^(\d{4}-?\d{2}-?\d{2})$/i", $q_edate) and
  (strlen($q_edate) > 0)
  )
{
	$gSmarty->assign('error_msg', DATE_PARAMETER_IS_INVALID_E);
}
else if(((strlen($q_sdate) > 0) and (strlen($q_edate) == 0)) or
       ((strlen($q_sdate) == 0) and (strlen($q_edate) > 0)))
{
        if(strlen($q_sdate) == 0)
        {
                $gSmarty->assign('error_msg', DATE_PARAMETER_IS_INVALID_S_ISNULL);
        }
        else if(strlen($q_edate) == 0)
        {
                $gSmarty->assign('error_msg', DATE_PARAMETER_IS_INVALID_E_ISNULL);
        }
}
else if($q_sdate > $q_edate)
{
	$gSmarty->assign('error_msg', DATE_PARAMETER_IS_INVALID_G);
}
else if( (strlen($q_msisdn) > 0) && !@preg_match("/^639[0-9]{9}$/i", $q_msisdn) )
{
	$gSmarty->assign('error_msg', INVALID_MSISDN);
}
else
{
	//get list
        if ($btnSubmit == 'Search')
		$listdata  = $master->ListSubscribers($extravars, $q_sdate, $q_edate, $q_msisdn );

}

//$btnSubmit='Search Wallet';
//init
$gSmarty->assign('master_fields', $allfields);
$gSmarty->assign('is_log_in' ,  $is_logged_in);
$gSmarty->assign('list_total',  intval($listdata['total']));
$gSmarty->assign('list_data' ,  $listdata['data'] );
$gSmarty->assign('list_nav'  ,  $listdata['links'] );
$gSmarty->assign('is_searched',($btnSubmit == 'Search' ? 1 : 0) );

//save-it
$gSmarty->assign('q_sdate'       , $q_sdate         );
$gSmarty->assign('q_edate'       , $q_edate         );
$gSmarty->assign('q_msisdn'      , $q_msisdn        );
$gSmarty->assign('q_sort'        , $q_sort          );
$gSmarty->assign('q_csv'         , trim($listdata['csv']) );


//show-it
$gSmarty->display('list_blacklisted.tpl');

?>
