{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

                
          <!-- form -->
          <form name="mainform" method="post" action="reset_user_pass.php">
      
              <input type="hidden" name="id"      value="{$id}"/>
              <input type="hidden" name="uid"     value="{php}echo get_uid();{/php}"/>
            
      	   <!--Pagetitle -->
      	   <div id="title">Reset Password</div>
           <b>Password Rules:</b><br/>
           1. New password should be different from the current and previous 5 passwords.<br/>
           2. Password should be alphanumeric.<br/> 3. Password length should be 8 characters. (example: abcd1xyz)<br/>
           4. Blank or white space (spaces, tabs, etc.) is not allowed for password use.
           <br/>
                 
            <!-- status msg -->
      	    {if $error_msg != ""}
      	    	    <div id="errorbox">
      	    	    <br/>
      	    	        {$error_msg}
      	    	        <br/>
      	    	    </div>
      	    {/if}
          
          
                  <!--Tablebody-->
                  {if $close_window != "1"}
                  
      	         <div id="wrapper">
      	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
      
                 	<tr>
      		   <td class="label">Username</td>
      		   <td class="child"><input type="text" name="user" value=""/></td>
      		</tr>
      		<!---//
      		<tr>
      		   <td class="label">Old Password</td>
      		   <td class="child"><input type="password" name="old_pass" value=""/></td>
      		</tr>
      		//--->
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
      				   <input type="reset"  name="btnCancel" value="Clear"/>
      
      			   </td>
      		          </tr>
      		        </table>
      		      </div>
      		<!--buttons-->
      		{else}
      
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
      	    		
      		
      		{/if}
          
          </form>
          <!-- form -->


    
  </div>
  <!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}
