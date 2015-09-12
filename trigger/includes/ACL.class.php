<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	ACL.class.php
#	@Description:	encapsulator for table ACL
#
#
#	@Date		  :	2009-04-30
#	@Ver		  :	ver1.1
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
class ACL
{

	//constructor
	function ACL()
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


		//update profile
		$sql     = " SELECT  id, role_code, mod_name, can_view, can_update, can_add, can_delete FROM ACL WHERE ID=$id ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("Get() : [$sql / $stmt ]");

		//bind
		OCIDefineByName( $stmt, 'ID'      , $P_ID      );
		OCIDefineByName( $stmt, 'ROLE_CODE'      , $P_ROLE_CODE      );
		OCIDefineByName( $stmt, 'MOD_NAME'      , $P_MOD_NAME      );
		OCIDefineByName( $stmt, 'CAN_VIEW'      , $P_CAN_VIEW      );
		OCIDefineByName( $stmt, 'CAN_UPDATE'      , $P_CAN_UPDATE      );
		OCIDefineByName( $stmt, 'CAN_ADD'      , $P_CAN_ADD      );
		OCIDefineByName( $stmt, 'CAN_DELETE'      , $P_CAN_DELETE      );


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
			$P_ID      ;
			$P_ROLE_CODE      ;
			$P_MOD_NAME      ;
			$P_CAN_VIEW      ;
			$P_CAN_UPDATE      ;
			$P_CAN_ADD      ;
			$P_CAN_DELETE      ;
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Get() : [ $sql // $totalrows // $is_error / $err_msg] ");

		$alldata = array(
				'error'            => $is_error, 
				'error_msg'        => $err_msg,
				'bind_total'       => $totalrows,
				'bind_data'        => array(
					'id'           => $P_ID,
					'role_code'    => $P_ROLE_CODE,
					'mod_name'     => $P_MOD_NAME,
					'can_view'     => $P_CAN_VIEW,
					'can_update'   => $P_CAN_UPDATE,
					'can_add'      => $P_CAN_ADD,
					'can_delete'   => $P_CAN_DELETE,
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
		$ret = $this->IfExists($data['role_code'], $data['mod_name'], $data['id'],$ok);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_RECORD_ALREADY_EXISTS;
			return $err;
		}

		//usr
		if(strlen(trim($data['role_code'])) <= 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_ROLE_CODE_NAME;
			return $err;
		}

		//fname		
		if(strlen(trim($data['mod_name'])) <= 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_MOD_NAME;
			return $err;
		}

		//ok
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
		$V[]       = "'".ora_addslashes( strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system')) . "'";
		$kbuff     = join(",", $K);
		$vbuff     = join(",", $V);


		//new profile
		$sql       = " INSERT INTO ACL ($kbuff, DT_CREATED )  VALUES ($vbuff, sysdate )";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Save() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Save() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "NEW ACL: $mbuff");

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
				$K[] = $col['name']. " = ".intval(trim($col['value']));

			if (strlen($col['value'])>0)
				$mbuff .= $col['name']."=".$col['value']." ";
		}

		//created-by
		$K[]       = " updated_by = '".ora_addslashes(strlen($_SESSION["session.smartygold.profile.user"]) > 0 ? ($_SESSION["session.smartygold.profile.user"]) : ('system'))."' ";

		$kbuff     = join(" , ", $K);

		//new profile
		$sql       = " UPDATE ACL SET DT_UPDATED=Sysdate, $kbuff WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Update() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Update() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "UPDATE ACL: $mbuff");

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
		$sql       = " DELETE FROM ACL WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Delete() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Delete() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "DELETE ACL: ID=$id");

		return $data;
	}


	//get uniq
	function IfExists($c1='',$c2='',$id=0,$ok=0)
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
			" SELECT  COUNT(1) as TOTAL FROM ACL WHERE ROLE_CODE='$c1' AND MOD_NAME='$c2' AND ID!=$id " :
			" SELECT  COUNT(1) as TOTAL FROM ACL WHERE ROLE_CODE='$c1' AND MOD_NAME='$c2'  ";

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


	//get form fields
	function getTableFields()
	{
		global $_POST;
		global $gSelectOptionsMaster;

		$opts_role = $gSelectOptionsMaster->getRoleMaster();
		$opts_mod  = $gSelectOptionsMaster->getModuleMaster();

		$table_fields= array(
				array('name' => 'role_code'      ,  'title'=>'Web Role',     'type' => 'select','data_type'=> 'string',  'value' => $_POST['role_code'          ]          ,'select_options'=>$opts_role,           'select_selected' => $_POST['role_code']),
				array('name' => 'mod_name'       ,  'title'=>'Module Name',  'type' => 'select','data_type'=> 'string',  'value' => $_POST['mod_name'           ]          ,'select_options'=>$opts_mod,            'select_selected' => $_POST['mod_name']),
				array('name' => 'can_view'       ,  'title'=>'Can View',     'type' => 'check', 'data_type'=> 'int',     'value' => $_POST['can_view'           ]),       
				array('name' => 'can_add'        ,  'title'=>'Can Add',      'type' => 'check', 'data_type'=> 'int',     'value' => $_POST['can_add'            ]),       
				array('name' => 'can_update'     ,  'title'=>'Can Update',   'type' => 'check', 'data_type'=> 'int',     'value' => $_POST['can_update'         ]),       
				array('name' => 'can_delete'     ,  'title'=>'Can Delete',   'type' => 'check', 'data_type'=> 'int',     'value' => $_POST['can_delete'         ]),       
				);

		return $table_fields;

	}


	//get form fields
	function getTableFields4Update($id="")
	{
		global $gSelectOptionsMaster;

		$data      = $this->Get($id);			
		$post      = $data['bind_data'];


		$opts_role = $gSelectOptionsMaster->getRoleMaster();
		$opts_mod  = $gSelectOptionsMaster->getModuleMaster();

		$table_fields= array(
				array('name' => 'role_code'      ,  'title'=>'Web Role',     'type' => 'select','data_type'=> 'string',  'value' => $post['role'               ]          ,'select_options'=>$opts_role,           'select_selected' => $post['role_code']),
				array('name' => 'mod_name'       ,  'title'=>'Module Name',  'type' => 'select','data_type'=> 'string',  'value' => $post['mod_name'           ]          ,'select_options'=>$opts_mod,            'select_selected' => $post['mod_name']),
				array('name' => 'can_view'       ,  'title'=>'Can View',     'type' => 'check', 'data_type'=> 'int',     'value' => $post['can_view'           ]),       
				array('name' => 'can_add'        ,  'title'=>'Can Add',      'type' => 'check', 'data_type'=> 'int',     'value' => $post['can_add'            ]),       
				array('name' => 'can_update'     ,  'title'=>'Can Update',   'type' => 'check', 'data_type'=> 'int',     'value' => $post['can_update'         ]),       
				array('name' => 'can_delete'     ,  'title'=>'Can Delete',   'type' => 'check', 'data_type'=> 'int',     'value' => $post['can_delete'         ]),       
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


		//fmt
		$whatsort    = $extravars['whatsort'];

		//type of order
		$ascdesc     = (!$whatsort) ? (" DESC ") : ( " ASC " );	

		$filters=0;
		$xwherelog = "";
		$temp_role = $_GET['role_code'];
		$temp_mod  = $_GET['mod_name'];
		if ($temp_role != '0'){ 
			$xwhere = " AND role_code like '$temp_role%' "; 
			$xwherelog .= " Role=$temp_role";
			$filters++; 
		}

		if ($temp_mod != '0') { 
			$xwhere .= " AND mod_name like '$temp_mod%' "; 
			$xwherelog .= " Module=$temp_mod";
			$filters++; 
		}

		if ($filters > 0) {
			$xwhere = "where  1=1". $xwhere;
		} else {
			$xwhere = "";
			$xwherelog .= "All";
		}	

		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT COUNT(1) AS COUNT FROM ACL $xwhere";

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
		//$gEventLogger->LogEvent(__CLASS__, "getList(#a) : [where='$xwhere'] [ total=>'$totalrows'] ");

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
		$page  = array();
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
		$sortedby       = $extravars['sortedby'];
		switch($sortedby)
		{
			case 'role':
				$orderby = " ORDER BY  role_code $ascdesc";
				break;
			case 'mod':
				$orderby = " ORDER BY  mod_name  $ascdesc";
				break;
			default:	
				$orderby = " ORDER BY id $ascdesc";
				break;
		}
		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, role_code, mod_name, can_view, can_update, can_add, can_delete  FROM ACL ".
			"  $xwhere $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("getList() : get_all_rows[$sql / $stmt / $totalrows]");


		//bind
		OCIDefineByName($stmt,'ID'                    , $p_id          );
		OCIDefineByName($stmt,'ROLE_CODE'             , $p_role_code   );
		OCIDefineByName($stmt,'MOD_NAME'              , $p_mod_name    );
		OCIDefineByName($stmt,'CAN_VIEW'              , $p_can_view    );
		OCIDefineByName($stmt,'CAN_ADD'               , $p_can_add     );
		OCIDefineByName($stmt,'CAN_UPDATE'            , $p_can_update  );
		OCIDefineByName($stmt,'CAN_DELETE'            , $p_can_delete  );

		//Execute SQL	
		OCIExecute($stmt);

		//Loop thru all the records selected and display it in the table
		$idx = 0;
		while (OCIFetch($stmt))
		{
			$alldata[$idx]['id'        ]            = $p_id          ;
			$alldata[$idx]['role_code' ]            = $p_role_code   ;
			$alldata[$idx]['mod_name'  ]            = $p_mod_name    ;
			$alldata[$idx]['can_view'  ]            = $p_can_view    ;
			$alldata[$idx]['can_add'   ]            = $p_can_add     ;
			$alldata[$idx]['can_update']            = $p_can_update  ;
			$alldata[$idx]['can_delete']            = $p_can_delete  ;
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
		//$gEventLogger->LogEvent(__CLASS__, "getList(#b) : [ total=>'$totalrows'] ");

		//return data
		return $retdata;
	}

	//1-only
	function &GetInstance()
	{
		static $instance;

		if(empty($instance) or !$instance or !defined($instance))
		{
			$instance   = new ACL();
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
