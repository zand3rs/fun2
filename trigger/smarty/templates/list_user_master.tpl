{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

         
    
    <!-- form -->
    <form name="mainform" method="get" action="list_user_master.php">

    
	   <!--Pagetitle -->
	   <div id="title">Web User List</div>
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
	    
	         <div id="wrapper">
				<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
				<tr>	
				<td width="150" class="label">Username</td>
				<td width="150" class="child"><input type="text" name="code" maxlength="20"/></td>
				<td width="150" class="label">Last Name</td>
				<td width="150" class="child"><input type="text" name="lname" maxlength="20"/></td>
                      		</td>
				</tr>
				<tr>	
				<td width="150" class="label">Email Address</td>
				<td width="150" class="child"><input type="text" name="email" maxlength="60"/></td>
				<td width="150" class="label">Role</td>
                      		<td width="150" class='child'>
					<select name="role_code" id="role_code">
						{html_options options=$srole_code selected=0}
					</select>  

				</td>
				</tr>
				<tr>	
				<td width="100" class="label">&nbsp;</td>
				<td colspan="3" class="child"><input type="submit" name="btnSubmit" value="Search" />
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
			<td width="200" class="label"><a class="hhref" href="{php}echo $_SERVER[PHP_SELF];{/php}?code={php}echo $_GET[code];{/php}&lname={php}echo $_GET[lname];{/php}&email={php}echo $_GET[email];{/php}&role_code={php}echo $_GET[role_code];{/php}&{$href_sort_user}">Username</a></td>
			<td width="200" class="label"><a class="hhref" href="{php}echo $_SERVER[PHP_SELF];{/php}?code={php}echo $_GET[code];{/php}&lname={php}echo $_GET[lname];{/php}&email={php}echo $_GET[email];{/php}&role_code={php}echo $_GET[role_code];{/php}&{$href_sort_name}">Full Name</a></td>
			<td width="200" class="label"><a class="hhref" href="{php}echo $_SERVER[PHP_SELF];{/php}?code={php}echo $_GET[code];{/php}&lname={php}echo $_GET[lname];{/php}&email={php}echo $_GET[email];{/php}&role_code={php}echo $_GET[role_code];{/php}&{$href_sort_role}">Role</a></td>
			<td width="200" class="label"><a class="hhref" href="{php}echo $_SERVER[PHP_SELF];{/php}?code={php}echo $_GET[code];{/php}&lname={php}echo $_GET[lname];{/php}&email={php}echo $_GET[email];{/php}&role_code={php}echo $_GET[role_code];{/php}&{$href_sort_email}">Email</a></td>
			<td width="200" class="label"><a class="hhref" href="{php}echo $_SERVER[PHP_SELF];{/php}?code={php}echo $_GET[code];{/php}&lname={php}echo $_GET[lname];{/php}&email={php}echo $_GET[email];{/php}&role_code={php}echo $_GET[role_code];{/php}&{$href_sort_position}">Position</a></td>
			<td width="200" class="label"><a class="hhref" href="{php}echo $_SERVER[PHP_SELF];{/php}?code={php}echo $_GET[code];{/php}&lname={php}echo $_GET[lname];{/php}&email={php}echo $_GET[email];{/php}&role_code={php}echo $_GET[role_code];{/php}&{$href_sort_expiry}">Password Expiry</a></td>

			<td width="200" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child" width="200"><a href="details_user_master.php?uid={php}echo get_uid();{/php}&id={$ldata.user_id}" title="View User Info" rel="gb_page_center[{$popup_win_width}, {$popup_win_height}]">{$ldata.user_code}</a></td>
		   <td class="child" width="200">&nbsp;{$ldata.last_name},&nbsp;{$ldata.first_name}</td>
		   <td class="child" width="200">&nbsp;{$ldata.role_code}</td>
		   <td class="child" width="200">&nbsp;{$ldata.email}</td>
		   <td class="child" width="200">&nbsp;{$ldata.position}</td>
		   <td class="child" width="200">&nbsp;{if $ldata.pass_exp_n_days == 0}
			    Never Expires
			 {elseif $ldata.pass_exp_n_days == 7}
			    1 Week
			 {elseif $ldata.pass_exp_n_days == 30}
			    1 Month
			 {else} 	
			    {$ldata.pass_exp_n_days} days
			 {/if}
		   </td>
		   <td class="child"  width="200">
			<a href="update_user_master.php?uid={php}echo get_uid();{/php}&id={$ldata.user_id}"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_user_master.php?uid={php}echo get_uid();{/php}&id={$ldata.user_id}"><img src="images/delete.jpg"  border="0"></a>
		   </td>
	   	</tr>
           	{/foreach}
           	<tr>
		   <td colspan=20 align="left" class="child">
		   {$list_nav|default:""}&nbsp;
		   </td>
           	</tr>

                        {if $list_total > "0" }
                        <tr>
                        <td colspan=20 align="left" class="child">
                        <a href="dl.php?muid={php}echo get_uid();{/php}&csv={$q_csv}">Download CSV</a>
                        </td>
                        </tr>
                        {/if}

           </table>
	</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			   <a href="add_user_master.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
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
