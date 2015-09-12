<?php

include_once('includes/init.php');




echo "\n\nPWD=".(md5($_GET['pass'].PWD_ENCRYPT_KEY)) ."\n";




?>

