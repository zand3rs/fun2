<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	View_Whitelisted.class.php
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


define('CSV_HEADER_SUBS_REPORT',  "msisdn, remarks, date created, created by");

//class
class View_Whitelisted
{

	var $_Data;

	//constructor
	function View_Whitelisted()
	{
		//reset
		$this->_Data = null;
	}


	//list
	function ListSubscribers($extravars, $q_sdate,$q_edate,$q_msisdn )
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
		  q_msisdn
		 **/
		$mwhere   = " WHERE 1=1 ";
		$mwherelog= "";
		//set-where
		if((strlen($q_sdate) > 0) and (strlen($q_edate) > 0))
		{
			$mwhere   .= " AND a.tran_dt >=to_date('$q_sdate','YYYY-MM-DD') AND a.tran_dt<=to_date('$q_edate','YYYY-MM-DD') ";
		        $mwherelog.= " StartDt=$q_sdate EndDt=$q_edate ";
		}
		else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_sdate)) 
		{
			$mwhere   .= " AND a.tran_dt=tx_date('$q_sdate','YYYY-MM-DD')";
		        $mwherelog.= " StartDt=$q_sdate ";
		}
                else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_edate))
                {
                        $mwhere   .= " AND a.tran_dt=to_date('$q_edate','YYYY-MM-DD') ";
		        $mwherelog.= " EndDt=$q_edate ";
                }

		if(strlen($q_msisdn)>0)
		{
			$mwhere .= " AND a.msisdn=". ora_addslashes($q_msisdn);	
		        $mwherelog.= " MSISDN=$q_msisdn ";
		}
		//print_r($mwhere);

		//fmt
		$whatsort    = $extravars['whatsort'];

		//type of order
		$ascdesc     = (!$whatsort) ? (" ASC ") : ( " DESC " );	


		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT COUNT(1) AS COUNT FROM WHITELISTED_LOG a $mwhere ";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"COUNT",$totalrows);		

		$this->debug("ListWhitelisted() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);

		// logoff connection
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "ListWhitelisted : [WHERE=>'$mwhere'] [ TOTAL=>'$totalrows'] ");

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
		$orderby = " ORDER BY a.dt_created $ascdesc";

                if ($sortedby == 'msisdn')
                        $orderby = " ORDER BY a.tran_id $ascdesc ";
                else if ($sortedby == 'dt_created')
                        $orderby = " ORDER BY a.dt_created $ascdesc ";

		//select values for dynamic SQL passing
		$mselect = "  SELECT msisdn,to_char(dt_created,'YYYY-MM-DD HH24:MI:SS') dt_created,".
		           "        created_by, remarks ".
		           "  FROM WHITELISTED_LOG  a $mwhere $orderby ";
		$sql     = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM (  $mselect  ) b ) WHERE MROW >=$slimit AND MROW < $elimit ";
		//print_r($mselect);

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("ListWhitelisted() : get_all_rows[$sql / $stmt / $totalrows]");


		//bind
		OCIDefineByName($stmt, "MSISDN"     , $p_msisdn       );
		OCIDefineByName($stmt, "REMARKS"    , $p_remarks      );
		OCIDefineByName($stmt, "DT_CREATED" , $p_dt_created   );
		OCIDefineByName($stmt, "CREATED_BY" , $p_created_by   );

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
			$alldata[$idx]['msisdn'       ] = $p_msisdn     ;
			$alldata[$idx]['remarks'      ] = $p_remarks    ;
			$alldata[$idx]['dt_created'   ] = $p_dt_created ;
			$alldata[$idx]['created_by'   ] = $p_created_by ;

		
			//csv-create
			//$csvdata   = null;
			//$csvdata[] = $alldata[$idx]['msisdn'      ]; 
			//$csvdata[] = $alldata[$idx]['remarks'     ]; 
			//$csvdata[] = $alldata[$idx]['dt_created'  ]; 
			//$csvdata[] = $alldata[$idx]['created_by'  ]; 
			//$csvstr    = join('","', $csvdata);
                        //
			//$csvcont .= "\"$csvstr\"\n";

			$idx++;
		}

		//close csv
		//file_save($csvfile,CSV_HEADER_SUBS_REPORT."\n\n".$csvcont);
                //$rep_gen_dt = date("Y-m-d H:i:s");
                //file_save($csvfile,"List of Whitelisted Subs\nReport Generation: $rep_gen_dt\n\n".CSV_HEADER_SUBS_REPORT."\n\n".$csvcont);

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
		//$gEventLogger->LogEvent(__CLASS__, "ListWhitelisted(#b) : [ total=>'$idx'] ");

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
