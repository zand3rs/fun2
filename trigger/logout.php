<?php
//include
include_once('includes/init.php');

//chk
$userm      =& new User();
  
//upd8
$userm->LogoutUpdate($_SESSION["session.smartygold.profile.uid"]);

//islog ?
logout_session();

header("Location: index.php");
?>
