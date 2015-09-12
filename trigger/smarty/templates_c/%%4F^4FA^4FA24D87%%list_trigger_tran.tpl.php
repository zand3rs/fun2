<?php /* Smarty version 2.6.20, created on 2011-11-10 19:14:46
         compiled from list_trigger_tran.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'list_trigger_tran.tpl', 23, false),array('function', 'html_options', 'list_trigger_tran.tpl', 42, false),)), $this); ?>
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
    <form name="mainform" method="post" action="list_trigger_tran.php">

    
	<!--Pagetitle -->
	<div id="title">Trigger Transactions</div>

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
		<!--
		<td width="150" class="label">Customer Type</td>
		<td width="100" class="child">
			<select name="<?php echo $this->_tpl_vars['master_fields'][0]['name']; ?>
" <?php echo $this->_tpl_vars['master_fields'][0]['js']; ?>
 id="<?php echo $this->_tpl_vars['master_fields'][0]['name']; ?>
" >
			<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['master_fields'][0]['select_options'],'selected' => $this->_tpl_vars['master_fields'][0]['select_selected']), $this);?>

			</select>  
		</td>
		<td width="150" class="label">Status</td>
		<td width="150" class='label'>
			<select name="q_status" id="q_status">
				<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['soptstatus'],'selected' => 'ALL'), $this);?>

			</select>  
		</td>
		-->
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
		</tr>
	</table>
	</div>

	<!--Tablebody-->

	<div id="wrapper">

		<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>

		<table border="0" cellspacing="0" cellpadding="0" id="tablestyle" width="100%">
			<tr>	
                        <td width="30"  class="label">ID              </td>
                        <td width="100" class="label">Tran Date       </td>
                        <td width="50"  class="label">Tran ID.        </td>
                        <td width="50"  class="label">A_NO            </td>
                        <td width="50"  class="label">B_NO            </td>
                        <td width="100" class="label">Message         </td>
                        <td width="30"  class="label">Status          </td>
                        <td width="50"  class="label">Balance         </td>
                        <td width="50"  class="label">Breathing Period</td>
                        <td width="30"  class="label">Origin          </td>
			</tr>                         
           		<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
				<tr >
				<?php if ($this->_tpl_vars['ldata']['stat'] < 2): ?>
                                <td class="child" width="30"><a href="details_trigger_tran.php?uid=<?php echo get_uid(); ?>&tran_id=<?php echo $this->_tpl_vars['ldata']['tran_id']; ?>
" title="Retry Failed Transaction" rel="gb_page_center[<?php echo $this->_tpl_vars['popup_win_width']; ?>
, <?php echo $this->_tpl_vars['popup_win_height']; ?>
]"><?php echo $this->_tpl_vars['ldata']['tran_id']; ?>
</a></td>
                                <?php else: ?>
                                <td class="child" width="30" ><?php echo $this->_tpl_vars['ldata']['tran_id']; ?>
</td>
                                <?php endif; ?>
                                <td class="child" width="100"><?php echo $this->_tpl_vars['ldata']['dt_created']; ?>
</td>
                                <td class="child" width="50" ><?php echo $this->_tpl_vars['ldata']['txid']; ?>
</td>
                                <td class="child" width="50" ><?php echo $this->_tpl_vars['ldata']['a_no']; ?>
</td>
                                <td class="child" width="50" ><?php echo $this->_tpl_vars['ldata']['b_no']; ?>
</td>
                                <td class="child" width="100"><?php echo $this->_tpl_vars['ldata']['msg']; ?>
</td>
                                <td class="child" width="30" ><?php echo $this->_tpl_vars['ldata']['status']; ?>
</td>
                                <td class="child" width="50" ><?php echo $this->_tpl_vars['ldata']['min_bal']; ?>
</td>
                                <td class="child" width="50" ><?php echo $this->_tpl_vars['ldata']['breathing_period']; ?>
</td>
                                <td class="child" width="30" ><?php echo $this->_tpl_vars['ldata']['request_origin']; ?>
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