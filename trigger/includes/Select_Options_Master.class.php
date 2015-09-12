<?php
/*
 #----------------------------------------------------------------------------------
 #	@Filename	:	Select_Options_Master.class.php
 #	@Description	:	
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
include_once("Pager/Pager.php");


//class
class Select_Options_Master
{

	//constructor
	function Select_Options_Master()
	{
	}


	//1 instance
	function &GetInstance()
	{
		static $instance;
		
		if(empty($instance) or !$instance or !defined($instance))
		{
		  $c          = new Select_Options_Master();
		  $instance   = $c;
		}
		
		return $instance;
	}
	
	//init
	function getRoleMaster()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT id, role_code, role_desc FROM USER_ROLES ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getRoleMaster() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "ID"        , $P_ID         );
		OCIDefineByName($stmt, "ROLE_CODE" , $P_ROLE_CODE  );
		OCIDefineByName($stmt, "ROLE_DESC" , $P_ROLE_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_ID       ;
			$P_ROLE_CODE;
			$P_ROLE_DESC;
			$data[$P_ROLE_CODE] = $P_ROLE_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getRoleMaster() [$totalrows // $sql]");


		return $data;
	}


	//init
	function getRoleMaster2()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT id, role_code, role_desc FROM USER_ROLES UNION SELECT 0 id, '0' role_code, '<Choose Role>' role_desc FROM DUAL";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getRoleMaster2() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "ID"        , $P_ID         );
		OCIDefineByName($stmt, "ROLE_CODE" , $P_ROLE_CODE  );
		OCIDefineByName($stmt, "ROLE_DESC" , $P_ROLE_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_ID       ;
			$P_ROLE_CODE;
			$P_ROLE_DESC;
			$data[$P_ROLE_CODE] = $P_ROLE_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getRoleMaster2() [$totalrows // $sql]");


		return $data;
	}


	//init
	function getModuleMaster()
	{

		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT id, mod_name, mod_desc FROM MODULES ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getRoleMaster() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "ID"       , $P_ID      );
		OCIDefineByName($stmt, "MOD_NAME" , $P_MOD_NAME);
		OCIDefineByName($stmt, "MOD_DESC" , $P_MOD_DESC);


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_ID       ;
			$P_MOD_NAME;
			$P_MOD_DESC;
			$data[$P_MOD_NAME] = $P_MOD_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getModuleMaster() [$totalrows // $sql]");


		return $data;	

	}

	function getModuleMaster2()
	{

		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT id, mod_name, mod_desc FROM MODULES  UNION SELECT 0 id, '0' mod_name, '<Choose Module>' mod_desc FROM DUAL";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getRoleMaster2() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "ID"       , $P_ID      );
		OCIDefineByName($stmt, "MOD_NAME" , $P_MOD_NAME);
		OCIDefineByName($stmt, "MOD_DESC" , $P_MOD_DESC);


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_ID       ;
			$P_MOD_NAME;
			$P_MOD_DESC;
			$data[$P_MOD_NAME] = $P_MOD_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);

		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getModuleMaster2() [$totalrows // $sql]");


		return $data;	

	}
	//expiry
	function getPwdExpirationDays()
	{
		global $gEventLogger;

		$data         = null;
		$data['0']    = 'Never';
		$data['3']    = '3 Days';
		$data['7']    = '1 Week';
		$data['30']   = '1 Month';

		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getPwdExpirationDays() ");

		return $data;
	}

	//init
	function getModule()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT mod_name P_CODE, mod_name P_DESC FROM user_log group by mod_name UNION select '%' P_CODE, '<Choose Module>' P_DESC from dual order by 1 ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getModule() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "P_CODE" , $P_CODE  );
		OCIDefineByName($stmt, "P_DESC" , $P_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_CODE;
			$P_DESC;
			$data[$P_CODE] = $P_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getModule() [$totalrows // $sql]");


		return $data;
	}



	//init
	function getRoleCode()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT role_Code P_CODE, role_code P_DESC FROM user_log group by role_code UNION select '%' P_CODE, '<Choose Role>' P_DESC from dual order by 1 ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getRoleCode() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "P_CODE" , $P_CODE  );
		OCIDefineByName($stmt, "P_DESC" , $P_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_CODE;
			$P_DESC;
			$data[$P_CODE] = $P_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getRoleCode() [$totalrows // $sql]");


		return $data;
	}


	//ACTIVE/INACTIVE STATUS
	function getStatus()
	{
		global $gEventLogger;

		$data         = null;
		$data['ACTIVE']    = 'Active';
		$data['INACTIVE']    = 'Inactive';

		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getStatus() ");

		return $data;
	}
	
	//ACTIVE/INACTIVE STATUS
	function getStatusSearch()
	{
		global $gEventLogger;

		$data              = null;
		$data['ALL']       = '<Choose Status>';
		$data['ACTIVE']    = 'Active';
		$data['INACTIVE']  = 'Inactive';

		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getStatus() ");

		return $data;
	}

	//ACTIVE/INACTIVE STATUS
	function getSubsStatusSearch()
	{
		global $gEventLogger;

		$data              = null;
		$data['ALL']       = '<Choose Status>';
		$data['ACTIVE']    = 'Active';
		$data['INACTIVE']  = 'Inactive';
                $data['PENDING']   = 'Pending';

		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getStatus() ");

		return $data;
	}

        //ACTIVE/INACTIVE STATUS
        function getSubsStatusSearch2()
        {
                global $gEventLogger;

                $data              = null;
                $data['ALL']       = '<Choose Status>';
                $data['ACTIVE']    = 'Active';
                $data['PENDING']   = 'Pending';

                //event-log
                //$gEventLogger->LogEvent(__CLASS__, "getStatus() ");

                return $data;
        }

	//Customer Types
	function getCustomerTypesSearch()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT customer_type P_CODE, customer_type P_DESC FROM customer_types group by customer_type UNION select '%' P_CODE, '<Choose Customer Type>' P_DESC from dual order by 1 ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getService() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "P_CODE" , $P_CODE  );
		OCIDefineByName($stmt, "P_DESC" , $P_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_CODE;
			$P_DESC;
			$data[$P_CODE] = $P_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getCustomerTypesSearch() [$totalrows // $sql]");


		return $data;
	}

	//Customer Types
	function getCustomerTypes()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT customer_type P_CODE, customer_type P_DESC FROM customer_types group by customer_type UNION select '%' P_CODE, '<Choose Customer Type>' P_DESC from dual order by 1 ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getCustomerTypes() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "P_CODE" , $P_CODE  );
		OCIDefineByName($stmt, "P_DESC" , $P_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_CODE;
			$P_DESC;
			$data[$P_CODE] = $P_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getCustomerTypes() [$totalrows // $sql]");


		return $data;
	}

	//Zones
	function getZones()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT to_char(zone_id) P_CODE, to_char(zone_id) P_DESC FROM zones  UNION select '0' P_CODE, '<Choose Zone>' P_DESC from dual order by 1 ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getCustomerTypes() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "P_CODE" , $P_CODE  );
		OCIDefineByName($stmt, "P_DESC" , $P_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_CODE;
			$P_DESC;
			$data[$P_CODE] = $P_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getZones() [$totalrows // $sql]");


		return $data;
	}


	//ACTIVE/INACTIVE STATUS
	function getMessageType()
	{
		global $gEventLogger;

		$data          = null;
		$data['ALERT'] = 'Alert';
		$data['ERROR'] = 'Error';

		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getMessageType() ");

		return $data;
	}
	
	//ACTIVE/INACTIVE STATUS
	function getMessageTypeSearch()
	{
		global $gEventLogger;

		$data           = null;
		$data['ALL']    = '<Choose Msg Type>';
		$data['ALERT']  = 'Alert';
		$data['ERROR']  = 'Error';

		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getMessageTypeSearch() ");

		return $data;
	}


	//Hlr_Server
	function getHlrSrvr()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT server_name P_CODE, server_name P_DESC FROM hlr_servers  UNION select '%' P_CODE, '<Choose HLR Server>' P_DESC from dual order by 1 ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getHlrSrvr() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "P_CODE" , $P_CODE  );
		OCIDefineByName($stmt, "P_DESC" , $P_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_CODE;
			$P_DESC;
			$data[$P_CODE] = $P_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getHlrSrvr() [$totalrows // $sql]");


		return $data;
	}

	//Hot/Yes/No
	function getHotSearch()
	{
		global $gEventLogger;

		$data       = null;
		$data['ALL']= '<Choose Hot>';
		$data['0']  = 'No';
		$data['1']  = 'Yes';

		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getHotSearch() ");

		return $data;
	}

	//Hot/Yes/No
	function getHot()
	{
		global $gEventLogger;

		$data       = null;
		$data['0']  = 'No';
		$data['1']  = 'Yes';

		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getHot() ");

		return $data;
	}

	//In_Server
	function getInSrvr()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT server_name P_CODE, server_name P_DESC FROM in_servers  UNION select '%' P_CODE, '<Choose IN Server>' P_DESC from dual order by 1 ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getHlrSrvr() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "P_CODE" , $P_CODE  );
		OCIDefineByName($stmt, "P_DESC" , $P_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_CODE;
			$P_DESC;
			$data[$P_CODE] = $P_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getInSrvr() [$totalrows // $sql]");


		return $data;
	}


	//Countries
	function getCountries()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT country_code P_CODE, country_name P_DESC FROM countries  UNION select '%' P_CODE, '<Choose Country>' P_DESC from dual order by 2 ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getCountries() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "P_CODE" , $P_CODE  );
		OCIDefineByName($stmt, "P_DESC" , $P_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_CODE;
			$P_DESC;
			$data[$P_CODE] = $P_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getCountries() [$totalrows // $sql]");


		return $data;
	}


	//getKeywords
	function getKeywords()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT KEYWORD P_CODE, KEYWORD P_DESC FROM keywords group by KEYWORD UNION select '%' P_CODE, '<Choose Keyword>' P_DESC from dual order by 1 ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getKeywords() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "P_CODE" , $P_CODE  );
		OCIDefineByName($stmt, "P_DESC" , $P_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_CODE;
			$P_DESC;
			$data[$P_CODE] = $P_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getKeywords() [$totalrows // $sql]");


		return $data;
	}

	//getSubKeywords
	function getSubKeywords()
	{
		//globals here
		global $gEventLogger;

		//db
		$myOraDb =& myOraDb::GetInstance(DB_USER,DB_PASS,DB_NAME);
		$conn    = $myOraDb->dbConnection();

		//select values for dynamic SQL passing
		$sql    = "  SELECT SUB_KEYWORD P_CODE, SUB_KEYWORD P_DESC FROM keywords group by SUB_KEYWORD  UNION select '%' P_CODE, '<Choose Sub-Keyword>' P_DESC from dual order by 1 ";

		//Parse SQL passed
		$stmt    = OCIParse($conn,$sql);

		//total
		$totalrows = 0;

		$this->debug("getSubKeywords() : [$sql / $stmt ]");

		//bind
		OCIDefineByName($stmt, "P_CODE" , $P_CODE  );
		OCIDefineByName($stmt, "P_DESC" , $P_DESC  );


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
		$data  = null;
		while(OCIFetch($stmt))
		{
			$totalrows++;
			//bind
			$P_CODE;
			$P_DESC;
			$data[$P_CODE] = $P_DESC;	
		}

		//Free Oracle connection
		OCIFreeStatement($stmt);
		OCICommit($conn);

		//Log-off connection		
		OCILogoff($conn);


		//event-log
		//$gEventLogger->LogEvent(__CLASS__, "getSubKeywords() [$totalrows // $sql]");


		return $data;
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
