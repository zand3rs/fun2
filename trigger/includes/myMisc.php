<?php
 /*
 #----------------------------------------------------------------------- 	
 #	@Filename	    :	myMisc.php
 #	@Description	:	misc functions used
 #	@Date		      :	2009-04-30
 #	@Ver		      :	ver1.1
 #	@Author		    :	yiucon
 #-----------------------------------------------------------------------
*/





function is_logged_in()
{
	global $_SESSION;

	debug("is_logged_in() : raw:". $_SESSION["session.smartygold.profile.is_login"]);

	if(isset($_SESSION["session.smartygold.profile.is_login"]) and $_SESSION["session.smartygold.profile.is_login"]>0)
	{
		if(isset($_SESSION['timeout'])) 
		{
			$session_life = time() - $_SESSION['timeout'];
			debug("is_logged_in() : 'timed-out1' maxlife=" . ini_get("session.gc_maxlifetime") . " time=" . time() . " session_tm=" . $_SESSION['timeout'] . " sesslife=" . $session_life);
			if( isset($session_life) and ($session_life>0) and $session_life>(ini_get("session.gc_maxlifetime")) )
			{
				include_once('includes/init.php');
				debug("is_logged_in() : 'timed-out1' maxlife=" . ini_get("session.gc_maxlifetime") . " sess time=" . $_SESSION['timeout']);
				$userm =& new User();
				$userm->LogoutUpdate($_SESSION["session.smartygold.profile.uid"]);
				logout_session();
				header("Location: timedout.php");
				unset($userm);
				$userm = null;
				exit;
			}
		} 
		if ($_SESSION['timeout']!=time())
		{
			$_SESSION['timeout']=time();
		}
		debug("is_logged_in() : '1' maxlife=" . ini_get("session.gc_maxlifetime") . " sess time=" . $_SESSION['timeout']);
		return true;	
	}
	debug("is_logged_in() : '0'");
	return false;

}
  
function logout_session()
{
	global $_SESSION;

	debug("logout_session() : uid=>" . $_SESSION["session.smartygold.profile.uid"]);

	//unset
	unset($_SESSION["session.smartygold.profile.id"]);
	unset($_SESSION["session.smartygold.profile.user"]);
	unset($_SESSION["session.smartygold.profile.uid"]);
	unset($_SESSION["session.smartygold.profile.role"]);
	unset($_SESSION["session.smartygold.profile.is_login"]);
	unset($_SESSION["session.smartygold.profile.allowed_dt"]);

	@session_destroy();
	
	debug("logout_session() : is_login=>" . $_SESSION["session.smartygold.profile.is_login"]);
	
}
  
function chk_pwd_valid($pwd='')
{
	
	$aa  = get_ord_small_cap($pwd);
	$bb  = get_ord_big_cap($pwd);
	$cc  = get_ord_num($pwd);
	
	debug( "chk_pwd_valid($pwd): [ $cc // $aa // $bb ]" );
	
	if(  (strlen($pwd) == 8) and 
	     (  $cc  >=1  ) and
	     ((  $aa  >=1 ) or
	      (  $bb  >=1 )) 
	  )
	  {
	  	return true;
	  }
	  
	  return false;

	/**
	if(  (strlen($pwd) == 8) and 
	     (@preg_match("/[0-9]{2,}/", $pwd) ) and
	     (@preg_match("/[a-z]{2,}/", $pwd) ) and
	     (@preg_match("/[A-Z]{2,}/", $pwd) ) 
	  )
	  {
	  	return true;
	  }
	  
	  return false;
	  **/
}

function get_ord_num($str)
{
	$found = 0;
	for($i=0;$i<strlen($str);$i++)
	{
		$ch = substr($str,$i,1);
		if(@preg_match("/^([0-9]){1}$/", $ch))
		{
			$found++;
		}
        }
        
       debug( "get_ord_num($str): $found");
        return $found;
}

function get_ord_small_cap($str)
{
	$found = 0;
	for($i=0;$i<strlen($str);$i++)
	{
		$ch = substr($str,$i,1);
		if(@preg_match("/^([a-z]){1}$/", $ch))
		{
			$found++;
		}
        }
        debug( "get_ord_small_cap($str): $found");
        return $found;
}

function get_ord_big_cap($str)
{
	$found = 0;
	for($i=0;$i<strlen($str);$i++)
	{
		$ch = substr($str,$i,1);
		if(@preg_match("/^([A-Z]){1}$/", $ch))
		{
			$found++;
		}
        }
       debug( "get_ord_big_cap($str): $found");
        return $found;
}
?>
