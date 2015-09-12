<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	BulkWhitelisting.class.php
#	@Description:	encapsulator for table BulkWhitelisting_log
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
class BulkWhitelisting
{

	//constructor
	function BulkWhitelisting()
	{

	}

	//save
	function SaveList($data=null,$grpid=null)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		$vgsmnum = ora_addslashes($data[0]);
		$vremarks = "'".ora_addslashes($data[1])."'";
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";
		$this->debug("SaveList() a#: $vgsmnum, $vgroup, $vname, $vcontact_number, $vremarks, $grpid");

                //msisdn_to
		$chkgsm = 0;
                if(!@preg_match("/^639[0-9]{9}$/i", $vgsmnum )) {
			$chkgsm = 1;
                       $vReason = "Invalid MSISDN";
                } else if (strlen($vgsmnum) < 0) {
			$chkgsm = 2;
                       $vReason = "Invalid MSISDN";
		} else if ($this->isWhitelisted($vgsmnum) >= 1) {
			$chkgsm = 3;
                       $vReason = "Already Whitelisted";
		}
		$this->debug("SaveList() : $vgsmnum // $chkgsm");

		if ($chkgsm == 0) 
		{
			$sql       = "DELETE FROM BLACKLISTED_LOG WHERE MSISDN = $vgsmnum";
                	$data      = $myOraDb->xQuery($sql);
                	$sql       = "INSERT INTO WHITELISTED_LOG ( MSISDN, TRAN_DT, CREATED_BY, DT_CREATED, REMARKS, bulk_grp_id ) ".
				     "VALUES ( $vgsmnum, trunc(sysdate), $vCreated_by, sysdate, $vremarks, '$grpid' ) ";
			$data      = $myOraDb->xQuery($sql);
		} else {
                        $sql       = "INSERT INTO WHITELISTED_ERR_LOG ( MSISDN, TRAN_DT, CREATED_BY, DT_CREATED, REMARKS, bulk_grp_id, REASON) ".
                                     "VALUES ( '$vgsmnum', trunc(sysdate), $vCreated_by, sysdate, $vremarks, '$grpid', '$vReason' ) ";
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
		$sql  = " SELECT COUNT(1) AS COUNT FROM WHITELISTED_LOG $xwhere";

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


		$orderby = "ORDER BY MSISDN";
		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT MSISDN, to_char(TRAN_DT, 'YYYY-MM-DD') TX_DATE, to_char(DT_CREATED, 'YYYY-MM-DD HH24:MI:SS') DT_CREATED, REMARKS, REASON FROM WHITELISTED_ERR_LOG ".
			"  $xwhere $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("getList() : get_all_rows[$sql / $stmt / $totalrows]");


		//bind
                OCIDefineByName($stmt,"MSISDN"            , $p_msisdn          );
                OCIDefineByName($stmt,"TX_DATE"           , $p_tx_date         );
                OCIDefineByName($stmt,"DT_CREATED"        , $p_dt_created      );
                OCIDefineByName($stmt,"REMARKS"           , $p_remarks         );
                OCIDefineByName($stmt,"REASON"            , $p_reason          );

		//Execute SQL
		OCIExecute($stmt);

		//Loop thru all the records selected and display it in the table
		$idx = 0;
		while (OCIFetch($stmt))
		{
			$alldata[$idx]['msisdn'         ]    = $p_msisdn           ;
			$alldata[$idx]['tx_date'        ]    = $p_tx_date          ;
			$alldata[$idx]['dt_created'     ]    = $p_dt_created       ;
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
		//	$retdata['links'] = 'No result found';
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
	function isWhitelisted($c1='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));

		//update profile
		$sql       = " SELECT COUNT(1) as TOTAL FROM whitelisted_log WHERE MSISDN = $c1 ";
		
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

		$this->debug("isWhitelisted() : // $sql // $totalrows");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "isWhitelisted() [ $sql // $totalrows ] ");

		return $totalrows;


	}

	//1-only
	function &GetInstance()
	{
		static $instance;

		if(empty($instance) or !$instance or !defined($instance))
		{
			$instance   = new BulkWhitelisting();
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
