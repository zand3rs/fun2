<?php
/*
 #----------------------------------------------------------------------------------
 #	@Filename	:	myOraDb.php
 #	@Description	:	
 #
 #
 #	@Date		   :	2009-04-30
 #	@Ver		   :	ver1.1
 #	@Author		:	yiucon
 #
 #
 #  @Modified Date:
 #  @Modified By  :
 #  @Modified Desc:
 #
 #----------------------------------------------------------------------------------
*/

//class
class myOraDb
{

	var $_Link     = null;
	var $_Usr      = null;
	var $_Pwd      = null;
	var $_Dsn      = null;


	//constructor
	function myOraDb($usr,$pwd,$dsn)
	{
		$this->_Usr = $usr;
		$this->_Pwd = $pwd;
		$this->_Dsn = $dsn;

		//init
		$this->init();

	}

	//set-usr
	function setUsr($v)
	{
		$this->_Usr  = $v;
		//$this->debug("setUsr() : $this->_Usr"); 
	}


	//set-pwd
	function setPwd($v)
	{
		$this->_Pwd  = $v;
		//$this->debug("setPwd() : $this->_Pwd"); 
	}

	//set-dsn
	function setDsn($v)
	{
		$this->_Dsn  = $v;
		//$this->debug("setDsn() : $this->_Dsn"); 
	}




	//init
	function init()
	{

		//$this->debug("init() : [$this->_Usr,$this->_Pwd,$this->_Dsn]"); 

		//logon to oracle database
		$this->_Link = OCILogon($this->_Usr,$this->_Pwd,$this->_Dsn);


	}



	//exec
	function query($sql='')
	{


		//Parse SQL passed
		$stmt = OCIParse($this->_Link,$sql);

		//$this->debug("query() : [$sql : $stmt : $conn]");

		//Execute Query
		$ores     = @OCIExecute($stmt);
		$is_error = false;
		$msg      = "SUCCESS";

		//error ?
		if(! $ores)
		{
			//Store error	
			$error    = OCIError ($stmt);
			$msg      = "Error: ${error['code']} ${error['message']}";
			$is_error = true;
		}

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit ($this->_Link);

		//$this->debug("query() : [ $is_error / $msg ]");

		// logoff connection
		OCILogoff($conn);

		return array("error" => $is_error, "error_msg" => "$msg");


	}




	//exec
	function xQuery($sql='')
	{

		//logon to oracle database
		$conn = OCILogon($this->_Usr,$this->_Pwd,$this->_Dsn);


		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//$this->debug("xQuery() : [$sql : $stmt : $conn]");

		//Execute Query
		$ores     = @OCIExecute($stmt);
		$is_error = false;
		$msg      = "SUCCESS";

		//error ?
		if(! $ores)
		{
			//Store error	
			$error    = OCIError ($stmt);
			$msg      = "Error: ${error['code']} ${error['message']}";
			$is_error = true;
		}

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit ($conn);

		//$this->debug("xQuery() : status [ $is_error / $msg ]");

		// logoff connection
		OCILogoff($conn);

		return array("error" => $is_error, "error_msg" => "$msg");

	}



	//exec
	function xBind($sql='',$bindK,$bindV)
	{
		//logon to oracle database
		$conn = OCILogon($this->_Usr,$this->_Pwd,$this->_Dsn);

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//$this->debug("xBind() : [$sql : $stmt : $conn]");

		//Parse SQL passed
		$stmt = OCIParse($conn,$sql);

		//bind
		for($i = 0; $i < count($bindV); $i++)
		{
			OCIDefineByName($stmt,$bindK[$i],  $bindV[$i]);			
			//$this->debug("xBind() : bind-vars( #$i : $bindK[$i] => $bindV[$i] )");
		}	


		//Execute Query
		$ores     = @OCIExecute($stmt);
		$is_error = false;
		$msg      = "SUCCESS";

		//error ?
		if(! $ores)
		{
			//Store error
			$error    = OCIError ($stmt);
			$msg      = "Error: ${error['code']} ${error['message']}";
			$is_error = true;
		}

		//fetch
		$bind_data = null;
		$bind_idx  = 0;

		echo "$stmt #<hr>";
		while(OCIFetch($stmt))
		{
			//bind
			for($i = 0; $i < count($bindV); $i++)
			{
				$bind_data[$bind_idx][$bindK[$i]] = $bindV[$i];			
				//$this->debug("xBind() : fetch-vars( #$i : $bindV[$i] )");

				
			}	
			echo "#$bind_idx<hr>";
			$bind_idx++;
		}

		//Free Query Handle
		OCIFreeStatement($stmt);
		OCICommit ($conn);

		//$this->debug("xBind() : [ $is_error / $msg ]");

		// logoff connection
		OCILogoff($conn);


		return array(
				"error"        => $is_error, 
				"error_msg"    => $msg,
				"bind"         => $bind_data,
				"total"        => $bind_idx
				);

	}


	//get-connection
	function getConnection()
	{
		//$this->debug("getConnection() : $this->_Link"); 
		return $this->_Link;
	}

	//get-link
	function getLink($usr,$pwd,$dsn)
	{

		$conn = OCILogon($usr,$pwd,$dsn);

		//$this->debug("getLink() : [ usr=>$usr / pwd=>$pwd / dsn=>$dsn ]"); 

		return $conn;
	}

	function dbConnection()
	{

		$link = OCILogon($this->_Usr,$this->_Pwd,$this->_Dsn);
		//$this->debug("dbConnection() : link#$link [ usr=>$this->_Usr / pwd=>$this->_Pwd / dsn=>$this->_Dsn]"); 
		return $link;
	}

	//close
	function close()
	{
		if($this->_Link)
		{
			// logoff connection
			OCILogoff($this->_Link);
		}
		$this->debug("close() : $this->_Link");
	}


	//get new
	function &GetInstance($usr,$pwd,$dsn)
	{
		static $instance;

		//set new
		//if(empty($instance) or !$instance or !defined($instance))
		//{
			//set defaults
			$instance   = new myOraDb($usr,$pwd,$dsn);
			//$instance->debug("GetInstance() : newest ......");
		//}

		//$instance->debug("GetInstance() : $instance [ $usr,$pwd,$dsn ]");

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
