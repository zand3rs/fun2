<?php /* Smarty version 2.6.20, created on 2010-12-04 18:53:55
         compiled from manual_cancel.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('function', 'html_options', 'manual_cancel.tpl', 46, false),)), $this); ?>
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
    <form name="mainform" method="post" action="manual_cancel.php">

	<!--Pagetitle -->
	<div id="title">Manual FUN Deactivation</div>
		<?php if (( $this->_tpl_vars['is_searched'] == '1' ) && ( $this->_tpl_vars['error_msg'] == 'SAVING_CANCEL_SUCCESS' )): ?>
			<div id="errorbox">
			<br/>
			CANCELLATION of service successfully entered...
			<br/>
			</div>
		<?php else: ?>
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
                                <td width="150" class="label">OFW SIM<br>(Ex. 63917xxxxxxx)</td>
                                <td class="child"><input type="text" name="q_msisdn" maxlength="20"  /></td>
                                </tr>
                                <tr>
                                <td width="150" class="label">LOCAL SIM <br>(Ex. 63917xxxxxxx)</td>
                                <td class="child"><input type="text" name="q_linkto" maxlength="20"  /></td>
                                </tr>

<!--
				<tr>
				<td width="150" class="label">Profile</td>
			                   		<td width="150" class='label'>
					<select name="q_service" id="q_profile">
						<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['soptcustomer'],'selected' => '%'), $this);?>

					</select>  
				</td>
				</tr>	
-->
                                <tr>
                                <td width="150" class="label">Remarks</td>
				<td class="child"><textarea rows="2" cols="60" name="q_remarks"></textarea>
                                </tr>
				<tr>	
				<td width="100" class="label">&nbsp;</td>
				<td colspan="3" class="child">
					<input type="submit" name="btnSubmit" value="Save" />
					&nbsp;
					<!-- input type="reset" name="btnReset" value="Clear" /></td -->
		           		<input type="reset"  name="btnCancel" value="Clear" onclick="clearOnUpdateForm();"/>
				</tr>
			</table>
			</div>
        		
			<!--Tablebody-->

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