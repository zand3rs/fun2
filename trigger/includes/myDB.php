<?php
 /*
 #----------------------------------------------------------------------- 	
 #	@Filename	    :	myDB.php
 #	@Description	:	web db utils
 #	@Date		      :	2009-04-30
 #	@Ver		      :	ver1.1
 #	@Author		    :	yiucon
 #-----------------------------------------------------------------------
*/
class mySqlDb 
{
   var $_link;
   var $_dbname;   
   
   function mySqlDb($host, $user, $pass, $dbname) {  	 	
   	$this->connect($host, $user, $pass, $dbname);
   }   
   
   function connect($host, $user, $pass, $dbname) {   	   	    
   	  $this->_dbname = $dbname;  
      $this->_link   = mysql_connect($host, $user, $pass) 
         or die ( $this->error("Database connection failed!", "HOST: $host| USER: $user|PASS: $pass") );         
      @mysql_select_db($dbname, $this->_link)   	
         or die ( $this->error("Cannot select database!", "DATABASE: $dbname") );
   }
   
   function is_connected() { return (empty ($this->_link))? False: True; }
   
   function close() { return @mysql_close($this->_link);  }
   
   function query($query, $errstr) {      
      $result = @mysql_query($query, $this->_link)
         or die("<hr><li><em>DB_ERROR: $errstr</em></li><hr>");
         return $result;    	
   }
   
   function insert_id() { return @mysql_insert_id($this->_link); }
   
   function affected_rows() { return @mysql_affected_rows($this->_link); }
   
   function num_rows($result) { return @mysql_num_rows($result); }
   
   function fetch_row($result){ return @mysql_fetch_row($result); }
   
   function fetch_array($result){ return @mysql_fetch_array($result); }
   
   function fetch_assoc($result){    	 		
      
      return   mysql_fetch_array($result);  	
   }
   
   function data_seek($result, $index) { return mysql_data_seek($result, $index); }
   
   
   function free_resource($result){ return @mysql_free_result($result); }   

   function num_fields($result){ return mysql_num_fields($result); }
   
   function list_fields($table) { return mysql_list_fields($this->_dbname, $table, $this->_link); }
   
   function field_name($result, $index) { return mysql_field_name($result, $index); }   
  
   function get_max_rec($query) {
      $result = $this->query($query, "mySqlDb::get_max_rec() failed!");      
      return $this->num_rows($result);
   }   

   function error($errstr, $debugstr="")
   {
	   global $gLoggerConf,$gDebug,$gLogDebug;

	   $errno     = @mysql_errno($this->_link);
	   $error     =  @mysql_error($this->_link);	
	   $ret       = $errstr."ERROR: (errno: $errno / msg=$error)";
	   $tm        = date("Y-m-d H:i:s");
	   $buff      = sprintf("DEBUG: [%15s] : %s : %s", " ----- " , $tm, $ret); 

	   if($gDebug)
	   {
		   echo "$buff<hr>\n";	

	   }
	   if($gLogDebug)
	   {
		   $logger = &Log::singleton('file', WEBLOG, __CLASS__, $gLoggerConf);
		   $logger->log($ret);
	   }
   }   	


 function get_lock($val, $secs=10) 
 {
 	    //fmt
 	    
      $this->query(
         "SELECT get_lock('$val', $secs) ",
         "mySqlDb::get_lock() failed!"
      );
 }

 function release_lock($val)
 {
 	    //fmt
 	     
      $this->query(
         "SELECT release_lock('$val') ",
         "mySqlDb::release_lock() failed!"
      );
 }



   function lock_table($table, $mode="WRITE") {
      $this->query(
         "LOCK TABLES $table $mode",
         "mySqlDb::lock() failed!"
      );
   }
   
   function unlock_tables() {
      $this->query(
         "UNLOCK TABLES",
         "mySqlDb::unlock_tables() failed!"
      );
   }
    
   function get_link() { return $this->_link; }
   
}

?>