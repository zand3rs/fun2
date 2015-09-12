<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	View_Transactions.class.php
#	@Description	:	encapsulator for table user
#
#
#	@Date		: 2009-04-30
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
require_once('HTTP/Request.php');


define('CSV_HEADER_SUBS_REPORT',  "tran_id, tran_dt, txid, a_no, b_no, step_no, last_step_no, status, customer_type, request_origin, dt_created");

//class
class View_Transactions
{

	var $_Data;

	//constructor
	function View_Transactions()
	{
		//reset
		$this->_Data = null;
	}


	//list
	function ListTransactions($extravars, $q_sdate,$q_edate,$q_tran_id, $q_status,$q_msisdn,$q_service)
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

		//param
		/**
		  q_sdate
		  q_edate
		  q_tran_id
		  q_status
		  q_msisdn
		 **/
		//$mwhere   = " WHERE customer_type = 'TRIGGER' ";
		$mwhere   = " WHERE (customer_type is null or customer_type <> 'FUN') ";
		$mwherelog= "";
		//set-where
		if((strlen($q_sdate) > 0) and (strlen($q_edate) > 0))
		{
			$mwhere   .= " AND a.tran_dt >=to_date('$q_sdate','YYYY-MM-DD') AND a.tran_dt<=to_date('$q_edate','YYYY-MM-DD') ";
		        $mwherelog.= " StartDt=$q_sdate EndDt=$q_edate ";
		}
		else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_sdate)) 
		{
			$mwhere   .= " AND a.tran_dt>=tx_date('$q_sdate','YYYY-MM-DD') ";
		        $mwherelog.= " StartDt=$q_sdate ";
		}
                else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_edate))
                {
                        $mwhere   .= " AND a.tran_dt=to_date('$q_edate','YYYY-MM-DD') ";
		        $mwherelog.= " EndDt=$q_edate ";
                }

		if((@preg_match("/^\d+$/i", $q_msisdn)))
		{
			$mwhere .= " AND a.a_no='". ora_addslashes($q_msisdn) ."' ";	
		        $mwherelog.= " A_NO=$q_msisdn ";
		}
		//print_r($mwhere);

		//$mwhere2   = " WHERE customer_type = 'TRIGGER' and status=0 ";
		$mwhere2   = " WHERE (customer_type is null or customer_type <> 'FUN') and status=0 ";
		//set-where
		if((strlen($q_sdate) > 0) and (strlen($q_edate) > 0))
		{
			$mwhere2   .= " AND a.tx_date >= to_date('$q_sdate','YYYY-MM-DD') AND a.tx_date <= to_date('$q_edate','YYYY-MM-DD') ";
		}
		else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_sdate)) 
		{
			$mwhere2   .= " AND a.tx_date >= tx_date('$q_sdate','YYYY-MM-DD') ";
		}
                else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_edate))
                {
                        $mwhere2   .= " AND a.tx_date = to_date('$q_edate','YYYY-MM-DD') ";
                }

		if((@preg_match("/^\d+$/i", $q_msisdn)))
		{
			$mwhere2 .= " AND a.msisdn='". ora_addslashes($q_msisdn) ."' ";	
		}


		//fmt
		$whatsort    = $extravars['whatsort'];

		//type of order
		$ascdesc     = (!$whatsort) ? (" ASC ") : ( " DESC " );	


		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT sum(count) AS COUNT FROM (".
			" SELECT COUNT(1) AS COUNT FROM REQUEST_LOG a $mwhere UNION ".
			" SELECT COUNT(1) AS COUNT FROM MANUAL_ENROLLMENT_LOG a $mwhere2) ";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	
		OCIDefineByName($stmt,"COUNT",$totalrows);		

		$this->debug("ListTransactions() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);

		// logoff connection
		OCILogoff($conn);

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

		//event
		$nPageNo=$pager->getCurrentPageID();
		$nPageTo=$pager->numPages();
		if($mwherelog=="")
		   $mwherelog="ALL";

		//event
		$gEventLogger->LogEvent(__CLASS__, "SEARCHED By:$mwherelog TOTAL Rows:$totalrows PAGE:$nPageNo/$nPageTo");


		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//orderby
		$orderby = " ORDER BY DT_CREATED desc";

                if ($_GET['sort'] == 'TRAN_ID')
                        $orderby = " id $ascdesc ";
                else if ($_GET['sort'] == 'STATUS')
                        $orderby = " status $ascdesc ";
                else if ($_GET['sort'] == 'MSISDN')
                        $orderby = " msisdn $ascdesc ";
                else if ($_GET['sort'] == 'START')
                        $orderby = " DT_CREATED $ascdesc ";

		//select values for dynamic SQL passing
		//$mselect = "  SELECT ID tran_id, TXID, TRAN_TYPE, to_char(tran_dt,'YYYY-MM-DD HH24:MI:SS') TRAN_DT, A_NO, B_NO, MSG, STEP_NO, LAST_STEP_NO,".
		//	   "         CUSTOMER_TYPE, REQUEST_ORIGIN, sf_display_process_status(step_seq, STEP_NO, LAST_STEP_NO, STATUS, '<br>') STATUS, to_char(DT_CREATED, 'YYYY-MM-DD HH24:MI:SS') DT_CREATED, STATUS STAT ".
		//           "  FROM REQUEST_LOG a $mwhere $orderby ";
                $mselect = "  SELECT ID tran_id, nvl(TXID, 'manual-' || lpad(to_char(nvl(REF_ID,ID)),8,'0')) TXID, TRAN_TYPE, to_char(tran_dt,'YYYY-MM-DD HH24:MI:SS') TRAN_DT, A_NO, B_NO, nvl(MSG, sf_display_tran_type(TRAN_TYPE)) MSG, STEP_NO, LAST_STEP_NO,".
                           "         CUSTOMER_TYPE, REQUEST_ORIGIN, sf_display_process_status(step_seq, STEP_NO, LAST_STEP_NO, STATUS, ERROR_CODE, '<br>') STATUS, to_char(DT_CREATED, 'YYYY-MM-DD HH24:MI:SS') DT_CREATED, STATUS STAT, decode(status,2,decode(sf_get_mbal_bp_dtls(ref_id,min_bal,tran_type,0), 0, 'Override', 'Regular'), 'Regular') MIN_BAL, decode(sf_get_mbal_bp_dtls(ref_id,BREATHING_PERIOD,tran_type,1), 1, 'Override', 'Regular') BREATHING_PERIOD ".
                           "  FROM REQUEST_LOG a $mwhere UNION".
                           "  SELECT 0 tran_id, 'manual-' || lpad(to_char(ID),8,'0') TXID, sf_get_tran_type(ENROLLMENT_TYPE) TRAN_TYPE, to_char(TX_DATE,'YYYY-MM-DD HH24:MI:SS') TRAN_DT, to_char(MSISDN) A_NO, null B_NO, decode(ENROLLMENT_TYPE, 'ACTIVATION', decode(OTHER_TYPE, 'REACTIVATION', 'REACTIVATION', 'ACTIVATION'),ENROLLMENT_TYPE) || ' ' || to_char(tx_date, 'MM/DD/RRRR') MSG,  0, 0 LAST_STEP_NO, ".
                           "         CUSTOMER_TYPE, REQUEST_ORIGIN, 'Scheduled' STATUS, to_char(DT_CREATED, 'YYYY-MM-DD HH24:MI:SS') DT_CREATED, 2 STAT, decode(MIN_BAL, 0, 'Override', 'Regular') MIN_BAL, decode(BREATHING_PERIOD, 1, 'Override', 'Regular') BREATHING_PERIOD ".
                           "  FROM  MANUAL_ENROLLMENT_LOG a $mwhere2 $orderby ";
		$sql     = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM (  $mselect  ) b ) WHERE MROW >=$slimit AND MROW < $elimit ";
		//print_r($mselect);

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("ListTransactions() : get_all_rows[$sql / $stmt / $totalrows]");

		//bind
		OCIDefineByName($stmt, "TRAN_ID"         , $p_tran_id          );
		OCIDefineByName($stmt, "TRAN_DT"         , $p_tran_dt          );
		OCIDefineByName($stmt, "TXID"            , $p_txid             );
		OCIDefineByName($stmt, "TRAN_TYPE"       , $p_tran_type        );
		OCIDefineByName($stmt, "A_NO"            , $p_a_no             );
		OCIDefineByName($stmt, "B_NO"            , $p_b_no             );
		OCIDefineByName($stmt, "MSG"             , $p_msg              );
		OCIDefineByName($stmt, "STEP_NO"         , $p_step_no          );
		OCIDefineByName($stmt, "LAST_STEP_NO"    , $p_last_step_no     );
		OCIDefineByName($stmt, "STATUS"          , $p_status           );
		OCIDefineByName($stmt, "CUSTOMER_TYPE"   , $p_customer_type    );
		OCIDefineByName($stmt, "REQUEST_ORIGIN"  , $p_request_origin   );
		OCIDefineByName($stmt, "DT_CREATED"      , $p_dt_created       );
		OCIDefineByName($stmt, "STAT"            , $p_stat             );
		OCIDefineByName($stmt, "MIN_BAL"         , $p_min_bal          );
		OCIDefineByName($stmt, "BREATHING_PERIOD", $p_breathing_period );

		//Execute SQL
		OCIExecute($stmt);

		//Loop thru all the records selected and display it in the table
		$idx = 0;
		while (OCIFetch($stmt))
		{
                        $alldata[$idx]['tran_id'          ] = $p_tran_id          ;
                        $alldata[$idx]['tran_dt'          ] = $p_tran_dt          ;
                        $alldata[$idx]['txid'             ] = $p_txid             ;
                        $alldata[$idx]['tran_type'        ] = $p_tran_type        ;
                        $alldata[$idx]['a_no'             ] = $p_a_no             ;
                        $alldata[$idx]['b_no'             ] = $p_b_no             ;
                        $alldata[$idx]['msg'              ] = $p_msg              ;
                        $alldata[$idx]['step_no'          ] = $p_step_no          ;
                        $alldata[$idx]['last_step_no'     ] = $p_last_step_no     ;
                        $alldata[$idx]['status'           ] = $p_status           ;
                        $alldata[$idx]['customer_type'    ] = $p_customer_type    ;
                        $alldata[$idx]['request_origin'   ] = $p_request_origin   ;
                        $alldata[$idx]['dt_created'       ] = $p_dt_created       ;
                        $alldata[$idx]['stat'             ] = $p_stat             ;
                        $alldata[$idx]['min_bal'          ] = $p_min_bal          ;
                        $alldata[$idx]['breathing_period' ] = $p_breathing_period ;

			$idx++;
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);

		//Log-off connection		
		OCILogoff($conn);

		//save
		$retdata['data']      = $alldata;
		$retdata['total']     = $totalrows ;
		$retdata['csv']       = $csvfnam;

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
		//$gEventLogger->LogEvent(__CLASS__, "ListTransactions : [ total=>'$idx'] ");

		//return data
		return $retdata;
	}


	//get form fields
	function getTableFields()
	{
		global $_POST;
		global $gSelectOptionsMaster;

		$opts_service = $gSelectOptionsMaster->getCustomerTypes();

		$table_fields= array(
				array('name' => 'q_service', 'title'=>'Service', 'type' => 'select','data_type'=> 'string', 'value' => $_POST['q_service'], 'select_options'=>$opts_service,           'select_selected' => $_POST['service']),
				);

		return $table_fields;

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
