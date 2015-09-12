<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	View_Subscribers.class.php
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


define('CSV_HEADER_SUBS_REPORT',  "msisdn, country, subscriber_name, name_of_req_party, activation_dt, deactivation_dt, source, daily_bal, extended, min_bal, breathing_period, last_daily_bal_chk, last_country_chk, hot_country_cnt");

//class
class View_Subscribers
{

	var $_Data;

	//constructor
	function View_Subscribers()
	{
		//reset
		$this->_Data = null;
	}


	//list
	function ListSubscribers($extravars, $q_sdate,$q_edate,$q_tran_id, $q_status,$q_msisdn,$q_service)
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
		$mwhere   = " WHERE status <> 'INACTIVE' ";
		$mwherelog= "";
		//set-where
		if((strlen($q_sdate) > 0) and (strlen($q_edate) > 0))
		{
			$mwhere   .= " AND a.activation_dt >=to_date('$q_sdate','YYYY-MM-DD') AND a.activation_dt<=to_date('$q_edate','YYYY-MM-DD') ";
		        $mwherelog.= " StartDt=$q_sdate EndDt=$q_edate ";
		}
		else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_sdate)) 
		{
			$mwhere   .= " AND a.activation_dt=to_date('$q_sdate','YYYY-MM-DD') "; 
		        $mwherelog.= " StartDt=$q_sdate ";
		}
                else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_edate))
                {
                        $mwhere   .= " AND a.activation_dt=to_date('$q_edate','YYYY-MM-DD') ";
		        $mwherelog.= " EndDt=$q_edate ";
                }

		if(strlen($q_msisdn) > 0)
		{
			$mwhere .= " AND a.msisdn='". ora_addslashes($q_msisdn) ."' ";	
		        $mwherelog.= " MSISDN=$q_msisdn ";
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
		$sql  = " SELECT COUNT(1) AS COUNT FROM SIM_ACTIVATION a $mwhere ";

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
		$orderby = " ORDER BY a.status, a.activation_dt desc, a.deactivation_dt $ascdesc";

                if ($_GET['sort'] == 'TRAN_ID')
                        $orderby = " a.tran_id $ascdesc ";
                else if ($_GET['sort'] == 'STATUS')
                        $orderby = " a.status $ascdesc ";
                else if ($_GET['sort'] == 'MSISDN')
                        $orderby = " a.msisdn $ascdesc ";
                else if ($_GET['sort'] == 'START')
                        $orderby = " a.activation_dt $ascdesc ";
                else if ($_GET['sort'] == 'END')
                        $orderby = " a.deactivation_dt $ascdesc ";


		//select values for dynamic SQL passing
		$mselect = "  SELECT id tran_id, msisdn, country_name country, subscriber_name, name_of_req_party, status, ".
                           "         to_char(activation_tm,'YYYY-MM-DD HH24:MI:SS') activation_dt, to_char(deactivation_dt,'YYYY-MM-DD') deactivation_dt, ".
		           "         request_origin act_deact_src, daily_bal, decode(nvl(extended,0),0,'No', 'Yes') extended, duration, ".
                           "         decode(min_bal, 0, 'Exempted', min_bal) min_bal, decode(breathing_period, 0, 'Regular', 'Exempted') breathing_period, ".
                           "         last_daily_bal, last_daily_bal_chk, last_country_chk, hot_country_cnt, customer_type, ".
                           "         to_char(reactivation_dt,'YYYY-MM-DD') reactivation_dt ".
		           "  FROM SIM_ACTIVATION a $mwhere $orderby ";
		$sql     = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM (  $mselect  ) b ) WHERE MROW >=$slimit AND MROW < $elimit ";
		//print_r($mselect);

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("ListSubscribers() : get_all_rows[$sql / $stmt / $totalrows]");


		//bind
		OCIDefineByName($stmt, "TRAN_ID"            , $p_tran_id                );
		OCIDefineByName($stmt, "MSISDN"             , $p_msisdn                 );
		OCIDefineByName($stmt, "COUNTRY"            , $p_country                );
		OCIDefineByName($stmt, "SUBSCRIBER_NAME"    , $p_subscriber_name        );
		OCIDefineByName($stmt, "NAME_OF_REQ_PARTY"  , $p_name_of_req_party      );
		OCIDefineByName($stmt, "STATUS"             , $p_status                 );
		OCIDefineByName($stmt, "CUSTOMER_TYPE"      , $p_customer_type          );
		OCIDefineByName($stmt, "ACTIVATION_DT"      , $p_activation_dt          );
		OCIDefineByName($stmt, "DEACTIVATION_DT"    , $p_deactivation_dt        );
		OCIDefineByName($stmt, "ACT_DEACT_SRC"      , $p_act_deact_src          );
		OCIDefineByName($stmt, "DAILY_BAL"          , $p_daily_bal              );
		OCIDefineByName($stmt, "EXTENDED"           , $p_extended               );
		OCIDefineByName($stmt, "MIN_BAL"            , $p_min_bal                );
		OCIDefineByName($stmt, "BREATHING_PERIOD"   , $p_breathing_period       );
		OCIDefineByName($stmt, "LAST_DAILY_BAL"     , $p_last_daily_bal         );
		OCIDefineByName($stmt, "LAST_DAILY_BAL_CHK" , $p_last_daily_bal_chk     );
		OCIDefineByName($stmt, "LAST_COUNTRY_CHK"   , $p_last_country_chk       );
		OCIDefineByName($stmt, "HOT_COUNTRY_CNT"    , $p_hot_country_cnt        );
		OCIDefineByName($stmt, "REACTIVATION_DT"    , $p_reactivation_dt        );

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
			$alldata[$idx]['country'            ] = $p_country             ;
			$alldata[$idx]['subscriber_name'    ] = $p_subscriber_name     ;
			$alldata[$idx]['name_of_req_party'  ] = $p_name_of_req_party   ;
			$alldata[$idx]['status'             ] = $p_status              ;
			$alldata[$idx]['customer_type'      ] = $p_customer_type       ;
			$alldata[$idx]['activation_dt'      ] = $p_activation_dt       ;
			$alldata[$idx]['deactivation_dt'    ] = $p_deactivation_dt     ;
			$alldata[$idx]['act_deact_src'      ] = $p_act_deact_src       ;
			$alldata[$idx]['daily_bal'          ] = $p_daily_bal           ;
			$alldata[$idx]['extended'           ] = $p_extended            ;
			$alldata[$idx]['min_bal'            ] = $p_min_bal             ;
			$alldata[$idx]['breathing_period'   ] = $p_breathing_period    ;
			$alldata[$idx]['last_daily_bal'     ] = $p_last_daily_bal      ;
			$alldata[$idx]['last_daily_bal_chk' ] = $p_last_daily_bal_chk  ;
			$alldata[$idx]['last_country_chk'   ] = $p_last_country_chk    ;
			$alldata[$idx]['hot_country_cnt'    ] = $p_hot_country_cnt     ;
			$alldata[$idx]['reactivation_dt'    ] = $p_reactivation_dt     ;
                                                     
		                                     
			//csv-create
			//$csvdata   = null;
			//$csvdata[] = $alldata[$idx]['tran_id'             ]; 
			//$csvdata[] = $alldata[$idx]['msisdn'              ]; 
			//$csvdata[] = $alldata[$idx]['country'             ]; 
			//$csvdata[] = $alldata[$idx]['subscriber_name'     ]; 
			//$csvdata[] = $alldata[$idx]['name_of_req_party'   ]; 
			//$csvdata[] = $alldata[$idx]['status'              ];    
			//$csvdata[] = $alldata[$idx]['activation_dt'       ];    
			//$csvdata[] = $alldata[$idx]['deactivation_dt'     ]; 
			//$csvdata[] = $alldata[$idx]['p_act_deact_src'     ];
			//$csvdata[] = $alldata[$idx]['p_daily_bal'         ];
			//$csvdata[] = $alldata[$idx]['p_extended'          ];
			//$csvdata[] = $alldata[$idx]['p_min_bal'           ];
			//$csvdata[] = $alldata[$idx]['p_breathing_period'  ];
			//$csvdata[] = $alldata[$idx]['p_last_daily_bal'    ];
			//$csvdata[] = $alldata[$idx]['p_last_daily_bal_chk'];
			//$csvdata[] = $alldata[$idx]['p_last_country_chk'  ];
			//$csvdata[] = $alldata[$idx]['p_hot_country_cnt'   ];
			//$csvstr    = join('","', $csvdata);
                        //
			//$csvcont .= "\"$csvstr\"\n";

			$idx++;
		}

		//close csv
		//file_save($csvfile,CSV_HEADER_SUBS_REPORT."\n\n".$csvcont);
                //$rep_gen_dt = date("Y-m-d H:i:s");
                //file_save($csvfile,"List of Subscribers\nReport Generation: $rep_gen_dt\n\n".CSV_HEADER_SUBS_REPORT."\n\n".$csvcont);

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


	//init
	function GetSubscriber($id='')
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
		$mselect = "  SELECT id tran_id, msisdn, country_name country, subscriber_name, name_of_req_party, status, ".
                           "         to_char(activation_dt,'YYYY-MM-DD') activation_dt, to_char(deactivation_dt,'YYYY-MM-DD') deactivation_dt, to_char(reactivation_dt,'YYYY-MM-DD') reactivation_dt,".
		           "         request_origin act_deact_src, decode(nvl(extended,0),0,'No', 'Yes') extended, duration, ".
                           "         decode(min_bal, 0, 'Exempted', min_bal) min_bal, duration, decode(breathing_period, 0, 'Exempted', breathing_period) breathing_period, to_char(breathing_period_dt,'YYYY-MM-DD') breathing_period_dt, ".
                           "         imsi, decode(trunc(last_daily_bal_chk), trunc(nvl(reactivation_dt, activation_dt)), null, daily_bal) daily_bal, decode(trunc(last_daily_bal_chk), trunc(nvl(reactivation_dt, activation_dt)), null, last_daily_bal ) last_daily_bal, ". 
                           "         decode(trunc(last_daily_bal_chk), trunc(nvl(reactivation_dt, activation_dt)), null, to_char(last_daily_bal_chk, 'YYYY-MM-DD')) last_daily_bal_chk, ". 
                           "         to_char(last_country_chk, 'YYYY-MM-DD') last_country_chk, hot_country_cnt, remarks, customer_type, cs_username, to_char(notify_date, 'YYYY-MM-DD') notify_date ".
		           "  FROM SIM_ACTIVATION a $mwhere ";
		$sql     = "  $mselect ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("GetSubscriber() a#: [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "TRAN_ID"            , $P_TRAN_ID                );
		OCIDefineByName($stmt, "MSISDN"             , $P_MSISDN                 );
		OCIDefineByName($stmt, "COUNTRY"            , $P_COUNTRY                );
		OCIDefineByName($stmt, "SUBSCRIBER_NAME"    , $P_SUBSCRIBER_NAME        );
		OCIDefineByName($stmt, "NAME_OF_REQ_PARTY"  , $P_NAME_OF_REQ_PARTY      );
		OCIDefineByName($stmt, "STATUS"             , $P_STATUS                 );
		OCIDefineByName($stmt, "ACTIVATION_DT"      , $P_ACTIVATION_DT          );
		OCIDefineByName($stmt, "DEACTIVATION_DT"    , $P_DEACTIVATION_DT        );
		OCIDefineByName($stmt, "REACTIVATION_DT"    , $P_REACTIVATION_DT        );
		OCIDefineByName($stmt, "ACT_DEACT_SRC"      , $P_ACT_DEACT_SRC          );
		OCIDefineByName($stmt, "DAILY_BAL"          , $P_DAILY_BAL              );
		OCIDefineByName($stmt, "EXTENDED"           , $P_EXTENDED               );
		OCIDefineByName($stmt, "MIN_BAL"            , $P_MIN_BAL                );
		OCIDefineByName($stmt, "DURATION"           , $P_DURATION               );
		OCIDefineByName($stmt, "BREATHING_PERIOD"   , $P_BREATHING_PERIOD       );
		OCIDefineByName($stmt, "BREATHING_PERIOD_DT", $P_BREATHING_PERIOD_DT    );
		OCIDefineByName($stmt, "IMSI"               , $P_IMSI                   );
		OCIDefineByName($stmt, "LAST_DAILY_BAL"     , $P_LAST_DAILY_BAL         );
		OCIDefineByName($stmt, "LAST_DAILY_BAL_CHK" , $P_LAST_DAILY_BAL_CHK     );
		OCIDefineByName($stmt, "LAST_COUNTRY_CHK"   , $P_LAST_COUNTRY_CHK       );
		OCIDefineByName($stmt, "HOT_COUNTRY_CNT"    , $P_HOT_COUNTRY_CNT        );
		OCIDefineByName($stmt, "CUSTOMER_TYPE"      , $P_CUSTOMER_TYPE          );
		OCIDefineByName($stmt, "REMARKS"            , $P_REMARKS                );
		OCIDefineByName($stmt, "CS_USERNAME"        , $P_CS_USERNAME            );
		OCIDefineByName($stmt, "NOTIFY_DATE"        , $P_NOTIFY_DATE            );

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
			$P_TRAN_ID             ;
			$P_MSISDN              ;
			$P_COUNTRY             ;
			$P_SUBSCRIBER_NAME     ;
			$P_NAME_OF_REQ_PARTY   ;
			$P_STATUS              ;
			$P_ACTIVATION_DT       ;
			$P_DEACTIVATION_DT     ;
			$P_REACTIVATION_DT     ;
			$P_ACT_DEACT_SRC       ;
			$P_DAILY_BAL           ;
			$P_EXTENDED            ;
			$P_MIN_BAL             ;
			$P_DURATION            ;
			$P_BREATHING_PERIOD    ;
			$P_BREATHING_PERIOD_DT ;
			$P_IMSI                ;
			$P_LAST_DAILY_BAL      ;
			$P_LAST_DAILY_BAL_CHK  ;
			$P_LAST_COUNTRY_CHK    ;
			$P_HOT_COUNTRY_CNT     ;
			$P_CUSTOMER_TYPE       ;
			$P_REMARKS             ;
			$P_CS_USERNAME         ;
			$P_NOTIFY_DATE         ;
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Get() : [ $sql // $totalrows // $is_error / $err_msg] ");
		$this->debug("GetSubscriber b#() : [$totalrows // $is_error / $err_msg] ]");
		$gEventLogger->LogEvent(__CLASS__, "View Details = > MSISDN: $P_MSISDN ");

		$alldata = array(
				'error'		=> $is_error, 
				'error_msg' 	=> $err_msg,
				'bind_total'	=> $totalrows,
				'bind_data'	=> array(
				'tran_id'            => $P_TRAN_ID             ,
				'msisdn'             => $P_MSISDN              ,
				'country'            => $P_COUNTRY             ,
				'subscriber_name'    => $P_SUBSCRIBER_NAME     ,
				'name_of_req_party'  => $P_NAME_OF_REQ_PARTY   ,
				'status'             => $P_STATUS              ,
				'activation_dt'      => $P_ACTIVATION_DT       ,
				'deactivation_dt'    => $P_DEACTIVATION_DT     ,
				'reactivation_dt'    => $P_REACTIVATION_DT     ,
				'act_deact_src'      => $P_ACT_DEACT_SRC       ,
				'daily_bal'          => $P_DAILY_BAL           ,
				'extended'           => $P_EXTENDED            ,
				'min_bal'            => $P_MIN_BAL             ,
				'duration'           => $P_DURATION            ,
				'breathing_period'   => $P_BREATHING_PERIOD    ,
				'breathing_period_dt'=> $P_BREATHING_PERIOD_DT ,
				'imsi'               => $P_IMSI                ,
				'last_daily_bal'     => $P_LAST_DAILY_BAL      ,
				'last_daily_bal_chk' => $P_LAST_DAILY_BAL_CHK  ,
				'last_country_chk'   => $P_LAST_COUNTRY_CHK    ,
				'hot_country_cnt'    => $P_HOT_COUNTRY_CNT     ,
				'customer_type'      => $P_CUSTOMER_TYPE       ,
				'remarks'            => $P_REMARKS             ,
				'cs_username'        => $P_CS_USERNAME         ,
				'notify_date'        => $P_NOTIFY_DATE         ,
				));

		return $alldata;

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

	//get form fields
	function getTableFields4Update($id="")
	{
		$data = $this->GetSubscriber($id);
		$post = $data['bind_data'];
		global $gSelectOptionsMaster;

		$table_fields= array(
				array('name' => 'tran_id'            , 'title'=>'TxID'                 , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['tran_id'            ] ),
				array('name' => 'msisdn'             , 'title'=>'MSISDN'               , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['msisdn'             ] ),
				array('name' => 'imsi'               , 'title'=>'IMSI'                 , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['imsi'               ] ),
				array('name' => 'customer_type'      , 'title'=>'Profile'              , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['customer_type'   ]  ),
				array('name' => 'subscriber_name'    , 'title'=>'Subscriber Name'      , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['subscriber_name'    ] ),
				array('name' => 'name_of_req_party'  , 'title'=>'Name Of Req Party'    , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['name_of_req_party'  ] ),
				array('name' => 'activation_dt'      , 'title'=>'Activation Date'      , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['activation_dt'      ] ),
				array('name' => 'deactivation_dt'    , 'title'=>'Deactivation Date'    , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['deactivation_dt'    ] ),
				array('name' => 'reactivation_dt'    , 'title'=>'Reactivation Date'    , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['reactivation_dt'    ] ),
				array('name' => 'extended'           , 'title'=>'Extended?'            , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['extended'           ] ),
				array('name' => 'min_bal'            , 'title'=>'Minimum Balance'      , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['min_bal'            ] ),
				array('name' => 'duration'           , 'title'=>'Duration'             , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['duration'           ] ),
				array('name' => 'cs_username'        , 'title'=>'CS Username'          , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['cs_username'        ] ),
				array('name' => 'act_deact_src'      , 'title'=>'Source'               , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['act_deact_src'      ] ),
				array('name' => 'remarks'            , 'title'=>'Remarks'              , 'type' => 'text', 'max' => 320, 'data_type'=> 'string', 'value' => $post['remarks'            ] ),
				array('name' => 'notify_date'        , 'title'=>'Expiry Notification'  , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['notify_date'        ] ),
				array('name' => 'last_daily_bal_chk' , 'title'=>'Last Daily Bal Check' , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['last_daily_bal_chk' ] ),
				array('name' => 'daily_bal'          , 'title'=>'Req. Daily Balance'   , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['daily_bal'          ] ),
				array('name' => 'last_daily_bal'     , 'title'=>'Last Daily Balance'   , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['last_daily_bal'     ] ),
				array('name' => 'last_country_chk'   , 'title'=>'Last Country Check'   , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['last_country_chk'   ] ),
				array('name' => 'country'            , 'title'=>'Country'              , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['country'            ] ),
				array('name' => 'breathing_period'   , 'title'=>'Breathing Period'     , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['breathing_period'   ] ),
				array('name' => 'breathing_period_dt', 'title'=>'Breathing Period Dt'  , 'type' => 'text', 'max' => 120, 'data_type'=> 'string', 'value' => $post['breathing_period_dt'] ),
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
