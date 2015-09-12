<?php /* Smarty version 2.6.20, created on 2010-12-08 10:22:34
         compiled from update_master.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'escape', 'update_master.tpl', 40, false),array('function', 'html_options', 'update_master.tpl', 45, false),)), $this); ?>
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
    <form name="mainform" method="post" action="<?php echo $this->_tpl_vars['form_action']; ?>
">




<input type="hidden" name="<?php echo $this->_tpl_vars['form_primary']; ?>
"      value="<?php echo $this->_tpl_vars['id']; ?>
"/>
<input type="hidden" name="uid"                  value="<?php echo get_uid(); ?>"/>


    
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
	<td class="label"><?php echo $this->_tpl_vars['master_field']['title']; ?>
</td>
	<td class="child">
		<?php if ($this->_tpl_vars['master_field']['type'] == 'text'): ?>
			<input type="<?php echo $this->_tpl_vars['master_field']['type']; ?>
" name="<?php echo $this->_tpl_vars['master_field']['name']; ?>
" value="<?php echo ((is_array($_tmp=$this->_tpl_vars['master_field']['value'])) ? $this->_run_mod_handler('escape', true, $_tmp, 'html') : smarty_modifier_escape($_tmp, 'html')); ?>
" style="width:<?php echo $this->_tpl_vars['master_field']['width']; ?>
px" maxlength="<?php echo $this->_tpl_vars['master_field']['max']; ?>
" "<?php echo $this->_tpl_vars['master_field']['read_only']; ?>
"/>
		<?php elseif ($this->_tpl_vars['master_field']['type'] == 'password'): ?>
			<input type="<?php echo $this->_tpl_vars['master_field']['type']; ?>
" name="<?php echo $this->_tpl_vars['master_field']['name']; ?>
" value="<?php echo ((is_array($_tmp=$this->_tpl_vars['master_field']['value'])) ? $this->_run_mod_handler('escape', true, $_tmp, 'html') : smarty_modifier_escape($_tmp, 'html')); ?>
" maxlength="<?php echo $this->_tpl_vars['master_field']['max']; ?>
"/>
		<?php elseif ($this->_tpl_vars['master_field']['type'] == 'select'): ?>
			<select name="<?php echo $this->_tpl_vars['master_field']['name']; ?>
" <?php echo $this->_tpl_vars['master_field']['js']; ?>
 id="<?php echo $this->_tpl_vars['master_field']['name']; ?>
" >
				<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['master_field']['select_options'],'selected' => $this->_tpl_vars['master_field']['select_selected']), $this);?>

			</select>  
		<?php elseif ($this->_tpl_vars['master_field']['type'] == 'check'): ?>
			<input type="checkbox" name="<?php echo $this->_tpl_vars['master_field']['name']; ?>
" value="1" <?php if ($this->_tpl_vars['master_field']['value'] == '1'): ?>checked=true<?php endif; ?>/>
		<?php elseif ($this->_tpl_vars['master_field']['type'] == 'textarea'): ?>
			<textarea rows = "<?php echo $this->_tpl_vars['master_field']['rows']; ?>
" cols="<?php echo $this->_tpl_vars['master_field']['cols']; ?>
" name="<?php echo $this->_tpl_vars['master_field']['name']; ?>
"><?php echo $this->_tpl_vars['master_field']['value']; ?>
</textarea>
		<?php endif; ?>
	</td>
	</tr>
	<?php endforeach; endif; unset($_from); ?>

	</table>
	</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			   <input type="submit" name="btnSubmit" value="<?php echo $this->_tpl_vars['form_submit']; ?>
"/>
		           <input type="reset"  name="btnCancel" value="Reset" onclick="clearOnUpdateForm();"/>
			   </td>
		          </tr>
		        </table>
		      </div>
		<!--buttons-->

    
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