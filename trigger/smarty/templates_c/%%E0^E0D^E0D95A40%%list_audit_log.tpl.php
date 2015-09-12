<?php /* Smarty version 2.6.20, created on 2010-05-09 12:48:38
         compiled from list_audit_log.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'list_audit_log.tpl', 22, false),array('function', 'html_options', 'list_audit_log.tpl', 42, false),)), $this); ?>
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
<form name="mainform" method="post" action="list_audit_log.php">

	<!--Pagetitle -->
	<div id="title">Audit Log</div>
	   
	<!-- status msg -->
	<?php if ($this->_tpl_vars['error_msg'] != ""): ?>
		<div id="errorbox">
		<br/>
		<?php echo $this->_tpl_vars['error_msg']; ?>

		<br/>
		</div>
	<?php else: ?>
		<div id="desc">
		<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>
			<?php echo ((is_array($_tmp=@$this->_tpl_vars['list_total'])) ? $this->_run_mod_handler('default', true, $_tmp, '0') : smarty_modifier_default($_tmp, '0')); ?>
 Result(s) found.&nbsp;
		<?php endif; ?>
		</div>
	<?php endif; ?>
	    
	<!--Tablebody-->
	<div id="wrapper">
	<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		<tr>	
		<td width="150" class="label">Start Date<br>(YYYY-MM-DD)</td>
		<td width="150" class="child"><input type="text" name="q_sdate" maxlength="10" value="<?php echo $this->_tpl_vars['q_sdate']; ?>
"/></td>
		<td width="150" class="label">End Date<br>(YYYY-MM-DD)</td>
		<td width="150" class="child"><input type="text" name="q_edate" maxlength="10" value="<?php echo $this->_tpl_vars['q_edate']; ?>
"/></td>
		</tr>
		<tr>
		<td width="150" class="label">User Code</td>
		<td width="150" class="child"><input type="text" name="q_user_code" maxlength="20" value="<?php echo $this->_tpl_vars['q_user_code']; ?>
"/></td>
		<td width="150" class="label">Role Code</td>
		<td width="150" class="child">
			<select name="<?php echo $this->_tpl_vars['master_fields'][1]['name']; ?>
" <?php echo $this->_tpl_vars['master_fields'][1]['js']; ?>
 id="<?php echo $this->_tpl_vars['master_fields'][1]['name']; ?>
" >
				<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['master_fields'][1]['select_options'],'selected' => $this->_tpl_vars['master_fields'][1]['select_selected']), $this);?>

			</select>  
		</td>
		</tr>
		<tr>	
		<td width="150" class="label">Module Name</td>
		<td width="150" class="child">
			<select name="<?php echo $this->_tpl_vars['master_fields'][0]['name']; ?>
" <?php echo $this->_tpl_vars['master_fields'][0]['js']; ?>
 id="<?php echo $this->_tpl_vars['master_fields'][0]['name']; ?>
" >
			<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['master_fields'][0]['select_options'],'selected' => $this->_tpl_vars['master_fields'][0]['select_selected']), $this);?>

			</select>  
		</td>
		<td width="150" class="label">&nbsp;</td>
		<td width="150" class="child"><input type="checkbox" value="1" name="sfile"/>Generate CSV FILE</td>
		</tr>
		<tr>	
		<td width="150" class="label">&nbsp;</td>
		<td colspan=3 class="child">
                        <input type="submit" name="btnSubmit" value="Search" />
                        &nbsp;
                        <?php if ($this->_tpl_vars['error_msg'] != ""): ?>
                                <input type="button" name="btnReset" value="Clear" onClick="clearOnListErrForm();"/></td>
                        <?php elseif ($this->_tpl_vars['is_searched'] == '1'): ?>
                                <input type="button" name="btnReset" value="Clear" onClick="clearOnSearchForm();"/></td>
                        <?php else: ?>
                                <input type="reset" name="btnReset" value="Clear"/></td>
                        <?php endif; ?>
		</td>
		</td>
		</tr>
	</table>
	</div>
	<!--Tablebody-->

	<div id="wrapper">
	<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>
	<table border="0" cellspacing="0" cellpadding="0" id="tablestyle" width="100%">
	<?php if ($this->_tpl_vars['q_csv'] <> ""): ?>
		<tr>
		<td colspan=20 align="left" class="child">
		<a href="dl.php?muid=<?php echo get_uid(); ?>&csv=<?php echo $this->_tpl_vars['q_csv']; ?>
">Download CSV</a>
		</td>
		</tr>
	<?php else: ?>
		<tr>	
			<td width="100" class="label">Tx Date</td>
			<td width="50"  class="label">User</td>
			<td width="50"  class="label">Module</td>
			<td width="50"  class="label">IP Address</td>
			<td width="50"  class="label">MSISDN</td>
			<td width="300" class="label">Message</td>
	   	</tr>
           	<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
			<tr>
			<td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['start_tm']; ?>
</td>
			<td class="child" width="50" >&nbsp;<?php echo $this->_tpl_vars['ldata']['user_code']; ?>
</td>
			<td class="child" width="50" >&nbsp;<?php echo $this->_tpl_vars['ldata']['mod_name']; ?>
</td>
			<td class="child" width="50" >&nbsp;<?php echo $this->_tpl_vars['ldata']['ip_addr']; ?>
</td>
			<td class="child" width="50" >&nbsp;<?php echo $this->_tpl_vars['ldata']['msisdn']; ?>
</td>
			<td class="child" width="300">&nbsp;<?php echo $this->_tpl_vars['ldata']['message']; ?>
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
	<?php endif; ?>
	</div>
</form>
<!-- form -->

    
<!-- CONTENT Ends  -->

<!-- footer -->
<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "footer.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>