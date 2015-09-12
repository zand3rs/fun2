<?php /* Smarty version 2.6.20, created on 2010-11-22 13:33:33
         compiled from list_user_master.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'list_user_master.tpl', 16, false),array('function', 'html_options', 'list_user_master.tpl', 44, false),)), $this); ?>
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
    <form name="mainform" method="get" action="list_user_master.php">

    
	   <!--Pagetitle -->
	   <div id="title">Web User List</div>
           <div id="desc">
		<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>
	                <?php echo ((is_array($_tmp=@$this->_tpl_vars['list_total'])) ? $this->_run_mod_handler('default', true, $_tmp, '0') : smarty_modifier_default($_tmp, '0')); ?>
 Result(s) found.&nbsp;
		<?php endif; ?>
	   </div>
	   
	      <!-- status msg -->
	    <?php if ($this->_tpl_vars['error_msg'] != ""): ?>
	    	    <div id="errorbox">
	    	    <br/>
	    	        <?php echo $this->_tpl_vars['error_msg']; ?>

	    	    <br/>
	    	    </div>
	    <?php endif; ?>
	    
	         <div id="wrapper">
				<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
				<tr>	
				<td width="150" class="label">Username</td>
				<td width="150" class="child"><input type="text" name="code" maxlength="20"/></td>
				<td width="150" class="label">Last Name</td>
				<td width="150" class="child"><input type="text" name="lname" maxlength="20"/></td>
                      		</td>
				</tr>
				<tr>	
				<td width="150" class="label">Email Address</td>
				<td width="150" class="child"><input type="text" name="email" maxlength="60"/></td>
				<td width="150" class="label">Role</td>
                      		<td width="150" class='child'>
					<select name="role_code" id="role_code">
						<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['srole_code'],'selected' => 0), $this);?>

					</select>  

				</td>
				</tr>
				<tr>	
				<td width="100" class="label">&nbsp;</td>
				<td colspan="3" class="child"><input type="submit" name="btnSubmit" value="Search" />
					&nbsp;
				<input type="reset" name="btnReset" value="Clear" /></td>
				</tr>
				</table>
			</div>
            <!--Tablebody-->
	<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		  <tr>	
			<td width="200" class="label"><a class="hhref" href="<?php echo $_SERVER[PHP_SELF]; ?>?code=<?php echo $_GET[code]; ?>&lname=<?php echo $_GET[lname]; ?>&email=<?php echo $_GET[email]; ?>&role_code=<?php echo $_GET[role_code]; ?>&<?php echo $this->_tpl_vars['href_sort_user']; ?>
">Username</a></td>
			<td width="200" class="label"><a class="hhref" href="<?php echo $_SERVER[PHP_SELF]; ?>?code=<?php echo $_GET[code]; ?>&lname=<?php echo $_GET[lname]; ?>&email=<?php echo $_GET[email]; ?>&role_code=<?php echo $_GET[role_code]; ?>&<?php echo $this->_tpl_vars['href_sort_name']; ?>
">Full Name</a></td>
			<td width="200" class="label"><a class="hhref" href="<?php echo $_SERVER[PHP_SELF]; ?>?code=<?php echo $_GET[code]; ?>&lname=<?php echo $_GET[lname]; ?>&email=<?php echo $_GET[email]; ?>&role_code=<?php echo $_GET[role_code]; ?>&<?php echo $this->_tpl_vars['href_sort_role']; ?>
">Role</a></td>
			<td width="200" class="label"><a class="hhref" href="<?php echo $_SERVER[PHP_SELF]; ?>?code=<?php echo $_GET[code]; ?>&lname=<?php echo $_GET[lname]; ?>&email=<?php echo $_GET[email]; ?>&role_code=<?php echo $_GET[role_code]; ?>&<?php echo $this->_tpl_vars['href_sort_email']; ?>
">Email</a></td>
			<td width="200" class="label"><a class="hhref" href="<?php echo $_SERVER[PHP_SELF]; ?>?code=<?php echo $_GET[code]; ?>&lname=<?php echo $_GET[lname]; ?>&email=<?php echo $_GET[email]; ?>&role_code=<?php echo $_GET[role_code]; ?>&<?php echo $this->_tpl_vars['href_sort_position']; ?>
">Position</a></td>
			<td width="200" class="label"><a class="hhref" href="<?php echo $_SERVER[PHP_SELF]; ?>?code=<?php echo $_GET[code]; ?>&lname=<?php echo $_GET[lname]; ?>&email=<?php echo $_GET[email]; ?>&role_code=<?php echo $_GET[role_code]; ?>&<?php echo $this->_tpl_vars['href_sort_expiry']; ?>
">Password Expiry</a></td>

			<td width="200" class="label">Action</td>
           	</tr>
           	<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
           	<tr >
		   <td class="child" width="200"><a href="details_user_master.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['user_id']; ?>
" title="View User Info" rel="gb_page_center[<?php echo $this->_tpl_vars['popup_win_width']; ?>
, <?php echo $this->_tpl_vars['popup_win_height']; ?>
]"><?php echo $this->_tpl_vars['ldata']['user_code']; ?>
</a></td>
		   <td class="child" width="200">&nbsp;<?php echo $this->_tpl_vars['ldata']['last_name']; ?>
,&nbsp;<?php echo $this->_tpl_vars['ldata']['first_name']; ?>
</td>
		   <td class="child" width="200">&nbsp;<?php echo $this->_tpl_vars['ldata']['role_code']; ?>
</td>
		   <td class="child" width="200">&nbsp;<?php echo $this->_tpl_vars['ldata']['email']; ?>
</td>
		   <td class="child" width="200">&nbsp;<?php echo $this->_tpl_vars['ldata']['position']; ?>
</td>
		   <td class="child" width="200">&nbsp;<?php if ($this->_tpl_vars['ldata']['pass_exp_n_days'] == 0): ?>
			    Never Expires
			 <?php elseif ($this->_tpl_vars['ldata']['pass_exp_n_days'] == 7): ?>
			    1 Week
			 <?php elseif ($this->_tpl_vars['ldata']['pass_exp_n_days'] == 30): ?>
			    1 Month
			 <?php else: ?> 	
			    <?php echo $this->_tpl_vars['ldata']['pass_exp_n_days']; ?>
 days
			 <?php endif; ?>
		   </td>
		   <td class="child"  width="200">
			<a href="update_user_master.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['user_id']; ?>
"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_user_master.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['user_id']; ?>
"><img src="images/delete.jpg"  border="0"></a>
		   </td>
	   	</tr>
           	<?php endforeach; endif; unset($_from); ?>
           	<tr>
		   <td colspan=20 align="left" class="child">
		   <?php echo ((is_array($_tmp=@$this->_tpl_vars['list_nav'])) ? $this->_run_mod_handler('default', true, $_tmp, "") : smarty_modifier_default($_tmp, "")); ?>
&nbsp;
		   </td>
           	</tr>

                        <?php if ($this->_tpl_vars['list_total'] > '0'): ?>
                        <tr>
                        <td colspan=20 align="left" class="child">
                        <a href="dl.php?muid=<?php echo get_uid(); ?>&csv=<?php echo $this->_tpl_vars['q_csv']; ?>
">Download CSV</a>
                        </td>
                        </tr>
                        <?php endif; ?>

           </table>
	</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			   <a href="add_user_master.php?uid=<?php echo get_uid(); ?>"><img src="images/add.jpg"  border="0"></a>
			   </td>
		          </tr>
		        </table>
		      </div>
		<!--buttons-->
	<?php endif; ?>

    
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