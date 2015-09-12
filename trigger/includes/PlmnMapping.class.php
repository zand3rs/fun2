<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	PlmnMapping.class.php
#	@Description:	encapsulator for table PLMN_MAPPING
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
define('CSV_HEADER_PLMNMAPPING_REPORT',  "COUNTRY_CODE, CARRIER, ADDR, HOT?, STATUS, CREATED BY, DATE, MODIFIED BY, DATE");

//class
class PlmnMapping
{

	//constructor
	function PlmnMapping()
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
		$sql     = " SELECT id, country_code, carrier, addr, decode(hot,0,'No','Yes') hot, status FROM PLMN_MAPPING WHERE ID=$id ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("Open() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt,'ID'           , $P_ID           );
		OCIDefineByName($stmt,'COUNTRY_CODE' , $P_COUNTRY_CODE );
		OCIDefineByName($stmt,'CARRIER'      , $P_CARRIER      );
		OCIDefineByName($stmt,'ADDR'         , $P_ADDR         );
		OCIDefineByName($stmt,'HOT'          , $P_HOT          );
		OCIDefineByName($stmt,'STATUS'       , $P_STATUS       );

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
			$P_ID          ;
			$P_COUNTRY_CODE;
			$P_CARRIER     ;
			$P_ADDR        ;
			$P_HOT         ;
			$P_STATUS      ;
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Get() : [ $sql // $totalrows // $is_error / $err_msg] ");

		$alldata = array(
				'error'		=> $is_error, 
				'error_msg' 	=> $err_msg,
				'bind_total'	=> $totalrows,
				'bind_data'	=> array(
				'id'            => $P_ID           ,
				'country_code'  => $P_COUNTRY_CODE ,
				'carrier'       => $P_CARRIER      ,
				'addr'          => $P_ADDR         ,
				'hot'           => $P_HOT          ,
				'status'        => $P_STATUS       ,
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
		$ret = $this->IfExists($data['addr'], $data['id'], $ok);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_RECORD_ALREADY_EXISTS;
			return $err;
		}

		//usr
		if((strlen(trim($data['country_code'])) <= 0) || (trim($data['country_code']) == 'ALL') )
		{
			$err['error'] = true;
			$err['msg']   = INVALID_COUNTRY_CODE;
			return $err;
		}

		//usr
		if(strlen(trim($data['addr'])) <= 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_ADDRESS;
			return $err;
		}

		//usr
		if(strlen(trim($data['carrier'])) <= 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_CARRIER;
			return $err;
		}
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
		$sql       = " INSERT INTO PLMN_MAPPING ($kbuff, DT_CREATED )  VALUES ($vbuff, sysdate )";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Save() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Save() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "NEW PLMN Mapping: $mbuff");

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
		$sql       = " UPDATE PLMN_MAPPING SET DT_UPDATED=Sysdate, $kbuff WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Update() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Update() [ res=>'$is_ok' // sql=$sql // errmsg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "UPDATE PLMN Mapping: $mbuff");

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
		$sql       = " DELETE FROM PLMN_MAPPING WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Delete() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Delete() [ res=>'$is_ok' // sql=$sql // errmsg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "DELETE PLMN Mapping: ID=$id");

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
		$c2        = ora_addslashes(trim($c2));
		$id        = intval(trim($id));

		//update profile
		$sql       = ($id>0 and $ok) ? 
			" SELECT  COUNT(1) as TOTAL FROM PLMN_MAPPING WHERE ADDR='$c1' AND ID!=$id " :
			" SELECT  COUNT(1) as TOTAL FROM PLMN_MAPPING WHERE ADDR='$c1' ";


		
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
		$opts_hot    = $gSelectOptionsMaster->getHot();
		$opts_country= $gSelectOptionsMaster->getCountries();

		$table_fields= array(
				array('name' => 'country_code',   'title'=>'Country Code',  'type' => 'select',  'max'=> 30,   'data_type'=> 'string', 'value' => $_POST['country_code'], 'select_options'=>$opts_country, 'select_selected' => $_POST['country_code']  ),
				array('name' => 'carrier',        'title'=>'Carrier',       'type' => 'text',    'max'=> 30,   'data_type'=> 'string', 'value' => $_POST['carrier'] ),
				array('name' => 'addr',           'title'=>'Address',       'type' => 'text',    'max'=> 30,   'data_type'=> 'string', 'value' => $_POST['addr'] ),
				array('name' => 'hot' ,           'title'=>'Hot',           'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $_POST['hot'], 'select_options'=>$opts_hot, 'select_selected' => $_POST['hot'] ),
				array('name' => 'status' ,        'title'=>'Status',        'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $_POST['status'], 'select_options'=>$opts_status, 'select_selected' => $_POST['status'] ),
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
		$opts_hot    = $gSelectOptionsMaster->getHot();
		$opts_country= $gSelectOptionsMaster->getCountries();

		$table_fields= array(
				array('name' => 'country_code',   'title'=>'Country Code',  'type' => 'select',  'max'=> 30,   'data_type'=> 'string', 'value' => $post['country_code'], 'select_options'=>$opts_country, 'select_selected' => $post['country_code']  ),
				array('name' => 'carrier',        'title'=>'Carrier',       'type' => 'text',    'max'=> 30,   'data_type'=> 'string', 'value' => $post['carrier'] ),
				array('name' => 'addr',           'title'=>'Address',       'type' => 'text',    'max'=> 30,   'data_type'=> 'string', 'value' => $post['addr'] ),
				array('name' => 'hot' ,           'title'=>'Hot',           'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $post['hot'], 'select_options'=>$opts_hot, 'select_selected' => $post['hot'] ),
				array('name' => 'status' ,        'title'=>'Status',        'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $post['status'], 'select_options'=>$opts_status, 'select_selected' => $post['status'] ),
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
		$xwherelog = "";
		$temp_code  = $_GET['scode_l'];
		$temp_name  = $_GET['sname'];
		$temp_zone  = $_GET['shot'];
		$temp_stat  = $_GET['sstat'];
		$temp_file  = $_GET['sfile'];
		$temp_sort  = $_GET['sortedby'];

		if ((strlen($temp_code) > 0) && ($temp_code != ' ALL')) { 
			$xwhere .= " AND country_code like '$temp_code%' "; 
			$xwherelog .= " Country=$temp_code";
			$filters++; 
		}

		if (strlen($temp_name) > 0) { 
			$xwhere .= " AND carrier like '$temp_name%' "; 
			$xwherelog .= " Carrier=$temp_name";
			$filters++; 
		}

		if ((strlen($temp_zone) > 0)  && ($temp_zone != 'ALL')) { 
			$xwhere .= " AND hot like '$temp_zone%' "; 
			$xwherelog .= " Zone=$temp_zone";
			$filters++; 
		}

		if ((strlen($temp_stat) > 0)  && ($temp_stat != 'ALL')) { 
			$xwhere .= " AND status like '$temp_stat%' "; 
			$xwherelog .= " Status=$temp_stat";
			$filters++; 
		}

		if ($filters > 0) {
			$xwhere = " where 1 =1 ". $xwhere;
		}
		else {
			$xwherelog .= " All";
		}

	
		//fmt
		$whatsort    = $extravars['whatsort'];
		//type of order
		$ascdesc     = (!$whatsort) ? (" ASC ") : ( " DESC " );	

		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT COUNT(1) AS COUNT FROM PLMN_MAPPING $xwhere";

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
		//$gEventLogger->LogEvent(__CLASS__, "getPlmnMappingList : [WHERE=>'$xwhere'] [ TOTAL=>'$totalrows'] ");

		//chk rowcount
		if(0 == $totalrows )     
		{                   
			$retdata['scode']     = $temp_code ;
			$retdata['sname']     = $temp_name ;
			$retdata['shot' ]     = $temp_zone ;
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
			$gEventLogger->LogEvent(__CLASS__, "SEARCHED By: $xwherelog TOTAL Rows=$totalrows PAGE=$nPageNo/$nPageTo");
                	
			//logon to oracle database
			$conn = $myOraDb->dbConnection();
                	
			//orderby
			//$sortedby       = $extravars['sortedby'];
			$sortedby       = $temp_sort;
			switch($sortedby)
			{
				case 'country_code':
					$orderby = " ORDER BY  country_code $ascdesc";
					break;
				case 'carrier':
					$orderby = " ORDER BY  carrier $ascdesc";
					break;
				case 'addr':
					$orderby = " ORDER BY  addr $ascdesc";
					break;
				case 'hot':
					$orderby = " ORDER BY  hot $ascdesc";
					break;
				case 'status':
					$orderby = " ORDER BY  status $ascdesc";
					break;
				default:	
					$orderby = " ORDER BY id $ascdesc";
					break;
			}
                	
			//select values for dynamic SQL passing
			$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, country_code, carrier, addr, decode(hot,0,'No','Yes') hot, status FROM PLMN_MAPPING ".
				"  $xwhere $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";
                	
			//Parse SQL passed
			$stmt = OCIParse($conn,$sql);
                	
			$this->debug("getPlmnMappingList() : get_all_rows[$sql / $stmt / $totalrows]");
                	
                	
			//bind
			OCIDefineByName($stmt,'ID'           , $p_id           );
			OCIDefineByName($stmt,'COUNTRY_CODE' , $p_country_code );
			OCIDefineByName($stmt,'CARRIER'      , $p_carrier      );
			OCIDefineByName($stmt,'ADDR'         , $p_addr         );
			OCIDefineByName($stmt,'HOT'          , $p_hot          );
			OCIDefineByName($stmt,'STATUS'       , $p_status       );
                	
			//Execute SQL
			OCIExecute($stmt);
                	
			//Loop thru all the records selected and display it in the table
			$idx = 0;
			while (OCIFetch($stmt))
			{
				$alldata[$idx]['id'          ]    = $p_id          ;
				$alldata[$idx]['country_code']    = $p_country_code;
				$alldata[$idx]['carrier'     ]    = $p_carrier     ;
				$alldata[$idx]['addr'        ]    = $p_addr        ;
				$alldata[$idx]['hot'         ]    = $p_hot         ;
				$alldata[$idx]['status'      ]    = $p_status      ;
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
		$retdata['sname']     = $temp_name ;
		$retdata['shot' ]     = $temp_zone ;
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
		//$gEventLogger->LogEvent(__CLASS__, "getPlmnMappingList(#b) : [ total=>'$totalrows'] ");

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
		$filters=0;
		$temp_code  = $_GET['scode'];
		$temp_name  = $_GET['sname'];
		$temp_zone  = $_GET['shot'];
		$temp_stat  = $_GET['sstat'];

		if ((strlen($temp_code) > 0) && ($temp_code != ' ALL')) { 
			$xwhere .= " AND country_code like '$temp_code%' "; 
			$filters++; 
		}

		if (strlen($temp_name) > 0) { 
			$xwhere .= " AND carrier like '$temp_name%' "; 
			$filters++; 
		}

		if ((strlen($temp_zone) > 0)  && ($temp_zone != 'ALL')) { 
			$xwhere .= " AND hot like '$temp_zone%' "; 
			$filters++; 
		}

		if ((strlen($temp_stat) > 0)  && ($temp_stat != 'ALL')) { 
			$xwhere .= " AND status like '$temp_stat%' "; 
			$filters++; 
		}

		if ($filters > 0) {
			$xwhere = " where 1 =1 ". $xwhere;
		}
	
		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//orderby
		$ascdesc = " ASC ";	
		$orderby = " ORDER BY  country_code $ascdesc";

		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, country_code, carrier, addr, decode(hot,0,'No','Yes') hot, status, created_by, ".
			"   to_char(dt_created, 'YYYY-MM-DD HH24:MI:SS') dt_created, updated_by modified_by, to_char(dt_updated, 'YYYY-MM-DD HH24:MI:SS') dt_modified ". 
			"  FROM PLMN_MAPPING ".
			"  $xwhere $orderby ) b ) ";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("getPlmnMappingList() : get_all_rows[$sql / $stmt / $totalrows]");


		//bind
		OCIDefineByName($stmt, 'ID'           , $p_id           );
		OCIDefineByName($stmt, 'COUNTRY_CODE' , $p_country_code );
		OCIDefineByName($stmt, 'CARRIER'      , $p_carrier      );
		OCIDefineByName($stmt, 'ADDR'         , $p_addr         );
		OCIDefineByName($stmt, 'HOT'          , $p_hot          );
		OCIDefineByName($stmt, 'STATUS'       , $p_status       );
		OCIDefineByName($stmt, 'CREATED_BY'   , $p_created_by   );
		OCIDefineByName($stmt, 'DT_CREATED'   , $p_dt_created   );
		OCIDefineByName($stmt, 'MODIFIED_BY'  , $p_modified_by  );
		OCIDefineByName($stmt, 'DT_MODIFIED'  , $p_dt_modified  );

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
			$csvdata[] = $p_country_code ; 
			$csvdata[] = $p_carrier      ; 
			$csvdata[] = $p_addr         ; 
			$csvdata[] = $p_hot          ; 
			$csvdata[] = $p_status       ; 
			$csvdata[] = $p_created_by   ; 
			$csvdata[] = $p_dt_created   ; 
			$csvdata[] = $p_modified_by  ;
			$csvdata[] = $p_dt_modified  ;
			$csvstr    = join('","', $csvdata);
			$csvcont .= "\"$csvstr\"\n";
			$idx++;
		}

		//close csv
                $rep_gen_dt = date("Y-m-d H:i:s");
                file_save($csvfile,"List of PLMN Mapping\nReport Generation: $rep_gen_dt\n\n".CSV_HEADER_PLMNMAPPING_REPORT."\n\n".$csvcont);
	
		//Free Oracle connection
		OCIFreeStatement($stmt);

		//Log-off connection		
		OCILogoff($conn);

		//event
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
			$instance   = new PlmnMapping();
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
