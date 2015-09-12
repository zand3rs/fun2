<?php
//include
include_once('includes/init.php');

define("LOGIN_FAILED_EMPTY_USERNAME", "Missing or wrong username.");
define("LOGIN_FAILED_EMPTY_PASSWORD", "Missing or wrong password.");
define("LOGIN_FAILED",                             "Sorry, your username and or password does not match.");
define("LOGIN_FAILED_ACCOUNT_IS_LOCKED",           "Sorry, your account has been locked.");
define("LOGIN_FAILED_PASSWORD_ALREADY_EXPIRED",    "Sorry, your password already expired.");
define("LOGIN_FAILED_ACCOUNT_IS_CURRENTLY_LOGIN" , "Sorry, your account is currently logged in.");
define("LOGIN_FAILED_NEED_TO_CHANGE_PASSWORD",    "Sorry, you need to change your password. Click Change Password link below.");

//process
if( strtoupper($_POST['btnSubmit']) == 'LOGIN')
{

	//fmt
	$mdata['name'] = trim($_POST['user']);
	$mdata['pass'] = trim($_POST['pass']);

	//chk
	$userm      =& new User();
	$userm_data = $userm->Open($mdata['name']);
	$tsnow      = date("Y-m-d H:i:s");
	$tsnow_ref  = $userm_data['bind_data']['allowed_dt'];

	if (strlen($mdata['name']) == 0) 
	{
		debug("login failed: not userm_data");
		$gSmarty->assign('error_msg',LOGIN_FAILED_EMPTY_USERNAME);
		$errflag = 1;
	}
	elseif (strlen($mdata['pass']) == 0) 
	{
		debug("login failed: not userm_data");
		$gSmarty->assign('error_msg',LOGIN_FAILED_EMPTY_PASSWORD);
		$errflag = 1;
	} 
	elseif(! $userm_data or null == $userm_data or $userm_data['bind_total']<=0)
	{
		debug("login failed: not userm_data");
		$gSmarty->assign('error_msg',LOGIN_FAILED);
	}
	elseif($userm_data['bind_data']['lockout']>0)
	{
		debug("login failed: LOCKOUT#a ! ! ! !");
		//lock-update
		$userm->LockoutCtrUpdate($mdata['name']);
		$gSmarty->assign('error_msg',LOGIN_FAILED_ACCOUNT_IS_LOCKED);
	}
	elseif($userm_data['bind_data']['is_login'] > 0 and ($userm_data['bind_data']['todi_dt'] < $userm_data['bind_data']['log_dt']))
	{
		debug("login failed: is login @ other console ! ! ! !");
		$gSmarty->assign('error_msg',LOGIN_FAILED_ACCOUNT_IS_CURRENTLY_LOGIN);

	}
	elseif($userm_data['bind_data']['on_first_login'] > 0 )
	{
		debug("login failed: need to change password ! ! ! !");
		$gSmarty->assign('error_msg',LOGIN_FAILED_NEED_TO_CHANGE_PASSWORD);
        
	}
	elseif($tsnow_ref < $tsnow or strlen($tsnow_ref) <= 0)
	{
		debug("login failed: PASSWORD_ALREADY_EXPIRED  ! ! ! ! ");

		$userm->LockoutCtrUpdate($mdata['name']);

		$gSmarty->assign('error_msg',LOGIN_FAILED_PASSWORD_ALREADY_EXPIRED);
	}
	else
	{
		$enc_pwd = md5( $mdata['pass']. PWD_ENCRYPT_KEY);

		//chk
		if( !strcmp($enc_pwd,$userm_data['bind_data']['password']) and strlen($mdata['pass'])>0 )
		{

			debug("LOGIN-OK => ". $userm_data['bind_data']['user_id']);
			$userm->LoginOk($mdata['name']);
			//save session
			$_SESSION["session.smartygold.profile.user"]      = $userm_data['bind_data']['user_code'];
			$_SESSION["session.smartygold.profile.uid"]       = $userm_data['bind_data']['user_id'];
			$_SESSION["session.smartygold.profile.is_login"]  = 1;
			$_SESSION["session.smartygold.profile.role"]      = $userm_data['bind_data']['role_code'];
			$_SESSION["session.smartygold.profile.allowed_dt"]= $tsnow_ref;
			$_SESSION["timeout"]				  = time();

			//upd8
			$userm->LoginUpdate($userm_data['bind_data']['user_id']);
			$userm->ResetLockoutCtrUpdate($userm_data['bind_data']['user_id']);

			$gSmarty->assign('is_log_in',true);
		}
		else
		{
			if($userm_data['bind_data']['lockout_ctr']+1 >= MAX_LOCKOUT_CTR)
			{
				debug("login failed: LOCKOUT#b ! ! ! !");

				//lock-update
				$userm->LockoutCtrUpdate($mdata['name']);
				$userm->Lock($mdata['name']);
				$gSmarty->assign('error_msg',LOGIN_FAILED_ACCOUNT_IS_LOCKED);

			}
			else
			{
				$gSmarty->assign('error_msg',LOGIN_FAILED);
				//lock-update
				$userm->LockoutCtrUpdate($mdata['name']);
			}

		}
	}

	//reset
	unset($userm);
	$userm = null;
}
$gSmarty->display('index.tpl');
?>
