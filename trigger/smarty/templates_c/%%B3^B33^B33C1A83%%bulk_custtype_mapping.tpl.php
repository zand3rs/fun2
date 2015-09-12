<?php /* Smarty version 2.6.20, created on 2010-12-08 10:44:07
         compiled from bulk_custtype_mapping.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'bulk_custtype_mapping.tpl', 11, false),)), $this); ?>
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

	<form name='mainform' action="bulk_custtype_processing.php" enctype="multipart/form-data" method="post" target="hiddenframe">

	   <!--Pagetitle -->
	   <div id="title">Upload Customer Mapping</div>
           <!-- div id="desc">
                <?php echo ((is_array($_tmp=@$this->_tpl_vars['list_total'])) ? $this->_run_mod_handler('default', true, $_tmp, '0') : smarty_modifier_default($_tmp, '0')); ?>
 Result(s) found.&nbsp;
	   </divi -->
	   
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
		  <tr id='upload_tr'>	
           	   <td width="200" class="label"><input type='file' size="50" name='uploaded_file' /></td>
           	   <td width="100" class="label"><input type='button' value='Upload' onclick="lockMenu();document.mainform.submit();"/></td>
           	</tr>
                <tr>
                <td colspan="3" class="child">Format: PROFILE,639xxxxxxxxx,639xxxxxxxxx</td>
                </tr>
                </table>
		</div>
	</form>
	<iframe name='hiddenframe' id = 'hiddenframe' style='width:100%;height:500px;border:none' frameborder=0 marginWidht=0 marginHeight=0></iframe>
  <!-- CONTENT Ends  -->

<!-- footer -->
<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "footer.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>