<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	CustomersMapping.class.php
#	@Description    :	encapsulator for table CUSTOMER_TYPES_MAPPING
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

define('CSV_HEADER_CUSTMAPPING_REPORT',  "Customer Type, MSISDN From, MSISDN To, Status, Created By, Date, Modified By, Date");

//class
class CustomersMapping
{

	//constructor
	function CustomersMapping()
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
		$sql     = " SELECT  id, customer_type, msisdn_fr, msisdn_to, status FROM CUSTOMER_TYPES_MAPPING WHERE ID=$id ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("Open() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, 'ID'         , $P_ID          );
		OCIDefineByName($stmt, 'CUSTOMER_TYPE', $P_CUSTOMER_TYPE );
		OCIDefineByName($stmt, 'MSISDN_FR'  , $P_MSISDN_FR   );
		OCIDefineByName($stmt, 'MSISDN_TO'  , $P_MSISDN_TO   );
		OCIDefineByName($stmt, 'STATUS'     , $P_STATUS      );

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
                        $P_ID         ;
                        $P_CUSTOMER_TYPE;
                        $P_MSISDN_FR  ;
                        $P_MSISDN_TO  ;
                        $P_STATUS     ;
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
				'customer_type' => $P_CUSTOMER_TYPE,
				'msisdn_fr'     => $P_MSISDN_FR    ,
				'msisdn_to'     => $P_MSISDN_TO    ,
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
		$ret = $this->IfExists($data['customer_type'], $data['msisdn_fr'], $data['msisdn_to'],$data['id'], $ok);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = "ERROR: Record already exists.";
			return $err;
		}

		//user
		if((strlen(trim($data['customer_type'])) <= 0) || (trim($data['customer_type']) == '%'))
		{
			$err['error'] = true;
			$err['msg']   = "ERROR: Please choose Customer Type.";
			return $err;
		}

                //msisdn_fr
		if(!@preg_match("/^639[0-9]{9}$/i", $data['msisdn_fr']) or (strlen($data['msisdn_fr']) == 0) )
                {
                        $err['error'] = true;
                        $err['msg']   = "ERROR: MSISDN_FR Should be in 639xxxxxxxxx format and 12 digit in length.";
                        return $err;
                }

                //msisdn_to
                if(!@preg_match("/^639[0-9]{9}$/i", $data['msisdn_to']) or (strlen($data['msisdn_to']) == 0) )
                {
                        $err['error'] = true;
                        $err['msg']   = "ERROR: MSISDN_TO Should be in 639xxxxxxxxx format and 12 digit in length.";
                        return $err;
                }

		//user
                if($data['msisdn_to'] < $data['msisdn_fr'])
                {
                        $err['error'] = true;
                        $err['msg']   = "ERROR: MSISDN_TO should be greater than MSISDN_FR";
                        return $err;
                }

                //user
                //$ret = $this->IfExistsStart($data['msisdn_fr']);
                //if($ret > 0)
                //{
                //        $err['error'] = true;
                //        $err['msg']   = "ERROR: Record already exists.";
                //        return $err;
                //}

		//user 
		$ret = $this->IfOverlapRange($data['msisdn_fr'], $data['msisdn_to'], $data['id']);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = "ERROR: Overlap with an existing range";
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
		$sql       = " INSERT INTO CUSTOMER_TYPES_MAPPING ($kbuff, DT_CREATED )  VALUES ($vbuff, sysdate )";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Save() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Save() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "NEW CUSTTYPE MAPPING: $mbuff ");

		return $data;
	}


	function SaveList($data=null, $grpid=null)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);

		$vgroup   = "'".ora_addslashes($data[0])."'";
		$vgsmnum1 = ora_addslashes($data[1]);
		$vgsmnum2 = ora_addslashes($data[2]);
		$vCreated_by = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"])."'";

		$is_valid=0;
                if(!@preg_match("/^[0-9]{12}$/i", trim($data[1]))) {
			$is_valid=1;
			$vReason ="Invalid MSISDN (from)";
		}
                else if(!@preg_match("/^[0-9]{12}$/i", trim($data[2]))) {
			$is_valid=2;
			$vReason ="Invalid MSISDN (to)";
		}
                else if(trim($data[1]) > trim($data[2])) {
			$is_valid=3;
			$vReason ="Invalid Range - MSISDN Fr is greater than MSISDN To";
		}
                else if($this->IfValidCustType(ora_addslashes($data[0])) == 0) {
			$is_valid=4;
			$vReason ="Invalid Customer Type";
		}
                else if($this->IfAlreadyExists(ora_addslashes($data[0]),$vgsmnum1,$vgsmnum2) >= 1) {
			$is_valid=5;
			$vReason ="Record already exists";
		}

		if ($is_valid==0) {
                	$ret = $this->IfOverlapRange($vgsmnum1, $vgsmnum2, 0);
                	if($ret <= 0)
                	{
				//new profile
				$sql       = "INSERT INTO CUSTOMER_TYPES_MAPPING (ID,CUSTOMER_TYPE,MSISDN_FR,MSISDN_TO,STATUS,CREATED_BY,DT_CREATED, BULK_GRP_ID ) ". 
				             "VALUES (customer_types_mapping_seq.nextval, $vgroup, $vgsmnum1, $vgsmnum2, 'ACTIVE', $vCreated_by, sysdate, '$grpid' ) ";
				//exect
				$data      = $myOraDb->xQuery($sql);
			}
			else {
				$vReason   = "Invalid Range - Overlap from existing range.";
				//new profile
				$sql       = "INSERT INTO CUSTOMER_TYPES_MAPPING_ERR (CUSTOMER_TYPE,MSISDN_FR,MSISDN_TO,REASON,CREATED_BY,DT_CREATED, BULK_GRP_ID ) ". 
				             "VALUES ($vgroup, '$vgsmnum1', '$vgsmnum2', '$vReason', $vCreated_by, sysdate, '$grpid' ) ";
				//exect
				$data      = $myOraDb->xQuery($sql);
			}
		}
		else {
			//new profile
			$sql       = "INSERT INTO CUSTOMER_TYPES_MAPPING_ERR (CUSTOMER_TYPE,MSISDN_FR,MSISDN_TO,REASON,CREATED_BY,DT_CREATED, BULK_GRP_ID ) ". 
			             "VALUES ($vgroup, '$vgsmnum1', '$vgsmnum2', '$vReason', $vCreated_by, sysdate, '$grpid' ) ";
			//exect
			$data      = $myOraDb->xQuery($sql);
		}

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("SaveList() :: $is_ok : $err_msg : $is_valid => $vgroup : $vgsmnum1 : $vgsmnum2");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "SaveList() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");

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
		$sql       = " UPDATE CUSTOMER_TYPES_MAPPING SET DT_UPDATED=Sysdate, $kbuff WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Update() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Update() [ res=>'$is_ok' // sql=$sql // errmsg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "UPDATE CUSTTYPE MAPPING: $mbuff ");

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
		$sql       = " DELETE FROM CUSTOMER_TYPES_MAPPING WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Delete() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Delete() [ res=>'$is_ok' // sql=$sql // errmsg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "DELETE CUSTTYPE MAPPING: ID=$id ");

		return $data;
	}

	//get uniq
	function IfExists($c1='', $c2='', $c3='', $id=0, $ok=0)
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));
		$c2        = ora_addslashes(trim($c2));
		$c3        = ora_addslashes(trim($c3));
		$id        = intval(trim($id));

		//update profile
		$sql       = ($id>0 and $ok) ? 
			" SELECT  COUNT(1) as TOTAL FROM CUSTOMER_TYPES_MAPPING WHERE CUSTOMER_TYPE='$c1' and MSISDN_FR = $c2 AND MSISDN_TO = $c3 AND $id " :
			" SELECT  COUNT(1) as TOTAL FROM CUSTOMER_TYPES_MAPPING WHERE CUSTOMER_TYPE='$c1' and MSISDN_FR = $c2 AND MSISDN_TO = $c3 ";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		

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

	//get uniq
	function IfExistsStart($c1='')
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
		$sql =	" SELECT  COUNT(1) as TOTAL FROM CUSTOMER_TYPES_MAPPING WHERE MSISDN_FR = $c1 ";

		
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		$this->debug("IfExistsStart() : // $sql // $totalrows");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "IfExists() [ $sql // $totalrows ] ");

		return $totalrows;


	}

	//get uniq
	function IfOverlapRange($c1='', $c2='', $id=0)
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
		$sql       = " SELECT SUM(TOTAL) TOTAL FROM (".
			     " SELECT COUNT(1) as TOTAL FROM CUSTOMER_TYPES_MAPPING WHERE $c1 BETWEEN MSISDN_FR AND MSISDN_TO AND ID!=$id".
			     " UNION ".
			     " SELECT COUNT(1) as TOTAL FROM CUSTOMER_TYPES_MAPPING WHERE $c2 BETWEEN MSISDN_FR AND MSISDN_TO AND ID!=$id".
			     " UNION ".
			     " SELECT COUNT(1) as TOTAL FROM CUSTOMER_TYPES_MAPPING WHERE MSISDN_FR BETWEEN $c1 AND $c2 AND ID!=$id".
			     " UNION ".
			     " SELECT COUNT(1) as TOTAL FROM CUSTOMER_TYPES_MAPPING WHERE MSISDN_TO BETWEEN $c1 AND $c2 AND ID!=$id)";
		
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"TOTAL",$totalrows);		

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit($conn);

		// logoff connection
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "IfExists() [ $sql // $totalrows ] ");

		return $totalrows;


	}


	//get form fields
	function getTableFields()
	{
		global $_POST;
		global $gSelectOptionsMaster;
		$opts_status  = $gSelectOptionsMaster->getStatus();
		$opts_custype = $gSelectOptionsMaster->getCustomerTypes();

		$table_fields= array(
				array('name' => 'customer_type'  ,'title'=>'Customer Type',   'type' => 'select',  'max'=> 60,   'data_type'=> 'string', 'value' => $_POST['customer_type'], 'select_options'=>$opts_custype, 'select_selected' => $_POST['customer_type'] ),
				array('name' => 'msisdn_fr'    ,'title'=>'MSISDN From',   'type' => 'text',    'max'=> 16,   'data_type'=> 'string', 'value' => $_POST['msisdn_fr'  ] ),
				array('name' => 'msisdn_to'    ,'title'=>'MSISDN To',     'type' => 'text',    'max'=> 16,   'data_type'=> 'string', 'value' => $_POST['msisdn_to'  ] ),
				array('name' => 'status'       ,'title'=>'Status',        'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $_POST['status'     ], 'select_options'=>$opts_status, 'select_selected' => $_POST['status'] ),
				);

		return $table_fields;

	}


	//get form fields
	function getTableFields4Update($id="")
	{
		global $gSelectOptionsMaster;

		$data         = $this->Get($id);			
		$post         = $data['bind_data'];
		$opts_status  = $gSelectOptionsMaster->getStatus();
		$opts_custype = $gSelectOptionsMaster->getCustomerTypes();

		$table_fields= array(
				array('name' => 'customer_type','title'=>'Customer Type', 'type' => 'select',  'max'=> 60,   'data_type'=> 'string', 'value' => $post['customer_type'], 'select_options'=>$opts_custype, 'select_selected' => $post['customer_type'] ),
				array('name' => 'msisdn_fr'    ,'title'=>'MSISDN From',   'type' => 'text',    'max'=> 16,   'data_type'=> 'string', 'value' => $post['msisdn_fr'  ] ),
				array('name' => 'msisdn_to'    ,'title'=>'MSISDN To',     'type' => 'text',    'max'=> 16,   'data_type'=> 'string', 'value' => $post['msisdn_to'  ] ),
				array('name' => 'status'       ,'title'=>'Status',        'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $post['status'     ], 'select_options'=>$opts_status, 'select_selected' => $post['status'] ),
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
		$temp_code      = $_GET['scode'];
		$temp_desc      = $_GET['sname_l'];
		$temp_stat      = $_GET['sstat'];
		$temp_file      = $_GET['sfile'];
		$temp_sort      = $_GET['sortedby'];
		$xwherelog      = "";
		$temp_sdall     = 0;

		if (strlen($temp_code) > 0) { 
			if (strlen($temp_code) >= 12)
				$xwhere .= " AND $temp_code between msisdn_fr and msisdn_to "; 
                        else
				$xwhere .= " AND msisdn_fr >= rpad($temp_code, 12, 0) AND msisdn_fr <= rpad($temp_code, 12, 9) "; 
			$xwherelog .= " MSISDN=$temp_code "; 
			$temp_sdall = 1;
			$filters++; 
		}
		if ((strlen($temp_desc) > 0) && ($temp_desc != '%')) { 
			$xwhere .= " AND customer_type like '$temp_desc%' "; 
			$xwherelog .= " Customer Type=$temp_desc "; 
			$temp_sdall = 1;
			$filters++; 
		}

		if ((strlen($temp_stat) > 0)  && ($temp_stat != 'ALL')) { 
			$xwhere .= " AND status like '$temp_stat' "; 
			$xwherelog .= " Status=$temp_stat "; 
			$temp_sdall = 1;
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
		$sql  = " SELECT COUNT(1) AS COUNT FROM CUSTOMER_TYPES_MAPPING $xwhere";

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
		//$gEventLogger->LogEvent(__CLASS__, "getCustomersMappingList : [WHERE=>'$xwhere'] [ TOTAL=>'$totalrows'] ");

		//chk rowcount
		if(0 == $totalrows )     
		{                   
                        $retdata['scode']     = $temp_code ;
                        $retdata['sname']     = $temp_desc ;
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
				case 'msisdn_fr':
					$orderby = " ORDER BY  msisdn_fr $ascdesc";
					break;
				case 'msisdn_to':
					$orderby = " ORDER BY  msisdn_to $ascdesc";
					break;
				case 'customer_type':
					$orderby = " ORDER BY  customer_type $ascdesc";
					break;
				case 'status':
					$orderby = " ORDER BY  status $ascdesc";
					break;
				default:	
					$orderby = " ORDER BY id $ascdesc";
					break;
			}
                	
			//select values for dynamic SQL passing
			$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, customer_type, msisdn_fr, msisdn_to, status FROM CUSTOMER_TYPES_MAPPING ".
				"  $xwhere $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";
                	
			//Parse SQL passed
			$stmt = OCIParse($conn,$sql);
                	
			$this->debug("getCustomersMappingList() : get_all_rows[$sql / $stmt / $totalrows]");
                	
                	
			//bind
			OCIDefineByName($stmt, 'ID'           , $p_id            );
			OCIDefineByName($stmt, 'MROW'         , $p_mrow          );
			OCIDefineByName($stmt, 'CUSTOMER_TYPE', $p_customer_type );
			OCIDefineByName($stmt, 'MSISDN_FR'    , $p_msisdn_fr     );
			OCIDefineByName($stmt, 'MSISDN_TO'    , $p_msisdn_to     );
			OCIDefineByName($stmt, 'STATUS'       , $p_status        );
                	
			//Execute SQL
			OCIExecute($stmt);
                	
			//Loop thru all the records selected and display it in the table
			$idx = 0;
			while (OCIFetch($stmt))
			{
				$alldata[$idx]['id'           ]    = $p_id            ;
				$alldata[$idx]['mrow'         ]    = $p_mrow          ;
				$alldata[$idx]['customer_type']    = $p_customer_type ;
				$alldata[$idx]['msisdn_fr'    ]    = $p_msisdn_fr     ;
				$alldata[$idx]['msisdn_to'    ]    = $p_msisdn_to     ;
				$alldata[$idx]['status'       ]    = $p_status        ;
                	
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
                $retdata['sname']     = $temp_desc ;
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
                        $retdata['sdall']     = $temp_sdall ;
		}				 

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getCustomersMappingList(#b) : [ total=>'$totalrows'] ");

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

		$retdata['csvfilename'] = null;

		$filters=0;
		$temp_code      = $_GET['scode'];
		$temp_desc      = $_GET['sname'];
		$temp_stat      = $_GET['sstat'];

		if (strlen($temp_code) > 0) { 
			if (strlen($temp_code) >= 12)
				$xwhere .= " AND $temp_code between msisdn_fr and msisdn_to "; 
                        else
				$xwhere .= " AND msisdn_fr >= rpad($temp_code, 12, 0) AND msisdn_fr <= rpad($temp_code, 12, 9) "; 
			$filters++; 
		}

		if ((strlen($temp_desc) > 0) && ($temp_desc != '%')) { 
			$xwhere .= " AND customer_type like '$temp_desc%' "; 
			$filters++; 
		}

		if ((strlen($temp_stat) > 0)  && ($temp_stat != 'ALL')) { 
			$xwhere .= " AND status like '$temp_stat' "; 
			$filters++; 
		}

		if ($filters > 0) {
			$xwhere = " where 1 =1 ". $xwhere;
		}
	
		//type of order
		$ascdesc     = " ASC ";	

		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, customer_type, msisdn_fr, msisdn_to, status, created_by, ".
			"   to_char(dt_created, 'YYYY-MM-DD HH24:MI:SS') dt_created, updated_by modified_by, to_char(dt_updated, 'YYYY-MM-DD HH24:MI:SS') dt_modified ". 
			"  FROM CUSTOMER_TYPES_MAPPING ".
			"  $xwhere $orderby ) b ) ";

		$this->debug("downloadList() : $sql ");
		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//bind
		OCIDefineByName($stmt, 'ID'           , $p_id            );
		OCIDefineByName($stmt, 'MROW'         , $p_mrow          );
		OCIDefineByName($stmt, 'CUSTOMER_TYPE', $p_customer_type );
		OCIDefineByName($stmt, 'MSISDN_FR'    , $p_msisdn_fr     );
		OCIDefineByName($stmt, 'MSISDN_TO'    , $p_msisdn_to     );
		OCIDefineByName($stmt, 'STATUS'       , $p_status        );
		OCIDefineByName($stmt, 'CREATED_BY'   , $p_created_by    );
		OCIDefineByName($stmt, 'DT_CREATED'   , $p_dt_created    );
		OCIDefineByName($stmt, 'MODIFIED_BY'  , $p_modified_by   );
		OCIDefineByName($stmt, 'DT_MODIFIED'  , $p_dt_modified   );

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
			$csvdata[] = $p_customer_type;
			$csvdata[] = $p_msisdn_fr;
			$csvdata[] = $p_msisdn_to;
			$csvdata[] = $p_status;
			$csvdata[] = $p_created_by;
			$csvdata[] = $p_dt_created;
			$csvdata[] = $p_modified_by;
			$csvdata[] = $p_dt_modified;
			$csvstr    = join('","', $csvdata);
			$csvcont .= "\"$csvstr\"\n";
			$idx++;
		}

		//close csv
                $rep_gen_dt = date("Y-m-d H:i:s");
                file_save($csvfile,"List of Customer Types Mapping\nReport Generation: $rep_gen_dt\n\n".CSV_HEADER_CUSTMAPPING_REPORT."\n\n".$csvcont);

		//Free Oracle connection
		OCIFreeStatement($stmt);

		//Log-off connection		
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Download Customer Type Mapping.");

		$retdata['csvfilename'] = $csvfnam;

		//return data
		return $retdata;
	}


	//list
	function getBulkList($extravars=null,$grpid=null)
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
		$xwhere  = " WHERE BULK_GRP_ID = '$grpid'";
		$xwherelog  = " BULK_GRP_ID = '$grpid'";

		//fmt
		$whatsort    = $extravars['whatsort'];
		//type of order
		$ascdesc     = (!$whatsort) ? (" ASC ") : ( " DESC " );	
		$ascdesc     = " ASC ";	


		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT COUNT(1) AS COUNT FROM CUSTOMER_TYPES_MAPPING $xwhere";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"COUNT",$totalrows);		

		$this->debug("BulkUpload() : get_max_row[$sql / $stmt / $totalrows]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);

		// logoff connection
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getCustomersMappingList : [WHERE=>'$xwhere'] [ TOTAL=>'$totalrows'] ");

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
				'totalItems'   => 1,
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
		$gEventLogger->LogEvent(__CLASS__, "BulkUpload: TOTAL Rows=$totalrows PAGE=$nPageNo/$nPageTo");


		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//orderby
		$sortedby       = $extravars['sortedby'];
		switch($sortedby)
		{
			case 'msisdn_fr':
				$orderby = " ORDER BY  msisdn_fr $ascdesc";
				break;
			case 'customer_type':
				$orderby = " ORDER BY  customer_type $ascdesc";
				break;
			case 'status':
				$orderby = " ORDER BY  status $ascdesc";
				break;
			default:	
				$orderby = " ORDER BY id $ascdesc";
				break;
		}
		$orderby = " ORDER BY  msisdn_fr asc";

		//select values for dynamic SQL passing
		//$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, customer_type, to_char(msisdn_fr) msisdn_fr, to_char(msisdn_to) msisdn_to, status FROM CUSTOMER_TYPES_MAPPING $xwhere ".
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT 0 id, customer_type, msisdn_fr, msisdn_to, reason status FROM CUSTOMER_TYPES_MAPPING_ERR $xwhere ".
			"  $orderby ) b )"; 
		// WHERE MROW >=$slimit AND MROW < $elimit";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("getCustomersMappingList() : get_all_rows[$sql / $stmt / $totalrows]");


		//bind
		OCIDefineByName($stmt, 'ID'         , $p_id          );
		OCIDefineByName($stmt, 'CUSTOMER_TYPE', $p_customer_type );
		OCIDefineByName($stmt, 'MSISDN_FR'  , $p_msisdn_fr   );
		OCIDefineByName($stmt, 'MSISDN_TO'  , $p_msisdn_to   );
		OCIDefineByName($stmt, 'STATUS'     , $p_status      );


		//Execute SQL
		OCIExecute($stmt);

		//Loop thru all the records selected and display it in the table
		$idx = 0;
		while (OCIFetch($stmt))
		{
			$alldata[$idx]['id'         ]    = $p_id          ;
			$alldata[$idx]['customer_type']  = $p_customer_type ;
			$alldata[$idx]['msisdn_fr'  ]    = $p_msisdn_fr   ;
			$alldata[$idx]['msisdn_to'  ]    = $p_msisdn_to   ;
			$alldata[$idx]['status'     ]    = $p_status      ;
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
		//$gEventLogger->LogEvent(__CLASS__, "getCustomersMappingList(#b) : [ total=>'$totalrows'] ");

		//return data
		return $retdata;
	}


	//check if already roamer
	function IfValidCustType($c1='')
	{
		//globals here
		global $gEventLogger;
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();
		$c1        = ora_addslashes(trim($c1));
		$sql       = " SELECT COUNT(1) as TOTAL FROM customer_types WHERE customer_type = '$c1' ";
		$stmt = OCIParse($conn,$sql);
		$totalrows = 0;	
		OCIDefineByName($stmt,"TOTAL",$totalrows);		
		OCIExecute($stmt);
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);
		//$this->debug("IfValidCustType() : get_all_rows[$sql / $totalrows]");
		OCIFreeStatement($stmt);
		OCICommit($conn);
		OCILogoff($conn);
		return $totalrows;
	}

	//check if already roamer
	function IfAlreadyExists($c1='',$c2='',$c3='')
	{
		//globals here
		global $gEventLogger;
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();
		$c1        = ora_addslashes(trim($c1));
		$sql       = " SELECT COUNT(1) as TOTAL FROM customer_types_mapping WHERE customer_type = '$c1' and msisdn_fr = $c2 and msisdn_to = $c3";
		$stmt = OCIParse($conn,$sql);
		$totalrows = 0;	
		OCIDefineByName($stmt,"TOTAL",$totalrows);		
		OCIExecute($stmt);
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);
		//$this->debug("IfAlreadyExists() : get_all_rows[$sql / $totalrows]");
		OCIFreeStatement($stmt);
		OCICommit($conn);
		OCILogoff($conn);
		return $totalrows;
	}


	//1-only
	function &GetInstance()
	{
		static $instance;

		if(empty($instance) or !$instance or !defined($instance))
		{
			$instance   = new CustomersMapping();
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
