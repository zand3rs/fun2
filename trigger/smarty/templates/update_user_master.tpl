{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

          
    <!-- form -->
    <form name="mainform" method="post" action="update_user_master.php">

        <input type="hidden" name="id"      value="{$id}"/>
        <input type="hidden" name="uid"     value="{php}echo get_uid();{/php}"/>
      
	   <!--Pagetitle -->
	   <div id="title">User Maintenance</div>
           
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
		{foreach from=$user_master_fields item=user_master_field}
           	<tr>
           	   <td class="label">{$user_master_field.title}</td>
           	   <td class="child">
           	   	{if $user_master_field.type == "text"}
           	   	  <input type="{$user_master_field.type}" name="{$user_master_field.name}" value="{$user_master_field.value|escape:"html"}" maxlength="{$user_master_field.max}"/>
           	   	{elseif $user_master_field.type == "password"}
           	   	  <input type="{$user_master_field.type}" name="{$user_master_field.name}" value="{$user_master_field.value|escape:"html"}" maxlength="{$user_master_field.max}"/>
           	   	{elseif $user_master_field.type == "select"}
			  <select name="{$user_master_field.name}">
				{html_options options=$user_master_field.select_options selected=$user_master_field.select_selected}
			  </select>  
           	   	{elseif $user_master_field.type == "check"}
           	   	  <input type="checkbox" name="{$user_master_field.name}" value="1" {if $user_master_field.value == "1"}checked=true{/if}/>
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
			   	   <input type="submit" name="btnSubmit" value="Update"/>
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