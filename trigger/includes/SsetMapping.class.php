<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	SsetMapping.class.php
#	@Description    :	encapsulator for table SSET_MAPPING
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
define('CSV_HEADER_SSETMAPPING_REPORT',  "SSET_ID, IRSSET, NORMALSSET, MSISDN_FR, MSISDN_TO, STATUS, CREATED BY, DATE, MODIFIED BY, DATE");

//class
class SsetMapping
{

	//constructor
	function SsetMapping()
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
		$sql     = " SELECT  id, sset_id, irsset, normalsset, msisdn_fr, msisdn_to, status FROM SSET_MAPPING WHERE ID=$id ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("Open() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, 'ID'         , $P_ID          );
		OCIDefineByName($stmt, 'SSET_ID'    , $P_SSET_ID     );
		OCIDefineByName($stmt, 'IRSSET'     , $P_IRSSET      );
		OCIDefineByName($stmt, 'NORMALSSET' , $P_NORMALSSET  );
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
			$P_SSET_ID    ;
			$P_IRSSET     ;
                        $P_NORMALSSET ;
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
				'id'            => $P_ID         ,
                        	'sset_id'       => $P_SSET_ID    ,
                        	'irsset'        => $P_IRSSET     ,
                        	'normalsset'    => $P_NORMALSSET ,
				'msisdn_fr'     => $P_MSISDN_FR  ,
				'msisdn_to'     => $P_MSISDN_TO  ,
				'status'        => $P_STATUS     ,
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
		$ret = $this->IfExists($data['msisdn_fr'], $data['msisdn_to'], $data['id'], $ok);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = "ERROR: Record already exists.";
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

                //usr
                //if(strlen(trim($data['sset_id'])) <= 0)
                //{
                //        $err['error'] = true;
                //        $err['msg']   = "ERROR: Please enter SSet ID";
                //        return $err;
                //}

                //usr
                if(strlen(trim($data['irsset'])) <= 0)
                {
                        $err['error'] = true;
                        $err['msg']   = "ERROR: Please enter IR Set";
                        return $err;
                }

                //usr
                if(strlen(trim($data['normalsset'])) <= 0)
                {
                        $err['error'] = true;
                        $err['msg']   = "ERROR: Please enter Normal Set";
                        return $err;
                }

		//user 
		$ret = $this->IfOverlapRange($data['msisdn_fr'], $data['msisdn_to'], $data['id']);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = "ERROR: Overlaps with an existing range";
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
		$sql       = " INSERT INTO SSET_MAPPING ($kbuff, DT_CREATED )  VALUES ($vbuff, sysdate )";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Save() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Save() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "NEW SSET Mapping: $mbuff");

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
		$sql       = " UPDATE SSET_MAPPING SET DT_UPDATED=Sysdate, $kbuff WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Update() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Update() [ res=>'$is_ok' // sql=$sql // errmsg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "UPDATE SSET Mapping: $mbuff");

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
		$sql       = " DELETE FROM SSET_MAPPING WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Delete() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Delete() [ res=>'$is_ok' // sql=$sql // errmsg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "DELETE SSET Mapping: ID=$id");

		return $data;
	}


	//get uniq
	function IfExists($c1='', $c2='', $id=0, $ok=0)
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
			" SELECT  COUNT(1) as TOTAL FROM SSET_MAPPING WHERE MSISDN_FR = $c1 AND MSISDN_TO = $c2 AND ID!=$id " :
			" SELECT  COUNT(1) as TOTAL FROM SSET_MAPPING WHERE MSISDN_FR = $c1 AND MSISDN_TO = $c2";


		
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
			     " SELECT COUNT(1) as TOTAL FROM SSET_MAPPING WHERE $c1 BETWEEN MSISDN_FR AND MSISDN_TO AND ID!=$id".
			     " UNION ".
			     " SELECT COUNT(1) as TOTAL FROM SSET_MAPPING WHERE $c2 BETWEEN MSISDN_FR AND MSISDN_TO AND ID!=$id".
			     " UNION ".
			     " SELECT COUNT(1) as TOTAL FROM SSET_MAPPING WHERE MSISDN_FR BETWEEN $c1 AND $c2 AND ID!=$id".
			     " UNION ".
			     " SELECT COUNT(1) as TOTAL FROM SSET_MAPPING WHERE MSISDN_TO BETWEEN $c1 AND $c2 AND ID!=$id)";
		
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
		$opts_status  = $gSelectOptionsMaster->getStatus();

		//array('name' => 'sset_id'      ,'title'=>'SSET ID',       'type' => 'text',    'max'=> 12,   'data_type'=> 'string', 'value' => $_POST['sset_id'    ] ),
		$table_fields= array(
				array('name' => 'irsset'       ,'title'=>'IR SSET',       'type' => 'text',    'max'=> 12,   'data_type'=> 'string', 'value' => $_POST['irsset'     ] ),
				array('name' => 'normalsset'   ,'title'=>'Normal SSET',   'type' => 'text',    'max'=> 12,   'data_type'=> 'string', 'value' => $_POST['normalsset' ] ),
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

		//array('name' => 'sset_id'      ,'title'=>'SSET ID',       'type' => 'text',    'max'=> 12,   'data_type'=> 'string', 'value' => $post['sset_id'    ] ),
		$table_fields= array(
				array('name' => 'irsset'       ,'title'=>'IR SSET',       'type' => 'text',    'max'=> 12,   'data_type'=> 'string', 'value' => $post['irsset'     ] ),
				array('name' => 'normalsset'   ,'title'=>'Normal SSET',   'type' => 'text',    'max'=> 12,   'data_type'=> 'string', 'value' => $post['normalsset' ] ),
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
		$xwherelog = "";
		$temp_code  = $_GET['scode'];
		$temp_desc  = $_GET['sname'];
		$temp_desc1 = $_GET['sname1'];
		$temp_desc2 = $_GET['sname2'];
		$temp_stat  = $_GET['sstat'];
		$temp_file  = $_GET['sfile'];
		$temp_sort  = $_GET['sortedby'];

		if (strlen($temp_code) > 0) { 
			if (strlen($temp_code) >= 12)
				$xwhere .= " AND $temp_code between msisdn_fr and msisdn_to "; 
                        else
				$xwhere .= " AND msisdn_fr >= rpad($temp_code, 12, 0) AND msisdn_fr <= rpad($temp_code, 12, 9) "; 
			$xwherelog .= " MSISDN=$temp_code";
			$filters++; 
		}

		if ((strlen($temp_desc) > 0))   { 
			$xwhere .= " AND sset_id like rtrim(ltrim('$temp_desc%')) "; 
			$xwherelog .= " SSET ID=$temp_desc";
			$filters++; 
		}

		if ((strlen($temp_desc1) > 0))   { 
			$xwhere .= " AND irsset like rtrim(ltrim('$temp_desc1%')) "; 
			$xwherelog .= " IR SET=$temp_desc";
			$filters++; 
		}

		if ((strlen($temp_desc2) > 0))   { 
			$xwhere .= " AND normalsset like rtrim(ltrim('$temp_desc2%')) "; 
			$xwherelog .= " IR SET=$temp_desc";
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
		$sql  = " SELECT COUNT(1) AS COUNT FROM SSET_MAPPING $xwhere";

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
		//$gEventLogger->LogEvent(__CLASS__, "getSsetMappingList : [WHERE=>'$xwhere'] [ TOTAL=>'$totalrows'] ");

		//chk rowcount
		if(0 == $totalrows )     
		{                   
                	$retdata['scode']     = $temp_code ;
                	$retdata['sname']     = $temp_desc ;
                	$retdata['sname1']    = $temp_desc1;
                	$retdata['sname2']    = $temp_desc2;
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
				case 'msisdn_fr':
					$orderby = " ORDER BY  msisdn_fr $ascdesc";
					break;
				case 'msisdn_to':
					$orderby = " ORDER BY  msisdn_to $ascdesc";
					break;
				case 'sset_id':
					$orderby = " ORDER BY  sset_id $ascdesc";
					break;
				case 'irsset':
					$orderby = " ORDER BY  irsset $ascdesc";
					break;
				case 'normalsset':
					$orderby = " ORDER BY  normalsset $ascdesc";
					break;
				case 'status':
					$orderby = " ORDER BY  status $ascdesc";
					break;
				default:	
					$orderby = " ORDER BY id $ascdesc";
					break;
			}
                	
			//select values for dynamic SQL passing
			$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, sset_id, irsset, normalsset, msisdn_fr, msisdn_to, status FROM SSET_MAPPING ".
				"  $xwhere $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";
                	
			//Parse SQL passed
			$stmt = OCIParse($conn,$sql);
                	
			$this->debug("getSsetMappingList() : get_all_rows[$sql / $stmt / $totalrows]");
                	
                	
			//bind
                	OCIDefineByName($stmt, 'ID'         , $p_id          );
                	OCIDefineByName($stmt, 'SSET_ID'    , $p_sset_id     );
                	OCIDefineByName($stmt, 'IRSSET'     , $p_irsset      );
                	OCIDefineByName($stmt, 'NORMALSSET' , $p_normalsset  );
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
				$alldata[$idx]['sset_id'    ]    = $p_sset_id     ;
				$alldata[$idx]['irsset'     ]    = $p_irsset      ;
				$alldata[$idx]['normalsset' ]    = $p_normalsset  ;
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
		}

		$retdata['total']     = $totalrows ;
                $retdata['scode']     = $temp_code ;
                $retdata['sname']     = $temp_desc ;
                $retdata['sname1']    = $temp_desc1;
                $retdata['sname2']    = $temp_desc2;
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
		//$gEventLogger->LogEvent(__CLASS__, "getSsetMappingList(#b) : [ total=>'$totalrows'] ");

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
		$temp_code      = $_GET['scode'];
		$temp_desc      = $_GET['sname'];
		$temp_desc1     = $_GET['sname1'];
		$temp_desc2     = $_GET['sname2'];
		$temp_stat      = $_GET['sstat'];

		if (strlen($temp_code) > 0) { 
			if (strlen($temp_code) >= 12)
				$xwhere .= " AND $temp_code between msisdn_fr and msisdn_to "; 
                        else
				$xwhere .= " AND msisdn_fr >= rpad($temp_code, 12, 0) AND msisdn_fr <= rpad($temp_code, 12, 9) "; 
			$filters++; 
		}

		if ((strlen($temp_desc) > 0))   { 
			$xwhere .= " AND sset_id like rtrim(ltrim('$temp_desc%')) "; 
			$xwherelog .= " SSET ID=$temp_desc";
			$filters++; 
		}

		if ((strlen($temp_desc1) > 0))   { 
			$xwhere .= " AND irsset like rtrim(ltrim('$temp_desc1%')) "; 
			$filters++; 
		}

		if ((strlen($temp_desc2) > 0))   { 
			$xwhere .= " AND normalsset like rtrim(ltrim('$temp_desc2%')) "; 
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
		$orderby = " ORDER BY  sset_id $ascdesc";

		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, sset_id, irsset, normalsset, msisdn_fr, msisdn_to, status, created_by, ".
			"   to_char(dt_created, 'YYYY-MM-DD HH24:MI:SS') dt_created, updated_by modified_by, to_char(dt_updated, 'YYYY-MM-DD HH24:MI:SS') dt_modified ". 
			"  FROM SSET_MAPPING ".
			"  $xwhere $orderby ) b ) ";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("downloadList() : get_all_rows[$sql / $stmt ]");

		//bind
                OCIDefineByName($stmt, 'ID'         , $p_id           );
                OCIDefineByName($stmt, 'SSET_ID'    , $p_sset_id      );
                OCIDefineByName($stmt, 'IRSSET'     , $p_irsset       );
                OCIDefineByName($stmt, 'NORMALSSET' , $p_normalsset   );
                OCIDefineByName($stmt, 'MSISDN_FR'  , $p_msisdn_fr    );
                OCIDefineByName($stmt, 'MSISDN_TO'  , $p_msisdn_to    );
                OCIDefineByName($stmt, 'STATUS'     , $p_status       );
		OCIDefineByName($stmt, 'CREATED_BY' , $p_created_by   );
		OCIDefineByName($stmt, 'DT_CREATED' , $p_dt_created   );
		OCIDefineByName($stmt, 'MODIFIED_BY', $p_modified_by  );
		OCIDefineByName($stmt, 'DT_MODIFIED', $p_dt_modified  );

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
			$csvdata[] = $p_sset_id      ; 
			$csvdata[] = $p_irsset       ; 
			$csvdata[] = $p_normalsset   ; 
			$csvdata[] = $p_msisdn_fr    ; 
			$csvdata[] = $p_msisdn_to    ; 
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
                file_save($csvfile,"List of SSET Mapping\nReport Generation: $rep_gen_dt\n\n".CSV_HEADER_SSETMAPPING_REPORT."\n\n".$csvcont);

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
			$instance   = new SsetMapping();
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
