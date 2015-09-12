<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	BulkActivation.class.php
#	@Description:	encapsulator for table BulkActivation_log
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
class BulkActivation
{

	//constructor
	function BulkActivation()
	{

	}

	//save
	//function SaveList($gsmnum, $group, $name, $contact_number, $remarks)
	function SaveList($data=null,$grpid=null)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		$vgsmnum = ora_addslashes($data[0]);
		$vname = "'".ora_addslashes($data[1])."'";
		$vcontact_number = "'".ora_addslashes($data[2])."'";
		$vremarks = "'".ora_addslashes($data[3])."'";
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";
		$this->debug("SaveList() a#: $vgsmnum, $vgroup, $vname, $vcontact_number, $vremarks, $grpid");

                //msisdn_to
		$chkgsm = 0;
                if(!@preg_match("/^639[0-9]{9}$/i", $vgsmnum )){
			$chkgsm = 1;
			$errReason = "Invalid MSISDN";
                } else if (strlen($vgsmnum) < 0) {
			$chkgsm = 2;
			$errReason = "Invalid MSISDN";
		} else if ($this->isValidGlobe($vgsmnum) == 0) {
			$chkgsm = 3;
			$errReason = "Invalid MSISDN";
		} else if ($this->isBlacklisted($vgsmnum) >= 1) {
			$chkgsm = 4;
			$errReason = "Blacklisted";
		} else if ($this->chkRoamer($vgsmnum) >= 1) {
			$chkgsm = 5;
			$errReason = "Already Roamer";
		} else if ($this->chkPendingRequest($vgsmnum,'ACTIVATION') >= 1) {
			$chkgsm = 6;
			$errReason = "With Pending Request";
		} 

		$this->debug("SaveList() : $vgsmnum // $chkgsm");

		if ($chkgsm == 0) 
		{
			//new profile
			$sql       = "INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, customer_type, request_origin ) ". 
				     "VALUES (manual_enrollment_log_seq.nextval, $vgsmnum, 'ACTIVATION', 1, trunc(sysdate), 0, $vCreated_by, sysdate, $vname, $vcontact_number, $vremarks, '$grpid', sf_get_min_bal('TRIGGER', 0, 0), 0, 'TRIGGER', 'WEB') ";
			//exect
			$data      = $myOraDb->xQuery($sql);
		}
		else{
			//new profile
			$sql       = "INSERT INTO BULK_ACTIVATION_ERR_LOG ( MSISDN, TX_DATE, SUBS_NAME, SUBS_CONTACT_NO, REMARKS, BULK_GRP_ID, CS_USERNAME, DT_CREATED, CS_REASON ) ". 
				     "VALUES ('$vgsmnum', trunc(sysdate), $vname, $vcontact_number, $vremarks, '$grpid', $vCreated_by, sysdate, '$errReason' ) ";
			//exect
			$data      = $myOraDb->xQuery($sql);
		}

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("SaveList() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "SaveList() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");

		return $data;
	}


	//list
	function getList($grp_id=null)
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
		$xwhere	= " where bulk_grp_id = '$grp_id'";

		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT COUNT(1) AS COUNT FROM MANUAL_ENROLLMENT_LOG $xwhere";

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
		//$gEventLogger->LogEvent(__CLASS__, "getServicesList : [WHERE=>'$xwhere'] [ TOTAL=>'$totalrows'] ");

		//chk rowcount
		//if(0 == $totalrows )     
		//{                   
		//	return $retdata;	
		//}                   

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
		$gEventLogger->LogEvent(__CLASS__, "BULK ID=$grp_id TOTAL Rows:$totalrows PAGE:$nPageNo/$nPageTo");


		//logon to oracle database
		$conn = $myOraDb->dbConnection();


		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT 0 ID, MSISDN, TX_DATE, CS_USERNAME, DT_CREATED, SUBS_NAME, SUBS_CONTACT_NO, REMARKS, BULK_GRP_ID, CS_REASON REASON  FROM BULK_ACTIVATION_ERR_LOG ".
			"  $xwhere $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("getList() : get_all_rows[$sql / $stmt / $totalrows]");


		//bind
                OCIDefineByName($stmt,"ID"                , $p_id              );
                OCIDefineByName($stmt,"MSISDN"            , $p_msisdn          );
                OCIDefineByName($stmt,"TX_DATE"           , $p_tx_date         );
                OCIDefineByName($stmt,"CS_USERNAME"       , $p_cs_username     );
                OCIDefineByName($stmt,"DT_CREATED"        , $p_dt_created      );
                OCIDefineByName($stmt,"SUBS_NAME"         , $p_subs_name       );
                OCIDefineByName($stmt,"SUBS_CONTACT_NO"   , $p_subs_contact_no );
                OCIDefineByName($stmt,"REMARKS"           , $p_remarks         );
                OCIDefineByName($stmt,"REASON"            , $p_reason          );

		//Execute SQL
		OCIExecute($stmt);

		//Loop thru all the records selected and display it in the table
		$idx = 0;
		while (OCIFetch($stmt))
		{
			$alldata[$idx]['id'             ]    = $p_id               ;
			$alldata[$idx]['msisdn'         ]    = $p_msisdn           ;
			$alldata[$idx]['tx_date'        ]    = $p_tx_date          ;
			$alldata[$idx]['cs_username'    ]    = $p_cs_username      ;
			$alldata[$idx]['dt_created'     ]    = $p_dt_created       ;
			$alldata[$idx]['subs_name'      ]    = $p_subs_name        ;
			$alldata[$idx]['subs_contact_no']    = $p_subs_contact_no  ;
			$alldata[$idx]['remarks'        ]    = $p_remarks          ;
			$alldata[$idx]['reason'         ]    = $p_reason           ;
			$idx++;
		}



		//Free Oracle connection
		OCIFreeStatement($stmt);

		//Log-off connection		
		OCILogoff($conn);

		//save
		$retdata['data']      = $alldata;
		$retdata['total']     = $totalrows ;

		//no rec
		//if($totalrows <=0 )
		//{
		//	//$retdata['links'] = 'No result found';
		//	$retdata['links'] = '';
		//}
		//else
		//{
			$navigator            = $page['links'];
			$retdata['links']     = $navigator;
		//}				 

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getList(#b) : [ total=>'$totalrows'] ");

		//return data
		return $retdata;
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
		$sql       = " SELECT SUM(total) TOTAL FROM (".
                             " SELECT COUNT(1) as TOTAL FROM sim_activation WHERE MSISDN = $c1 and status<>'INACTIVE') ";
		
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
		$sql       = " SELECT COUNT(1) as TOTAL FROM manual_enrollment_log WHERE MSISDN = $c1 and enrollment_type='$c2' and status=0 ";
		
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

	//1-only
	function &GetInstance()
	{
		static $instance;

		if(empty($instance) or !$instance or !defined($instance))
		{
			$instance   = new BulkActivation();
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
