{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

      
    
    <!-- form -->
    <form name="mainform" method="get" action="list_insrvr.php">

    
	   <!--Pagetitle -->
	   <div id="title">IN Server List</div>

	   <!-- status msg -->
	   {if $error_msg != ""}
		<div id="errorbox">
		<br/>{$error_msg}<br/>
		</div>
	   {else}
           	<div id="desc">
			{if $is_searched=="1"}
	   	             {$list_total|default:"0"} Result(s) found.&nbsp;
			{/if}
	   	</div>
	   {/if}

	         <div id="wrapper">
				<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
				<tr>	
				<td width="150" class="label">Server IP</td>
				<td width="150" class="child"><input type="text" name="scode" maxlength="30" style="width:200px" value="{$q_scode}"/></td>
				<td width="150" class="label">Server Name</td>
				<td width="150" class="child"><input type="text" name="sname" maxlength="60" style="width:200px" value="{$q_sname}"/></td>
				</tr>
				<tr>
				<td width="150" class="label">Status</td>
                      		<td width="150" class='child'>
					<select name="sstat" id="sstat">
						{html_options options=$soptstatus selected=$q_sstat}
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
                		        {if $error_msg != ""}
                                		<input type="button" name="btnReset" value="Clear" onClick="clearOnListErrForm();"/></td>
		                        {elseif $is_searched == "1"}
                		                <input type="button" name="btnReset" value="Clear" onClick="clearOnSearchForm();"/></td>
		                        {else}
                		                <input type="reset" name="btnReset" value="Clear"/></td>
		                        {/if}
				</tr>
				</table>
			</div>

	{if $is_searched=="1"}
            	<!--Tablebody-->
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		{if $q_csv <> "" }
		<tr>
		<td colspan=20 align="left" class="child">
		<a href="dl.php?muid={php}echo get_uid();{/php}&csv={$q_csv}">Download CSV</a>
		</td>
		</tr>
		{else}
		  <tr>	
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_name}&scode={$q_scode}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Server Name</a></td>
           	   <td width="75"  class="label"><a class="hhref" href="{$href_sort_code}&scode={$q_scode}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Server IP</a></td>
           	   <td width="50"  class="label">Port</td>
           	   <td width="50"  class="label">Svc Name</td>
           	   <td width="75"  class="label">Username</td>
           	   <td width="75"  class="label">Password</td>
            	   <td width="50"  class="label">Timeout (ms)</td>
           	   <td width="50"  class="label">Retry</td>
           	   <td width="50"  class="label">Expiry</td>
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_status}&scode={$q_scode}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Status</a></td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child" width="100">&nbsp;{$ldata.server_name} </td>
		   <td class="child" width="75" >&nbsp;{$ldata.server_ip}   </td>
		   <td class="child" width="50" >&nbsp;{$ldata.server_port} </td>
		   <td class="child" width="50" >&nbsp;{$ldata.svc_name}    </td>
		   <td class="child" width="75" >&nbsp;{$ldata.username}    </td>
		   <td class="child" width="75" >&nbsp;{$ldata.password}    </td>
		   <td class="child" width="50" >&nbsp;{$ldata.timeout}     </td>
		   <td class="child" width="50" >&nbsp;{$ldata.retry_cnt}   </td>
		   <td class="child" width="50" >&nbsp;{$ldata.expiry}      </td>
		   <td class="child" width="100">&nbsp;{$ldata.status}      </td>
		   <td class="child" width="200">
			<a href="update_insrvr.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_insrvr.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/delete.jpg"  border="0"></a>
		   </td>
	   	</tr>
           	{/foreach}
           	<tr>
		   <td colspan=20 align="left" class="child">
		   {$list_nav|default:""}&nbsp;
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
			   <a href="add_insrvr.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
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
