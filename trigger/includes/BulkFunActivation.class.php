<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	BulkFunActivation.class.php
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
class BulkFunActivation
{

	//constructor
	function BulkFunActivation()
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

		$vgsmnum1 = ora_addslashes($data[0]);
		$vgsmnum2 = ora_addslashes($data[1]);
		$vremarks = "'".ora_addslashes($data[2])."'";
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";
		$this->debug("SaveList() a#: $vgsmnum, $vgroup, $vname, $vcontact_number, $vremarks, $grpid");

                //msisdn_to
		$chkgsm = 0;
                if(!@preg_match("/^639[0-9]{9}$/i", $vgsmnum1 )){
			$chkgsm = 1;
			$errReason = "Invalid OFW SIM";
                } else if (strlen($vgsmnum1) < 0) {
			$chkgsm = 2;
			$errReason = "Invalid OFW SIM";
                } else if ($vgsmnum1==$vgsmnum2) {
                        $chkgsm = 2;
                        $errReason = "OFW SIM is the same with LOCAL SIM";
		} else if ($this->isValidGlobe($vgsmnum1) == 0) {
			$chkgsm = 3;
			$errReason = "OFW SIM is not a PREPAID No.";
		} else if ($this->isBlacklisted($vgsmnum1) >= 1) {
			$chkgsm = 4;
			$errReason = "OFW SIM is Blacklisted";
		} else if ($this->chkLocalActiveFun($vgsmnum1) >= 1) {
			$chkgsm = 4;
			$errReason = "OFW SIM already used as Local SIM";
		} else if ($this->chkActiveFun($vgsmnum1) >= 2) {
			$chkgsm = 5;
			$errReason = "Exceed Max Number of links";

		} else if(!@preg_match("/^639[0-9]{9}$/i", $vgsmnum2 )){
			$chkgsm = 1;
			$errReason = "Invalid LOCAL SIM";
                } else if (strlen($vgsmnum2) < 0) {
			$chkgsm = 2;
			$errReason = "Invalid LOCAL SIM";
		} else if ($this->isValidGlobe($vgsmnum2) == 0) {
			$chkgsm = 3;
			$errReason = "LOCAL SIM is not a PREPAID No.";
		} else if ($this->isBlacklisted($vgsmnum2) >= 1) {
			$chkgsm = 4;
			$errReason = "LOCAL SIM is Blacklisted";
		} else if ($this->chkActiveFun($vgsmnum2) >= 1) {
			$chkgsm = 4;
			$errReason = "LOCAL SIM already used as OFW SIM";
		} else if ($this->chkLocalActiveFun($vgsmnum2) >= 1) {
			$chkgsm = 4;
			$errReason = "Local SIM already link";

		} else if ($this->chkPendingRequest($vgsmnum1,$vgsmnum2,'REG') >= 1) {
			$chkgsm = 6;
			$errReason = "With Pending Request";
		} 

		$this->debug("SaveList() : $vgsmnum // $chkgsm");

		if ($chkgsm == 0) 
		{
			//new profile
			$sql       = "insert into sim_activation_links (id, customer_type, msisdn, link_to, link_to_type, status, cust_activation_dt, dt_created, link_cmd, ods_last_chk_dt, cust_activation_tm, cs_username, bulk_grp_id) ".
                                     "values (sim_activation_links_seq.nextval, 'FUN', $vgsmnum1, $vgsmnum2, 'OFW', 'ACTIVE', trunc(sysdate), sysdate, 'BULK', trunc(sysdate), sysdate,  $vCreated_by, '$grpid')";

			//exect
			$data      = $myOraDb->xQuery($sql);
			$is_ok     = $data['error'] ? false : true;
			$err_msg   = $data['error_msg'];
			if ($err_msg != 'SUCCESS'){
			    $this->debug("SaveList() : $sql // $is_ok // $err_msg");
			    $sql       = "update sim_activation_links set status='ACTIVE', ".
					 "cust_activation_dt=trunc(sysdate), ".
					 "cust_deactivation_dt=null, ".
					 "cust_deactivation_tm=null, ".
					 "expiry_date=null, ".
					 "notify_date=null, ".
					 "ods_last_chk_dt=trunc(sysdate), ".
					 "cs_username=$vCreated_by, ".
					 "bulk_grp_id='$grpid' ".
					 "where msisdn=$vgsmnum1 and link_to=$vgsmnum2";
				$data      = $myOraDb->xQuery($sql);
				$is_ok     = $data['error'] ? false : true;
				$err_msg   = $data['error_msg'];
			}
		}
		else{
			//new profile
			$sql       = "INSERT INTO SIM_ACTIVATION_LINKS_ERR_BULK ( OFW_SIM, LOCAL_SIM, REASON, REMARKS, BULK_GRP_ID, CREATED_BY, DT_CREATED, TRAN_DT ) ". 
				     "VALUES ('$vgsmnum1', '$vgsmnum2', '$errReason', $vremarks, '$grpid', $vCreated_by, sysdate,  trunc(sysdate)) ";
			//exect
			$data      = $myOraDb->xQuery($sql);
                        $is_ok     = $data['error'] ? false : true;
                        $err_msg   = $data['error_msg'];

		}

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
		$sql  = " SELECT COUNT(1) AS COUNT FROM SIM_ACTIVATION_LINKS $xwhere";

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
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT 0 ID, OFW_SIM, LOCAL_SIM, CREATED_BY, DT_CREATED, BULK_GRP_ID, REASON REASON  FROM SIM_ACTIVATION_LINKS_ERR_BULK ".
			"  $xwhere $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("getList() : get_all_rows[$sql / $stmt / $totalrows]");


		//bind
                OCIDefineByName($stmt,"ID"                , $p_id              );
                OCIDefineByName($stmt,"OFW_SIM"           , $p_ofw_sim         );
                OCIDefineByName($stmt,"LOCAL_SIM"         , $p_local_sim       );
                OCIDefineByName($stmt,"CREATED_BY"        , $p_created_by      );
                OCIDefineByName($stmt,"DT_CREATED"        , $p_dt_created      );
                OCIDefineByName($stmt,"REASON"            , $p_reason          );

		//Execute SQL
		OCIExecute($stmt);

		//Loop thru all the records selected and display it in the table
		$idx = 0;
		while (OCIFetch($stmt))
		{
			$alldata[$idx]['id'             ]    = $p_id              ;
			$alldata[$idx]['ofw_sim'        ]    = $p_ofw_sim         ;
			$alldata[$idx]['local_sim'      ]    = $p_local_sim       ;
			$alldata[$idx]['created_by'     ]    = $p_created_by      ;
			$alldata[$idx]['dt_created'     ]    = $p_dt_created      ;
			$alldata[$idx]['reason'         ]    = $p_reason          ;
			$idx++;
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCILogoff($conn);

		//save
		$retdata['data']      = $alldata;
		$retdata['total']     = $totalrows ;

		$navigator            = $page['links'];
		$retdata['links']     = $navigator;

		//return data
		return $retdata;
	}

	//get uniq
	function isValidGlobe($c1='')
	{
		//globals here
		global $gEventLogger;
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));

		//update profile
		$sql       = " SELECT sf_is_in_hlr_range($c1) as TOTAL FROM dual";
		$stmt = OCIParse($conn,$sql);
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		
		OCIExecute($stmt);
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);
		OCIFreeStatement($stmt);
		OCICommit($conn);
		OCILogoff($conn);

		$this->debug("isValidGlobe() : // $sql // $totalrows");

		return $totalrows;
	}

	//check if already fun
	function chkActiveFun($c1='')
	{
		//globals here
		global $gEventLogger;
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));

		//update profile
		$sql       = " SELECT sf_is_active_fun($c1) TOTAL FROM DUAL";
		$stmt = OCIParse($conn,$sql);
		$totalrows = 0;	
		OCIDefineByName($stmt,"TOTAL",$totalrows);		
		OCIExecute($stmt);
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);
		OCILogoff($conn);

		$this->debug("chkActiveFun() : // $sql // $totalrows");

		return $totalrows;

	}

	//check if already fun
	function chkLocalActiveFun($c1='')
	{
		//globals here
		global $gEventLogger;
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));

		//update profile
		$sql       = " SELECT sf_is_local_sim_active_link($c1) TOTAL FROM DUAL";
		$stmt = OCIParse($conn,$sql);
		$totalrows = 0;	
		OCIDefineByName($stmt,"TOTAL",$totalrows);		
		OCIExecute($stmt);
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);
		OCILogoff($conn);

		$this->debug("chkLocalActiveFun() : // $sql // $totalrows");

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
