<?php



$w_page = basename($_SERVER["SCRIPT_NAME"]);
$w_mod  = $gModPages[$w_page]['mod'];
$w_allow= $gModPages[$w_page]['allow'];
$w_role = $_SESSION["session.smartygold.profile.role"];

debug("is_allowed() : $w_page // $w_mod // $w_allow // $w_role");

if(! $gRoleAccess->Allowed($w_role,$w_mod,$w_allow) )
{
    //show
   $gSmarty->assign('error_msg',    PAGE_ACCESS_NOT_ALLOWED ); 
   $gSmarty->assign('is_log_in',    $is_logged_in);
   $gSmarty->display('access_denied.tpl');
   exit;	
}
?>