<?php
//include
include_once('includes/init.php');

/*
//chk
if(! is_logged_in())
{
    redir_page("index.php");
    exit;
}
*/

define ("INVALID_PASSWORD_MUST_BE_ALPHANUMERIC_AND_8CHARS_IN_LENGTH", "Password invalid, it should contain alphanumeric and 8 characters in length");
define ("PLEASE_CHECK_OLD_PASSWORD", "Invalid old password.");
define ("CHANGE_PASSWORD_FAILED_PLEASE_CHECK_NEW_PASSWORD_AND_CONFIRM_PASSWORD", "Sorry, passwords do not match");
define ("CHANGE_PASSWORD_FAILED", "Sorry, password was not successfully changed");
define ("CHANGE_PASSWORD_SUCCESS", "Password changed successfully!");
define ("CHANGE_PASSWORD_FAILED_USER_NOT_EXISTS", "Sorry, User does not exist");
define ("INVALID_PASSWORD_LENGTH", "Sorry, passowrd should be 8 characters in length");
define ("CHANGE_PASSWORD_FAILED_PLEASE_DONT_REUSE_PREVIOUS_5_CHANGED_PASSWORDS", "Sorry, you are not allowed to reuse previous 5 passwords.");
define ("CHANGE_PASSWORD_FAILED_PLEASE_CHECK_NEW_PASSWORD_IS_SAME_AS_OLD_PASSWORD", "Sorry, new password is same as old password.");


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
			$old_pass_enc  = md5($old_pass  .PWD_ENCRYPT_KEY);
			$new_pass1_enc = md5($new_pass1 .PWD_ENCRYPT_KEY);
			$new_pass2_enc = md5($new_pass2 .PWD_ENCRYPT_KEY);

			//chk len

			if(
					!@preg_match("/^[a-z0-9]{8}$/i", $new_pass1)  or
					!@preg_match("/^[a-z0-9]{8}$/i", $new_pass2) 
					//!@preg_match("/^[a-z0-9]{8}$/i", $old_pass )  
			  )
			{
			   echo "OOPS#1";
			   $gSmarty->assign('error_msg', INVALID_PASSWORD_MUST_BE_ALPHANUMERIC_AND_8CHARS_IN_LENGTH);
			   break;
			}
			
			if(
				! chk_pwd_valid($new_pass1) or
				! chk_pwd_valid($new_pass2) //or
				//! chk_pwd_valid($old_pass) 
			  )
			  {
			  	   echo "OOPS#2";
				   $gSmarty->assign('error_msg', INVALID_PASSWORD_MUST_BE_ALPHANUMERIC_AND_8CHARS_IN_LENGTH);
				   break;
			  }
			//chk old
			if(strcmp($old_pass_enc , $odata['bind_data']['password']))
			{
			   $gSmarty->assign('error_msg', PLEASE_CHECK_OLD_PASSWORD);
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
			//print_r($prevpass);
			if(is_array($prevpass) and @in_array($new_pass1_enc, $prevpass) )
			{
			   $gSmarty->assign('error_msg', CHANGE_PASSWORD_FAILED_PLEASE_DONT_REUSE_PREVIOUS_5_CHANGED_PASSWORDS);
			   break;
			}
			
			//update
			if(! $umaster->UpdatePassword($user,$new_pass1_enc) )
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
$gSmarty->display('change_user_pass.tpl');
?>

