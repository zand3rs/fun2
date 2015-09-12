<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	View_Fun_Subscribers.class.php
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


define('CSV_HEADER_SUBS_REPORT',  "msisdn, customer type, link_to, activation_dt, deactivation_dt");

//class
class View_Fun_Subscribers
{

	var $_Data;

	//constructor
	function View_Fun_Subscribers()
	{
		//reset
		$this->_Data = null;
	}


	//list
	function ListSubscribers($extravars, $q_sdate,$q_edate,$q_tran_id, $q_status,$q_msisdn,$q_linkto,$q_service)
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
		$mwhere   = " WHERE 1=1 ";
		$mwherelog= "";
		//set-where
		if((strlen($q_sdate) > 0) and (strlen($q_edate) > 0))
		{
			$mwhere   .= " AND a.cust_activation_dt >=to_date('$q_sdate','YYYY-MM-DD') AND a.cust_activation_dt<=to_date('$q_edate','YYYY-MM-DD') ";
		        $mwherelog.= " StartDt=$q_sdate EndDt=$q_edate ";
		}
		else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_sdate)) 
		{
			$mwhere   .= " AND a.cust_activation_dt>=tx_date('$q_sdate','YYYY-MM-DD') AND a.cust_activation_dt<=to_date('$q_sdate','YYYY-MM-DD') ";
		        $mwherelog.= " StartDt=$q_sdate ";
		}
                else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_edate))
                {
                        $mwhere   .= " AND a.cust_activation_dt=to_date('$q_edate','YYYY-MM-DD')AND a.cust_activation_dt<=to_date('$q_edate','YYYY-MM-DD') ";
		        $mwherelog.= " EndDt=$q_edate ";
                }

		if(strlen($q_msisdn) > 0)
		{
			$mwhere .= " AND a.msisdn='". ora_addslashes($q_msisdn) ."' ";	
		        $mwherelog.= " OFW SIM=$q_msisdn ";
		}

		if(strlen($q_linkto) > 0)
		{
			$mwhere .= " AND a.link_to='". ora_addslashes($q_linkto) ."' ";	
		        $mwherelog.= " LOCAL SIM=$q_linkto ";
		}

		if($q_status!='ALL')
		{
			$mwhere .= " AND a.status='$q_status' ";
		        $mwherelog.= " Status=$q_status ";
		}
		//print_r($mwhere);

		//fmt
		$whatsort    = $extravars['whatsort'];

		//type of order
		$ascdesc     = (!$whatsort) ? (" ASC ") : ( " DESC " );	


		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT COUNT(1) AS COUNT FROM SIM_ACTIVATION_LINKS a $mwhere ";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"COUNT",$totalrows);		

		$this->debug("ListSubscribers() : get_max_row[$sql / $stmt / $totalrows]");

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
		$gEventLogger->LogEvent(__CLASS__, "SEARCHED By:$mwherelog  TOTAL Rows:$totalrows PAGE:$nPageNo/$nPageTo");

		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//orderby
		$orderby = " ORDER BY a.status, a.cust_activation_dt desc, a.cust_deactivation_dt $ascdesc";

                if ($_GET['sort'] == 'TRAN_ID')
                        $orderby = " a.id $ascdesc ";
                else if ($_GET['sort'] == 'STATUS')
                        $orderby = " a.status $ascdesc ";
                else if ($_GET['sort'] == 'MSISDN')
                        $orderby = " a.msisdn $ascdesc ";
                else if ($_GET['sort'] == 'START')
                        $orderby = " a.cust_activation_dt $ascdesc ";
                else if ($_GET['sort'] == 'END')
                        $orderby = " a.cust_deactivation_dt $ascdesc ";


		//select values for dynamic SQL passing
		$mselect = "  SELECT id tran_id, msisdn, link_to, link_to_type, customer_type, status, cs_username, ".
                           "         to_char(cust_activation_tm,'YYYY-MM-DD HH24:MI:SS') activation_dt, to_char(cust_deactivation_tm,'YYYY-MM-DD HH24:MI:SS') deactivation_dt, ".
                           "         to_char(notify_date,'YYYY-MM-DD HH24:MI:SS') notify_date, to_char(ods_last_chk_dt,'YYYY-MM-DD') ods_last_chk_dt ".
		           "  FROM SIM_ACTIVATION_LINKS a $mwhere $orderby ";
		$sql     = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM (  $mselect  ) b ) WHERE MROW >=$slimit AND MROW < $elimit ";
		//print_r($mselect);

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("ListSubscribers() : get_all_rows[$sql / $stmt / $totalrows]");


		//bind
		OCIDefineByName($stmt, "TRAN_ID"            , $p_tran_id                );
		OCIDefineByName($stmt, "CUSTOMER_TYPE"      , $p_customer_type          );
		OCIDefineByName($stmt, "MSISDN"             , $p_msisdn                 );
		OCIDefineByName($stmt, "STATUS"             , $p_status                 );
		OCIDefineByName($stmt, "LINK_TO"            , $p_link_to                );
		OCIDefineByName($stmt, "LINK_TO_TYPE"       , $p_link_to_type           );
		OCIDefineByName($stmt, "ACTIVATION_DT"      , $p_activation_dt          );
		OCIDefineByName($stmt, "DEACTIVATION_DT"    , $p_deactivation_dt        );
		OCIDefineByName($stmt, "ODS_LAST_CHK_DT"    , $p_ods_last_chk_dt        );
		OCIDefineByName($stmt, "CS_USERNAME"        , $p_cs_username            );
		OCIDefineByName($stmt, "NOTIFY_DATE"        , $p_notify_date            );

		//Execute SQL
		OCIExecute($stmt);

		//open csv here
		//$csvfnam = date("Ymd").'.'.get_uid().".csv";
		//$csvfile = SMARTY_INC_DIR."/log/$csvfnam";
		//$csvcont = '';

		
		//Loop thru all the records selected and display it in the table
		$idx = 0;
		while (OCIFetch($stmt))
		{
			$alldata[$idx]['tran_id'            ] = $p_tran_id             ;
			$alldata[$idx]['msisdn'             ] = $p_msisdn              ;
			$alldata[$idx]['status'             ] = $p_status              ;
			$alldata[$idx]['customer_type'      ] = $p_customer_type       ;
			$alldata[$idx]['link_to'            ] = $p_link_to             ;
			$alldata[$idx]['link_to_type'       ] = $p_link_to_type        ;
			$alldata[$idx]['activation_dt'      ] = $p_activation_dt       ;
			$alldata[$idx]['deactivation_dt'    ] = $p_deactivation_dt     ;
			$alldata[$idx]['ods_last_chk_dt'    ] = $p_ods_last_chk_dt     ;
			$alldata[$idx]['cs_username'        ] = $p_cs_username         ;
			$alldata[$idx]['notify_date'        ] = $p_notify_date         ;
                                                     
		                                     
			//csv-create
			//$csvdata   = null;
			//$csvdata[] = $alldata[$idx]['tran_id'        ]; 
			//$csvdata[] = $alldata[$idx]['msisdn'         ]; 
			//$csvdata[] = $alldata[$idx]['customer_type'  ]; 
			//$csvdata[] = $alldata[$idx]['link_to'        ]; 
			//$csvdata[] = $alldata[$idx]['link_to_type'   ]; 
			//$csvdata[] = $alldata[$idx]['activation_dt'  ]; 
			//$csvdata[] = $alldata[$idx]['deactivation_dt'];    
			//$csvstr    = join('","', $csvdata);
                        //
			//$csvcont .= "\"$csvstr\"\n";

			$idx++;
		}

		//close csv
		//file_save($csvfile,CSV_HEADER_SUBS_REPORT."\n\n".$csvcont);
                //$rep_gen_dt = date("Y-m-d H:i:s");
                //file_save($csvfile,"List of Fun Subscribers\nReport Generation: $rep_gen_dt\n\n".CSV_HEADER_SUBS_REPORT."\n\n".$csvcont);

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
		//$gEventLogger->LogEvent(__CLASS__, "ListSubscribers(#b) : [ total=>'$idx'] ");

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
