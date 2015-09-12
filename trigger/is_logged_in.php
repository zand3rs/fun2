<?php
//islog ?
if(! is_logged_in())
{
    header("Location: index.php");
    exit;	
}
?>