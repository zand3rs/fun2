<?php /* Smarty version 2.6.20, created on 2010-12-08 14:46:55
         compiled from list_hlrsrvr.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'list_hlrsrvr.tpl', 16, false),array('function', 'html_options', 'list_hlrsrvr.tpl', 40, false),)), $this); ?>
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
    <form name="mainform" method="get" action="list_hlrsrvr.php">

    
	   <!--Pagetitle -->
	   <div id="title">HLR Server List</div>
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
				<td width="150" class="label">Server IP</td>
				<td width="150" class="child"><input type="text" name="scode" maxlength="30" value="<?php echo $this->_tpl_vars['q_scode']; ?>
"/></td>
				<td width="150" class="label">Server Name</td>
				<td width="150" class="child"><input type="text" name="sname" maxlength="60" style="width:200px" value="<?php echo $this->_tpl_vars['q_sname']; ?>
"/></td>
				</tr>
				<tr>
				<td width="150" class="label">Status</td>
                      		<td width="150" class='child'>
					<select name="sstat" id="sstat">
						<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['soptstatus'],'selected' => $this->_tpl_vars['q_sstat']), $this);?>

					</select>  
				</td>
				<td width="150" class="label">&nbsp;</td>
				<td width="150" class="child"><input type="checkbox" value="1" name="sfile"/>Generate CSV FILE</td>
				</tr>
				<tr>
				<td width="150" class="label">&nbsp;</td>
				<td colspan="3" class="child">
					<input type="submit" name="btnSubmit" value="Search" />
					&nbsp;
                                        <?php if ($this->_tpl_vars['error_msg'] != ""): ?>
                                                <input type="button" name="btnReset" value="Clear" onClick="clearOnListErrForm();"/>
                                        <?php elseif ($this->_tpl_vars['is_searched'] == '1'): ?>
                                                <input type="button" name="btnReset" value="Clear" onClick="clearOnSearchForm();"/></td>
                                        <?php else: ?>
                                                <input type="reset" name="btnReset" value="Clear" />
                                        <?php endif; ?>
				</tr>
				</table>
			</div>

	<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>
            <!--Tablebody-->
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		<?php if ($this->_tpl_vars['q_csv'] <> ""): ?>
		<tr>
		<td colspan=20 align="left" class="child">
		<a href="dl.php?muid=<?php echo get_uid(); ?>&csv=<?php echo $this->_tpl_vars['q_csv']; ?>
">Download CSV</a>
		</td>
		</tr>
		<?php else: ?>
		  <tr>	
           	   <td width="100" class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_name']; ?>
&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Server Name</a></td>
           	   <td width="75"  class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_code']; ?>
&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Server IP</a></td>
           	   <td width="50"  class="label">Port</td>
           	   <td width="75"  class="label">Username</td>
           	   <td width="75"  class="label">Password</td>
            	   <td width="50"  class="label">Timeout (ms)</td>
           	   <td width="50"  class="label">Retry</td>
           	   <td width="100" class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_status']; ?>
&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Status</a></td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
           	<tr >
		   <td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['server_name']; ?>
 </td>
		   <td class="child" width="75" >&nbsp;<?php echo $this->_tpl_vars['ldata']['server_ip']; ?>
   </td>
		   <td class="child" width="50" >&nbsp;<?php echo $this->_tpl_vars['ldata']['server_port']; ?>
 </td>
		   <td class="child" width="75" >&nbsp;<?php echo $this->_tpl_vars['ldata']['username']; ?>
    </td>
		   <td class="child" width="75" >&nbsp;<?php echo $this->_tpl_vars['ldata']['password']; ?>
    </td>
		   <td class="child" width="50" >&nbsp;<?php echo $this->_tpl_vars['ldata']['timeout']; ?>
     </td>
		   <td class="child" width="50" >&nbsp;<?php echo $this->_tpl_vars['ldata']['retry_cnt']; ?>
   </td>
		   <td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['status']; ?>
      </td>
		   <td class="child" width="200">
			<a href="update_hlrsrvr.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_hlrsrvr.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
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

		<?php endif; ?>

           	</table>
		</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			   <a href="add_hlrsrvr.php?uid=<?php echo get_uid(); ?>"><img src="images/add.jpg"  border="0"></a>
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