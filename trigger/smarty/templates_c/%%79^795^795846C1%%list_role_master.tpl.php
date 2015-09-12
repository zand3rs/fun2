<?php /* Smarty version 2.6.20, created on 2010-12-08 14:45:33
         compiled from list_role_master.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'list_role_master.tpl', 15, false),)), $this); ?>
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
    <form name="mainform" method="post" action="list_role_master.php">

    
	   <!--Pagetitle -->
	   <div id="title">Web Role List</div>
           <div id="desc">
                <?php echo ((is_array($_tmp=@$this->_tpl_vars['list_total'])) ? $this->_run_mod_handler('default', true, $_tmp, '0') : smarty_modifier_default($_tmp, '0')); ?>
 Result(s) found.&nbsp;
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
		  <tr>	
           	   <td width="200" class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_name']; ?>
">Name</a></td>
           	   <td width="200" class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_desc']; ?>
">Description</a></td>
           	   
           	   <td width="200" class="label">Action</td>
           	</tr>
           	<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
           	<tr >
		   <td class="child" width="200"><?php echo $this->_tpl_vars['ldata']['role_code']; ?>
</td>
		   <td class="child" width="200">&nbsp;<?php echo $this->_tpl_vars['ldata']['role_desc']; ?>
</td>
		   <td class="child"  width="200">
			<?php if ($this->_tpl_vars['ldata']['role_code'] != 'admin'): ?>
			<a href="update_role_master.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_role_master.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
"><img src="images/delete.jpg"  border="0"></a>
			<?php else: ?>
			&nbsp;
			<?php endif; ?>
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
	</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			   <a href="add_role_master.php?uid=<?php echo get_uid(); ?>"><img src="images/add.jpg"  border="0"></a>
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