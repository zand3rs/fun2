<?php /* Smarty version 2.6.20, created on 2010-01-29 13:13:33
         compiled from manual_blacklist.tpl */ ?>
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
    <form name="mainform" method="post" action="manual_blacklist.php">

	<!--Pagetitle -->
	<div id="title">Manual Blacklisting</div>
		<?php if (( $this->_tpl_vars['is_searched'] == '1' ) && ( $this->_tpl_vars['error_msg'] == 'SAVING_BLACKLIST_SUCCESS' )): ?>
			<div id="errorbox">
			<br/>
			BLACKLISTING successfully entered...
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
                                <td width="150" class="label">Remarks</td>
                                <td class="child"><input type="text" name="q_remarks" maxlength="220" style="width:400px" /></td>
                                </tr>
				<tr>	
				<td width="100" class="label">&nbsp;</td>
				<td colspan="3" class="child">
					<input type="submit" name="btnSubmit" value="Blacklist" />
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