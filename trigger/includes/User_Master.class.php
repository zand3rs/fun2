<?php
/*
 #----------------------------------------------------------------------------------
 #	@Filename	:	User_Master.class.php
 #	@Description	:	encapsulator for table user_master
 #
 #
 #	@Date		:	2008-07-01
 #	@Ver		:	ver1.1
 #	@Author		:	bayugyug
 #
 #
 #  @Modified Date:
 #  @Modified By  :
 #  @Modified Desc:
 #
 #----------------------------------------------------------------------------------
*/
//include
include_once("includes/Pager/Pager.php");


//class
class User_Master
{

	var $_Master;
        
	//constructor
	function User_Master()
	{
	    //reset
	    $this->_Master = null;
	}

	
	//init
	function Open($user='')
	{
		//globals here
		global $gDB_Handle;

		//format the insert-parameters
		$user      = addslashes(trim($user));
		
		//update profile
		$sql       = " SELECT  * FROM user_master WHERE user_code='$user' LIMIT 1";
		$res       = $gDB_Handle->query($sql, "Error in Open($sql)");

		//check res
		$is_ok     =($gDB_Handle->num_rows($res) > 0)  ? (true) : (false);

		debug("Open()::query($sql) / $is_ok");

		$data      = null;

		//save
		$data        = $gDB_Handle->fetch_assoc($res);
		$this->_Master = $data;
		
		//free-resources
		if($res)
			$gDB_Handle->free_resource($res);

                
		return $data;
        }
        
        
        //init
	function GetUser($user='')
	{
			//globals here
			global $gDB_Handle;
	
			//format the insert-parameters
			$user      = addslashes(trim($user));
			
			//update profile
			$sql       = " SELECT  * FROM user_master WHERE user_id='$user' LIMIT 1";
			$res       = $gDB_Handle->query($sql, "Error in GetUser()");
	
			//check res
			$is_ok     =($gDB_Handle->num_rows($res) > 0)  ? (true) : (false);
	
			debug("GetUser()::query($sql) / $is_ok");
	
			$data      = null;
	
			//save
			$data        = $gDB_Handle->fetch_assoc($res);
			
			//free-resources
			if($res)
				$gDB_Handle->free_resource($res);
	
	                
			return $data;
	}
        
        
        //chk fields
        function CheckFields($data=null,$ok=0)
        {
        	//default
        	$err          = null;
        	$err['error'] = false;
        	$err['msg']   = null;
        	
        	
        	//user 
        	$ret = $this->IfExists($data['user_code'], $data['user_id'],$ok);
		if($ret['exists'])
		{
			$err['error'] = true;
			$err['msg']   = INVALID_USER_CODE_ALREADY_EXISTS;
			return $err;
		}

        	if(strlen(trim($data['user_code'])) <= 3)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_USER_CODE;
			return $err;
		}
        
        	if(strlen(trim($data['password'])) <= 3)
		{
			$err['error'] = true;
			$err['msg']   = INVALID_PASSWORD_LENGTH;
			return $err;
		}

        	if(
        	  (strlen(trim($data['office_code'])) <= 0 ) 
        	  )
		{
			$err['error'] = true;
			$err['msg']   = INVALID_MUST_SELECT_OFFICE_CODE;
			return $err;
		}
		
		//ok
		return $err;
        	
        }
        
        //save
	function Save($data=null)
	{
		//globals here
		global $gDB_Handle;

		//format the insert-parameters
		$K     = null;
		$V     = null;
		$kbuff = null;
		$vbuff = null;
		
		while( list($idx,$col) = each($data) )
		{
			$K[] = $col['name'];
			
			$V[] = "'".addslashes(trim($col['value'])) . "'";
		}
		
		$kbuff     = join(",", $K);
		$vbuff     = join(",", $V);


		//new profile
		$sql       = " INSERT INTO user_master ($kbuff, dt_created) ".
		             " VALUES ($vbuff, now() )                      ";
		$res       = $gDB_Handle->query($sql, "Error in Save($sql)");

		//check res
		$is_ok     =($gDB_Handle->affected_rows() > 0)  ? (true) : (false);

		debug("Save()::query($sql) / $is_ok");

		//free-resources
		if($res)
			$gDB_Handle->free_resource($res);


		return $is_ok;
	}
	

        //update
	function Update($data=null)
	{
		//globals here
		global $gDB_Handle;

		//format the insert-parameters
		
		$K     = null;
		$kbuff = null;
		while( list($idx,$col) = each($data) )
		{
		        debug("Update() : $col[name] ===> $col[value]" );
			if($col['name'] == 'user_id') 
			{
				$id    = addslashes(trim($col['value']));
				continue;
			}
			
			
			
			$K[] = $col['name']. " = '".addslashes(trim($col['value'])) . "'";
			
		}
		
		$K[] = " updated_by = '".addslashes($_SESSION["session.smartygold.profile.user"])."' ";
		
		$kbuff     = join(" , ", $K);
                
                //new profile
		$sql       = " UPDATE user_master SET dt_updated=sysdate, $kbuff WHERE user_id='$id' LIMIT 1";
		$res       = $gDB_Handle->query($sql, "Error in Update($sql)");

		//check res
		$is_ok     =($gDB_Handle->affected_rows() > 0)  ? (true) : (false);

		debug("Update()::query($sql) / $is_ok");

		//free-resources
		if($res)
			$gDB_Handle->free_resource($res);

		return $is_ok;
	}
	
	        
	//update login
	function loginUpdate($user="",$ip='')
	{
		//globals here
		global $gDB_Handle;

		//format the insert-parameters
		$user    = addslashes(trim($user));
		$ip      = addslashes(trim($ip));

		//new profile
		$sql       = " UPDATE  user_master SET login=sysdate ip_address='$ip' WHERE user_code='$user' LIMIT 1";
		$res       = $gDB_Handle->query($sql, "Error in loginUpdate()");

		//check res
		$is_ok     =($gDB_Handle->affected_rows() > 0)  ? (true) : (false);

		debug("loginUpdate()::query($sql) / $is_ok");

		//free-resources
		if($res)
			$gDB_Handle->free_resource($res);

		return $is_ok;
	}

        //delete
	function Delete($user=null)
	{
		//globals here
		global $gDB_Handle;

		//format the insert-parameters
		$user      = addslashes(trim($user));
		
		//new profile
		$sql       = " DELETE FROM user_master WHERE user_id='$user' LIMIT 1";
		$res       = $gDB_Handle->query($sql, "Error in Delete()");

		//check res
		$is_ok     =($gDB_Handle->affected_rows() > 0)  ? (true) : (false);

		debug("Delete()::query($sql) / $is_ok");

		//free-resources
		if($res)
			$gDB_Handle->free_resource($res);

		return $is_ok;
	}
	
        
        
	//get uniq
	function IfExists($user='',$id=0,$ok=0)
	{
		//globals here
		global $gDB_Handle;

		//format the insert-parameters
		$user      = addslashes(trim($user));

		//update profile
		if($id>0 and $ok)
		{
		   $sql      = " SELECT  * FROM user_master WHERE user_code='$user' AND user_id!='$id' LIMIT 1";
		}
		else
		{
		  $sql       = " SELECT  * FROM user_master WHERE user_code='$user' LIMIT 1";
		}
		
		$res       = $gDB_Handle->query($sql, "Error in IfExists()");

		//check res
		$is_ok     =($gDB_Handle->num_rows($res) > 0)  ? (true) : (false);

		debug("IfExists()::query($sql) / $is_ok");

		$data      = null;

		//save
		$data           = $gDB_Handle->fetch_assoc($res);
		$data['exists'] = $is_ok;

		//free-resources
		if($res)
			$gDB_Handle->free_resource($res);


		return $data;

	
	}
	
	//get form fields
	function getTableFields()
	{
		global $_POST;
		global $gSelectOptionsMaster;
		
		$opts_role          = $gSelectOptionsMaster->getRoleMaster();
		$opts_office        = $gSelectOptionsMaster->getOfficeCodeMaster();

 $table_fields= array(
		array(name => 'user_code'          ,  title=>'Username',             'type' => 'text',    'value' => $_POST['user_code'          ]          ,'max' => '20'),          
		array(name => 'password'           ,  title=>'Password',             'type' => 'password','value' => $_POST['password'           ]          ,'max' => '20'),
		array(name => 'last_name'          ,  title=>'Last Name',            'type' => 'text',    'value' => $_POST['last_name'          ]          ,'max' => '255'),          
		array(name => 'first_name'         ,  title=>'First Name',           'type' => 'text',    'value' => $_POST['first_name'         ]          ,'max' => '255'),         
		array(name => 'address'            ,  title=>'Address',              'type' => 'text',    'value' => $_POST['address'            ]          ,'max' => '255'),           
		array(name => 'email'              ,  title=>'Email',                'type' => 'text',    'value' => $_POST['email'              ]          ,'max' => '255'),             
		array(name => 'telno'              ,  title=>'Telephone No.',        'type' => 'text',    'value' => $_POST['telno'              ]          ,'max' => '13'),                           
		array(name => 'gsmnum'             ,  title=>'Mobile No.',           'type' => 'text',    'value' => $_POST['gsmnum'             ]          ,'max' => '13'),                                        
		array(name => 'role'               ,  title=>'Role Code',            'type' => 'select',  'value' => $_POST['role'               ]          ,'max' => '20', 'select_options'=>$opts_role,            'select_selected' => $_POST['role']),
		array(name => 'office_code'        ,  title=>'Office Code',          'type' => 'select',  'value' => $_POST['office_code'    ]             ,'max' => '20', 'select_options'=>$opts_office,            'select_selected' => $_POST['office_code'] ),
		
		
	   );

	   

	   return $table_fields;

	}


        //get form fields
	function getTableFields4Update($id="")
	{


		global $gSelectOptionsMaster;

$opts_role          = $gSelectOptionsMaster->getRoleMaster();
$opts_office        = $gSelectOptionsMaster->getOfficeCodeMaster();

	    $post = $this->GetUser($id);			
	    
	    $table_fields= array(
	    			array(name => 'user_code'          ,  title=>'Username',             'type' => 'text',    'value' => $post['user_code'          ]          ,'max' => '20'),          
	    			array(name => 'password'           ,  title=>'Password',             'type' => 'password','value' => $post['password'           ]          ,'max' => '20'),
	    			array(name => 'last_name'          ,  title=>'Last Name',            'type' => 'text',    'value' => $post['last_name'          ]          ,'max' => '255'),          
	    			array(name => 'first_name'         ,  title=>'First Name',           'type' => 'text',    'value' => $post['first_name'         ]          ,'max' => '255'),         
	    			array(name => 'address'            ,  title=>'Address',              'type' => 'text',    'value' => $post['address'            ]          ,'max' => '255'),           
	    			array(name => 'email'              ,  title=>'Email',                'type' => 'text',    'value' => $post['email'              ]          ,'max' => '255'),             
	    			array(name => 'telno'              ,  title=>'Telephone No.',        'type' => 'text',    'value' => $post['telno'              ]          ,'max' => '13'),                           
	    			array(name => 'gsmnum'             ,  title=>'Mobile No.',           'type' => 'text',    'value' => $post['gsmnum'             ]          ,'max' => '13'),                                        
				array(name => 'role'               ,  title=>'Role Code',            'type' => 'select',  'value' => $post['role'               ]          ,'max' => '20', 'select_options'=>$opts_role,            'select_selected' => $post['role']),
		array(name => 'office_code'        ,  title=>'Office Code',          'type' => 'select',  'value' => $post['office_code'    ]             ,'max' => '20', 'select_options'=>$opts_office,            'select_selected' => $post['office_code'] ),
					);


	   return $table_fields;

	}

	function getList()
	{
		//globals here
		global $gDB_Handle;


		//defaults
		$retdata               = null;
		$retdata['total']      = 0;
		$retdata['data']       = null;
		$retdata['links']      = 'No result found';



		//type of order
		$ascdesc     = ($whatsort) ? (" ASC ") : ( " DESC " );


		//sortedby
		$orderby = " user_id $ascdesc ";
		

		

		//sid to DB to get max row
		$sql       = "SELECT count(1) as total FROM user_master WHERE 1=1 LIMIT 1";
		$res       = $gDB_Handle->query($sql, "Error in :: getList($sql)");



		$totalItems= 0;
		if ($gDB_Handle->num_rows($res) > 0)
		{
		$row = $gDB_Handle->fetch_assoc($res);
		$totalItems = intval($row['total']);
		}

		debug("getList() :: $sql :: $totalItems ");


		//oops...
		if(0 == $totalItems )
		{
		return $retdata;
		}

		$offset        = SEARCH_MAX_OFFSET_MORE;
		$pager_options = array(
		'totalItems'   => $totalItems,
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

		//-------------------------------
		//  set up pager
		//-------------------------------

		$pager = Pager::factory($pager_options);
		$page = array();
		$page['totalItems']   = $pager_options['totalItems'];
		$page['links']        = $pager->links;
		$page['page_numbers'] = array(
		'current' => $pager->getCurrentPageID(),
		'total'   => $pager->numPages()
		);
		list($page['from'], $page['to']) = $pager->getOffsetByPageId();

		//-------------------------------
		//  get the limited rows only
		//-------------------------------
		if(strlen(trim($orderby)) > 0)
		$sort_order      = " ORDER BY $orderby ";

		$slimit    = intval($page['from'])-1;
		$elimit    = intval($pager_options['perPage']);
		$sql       = "SELECT * FROM user_master WHERE 1=1 $xwhere $sort_order LIMIT $slimit, $elimit ";
		$res       = $gDB_Handle->query($sql, "Error in :: getList($sql)");

		debug("getList() :: $sql");


		//format return values
		$data_line     =  '';
		$j             =  0;
		$alldata       = null;
		$retdata       = null;

		if ($gDB_Handle->num_rows($res) > 0)
		{
			while($row = $gDB_Handle->fetch_assoc($res))
			{
			$alldata[]      = $row;
			}
		}

		//save
		$retdata['data']      = $alldata;
		$retdata['total']     = $totalItems;
		//no rec
		if($totalItems <=0 )
		{
		$retdata['links'] = 'No result found';
		}
		else
		{
		$navigator            = $page['links'];
		$retdata['links']     = $navigator;
		}

		//return
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
