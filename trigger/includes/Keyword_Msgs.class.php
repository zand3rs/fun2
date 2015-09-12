<?php
/**
#----------------------------------------------------------------------------------
#	@Filename	:	Keyword_Msgs.class.php
#	@Description:	encapsulator for table KEYWORD_MSGS
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
define('CSV_HEADER_KEYWORD_MSG_REPORT',  "KEYWORD, SUB_KEYWORD, MESSAGE_ID, DESCRIPTION, CUSTOMER_TYPE, MESSAGE_TYPE, STATUS, MSG, CREATED BY, DATE, MODIFIED BY, DATE");


//class
class Keyword_Msgs
{

	//constructor
	function Keyword_Msgs()
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
		$sql     = " SELECT  id, keyword, sub_keyword, description, customer_type, message_id, replace(message_id, '_', ' ') message_id_disp, message_type, status, msg_1 FROM KEYWORD_MSG WHERE ID=$id ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("Open() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, 'ID'            , $P_ID            );
		OCIDefineByName($stmt, 'KEYWORD'       , $P_KEYWORD       );
		OCIDefineByName($stmt, 'SUB_KEYWORD'   , $P_SUB_KEYWORD   );
		OCIDefineByName($stmt, 'DESCRIPTION'   , $P_DESCRIPTION   );
		OCIDefineByName($stmt, 'CUSTOMER_TYPE' , $P_CUSTOMER_TYPE );
		OCIDefineByName($stmt, 'MESSAGE_ID'    , $P_MESSAGE_ID    );
		OCIDefineByName($stmt, 'MESSAGE_ID_DISP', $P_MESSAGE_ID_DISP);
		OCIDefineByName($stmt, 'MESSAGE_TYPE'  , $P_MESSAGE_TYPE  );
		OCIDefineByName($stmt, 'MSG_1'         , $P_MSG_1         );
		OCIDefineByName($stmt, 'STATUS'        , $P_STATUS        );

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
                        $P_ID            ;
                        $P_KEYWORD       ;
                        $P_SUB_KEYWORD   ;
                        $P_DESCRIPTION   ;
                        $P_CUSTOMER_TYPE ;
                        $P_MESSAGE_ID    ;
                        $P_MESSAGE_ID_DISP;
                        $P_MESSAGE_TYPE  ;
                        $P_MSG_1         ;
                        $P_STATUS        ;
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
				'id'            => $P_ID            ,
				'keyword'       => $P_KEYWORD       ,
				'sub_keyword'   => $P_SUB_KEYWORD   ,
				'description'   => $P_DESCRIPTION   ,
				'customer_type' => $P_CUSTOMER_TYPE ,
				'message_id'    => $P_MESSAGE_ID    ,
				'message_id_disp'=> $P_MESSAGE_ID_DISP,
				'message_type'  => $P_MESSAGE_TYPE  ,
				'msg_1'         => $P_MSG_1         ,
				'status'        => $P_STATUS        ,
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
		$ret = $this->IfExists($data['keyword'], $data['sub_keyword'], $data['message_id'], $data['id'], $ok);
		if($ret > 0)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_RECORD_ALREADY_EXISTS;
			return $err;
		}

		//usr
		if( (strlen(trim($data['keyword'])) <= 0) || (trim($data['keyword'])=='%'))
		{
			$err['error'] = true;
			$err['msg']   = INVALID_KEYWORD_MSG;
			return $err;
		}

		//usr
		if( (trim($data['sub_keyword'])=='%'))
		{
			$err['error'] = true;
			$err['msg']   = INVALID_SUB_KEYWORD_MSG;
			return $err;
		}

		//usr
		if( (strlen(trim($data['customer_type'])) <= 0) || (trim($data['customer_type'])=='%') )
		{
			$err['error'] = true;
			$err['msg']   = INVALID_CUSTOMER_TYPE;
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
		$sql       = " INSERT INTO KEYWORD_MSG ($kbuff, DT_CREATED )  VALUES ($vbuff, sysdate )";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Save() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Save() [ res=>'$is_ok' // sql=$sql // msg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "NEW KEYWORD MSG: $mbuff");

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
		$sql       = " UPDATE KEYWORD_MSG SET DT_UPDATED=Sysdate, $kbuff WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Update() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Update() [ res=>'$is_ok' // sql=$sql // errmsg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "UPDATE KEYWORD MSG: $mbuff");

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
		$sql       = " DELETE FROM KEYWORD_MSG WHERE ID=$id ";

		//exect
		$data      = $myOraDb->xQuery($sql);

		//ret
		$is_ok     = $data['error'] ? false : true;
		$err_msg   = $data['error_msg'];

		$this->debug("Delete() : $sql // $is_ok // $err_msg");

		//event
		//$gEventLogger->LogEvent(__CLASS__, "Delete() [ res=>'$is_ok' // sql=$sql // errmsg=>'$err_msg'] ");
		$gEventLogger->LogEvent(__CLASS__, "DELETE KEYWORD MSG: ID=$id");

		return $data;
	}


	//get uniq
	function IfExists($c1='',$c2='',$c3='',$id=0,$ok=0)
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
			" SELECT  COUNT(1) as TOTAL FROM KEYWORD_MSG WHERE KEYWORD='$c1' AND SUB_KEYWORD='$c2' AND MESSAGE_ID='$c3' AND ID!=$id " :
			" SELECT  COUNT(1) as TOTAL FROM KEYWORD_MSG WHERE KEYWORD='$c1' AND SUB_KEYWORD='$c2' AND MESSAGE_ID='$c3' ";


		
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
		$opts_msgtyp= $gSelectOptionsMaster->getMessageType();
		$opts_custyp= $gSelectOptionsMaster->getCustomerTypes();
		$opts_keyword = $gSelectOptionsMaster->getKeywords();
		$opts_subkey  = $gSelectOptionsMaster->getSubKeywords();

		$table_fields= array(
				array('name' => 'keyword',        'title'=>'Keyword',      'type' => 'select',  'max'=> 30,   'data_type'=> 'string', 'value' => $_POST['keyword'], 'select_options'=>$opts_keyword, 'select_selected' => $_POST['keyword']  ),
				array('name' => 'sub_keyword',    'title'=>'Sub-Keyword',  'type' => 'select',  'max'=> 30,   'data_type'=> 'string', 'value' => $_POST['sub_keyword'], 'select_options'=>$opts_subkey, 'select_selected' => $_POST['sub_keyword']  ),
				array('name' => 'message_id',    'title'=>'Message ID',  'type' => 'text',    'max'=> 60,   'data_type'=> 'string', 'value' => $_POST['message_id'], 'width'=>480 ),
				array('name' => 'customer_type' , 'title'=>'Customer Type','type' => 'select',  'max'=> 16,   'data_type'=> 'string', 'value' => $_POST['customer_type'], 'select_options'=>$opts_custyp, 'select_selected' => $_POST['customer_type'] ),
				array('name' => 'message_type' ,  'title'=>'Message Type', 'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $_POST['message_type'], 'select_options'=>$opts_msgtyp, 'select_selected' => $_POST['message_type'] ),
				array('name' => 'description' ,   'title'=>'Description',  'type' => 'text',    'max'=> 60,   'data_type'=> 'string', 'value' => $_POST['description'], 'width'=>480 ),
				array('name' => 'msg_1' ,         'title'=>'Message',      'type' => 'textarea','max'=> 1024, 'data_type'=> 'string', 'value' => $_POST['msg_1'], 'rows'=>12, 'cols'=>75 ),
				array('name' => 'status' ,        'title'=>'Status',       'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $_POST['status'], 'select_options'=>$opts_status, 'select_selected' => $_POST['status'] ),
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
		$opts_msgtyp  = $gSelectOptionsMaster->getMessageType();
		$opts_custyp  = $gSelectOptionsMaster->getCustomerTypes();
		$opts_keyword = $gSelectOptionsMaster->getKeywords();
		$opts_subkey  = $gSelectOptionsMaster->getSubKeywords();

		$table_fields= array(
				array('name' => 'keyword',        'title'=>'Keyword',      'type' => 'select',  'max'=> 30,   'data_type'=> 'string', 'value' => $post['keyword'], 'select_options'=>$opts_keyword, 'select_selected' => $post['keyword']  ),
				array('name' => 'sub_keyword',    'title'=>'Sub-Keyword',  'type' => 'select',  'max'=> 30,   'data_type'=> 'string', 'value' => $post['sub_keyword'], 'select_options'=>$opts_subkey, 'select_selected' => $post['sub_keyword']  ),
				array('name' => 'message_id',     'title'=>'Message ID',   'type' => 'text',    'max'=> 60,   'data_type'=> 'string', 'value' => $post['message_id'], 'width'=>480 ),
				array('name' => 'customer_type' , 'title'=>'Customer Type','type' => 'select',  'max'=> 17,   'data_type'=> 'string', 'value' => $post['customer_type'], 'select_options'=>$opts_custyp, 'select_selected' => $post['customer_type'] ),
				array('name' => 'message_type' ,  'title'=>'Message Type', 'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $post['message_type'], 'select_options'=>$opts_msgtyp, 'select_selected' => $post['message_type'] ),
				array('name' => 'description' ,   'title'=>'Description',  'type' => 'text',    'max'=> 60,   'data_type'=> 'string', 'value' => $post['description'], 'width'=>480 ),
				array('name' => 'msg_1' ,         'title'=>'Message',      'type' => 'textarea','max'=> 1024, 'data_type'=> 'string', 'value' => $post['msg_1'], 'rows'=>12, 'cols'=>75 ),
				array('name' => 'status' ,        'title'=>'Status',       'type' => 'select',  'max'=> 12,   'data_type'=> 'string', 'value' => $post['status'], 'select_options'=>$opts_status, 'select_selected' => $post['status'] ),
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
		$temp_code      = $_GET['scode_l'];
		$temp_sub_code  = $_GET['ssubcode'];
		$temp_cust      = $_GET['scust'];
		$temp_mesg      = $_GET['smesg'];
		$temp_desc      = $_GET['sname'];
		$temp_stat      = $_GET['sstat'];
		$temp_file      = $_GET['sfile'];
		$temp_sort      = $_GET['sortedby'];
		$xwherelog = "";

		if ((strlen($temp_code) > 0)  && ($temp_code != '%')) { 
			$xwhere .= " AND keyword like '$temp_code%' "; 
			$xwherelog .= " Keyword=$temp_code";
			$filters++; 
		}

		if (strlen($temp_mesg) > 0) { 
			$xwhere .= " AND upper(message_id) like upper('%$temp_mesg%') "; 
			$xwherelog .= " Message ID=$temp_mesg";
			$filters++; 
		}

		if ((strlen($temp_cust) > 0)  && ($temp_cust != '%')) { 
			$xwhere .= " AND customer_type like '$temp_cust%' "; 
			$xwherelog .= " Customer Type=$temp_cust";
			$filters++; 
		}

		if ((strlen($temp_sub_code) > 0) && ($temp_sub_code != '%')) { 
			$xwhere .= " AND sub_keyword like '$temp_sub_code%' "; 
			$xwherelog .= " Sub Keyword=$temp_sub_code";
			$filters++; 
		}
		if ((strlen($temp_sub_code) == 0) && ($temp_sub_code != '%')) { 
                        $xwhere .= " AND sub_keyword is null ";
                        $xwherelog .= " Sub Keyword=All with No Sub keyword";
                        $filters++;
		}

		if (strlen($temp_desc) > 0) { 
			$xwhere .= " AND msg_1 like '%$temp_desc%' "; 
			$xwherelog .= " Message=$temp_desc";
			$filters++; 
		}

		if ((strlen($temp_stat) > 0)  && ($temp_stat != 'ALL')) { 
			$xwhere .= " AND status like '$temp_stat' "; 
			$xwherelog .= " STATUS=$temp_stat";
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
		$sql  = " SELECT COUNT(1) AS COUNT FROM KEYWORD_MSG $xwhere";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//reset	
		$totalrows = 0;	

		//bind
		OCIDefineByName($stmt,"COUNT",$totalrows);		

		$this->debug("getList() : get_max_row[$sql / $stmt / $totalrows / subcode=$temp_sub_code]");

		//Execute Query
		OCIExecute($stmt);

		//chk
		$totalrows = (OCIFetch($stmt)) ? ($totalrows) : (0);

		//Free Query Handle
		OCIFreeStatement($stmt);

		// logoff connection
		OCILogoff($conn);

		//event
		//$gEventLogger->LogEvent(__CLASS__, "getKeywordMsgList : [WHERE=>'$xwhere'] [ TOTAL=>'$totalrows'] ");

		//chk rowcount
		if(0 == $totalrows )     
		{                   
			$retdata['scode']     = $temp_code ;
			$retdata['ssubcode']  = $temp_sub_code ;
			$retdata['scust']     = $temp_cust ;
			$retdata['smesg']     = $temp_mesg ;
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
                	
			$nPageNo=$pager->getCurrentPageID();
			$nPageTo=$pager->numPages();
			//event
			$gEventLogger->LogEvent(__CLASS__, "SEARCHED By: $xwherelog TOTAL Rows=$totalrows PAGE=$nPageNo/$nPageTo");
                	
			//logon to oracle database
			$conn = $myOraDb->dbConnection();
                	
			//orderby
			//$sortedby       = $extravars['sortedby'];
			$sortedby       = $temp_sort;
			switch($sortedby)
			{
				case 'keyword':
					$orderby = " ORDER BY  keyword $ascdesc";
					break;
				case 'customer_type':
					$orderby = " ORDER BY  customer_type $ascdesc";
					break;
				case 'message_id':
					$orderby = " ORDER BY  message_id $ascdesc";
					break;
				case 'keyword_desc':
					$orderby = " ORDER BY  description $ascdesc";
					break;
				case 'sub_keyword':
					$orderby = " ORDER BY  sub_keyword $ascdesc";
					break;
				case 'status':
					$orderby = " ORDER BY  status $ascdesc";
					break;
				default:	
					$orderby = " ORDER BY id $ascdesc";
					break;
			}
                	
			//select values for dynamic SQL passing
			$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, keyword, sub_keyword, message_id, replace(message_id, '_', ' ') message_id_disp, description, customer_type, message_type, status, msg_1, msg_1 msg FROM KEYWORD_MSG ".
				"  $xwhere $orderby ) b ) WHERE MROW >=$slimit AND MROW < $elimit";
                	
			//Parse SQL passed
			$stmt = OCIParse($conn,$sql);
                	
			$this->debug("getKeywordMsgList() : get_all_rows[$sql / $stmt / $totalrows]");
                	
                	
			//bind
			OCIDefineByName($stmt,'ID'               , $p_id            );
			OCIDefineByName($stmt,'KEYWORD'          , $p_keyword       );
			OCIDefineByName($stmt,'SUB_KEYWORD'      , $p_sub_keyword   );
			OCIDefineByName($stmt,'MESSAGE_ID'       , $p_message_id    );
			OCIDefineByName($stmt,'MESSAGE_ID_DISP'  , $p_message_id_disp);
			OCIDefineByName($stmt,'DESCRIPTION'      , $p_description   );
			OCIDefineByName($stmt,'CUSTOMER_TYPE'    , $p_customer_type );
			OCIDefineByName($stmt,'MESSAGE_TYPE'     , $p_message_type  );
			OCIDefineByName($stmt,'MSG'              , $p_msg           );
			OCIDefineByName($stmt,'MSG_1'            , $p_msg_1         );
			OCIDefineByName($stmt,'STATUS'           , $p_status        );
                	
                	
			//Execute SQL
			OCIExecute($stmt);
                	
			//Loop thru all the records selected and display it in the table
			$idx = 0;
			while (OCIFetch($stmt))
			{
				$alldata[$idx]['id'           ]    = $p_id            ;
				$alldata[$idx]['keyword'      ]    = $p_keyword       ;
				$alldata[$idx]['sub_keyword'  ]    = $p_sub_keyword   ;
				$alldata[$idx]['message_id'   ]    = $p_message_id    ;
				$alldata[$idx]['message_id_disp']  = $p_message_id_disp;
				$alldata[$idx]['description'  ]    = $p_description   ;
				$alldata[$idx]['customer_type']    = $p_customer_type ;
				$alldata[$idx]['message_type' ]    = $p_message_type  ;
				$alldata[$idx]['msg'          ]    = $p_msg           ;
				$alldata[$idx]['msg_1'        ]    = $p_msg_1         ;
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
		$retdata['ssubcode']  = $temp_sub_code ;
		$retdata['scust']     = $temp_cust ;
		$retdata['smesg']     = $temp_mesg ;
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
		//$gEventLogger->LogEvent(__CLASS__, "getKeywordMsgList(#b) : [ total=>'$totalrows'] ");

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
		$temp_sub_code  = $_GET['ssubcode'];
		$temp_cust      = $_GET['scust'];
		$temp_mesg      = $_GET['smesg'];
		$temp_desc      = $_GET['sname'];
		$temp_stat      = $_GET['sstat'];

		if ((strlen($temp_code) > 0)  && ($temp_code != '%')) { 
			$xwhere .= " AND keyword like '$temp_code%' "; 
			$filters++; 
		}

		if (strlen($temp_mesg) > 0) { 
			$xwhere .= " AND upper(message_id) like upper('%$temp_mesg%') "; 
			$filters++; 
		}

		if ((strlen($temp_cust) > 0)  && ($temp_cust != '%')) { 
			$xwhere .= " AND customer_type like '$temp_cust%' "; 
			$filters++; 
		}

		if ((strlen($temp_sub_code) > 0) && ($temp_sub_code != '%')) { 
			$xwhere .= " AND sub_keyword like '$temp_sub_code%' "; 
			$filters++; 
		}

		if (strlen($temp_desc) > 0) { 
			$xwhere .= " AND msg_1 like '%$temp_desc%' "; 
			$filters++; 
		}

		if ((strlen($temp_stat) > 0)  && ($temp_stat != 'ALL')) { 
			$xwhere .= " AND status like '$temp_stat' "; 
			$filters++; 
		}

		if ($filters > 0) {
			$xwhere = " where 1 =1 ". $xwhere;
		}

		// logon to oracle database
		$conn = $myOraDb->dbConnection();

		//type of order
		$ascdesc = " ASC ";	
		$orderby = " ORDER BY  keyword $ascdesc";

		//select values for dynamic SQL passing
		$sql  = "  SELECT * FROM ( SELECT ROWNUM MROW, b.* FROM ( SELECT id, keyword, sub_keyword, message_id, description, customer_type, message_type, status, msg_1 msg, created_by, ".
			"   to_char(dt_created, 'YYYY-MM-DD HH24:MI:SS') dt_created, updated_by modified_by, to_char(dt_updated, 'YYYY-MM-DD HH24:MI:SS') dt_modified ". 
			"  FROM KEYWORD_MSG ".
			"  $xwhere $orderby ) b ) ";

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		$this->debug("downloadList() : get_all_rows[$sql / $stmt]");

		//bind
		OCIDefineByName($stmt,'ID'               , $p_id            );
		OCIDefineByName($stmt,'KEYWORD'          , $p_keyword       );
		OCIDefineByName($stmt,'SUB_KEYWORD'      , $p_sub_keyword   );
		OCIDefineByName($stmt,'MESSAGE_ID'       , $p_message_id    );
		OCIDefineByName($stmt,'DESCRIPTION'      , $p_description   );
		OCIDefineByName($stmt,'CUSTOMER_TYPE'    , $p_customer_type );
		OCIDefineByName($stmt,'MESSAGE_TYPE'     , $p_message_type  );
		OCIDefineByName($stmt,'MSG'              , $p_msg           );
		OCIDefineByName($stmt,'STATUS'           , $p_status        );
		OCIDefineByName($stmt,'CREATED_BY'       , $p_created_by    );
		OCIDefineByName($stmt,'DT_CREATED'       , $p_dt_created    );
		OCIDefineByName($stmt,'MODIFIED_BY'      , $p_modified_by   );
		OCIDefineByName($stmt,'DT_MODIFIED'      , $p_dt_modified   );

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
			$csvdata[] = $p_keyword       ;
			$csvdata[] = $p_sub_keyword   ;
			$csvdata[] = $p_message_id    ;
			$csvdata[] = $p_description   ;
			$csvdata[] = $p_customer_type ;
			$csvdata[] = $p_message_type  ;
			$csvdata[] = $p_status        ;
			$csvdata[] = $p_msg           ;
			$csvdata[] = $p_created_by    ;
			$csvdata[] = $p_dt_created    ;
			$csvdata[] = $p_modified_by   ;
			$csvdata[] = $p_dt_modified   ;
			$csvstr    = join('","', $csvdata);
			$csvcont .= "\"$csvstr\"\n";
			$idx++;
		}

		//close csv
                $rep_gen_dt = date("Y-m-d H:i:s");
                file_save($csvfile,"List of Keyword Messages\nReport Generation: $rep_gen_dt\n\n".CSV_HEADER_KEYWORD_MSG_REPORT."\n\n".$csvcont);

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
			$instance   = new Keyword_Msgs();
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
