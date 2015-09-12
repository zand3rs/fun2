<?php /* Smarty version 2.6.20, created on 2010-06-21 10:36:23
         compiled from list_countries.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'default', 'list_countries.tpl', 24, false),array('function', 'html_options', 'list_countries.tpl', 42, false),)), $this); ?>
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
    <form name="mainform" method="get" action="list_countries.php">


	   <!--Pagetitle -->
	   <div id="title">Countries List</div>

	   <!-- status msg -->
	   <?php if ($this->_tpl_vars['error_msg'] != ""): ?>
	    	<div id="errorbox">
	    	<br/><?php echo $this->_tpl_vars['error_msg']; ?>
<br/>
	    	</div>
	   <?php else: ?>
           	<div id="desc">
	   	<?php if ($this->_tpl_vars['error_msg'] != 'DELETE_COUNTRY_SUCCESS'): ?>
			<?php if ($this->_tpl_vars['is_searched'] == '1'): ?>
	   	            <?php echo ((is_array($_tmp=@$this->_tpl_vars['list_total'])) ? $this->_run_mod_handler('default', true, $_tmp, '0') : smarty_modifier_default($_tmp, '0')); ?>
 Result(s) found.&nbsp;
			<?php endif; ?>
	   	<?php endif; ?>
	   	</div>
	   <?php endif; ?>

	         <div id="wrapper">
				<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
				<tr>	
				<td width="150" class="label">Country Code</td>
				<td width="150" class="child"><input type="text" name="scode" maxlength="10" value="<?php echo $this->_tpl_vars['q_scode']; ?>
"/></td>
				<td width="150" class="label">Country Name</td>
				<td width="150" class="child"><input type="text" name="sname" maxlength="30" value="<?php echo $this->_tpl_vars['q_sname']; ?>
"/></td>
				</tr>
				<tr>
				<td width="150" class="label">Zone</td>
                      		<td width="150" class='child'>
					<select name="szone" id="szone">
						<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['soptzones'],'selected' => $this->_tpl_vars['q_szone']), $this);?>

					</select>  
				</td>
				<td width="150" class="label">Status</td>
                      		<td width="150" class='child'>
					<select name="sstat" id="sstat">
						<?php echo smarty_function_html_options(array('options' => $this->_tpl_vars['soptstatus'],'selected' => $this->_tpl_vars['q_sstat']), $this);?>

					</select>  
				</td>
				</tr>
				<tr>	
				<td width="150" class="label">&nbsp;</td>
				<td width="150" class="child"><input type="checkbox" value="1" name="sfile"/>Generate CSV FILE</td>
				<td width="150" class="label">&nbsp;</td>
				<td width="150" class="child">
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
           	   <td width="100" class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_code']; ?>
&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&szone=<?php echo $this->_tpl_vars['q_szone']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Country Code</a></td>
           	   <td width="200" class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_name']; ?>
&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&szone=<?php echo $this->_tpl_vars['q_szone']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Country Name</a></td>
           	   <td width="70"  class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_zone']; ?>
&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&szone=<?php echo $this->_tpl_vars['q_szone']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Zone ID</a></td>
           	   <td width="300" class="label">Charge</td>
           	   <td width="50"  class="label"><a class="hhref" href="<?php echo $this->_tpl_vars['href_sort_status']; ?>
&scode=<?php echo $this->_tpl_vars['q_scode']; ?>
&sname=<?php echo $this->_tpl_vars['q_sname']; ?>
&sstat=<?php echo $this->_tpl_vars['q_sstat']; ?>
&szone=<?php echo $this->_tpl_vars['q_szone']; ?>
&wsort=<?php echo $this->_tpl_vars['q_ssort']; ?>
">Status</a></td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	<?php $_from = $this->_tpl_vars['list_data']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['ldata']):
?>
           	<tr >
		   <td class="child" width="100">&nbsp;<?php echo $this->_tpl_vars['ldata']['country_code']; ?>
</td>
		   <td class="child" width="200">&nbsp;<?php echo $this->_tpl_vars['ldata']['country_name']; ?>
</td>
		   <td class="child" width="70" >&nbsp;<?php echo $this->_tpl_vars['ldata']['zone_id']; ?>
</td>
		   <td class="child" width="300">&nbsp;<?php echo $this->_tpl_vars['ldata']['charge_desc']; ?>
</td>
		   <td class="child" width="50" >&nbsp;<?php echo $this->_tpl_vars['ldata']['status']; ?>
</td>
		   <td class="child" width="200">
			<a href="update_country.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_country.php?uid=<?php echo get_uid(); ?>&id=<?php echo $this->_tpl_vars['ldata']['id']; ?>
"><img src="images/delete.jpg"  border="0"></a>
		   </td>
	   	</tr>
           	<?php endforeach; endif; unset($_from); ?>
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
			   <a href="add_country.php?uid=<?php echo get_uid(); ?>"><img src="images/add.jpg"  border="0"></a>
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