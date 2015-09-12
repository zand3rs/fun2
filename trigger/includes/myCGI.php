<?php
/*---------------------------------------------------------------------------------------------
*	Filename	  :	CGIUtils.php
*	Description	:	this script is a utils for cgi vars/ query strings / post vars
*	Date		    :	2009-04-30
*	Version	   	:	ver1.1
*	Author		  :	yiucon
*---------------------------------------------------------------------------------------------*/

class CGIUtils
{
	function CGIUtils()
	{

	}

	function is_older_php()
	{
		$mVersion = phpversion();
		list($yFirst, $ySecond, $yThird) = split("\.", $mVersion);
		if (intval($yFirst) >= 4)
			if (intval($ySecond) > 0)
				return False;
		return True;      
	}

	function get_php_self()
	{
		if ($this->is_older_php())
		{
			global $PHP_SELF;
			return basename($PHP_SELF);
		}
		return basename($_SERVER['PHP_SELF']);
	}

	function get_dir_name()
	{
		if ($this->is_older_php())
		{
			global $PHP_SELF;
			return dirname($PHP_SELF);
		}
		return dirname($_SERVER['PHP_SELF']);
	}

	function get_get_vars()
	{
		if ($this->is_older_php())
		{
			global $HTTP_GET_VARS;
			$mVars = $HTTP_GET_VARS;  
		}   
		else
			$mVars = $_GET;	
		if (!empty($mVars))
		{
			switch(func_num_args())	   
			{
				case 0 :
					return $mVars;
				case 1 :
					$mKey = func_get_arg(0);
					return $mVars[$mKey];
				default:
					$mKeys = func_get_args();
					while (list(, $mKey) = each($mKeys) )
					{
						if (!empty($mVars[$mKey]))
							$mData[$mKey] = $mVars[$mKey];
					}				
					return $mData;	         
			}
		}	
		return "";
	}

	function get_post_vars()
	{
		if ($this->is_older_php())
		{
			global $HTTP_POST_VARS;
			$mVars = $HTTP_POST_VARS;  
		}   
		else
			$mVars = $_POST;	
		if (!empty($mVars))
		{
			switch(func_num_args())	   
			{
				case 0 :
					return $mVars;
				case 1 :
					$mKey = func_get_arg(0);
					return $mVars[$mKey];
				default:
					$mKeys = func_get_args();
					while (list(, $mKey) = each($mKeys) )
					{
						if (!empty($mVars[$mKey]))
							$mData[$mKey] = $mVars[$mKey];
					}				
					return $mData;	         
			}
		}	
		return "";
	}

	function get_post_files()
	{
		if ($this->is_older_php())
		{
			global $HTTP_POST_FILES;
			$mVars = $HTTP_POST_FILES;  
		}   
		else
			$mVars = $_FILES;	
		if (!empty($mVars))
		{
			switch(func_num_args())	   
			{
				case 0 :
					return $mVars;
				case 1 :
					$mKey = func_get_arg(0);
					return $mVars[$mKey];
				default:
					$mKeys = func_get_args();
					while (list(, $mKey) = each($mKeys) )
					{
						if (!empty($mVars[$mKey]))
							$mData[$mKey] = $mVars[$mKey];
					}				
					return $mData;	         
			}
		}	
		return "";
	}

	function get_session_vars()
	{
		if ($this->is_older_php())
		{
			global $HTTP_SESSION_VARS;
			$mVars = $HTTP_SESSION_VARS;  
		}   
		else
			$mVars = $_SESSION; 
		if (!empty($mVars))
		{
			switch(func_num_args())	   
			{
				case 0 :
					return $mVars;
				case 1 :
					$mKey = func_get_arg(0);
					return $mVars[$mKey];
				default:
					$mKeys = func_get_args();
					while (list(, $mKey) = each($mKeys) )
					{
						if (!empty($mVars[$mKey]))
							$mData[$mKey] = $mVars[$mKey];
					}				
					return $mData;	         
			}
		}	
		return "";
	}

	function get_cookie_vars()
	{
		if ($this->is_older_php())
		{
			global $HTTP_COOKIE_VARS;
			$mVars = $HTTP_COOKIE_VARS;  
		}   
		else
			$mVars = $_COOKIE;	
		if (!empty($mVars))
		{
			switch(func_num_args())	   
			{
				case 0 :
					return $mVars;
				case 1 :
					$mKey = func_get_arg(0);
					return $mVars[$mKey];
				default:
					$mKeys = func_get_args();
					while (list(, $mKey) = each($mKeys) )
					{
						if (!empty($mVars[$mKey]))
							$mData[$mKey] = $mVars[$mKey];
					}				
					return $mData;	         
			}
		}	
		return "";
	}

	function make_query_string($xPairs)
	{
		foreach ($xPairs as $mKey=>$mValue)
		{
			$mArrQstr[] = "$mKey=$mValue";
		}
		$mQstr = join("&", $mArrQstr);
		return "?$mQstr";
	}

	function add_query_string($xPairs)
	{
		foreach ($xPairs as $mKey=>$mValue)
		{
			$mArrQstr[] = "$mKey=$mValue";
		}
		$mQstr = join("&", $mArrQstr);
		return "&$mQstr";   
	}

	function set_session_var($xVarName, $xValue)
	{
		if ($this->is_older_php())
		{
			global $HTTP_SESSION_VARS;
			$HTTP_SESSION_VARS[$xVarName] = $xValue;  
		}   
		else
			$_SESSION[$xVarName] = $xValue;
	}

	function unset_session_vars()
	{
		if ($this->is_older_php())
		{
			global $HTTP_SESSION_VARS;
			unset($HTTP_SESSION_VARS);  
		}   
		else
		{      
			foreach ($_SESSION as $mKey=>$mVal)
			{
				unset($_SESSION[$mKey]);
			}  
			unset($_SESSION);
		}
	}

	function set_cookies($xCookies, $xExpire=0)
	{
		foreach($xCookies as $mKey=>$mVal)
		{
			setcookie($mKey, $mVal, $xExpire);
		}
	}


}
?>