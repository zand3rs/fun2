<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	Services.class.php
#	@Description:	encapsulator for table SERVICES
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

define('CSV_HEADER_SERVICES_REPORT',  "SERVICE_CODE, SERVICE_DESC, PRIORITY, STATUS, SVC_URL, SVC_HOST, SVC_PORT, SVC_USER, SVC_PASS, DB_HOST, DB_USER, DB_PASS, TIMEOUT_SEC, THREAD_COUNT, MAX_RETRY, APP_LIB, SVC_NAME, EXPIRY, CREATED BY, DATE, MODIFIED BY, DATE");

//class
class Services
{

	//constructor
	function Services()
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
		$sql     = " SELECT  id, service_code, service_desc, priority, svc_url,svc_host,svc_port,svc_user,svc_pass,db_host,db_user,db_pass,timeout_sec,thread_count,max_retry,app_lib,svc_name,expiry,status FROM SERVICES WHERE ID=$id ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("Open() : [$sql / $stmt ]");

		//bind
                OCIDefineByName($stmt, "ID"           , $P_ID           );
                OCIDefineByName($stmt, "SERVICE_CODE" , $P_SERVICE_CODE );
                OCIDefineByName($stmt, "SERVICE_DESC" , $P_SERVICE_DESC );
                OCIDefineByName($stmt, "PRIORITY"     , $P_PRIORITY     );
                OCIDefineByName($stmt, "STATUS"       , $P_STATUS       );
                OCIDefineByName($stmt, "SVC_URL"      , $P_SVC_URL      );
                OCIDefineByName($stmt, "SVC_HOST"     , $P_SVC_HOST     );
                OCIDefineByName($stmt, "SVC_PORT"     , $P_SVC_PORT     );
                OCIDefineByName($stmt, "SVC_USER"     , $P_SVC_USER     );
                OCIDefineByName($stmt, "SVC_PASS"     , $P_SVC_PASS     );
                OCIDefineByName($stmt, "DB_HOST"      , $P_DB_HOST      );
                OCIDefineByName($stmt, "DB_USER"      , $P_DB_USER      );
                OCIDefineByName($stmt, "DB_PASS"      , $P_DB_PASS      );
                OCIDefineByName($stmt, "TIMEOUT_SEC"  , $P_TIMEOUT_SEC  );
                OCIDefineByName($stmt, "THREAD_COUNT" , $P_THREAD_COUNT );
                OCIDefineByName($stmt, "MAX_RETRY"    , $P_MAX_RETRY    );
                OCIDefineByName($stmt, "APP_LIB"      , $P_APP_LIB      );
                OCIDefineByName($stmt, "SVC_NAME"     , $P_SVC_NAME     );
                OCIDefineByName($stmt, "EXPIRY"       , $P_EXPIRY       );

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
			$P_ID           ;
			$P_SERVICE_CODE ;
			$P_SERVICE_DESC ;
			$P_PRIORITY     ;
			$P_STATUS       ;
                        $P_SVC_URL      ;
                        $P_SVC_HOST     ;
                        $P_SVC_PORT     ;
                        $P_SVC_USER     ;
                        $P_SVC_PASS     ;
                        $P_DB_HOST      ;
                        $P_DB_USER      ;
                        $P_DB_PASS      ;
                        $P_TIMEOUT_SEC  ;
                        $P_THREAD_COUNT ;
                        $P_MAX_RETRY    ;
                        $P_APP_LIB      ;
                        $P_SVC_NAME     ;
                        $P_EXPIRY       ;
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
				'id'		=> $P_ID           ,
				'service_code'	=> $P_SERVICE_CODE ,
				'service_desc'	=> $P_SERVICE_DESC ,
				'priority'	=> $P_PRIORITY     ,
				'status'	=> $P_STATUS       ,
                                'svc_url'       => $P_SVC_URL      ,
                                'svc_host'      => $P_SVC_HOST     ,
                                'svc_port'      => $P_SVC_PORT     ,
                                'svc_user'      => $P_SVC_USER     ,
                                'svc_pass'      => $P_SVC_PASS     ,
                                'db_host'       => $P_DB_HOST      ,
                                'db_user'       => $P_DB_USER      ,
                                'db_pass'       => $P_DB_PASS      ,
                                'timeout_sec'   => $P_TIMEOUT_SEC  ,
                                'thread_count'  => $P_THREAD_COUNT ,
                                'max_retry'     => $P_MAX_RETRY    ,
                                'app_lib'       => $P_APP_LIB      ,
                                'svc_name'      => $P_SVC_NAME     ,
                                'expiry'        => $P_EXPIRY       ,
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
		$ret = $this->IfExists($data['service_code'], $data['id'],$ok);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = "ERROR: Service Code Already Exists.";
			return $err;
		}

		//user 
		$ret = $this->chkPriority($data['priority'], $data['id']);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = "ERROR: Priority Number Already Used.";
			return $err;
		}

		//usr
		if(strlen(trim($data['service_code'])) <= 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_SERVICE;
			$err['msg']   = "ERROR: Please enter Service Code.";
			return $err;
		}

		//fname		
		//if(strlen(trim($data['SERVICE_DESC'])) <= 0)
		//{
		//	$err['error'] = true;
		//	$err['msg']   = INVALID_SERVICE_DESC;
		//	return $err;
		//}

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
		$V[]       = "'".ora_addslashes($_SESSION["session.smartygold.profile.user"]) . "'";
		$kbuff     = join(", ", $K);
		$vbuff     = join(", ", $V);


		//new profile
		$sql       = " INSERT INTO SERVICES ($kbuff, DT_CREATED )  VALUES ($vbuff, sysdate )";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Save() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "NEW SERVICE: $mbuff ");

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
		$sql       = " UPDATE SERVICES SET DT_UPDATED=Sysdate, $kbuff WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Update() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "UPDATE SERVICE: $mbuff");

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
		$sql       = " DELETE FROM SERVICES WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Delete() : $sql // $is_ok // $err_msg");

		//event
		$gEventLogger->LogEvent(__CLASS__, "DELETE SERVICE: ID=$id ");

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
		$id        = intval(trim($id));

		//update profile
		$sql       = ($id>0 and $ok) ? 
			" SELECT  COUNT(1) as TOTAL FROM SERVICES WHERE SERVICE_CODE='$c1' AND ID!=$id " :
			" SELECT  COUNT(1) as TOTAL FROM SERVICES WHERE SERVICE_CODE='$c1' ";


		
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

		$table_fields= array(
				array('name' => 'service_code' ,  'title'=>'Service Code'  ,   'type' => 'text',    'max'=> 30,   'data_type'=> 'string', 'value' => $_POST['service_code'], 'width' => '240' ),
				array('name' => 'service_desc' ,  'title'=>'Description'   ,   'type' => 'text',    'max'=> 255,  'data_type'=> 'string', 'value' => $_POST['service_desc'], 'width' => '480' ),
				array('name' => 'priority'     ,  'title'=>'Priority'      ,   'type' => 'text',    'max'=> 4,    'data_type'=> 'string', 'value' => $_POST['priority'] ),
				array('name' => 'status'       ,  'title'=>'Status'        ,   'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $_POST['status'], 'select_options'=>$opts_status, 'select_selected' => $_POST['status'] ),
                                array('name' => 'svc_url'      ,  'title'=>'SVC URL'       ,   'type' => 'text',  'max'=> 256,   'data_type'=> 'string', 'value' => $_POST['svc_url'     ], 'width' => '480' ),
                                array('name' => 'svc_host'     ,  'title'=>'SVC Host'      ,   'type' => 'text',  'max'=> 64,   'data_type'=> 'string', 'value' => $_POST['svc_host'    ], 'width' => '240' ),
                                array('name' => 'svc_port'     ,  'title'=>'SVC Port'      ,   'type' => 'text',  'max'=> 16,   'data_type'=> 'string', 'value' => $_POST['svc_port'    ] ),
                                array('name' => 'svc_user'     ,  'title'=>'SVC Username'  ,   'type' => 'text',  'max'=> 32,   'data_type'=> 'string', 'value' => $_POST['svc_user'    ] ),
                                array('name' => 'svc_pass'     ,  'title'=>'SVC Password'  ,   'type' => 'text',  'max'=> 32,   'data_type'=> 'string', 'value' => $_POST['svc_pass'    ] ),
                                array('name' => 'db_host'      ,  'title'=>'DB Host'       ,   'type' => 'text',  'max'=> 64,   'data_type'=> 'string', 'value' => $_POST['db_host'     ] ),
                                array('name' => 'db_user'      ,  'title'=>'DB Username'   ,   'type' => 'text',  'max'=> 32,   'data_type'=> 'string', 'value' => $_POST['db_user'     ] ),
                                array('name' => 'db_pass'      ,  'title'=>'DB Password'   ,   'type' => 'text',  'max'=> 32,   'data_type'=> 'string', 'value' => $_POST['db_pass'     ] ),
                                array('name' => 'timeout_sec'  ,  'title'=>'Timeout (s)'   ,   'type' => 'text',  'max'=> 16,   'data_type'=> 'string', 'value' => $_POST['timeout_sec' ] ),
                                array('name' => 'thread_count' ,  'title'=>'Thread Count'  ,   'type' => 'text',  'max'=> 16,   'data_type'=> 'string', 'value' => $_POST['thread_count'] ),
                                array('name' => 'max_retry'    ,  'title'=>'Retry'         ,   'type' => 'text',  'max'=> 16,   'data_type'=> 'string', 'value' => $_POST['max_retry'   ] ),
                                array('name' => 'app_lib'      ,  'title'=>'Appl Library'  ,   'type' => 'text',  'max'=> 32,   'data_type'=> 'string', 'value' => $_POST['app_lib'     ], 'width' => '480' ),
                                array('name' => 'svc_name'     ,  'title'=>'SVC Name'      ,   'type' => 'text',  'max'=> 64,   'data_type'=> 'string', 'value' => $_POST['svc_name'    ], 'width' => '240' ),
                                array('name' => 'expiry'       ,  'title'=>'Expiry'        ,   'type' => 'text',  'max'=> 64,   'data_type'=> 'string', 'value' => $_POST['expiry'      ] ),

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

		$table_fields= array(
				array('name' => 'service_code' ,  'title'=>'Service Code'  ,  'type' => 'text',    'max'=> 30,   'data_type'=> 'string', 'value' => $post['service_code'], 'width' => '240' ),
				array('name' => 'service_desc' ,  'title'=>'Description'   ,  'type' => 'text',    'max'=> 255,  'data_type'=> 'string', 'value' => $post['service_desc'], 'width' => '480' ),
				array('name' => 'priority'     ,  'title'=>'Priority'      ,  'type' => 'text',    'max'=> 4,    'data_type'=> 'string', 'value' => $post['priority'] ),
				array('name' => 'status'       ,  'title'=>'Status'        ,  'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $post['status'], 'select_options'=>$opts_status, 'select_selected' => $post['status'] ),
                                array('name' => 'svc_url'      ,  'title'=>'SVC URL'       ,  'type' => 'text',    'max'=> 256,   'data_type'=> 'string', 'value' => $post['svc_url'     ], 'width' => '480'  ),
                                array('name' => 'svc_host'     ,  'title'=>'SVC Host'      ,  'type' => 'text',    'max'=> 64,   'data_type'=> 'string', 'value' => $post['svc_host'    ], 'width' => '240'  ),
                                array('name' => 'svc_port'     ,  'title'=>'SVC Port'      ,  'type' => 'text',    'max'=> 16,   'data_type'=> 'string', 'value' => $post['svc_port'    ] ),
                                array('name' => 'svc_user'     ,  'title'=>'SVC Username'  ,  'type' => 'text',    'max'=> 32,   'data_type'=> 'string', 'value' => $post['svc_user'    ] ),
                                array('name' => 'svc_pass'     ,  'title'=>'SVC Password'  ,  'type' => 'text',    'max'=> 32,   'data_type'=> 'string', 'value' => $post['svc_pass'    ] ),
                                array('name' => 'db_host'      ,  'title'=>'DB Host'       ,  'type' => 'text',    'max'=> 64,   'data_type'=> 'string', 'value' => $post['db_host'     ] ),
                                array('name' => 'db_user'      ,  'title'=>'DB Username'   ,  'type' => 'text',    'max'=> 32,   'data_type'=> 'string', 'value' => $post['db_user'     ] ),
                                array('name' => 'db_pass'      ,  'title'=>'DB Password'   ,  'type' => 'text',    'max'=> 32,   'data_type'=> 'string', 'value' => $post['db_pass'     ] ),
                                array('name' => 'timeout_sec'  ,  'title'=>'Timeout (s)'   ,  'type' => 'text',    'max'=> 16,   'data_type'=> 'string', 'value' => $post['timeout_sec' ] ),
                                array('name' => 'thread_count' ,  'title'=>'Thread Count'  ,  'type' => 'text',    'max'=> 16,   'data_type'=> 'string', 'value' => $post['thread_count'] ),
                                array('name' => 'max_retry'    ,  'title'=>'Retry'         ,  'type' => 'text',    'max'=> 16,   'data_type'=> 'string', 'value' => $post['max_retry'   ] ),
                                array('name' => 'app_lib'      ,  'title'=>'Appl Library'  ,  'type' => 'text',    'max'=> 32,   'data_type'=> 'string', 'value' => $post['app_lib'     ], 'width' => '480'  ),
                                array('name' => 'svc_name'     ,  'title'=>'SVC Name'      ,  'type' => 'text',    'max'=> 64,   'data_type'=> 'string', 'value' => $post['svc_name'    ], 'width' => '240'  ),
                                array('name' => 'expiry'       ,  'title'=>'Expiry'        ,  'type' => 'text',    'max'=> 64,   'data_type'=> 'string', 'value' => $post['expiry'      ] ),

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
		$temp_code  = $_GET['scode'];
		$temp_desc  = $_GET['sname'];
		$temp_stat  = $_GET['sstat'];
		$temp_file  = $_GET['sfile'];
		$temp_sort  = $_GET['sortedby'];

		$xwherelog  = "";
		if (strlen($temp_code) > 0) { 
			$xwhere .= " AND service_code like '$temp_code%' "; 
			$xwherelog .= " SERVICE CODE=$temp_code "; 
			$filters++; 
		}

		if (strlen($temp_desc) > 0) { 
			$xwhere .= " AND service_desc like '$temp_desc%' "; 
			$xwherelog .= " SERVICE DESC=$temp_desc "; 
			$filters++; 
		}

		if ((strlen($temp_stat) > 0)  && ($temp_stat != 'ALL')) { 
			$xwhere .= " AND status like '$temp_stat%' "; 
			$xwherelog .= " STATUS=$temp_stat "; 
			$filters++; 
		}

		if ($filters > 0) {
			$xwhere = " where 1 =1 ". $xwhere;
		}

	
		//fmt
		$whatsort    = $extravars['whatsort'];
		//type of order
		$ascdesc     = (!$whatsort) ? (" ASC ") : ( " DESC " );	

		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql  = " SELECT COUNT(1) AS COUNT FROM SERVICES $xwhere";

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
			if ($xwherelog=="")
				$xwherelog="ALL";
			//event
			$gEventLogger->LogEvent(__CLASS__, "SEARCHED By=$xwherelog TOTAL Rows=$totalrows PAGE=$nPageNo/$nPageTo");
                	
                	
			//logon to oracle database
			$conn = $myOraDb->dbConnection();
                	
			//orderby
			//$sortedby       = $extravars['sortedby'];
			$sortedby       = $temp_sort;
			switch($sortedby)
			{
				case 'name':
					$orderby = " ORDER BY  service_code $ascdesc";
					break;
				case 'desc':
					$orderby = " ORDER BY  service_desc $ascdesc";
					break;
				case 'priority':
					$orderby = " ORDER BY  priority $ascdesc";
					break;
				case 'status':
					$orderby = " ORDER BY  status $ascdesc";
					break;
				default:	
					$orderby = " ORDER BY priority $ascdesc";
					break;
			}
                	
			//select values for dynamic SQL passing
			$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, service_code, service_desc, priority, svc_url,svc_host,svc_port,svc_user,svc_pass,db_host,db_user,db_pass,timeout_sec,thread_count,max_retry,app_lib,svc_name,expiry, status FROM SERVICES ".
				"  $xwhere $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";
                	
			//Parse SQL passed
			$stmt = OCIParse($conn,$sql);
                	
			$this->debug("getList() : get_all_rows[$sql / $stmt / $totalrows]");
                	
                	
			//bind
			OCIDefineByName($stmt, "ID"           , $p_id           );
			OCIDefineByName($stmt, "SERVICE_CODE" , $p_code         );
			OCIDefineByName($stmt, "SERVICE_DESC" , $p_desc         );
			OCIDefineByName($stmt, "PRIORITY"     , $p_priority     );
			OCIDefineByName($stmt, "STATUS"       , $p_status       );
                	OCIDefineByName($stmt, "SVC_URL"      , $p_svc_url      );
                	OCIDefineByName($stmt, "SVC_HOST"     , $p_svc_host     );
                	OCIDefineByName($stmt, "SVC_PORT"     , $p_svc_port     );
                	OCIDefineByName($stmt, "SVC_USER"     , $p_svc_user     );
                	OCIDefineByName($stmt, "SVC_PASS"     , $p_svc_pass     );
                	OCIDefineByName($stmt, "DB_HOST"      , $p_db_host      );
                	OCIDefineByName($stmt, "DB_USER"      , $p_db_user      );
                	OCIDefineByName($stmt, "DB_PASS"      , $p_db_pass      );
                	OCIDefineByName($stmt, "TIMEOUT_SEC"  , $p_timeout_sec  );
                	OCIDefineByName($stmt, "THREAD_COUNT" , $p_thread_count );
                	OCIDefineByName($stmt, "MAX_RETRY"    , $p_max_retry    );
                	OCIDefineByName($stmt, "APP_LIB"      , $p_app_lib      );
                	OCIDefineByName($stmt, "SVC_NAME"     , $p_svc_name     );
                	OCIDefineByName($stmt, "EXPIRY"       , $p_expiry       );
                	
                	
			//Execute SQL
			OCIExecute($stmt);
                	
			//Loop thru all the records selected and display it in the table
			$idx = 0;
			while (OCIFetch($stmt))
			{
				$alldata[$idx]['id']              = $p_id            ;
				$alldata[$idx]['service_code']    = $p_code          ;
				$alldata[$idx]['service_desc']    = $p_desc          ;
				$alldata[$idx]['priority']        = $p_priority      ;
				$alldata[$idx]['status']          = $p_status        ;
                	        $alldata[$idx]['svc_url']         = $p_svc_url       ;
                	        $alldata[$idx]['svc_host']        = $p_svc_host      ;
                	        $alldata[$idx]['svc_port']        = $p_svc_port      ;
                	        $alldata[$idx]['svc_user']        = $p_svc_user      ;
                	        $alldata[$idx]['svc_pass']        = $p_svc_pass      ;
                	        $alldata[$idx]['db_host']         = $p_db_host       ;
                	        $alldata[$idx]['db_user']         = $p_db_user       ;
                	        $alldata[$idx]['db_pass']         = $p_db_pass       ;
                	        $alldata[$idx]['timeout_sec']     = $p_timeout_sec   ;
                	        $alldata[$idx]['thread_count']    = $p_thread_count  ;
                	        $alldata[$idx]['max_retry']       = $p_max_retry     ;
                	        $alldata[$idx]['app_lib']         = $p_app_lib       ;
                	        $alldata[$idx]['svc_name']        = $p_svc_name      ;
                	        $alldata[$idx]['expiry']          = $p_expiry        ;
                	
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
		}				 

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getList(#b) : [ total=>'$totalrows'] ");

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
		$retdata['total']      = 0;
		$retdata['data']       = null;
		$retdata['links']      = 'No result found';


		$filters=0;
		$temp_code  = $_GET['scode'];
		$temp_desc  = $_GET['sname'];
		$temp_stat  = $_GET['sstat'];

		if (strlen($temp_code) > 0) { 
			$xwhere .= " AND service_code like '$temp_code%' "; 
			$filters++; 
		}

		if (strlen($temp_desc) > 0) { 
			$xwhere .= " AND service_desc like '$temp_desc%' "; 
			$filters++; 
		}

		if ((strlen($temp_stat) > 0)  && ($temp_stat != 'ALL')) { 
			$xwhere .= " AND status like '$temp_stat%' "; 
			$filters++; 
		}

		if ($filters > 0) {
			$xwhere = " where 1 =1 ". $xwhere;
		}

	
		//fmt
		$ascdesc     = " ASC ";	

		//logon to oracle database
		$conn = $myOraDb->dbConnection();

		//orderby
		$orderby = " ORDER BY  service_code $ascdesc";

		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, service_code, service_desc, priority, svc_url,svc_host,svc_port,svc_user,svc_pass,db_host,db_user,db_pass,timeout_sec,thread_count,max_retry,app_lib,svc_name,expiry, status, created_by, ".
			"   to_char(dt_created, 'YYYY-MM-DD HH24:MI:SS') dt_created, updated_by modified_by, to_char(dt_updated, 'YYYY-MM-DD HH24:MI:SS') dt_modified ". 
			"  FROM SERVICES ".
			"  $xwhere $orderby ) b )";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("downloadList() : get_all_rows[$sql / $stmt / $totalrows]");

		//bind
		OCIDefineByName($stmt, "ID"           , $p_id           );
		OCIDefineByName($stmt, "SERVICE_CODE" , $p_code         );
		OCIDefineByName($stmt, "SERVICE_DESC" , $p_desc         );
		OCIDefineByName($stmt, "PRIORITY"     , $p_priority     );
		OCIDefineByName($stmt, "STATUS"       , $p_status       );
                OCIDefineByName($stmt, "SVC_URL"      , $p_svc_url      );
                OCIDefineByName($stmt, "SVC_HOST"     , $p_svc_host     );
                OCIDefineByName($stmt, "SVC_PORT"     , $p_svc_port     );
                OCIDefineByName($stmt, "SVC_USER"     , $p_svc_user     );
                OCIDefineByName($stmt, "SVC_PASS"     , $p_svc_pass     );
                OCIDefineByName($stmt, "DB_HOST"      , $p_db_host      );
                OCIDefineByName($stmt, "DB_USER"      , $p_db_user      );
                OCIDefineByName($stmt, "DB_PASS"      , $p_db_pass      );
                OCIDefineByName($stmt, "TIMEOUT_SEC"  , $p_timeout_sec  );
                OCIDefineByName($stmt, "THREAD_COUNT" , $p_thread_count );
                OCIDefineByName($stmt, "MAX_RETRY"    , $p_max_retry    );
                OCIDefineByName($stmt, "APP_LIB"      , $p_app_lib      );
                OCIDefineByName($stmt, "SVC_NAME"     , $p_svc_name     );
                OCIDefineByName($stmt, "EXPIRY"       , $p_expiry       );
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
			$csvdata   = null;
			$csvdata[]    = $p_code          ;
			$csvdata[]    = $p_desc          ;
			$csvdata[]    = $p_priority      ;
			$csvdata[]    = $p_status        ;
                        $csvdata[]    = $p_svc_url       ;
                        $csvdata[]    = $p_svc_host      ;
                        $csvdata[]    = $p_svc_port      ;
                        $csvdata[]    = $p_svc_user      ;
                        $csvdata[]    = $p_svc_pass      ;
                        $csvdata[]    = $p_db_host       ;
                        $csvdata[]    = $p_db_user       ;
                        $csvdata[]    = $p_db_pass       ;
                        $csvdata[]    = $p_timeout_sec   ;
                        $csvdata[]    = $p_thread_count  ;
                        $csvdata[]    = $p_max_retry     ;
                        $csvdata[]    = $p_app_lib       ;
                        $csvdata[]    = $p_svc_name      ;
                        $csvdata[]    = $p_expiry        ;
			$csvdata[]    = $p_created_by    ;
			$csvdata[]    = $p_dt_created    ;
			$csvdata[]    = $p_modified_by   ;
			$csvdata[]    = $p_dt_modified   ;

			$csvstr    = join('","', $csvdata);
			$csvcont .= "\"$csvstr\"\n";
			$idx++;
		}

		//close csv
                $rep_gen_dt = date("Y-m-d H:i:s");
                file_save($csvfile,"List of Services\nReport Generation: $rep_gen_dt\n\n".CSV_HEADER_SERVICES_REPORT."\n\n".$csvcont);

		//Free Oracle connection
		OCIFreeStatement($stmt);

		//Log-off connection		
		OCILogoff($conn);

		//save
		$retdata['csvfilename'] = $csvfnam;

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getList(#b) : [ total=>'$totalrows'] ");

		//return data
		return $retdata;
	}

	function viewServiceDetail($c1='')
	{
		global $gEventLogger;
		$gEventLogger->LogEvent(__CLASS__, "View Service Detail: $c1 ");
	}

	//get uniq
	function chkPriority($c1='',$c2='')
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//format the insert-parameters
		$c1        = ora_addslashes(trim($c1));
		$c2        = ora_addslashes(trim($c2));

		//update profile
		if ($c2=='')
			$sql       = " SELECT COUNT(1) as TOTAL FROM services WHERE priority=$c1";
		else
			$sql       = " SELECT COUNT(1) as TOTAL FROM services WHERE priority=$c1 and id <> $c2";
		
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

		$this->debug("chkPriority() : // $sql // $totalrows");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "isBlacklisted() [ $sql // $totalrows ] ");

		return $totalrows;


	}

	//1-only
	function &GetInstance()
	{
		static $instance;

		if(empty($instance) or !$instance or !defined($instance))
		{
			$instance   = new Services();
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
