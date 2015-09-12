<?php
/*
#----------------------------------------------------------------------- 	
#	@Filename	    :	log.php
#	@Description	:	web logger
#	@Date		      :	2009-04-30
#	@Ver		      :	ver1.1
#	@Author		    :	yiucon
#-----------------------------------------------------------------------
*/
class myLogger 
{
   var $_Handle;
   var $_File;
   var $_Mode;
   
   
   function myLogger($logfile="/tmp/myLogger.log",$mode="a")
   {  	 	
   	  //save
      $this->_File   = $logfile;
      $this->_Mode   = (!strcasecmp($mode,'a') or !strcasecmp($mode,'append')) ? ('a') : ('w');
      //init
      $this->_Handle = @fopen($this->_File,$this->_Mode);	
   }   
   
   function debug($msg="")
   {  
   	  //fmt
   	  $tm   = date("Y-m-d H:i:s");
	    $buff = sprintf("DEBUG: [%15s] : %s : %s", " ----- " , $tm, $msg); 
   	  //save
   	  @fwrite($this->_Handle,"$buff\n");
   }   
   
   function log($msg="")
   {
   	  $this->debug($msg);
   }   
   
   function write($msg="")
   {
   	  $this->debug($msg);
   }
   
   function destroy()
   {
   	  //close
      @fclose($this->_Handle);
   }
   
}
?>