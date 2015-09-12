<?php /* Smarty version 2.6.20, created on 2010-06-18 15:37:24
         compiled from list_keyword_msgs.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'list_keyword_msgs.tpl', 26, false),array('function', 'html_options', 'list_keyword_msgs.tpl', 39, false),)), $this); ?>
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
    <form name="mainform" method="get" action="list_keyword_msgs.php">

    
	   <!--Pagetitle -->
	   <div id="title">Message List</div>

	   <!-- status msg -->
	   <?php if ($this->_tpl_vars['error_msg'] != ""): ?>
	    	<div id="errorbox">
	    	<br/>
	    	    <?php echo $this->_tpl_vars['error_msg']; ?>

	    	    <br/>
	    	</div>
	   <?php else: ?>
            	<div id="desc">
	    	<?php if ($this->_tpl_vars['error_msg'] != 'DELETE_MESSAGE_SUCCESS'): ?>
			<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>
	    	            <?php echo ((is_array($_tmp=@$this->_tpl_vars['list_total'])) ? $this->_run_mod_handler('default', true, $_tmp, '0') : smarty_modifier_default($_tmp, '0')); ?>
 Result(s) found.&nbsp;
			<?php endif; ?>
	    	<?php endif; ?>
	    	</div>
	   <?php endif; ?>

	         <div id="wrapper">
				<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
				<tr>	
				<td width="150" class="label">Keyword</td>
				<!-- td width="150" class="child"><input type="text" name="scode_l" maxlength="10"/></td -->
                      		<td width="150" class='child'>
					<select name="scode_l" id="scode_l">
						<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['soptkeyword'],'selected' => $this->_tpl_vars['q_scode_l']), $this);?>

					</select>  
				</td>
				<td width="150" class="label">Sub-Keyword</td>
				<!-- td width="150" class="child"><input type="text" name="ssubcode" maxlength="10"/></td -->
                      		<td width="150" class='child'>
					<select name="ssubcode" id="ssubcode">
						<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['soptsubkeyword'],'selected' => $this->_tpl_vars['q_ssubcode']), $this);?>

					</select>  
				</td>
				</tr>
				<tr>
				<td width="150" class="label">Customer Type</td>
                      		<td width="150" class='child'>
					<select name="scust" id="scust">
						<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['soptcustomer'],'selected' => $this->_tpl_vars['q_scust']), $this);?>

					</select>  
				</td>
                                <td width="150" class="label">Status</td>
                                <td width="150" class='child'>
                                        <select name="sstat" id="sstat">
                                                <?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['soptstatus'],'selected' => $this->_tpl_vars['q_sstat']), $this);?>

                                        </select>

                                </td>
				</tr>
				<tr>
                                <td width="150" class="label">Message ID</td>
                                <td colspan="3" class="child"><input type="text" name="smesg" maxlength="60"  value="<?php echo $this->_tpl_vars['q_smesg']; ?>
" style="width:500px"/></td>
				</tr>

				<tr>	
				<td width="150" class="label">Message</td>
				<!-- td colspan="3" class="child"><input type="text" name="sname" maxlength="60" style="width:500px"/></td -->
				<td colspan="3" class="child"><textarea rows = "4" cols="60" name="sname"><?php echo $this->_tpl_vars['q_sname']; ?>
</textarea></td>
				</tr>
				<tr>	
				<td width="150" class="label">&nbsp;</td>
				<td width="150" class="child"><input type="checkbox" value="1" name="sfile"/>Generate CSV FILE</td>
				<td width="150" class="label">&nbsp;</td>
				<td width="150" class="child">
					<input type="submit" name="btnSubmit" value="Search" />
					&nbsp;
					<?php if ($this->_tpl_vars['error_msg'] != ""): ?>
						<input type="button" name="btnReset" value="Clear" onClick="clearOnListErrForm();"/>
                                        <?php elseif ($this->_tpl_vars['is_searched'] == '1'): ?>
                                                <input type="button" name="btnReset" value="Clear" onClick="clearOnSearchForm();"/></td>
					<?php else: ?>
						<input type="reset" name="btnReset" value="Clear" />
					<?php endif; ?>
				</td>
				</tr>
				</table>
			</div>

            <!--Tablebody-->
	<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>
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
           	   <td width="70"  class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_code']; ?>
&scode_l=<?php echo $this->_tpl_vars['q_scode']; ?>
&ssubcode=<?php echo $this->_tpl_vars['q_ssubcode']; ?>
&scust=<?php echo $this->_tpl_vars['q_scust']; ?>
&smesg=<?php echo $this->_tpl_vars['q_smesg']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Keyword</a></td>
           	   <td width="70"  class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_name']; ?>
&scode_l=<?php echo $this->_tpl_vars['q_scode']; ?>
&ssubcode=<?php echo $this->_tpl_vars['q_ssubcode']; ?>
&scust=<?php echo $this->_tpl_vars['q_scust']; ?>
&smesg=<?php echo $this->_tpl_vars['q_smesg']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Sub-Keyword</a></td>
           	   <td width="100"  class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_mesg']; ?>
&scode_l=<?php echo $this->_tpl_vars['q_scode']; ?>
&ssubcode=<?php echo $this->_tpl_vars['q_ssubcode']; ?>
&scust=<?php echo $this->_tpl_vars['q_scust']; ?>
&smesg=<?php echo $this->_tpl_vars['q_smesg']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Message ID</td>
           	   <!-- td width="70"  class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_cust']; ?>
">Customer Type</a></td -->
           	   <!-- td width="30"  class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_mesg']; ?>
">Message Type</a></td -->
           	   <td width="100" class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_desc']; ?>
&scode_l=<?php echo $this->_tpl_vars['q_scode']; ?>
&ssubcode=<?php echo $this->_tpl_vars['q_ssubcode']; ?>
&scust=<?php echo $this->_tpl_vars['q_scust']; ?>
&smesg=<?php echo $this->_tpl_vars['q_smesg']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Description</a></td>
           	   <td width="400" class="label">Message</td>
           	   <td width="50"  class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_status']; ?>
&scode_l=<?php echo $this->_tpl_vars['q_scode']; ?>
&ssubcode=<?php echo $this->_tpl_vars['q_ssubcode']; ?>
&scust=<?php echo $this->_tpl_vars['q_scust']; ?>
&smesg=<?php echo $this->_tpl_vars['q_smesg']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Status</a></td>
           	   <td width="70" class="label">Action</td>
           	</tr>
           	<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
           	<tr >
		   <td class="child" width="70">&nbsp;<?php echo $this->_tpl_vars['ldata']['keyword']; ?>
       </td>
		   <td class="child" width="70">&nbsp;<?php echo $this->_tpl_vars['ldata']['sub_keyword']; ?>
   </td>
		   <td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['message_id_disp']; ?>
</td>
		   <!-- td class="child" width="70">&nbsp;<?php echo $this->_tpl_vars['ldata']['customer_type']; ?>
 </td -->
		   <!-- td class="child" width="30" >&nbsp;<?php echo $this->_tpl_vars['ldata']['message_type']; ?>
 </td -->
		   <td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['description']; ?>
  </td>
		   <td class="child" width="400">&nbsp;<?php echo $this->_tpl_vars['ldata']['msg']; ?>
          </td>
		   <td class="child" width="50">&nbsp;<?php echo $this->_tpl_vars['ldata']['status']; ?>
        </td>
		   <td class="child" width="70">
			<a href="update_keyword_msg.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
"><img src="images/update.jpg"  border="0"></a>
<!--
			&nbsp;
			<a href="delete_keyword_msg.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
"><img src="images/delete.jpg"  border="0"></a>
-->
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
			   <a href="add_keyword_msg.php?uid=<?php echo get_uid(); ?>"><img src="images/add.jpg"  border="0"></a>
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