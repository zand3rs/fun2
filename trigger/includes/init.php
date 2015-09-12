<?php
 /*
 #----------------------------------------------------------------------- 	
 #	@Filename	    :	init.php
 #	@Description	:	initialization
 #	@Date		      :	2009-04-30
 #	@Ver		      :	ver1.1
 #	@Author		    :	yiucon
 #-----------------------------------------------------------------------
*/

define('MAX_TIMEOUT_MINS', 15);
@session_start();
@ini_set("session.gc_maxlifetime", 900);//max 10 minutes
@header( "Expires: Sat, 07 Aug 1976 01:00:00 GMT" );
@header( "Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT" );
@header( "Cache-Control: no-cache, must-revalidate" );
@header( "Pragma: no-cache" );

//-----
//@includes
//-----
//misc
require_once("Smarty/Smarty.class.php");
require_once("myConfig.php");
include_once('Logger/Log.php');
require_once("myUtils.php");
require_once("myLogger.php");
require_once("myOraDb.php");
require_once("myCGI.php");
require_once("myMisc.php");
require_once("myFuncs.php");

//app-mods
require_once('EventLogger.class.php');
require_once('User.class.php');
require_once('ACL.class.php');
require_once('Role.class.php');
require_once('UserACL.class.php');
require_once('Services.class.php');
require_once('Keywords.class.php');
require_once('Keyword_Msgs.class.php');
require_once('Zones.class.php');
require_once('Countries.class.php');
require_once('Customers.class.php');
require_once('CustomersMapping.class.php');
require_once('HlrSrvr.class.php');
require_once('HlrSrvrMapping.class.php');
require_once('InSrvr.class.php');
require_once('InSrvrMapping.class.php');
require_once('SsetMapping.class.php');
require_once('PlmnMapping.class.php');
require_once('View_Subscribers.class.php');
require_once('View_Subscribers_History.class.php');
require_once('View_Fun_Subscribers.class.php');
require_once('View_Transactions.class.php');
require_once('View_Fun_Transactions.class.php');
require_once('View_Blacklisted.class.php');
require_once('View_Whitelisted.class.php');
require_once('View_Audit_Log.class.php');
require_once('Select_Options_Master.class.php');
require_once('Enrollment.class.php');
require_once('BulkActivation.class.php');
require_once('BulkDeactivation.class.php');
require_once('BulkBlacklisting.class.php');
require_once('BulkWhitelisting.class.php');
require_once('View_Blacklist_Whitelist.class.php');
require_once('BulkFunActivation.class.php');
require_once('BulkFunDeactivation.class.php');
require_once('HlrIMSIMapping.class.php');

//-----
//@misc
//-----
$gSmarty  = new Smarty();
$gSmarty->template_dir = SMARTY_INC_DIR."/smarty/templates";
$gSmarty->compile_dir  = SMARTY_INC_DIR."/smarty/templates_c";
$gSmarty->cache_dir    = SMARTY_INC_DIR."/smarty/cache";
$gSmarty->config_dir   = SMARTY_INC_DIR."/smarty/configs";
$upload_dir = "uploads/";

//pop-up window
$gSmarty->assign('popup_win_width',  500);
$gSmarty->assign('popup_win_height', 500);


//options
$gSelectOptionsMaster  =& Select_Options_Master::GetInstance();
$gEventLogger          =& EventLogger::GetInstance();
$gRoleAccess           =& UserACL::GetInstance();

//init here the db connection
//$gOraDb    =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);


//logger-formatting
$gLoggerConf = array('append' => true,'mode' => 0666, 'timeFormat' => '%Y%m%d %H:%M:%S');
?>
