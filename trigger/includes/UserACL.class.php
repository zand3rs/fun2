<?php
/*
 #----------------------------------------------------------------------------------
 #	@Filename	:	UserACL.class.php
 #	@Description	:	
 #
 #
 #	@Date		:	2009-04-30
 #	@Ver		:	ver1.1
 #	@Author		:	bayugyug
 #
 #
 #  @Modified Date:
 #  @Modified By  :
 #  @Modified Desc:
 #
 #----------------------------------------------------------------------------------
*/


//class
class UserACL
{

	//constructor
	function UserACL()
	{

	}
	
	//1 instance
	function &GetInstance()
	{
		static $instance;
		
		if(empty($instance) or !$instance or !defined($instance))
		{
		  $c          = new UserACL();
		  $instance   = $c;
		}
		
		return $instance;
	}
	
	
        //init
	function Get($role='',$mod='')
	{
		//globals here
		global  $gEventLogger;


		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$role      = ora_addslashes(trim($role));
		$mod       = ora_addslashes(trim($mod));

		//update profile
		$sql     = " SELECT  id, role_code, mod_name, can_view, can_update, can_add, can_delete FROM ACL WHERE ROLE_CODE='$role' AND MOD_NAME='$mod' ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("Get() : [$sql / $stmt ]");

		//bind
		OCIDefineByName( $stmt, 'ID'              , $P_ID             );
		OCIDefineByName( $stmt, 'ROLE_CODE'       , $P_ROLE_CODE      );
		OCIDefineByName( $stmt, 'MOD_NAME'        , $P_MOD_NAME       );
		OCIDefineByName( $stmt, 'CAN_VIEW'        , $P_CAN_VIEW       );
		OCIDefineByName( $stmt, 'CAN_UPDATE'      , $P_CAN_UPDATE     );
		OCIDefineByName( $stmt, 'CAN_ADD'         , $P_CAN_ADD        );
		OCIDefineByName( $stmt, 'CAN_DELETE'      , $P_CAN_DELETE     );


		//Execute SQL
		OCIExecute($stmt);

		//Execute Query
		$ores     = OCIExecute($stmt);
		$is_error = false;
		$err_msg  = "SUCCESS";
		//error ?
		if(! $ores)
		{
			//Store error	
			$error    = OCIError ($stmt);
			$err_msg  = "Error: ${error['code']} ${error['message']}";
			$is_error = true;
		}

		//Loop thru all the records selected and display it in the table
		$alldata = null;
		if(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_ID            ;
			$P_ROLE_CODE     ;
			$P_MOD_NAME      ;
			$P_CAN_VIEW      ;
			$P_CAN_UPDATE    ;
			$P_CAN_ADD       ;
			$P_CAN_DELETE    ;
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Get() : [ $sql // $totalrows // $is_error / $err_msg] ");

		$alldata = array(
				'error'            => $is_error, 
				'error_msg'        => $err_msg,
				'bind_total'       => $totalrows,
				'bind_data'        => array(
					'id'           => $P_ID,
					'role_code'    => $P_ROLE_CODE,
					'mod_name'     => $P_MOD_NAME,
					'can_view'     => $P_CAN_VIEW,
					'can_update'   => $P_CAN_UPDATE,
					'can_add'      => $P_CAN_ADD,
					'can_delete'   => $P_CAN_DELETE,
					));

				return $alldata;

	}


	function Allowed($role='',$mod='',$type='can_view')
	{
		$this->debug("ALLOWED =====> $role // $mod // $type");

		$acl = $this->Get($role,$mod);
		return $acl['bind_data'][$type] > 0 ? (true) : (false);
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
