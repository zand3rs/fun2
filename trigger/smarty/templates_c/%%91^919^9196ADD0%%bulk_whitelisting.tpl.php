<?php /* Smarty version 2.6.20, created on 2010-05-11 16:31:47
         compiled from bulk_whitelisting.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'bulk_whitelisting.tpl', 60, false),)), $this); ?>
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

	<form action="bulk_whitelisting.php" enctype="multipart/form-data" method="post">

	   <!--Pagetitle -->
	   <div id="title">Bulk Whitelisting</div>
           <div id="desc">
		<?php if ($this->_tpl_vars['list_total'] == 0): ?>
	                No records were uploaded.&nbsp;
		<?php else: ?>
			<?php if ($this->_tpl_vars['list_total'] > 0): ?>
	                	<?php echo $this->_tpl_vars['list_total']; ?>
 of <?php echo $this->_tpl_vars['list_all']; ?>
 record(s) were uploaded.&nbsp;
			<?php endif; ?>
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
           	   <td width="200" class="label"><input type='file' size="50" name='uploaded_file' /></td>
           	   <td width="100" class="label"><input type='submit' value='Upload' /></td>
           	</tr>
                <tr>
                <td colspan="3" class="child">Format: 639xxxxxxxxx,Remarks</td>
                </tr>
		</table>
		</div>
	</form>

	<?php if ($this->_tpl_vars['list_total'] >= 0): ?>
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		  <tr>	
           	   <td width="100" class="label">MSISDN</td>
           	   <td width="300" class="label">Remarks</td>
           	   <td width="300" class="label">Reason</td>
           	</tr>
           	<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
           	<tr >
		   <td class="child" width="100"><?php echo $this->_tpl_vars['ldata']['msisdn']; ?>
</td>
		   <td class="child" width="300">&nbsp;<?php echo $this->_tpl_vars['ldata']['remarks']; ?>
</td>
		   <td class="child" width="300">&nbsp;<?php echo $this->_tpl_vars['ldata']['reason']; ?>
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
	    <?php endif; ?>
  <!-- CONTENT Ends  -->

<!-- footer -->
<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "footer.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>