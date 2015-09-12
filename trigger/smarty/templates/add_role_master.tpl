{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

    
    <!-- form -->
    <form name="mainform" method="post" action="add_role_master.php">

    
	   <!--Pagetitle -->
	   <div id="title">Role Maintenance</div>
           
           
            <!-- status msg -->
	    {if $error_msg != ""}
	    	    <div id="errorbox">
	    	    <br/>
	    	        {$error_msg}
	    	        <br/>
	    	    </div>
	    {/if}
            <!--Tablebody-->
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
	         
	          {foreach from=$role_master_fields item=role_master_field}
		            	<tr>
		            	   <td class="label">{$role_master_field.title}</td>
		            	   <td class="child">
		            	   	{if $role_master_field.type == "text"}
		            	   	  <input type="{$role_master_field.type}" name="{$role_master_field.name}" value="{$role_master_field.value|escape:"html"}" maxlength="{$role_master_field.max}"/>
		            	   	{elseif $role_master_field.type == "password"}
		            	   	  <input type="{$role_master_field.type}" name="{$role_master_field.name}" value="{$role_master_field.value|escape:"html"}" maxlength="{$role_master_field.max}"/>
		            	   	{elseif $role_master_field.type == "select"}
		 			  <select name="{$role_master_field.name}">
		 				{html_options options=$role_master_field.select_options selected=$role_master_field.select_selected}
		 			  </select>  
		            	   	{elseif $role_master_field.type == "check"}
		            	   	  <input type="checkbox" name="{$role_master_field.name}" value="1" {if $role_master_field.value == "1"}checked=true{/if}/>
		            	   	{/if}
		            	   </td>
		            	</tr>
           	{/foreach}
           	
	         
           </table>
	</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			   <input type="submit" name="btnSubmit" value="Save"/>
		           <input type="reset"  name="btnCancel" value="Clear"/>
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
{* footer *}
{include file="footer.tpl"}