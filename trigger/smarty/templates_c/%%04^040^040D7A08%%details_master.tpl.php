<?php /* Smarty version 2.6.20, created on 2010-12-08 14:05:52
         compiled from details_master.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'escape', 'details_master.tpl', 33, false),)), $this); ?>
<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "header.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
	    	    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "pop_menu.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>

         
    <!-- form -->
    <form name="mainform" method="post" >

<input type="hidden" name="id"      value="<?php echo $this->_tpl_vars['id']; ?>
"/>
<input type="hidden" name="uid"     value="<?php echo get_uid(); ?>"/>


	   <!--Pagetitle -->
	   <div id="title"><?php echo $this->_tpl_vars['form_title']; ?>
</div>
	   
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

 		<?php $_from = $this->_tpl_vars['master_fields']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['master_field']):
?>
           	<tr>               
           	   <td class="label" style="width:250px"><?php echo $this->_tpl_vars['master_field']['title']; ?>
</td>
           	   <td class="child" style="width:350px"><?php echo ((is_array($_tmp=$this->_tpl_vars['master_field']['value'])) ? $this->_run_mod_handler('escape', true, $_tmp, 'html') : smarty_modifier_escape($_tmp, 'html')); ?>
</td>           
           	</tr>              
           	<?php endforeach; endif; unset($_from); ?>
           	
           </table>
	</div>
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