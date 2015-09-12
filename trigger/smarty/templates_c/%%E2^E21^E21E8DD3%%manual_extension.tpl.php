<?php /* Smarty version 2.6.20, created on 2010-01-29 13:11:21
         compiled from manual_extension.tpl */ ?>
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
    <form name="mainform" method="post" action="manual_extension.php">

    
	<!--Pagetitle -->
	<div id="title">Manual Extension</div>
		<?php if (( $this->_tpl_vars['is_searched'] == '1' ) && ( $this->_tpl_vars['error_msg'] == 'SAVING_EXTENSION_SUCCESS' )): ?>
			<div id="errorbox">
			<br/>
			EXTENSION successfully entered...
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
				<td width="150" class="label">MSISDN <br>(Ex. 63917xxxxxxx)</td>
				<td class="child"><input type="text" name="q_msisdn" maxlength="20"  /></td>
				</tr>
				<tr>	
				<td width="150" class="label">Duration</td>
				<td class="child"><input type="text" name="q_duration" maxlength="60" style='width:50px'/></td>
				</tr>	
			<tr>	
				<td width="150" class="label">Representative's Name</td>
				<td class="child"><input type="text" name="q_name" maxlength="60" style="width:400px" /></td>
				</tr>	
				<tr>	
				<td width="150" class="label">Contact Number</td>
				<td class="child"><input type="text" name="q_contact" maxlength="60" style="width:200px"/></td>
				</tr>	
                                <tr>
                                <td width="150" class="label">Override Balance Check?</td>
                                <td class="child"><input type='checkbox' name="q_balchk" value='0' style='display:block'/>
                                </td>
                                </tr>
                                <tr>
                                <td width="150" class="label">Override Breathing Period?</td>
                                <td class="child"><input type='checkbox' name="q_bperiod" value='0' style='display:block'/>
                                </td>
                                </tr>
					<tr>	
				<td width="150" class="label">Remarks</td>
                                <td class="child"><textarea rows="2" cols="60" name="q_remarks"></textarea>
				</tr>	
				<tr>	
				<td width="100" class="label">&nbsp;</td>
				<td colspan="3" class="child">
					<input type="submit" name="btnSubmit" value="Extend" />
					&nbsp;
					<!-- input type="reset" name="btnReset" value="Clear" /></td -->
		           		<input type="reset"  name="btnCancel" value="Clear" onclick="clearOnUpdateForm();"/>
				</tr>
			</table>
			</div>
        		
			<!--Tablebody-->

	<?php endif; ?>
   
    </form>
    <!-- form -->

    
  <!-- CONTENT Ends  -->

<!-- footer -->
<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "footer.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>