<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Pragma" content="no-cache"> 
<meta http-equiv="no-cache"> 
<meta http-equiv="Expires" content="-1"> 
<meta http-equiv="Cache-Control" content="no-cache"> 

<title>Admin Page</title>
<link   href="css/styles.css"  rel="stylesheet" type="text/css" />

<!--//
<script type="text/JavaScript"   src="js/prototype.js"></script> 
<script type="text/javascript"   src="js/default.js">  </script>
//-->

<script type="text/javascript"   src="js/overlib_mini.js">  </script>
<script language="Javascript">
    var GB_ROOT_DIR = "{php} echo GREYBOX_ROOT_URL {/php}";
</script>
<script type="text/javascript" src="greybox/AJS.js"></script>
<script type="text/javascript" src="greybox/AJS_fx.js"></script>
<script type="text/javascript" src="greybox/gb_scripts.js"></script>
<script type="text/javascript" src="js/mootools-1.2.1.js"></script>
<link href="greybox/gb_styles.css" rel="stylesheet" type="text/css" />

<script language="Javascript">
/* DYNAMIC MENU HERE */
{php}
include_once("menu.php");
{/php}
</script>
</head>
<body>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
<!--Logo -->
    <div id="logo"><img src="images/globebroadband_head.jpg" alt="Company Logo here" width="100%" height="80" /></div>
	<div id="title"><div align="right">{php}echo $_SESSION["session.smartygold.profile.user"];{/php}</div></div>
