<?php
/*
#----------------------------------------------------------------------- 	
#	@Filename	    :	myConfig.php
#	@Description	:	web cfg 
#	@Date		      :	2009-07-01
#	@Ver		      :	ver1.1
#	@Author		    :	yiucon
#-----------------------------------------------------------------------
 */

error_reporting(E_ALL ^ E_NOTICE);


//------------------------------------
// globals & init here
//------------------------------------
$gDB_Handle       = null;
$gDebug           = false;
$gToday           = date("Ymd");
$gLogDebug        = true;
$gLoggerConf      = null;
$gApp_name        = "Trigger";
$gSession_id      = session_id();


//------------------------------------
// constants here
//------------------------------------
$gDev  = 0;



if(1 == $gDev)
{
	//local both db & code
	define('WEBLOG' ,               "log/$gToday.$gApp_name.log");
	define('DB_USER', 'trigger_owner');
	define('DB_PASS', 'testing');	
	define('DB_NAME', 'trgdbdev');
	define('GREYBOX_ROOT_URL',"./greybox/");
	define('SMARTY_INC_DIR','/usr/local/apache/htdocs/trigger');

	//ora-conn
	putenv("ORACLE_HOME=/u01/oracle/app/product/10g");
	putEnv("TNS_ADMIN=/u01/oracle/app/product/10g/network/admin");

}  
elseif(2 == $gDev)
{
	//local both db & code
	define('WEBLOG' , "log/$gToday.$gApp_name.log");
	define('DB_USER', 'trigger_owner');
	define('DB_PASS', 'testing');	
	define('DB_NAME', 'trgdbdev');
	define('GREYBOX_ROOT_URL',"./greybox/");
	define('SMARTY_INC_DIR',   'C:\wamp\www\trigger');

	//ora-conn
	putenv('ORACLE_BASE=C:\orant');
	putenv('ORACLE_HOME=C:\orant');
	putEnv('TNS_ADMIN=C:\orant\net80\ADMIN');

}  
else
{
	//prod both db & code
	define('WEBLOG' , "log/$gToday.swatch.log");
	define('DB_USER', 'trigger_owner');
	define('DB_PASS', 'trgown123');	
	define('DB_NAME', 'trgprd');

	//ora-conn
	putenv("ORACLE_HOME=/u01/app/oracle/product/10.2.0");
	putEnv("TNS_ADMIN=/u01/app/oracle/product/10.2.0/network/admin");

	define('GREYBOX_ROOT_URL',"./greybox/");
	define('SMARTY_INC_DIR', '/usr/local/apache/htdocs/trigger');
}	



//more
define('SEARCH_MAX_OFFSET',         10);
define('SEARCH_MAX_OFFSET_MORE',    20);
define('PWD_ENCRYPT_KEY',           md5('#!/bin/smartysmartyyiucon/uid'));
define('MAX_LOCKOUT_CTR',           3);

//flags
define('CAN_VIEW',   'can_view');
define('CAN_ADD',    'can_add');
define('CAN_UPDATE', 'can_update');
define('CAN_DELETE', 'can_delete');


//all-pages
$gModPages = array(
		
'list_acl_master.php'         => array('mod' => 'USER_ACL_MASTER' ,     'allow' => CAN_VIEW),
'add_acl_master.php'          => array('mod' => 'USER_ACL_MASTER' ,     'allow' => CAN_ADD),
'update_acl_master.php'       => array('mod' => 'USER_ACL_MASTER' ,     'allow' => CAN_UPDATE),
'delete_acl_master.php'       => array('mod' => 'USER_ACL_MASTER' ,     'allow' => CAN_DELETE),

'list_role_master.php'        => array('mod' => 'USER_ROLE_MASTER',     'allow' => CAN_VIEW),
'add_role_master.php'         => array('mod' => 'USER_ROLE_MASTER',     'allow' => CAN_ADD),
'update_role_master.php'      => array('mod' => 'USER_ROLE_MASTER',     'allow' => CAN_UPDATE),
'delete_role_master.php'      => array('mod' => 'USER_ROLE_MASTER',     'allow' => CAN_DELETE),

'list_user_master.php'        => array('mod' => 'USER_MASTER',          'allow' => CAN_VIEW),
'add_user_master.php'         => array('mod' => 'USER_MASTER',          'allow' => CAN_ADD),
'update_user_master.php'      => array('mod' => 'USER_MASTER',          'allow' => CAN_UPDATE),
'delete_user_master.php'      => array('mod' => 'USER_MASTER',          'allow' => CAN_DELETE),
'details_user_master.php'     => array('mod' => 'USER_MASTER',          'allow' => CAN_VIEW),
'reset_user_pass.php'         => array('mod' => 'USER_PASS_LOG_MASTER', 'allow' => CAN_UPDATE),

'list_services.php'           => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_service.php'             => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_service.php'          => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_service.php'          => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),
'details_service.php'         => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),

'list_keywords.php'           => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_keyword.php'             => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_keyword.php'          => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_keyword.php'          => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_keyword_msgs.php'       => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_keyword_msg.php'         => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_keyword_msg.php'      => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_keyword_msg.php'      => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_zones.php'              => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_zone.php'                => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_zone.php'             => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_zone.php'             => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_countries.php'          => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_country.php'             => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_country.php'          => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_country.php'          => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_customers.php'          => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_customer.php'            => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_customer.php'         => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_customer.php'         => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_custtype_mapping.php'   => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_custtype_mapping.php'    => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_custtype_mapping.php' => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_custtype_mapping.php' => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),
'bulk_custtype_mapping.php'   => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_hlrsrvr.php'            => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_hlrsrvr.php'             => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_hlrsrvr.php'          => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_hlrsrvr.php'          => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_hlrsrvr_mapping.php'    => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_hlrsrvr_mapping.php'     => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_hlrsrvr_mapping.php'  => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_hlrsrvr_mapping.php'  => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_insrvr.php'             => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_insrvr.php'              => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_insrvr.php'           => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_insrvr.php'           => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_insrvr_mapping.php'     => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_insrvr_mapping.php'      => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_insrvr_mapping.php'   => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_insrvr_mapping.php'   => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_sset_mapping.php'       => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_sset_mapping.php'        => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_sset_mapping.php'     => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_sset_mapping.php'     => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_plmn_mapping.php'       => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_plmn_mapping.php'        => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_plmn_mapping.php'     => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_plmn_mapping.php'     => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'list_hlrimsi_mapping.php'    => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_VIEW),
'add_hlrimsi_mapping.php'     => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_ADD),
'update_hlrimsi_mapping.php'  => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_UPDATE),
'delete_hlrimsi_mapping.php'  => array('mod' => 'APPLICATION_MASTER',   'allow' => CAN_DELETE),

'manual_activation.php'       => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_ADD),
'manual_deactivation.php'     => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_ADD),
'manual_extension.php'        => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_ADD),
'manual_reactivation.php'     => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_ADD),
'manual_blacklist.php'        => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_ADD),
'manual_whitelist.php'        => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_ADD),
'manual_fun_activation.php'   => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_ADD),
'manual_cancel.php'           => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_ADD),
'manual_retry.php'            => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_ADD),
'bulk_activation.php'         => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_VIEW),
'bulk_deactivation.php'       => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_VIEW),
'bulk_fun_activation.php'     => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_VIEW),
'bulk_fun_deactivation.php'   => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_VIEW),
'bulk_blacklisting.php'       => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_VIEW),
'bulk_whitelisting.php'       => array('mod' => 'ENROLLMENT_MASTER',    'allow' => CAN_VIEW),
 
'list_trigger_subs.php'       => array('mod' => 'FUN_TRAN_MASTER',      'allow' => CAN_VIEW),
'list_trigger_subs_hist.php'  => array('mod' => 'FUN_TRAN_MASTER',      'allow' => CAN_VIEW),
'list_trigger_tran.php'       => array('mod' => 'FUN_TRAN_MASTER',      'allow' => CAN_VIEW),
'details_trigger_subs.php'    => array('mod' => 'FUN_TRAN_MASTER',      'allow' => CAN_VIEW),
'details_trigger_subs_hist.php'=> array('mod' => 'FUN_TRAN_MASTER',      'allow' => CAN_VIEW),
'details_trigger_tran.php'    => array('mod' => 'FUN_TRAN_MASTER',      'allow' => CAN_VIEW),
'list_blacklisted.php'        => array('mod' => 'FUN_TRAN_MASTER',      'allow' => CAN_VIEW),
'list_whitelisted.php'        => array('mod' => 'FUN_TRAN_MASTER',      'allow' => CAN_VIEW),
'list_blackwhite.php'         => array('mod' => 'FUN_TRAN_MASTER',      'allow' => CAN_VIEW),
'list_fun_subs.php'           => array('mod' => 'FUN_TRAN_MASTER',      'allow' => CAN_VIEW),
'list_fun_tran.php'           => array('mod' => 'FUN_TRAN_MASTER',      'allow' => CAN_VIEW),

'list_audit_log.php'         => array('mod' => 'USER_LOG_MASTER',       'allow' => CAN_VIEW),

);



?>
