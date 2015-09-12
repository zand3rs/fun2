<?php
//include
include_once('includes/init.php');


//chk
$is_logged_in = is_logged_in();
if(! $is_logged_in)
{
    redir_page("index.php");
    exit;
}

//allowed?
include_once('is_allowed.php');


//error msg
define('INVALID_PASSWORD_MUST_BE_ALPHANUMERIC_AND_8CHARS_IN_LENGTH', "Password Change failed. Password should be alphanumeric.");
define('NEW_PASSWORD_IS_BLANK', "Password Change Failed. New password should not be blank or white space (spaces, tabs, etc.)");
define('CHANGE_PASSWORD_FAILED_PLEASE_DONT_REUSE_PREVIOUS_5_CHANGED_PASSWORDS', "Password Change Failed. You are not allowed to use your recent 5 previous passwords.");
define('CHANGE_PASSWORD_FAILED_PLEASE_CHECK_NEW_PASSWORD_IS_SAME_AS_OLD_PASSWORD', "Password Change Failed. Your new password is same with your old password.");
//new
$umaster =& new User();

//chk submit
if(strtoupper($_POST['btnSubmit']) == "CHANGE")
{
	while(1)
	{
		//valid
		$user      = trim($_POST['user']);
		$old_pass  = trim($_POST['old_pass']);
		$new_pass1 = trim($_POST['new_pass1']);
		$new_pass2 = trim($_POST['new_pass2']);
		//get
		$odata     = $umaster->Open($user);
		if(! $odata or null == $odata or $odata['bind_total']<=0)
		{
			$gSmarty->assign('error_msg', CHANGE_PASSWORD_FAILED_USER_NOT_EXISTS);
			break;
		}
		else
		{
			$old_pass_enc  = $odata['bind_data']['password'];
			$new_pass1_enc = md5($new_pass1 .PWD_ENCRYPT_KEY);
			$new_pass2_enc = md5($new_pass2 .PWD_ENCRYPT_KEY);

			//chk len

			if(!strlen($new_pass1) or !strlen($new_pass2))
			{
				$gSmarty->assign('error_msg', NEW_PASSWORD_IS_BLANK);
				break;
			}
			if(
					!@preg_match("/^[a-z0-9]{8}$/i", $new_pass1)  or
					!@preg_match("/^[a-z0-9]{8}$/i", $new_pass2)  
			  )
			{
			   $gSmarty->assign('error_msg', INVALID_PASSWORD_MUST_BE_ALPHANUMERIC_AND_8CHARS_IN_LENGTH);
			   break;
			}
			
			if(
			! chk_pwd_valid($new_pass1) or
			! chk_pwd_valid($new_pass2) 
			  )
			  {
				   $gSmarty->assign('error_msg', INVALID_PASSWORD_MUST_BE_ALPHANUMERIC_AND_8CHARS_IN_LENGTH);
				   break;
			  }
			
			//chk old
			if(strcmp($new_pass1_enc , $new_pass2_enc))
			{
			   $gSmarty->assign('error_msg', CHANGE_PASSWORD_FAILED_PLEASE_CHECK_NEW_PASSWORD_AND_CONFIRM_PASSWORD);
			   break;
			}
			//chk if same passwd used
			if(!strcmp($new_pass1_enc , $old_pass_enc))
			{
			   $gSmarty->assign('error_msg', CHANGE_PASSWORD_FAILED_PLEASE_CHECK_NEW_PASSWORD_IS_SAME_AS_OLD_PASSWORD);
			   break;
			}
			//chk if previously used
			$prevpass = $umaster->GetOldPasswords($user);
			if(is_array($prevpass) and @in_array($new_pass1_enc, $prevpass) )
			{
			   $gSmarty->assign('error_msg', CHANGE_PASSWORD_FAILED_PLEASE_DONT_REUSE_PREVIOUS_5_CHANGED_PASSWORDS);
			   break;
			}
			
			//update
			if(! $umaster->ResetPassword($user,$new_pass1_enc) )
			{
			   $gSmarty->assign('error_msg', CHANGE_PASSWORD_FAILED);
			   break;
			}
			
			//log old-pass
			$umaster->SavePasswordLog($user,$new_pass1_enc);
			
			//close window
			$gSmarty->assign('close_window',          1 );
			$gSmarty->assign('error_msg', CHANGE_PASSWORD_SUCCESS);
			
			break;
			
		}
		break;//last
	}
}
//show
$gSmarty->assign('is_log_in',          is_logged_in() );
$gSmarty->display('reset_user_pass.tpl');
?>

