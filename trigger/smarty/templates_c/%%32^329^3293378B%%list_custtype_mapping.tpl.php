<?php /* Smarty version 2.6.20, created on 2010-12-08 10:12:37
         compiled from list_custtype_mapping.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'list_custtype_mapping.tpl', 30, false),array('function', 'html_options', 'list_custtype_mapping.tpl', 42, false),)), $this); ?>
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
    <form name="mainform" method="get" action="list_custtype_mapping.php">

    
	<!--Pagetitle -->
	<div id="title">Customer Type Mapping List</div>

	<!-- status msg -->
	<?php if ($this->_tpl_vars['error_msg'] != ""): ?>
		<div id="errorbox">
		<br/><?php echo $this->_tpl_vars['error_msg']; ?>
<br/>
		</div>
	<?php else: ?>
		<div id="desc">
		<?php if ($this->_tpl_vars['error_msg'] != 'DELETE_CUSTOMER_TYPE_MAPPING_SUCCESS'): ?>
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
		<?php endif; ?>
		</div>
	<?php endif; ?>

	<div id="wrapper">
		<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		<tr>
		<td width="150" class="label">Customer Type</td>
              	<td width="150" class='child'>
			<select name="sname_l" id="sname_l">
				<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['soptcustyp'],'selected' => $this->_tpl_vars['q_sname_l']), $this);?>

			</select>  
		</td>
		<td width="150" class="label">MSISDN <br>(Ex. 63917xxxxxxx)</td>
		<td width="150" class="child"><input type="text" name="scode" maxlength="12" value="<?php echo $this->_tpl_vars['q_scode']; ?>
"/></td>
		</tr>
		<tr>
		<td width="150" class="label">Status</td>
              	<td width="150" class='child'>
			<select name="sstat" id="sstat">
				<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['soptstatus'],'selected' => $this->_tpl_vars['q_sstat']), $this);?>

			</select>  
		</td>
		<td width="150" class="label">&nbsp;</td>
		<td width="150" class="child"><input type="checkbox" value="1" name="sfile"/>Generate CSV FILE</td>
		</tr>
		<tr>
		<td width="150" class="label">&nbsp;</td>
		<td colspan="3" class="child">
			<input type="submit" name="btnSubmit" value="Search" />
			&nbsp;
			<?php if ($this->_tpl_vars['error_msg'] != ""): ?>
				<input type="button" name="btnReset" value="Clear" onClick="clearOnListErrForm();"/>
                        <?php elseif ($this->_tpl_vars['is_searched'] == '1'): ?>
                                <input type="button" name="btnReset" value="Clear" onClick="clearOnSearchForm();"/></td>
			<?php else: ?>
				<input type="reset" name="btnReset" value="Clear" />
			<?php endif; ?>
		</td>
		</tr>
		</table>
	</div>

	<!--Tablebody-->
	<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		<?php if ($this->_tpl_vars['q_csv'] <> ""): ?>
		<tr>
		<td colspan=20 align="left" class="child">
		<a href="dl.php?muid=<?php echo get_uid(); ?>&csv=<?php echo $this->_tpl_vars['q_csv']; ?>
">Download CSV</a>
		</td>
		</tr>
		<?php else: ?>
		  <tr>	
           	   <!-- td width="20"  class="label"><input type="checkbox" name="Check_Rec" value="yes" onclick="checkAllBox(document.mainform.check_rec)"/></td -->
           	   <td width="20"  class="label"><input type="checkbox" name="Check_Rec" value="yes" onclick="selectAllCheckBoxes('mainform', 'check_rec[]')"/></td>
           	   <td width="150" class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_name']; ?>
&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname_l=<?php echo $this->_tpl_vars['q_sname_l']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Customer Type</a></td>
           	   <td width="100" class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_code']; ?>
&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname_l=<?php echo $this->_tpl_vars['q_sname_l']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">MSISDN Fr</a></td>
           	   <td width="100" class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_desc']; ?>
&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname_l=<?php echo $this->_tpl_vars['q_sname_l']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">MSISDN To</td>
           	   <td width="100" class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_status']; ?>
&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname_l=<?php echo $this->_tpl_vars['q_sname_l']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Status</a></td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
           	<tr>
		   <td class="child" width="20" ><input type="checkbox" name="check_rec[]" value="<?php echo $this->_tpl_vars['ldata']['id']; ?>
"/></td>
		   <td class="child" width="150">&nbsp;<?php echo $this->_tpl_vars['ldata']['customer_type']; ?>
 </td>
		   <td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['msisdn_fr']; ?>
   </td>
		   <td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['msisdn_to']; ?>
   </td>
		   <td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['status']; ?>
      </td>
		   <td class="child" width="200">
			<a href="update_custtype_mapping.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
"><img src="images/update.jpg" border="0"></a>
			&nbsp;
			<a href="delete_custtype_mapping.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
" onclick="return confirm('Are you sure you want to delete?')"><img src="images/delete.jpg" border="0"></a>
		   </td>
	   	</tr>
           	<?php endforeach; endif; unset($_from); ?>


		<?php if ($this->_tpl_vars['q_sdall'] == '1'): ?>
           	<tr>
		<td colspan=20 align="left" class="child">
	        <a href="delete_all_custtype_mapping.php?uid=<?php echo get_uid(); ?>&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname_l=<?php echo $this->_tpl_vars['q_sname_l']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
" onclick="return confirm('Are you sure you want to delete?')">Delete All Searched Items</a>
		</td>
           	</tr>
		<?php endif; ?>

           	<tr>
		<td colspan=20 align="left" class="child">
		<?php echo ((is_array($_tmp=@$this->_tpl_vars['list_nav'])) ? $this->_run_mod_handler('default', true, $_tmp, "") : smarty_modifier_default($_tmp, "")); ?>
&nbsp;
		</td>
           	</tr>

		<?php endif; ?>

           	</table>
		</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			   <a href="add_custtype_mapping.php?uid=<?php echo get_uid(); ?>"><img src="images/add.jpg"  border="0"></a>
			   </td>
                           <td colspan=20 align="left">
                           <a href="bulk_custtype_mapping.php?uid=<?php echo get_uid(); ?>"><img src="images/bulk.jpg"  border="0"></a>
                           </td>
                           <td colspan=20 align="left">
                           <input type="image" name="btnSubmit" value="Delete Multiple" onclick="return confirm('Are you sure you want to delete?')" src="images/delete_all.jpg"/>
                           </td>
		          </tr>
		        </table>
		      </div>
		<!--buttons-->

	    <?php endif; ?>

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