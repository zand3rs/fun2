<?php /* Smarty version 2.6.20, created on 2010-01-29 16:25:53
         compiled from list_blacklisted.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'list_blacklisted.tpl', 25, false),)), $this); ?>
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
    <form name="mainform" method="post" action="list_blacklisted.php">

    
	<!--Pagetitle -->
	<div id="title">Blacklisted Subscribers</div>

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
		<td width="150" class="label">Start Date <br>(YYYY-MM-DD)</td>
		<td width="100" class="child"><input type="text" name="q_sdate" maxlength="10" value="<?php echo $this->_tpl_vars['q_sdate']; ?>
"/></td>
		<td width="150" class="label">End Date <br>(YYYY-MM-DD)</td>
		<td width="100" class="child"><input type="text" name="q_edate" maxlength="10" value="<?php echo $this->_tpl_vars['q_edate']; ?>
"/></td>
		</tr>
		<tr>	
		<td width="150" class="label">MSISDN <br>(Ex. 63917xxxxxxx)</td>
		<td class="child"><input type="text" name="q_msisdn" maxlength="20" value="<?php echo $this->_tpl_vars['q_msisdn']; ?>
"/></td>
                <td width="150" class="label">&nbsp</td>
                <td class="child">&nbsp</td>
		</tr>
		<tr>
		<td width="100" class="label">&nbsp;</td>
		<td colspan="3" class="child">
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
		</tr>
	</table>
	</div>

	<!--Tablebody-->

	<div id="wrapper">

	<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>

		<table border="0" cellspacing="0" cellpadding="0" id="tablestyle" width="100%">
			<tr>	
           	   <td width="50"  class="label">MSISDN</td>
			<td width="300" class="label">Remarks</td>
			<td width="100" class="label">Date Created</td>
			<td width="100" class="label">Created By</td>
			</tr>
           		<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
				<tr >
				<td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['msisdn']; ?>
</td>
				<td class="child" width="300">&nbsp;<?php echo $this->_tpl_vars['ldata']['remarks']; ?>
</td>
				<td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['dt_created']; ?>
</td>
				<td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['created_by']; ?>
</td>
				</tr>
           		<?php endforeach; endif; unset($_from); ?>
                	
			<tr>
			<td colspan=20 align="left" class="child">
			<?php echo ((is_array($_tmp=@$this->_tpl_vars['list_nav'])) ? $this->_run_mod_handler('default', true, $_tmp, "") : smarty_modifier_default($_tmp, "")); ?>
&nbsp;
			</td>
           		</tr>
                	
			<?php if ($this->_tpl_vars['list_total'] == ".1"): ?>
			<tr>
			<td colspan=20 align="left" class="child">
			<a href="dl.php?muid=<?php echo get_uid(); ?>&csv=<?php echo $this->_tpl_vars['q_csv']; ?>
">Download CSV</a>
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