<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	View_Audit_Log.class.php
#	@Description	:	encapsulator for table user
#
#
#	@Date		: 2009-07-18
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

define("CSV_HEADER_AUDIT_REPORT", "Tran ID,User Code,Role Code,Module Name,IP Address, MSISDN, URL, Message,Start Time\n");

//class
class View_Audit_Log
{

	var $_Data;

	//constructor
	function View_Audit_Log()
	{
		//reset
		$this->_Data = null;
	}


	//list
	function List_Audit_Log($extravars, $q_sdate, $q_edate, $q_user_code, $q_role_code, $q_mod_name)
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
		  q_mod_name
		  q_user_code
		  q_role_code
		 **/
		$temp_file  = $_POST['sfile'];
		$mwhere   = " WHERE 1=1 ";
		//set-where
		if((strlen($q_sdate) > 0) and (strlen($q_edate) > 0))
		{
			$mwhere   .= " AND dt_created>=to_date('$q_sdate','YYYY-MM-DD') AND dt_created<=to_date('$q_edate','YYYY-MM-DD')+1 ";
		}
		else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_sdate)) 
		{
			$mwhere   .= " AND dt_created>=to_date('$q_sdate','YYYY-MM-DD') AND dt_created<=to_date('$q_sdate','YYYY-MM-DD')+1 ";
		}
                else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_edate))
                {
                        $mwhere   .= " AND dt_created=to_date('$q_edate','YYYY-MM-DD')AND dt_created<=to_date('$q_edate','YYYY-MM-DD')+1 ";
                }

		if(strlen($q_mod_name) > 0 and ($q_mod_name!='%'))
		{
			$mwhere .= " AND mod_name like'%". ora_addslashes($q_mod_name) ."%' ";	
		}
		if(strlen($q_user_code) > 0)
		{
			$mwhere .= " AND user_code like '%". ora_addslashes($q_user_code) ."%' ";	
		}
		if(strlen($q_role_code) > 0 and ($q_role_code!='%'))
		{
			$mwhere .= " AND role_code like '%". ora_addslashes($q_role_code) ."%' ";	
		}


		//fmt
		$whatsort    = $extravars['whatsort'];

		//type of order
		//$ascdesc     = (!$whatsort) ? ( " DESC " ) : (" ASC ") ;	
		$ascdesc     = " DESC ";	


		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT COUNT(1) AS CNT FROM USER_LOG  $mwhere ";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"CNT",$totalrows);		

		$this->debug("List_Audit_Log() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);

		// logoff connection
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "List_Audit_Log(#a) : [ searched by=>'$mwhere'] ");

		//chk rowcount
		if(0 == $totalrows )     
		{                   
			return $retdata;	
		}                   

		// prepare download file
		if ((strlen($temp_file)>0) && ($temp_file==1)) {
			$csvdtl = $this->Download_Audit_Log($q_sdate, $q_edate, $q_user_code, $q_role_code, $q_mod_name);
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
                	
                	
			//logon to oracle database
			$conn = $myOraDb->dbConnection();
                	
			//orderby
			$orderby = " ORDER BY  dt_created $ascdesc"; //, id $ascdesc";
                	
                	
			//select values for dynamic SQL passing
			$mselect  = "select ID TRAN_ID, USER_CODE, ROLE_CODE, MOD_NAME, IP_ADDR, URL, LOGS MESSAGE, to_char(DT_CREATED,'YYYY-MM-DD HH24:MI:SS') START_TM, MSISDN ".
				    "from   USER_LOG a $mwhere $orderby";
			$sql     =  "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM (  $mselect  ) b ) WHERE MROW >=$slimit AND MROW < $elimit ";
                	
			//Parse SQL passed
			$stmt = OCIParse($conn,$sql);
                	
			$this->debug("List_Audit_Log() : get_all_rows[$sql / $stmt / $totalrows]");
                	
                	
			//bind
			OCIDefineByName($stmt, 'TRAN_ID'      , $tran_id      );
			OCIDefineByName($stmt, 'USER_CODE'    , $user_code    );
			OCIDefineByName($stmt, 'ROLE_CODE'    , $role_code    );
			OCIDefineByName($stmt, 'MOD_NAME'     , $mod_name     );
			OCIDefineByName($stmt, 'IP_ADDR'      , $ip_addr      );
			OCIDefineByName($stmt, 'URL'          , $url          );
			OCIDefineByName($stmt, 'MESSAGE'      , $message      );
			OCIDefineByName($stmt, 'START_TM'     , $start_tm     );
			OCIDefineByName($stmt, 'MSISDN'       , $msisdn       );
                	
			//Execute SQL
			OCIExecute($stmt);
                	
			//Loop thru all the records selected and display it in the table
			$idx = 0;
			while (OCIFetch($stmt))
			{
				$alldata[$idx]['tran_id'  ] = $tran_id        ;
				$alldata[$idx]['user_code'] = $user_code      ;
				$alldata[$idx]['role_code'] = $role_code      ;
				$alldata[$idx]['mod_name' ] = $mod_name       ;
				$alldata[$idx]['ip_addr'  ] = $ip_addr        ;
				$alldata[$idx]['url'      ] = $url            ;
				$alldata[$idx]['message'  ] = $message        ;
				$alldata[$idx]['start_tm' ] = $start_tm       ;
				$alldata[$idx]['msisdn'   ] = $msisdn         ;
                	
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
		//$gEventLogger->LogEvent(__CLASS__, "List_Audit_Log(#b) : [ total=>'$idx'] ");

		//return data
		return $retdata;
	}

	//list
	function Download_Audit_Log($q_sdate, $q_edate, $q_user_code, $q_role_code, $q_mod_name)
	{

		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		//reset
		$retdata  = null;
		$mwhere   = " WHERE 1=1 ";

		//set-where
		if((strlen($q_sdate) > 0) and (strlen($q_edate) > 0))
		{
			$mwhere   .= " AND dt_created>=to_date('$q_sdate','YYYY-MM-DD') AND dt_created<=to_date('$q_edate','YYYY-MM-DD')+1 ";
		}
		else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_sdate)) 
		{
			$mwhere   .= " AND dt_created>=to_date('$q_sdate','YYYY-MM-DD') AND dt_created<=to_date('$q_sdate','YYYY-MM-DD')+1 ";
		}
                else if(@preg_match("/^\d{4}-?\d{2}-?\d{2}$/i", $q_edate))
                {
                        $mwhere   .= " AND dt_created=to_date('$q_edate','YYYY-MM-DD')AND dt_created<=to_date('$q_edate','YYYY-MM-DD')+1 ";
                }

		if(strlen($q_mod_name) > 0 and ($q_mod_name!='%'))
		{
			$mwhere .= " AND mod_name like'%". ora_addslashes($q_mod_name) ."%' ";	
		}
		if(strlen($q_user_code) > 0)
		{
			$mwhere .= " AND user_code like '%". ora_addslashes($q_user_code) ."%' ";	
		}
		if(strlen($q_role_code) > 0 and ($q_role_code!='%'))
		{
			$mwhere .= " AND role_code like '%". ora_addslashes($q_role_code) ."%' ";	
		}

		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//orderby
		$ascdesc = " DESC ";	
		$orderby = " ORDER BY  dt_created $ascdesc"; //, id $ascdesc";

		//select values for dynamic SQL passing
		$mselect  = "select ID TRAN_ID, USER_CODE, ROLE_CODE, MOD_NAME, IP_ADDR, URL, LOGS MESSAGE, to_char(DT_CREATED,'YYYY-MM-DD HH24:MI:SS') START_TM, MSISDN ".
			    "from   USER_LOG a $mwhere $orderby";
		$sql     =  "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM (  $mselect  ) b ) ";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("Download_Audit_Log() : get_all_rows[$sql / $stmt]");

		//bind
		OCIDefineByName($stmt, 'TRAN_ID'      , $tran_id      );
		OCIDefineByName($stmt, 'USER_CODE'    , $user_code    );
		OCIDefineByName($stmt, 'ROLE_CODE'    , $role_code    );
		OCIDefineByName($stmt, 'MOD_NAME'     , $mod_name     );
		OCIDefineByName($stmt, 'IP_ADDR'      , $ip_addr      );
		OCIDefineByName($stmt, 'URL'          , $url          );
		OCIDefineByName($stmt, 'MESSAGE'      , $message      );
		OCIDefineByName($stmt, 'START_TM'     , $start_tm     );
		OCIDefineByName($stmt, 'MSISDN'       , $msisdn       );

		//Execute SQL
		OCIExecute($stmt);

		//open csv here
		$csvfnam = date("Ymd").'.'.get_uid().".csv";
		$csvfile = SMARTY_INC_DIR."/log/$csvfnam";
		$csvcont = '';

		//Loop thru all the records selected and display it in the table
		$idx = 0;
		while (OCIFetch($stmt))
		{
			//csv-create
			$csvdata   = null;
			$csvdata[] = $tran_id  ;
			$csvdata[] = $user_code;
			$csvdata[] = $role_code;
			$csvdata[] = $mod_name ; 
			$csvdata[] = $ip_addr  ; 
			$csvdata[] = $msisdn   ;
			$csvdata[] = $url      ; 
			$csvdata[] = $message  ; 
			$csvdata[] = $start_tm ; 
			$csvstr    = join('","', $csvdata);
                        
			$csvcont .= "\"$csvstr\"\n";

			$idx++;
		}

		//close csv
                $rep_gen_dt = date("Y-m-d H:i:s");
                file_save($csvfile,"List of Audit Log\nReport Generation: $rep_gen_dt\n\n".CSV_HEADER_AUDIT_REPORT."\n\n".$csvcont);

		//Free Oracle connection
		OCIFreeStatement($stmt);

		//Log-off connection		
		OCILogoff($conn);

		//event
		$retdata['csvfilename'] = $csvfnam;

		//return data
		return $retdata;
	}

	//get form fields
	function getTableFields()
	{
		global $_POST;
		global $gSelectOptionsMaster;

		$opts_module = $gSelectOptionsMaster->getModule();
		$opts_roles  = $gSelectOptionsMaster->getRoleCode();

		$table_fields= array(
				array('name' => 'q_mod_name',  'title'=>'Module Name', 'type' => 'select','data_type'=> 'string', 'value' => $_POST['q_mod_name'],  'select_options'=>$opts_module,           'select_selected' => $_POST['q_mod_name']),
				array('name' => 'q_role_code', 'title'=>'Role Code',   'type' => 'select','data_type'=> 'string', 'value' => $_POST['q_role_code'], 'select_options'=>$opts_roles,            'select_selected' => $_POST['q_role_code']),
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
