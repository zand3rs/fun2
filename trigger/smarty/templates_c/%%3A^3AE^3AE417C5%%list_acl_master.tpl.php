<?php /* Smarty version 2.6.20, created on 2010-12-08 14:43:57
         compiled from list_acl_master.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'list_acl_master.tpl', 16, false),array('function', 'html_options', 'list_acl_master.tpl', 35, false),)), $this); ?>
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
    <form name="mainform" method="get" action="list_acl_master.php">

    
	   <!--Pagetitle -->
	   <div id="title">Web ACL List</div>
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
			<!--Tablebody-->
	         <div id="wrapper">
				<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
				<tr>	
				<td width="150" class="label">Role</td>
				<!-- <td colspan="3" class="child"><input type="text" name="role_code" maxlength="10"/></td> -->
                      		<td colspan="3" class='child'>
					<select name="role_code" id="role_code">
						<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['srole_code'],'selected' => 0), $this);?>

					</select>  

				</td>
				</tr>
				<tr>	
				<td width="150" class="label">Module</td>
				<!-- <td colspan="3" class="child"><input type="text" name="mod_name" maxlength="10"/></td> -->
                      		<td colspan="3" class='child'>
					<select id='mod_name' name='mod_name'>
						<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['smod_code'],'selected' => 0), $this);?>

					</select>
				</td>
				</tr>
				<tr>	
				<td width="100" class="label">&nbsp;</td>
				<td colspan="3" class="child">
					<input type="submit" name="btnSubmit" value="Search" />
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
           	   <td width="200" class="label"><a  class="hhref" href="<?php echo $_SERVER[PHP_SELF]; ?>?role_code=<?php echo $_GET[role_code]; ?>&mod_name=<?php echo $_GET[mod_name]; ?>&<?php echo $this->_tpl_vars['href_sort_role']; ?>
">Role</a></td>
           	   <td width="200" class="label"><a  class="hhref" href="<?php echo $_SERVER[PHP_SELF]; ?>?role_code=<?php echo $_GET[role_code]; ?>&mod_name=<?php echo $_GET[mod_name]; ?>&<?php echo $this->_tpl_vars['href_sort_mod_name']; ?>
">Module</a></td>

           	   <td width="200" class="label">Can View</td>
           	   <td width="200" class="label">Can Add</td>
           	   <td width="200" class="label">Can Update</td>
           	   <td width="200" class="label">Can Delete</td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
           	<tr >
		   <td class="child"  width="200"><?php echo $this->_tpl_vars['ldata']['role_code']; ?>
</td>
		   <td class="child"  width="200">&nbsp;<?php echo $this->_tpl_vars['ldata']['mod_name']; ?>
</td>
		   <td class="child"  width="200">&nbsp;<?php if ($this->_tpl_vars['ldata']['can_view'] == 1): ?> Yes <?php else: ?> No <?php endif; ?></td>
		   <td class="child"  width="200">&nbsp;<?php if ($this->_tpl_vars['ldata']['can_add'] == 1): ?> Yes <?php else: ?> No <?php endif; ?></td>
		   <td class="child"  width="200">&nbsp;<?php if ($this->_tpl_vars['ldata']['can_update'] == 1): ?> Yes <?php else: ?> No <?php endif; ?></td>
		   <td class="child"  width="200">&nbsp;<?php if ($this->_tpl_vars['ldata']['can_delete'] == 1): ?> Yes <?php else: ?> No <?php endif; ?></td>
		   <td class="child"  width="200">
			<a href="update_acl_master.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_acl_master.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
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

           </table>
	</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			   <a href="add_acl_master.php?uid=<?php echo get_uid(); ?>"><img src="images/add.jpg"  border="0"></a>
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