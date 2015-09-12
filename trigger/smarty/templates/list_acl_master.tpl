{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

         
    
    <!-- form -->
    <form name="mainform" method="get" action="list_acl_master.php">

    
	   <!--Pagetitle -->
	   <div id="title">Web ACL List</div>
           <div id="desc">
		{if $is_searched=="1"}
	                {$list_total|default:"0"} Result(s) found.&nbsp;
		{/if}
	   </div>
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
				<tr>	
				<td width="150" class="label">Role</td>
				<!-- <td colspan="3" class="child"><input type="text" name="role_code" maxlength="10"/></td> -->
                      		<td colspan="3" class='child'>
					<select name="role_code" id="role_code">
						{html_options options=$srole_code selected=0}
					</select>  

				</td>
				</tr>
				<tr>	
				<td width="150" class="label">Module</td>
				<!-- <td colspan="3" class="child"><input type="text" name="mod_name" maxlength="10"/></td> -->
                      		<td colspan="3" class='child'>
					<select id='mod_name' name='mod_name'>
						{html_options options=$smod_code selected=0}
					</select>
				</td>
				</tr>
				<tr>	
				<td width="100" class="label">&nbsp;</td>
				<td colspan="3" class="child">
					<input type="submit" name="btnSubmit" value="Search" />
					&nbsp;
					<input type="reset" name="btnReset" value="Clear" /></td>
				</tr>
				</table>
			</div>
            <!--Tablebody-->
	{if $is_searched=="1"}
            	 <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		  <tr>	
           	   <td width="200" class="label"><a  class="hhref" href="{php}echo $_SERVER[PHP_SELF];{/php}?role_code={php}echo $_GET[role_code];{/php}&mod_name={php}echo $_GET[mod_name];{/php}&{$href_sort_role}">Role</a></td>
           	   <td width="200" class="label"><a  class="hhref" href="{php}echo $_SERVER[PHP_SELF];{/php}?role_code={php}echo $_GET[role_code];{/php}&mod_name={php}echo $_GET[mod_name];{/php}&{$href_sort_mod_name}">Module</a></td>

           	   <td width="200" class="label">Can View</td>
           	   <td width="200" class="label">Can Add</td>
           	   <td width="200" class="label">Can Update</td>
           	   <td width="200" class="label">Can Delete</td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child"  width="200">{$ldata.role_code}</td>
		   <td class="child"  width="200">&nbsp;{$ldata.mod_name}</td>
		   <td class="child"  width="200">&nbsp;{if $ldata.can_view    == 1} Yes {else} No {/if}</td>
		   <td class="child"  width="200">&nbsp;{if $ldata.can_add     == 1} Yes {else} No {/if}</td>
		   <td class="child"  width="200">&nbsp;{if $ldata.can_update  == 1} Yes {else} No {/if}</td>
		   <td class="child"  width="200">&nbsp;{if $ldata.can_delete  == 1} Yes {else} No {/if}</td>
		   <td class="child"  width="200">
			<a href="update_acl_master.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_acl_master.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/delete.jpg"  border="0"></a>
		   </td>
	   	</tr>
           	{/foreach}
           	<tr>
		   <td colspan=20 align="left" class="child">
		   {$list_nav|default:""}&nbsp;
		   </td>
           	</tr>

           </table>
	</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			   <a href="add_acl_master.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
			   </td>
		          </tr>
		        </table>
		      </div>
		<!--buttons-->
	{/if}

    
    </form>
    <!-- form -->

    
  </div>
  <!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}
