<?php /* Smarty version 2.6.20, created on 2010-12-04 18:53:03
         compiled from index.tpl */ ?>
<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "header.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "menu.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>



<!-- form -->
<form name="mainform" method="post" action="login.php">


<!--Pagetitle -->

<?php if ($this->_tpl_vars['is_log_in'] == ""): ?>
<div id="title">Login</div>

<!-- status msg -->
<?php if ($this->_tpl_vars['error_msg'] != ""): ?>
<div id="errorbox">
<br/>
<?php echo $this->_tpl_vars['error_msg']; ?>

<br/>
</div>
<?php endif; ?>
<!--Tablebody-->
<div id="wrapper">
<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">


<tr>
<td class="label">Username</td>
<td class="child"><input type="text" name="user" value=""/></td>
</tr>
<tr>
<td class="label">Password</td>
<td class="child"><input type="password" name="pass" value=""/></td>
</tr>
<tr>
<td class="label">&nbsp;</td>
<td class="child"><input type="submit" name="btnSubmit" value="Login"/></td>
</tr>

<?php if (( $this->_tpl_vars['error_msg'] == "Sorry, your password already expired." ) || ( $this->_tpl_vars['error_msg'] == "Sorry, you need to change your password. Click Change Password link below." )): ?>
<tr>
<td class="label">&nbsp;</td>
<td class="child">
		<a href="change_user_pass.php?uid=<?php echo get_uid(); ?>" title="Admin Page" rel="gb_page_center[<?php echo $this->_tpl_vars['popup_win_width']; ?>
, <?php echo $this->_tpl_vars['popup_win_height']; ?>
]">Change Password</a>

</td>
</tr>
<?php endif; ?>



</table>


<?php else: ?>
<div id="title">Home</div>
<!-- status msg -->
<?php if ($this->_tpl_vars['error_msg'] != ""): ?>
<div id="errorbox">
<br/>
<?php echo $this->_tpl_vars['error_msg']; ?>

<br/>
</div>
<?php endif; ?>
<div style="height:420px">
<br/><br/><br/><br/>
Welcome, <?php echo $_SESSION["session.smartygold.profile.user"]; ?>, you are now logged in!
		<br/><br/><br/><br/>
		<a class="hhref" href="change_user_pass.php?uid=<?php echo get_uid(); ?>" title="Admin Page" rel="gb_page_center[<?php echo $this->_tpl_vars['popup_win_width']; ?>
, <?php echo $this->_tpl_vars['popup_win_height']; ?>
]">Change Password</a>
		</div>


<?php endif; ?>


</div>


</form>
<!-- form -->


</div>
<!-- CONTENT Ends  -->

<!-- footer -->
<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "footer.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>