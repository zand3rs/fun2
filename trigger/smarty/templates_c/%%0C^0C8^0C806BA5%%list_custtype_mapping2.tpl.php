<?php /* Smarty version 2.6.20, created on 2010-10-13 11:46:36
         compiled from list_custtype_mapping2.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'list_custtype_mapping2.tpl', 18, false),)), $this); ?>
<link   href="css/styles.css"  rel="stylesheet" type="text/css" />
<div style='padding-left:30px'>
   
	<!-- status msg -->
	<?php if ($this->_tpl_vars['error_msg'] != ""): ?>
		<div id="errorbox">
		<br/><?php echo $this->_tpl_vars['error_msg']; ?>
<br/>
		</div>
	<?php else: ?>
		<div id="desc">
		<?php if (( $this->_tpl_vars['is_searched'] == '1' ) && ( $this->_tpl_vars['list_all'] > 0 )): ?>
			<?php if ($this->_tpl_vars['list_total'] == '0'): ?>
				No records were uploaded out of <?php echo $this->_tpl_vars['list_all']; ?>
 record(s)&nbsp;
			<?php else: ?>
				Uploaded <?php echo $this->_tpl_vars['list_total']; ?>
 of <?php echo $this->_tpl_vars['list_all']; ?>
 record(s)&nbsp;
			<?php endif; ?>
		<?php elseif (( $this->_tpl_vars['is_searched'] == '1' )): ?>
	        	<?php echo ((is_array($_tmp=@$this->_tpl_vars['list_total'])) ? $this->_run_mod_handler('default', true, $_tmp, '0') : smarty_modifier_default($_tmp, '0')); ?>
 Result(s) found.&nbsp;
		<?php endif; ?>
		</div>
	<?php endif; ?>

	<!--Tablebody-->
	<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		  <tr>	
           	   <td width="150" class="label">Customer Type</td>
           	   <td width="100" class="label">MSISDN Fr</td>
           	   <td width="100" class="label">MSISDN To</td>
           	   <td width="300" class="label">Status</td>
           	</tr>
           	<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
           	<tr>
		   <td class="child" width="150">&nbsp;<?php echo $this->_tpl_vars['ldata']['customer_type']; ?>
 </td>
		   <td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['msisdn_fr']; ?>
   </td>
		   <td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['msisdn_to']; ?>
   </td>
		   <td class="child" width="300">&nbsp;<?php echo $this->_tpl_vars['ldata']['status']; ?>
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

	    <?php endif; ?>
</div>