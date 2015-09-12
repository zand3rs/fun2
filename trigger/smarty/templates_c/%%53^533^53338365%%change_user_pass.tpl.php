<?php /* Smarty version 2.6.20, created on 2010-02-11 10:41:57
         compiled from change_user_pass.tpl */ ?>
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
    <form name="mainform" method="post" action="change_user_pass.php">

        <input type="hidden" name="id"      value="<?php echo $this->_tpl_vars['id']; ?>
"/>
        <input type="hidden" name="uid"     value="<?php echo get_uid(); ?>"/>
      
	   <!--Pagetitle -->
	   <div id="title">Change Password</div>
           <b>Password Rules:</b><br/>
           1. New password should be different from the current and previous 5 passwords.<br/>
           2. Password should be alphanumeric.<br/> 3. Password length should be 8 characters. (example: abcd1xyz)<br/>
           4. Blank or white space (spaces, tabs, etc.) is not allowed for password use.
           <br/>
             <!-- status msg -->
	    <?php if ($this->_tpl_vars['error_msg'] != ""): ?>
	    	    <div id="errorbox">
	    	    <br/>
	    	        <?php echo $this->_tpl_vars['error_msg']; ?>

	    	        <br/>
	    	    </div>
	    <?php endif; ?>
    
    
            <!--Tablebody-->
            <?php if ($this->_tpl_vars['close_window'] != '1'): ?>
            
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">

           	<tr>
		   <td class="label">Username</td>
		   <td class="child"><input type="text" name="user" value=""/></td>
		</tr>
		<tr>
		   <td class="label">Old Password</td>
		   <td class="child"><input type="password" name="old_pass" value=""/></td>
		</tr>
		<tr>
		   <td class="label">New Password</td>
		   <td class="child"><input type="password" name="new_pass1" value=""/></td>
		</tr>
		<tr>
		   <td class="label">Confirm New Password</td>
		   <td class="child"><input type="password" name="new_pass2" value=""/></td>
		</tr>

		   </table>
		</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			           <input type="submit" name="btnSubmit" value="Change"/>
				   <input type="reset"  name="btnCancel" value="Cancel"/>

			   </td>
		          </tr>
		        </table>
		      </div>
		<!--buttons-->
		<?php else: ?>

		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
				<!---//
				<a href="#" onClick="javascript:HideGreyBox();">Close Window</a>
				//--->
			   </td>
		          </tr>
		        </table>
		      </div>
	    		
		
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