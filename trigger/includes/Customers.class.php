<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	Customer.class.php
#	@Description:	encapsulator for table CUSTOMER_TYPES
#
#
#	@Date		:	2009-07-10
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

define('CSV_HEADER_CUSTTYPE_REPORT',  "CUSTOMER_TYPE, DAILY_BALANCE, MAX_DURATION, MIN_BALANCE, EXT_MIN_BALANCE, MIN_VOICE, SMS_TRESHOLD, BREATHING_PERIOD, PRE_ACT_DAY, NUM_LINK, ARDS_1ST_NOTIFICATION, ARDS_2ND_NOTIFICATION, ARDS_EXPIRY, STATUS, CREATED BY, DATE, MODIFIED BY, DATE");

//class
class Customers
{

	//constructor
	function Customers()
	{

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

		//format the insert-parameters
		$sql     = " SELECT  id, customer_type, daily_balance, max_duration, min_balance, ext_min_balance, min_voice, sms_treshold, breathing_period, pre_act_day, num_link, status, ards_1st_notify, ards_2nd_notify, ards_tmp_duration FROM customer_types WHERE ID=$id ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("Open() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "ID"            ,   $P_ID              );
		OCIDefineByName($stmt, "CUSTOMER_TYPE" ,   $P_CUSTOMER_TYPE   );
		OCIDefineByName($stmt, "DAILY_BALANCE" ,   $P_DAILY_BALANCE   );
		OCIDefineByName($stmt, "MAX_DURATION"  ,   $P_MAX_DURATION    );
		OCIDefineByName($stmt, "MIN_BALANCE"   ,   $P_MIN_BALANCE     );
		OCIDefineByName($stmt, "EXT_MIN_BALANCE",  $P_EXT_MIN_BALANCE );
		OCIDefineByName($stmt, "MIN_VOICE"     ,   $P_MIN_VOICE       );
		OCIDefineByName($stmt, "SMS_TRESHOLD"  ,   $P_SMS_TRESHOLD    );
		OCIDefineByName($stmt, "BREATHING_PERIOD", $P_BREATHING_PERIOD);
		OCIDefineByName($stmt, "PRE_ACT_DAY",      $P_PRE_ACT_DAY     );
		OCIDefineByName($stmt, "NUM_LINK",         $P_NUM_LINK        );
		OCIDefineByName($stmt, "STATUS",           $P_STATUS          );
		OCIDefineByName($stmt, "ARDS_1ST_NOTIFY",  $P_ARDS_1ST_NOTIFY );
		OCIDefineByName($stmt, "ARDS_2ND_NOTIFY",  $P_ARDS_2ND_NOTIFY );
		OCIDefineByName($stmt, "ARDS_TMP_DURATION",$P_ARDS_TMP_DURATION);

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
			$P_ID             ;
			$P_CUSTOMER_TYPE  ;
			$P_DAILY_BALANCE  ;
			$P_MAX_DURATION   ;
                        $P_MIN_BALANCE    ;
			$P_MIN_VOICE      ;
                        $P_SMS_TRESHOLD   ;
                        $P_BREATHING_PERIOD;
                        $P_PRE_ACT_DAY    ;
                        $P_NUM_LINK       ;
                        $P_STATUS         ;
                        $P_ARDS_1ST_NOTIFY;
                        $P_ARDS_2ND_NOTIFY;
                        $P_ARDS_TMP_DURATION;
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "GetCustomerType() : [ ID=$id // $totalrows // $is_error / $err_msg] ");

		$alldata = array(
				'error'		=> $is_error, 
				'error_msg' 	=> $err_msg,
				'bind_total'	=> $totalrows,
				'bind_data'	=> array(
				'id'		    => $P_ID               ,
				'customer_type'	    => $P_CUSTOMER_TYPE    ,
				'daily_balance'	    => $P_DAILY_BALANCE    ,
				'max_duration'	    => $P_MAX_DURATION     ,
                                'min_balance'       => $P_MIN_BALANCE      ,
                                'ext_min_balance'   => $P_EXT_MIN_BALANCE  ,
				'min_voice'	    => $P_MIN_VOICE        ,
				'sms_treshold'	    => $P_SMS_TRESHOLD     ,
				'breathing_period'  => $P_BREATHING_PERIOD ,
				'pre_act_day'       => $P_PRE_ACT_DAY      ,
				'num_link'          => $P_NUM_LINK         ,
				'status'	    => $P_STATUS           ,
				'ards_1st_notify'   => $P_ARDS_1ST_NOTIFY  ,
				'ards_2nd_notify'   => $P_ARDS_2ND_NOTIFY  ,
				'ards_tmp_duration' => $P_ARDS_TMP_DURATION,
				));

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
		$ret = $this->IfExists($data['customer_type'], $data['id'],$ok);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_RECORD_ALREADY_EXISTS;
			return $err;
		}

		//usr
		if(strlen(trim($data['customer_type'])) <= 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_CUSTOMER_TYPE;
			return $err;
		}

                //usr
                if(strlen(trim($data['daily_balance'])) <= 0)
                {
                        $err['error'] = true;
                        $err['msg']   = "Please enter Daily Balance";
                        return $err;
                }

                //usr
                if(strlen(trim($data['max_duration'])) <= 0)
                {
                        $err['error'] = true;
                        $err['msg']   = "Please enter Max Duration";
                        return $err;
                }

                //usr
                if(strlen(trim($data['min_voice'])) <= 0)
                {
                        $err['error'] = true;
                        $err['msg']   = "Please enter Min Voice";
                        return $err;
                }

                //usr
                if(strlen(trim($data['sms_treshold'])) <= 0)
                {
                        $err['error'] = true;
                        $err['msg']   = "Please enter Sms Treshold";
                        return $err;
                }

                //usr
                if(strlen(trim($data['min_balance'])) <= 0)
                {
                        $err['error'] = true;
                        $err['msg']   = "Please enter Minimum Balance";
                        return $err;
                }

                //usr
                if(strlen(trim($data['ext_min_balance'])) <= 0)
                {
                        $err['error'] = true;
                        $err['msg']   = "Please enter Minimum Balance for Extension";
                        return $err;
                }

                //usr
                if(strlen(trim($data['breathing_period'])) <= 0)
                {
                        $err['error'] = true;
                        $err['msg']   = "Please enter Breathing Period";
                        return $err;
                }

                //usr
                if(strlen(trim($data['pre_act_day'])) <= 0)
                {
                        $err['error'] = true;
                        $err['msg']   = "Please enter Pre-Activation Day";
                        return $err;
                }

                //usr
                if(strlen(trim($data['num_link'])) <= 0)
                {
                        $err['error'] = true;
                        $err['msg']   = "Please enter Max Number of Links";
                        return $err;
                }


		//ok
		return $err;

	}


	//save
	function Save($data=null)
	{
		//globals here
		global $gEventLogger;

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
			if($col['data_type'] == 'string')
				$V[] = "'".ora_addslashes(trim($col['value'])) . "'";
			else
				$V[] = intval(trim($col['value'])) ;

			if (strlen($col['value'])>0)
				$mbuff .= $col['name']."=".$col['value']." ";
		}
		//created-by
		$K[]       = 'created_by';
		$V[]       = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"]) . "'";
		$kbuff     = join(",", $K);
		$vbuff     = join(",", $V);


		//new profile
		$sql       = " INSERT INTO customer_types ($kbuff, DT_CREATED )  VALUES ($vbuff, sysdate )";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Save() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "NEW CUSTOMER TYPE: $mbuff ");

		return $data;
	}




	//update
	function Update($data=null)
	{
		//globals here
		global $gEventLogger;

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
			if($col['data_type'] == 'string')
				$K[] = $col['name']. " = '".ora_addslashes(trim($col['value'])) . "'";
			else
				$K[] = $col['name']. " = ". intval(trim($col['value']));

			if (strlen($col['value'])>0)
				$mbuff .= $col['name']."=".$col['value']." ";
		}

		//created-by
		$K[] = " updated_by = '".ora_addslashes($_SESSION["session.smartygold.profile.user"])."' ";

		$kbuff     = join(" , ", $K);

		//new profile
		$sql       = " UPDATE customer_types SET DT_UPDATED=Sysdate, $kbuff WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Update() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Update() [ res=>'$is_ok' // msg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "UPDATE CUSTOMER TYPE: $mbuff ");

		return $data;
	}



	//delete
	function Delete($id=0)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		$id        = intval($id);
		//update profile
		$sql       = " DELETE FROM CUSTOMER_TYPES WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Delete() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Delete() [ res=>'$is_ok' // msg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "DELETE CUSTOMER TYPE: ID=$id  ");

		return $data;
	}


	//get uniq
	function IfExists($c1='',$id=0,$ok=0)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));
		$id        = intval(trim($id));

		//update profile
		$sql       = ($id>0 and $ok) ? 
			" SELECT  COUNT(1) as TOTAL FROM CUSTOMER_TYPES WHERE customer_type='$c1' AND id!=$id " :
			" SELECT  COUNT(1) as TOTAL FROM CUSTOMER_TYPES WHERE customer_type='$c1' ";


		
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
		$opts_status = $gSelectOptionsMaster->getStatus();

		$table_fields= array(
				array('name' => 'customer_type',      'title'=>'Customer Type',      'type' => 'text',    'max'=> 30,   'data_type'=> 'string', 'value' => $_POST['customer_type']),
				array('name' => 'daily_balance' ,     'title'=>'Daily Balance',      'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['daily_balance'] ),
				array('name' => 'max_duration' ,      'title'=>'Max Duration',       'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['max_duration'] ),
				array('name' => 'breathing_period',   'title'=>'Breathing Period',   'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['breathing_period'] ),
				array('name' => 'min_balance' ,       'title'=>'Min Balance',        'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['min_balance'] ),
				array('name' => 'ext_min_balance' ,   'title'=>'Ext Min Balance',    'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['ext_min_balance'] ),
				array('name' => 'min_voice' ,         'title'=>'Min Voice',          'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['min_voice'] ),
				array('name' => 'sms_treshold' ,      'title'=>'SMS Threshold',      'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['sms_treshold'] ),
				array('name' => 'pre_act_day' ,       'title'=>'Pre-Act Day',        'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['pre_act_day'] ),
				array('name' => 'num_link' ,          'title'=>'Max Link',           'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['num_link'] ),
				array('name' => 'ards_1st_notify',    'title'=>'ARDS 1st Notification',    'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['ards_1st_notify'] ),
				array('name' => 'ards_2nd_notify',    'title'=>'ARDS 2nd Notification',    'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['ards_2nd_notify'] ),
				array('name' => 'ards_tmp_duration',  'title'=>'ARDS Expiry',        'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $_POST['ards_tmp_duration'] ),
				array('name' => 'status' ,            'title'=>'Status',             'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $_POST['status'], 'select_options'=>$opts_status, 'select_selected' => $_POST['status'] ),
				);

		return $table_fields;

	}


	//get form fields
	function getTableFields4Update($id="")
	{
		global $gSelectOptionsMaster;

		$data      = $this->Get($id);			
		$post      = $data['bind_data'];
		$opts_status = $gSelectOptionsMaster->getStatus();

		$table_fields= array(
				array('name' => 'customer_type',      'title'=>'Customer Type',      'type' => 'text',    'max'=> 30,   'data_type'=> 'string', 'value' => $post['customer_type'], 'read_only' => 'readonly' ),
				array('name' => 'daily_balance' ,     'title'=>'Daily Balance',      'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['daily_balance'] ),
				array('name' => 'max_duration' ,      'title'=>'Max Duration',       'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['max_duration'] ),
				array('name' => 'breathing_period',   'title'=>'Breathing Period',   'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['breathing_period'] ),
				array('name' => 'min_balance' ,       'title'=>'Min Balance',        'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['min_balance'] ),
				array('name' => 'ext_min_balance' ,   'title'=>'Ext Min Balance',    'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['ext_min_balance'] ),
				array('name' => 'min_voice' ,         'title'=>'Min Voice',          'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['min_voice'] ),
				array('name' => 'sms_treshold' ,      'title'=>'SMS Threshold',      'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['sms_treshold'] ),
				array('name' => 'pre_act_day' ,       'title'=>'Pre-Act Day',        'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['pre_act_day'] ),
				array('name' => 'num_link' ,          'title'=>'Max Link',           'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['num_link'] ),
				array('name' => 'ards_1st_notify',    'title'=>'ARDS 1st Notification',    'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['ards_1st_notify'] ),
				array('name' => 'ards_2nd_notify',    'title'=>'ARDS 2nd Notification',    'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['ards_2nd_notify'] ),
				array('name' => 'ards_tmp_duration',  'title'=>'ARDS Expiry',        'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => $post['ards_tmp_duration'] ),
				array('name' => 'status' ,            'title'=>'Status',             'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $post['status'], 'select_options'=>$opts_status, 'select_selected' => $post['status'] ),
				);

		return $table_fields;

	}

	//get form fields
	function getTableFields4Clear()
	{
		global $gSelectOptionsMaster;

		$post      = $data['bind_data'];
		$opts_status = $gSelectOptionsMaster->getStatus();

		$table_fields= array(
				array('name' => 'customer_type',      'title'=>'Customer Type',      'type' => 'text',    'max'=> 30,   'data_type'=> 'string', 'value' => '' ),
				array('name' => 'daily_balance' ,     'title'=>'Daily Balance',      'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'max_duration' ,      'title'=>'Max Duration',       'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'breathing_period',   'title'=>'Breathing Period',   'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'min_balance' ,       'title'=>'Min Balance',        'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'ext_min_balance' ,   'title'=>'Ext Min Balance',    'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'min_voice' ,         'title'=>'Min Voice',          'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'sms_treshold' ,      'title'=>'SMS Threshold',      'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'pre_act_day' ,       'title'=>'Pre-Act Day',        'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'num_link' ,          'title'=>'Max Link',           'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'ards_1st_notify',    'title'=>'ARDS 1st Notification',    'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'ards_2nd_notify',    'title'=>'ARDS 2nd Notification',    'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'ards_tmp_duration',  'title'=>'ARDS Expiry',        'type' => 'text',    'max'=> 6,    'data_type'=> 'string', 'value' => '' ),
				array('name' => 'status' ,            'title'=>'Status',             'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $post['status'], 'select_options'=>$opts_status, 'select_selected' => '%' ),
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


		$filters=0;
		$temp_code  = $_GET['scode'];
		$temp_stat  = $_GET['sstat'];
		$temp_file  = $_GET['sfile'];
		$temp_sort  = $_GET['sortedby'];
		$xwherelog  = "";

		if (strlen($temp_code) > 0) { 
			$xwhere .= " AND customer_type like '$temp_code%' "; 
			$xwherelog .= " CUSTOMER TYPE=$temp_code "; 
			$filters++; 
		}

		if ((strlen($temp_stat) > 0)  && ($temp_stat != 'ALL')) { 
			$xwhere .= " AND status like '$temp_stat' "; 
			$xwherelog .= " STATUS=$temp_stat "; 
			$filters++; 
		}

		if ($filters > 0) {
			$xwhere = " where 1 =1 ". $xwhere;
		}
		else {
			$xwherelog .= " All "; 
		}

		//fmt
		$whatsort    = $extravars['whatsort'];
		//type of order
		$ascdesc     = (!$whatsort) ? (" ASC ") : ( " DESC " );	

		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT COUNT(1) AS COUNT FROM customer_types $xwhere";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"COUNT",$totalrows);		

		$this->debug("getList() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);

		// logoff connection
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getCustomerTypeList : [ WHERE=$xwhere // TOTAL=>'$totalrows'] ");

		//chk rowcount
		if(0 == $totalrows )     
		{                   
                	$retdata['scode']     = $temp_code ;
                	$retdata['sstat']     = $temp_stat ;
			return $retdata;	
		}                   

		// prepare download file
		if ((strlen($temp_file)>0) && ($temp_file==1)) {
			$csvdtl = $this->downloadList($extravars=null);
			$retdata['csv'] = $csvdtl['csvfilename'];
		}
		else {
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
			$gEventLogger->LogEvent(__CLASS__, "SEARCHED By=$xwherelog TOTAL Rows=$totalrows PAGE=$nPageNo/$nPageTo");
                	
			//logon to oracle database
			$conn = $myOraDb->dbConnection();
                	
			//orderby
			//$sortedby       = $extravars['sortedby'];
			$sortedby       = $temp_sort;
			switch($sortedby)
			{
				case 'name':
					$orderby = " ORDER BY  customer_type $ascdesc";
					break;
				case 'desc':
					$orderby = " ORDER BY  status $ascdesc";
					break;
				default:	
					$orderby = " ORDER BY id $ascdesc";
					break;
			}
                	
			//select values for dynamic SQL passing
			$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, customer_type, daily_balance, max_duration, min_balance, ext_min_balance, min_voice, sms_treshold, breathing_period, pre_act_day, num_link, status, ards_1st_notify, ards_2nd_notify, ards_tmp_duration FROM customer_types ".
				"  $xwhere $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";
                	
			//Parse SQL passed
			$stmt = OCIParse($conn,$sql);
                	
			$this->debug("getList() : get_all_rows[$sql / $stmt / $totalrows]");
                	
                	
			//bind
			OCIDefineByName($stmt, 'ID'               , $p_id               );
			OCIDefineByName($stmt, 'CUSTOMER_TYPE'    , $p_customer_type    );
			OCIDefineByName($stmt, 'DAILY_BALANCE'    , $p_daily_balance    );
			OCIDefineByName($stmt, 'MAX_DURATION'     , $p_max_duration     );
			OCIDefineByName($stmt, 'MIN_BALANCE'      , $p_min_balance      );
			OCIDefineByName($stmt, 'EXT_MIN_BALANCE'  , $p_ext_min_balance  );
			OCIDefineByName($stmt, 'MIN_VOICE'        , $p_min_voice        );
			OCIDefineByName($stmt, 'SMS_TRESHOLD'     , $p_sms_treshold     );
			OCIDefineByName($stmt, 'BREATHING_PERIOD' , $p_breathing_period );
			OCIDefineByName($stmt, 'PRE_ACT_DAY'      , $p_pre_act_day      );
			OCIDefineByName($stmt, 'NUM_LINK'         , $p_num_link         );
			OCIDefineByName($stmt, 'STATUS'           , $p_status           );
			OCIDefineByName($stmt, 'ARDS_1ST_NOTIFY'  , $p_ards_1st_notify  );
			OCIDefineByName($stmt, 'ARDS_2ND_NOTIFY'  , $p_ards_2nd_notify  );
			OCIDefineByName($stmt, 'ARDS_TMP_DURATION', $p_ards_tmp_duration);
                	
                	
			//Execute SQL
			OCIExecute($stmt);
                	
			//Loop thru all the records selected and display it in the table
			$idx = 0;
			while (OCIFetch($stmt))
			{
				$alldata[$idx]['id'           ]        = $p_id               ;
				$alldata[$idx]['customer_type']        = $p_customer_type    ;
				$alldata[$idx]['daily_balance']        = $p_daily_balance    ;
				$alldata[$idx]['max_duration' ]        = $p_max_duration     ;
				$alldata[$idx]['min_balance'  ]        = $p_min_balance      ;
				$alldata[$idx]['ext_min_balance']      = $p_ext_min_balance  ;
				$alldata[$idx]['min_voice'    ]        = $p_min_voice        ;
				$alldata[$idx]['sms_treshold' ]        = $p_sms_treshold     ;
				$alldata[$idx]['breathing_period']     = $p_breathing_period ;
				$alldata[$idx]['pre_act_day']          = $p_pre_act_day      ;
				$alldata[$idx]['num_link']             = $p_num_link         ;
				$alldata[$idx]['status'       ]        = $p_status           ;
				$alldata[$idx]['ards_1st_notify'  ]    = $p_ards_1st_notify  ;
				$alldata[$idx]['ards_2nd_notify'  ]    = $p_ards_2nd_notify  ;
				$alldata[$idx]['ards_tmp_duration']    = $p_ards_tmp_duration;
				$idx++;
			}
                	
                	
                	
			//Free Oracle connection
			OCIFreeStatement($stmt);
                	
			//Log-off connection		
			OCILogoff($conn);
                	
			//save
			$retdata['data']      = $alldata;
		}
		$retdata['total']     = $totalrows ;

                $retdata['scode']     = $temp_code ;
                $retdata['sstat']     = $temp_stat ;

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

	//list
	function downloadList($extravars=null)
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

		$filters=0;
		$temp_code  = $_GET['scode'];
		$temp_stat  = $_GET['sstat'];
		$xwherelog  = "";

		if (strlen($temp_code) > 0) { 
			$xwhere .= " AND customer_type like '$temp_code%' "; 
			$filters++; 
		}

		if ((strlen($temp_stat) > 0)  && ($temp_stat != 'ALL')) { 
			$xwhere .= " AND status like '$temp_stat' "; 
			$filters++; 
		}

		if ($filters > 0) {
			$xwhere = " where 1 =1 ". $xwhere;
		}
		else {
			$xwherelog .= " All "; 
		}

		//type of order
		$ascdesc     = " ASC ";	


		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//orderby
		$orderby = " ORDER BY  customer_type $ascdesc";

		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, customer_type, daily_balance, max_duration, min_balance, ext_min_balance, min_voice, sms_treshold, breathing_period, pre_act_day, num_link, status, created_by, ".
			"   to_char(dt_created, 'YYYY-MM-DD HH24:MI:SS') dt_created, updated_by modified_by, to_char(dt_updated, 'YYYY-MM-DD HH24:MI:SS') dt_modified, ards_1st_notify, ards_2nd_notify, ards_tmp_duration ". 
			"   FROM customer_types ".
			"  $xwhere $orderby ) b ) ";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("getList() : get_all_rows[$sql / $stmt / $totalrows]");


		//open csv here
		$csvfnam = date("Ymd").'.'.get_uid().".csv";
		$csvfile = SMARTY_INC_DIR."/log/$csvfnam";
		$csvcont = '';

		//bind
		OCIDefineByName($stmt, 'ID'               , $p_id               );
		OCIDefineByName($stmt, 'CUSTOMER_TYPE'    , $p_customer_type    );
		OCIDefineByName($stmt, 'DAILY_BALANCE'    , $p_daily_balance    );
		OCIDefineByName($stmt, 'MAX_DURATION'     , $p_max_duration     );
		OCIDefineByName($stmt, 'MIN_BALANCE'      , $p_min_balance      );
		OCIDefineByName($stmt, 'EXT_MIN_BALANCE'  , $p_ext_min_balance  );
		OCIDefineByName($stmt, 'MIN_VOICE'        , $p_min_voice        );
		OCIDefineByName($stmt, 'SMS_TRESHOLD'     , $p_sms_treshold     );
		OCIDefineByName($stmt, 'BREATHING_PERIOD' , $p_breathing_period );
		OCIDefineByName($stmt, 'PRE_ACT_DAY'      , $p_pre_act_day      );
		OCIDefineByName($stmt, 'NUM_LINK'         , $p_num_link         );
		OCIDefineByName($stmt, 'ARDS_1ST_NOTIFY'  , $p_ards_1st_notify  );
		OCIDefineByName($stmt, 'ARDS_2ND_NOTIFY'  , $p_ards_2nd_notify  );
		OCIDefineByName($stmt, 'ARDS_TMP_DURATION', $p_ards_tmp_duration);
		OCIDefineByName($stmt, 'STATUS'           , $p_status           );
		OCIDefineByName($stmt, 'CREATED_BY'       , $p_created_by       );
		OCIDefineByName($stmt, 'DT_CREATED'       , $p_dt_created       );
		OCIDefineByName($stmt, 'MODIFIED_BY'      , $p_modified_by      );
		OCIDefineByName($stmt, 'DT_MODIFIED'      , $p_dt_modified      );


		//Execute SQL
		OCIExecute($stmt);

		//Loop thru all the records selected and display it in the table
		$idx = 0;
		while (OCIFetch($stmt))
		{
			//csv-create
			$csvdata   = null;
			$csvdata[] = $p_customer_type    ;
			$csvdata[] = $p_daily_balance    ;
			$csvdata[] = $p_max_duration     ;
			$csvdata[] = $p_min_balance      ;
			$csvdata[] = $p_ext_min_balance  ;
			$csvdata[] = $p_min_voice        ;
			$csvdata[] = $p_sms_treshold     ;
			$csvdata[] = $p_breathing_period ;
			$csvdata[] = $p_pre_act_day      ;
			$csvdata[] = $p_num_link         ;
			$csvdata[] = $p_ards_1st_notify  ;
			$csvdata[] = $p_ards_2nd_notify  ;
			$csvdata[] = $p_ards_tmp_duration;
			$csvdata[] = $p_status           ;
			$csvdata[] = $p_created_by       ;
			$csvdata[] = $p_dt_created       ;
			$csvdata[] = $p_modified_by      ;
			$csvdata[] = $p_dt_modified      ;
			$csvstr    = join('","', $csvdata);
			$csvcont .= "\"$csvstr\"\n";
			$idx++;
		}

		//close csv
                $rep_gen_dt = date("Y-m-d H:i:s");
                file_save($csvfile,"List of Customer Types\nReport Generation: $rep_gen_dt\n\n".CSV_HEADER_CUSTTYPE_REPORT."\n\n".$csvcont);

		//Free Oracle connection
		OCIFreeStatement($stmt);

		//Log-off connection		
		OCILogoff($conn);

		//save
		$retdata['csvfilename'] = $csvfnam;

		//return data
		return $retdata;
	}

	//1-only
	function &GetInstance()
	{
		static $instance;

		if(empty($instance) or !$instance or !defined($instance))
		{
			$instance   = new Customers();
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
