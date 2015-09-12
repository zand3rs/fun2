<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	Enrollment.class.php
#	@Description:	encapsulator for table Enrollment_log
#
#
#	@Date		:	2009-05-25
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


//class
class Enrollment
{

	//constructor
	function Enrollment()
	{

	}

	//activation
	function Activation($gsmnum=null, $startdt=null, $duration=null, $remarks=null, $balchk=null, $bperiod=null)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		$vGsmnum = ora_addslashes($gsmnum);
		$dStartdt = "to_date('".ora_addslashes($startdt)."', 'YYYY-MM-DD')";
		$nDuration = ora_addslashes($duration);
		if ($nDuration == '') {
			$nDuration = 0;
		}
		$vRemarks = "'".ora_addslashes($remarks)."'";
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";

		//new profile
		$sql       = "INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, customer_type, request_origin ) ". 
			     "VALUES (manual_enrollment_log_seq.nextval, $vGsmnum, 'ACTIVATION', 0, $dStartdt, $nDuration, 0, $vCreated_by, sysdate, NULL, 'TRIGGER', NULL, $vRemarks, NULL, sf_get_min_bal('TRIGGER', 0, $balchk), $bperiod, 'TRIGGER', 'WEB' ) ";
                      	
		//exect
		$data      = $myOraDb->xQuery($sql);
		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Activation() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Activation() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");
                if ($balchk=='1')
                   if ($bperiod=='1')
			$gEventLogger->LogEvent("Manual Activation", "MSISDN=$gsmnum TXDATE=$startdt Duration=$nDuration Minbal=Override BreathingPeriod=Override REMARKS=$remarks ");
		   else
			$gEventLogger->LogEvent("Manual Activation", "MSISDN=$gsmnum TXDATE=$startdt Duration=$nDuration Minbal=Override BreathingPeriod=Regular REMARKS=$remarks ");
		else
                   if ($bperiod=='1')
			$gEventLogger->LogEvent("Manual Activation", "MSISDN=$gsmnum TXDATE=$startdt Duration=$nDuration Minbal=Regular BreathingPeriod=Override REMARKS=$remarks ");
		   else
			$gEventLogger->LogEvent("Manual Activation", "MSISDN=$gsmnum TXDATE=$startdt Duration=$nDuration Minbal=Regular BreathingPeriod=Regular REMARKS=$remarks ");

		return $data;
	}

	//deactivation
	function Deactivation($gsmnum=null, $profile=null, $name=null, $contact=null, $remarks=null)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		$vGsmnum  = ora_addslashes($gsmnum);
		$vProfile = "'".ora_addslashes($profile)."'";
		$vName    = "'".ora_addslashes($name)."'";
		$vContact = "'".ora_addslashes($contact)."'";
		$vRemarks = "'".ora_addslashes($remarks)."'";
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";

		//new profile
		$sql       = "INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, customer_type, request_origin ) ". 
			     "VALUES (manual_enrollment_log_seq.nextval, $vGsmnum, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, $vCreated_by, sysdate, $vName, 'TRIGGER', $vContact, $vRemarks, NULL, 'TRIGGER', 'WEB') ";
                      	
		//exect
		$data      = $myOraDb->xQuery($sql);
		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Deactivation() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent("Manual Deactivation", "MSISDN=$gsmnum Name=$name ContactNo=$contact REMARKS=$remarks ");


		return $data;
	}

	//extension
	function Extension($gsmnum=null, $duration=null, $name=null, $contact=null, $remarks=null, $balchk=null, $bperiod=null)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		$vGsmnum  = ora_addslashes($gsmnum);
		if (strlen($duration)>0) 
			$nDuration = ora_addslashes($duration);
		else
			$nDuration = 0;
		$vName    = "'".ora_addslashes($name)."'";
		$vContact = "'".ora_addslashes($contact)."'";
		$vRemarks = "'".ora_addslashes($remarks)."'";
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";

		//new profile
		$sql       = "INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, min_bal, breathing_period, customer_type, request_origin ) ". 
			     "VALUES (manual_enrollment_log_seq.nextval, $vGsmnum, 'EXTENSION', 0, trunc(sysdate), $nDuration, 0, $vCreated_by, sysdate, $vName, 'TRIGGER', $vContact, $vRemarks, sf_get_min_bal('TRIGGER', 1, $balchk), $bperiod, 'TRIGGER', 'WEB' ) ";
                      	
		//exect
		$data      = $myOraDb->xQuery($sql);
		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Extension() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Activation() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");
                if ($balchk=='1')
                   if ($bperiod=='1')
			$gEventLogger->LogEvent("Manual Extension", "MSISDN=$gsmnum Name=$name Duration=$nDuration Minbal=Override BreathingPeriod=Override ContactNo=$contact REMARKS:$remarks ");
		   else
			$gEventLogger->LogEvent("Manual Extension", "MSISDN=$gsmnum Name=$name Duration=$nDuration Minbal=Override BreathingPeriod=Regular ContactNo=$contact REMARKS:$remarks ");
		else
                   if ($bperiod=='1')
			$gEventLogger->LogEvent("Manual Extension", "MSISDN=$gsmnum Name=$name Duration=$nDuration Minbal=Regular BreathingPeriod=Override ContactNo=$contact REMARKS=$remarks ");
		   else
			$gEventLogger->LogEvent("Manual Extension", "MSISDN=$gsmnum Name=$name Duration=$nDuration Minbal=Regular BreathingPeriod=Regular ContactNo=$contact REMARKS=$remarks ");

		return $data;
	}

	//activation
	function Reactivation($gsmnum=null, $remarks=null)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		$vGsmnum = ora_addslashes($gsmnum);
		$vRemarks = "'".ora_addslashes($remarks)."'";
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";

		//new profile
		$sql       = "INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, customer_type, request_origin, other_type ) ". 
			     "SELECT manual_enrollment_log_seq.nextval, msisdn, 'ACTIVATION', 0, trunc(sysdate), duration, 0, $vCreated_by, sysdate, NULL, 'TRIGGER', NULL, $vRemarks, NULL, min_bal, breathing_period, 'TRIGGER', 'WEB', 'REACTIVATION' ".
			     "FROM   sim_activation ".
			     "WHERE  msisdn = $vGsmnum";
                      	
		//exect
		$data      = $myOraDb->xQuery($sql);
		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Reactivation() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Activation() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");
		$gEventLogger->LogEvent("Manual Reactivation", "MSISDN=$gsmnum REMARKS=$remarks ");

		return $data;
	}

	//blacklist
	function Blacklist($gsmnum=null, $remarks=null)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		$vGsmnum  = ora_addslashes($gsmnum);

		$vRemarks  = "'".ora_addslashes($remarks)."'";
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";

                //new profile
                $sql       = "DELETE FROM WHITELISTED_LOG WHERE MSISDN = $vGsmnum";
                //exect
                $data      = $myOraDb->xQuery($sql);

		//new profile
		$sql       = "INSERT INTO BLACKLISTED_LOG ( MSISDN, TRAN_DT, CREATED_BY, DT_CREATED, REMARKS ) ". 
			     "VALUES ($vGsmnum, trunc(sysdate), $vCreated_by, sysdate, $vRemarks ) ";
                      	
		//exect
		$data      = $myOraDb->xQuery($sql);
		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Blacklist() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent("Manual Blacklisting", "MSISDN=$gsmnum REMARKS=$remarks ");

		return $data;
	}

        //whitelist
        function Whitelist($gsmnum=null, $remarks=null)
        {
                //globals here
                global $gEventLogger;

                //db
                $myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

                $vGsmnum  = ora_addslashes($gsmnum);
                $vRemarks  = "'".ora_addslashes($remarks)."'";
                $vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";

                //new profile
		$sql       = "DELETE FROM BLACKLISTED_LOG WHERE MSISDN = $vGsmnum";
                //exect
                $data      = $myOraDb->xQuery($sql);

                //new profile
		$sql       = "INSERT INTO WHITELISTED_LOG ( MSISDN, TRAN_DT, CREATED_BY, DT_CREATED, REMARKS ) ". 
			     "VALUES ($vGsmnum, trunc(sysdate), $vCreated_by, sysdate, $vRemarks ) ";

                //exect
                $data      = $myOraDb->xQuery($sql);
                //ret
                $is_ok     = $data['error'] ? false : true;
                $err_msg   = $data['error_msg'];

                $this->debug("list() : $sql // $is_ok // $err_msg");

                //event
		$gEventLogger->LogEvent("Manual Whitelisting", "MSISDN=$vGsmnum REMARKS=$remarks ");

                return $data;
        }

	//activation
	function FunActivation($gsmnum=null, $linkto=null, $remarks=null, $nBalChk=null )
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		$vGsmnum = ora_addslashes($gsmnum);
		$vLinkTo = ora_addslashes($linkto);
		$vRemarks = "'".ora_addslashes($remarks)."'";
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";

		//new profile
		$sql       = "INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, LINK_TO, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, customer_type, request_origin ) ". 
			     "VALUES (manual_enrollment_log_seq.nextval, $vLinkTo, $vGsmnum, 'REG', 0, trunc(sysdate), 0, 0, $vCreated_by, sysdate, NULL, 'FUN', NULL, $vRemarks, NULL, sf_get_min_bal('FUN', 0, $nBalChk), 0, 'FUN', 'WEB' ) ";
		//exect
		$data      = $myOraDb->xQuery($sql);
		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("FunActivation() : $sql // $is_ok // $err_msg");

		//event
                if ($nBalChk=1)
			$gEventLogger->LogEvent("Manual Fun Activation", "MSISDN=$vLinkTo LinkTo=$vGsmnum Minbal=Override REMARKS=$remarks ");
		else
			$gEventLogger->LogEvent("Manual Fun Activation", "MSISDN=$vLinkTo LinkTo=$vGsmnum Minbal=Regular REMARKS=$remarks ");


		return $data;
	}

	//cancel service
	function CancelSvc($gsmnum=null, $linkto=null, $profile=null, $remarks=null)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//user 
		$ret = $this->isBlacklisted($vGsmnum);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = MSISDN_IS_ALREADY_BLACKLISTED;
			return $err;
		}

		$vGsmnum  = ora_addslashes($gsmnum);
		$vLinkTo = ora_addslashes($linkto);
		$vRemarks = "'".ora_addslashes($remarks)."'";
		$vProfile = "'".ora_addslashes($profile)."'";
                $vRemarks  = "'".ora_addslashes($remarks)."'";
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";

		//new profile
		$sql       = "INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, LINK_TO, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, customer_type, request_origin ) ". 
			     "VALUES (manual_enrollment_log_seq.nextval, $vGsmnum, $vLinkTo, 'CANCEL', 0, trunc(sysdate), 0, 0, $vCreated_by, sysdate, null, 'FUN', null, $vRemarks, NULL, 'FUN', 'WEB' ) ";
                      	
		//exect
		$data      = $myOraDb->xQuery($sql);
		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("CancelSvc() : $sql // $is_ok // $err_msg");

		//event
			$gEventLogger->LogEvent("Manual Fun Deactivation", "Manual FUN DEACTIVATION: MSISDN=$vGsmnum REMARKS=$remarks ");

		return $data;
	}

	//retry service
	function RetrySvc($gsmnum=null, $profile=null, $remarks=null)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		$vGsmnum  = "'".ora_addslashes($gsmnum)."'";

		//user 
		$ret = $this->isBlacklisted($vGsmnum);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = MSISDN_IS_ALREADY_BLACKLISTED;
			return $err;
		}

		$vProfile = "'".ora_addslashes($profile)."'";
                $vRemarks  = "'".ora_addslashes($remarks)."'";
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";

		//new profile
		$sql       = "INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, request_origin ) ". 
			     "VALUES (manual_enrollment_log_seq.nextval, $vGsmnum, 'RETRY', 0, trunc(sysdate), 0, 0, $vCreated_by, sysdate, null, 'TRIGGER', null, $vRemarks, NULL, 'WEB' ) ";
                      	
		//exect
		$data      = $myOraDb->xQuery($sql);
		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("RetrySvc() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent("Manual Retry of Service", "MSISDN=$vGsmnum REMARKS=$remarks ");

		return $data;
	}

	//get form fields
	function getActivationFields()
	{
		global $_POST;
		global $gSelectOptionsMaster;
		$opts_status = $gSelectOptionsMaster->getStatus();

		$table_fields= array(
				array('name' => 'msisdn',     'title'=>'MSISDN',      'type' => 'text',    'max'=> 16,   'data_type'=> 'string', 'value' => $_POST['msisdn'] ),
				array('name' => 'tx_date',    'title'=>'Start Date',  'type' => 'text',    'max'=> 14,   'data_type'=> 'string', 'value' => $_POST['tx_date'] ),
				array('name' => 'duration' ,  'title'=>'Duration',    'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['duration'] ),
				array('name' => 'profile' ,   'title'=>'Profile',     'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $_POST['profile'], 'select_options'=>$opts_status, 'select_selected' => $_POST['profile'] ),
				);

		return $table_fields;

	}

	//get uniq
	function isBlacklisted($c1='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));

		//update profile
		$sql       = " SELECT COUNT(1) as TOTAL FROM blacklisted_log WHERE MSISDN = $c1 ";
		
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		

		$this->debug("getList() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		$this->debug("isBlacklisted() : // $sql // $totalrows");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "isBlacklisted() [ $sql // $totalrows ] ");

		return $totalrows;


	}

	//get uniq
	function isValidGlobe($c1='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));

		//update profile
		$sql       = " SELECT sf_is_in_hlr_range($c1) as TOTAL FROM dual";
		
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		

		$this->debug("getList() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		$this->debug("isValidGlobe() : // $sql // $totalrows");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "isValidGlobe() [ $sql // $totalrows ] ");

		return $totalrows;


	}

	//get uniq
	function isValidActivationDt($c1='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));

		//update profile
		$sql       = " SELECT sf_is_valid_activation_dt('$c1', 'TRIGGER') as TOTAL FROM dual";
		
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		

		$this->debug("getList() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		$this->debug("isValidActivationDt() : // $sql // $totalrows");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "isValidActivationDt() [ $sql // $totalrows ] ");

		return $totalrows;


	}

	//check if already roamer
	function chkRoamer($c1='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));

		//update profile
		$sql       = " SELECT COUNT(1) as TOTAL FROM sim_activation WHERE MSISDN = $c1 and status<>'INACTIVE' ";
		
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		

		$this->debug("chkRoamer() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		$this->debug("chkRoamer() : // $sql // $totalrows");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "chkRoamer() [ $sql // $totalrows ] ");

		return $totalrows;


	}

	//check if already FUN member
	function chkFun($c1='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));

		//update profile
		$sql       = " SELECT COUNT(1) as TOTAL FROM sim_activation_links WHERE MSISDN = $c1 and status<>'INACTIVE' ";
		
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		

		$this->debug("chkFun() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		$this->debug("chkFun() : // $sql // $totalrows");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "chkFun() [ $sql // $totalrows ] ");

		return $totalrows;


	}

	//check if already FUN member
	function chkFunLocal($c1='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));

		//update profile
		$sql       = " SELECT COUNT(1) as TOTAL FROM sim_activation_links WHERE LINK_TO = $c1 and status<>'INACTIVE' ";
		
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		

		$this->debug("chkFunLocal() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		$this->debug("chkFunLocal() : // $sql // $totalrows");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "chkFun() [ $sql // $totalrows ] ");

		return $totalrows;


	}

	//check if already roamer
	function chkExtensionAllowedDuration($c1='', $c2='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));
		$c2        = ora_addslashes(trim($c2));
		$sql       = " SELECT sf_validate_ext_duration ($c1,  '$c2') TOTAL FROM DUAL ";
		
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);
		$totalrows = 0;	
		OCIDefineByName($stmt,"TOTAL",$totalrows);		
		OCIExecute($stmt);
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);
		OCILogoff($conn);

		$this->debug("chkExtensionAllowedDuration() : // $sql // $totalrows");
		return $totalrows;
	}

	//check if already FUN member
	function chkFunMaxLinkReached($c1='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();
		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));
		//update profile
		$sql       = " SELECT sf_is_max_link_reached($c1) TOTAL FROM dual ";
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);
		//reset	
		$totalrows = 0;	
		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		
		$this->debug("chkFunMaxLinkReached() : get_max_row[$sql / $stmt / $totalrows]");
		//Execute Query
		OCIExecute($stmt);
		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);
		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);
		// logoff connection
		OCILogoff($conn);
		$this->debug("chkFunMaxLinkReached() : // $sql // $totalrows");

		return $totalrows;
	}

	//check if already roamer
	function chkPendingRequest($c1='',$c2='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));

		//update profile
		$sql       = " SELECT COUNT(1) as TOTAL FROM manual_enrollment_log WHERE MSISDN = $c1 and tx_date >= trunc(sysdate) and enrollment_type='$c2' and status=0 ";
		
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		

		$this->debug("chkPendingRequest() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		$this->debug("chkPendingRequest() : // $sql // $totalrows");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "chkPendingRequest() [ $sql // $totalrows ] ");

		return $totalrows;


	}

	//check if valid for reactivation
	function chkValidForReactivation($c1='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));
		$sql       = " SELECT COUNT(1) as TOTAL FROM sim_activation WHERE MSISDN = $c1 and status='INACTIVE' and remarks = 'Due to Insuffbal' ";
		
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		
		$this->debug("chkFun() : get_max_row[$sql / $stmt / $totalrows]");
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);
		OCILogoff($conn);

		$this->debug("chkFun() : // $sql // $totalrows");
		return $totalrows;
	}

	//init
	function Get($id='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$id      = intval (trim($id));


		$mwhere  = " WHERE a.id = $id ";
		//format the insert-parameters
		$mselect = "  SELECT id tran_id, a_no, b_no, msg, step_no, last_step_no, status, customer_type, tran_type, ".
                           "         to_char(tran_dt,'YYYY-MM-DD') tran_dt, to_char(dt_created,'YYYY-MM-DD HH24MISS') dt_created, ".
		           "         txid, ref_id, min_bal, step_seq, request_origin ".
		           "  FROM REQUEST_LOG a $mwhere ";
		$sql     = "  $mselect ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("GetSubscriber() a#: [$sql / $stmt ]");

		//bind
                OCIDefineByName($stmt, "TRAN_ID"          , $P_TRAN_ID       );
                OCIDefineByName($stmt, "A_NO"             , $P_A_NO          );
                OCIDefineByName($stmt, "B_NO"             , $P_B_NO          );
                OCIDefineByName($stmt, "MSG"              , $P_MSG           );
                OCIDefineByName($stmt, "STEP_NO"          , $P_STEP_NO       );
                OCIDefineByName($stmt, "LAST_STEP_NO"     , $P_LAST_STEP_NO  );
                OCIDefineByName($stmt, "STATUS"           , $P_STATUS        );
                OCIDefineByName($stmt, "TRAN_TYPE"        , $P_TRAN_TYPE     );
                OCIDefineByName($stmt, "TRAN_DT"          , $P_TRAN_DT       );
                OCIDefineByName($stmt, "DT_CREATED"       , $P_DT_CREATED    );
                OCIDefineByName($stmt, "TXID"             , $P_TXID          );
                OCIDefineByName($stmt, "REF_ID"           , $P_REF_ID        );
                OCIDefineByName($stmt, "MIN_BAL"          , $P_MIN_BAL       );
                OCIDefineByName($stmt, "STEP_SEQ"         , $P_STEP_SEQ      );
                OCIDefineByName($stmt, "REQUEST_ORIGIN"   , $P_REQUEST_ORIGIN);

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
                        $P_TRAN_ID       ;
                        $P_A_NO          ;
                        $P_B_NO          ;
                        $P_MSG           ;
                        $P_STEP_NO       ;
                        $P_LAST_STEP_NO  ;
                        $P_STATUS        ;
                        $P_TRAN_TYPE     ;
                        $P_TRAN_DT       ;
                        $P_DT_CREATED    ;
                        $P_TXID          ;
                        $P_REF_ID        ;
                        $P_MIN_BAL       ;
                        $P_STEP_SEQ      ;
                        $P_REQUEST_ORIGIN;
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Get() : [ $sql // $totalrows // $is_error / $err_msg] ");
		$this->debug("Get b#() : [$totalrows // $is_error / $err_msg] ]");

                $alldata = array(
                                'error'         => $is_error,
                                'error_msg'     => $err_msg,
                                'bind_total'    => $totalrows,
                                'bind_data'     => array(
                                'tran_id'       => $P_TRAN_ID         ,
                                'a_no'          => $P_A_NO            ,
                                'b_no'          => $P_B_NO            ,
                                'msg'           => $P_MSG             ,
                                'step_no'       => $P_STEP_NO         ,
                                'last_step_no'  => $P_LAST_STEP_NO    ,
                                'status'        => $P_STATUS          ,
                                'tran_type'     => $P_TRAN_TYPE       ,
                                'tran_dt'       => $P_TRAN_DT         ,
                                'dt_created'    => $P_DT_CREATED      ,
                                'txid'          => $P_TXID            ,
                                'ref_id'        => $P_REF_ID          ,
                                'min_bal'       => $P_MIN_BAL         ,
                                'step_seq'      => $P_STEP_SEQ        ,
                                'request_origin'=> $P_REQUEST_ORIGIN  ,
                                ));


		return $alldata;

	}


	//get form fields
	function getTableFields4Update($id="")
	{
		$data = $this->Get($id);
		$post = $data['bind_data'];

                $table_fields= array(
                                array('name' => 'tran_id'       , 'title'=>'TxID'              , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['tran_id'       ] ),
                                array('name' => 'a_no'          , 'title'=>'A No'              , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['a_no'          ] ),
                                array('name' => 'b_no'          , 'title'=>'B No'              , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['b_no'          ] ),
                                array('name' => 'msg'           , 'title'=>'Message'           , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['msg'           ] ),
                                array('name' => 'step_no'       , 'title'=>'Step No'           , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['step_no'       ] ),
                                array('name' => 'last_step_no'  , 'title'=>'Last Step No'      , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['last_step_no'  ] ),
                                array('name' => 'status'        , 'title'=>'Status'            , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['status'        ] ),
                                array('name' => 'tran_type'     , 'title'=>'Tran Type'         , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['tran_type'     ] ),
                                array('name' => 'tran_dt'       , 'title'=>'Tran Date'         , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['tran_dt'       ] ),
                                array('name' => 'dt_created'    , 'title'=>'Date Created'      , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['dt_created'    ] ),
                                array('name' => 'txid'          , 'title'=>'Tx ID'             , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['txid'          ] ),
                                array('name' => 'ref_id'        , 'title'=>'Ref ID'            , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['ref_id'        ] ),
                                array('name' => 'min_bal'       , 'title'=>'Min Bal'           , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['min_bal'       ] ),
                                array('name' => 'step_seq'      , 'title'=>'Step Seq'          , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['step_seq'      ] ),
                                array('name' => 'request_origin', 'title'=>'Source'            , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['request_origin'] ),
                                );

		return $table_fields;

	}

	function RetryFailedTran($id="")
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);


		//new profile
		$sql       = " UPDATE REQUEST_LOG SET STATUS=0, step_no=decode(step_no, -1, 0, step_no) WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Update() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "RetryFailedTran: ID=$id");

		return $data;
	}



	//1-only
	function &GetInstance()
	{
		static $instance;

		if(empty($instance) or !$instance or !defined($instance))
		{
			$instance   = new Enrollment();
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
