<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	User.class.php
#	@Description	:	encapsulator for table user
#
#
#	@Date		:	2009-04-30
#	@Ver		:	ver1.1
#	@Author		:	yiucon
#
#
#  @Modified Date:
#  @Modified By  :
#  @Modified Desc:
#
#----------------------------------------------------------------------------------
 **/

//include
include_once("Pager/Pager.php");
define('MAX_TIMEOUT_MINS', 20);
define("CSV_HEADER_REPORT", "User Code,Role Code, Last Name, First Name, Email, Position, Mobile No., Tel. No., Password Expiry, Lock, Created By, Date, Updated By, Date\n");

//class
class User
{

	var $_Data;

	//constructor
	function User()
	{
		//reset
		$this->_Data = null;
	}


	//init
	function Open($user='')
	{
		//globals here
		global $gEventLogger;
	
		// logon to oracle database
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();
	
		//format the insert-parameters
		$user    = addslashes(trim($user));
		$maxmin  = MAX_TIMEOUT_MINS;
		
		//update profile
		$sql     = " SELECT USER_ID,USER_CODE,PASSWORD,ROLE_CODE,PASS_EXP_N_DAYS,PASS_EXP_DT,to_char(PASS_EXP_DT,'YYYY-mm-dd HH24:MI:SS') as ALLOWED_DT, LOCKOUT, LOCKOUT_CTR,FIRST_NAME, LAST_NAME, IS_LOGIN, TO_CHAR(sysdate, 'YYYY-MM-DD HH24:MI:SS') TODI_DT , TO_CHAR(login + ($maxmin/1440), 'YYYY-MM-DD HH24:MI:SS') LOG_DT, ON_FIRST_LOGIN FROM USER_MASTER WHERE USER_CODE='$user' ";
	
		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);
	
		//total
		$totalrows = 0;
	
		$this->debug("Open() : [$sql / $stmt ]");
	
		//bind
		OCIDefineByName($stmt, "USER_ID"        , $P_USER_ID         );
		OCIDefineByName($stmt, "USER_CODE"      , $P_USER_CODE       );
		OCIDefineByName($stmt, "PASSWORD"       , $P_PASSWORD        );
		OCIDefineByName($stmt, "ROLE_CODE"      , $P_ROLE_CODE       );
		OCIDefineByName($stmt, "PASS_EXP_N_DAYS", $P_PASS_EXP_N_DAYS );
		OCIDefineByName($stmt, "PASS_EXP_DT"    , $P_PASS_EXP_DT     );
		OCIDefineByName($stmt, "ALLOWED_DT"     , $P_ALLOWED_DT      );
		OCIDefineByName($stmt, "LOCKOUT"        , $P_LOCKOUT);
		OCIDefineByName($stmt, "LOCKOUT_CTR"    , $P_LOCKOUT_CTR);
		OCIDefineByName($stmt, "FIRST_NAME"     , $P_FIRST_NAME);
		OCIDefineByName($stmt, "LAST_NAME"      , $P_LAST_NAME);
		OCIDefineByName($stmt, "IS_LOGIN"       , $P_IS_LOGIN);
		OCIDefineByName($stmt, "TODI_DT"        , $P_TODI_DT);
		OCIDefineByName($stmt, "LOG_DT"         , $P_LOG_DT);
		OCIDefineByName($stmt, "ON_FIRST_LOGIN" , $P_ON_FIRST_LOGIN);
	
		//Execute SQL
		@OCIExecute($stmt);
	
		//Execute Query
		$ores     = @OCIExecute($stmt);
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
		if(@OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_USER_ID;
			$P_USER_CODE;
			$P_PASSWORD;
			$P_ROLE_CODE;
			$P_PASS_EXP_N_DAYS;
			$P_PASS_EXP_DT;
			$P_ALLOWED_DT;
			$P_LOCKOUT;
			$P_LOCKOUT_CTR;
			$P_FIRST_NAME;
			$P_LAST_NAME;
			$P_IS_LOGIN;
			$P_TODI_DT;
			$P_LOG_DT;
			$P_ON_FIRST_LOGIN;
		}
	
		//Free Oracle connection
		@OCIFreeStatement($stmt);
		OCICommit($conn);
	
		//Log-off connection		
		OCILogoff($conn);
	
		//event
		//$gEventLogger->LogEvent(__CLASS__, "LOGIN: User=$user Status=$err_msg");
	
		$alldata = array(
				'error'            => $is_error, 
				'error_msg'        => $err_msg,
				'bind_total'       => $totalrows,
				'bind_data'        => array(
				'user_id'          => $P_USER_ID         ,
				'user_code'        => $P_USER_CODE       ,
				'password'         => $P_PASSWORD        ,
				'role_code'        => $P_ROLE_CODE       ,
				'pass_exp_n_days'  => $P_PASS_EXP_N_DAYS ,
				'allowed_dt'       => $P_ALLOWED_DT      ,
				'lockout'          => $P_LOCKOUT         ,
				'first_name'       => $P_FIRST_NAME      ,
				'last_name'        => $P_LAST_NAME       ,
				'lockout_ctr'      => $P_LOCKOUT_CTR     ,
				'pass_exp_dt'      => $P_PASS_EXP_DT     ,
				'is_login'         => $P_IS_LOGIN        ,
				'log_dt'           => $P_LOG_DT          ,
				'todi_dt'          => $P_TODI_DT         ,
				'on_first_login'   => $P_ON_FIRST_LOGIN  ,
				));
	
		return $alldata;
	}

	function LoginOk($user='')
	{
		global $gEventLogger;
		$gEventLogger->LogEvent(__CLASS__, "LOGIN: User=$user Status=OK");
	}


	//init
	function Get($id=0)
	{
		//globals here
		global $gEventLogger;

		// logon to oracle database
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$id      = intval(trim($id));

		//update profile
		$sql     = " SELECT user_id,user_code,last_name,first_name,address,email,position,department,telno,gsmnum,role_code,pass_exp_n_days,pass_exp_dt,created_by,dt_created,updated_by,dt_updated,login,logout,lockout,lockout_ctr,password,to_char(PASS_EXP_DT,'YYYY-mm-dd HH24:MI:SS') as allowed_dt, is_login FROM USER_MASTER WHERE USER_ID=$id ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("Open() : [$sql / $stmt ]");

		//bind
		OCIDefineByName( $stmt, 'USER_ID'           , $P_USER_ID          );
		OCIDefineByName( $stmt, 'USER_CODE'         , $P_USER_CODE        );
		OCIDefineByName( $stmt, 'LAST_NAME'         , $P_LAST_NAME        );
		OCIDefineByName( $stmt, 'FIRST_NAME'        , $P_FIRST_NAME       );
		OCIDefineByName( $stmt, 'ADDRESS'           , $P_ADDRESS          );
		OCIDefineByName( $stmt, 'EMAIL'             , $P_EMAIL            );
		OCIDefineByName( $stmt, 'POSITION'          , $P_POSITION         );
		OCIDefineByName( $stmt, 'DEPARTMENT'        , $P_DEPARTMENT       );
		OCIDefineByName( $stmt, 'TELNO'             , $P_TELNO            );
		OCIDefineByName( $stmt, 'GSMNUM'            , $P_GSMNUM           );
		OCIDefineByName( $stmt, 'ROLE_CODE'         , $P_ROLE_CODE        );
		OCIDefineByName( $stmt, 'PASS_EXP_N_DAYS'   , $P_PASS_EXP_N_DAYS  );
		OCIDefineByName( $stmt, 'PASS_EXP_DT'       , $P_PASS_EXP_DT      );
		OCIDefineByName( $stmt, 'CREATED_BY'        , $P_CREATED_BY       );
		OCIDefineByName( $stmt, 'DT_CREATED'        , $P_DT_CREATED       );
		OCIDefineByName( $stmt, 'UPDATED_BY'        , $P_UPDATED_BY       );
		OCIDefineByName( $stmt, 'DT_UPDATED'        , $P_DT_UPDATED       );
		OCIDefineByName( $stmt, 'LOGIN'             , $P_LOGIN            );
		OCIDefineByName( $stmt, 'LOGOUT'            , $P_LOGOUT           );
		OCIDefineByName( $stmt, 'LOCKOUT'           , $P_LOCKOUT          );
		OCIDefineByName( $stmt, 'LOCKOUT_CTR'       , $P_LOCKOUT_CTR      );
		OCIDefineByName( $stmt, 'PASSWORD'          , $P_PASSWORD         );
		OCIDefineByName( $stmt, 'ALLOWED_DT'        , $P_ALLOWED_DT       );
		OCIDefineByName( $stmt, 'IS_LOGIN'          , $P_IS_LOGIN         );
		OCIDefineByName( $stmt, 'ON_FIRST_LOGIN'    , $P_ON_FIRST_LOGIN   );

		//Execute SQL
		@OCIExecute($stmt);

		//Execute Query
		$ores     = @OCIExecute($stmt);
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
		if(@OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_USER_ID         ;
			$P_USER_CODE       ;
			$P_LAST_NAME       ;
			$P_FIRST_NAME      ;
			$P_ADDRESS         ;
			$P_EMAIL           ;
			$P_POSITION        ;
			$P_DEPARTMENT      ;
			$P_TELNO           ;
			$P_GSMNUM          ;
			$P_ROLE_CODE       ;
			$P_PASS_EXP_N_DAYS ;
			$P_PASS_EXP_DT     ;
			$P_CREATED_BY      ;
			$P_DT_CREATED      ;
			$P_UPDATED_BY      ;
			$P_DT_UPDATED      ;
			$P_LOGIN           ;
			$P_LOGOUT          ;
			$P_LOCKOUT         ;
			$P_LOCKOUT_CTR     ;
			$P_PASSWORD        ;
			$P_ALLOWED_DT      ;
			$P_ISLOGIN         ;
			$P_ON_FIRST_LOGIN  ;
		}

		//Free Oracle connection
		@OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Open() : [ $sql // $totalrows // $is_error / $err_msg] ");

		$alldata = array(
				'error'             => $is_error, 
				'error_msg'         => $err_msg,
				'bind_total'        => $totalrows,
				'bind_data'         => array(
				'user_id'           => $P_USER_ID          ,
				'user_code'         => $P_USER_CODE        ,
				'last_name'         => $P_LAST_NAME        ,
				'first_name'        => $P_FIRST_NAME       ,
				'address'           => $P_ADDRESS          ,
				'email'             => $P_EMAIL            ,
				'position'          => $P_POSITION         ,
				'department'        => $P_DEPARTMENT       ,
				'telno'             => $P_TELNO            ,
				'gsmnum'            => $P_GSMNUM           ,
				'role_code'         => $P_ROLE_CODE        ,
				'pass_exp_n_days'   => $P_PASS_EXP_N_DAYS  ,
				'pass_exp_dt'       => $P_PASS_EXP_DT      ,
				'created_by'        => $P_CREATED_BY       ,
				'dt_created'        => $P_DT_CREATED       ,
				'updated_by'        => $P_UPDATED_BY       ,
				'dt_updated'        => $P_DT_UPDATED       ,
				'login'             => $P_LOGIN            ,
				'logout'            => $P_LOGOUT           ,
				'lockout'           => $P_LOCKOUT          ,
				'lockout_ctr'       => $P_LOCKOUT_CTR      ,
				'password'          => $P_PASSWORD         ,
				'allowed_dt'        => $P_ALLOWED_DT       ,
				'is_login'          => $P_IS_LOGIN         ,
				'on_first_login'    => $P_ON_FIRST_LOGIN   ,
				) );

		return $alldata;
	}
	


	//chk fields
	function CheckFields($data=null,$ok=0)
	{
		global $gEventLogger;

		//default
		$err          = null;
		$err['error'] = false;
		$err['msg']   = null;

		//user 
		$ret = $this->IfExists($data['user_code'], $data['id'],$ok);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_RECORD_ALREADY_EXISTS;
			return $err;
		}

		//usr
		if(strlen(trim($data['user_code'])) <= 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_USER_CODE;
			return $err;
		}

		//fname		
		if((!@preg_match("/^[a-z0-9]{8}$/i", $data['password']) or !chk_pwd_valid($data['password']) ) and $ok <= 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_PASSWORD_MUST_BE_ALPHANUMERIC_AND_8CHARS_IN_LENGTH;
			return $err;
		}
		//fname		
		if(strlen(trim($data['last_name'])) <= 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_LAST_NAME;
			return $err;
		}		
		//fname		
		if(strlen(trim($data['first_name'])) <= 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_FIRST_NAME;
			return $err;
		}
		//email
		if(!check_email($data['email']) )
		{
			$err['error'] = true;
			$err['msg']   = INVALID_EMAIL_ADDRESS;
			return $err;
		}


		//ok
		return $err;

	}	

	//save
	function Save($data=null)
	{
		//globals here
		global $gEventLogger, $_SESSION;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//format the insert-parameters
		$K     = null;
		$V     = null;
		$kbuff = null;
		$vbuff = null;
		$mbuff = "";

		while( list($idx,$col) = each($data) )
		{
			$K[] = $col['name'];
			//fmt-pwd
			if($col['name'] == 'password')
			{
				$V[] = "'".ora_addslashes(trim(md5($col['value'].PWD_ENCRYPT_KEY))) . "'";
				continue;
			}
			else
			{
				if (strlen($col['value'])>0)
					$mbuff .= $col['name']."=".$col['value']." ";
			}

			if($col['data_type'] == 'string')
				$V[] = "'".ora_addslashes(trim($col['value'])) . "'";
			else
				$V[] = intval(trim($col['value']))  ;


			//fmt-exp-date
			if($col['name'] == 'pass_exp_n_days')
			{
				//colname
				$K[] = 'pass_exp_dt';
				//dt
				$dt  = ($col['value'] == 0) ? (2000) : ($col['value']);
				list($yy,$mm,$dd) = split('-', date('Y-m-d'));
				$maxdt = date("Y-m-d", mktime(0, 0, 0, $mm, $dd+$dt, $yy))."\n";

				$this->debug("#####################pass_exp_dt ===> $dt ====> $maxdt ===> ($mm, $dd+$dt, $yy)");

				$V[]   = " to_date('$maxdt','YYYY-MM-DD') ";
				continue;
			}

		}

		//created-by
		$K[] = 'created_by';
		$V[] = "'".ora_addslashes( strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system')) . "'";
		//set exp days
		$kbuff     = join(",", $K);
		$vbuff     = join(",", $V);

		//new profile
		$sql       = " INSERT INTO user_master ($kbuff, DT_CREATED) VALUES ($vbuff, sysdate )";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Save() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "NEW USER: $mbuff");

		return $data;

	}


	//update
	function Update($data=null)
	{
		//globals here
		global $gEventLogger, $_SESSION;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//format the insert-parameters
		$K     = null;
		$kbuff = null;
		$mbuff = "";
		while( list($idx,$col) = each($data) )
		{
			$this->debug("Update() : $col[name] ===> $col[value]" );

			if($col['name'] == 'id') 
			{
				$id    = intval(trim($col['value']));
				continue;
			}

			//fmt-pwd
			if($col['name'] == 'password')
			{
				//$K[] = $col['name'].  " = '".ora_addslashes(trim(md5($col['value'].PWD_ENCRYPT_KEY))) . "'";
				$K[] = $col['name']. " = password";
				continue;
			}
			else
			{
				if (strlen($col['value'])>0)
					$mbuff .= $col['name']."=".$col['value']." ";
			}

			//reset-lock
			if($col['name'] == 'lockout')
			{

				$K[] = $col['name'].  " = ".intval(trim($col['value']));
				//reset
				if($col['value'] == 0)
				{
					$K[] = " lockout_ctr = 0 ";
				}
				continue;
			}
			
			//date expiry
			if($col['name'] == 'pass_exp_n_days')
			{
                                $K[] = " pass_exp_dt = decode(pass_exp_n_days, ".intval(trim($col['value'])). ",pass_exp_dt,decode(".intval(trim($col['value'])).", 0, sysdate+1825, sysdate+".intval(trim($col['value'])).")), pass_exp_n_days=".intval(trim($col['value']));
                                continue;
			}
			

			if($col['data_type'] == 'string')
				$K[] = $col['name']. " = '".ora_addslashes(trim($col['value'])) . "'";
			else
				$K[] = $col['name']. " = ".intval(trim($col['value']));

		}
		//created-by
		$K[] = " updated_by = '".ora_addslashes(strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system'))."' ";

		$kbuff     = join(" , ", $K);

		//new profile
		$sql       = " UPDATE USER_MASTER SET DT_UPDATED=sysdate, $kbuff WHERE USER_ID=$id ";

		//exec
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Update() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "UPDATE ACCOUNT: $mbuff");

		return $data;
	}



	//update passwd
	function UpdatePassword($user='',$pass='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//format the insert-parameters
		$user  = ora_addslashes(trim($user));
		$pass  = ora_addslashes(trim($pass));
		$upd   = ora_addslashes(strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system'));

		//new profile
		$sql       = " UPDATE USER_MASTER SET password='$pass',  updated_by='$upd', DT_UPDATED=sysdate, pass_exp_dt=decode(nvl(pass_exp_n_days,0),0,sysdate+1825,sysdate+pass_exp_n_days), on_first_login=0 WHERE user_code='$user' ";

		//exec
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("UpdatePassword() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "CHANGE PASSWORD: User=$user");

		return $data;
	}


        //reset passwd
        function ResetPassword($user='',$pass='')
        {
                //globals here
                global $gEventLogger;

                //db
                $myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

                //format the insert-parameters
                $user  = ora_addslashes(trim($user));
                $pass  = ora_addslashes(trim($pass));
                $upd   = ora_addslashes(strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system'));

                //new profile
                $sql       = " UPDATE USER_MASTER SET password='$pass',  updated_by='$upd', DT_UPDATED=sysdate, pass_exp_dt=sysdate-1, lockout_ctr=0, lockout=0 WHERE user_code='$user' ";

                //exec
                $data      = $myOraDb->xQuery($sql);

                //ret
                $is_ok     = $data['error'] ? false : true;
                $err_msg   = $data['error_msg'];

                $this->debug("ResetPassword() : $sql // $is_ok // sql=$sql // $err_msg");

                //event
                $gEventLogger->LogEvent(__CLASS__, "RESET PASSWORD: User=$user");

                return $data;
        }


	function GetOldPasswords($user='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//format the insert-parameters
		$user  = ora_addslashes(trim($user));

		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT ID, USER_CODE, PASSWORD FROM PASS_LOG WHERE user_code='$user' ".
			"  ORDER BY ID DESC) b ) WHERE MROW >=0 AND MROW <= 5";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("GetOldPasswords() : [$sql / $stmt / $totalrows]");

		//bind
		OCIDefineByName($stmt,'ID'             , $P_ID         );
		OCIDefineByName($stmt,'USER_CODE'      , $P_USER_CODE  );
		OCIDefineByName($stmt,'PASSWORD'       , $P_PASSWORD   );


		//Execute SQL
		@OCIExecute($stmt);

		//Loop thru all the records selected and display it in the table
		$alldata = null;
		while (@OCIFetch($stmt))
		{
			$alldata[] =  $P_PASSWORD   ;
			
			$this->debug("GetOldPasswords() : [OLDPASS=$P_PASSWORD]");
		}



		//Free Oracle connection
		@OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		return $alldata;
	}


	//save
	function SavePasswordLog($user='',$pass='')
	{
		//globals here
		global $gEventLogger, $_SESSION;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//created-by
		$user       = ora_addslashes($user);
		$pass       = ora_addslashes($pass);
		$created_by = ora_addslashes( strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system')) ;

		//new profile
		$sql       = " INSERT INTO pass_log (user_code, password,created_by, dt_created) ".
			     " VALUES ('$user','$pass', '$created_by',sysdate )                      ";
		//exec
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("SavePasswordLog() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "SavePasswordLog() [ res=>'$is_ok' // msg=>'$err_msg'] ");

		return $data;	     
	}



	//update login
	function LoginUpdate($id=0)
	{
		//globals here
		global $gEventLogger,$_SESSION;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//format the insert-parameters
		$id        = intval(trim($id));
		$upd       = ora_addslashes(strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system'));

		//new profile
		$sql       = " UPDATE  USER_MASTER SET is_login=1, lockout_ctr=0, login=sysdate , UPDATED_BY='$upd', DT_UPDATED=sysdate WHERE USER_ID=$id ";

		//exec
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("LoginUpdate() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "LoginUpdate() [ user=>$upd // res=>'$is_ok' // msg=>'$err_msg'] ");

		return $data;	     

	}

	//update logout
	function LogoutUpdate($id=0)
	{
		//globals here
		global $gEventLogger,$_SESSION;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//format the insert-parameters
		$id        = intval(trim($id));
		$upd       = ora_addslashes(strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system'));
		
		//new profile
		$sql       = " UPDATE  USER_MASTER SET is_login=0, logout=sysdate , UPDATED_BY='$upd', DT_UPDATED=sysdate WHERE USER_ID=$id ";

		//exec
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("LoginUpdate() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "LOGOUT: User=$upd");

		return $data;	     
	}


	//update logout
	function LockoutCtrUpdate($id=0)
	{
		//globals here
		global $gEventLogger,$_SESSION;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//format the insert-parameters
		$id        = ora_addslashes(trim($id));
		$upd       = ora_addslashes(strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system'));
		
		//new profile
		$sql       = " UPDATE  USER_MASTER SET lockout_ctr=lockout_ctr+ 1 , UPDATED_BY='$upd', DT_UPDATED=sysdate WHERE USER_CODE='$id' ";

		//exec
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("LoginUpdate() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "LOGIN ERROR: User=$id");

		return $data;	     
	}


	//update logout
	function ResetLockoutCtrUpdate($id=0)
	{
		//globals here
		global $gEventLogger,$_SESSION;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//format the insert-parameters
		$id        = ora_addslashes(trim($id));
		$upd       = ora_addslashes(strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system'));
		
		//new profile
		$sql       = " UPDATE  USER_MASTER SET lockout_ctr=0 , UPDATED_BY='$upd', DT_UPDATED=sysdate WHERE USER_ID=$id ";

		//exec
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("ResetLockoutCtrUpdate() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "ResetLockoutCtrUpdate() [ res=>'$is_ok' // msg=>'$err_msg'] ");

		return $data;	     
	}


	function Lock($id=0)
	{
		//globals here
		global $gEventLogger,$_SESSION;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//format the insert-parameters
		$id        = ora_addslashes(trim($id));
		$upd       = ora_addslashes(strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system'));

		//new profile
		$sql       = " UPDATE  USER_MASTER SET lockout=1 , UPDATED_BY='$upd', DT_UPDATED=sysdate WHERE USER_CODE='$id' ";

		//exec
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Lock() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "LOCK ACCOUNT: User=$id");

		return $data;	     
	}



	//delete
	function Delete($id=0)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//format the insert-parameters
		$id        = intval(trim($id));

		//new profile
		$sql       = " DELETE FROM USER_MASTER WHERE USER_ID=$id ";
		//exec
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Delete() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "DELETE ACCOUNT: USER_ID=$id");

		return $data;	     

	}



	//get uniq
	function IfExists($c1='',$id=0,$ok=0)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//format the insert-parameters
		$c1      = ora_addslashes(trim($c1));
		$id      = intval(trim($id));

		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql       = ($id>0 and $ok) ? 
			" SELECT  COUNT(1) as TOTAL FROM USER_MASTER WHERE USER_CODE='$c1' AND ID != $id " :
			" SELECT  COUNT(1) as TOTAL FROM USER_MASTER WHERE USER_CODE='$c1' ";


		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		

		$this->debug("IfExists() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		@OCIExecute($stmt);

		//chk
		$totalrows = (@OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		@OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		$this->debug("IfExists() : // $sql // $totalrows");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "IfExists() [ $sql // $totalrows ] ");

		return $totalrows;
	}

	//get form fields
	function getTableFields()
	{
		global $_POST;
		global $gSelectOptionsMaster;
		$opts_exp  = $gSelectOptionsMaster->getPwdExpirationDays();
		$opts_role = $gSelectOptionsMaster->getRoleMaster();

		$table_fields= array(
				array(name => 'user_code'      ,  title=>'Username',             'type' => 'text',     'data_type'=>'string', 'value' => $_POST['user_code'          ]          ,'max' => '50'),          
				array(name => 'password'       ,  title=>'Password',             'type' => 'password', 'data_type'=>'string', 'value' => $_POST['password'           ]          ,'max' => '255'),
				array(name => 'last_name'      ,  title=>'Last Name',            'type' => 'text',     'data_type'=>'string', 'value' => $_POST['last_name'           ]          ,'max' => '255'),          
				array(name => 'first_name'     ,  title=>'First Name',           'type' => 'text',     'data_type'=>'string', 'value' => $_POST['first_name'          ]          ,'max' => '255'),         
				array(name => 'address'        ,  title=>'Address',              'type' => 'text',     'data_type'=>'string', 'value' => $_POST['address'              ]          ,'max' => '255'),             
				array(name => 'email'          ,  title=>'Email',                'type' => 'text',     'data_type'=>'string', 'value' => $_POST['email'              ]          ,'max' => '255'),             
				array(name => 'position'       ,  title=>'Position',             'type' => 'text',     'data_type'=>'string', 'value' => $_POST['position'           ]          ,'max' => '255'),           
				array(name => 'department'     ,  title=>'Department',           'type' => 'text',     'data_type'=>'string', 'value' => $_POST['department'         ]          ,'max' => '255'),           
				array(name => 'gsmnum'         ,  title=>'Mobile No.',           'type' => 'text',     'data_type'=>'string', 'value' => $_POST['gsmnum'             ]          ,'max' => '13'),           
				array(name => 'telno'          ,  title=>'Telephone No.',        'type' => 'text',     'data_type'=>'string', 'value' => $_POST['telno'              ]          ,'max' => '13'),           
				array(name => 'role_code'      ,  title=>'Web Role',             'type' => 'select',   'data_type'=>'string', 'value' => $_POST['role_code'          ]          ,'select_options'=>$opts_role,           'select_selected' => $_POST['role_code']),
				array(name => 'pass_exp_n_days',  title=>'Password Expire in',   'type' => 'select',   'data_type'=>'int',    'value' => $_POST['pass_exp_n_days'    ]          ,'select_options'=>$opts_exp,            'select_selected' => $_POST['pass_exp_n_days']),
				array(name => 'lockout'        ,  title=>'Account Lock',         'type' => 'check',    'data_type'=>'int',    'value' => $_POST['lockout'            ]),       
				array(name => 'is_login'       ,  title=>'Is Logged In',         'type' => 'check',    'data_type'=>'int',    'value' => $_POST['is_login'            ]),       
				array(name => 'on_first_login' ,  title=>'Change passwd on Login',         'type' => 'check',    'data_type'=>'int',    'value' => $_POST['on_first_login'            ]),       
				);



		return $table_fields;

	}


	//get form fields
	function getTableFields4Update($id="")
	{


		global $gSelectOptionsMaster;
		$opts_exp  = $gSelectOptionsMaster->getPwdExpirationDays();
		$opts_role = $gSelectOptionsMaster->getRoleMaster();
		$data      = $this->Get($id);			
		$post      = $data['bind_data'];

		$table_fields= array(
				array(name => 'user_code'      ,  title=>'Username',             'type' => 'text',     'data_type'=>'string', 'value' => $post['user_code'          ]          ,'max' => '50'),          
				array(name => 'last_name'      ,  title=>'Last Name',            'type' => 'text',     'data_type'=>'string', 'value' => $post['last_name'           ]          ,'max' => '255'),          
				array(name => 'first_name'     ,  title=>'First Name',           'type' => 'text',     'data_type'=>'string', 'value' => $post['first_name'          ]          ,'max' => '255'),         
				array(name => 'address'        ,  title=>'Address',              'type' => 'text',     'data_type'=>'string', 'value' => $post['address'            ]          ,'max' => '255'),             
				array(name => 'email'          ,  title=>'Email',                'type' => 'text',     'data_type'=>'string', 'value' => $post['email'              ]          ,'max' => '255'),             
				array(name => 'position'       ,  title=>'Position',             'type' => 'text',     'data_type'=>'string', 'value' => $post['position'           ]          ,'max' => '255'),           
				array(name => 'department'     ,  title=>'Department',           'type' => 'text',     'data_type'=>'string', 'value' => $post['department'         ]          ,'max' => '255'),           
				array(name => 'gsmnum'         ,  title=>'Mobile No.',           'type' => 'text',     'data_type'=>'string', 'value' => $post['gsmnum'             ]          ,'max' => '13'),           
				array(name => 'telno'          ,  title=>'Telephone No.',        'type' => 'text',     'data_type'=>'string', 'value' => $post['telno'              ]          ,'max' => '13'),           
				array(name => 'role_code'      ,  title=>'Web Role',             'type' => 'select',   'data_type'=>'string', 'value' => $post['role_code'          ]          ,'select_options'=>$opts_role,           'select_selected' => $post['role_code']),
				array(name => 'pass_exp_n_days',  title=>'Password Expire in',   'type' => 'select',   'data_type'=>'int',    'value' => $post['pass_exp_n_days'    ]          ,'select_options'=>$opts_exp,            'select_selected' => $post['pass_exp_n_days']),
				array(name => 'lockout'        ,  title=>'Account Lock',         'type' => 'check',    'data_type'=>'int',    'value' => $post['lockout'            ]),       
				array(name => 'is_login'       ,  title=>'Is Logged In',         'type' => 'check',    'data_type'=>'int',    'value' => $post['is_login'            ]),       
				array(name => 'on_first_login' ,  title=>'Change passwd on Login',         'type' => 'check',    'data_type'=>'int',    'value' => $post['on_first_login']),       
				);


		return $table_fields;

	}


	//list
	function getList($extravars=null)
	{

		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);


		//reset
		$retdata               = null;
		$retdata['total']      = 0;
		$retdata['data']       = null;
		$retdata['links']      = 'No result found';


		//fmt
		$whatsort    = $extravars['whatsort'];
		
		//type of order
		$ascdesc     = (!$whatsort) ? (" DESC ") : ( " ASC " );	

		$filters=0;
		$temp_code  = $_GET['code'];
		$temp_lname = $_GET['lname'];
		$temp_email = $_GET['email'];
		$temp_role  = $_GET['role_code'];
		$xwherelog  = "";

		if (strlen($temp_code) > 0) { 
			$xwhere .= " AND a.user_code like '$temp_code%' "; 
			$xwherelog .= " User=$temp_code";
			$filters++; 
		}

		if (strlen($temp_lname) > 0) { 
			$xwhere .= " AND a.last_name like '$temp_lname%' "; 
			$xwherelog .= " Lastname=$temp_lname";
			$filters++; 
		}

		if (strlen($temp_email) > 0) { 
			$xwhere .= " AND a.email like '$temp_email%' "; 
			$xwherelog .= " Email=$temp_email";
			$filters++; 
		}

		if ($temp_role != '0') { 
			$xwhere = " AND a.role_code like '$temp_role%' "; 
			$xwherelog .= " Role=$temp_role";
			$filters++; 
		}

		if ($filters > 0) {
			$xwhere = " where  a.role_code = b.role_code ". $xwhere;
		} else {
			$xwhere = " where  a.role_code = b.role_code ";
			$xwherelog = "All";
		}	

		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT COUNT(1) AS COUNT FROM USER_MASTER a, USER_ROLES b $xwhere";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"COUNT",$totalrows);		

		$this->debug("getList() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		@OCIExecute($stmt);

		//chk
		$totalrows = (@OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		@OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getList(#a) : [where=>'$xwhere'] [ total=>'$totalrows'] ");

		//chk rowcount
		if(0 == $totalrows )     
		{                   
			return $retdata;	
		}                   

		// Generate CSV File
		$csvFileName  = $this->downloadList($extravars=null);

		//-------------------------------
		//  set up pager
		//-------------------------------

		$offset        = SEARCH_MAX_OFFSET_MORE;
		$pager_options = array(
				'totalItems'   => $totalrows,
				'perPage'      => $offset,
				'delta'        => 2,             // for 'Jumping'-style a lower number is better
				'append'       => true,
				'separator'    => '&nbsp;',
				'clearIfVoid'  => false,
				'urlVar'       => 'nextpg',
				'extraVars'    => $extravars,
				'useSessions'  => true,
				'closeSession' => true,
				'httpMethod'   => 'GET',
				'mode'         => 'Sliding',    //try switching modes
				//'mode'  => 'Jumping',
				);

		$pager = Pager::factory($pager_options);
		$page = array();
		$page['totalItems']   = $pager_options['totalItems'];
		$page['links']        = $pager->links;
		$page['page_numbers'] = array(
				'current' => $pager->getCurrentPageID(),
				'total'   => $pager->numPages()
				);
		//paging here
		list($page['from'], $page['to']) = $pager->getOffsetByPageId();
		$slimit    = intval($page['from']);
		$elimit    = $slimit + intval($pager_options['perPage']);    

		//event
		$nPageNo=$pager->getCurrentPageID();
		$nPageTo=$pager->numPages();
		//event
		$gEventLogger->LogEvent(__CLASS__, "SEARCHED By: $xwherelog TOTAL Rows=$totalrows PAGE=$nPageNo/$nPageTo");

		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//orderby
		$sortedby       = $extravars['sortedby'];
		switch($sortedby)
		{
			case 'lastname':
				$orderby = " ORDER BY  a.last_name $ascdesc, a.first_name  $ascdesc";
				break;
			case 'user':
				$orderby = " ORDER BY  a.user_code $ascdesc";
				break;
			
			case 'role':
				$orderby = " ORDER BY b.role_desc $ascdesc";
				break;
			case 'email':
				$orderby = " ORDER BY a.email $ascdesc";
				break;
			case 'position':
				$orderby = " ORDER BY a.position $ascdesc";
				break;
			case 'expiry':
				$orderby = " ORDER BY a.pass_exp_dt $ascdesc";
				break;
			default:	
				$orderby = " ORDER BY a.user_id $ascdesc";
				break;
		}
		

		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT a.user_id,a.user_code,a.password,b.role_desc role_code,a.pass_exp_n_days,a.pass_exp_dt,a.lockout,".
			"                 a.first_name, a.last_name, a.email, a.position  FROM USER_MASTER a, USER_ROLES b ".
			"  $xwhere $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";

		$this->debug("getList() : get_all_rows[$sql / $stmt / $totalrows]");

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);


		//bind
		OCIDefineByName($stmt, 'USER_ID'         , $P_USER_ID          );
		OCIDefineByName($stmt, 'USER_CODE'       , $P_USER_CODE        );
		OCIDefineByName($stmt, 'PASSWORD'        , $P_PASSWORD         );
		OCIDefineByName($stmt, 'ROLE_CODE'       , $P_ROLE_CODE        );
		OCIDefineByName($stmt, 'PASS_EXP_N_DAYS' , $P_PASS_EXP_N_DAYS  );
		OCIDefineByName($stmt, 'PASS_EXP_DT'     , $P_PASS_EXP_DT      ); 		
		OCIDefineByName($stmt, 'LOCKOUT'         , $P_LOCKOUT          );
		OCIDefineByName($stmt, 'FIRST_NAME'      , $P_FIRST_NAME);
		OCIDefineByName($stmt, 'LAST_NAME'       , $P_LAST_NAME);
		OCIDefineByName($stmt, 'EMAIL'           , $P_EMAIL);
		OCIDefineByName($stmt, 'POSITION'        , $P_POSITION);


		//Execute SQL
		@OCIExecute($stmt);

		//Loop thru all the records selected and display it in the table
		$idx = 0;
		while (@OCIFetch($stmt))
		{
			$alldata[$idx]['user_id'         ] =  $P_USER_ID          ;
			$alldata[$idx]['user_code'       ] =  $P_USER_CODE        ;
			$alldata[$idx]['password'        ] =  $P_PASSWORD         ;
			$alldata[$idx]['role_code'       ] =  $P_ROLE_CODE        ;
			$alldata[$idx]['pass_exp_n_days' ] =  $P_PASS_EXP_N_DAYS  ;
			$alldata[$idx]['pass_exp_dt'     ] =  $P_PASS_EXP_DT	  ; 		
			$alldata[$idx]['lockout'         ] =  $P_LOCKOUT          ;
			$alldata[$idx]['first_name'      ] =  $P_FIRST_NAME       ;
			$alldata[$idx]['last_name'       ] =  $P_LAST_NAME        ;
			$alldata[$idx]['email'           ] =  $P_EMAIL            ;
			$alldata[$idx]['position'        ] =  $P_POSITION         ;
			$idx++;
		}



		//Free Oracle connection
		@OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//save
		$retdata['data']      = $alldata;
		$retdata['total']     = $totalrows ;

		//return CSV Filename
                $retdata['csv']       = $csvFileName;

		//no rec
		if($totalrows <=0 )
		{
			$retdata['links'] = 'No result found';
		}
		else
		{
			$navigator            = $page['links'];
			$retdata['links']     = $navigator;
		}				 

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getList(#b) : [ total=>'$totalrows'] ");

		//return data
		return $retdata;
	}


	//download
	function downloadList($extravars=null)
	{

		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);


		//fmt
		$whatsort    = $extravars['whatsort'];
		
		//type of order
		$ascdesc     = (!$whatsort) ? (" DESC ") : ( " ASC " );	

		$filters=0;
		$temp_code  = $_GET['code'];
		$temp_lname = $_GET['lname'];
		$temp_email = $_GET['email'];
		$temp_role  = $_GET['role_code'];
		$xwherelog  = "";

		if (strlen($temp_code) > 0) { 
			$xwhere .= " AND a.user_code like '$temp_code%' "; 
			$xwherelog .= " User=$temp_code";
			$filters++; 
		}

		if (strlen($temp_lname) > 0) { 
			$xwhere .= " AND a.last_name like '$temp_lname%' "; 
			$xwherelog .= " Lastname=$temp_lname";
			$filters++; 
		}

		if (strlen($temp_email) > 0) { 
			$xwhere .= " AND a.email like '$temp_email%' "; 
			$xwherelog .= " Email=$temp_email";
			$filters++; 
		}

		if ($temp_role != '0') { 
			$xwhere = " AND a.role_code like '$temp_role%' "; 
			$xwherelog .= " Role=$temp_role";
			$filters++; 
		}

		if ($filters > 0) {
			$xwhere = " where  a.role_code = b.role_code ". $xwhere;
		} else {
			$xwhere = " where  a.role_code = b.role_code ";
			$xwherelog = "All";
		}	

		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//orderby
		$sortedby       = $extravars['sortedby'];
		switch($sortedby)
		{
			case 'lastname':
				$orderby = " ORDER BY  a.last_name $ascdesc, a.first_name  $ascdesc";
				break;
			case 'user':
				$orderby = " ORDER BY  a.user_code $ascdesc";
				break;
			
			case 'role':
				$orderby = " ORDER BY b.role_desc $ascdesc";
				break;
			case 'email':
				$orderby = " ORDER BY a.email $ascdesc";
				break;
			case 'position':
				$orderby = " ORDER BY a.position $ascdesc";
				break;
			case 'expiry':
				$orderby = " ORDER BY a.pass_exp_dt $ascdesc";
				break;
			default:	
				$orderby = " ORDER BY a.user_id $ascdesc";
				break;
		}
		

		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT a.user_id,a.user_code,a.password,b.role_desc role_code,".
			"   decode(a.pass_exp_n_days, 0,'Never Expires', 7,'1 Week', 30,'1 Month', to_char(a.pass_exp_n_days) || ' Days') pass_exp_n_days, ".
			"   to_char(a.pass_exp_dt, 'DD-MON-YYYY HH24:MI:SS') pass_exp_dt, decode(a.lockout,0,'No','Yes') lockout, a.gsmnum, a.telno, a.updated_by, to_char(a.dt_updated, 'DD-MON-YYYY HH24:MI:SS') dt_updated, ".
			"   a.first_name, a.last_name, a.email, a.position, a.created_by, to_char(a.dt_created, 'DD-MON-YYYY HH24:MI:SS') dt_created".
			"  FROM USER_MASTER a, USER_ROLES b ".
			"  $xwhere $orderby ) b )";

		$this->debug("downloadList() : get_all_rows[$sql / $stmt]");

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//bind
		OCIDefineByName($stmt, 'USER_ID'         , $P_USER_ID          );
		OCIDefineByName($stmt, 'USER_CODE'       , $P_USER_CODE        );
		OCIDefineByName($stmt, 'PASSWORD'        , $P_PASSWORD         );
		OCIDefineByName($stmt, 'ROLE_CODE'       , $P_ROLE_CODE        );
		OCIDefineByName($stmt, 'PASS_EXP_N_DAYS' , $P_PASS_EXP_N_DAYS  );
		OCIDefineByName($stmt, 'PASS_EXP_DT'     , $P_PASS_EXP_DT      ); 		
		OCIDefineByName($stmt, 'LOCKOUT'         , $P_LOCKOUT          );
		OCIDefineByName($stmt, 'FIRST_NAME'      , $P_FIRST_NAME       );
		OCIDefineByName($stmt, 'LAST_NAME'       , $P_LAST_NAME        );
		OCIDefineByName($stmt, 'EMAIL'           , $P_EMAIL            );
		OCIDefineByName($stmt, 'POSITION'        , $P_POSITION         );
		OCIDefineByName($stmt, 'GSMNUM'          , $P_GSMNUM           );
		OCIDefineByName($stmt, 'TELNO'           , $P_TELNO            );
		OCIDefineByName($stmt, 'CREATED_BY'      , $P_CREATED_BY       );
		OCIDefineByName($stmt, 'DT_CREATED'      , $P_DT_CREATED       );
		OCIDefineByName($stmt, 'UPDATED_BY'      , $P_UPDATED_BY       );
		OCIDefineByName($stmt, 'DT_UPDATED'      , $P_DT_UPDATED       );

		//Execute SQL
		@OCIExecute($stmt);

                //open csv here
                $csvfnam = date("Ymd").'.'.get_uid().".csv";
                $csvfile = SMARTY_INC_DIR."/log/$csvfnam";
                $csvcont = '';

		//Loop thru all the records selected and display it in the table
		while (@OCIFetch($stmt))
		{
                        //csv-create
                        $csvdata   = null;
                        $csvdata[] = $P_USER_CODE       ;
                        $csvdata[] = $P_ROLE_CODE       ;
                        $csvdata[] = $P_LAST_NAME       ;
                        $csvdata[] = $P_FIRST_NAME      ;
                        $csvdata[] = $P_EMAIL           ;
                        $csvdata[] = $P_POSITION        ;
                        $csvdata[] = $P_GSMNUM          ;
                        $csvdata[] = $P_TELNO           ;
                        $csvdata[] = $P_PASS_EXP_N_DAYS ;
                        $csvdata[] = $P_LOCKOUT         ;
                        $csvdata[] = $P_CREATED_BY      ;
                        $csvdata[] = $P_DT_CREATED      ;
                        $csvdata[] = $P_UPDATED_BY      ;
                        $csvdata[] = $P_DT_UPDATED      ;
                        $csvstr    = join('","', $csvdata);
			$csvcont .= "\"$csvstr\"\n";
		}

                //close csv
		//"User Code,Role Code, Last Name, First Name, Email, Position, Mobile No., Tel. No., Password Expiry, Lock, Created By, Date, Updated By, Date\n
                //file_save($csvfile,CSV_HEADER_REPORT."\n\n".$csvcont);
                $rep_gen_dt = date("Y-m-d H:i:s");
                file_save($csvfile,"List of Users\nReport Generation: $rep_gen_dt\n\n".CSV_HEADER_REPORT."\n\n".$csvcont);

		//Free Oracle connection
		@OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getList(#b) : [ total=>'$totalrows'] ");

		//return data
		return $csvfnam;
	}




	//list
	function getListUserPassLog($extravars=null)
	{

		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);


		//reset
		$retdata               = null;
		$retdata['total']      = 0;
		$retdata['data']       = null;
		$retdata['links']      = 'No result found';


		//fmt
		$whatsort    = $extravars['whatsort'];
		
		//type of order
		$ascdesc     = (!$whatsort) ? (" ASC ") : ( " DESC " );	


		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT COUNT(1) AS COUNT FROM PASS_LOG ";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"COUNT",$totalrows);		

		$this->debug("getListUserPassLog() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		@OCIExecute($stmt);

		//chk
		$totalrows = (@OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		@OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getListUserPassLog(#a) : [ total=>'$totalrows'] ");

		//chk rowcount
		if(0 == $totalrows )     
		{                   
			return $retdata;	
		}                   

		//-------------------------------
		//  set up pager
		//-------------------------------

		$offset        = SEARCH_MAX_OFFSET_MORE;
		$pager_options = array(
				'totalItems'   => $totalrows,
				'perPage'      => $offset,
				'delta'        => 2,             // for 'Jumping'-style a lower number is better
				'append'       => true,
				'separator'    => '&nbsp;',
				'clearIfVoid'  => false,
				'urlVar'       => 'nextpg',
				'extraVars'    => $extravars,
				'useSessions'  => true,
				'closeSession' => true,
				'httpMethod'   => 'GET',
				'mode'         => 'Sliding',    //try switching modes
				//'mode'  => 'Jumping',
				);

		$pager = Pager::factory($pager_options);
		$page = array();
		$page['totalItems']   = $pager_options['totalItems'];
		$page['links']        = $pager->links;
		$page['page_numbers'] = array(
				'current' => $pager->getCurrentPageID(),
				'total'   => $pager->numPages()
				);
		//paging here
		list($page['from'], $page['to']) = $pager->getOffsetByPageId();
		$slimit    = intval($page['from']);
		$elimit    = $slimit + intval($pager_options['perPage']);    


		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//orderby
		$sortedby       = $extravars['sortedby'];
		switch($sortedby)
		{
			case 'user':
				$orderby = " ORDER BY  USER_CODE  $ascdesc";
				break;
			case 'date':
				$orderby = " ORDER BY  DT_CREATED $ascdesc";
				break;
			default:	
				$orderby = " ORDER BY id $ascdesc";
				break;
		}
		

		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT  id, user_code, password, created_by, dt_created ".
			"  FROM USER_MASTER  ".
			"  $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("getListUserPassLog() : get_all_rows[$sql / $stmt / $totalrows]");


		//bind
		OCIDefineByName($stmt, 'ID'          , $p_id          );
		OCIDefineByName($stmt, 'USER_CODE'   , $p_user_code   );
		OCIDefineByName($stmt, 'PASSWORD'    , $p_password    );
		OCIDefineByName($stmt, 'CREATED_BY'  , $p_created_by  );
		OCIDefineByName($stmt, 'DT_CREATED'  , $p_dt_created  );

		//Execute SQL
		@OCIExecute($stmt);

		//Loop thru all the records selected and display it in the table
		$idx = 0;
		while (@OCIFetch($stmt))
		{
			$alldata[$idx]['id'          ] =  $p_id          ;
			$alldata[$idx]['user_code'   ] =  $p_user_code   ;
			$alldata[$idx]['password'    ] =  $p_password    ;
			$alldata[$idx]['created_by'  ] =  $p_created_by  ;
			$alldata[$idx]['dt_created'  ] =  $p_dt_created  ;

			$idx++;
		}



		//Free Oracle connection
		@OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//save
		$retdata['data']      = $alldata;
		$retdata['total']     = $totalrows ;

		//no rec
		if($totalrows <=0 )
		{
			$retdata['links'] = 'No result found';
		}
		else
		{
			$navigator            = $page['links'];
			$retdata['links']     = $navigator;
		}				 

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getListUserPassLog(#b) : [ total=>'$totalrows'] ");

		//return data
		return $retdata;
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
