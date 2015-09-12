<?php
/*
#----------------------------------------------------------------------------------
#	@Filename	:	EventLogger.class.php
#	@Description	:	
#
#
#	@Date		  :	2009-04-30
#	@Ver		  :	ver1.1
#	@Author		:	yiucon
#
#
#  @Modified Date:
#  @Modified By  :
#  @Modified Desc:
#
#----------------------------------------------------------------------------------
 */


//class
class EventLogger
{


	//constructor
	function EventLogger()
	{

	}

	//event
	function LogEvent($mod="",$msg="")
	{
		//globals
		global $_SERVER;
		global $_GET;
		global $_POST;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//url
		$url    = $_SERVER["REQUEST_URI"];
		$ip     = $_SERVER["SERVER_ADDR"];

		//user
		$usr_p  = "";  	   
		$usr_p .= "id =" . $_SESSION["session.smartygold.profile.id"]     . "\n" ;
		$usr_p .= "usr=" . $_SESSION["session.smartygold.profile.user"]   . "\n" ;
		$usr_p .= "uid=" . $_SESSION["session.smartygold.profile.uid"]    . "\n" ;

		//more
		$role   = $_SESSION["session.smartygold.profile.role"];
		$user   = $_SESSION["session.smartygold.profile.user"];
		$srvr   = " IP => [$ip] \n";

		//get
		$get    = '';
		if(!empty($_GET) and is_array($_GET))
		{
			while( list($k,$v) = each($_GET) ) { 
				if(!(($k == 'pass') || ($k == 'muid') || ($k == 'uid'))){
					$get    .= "$k => $v \n"; 
				}
			}
		}

		//post
		$post    = '';
		if(!empty($_POST) and is_array($_POST))
		{
			while( list($k,$v) = each($_POST) ) 
			{ 
				if(!(($k == 'pass') || ($k == 'muid') || ($k == 'uid'))){
					$post    .= "$k => $v \n"; 
				}
			}
		}


		//fmt
		$user      = ora_addslashes($user);
		$mod       = ora_addslashes($mod);
		$role      = ora_addslashes($role);
		//$log       = ora_addslashes(str_replace("\n","",$srvr . $get . $post ."\n". " msg=$msg"));
		//$log       = ora_addslashes(str_replace("\n","",$get . $post ."\n". " msg=$msg"));
		$log       = ora_addslashes(str_replace("\n","","$msg"));
		$created_by= (strlen(trim($user))<=0) ? ('system') : ($user);

		//SQL
		$sql       = " INSERT INTO user_log (user_code, role_code, mod_name, ip_addr, url, logs, created_by, dt_created) ".
			     " VALUES ('$user','$role', '$mod', '$ip', '$url', '$log', '$created_by', sysdate )                      ";


		//logon to oracle database
		$res       = $myOraDb->xQuery($sql);

		//log
		$this->debug("LogEvent() : ( $res[error] => $res[msg] => $sql)");

		return $res;
	}




	function &GetInstance()
	{
		static $instance;

		if(empty($instance) or !$instance or !defined($instance))
		{
			$instance   = new EventLogger();
		}

		return $instance;
	}


	//for debugging
	function debug($msg='')
	{
		global $gLoggerConf,$gDebug,$gLogDebug;

		$tm   = date("Y-m-d H:i:s");
		$buff = sprintf("DEBUG: [%15s] : %s : %s", " ----- " , $tm, $msg); 

		if($gDebug)
		{
			echo "$buff<hr>\n";	

		}
		if($gLogDebug)
		{
			$logger = &Log::singleton('file', WEBLOG, __CLASS__, $gLoggerConf);
			$logger->log($msg);
		}
	}

}//class	


?>
