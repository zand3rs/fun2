{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

      
    
    <!-- form -->
    <form name="mainform" method="get" action="list_hlrsrvr_mapping.php">

    
	   <!--Pagetitle -->
	   <div id="title">HLR Server Mapping List</div>

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
				<td width="150" class="label">Server Name</td>
                      		<td width="150" class='child'>
					<select name="sname_l" id="sname_l">
						{html_options options=$sopthlrsvr selected=$q_sname_l}
					</select>  
				</td>
				<td width="150" class="label">MSISDN <br>(Ex. 63917xxxxxxx)</td>
				<td width="150" class="child"><input type="text" name="scode" maxlength="12" value="{$q_scode}"/></td>
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
						<input type="button" name="btnReset" value="Clear" onClick="clearOnListErrForm();"/>
                        		{elseif $is_searched == "1"}
                                		<input type="button" name="btnReset" value="Clear" onClick="clearOnSearchForm();"/></td>
					{else}
						<input type="reset" name="btnReset" value="Clear" />
					{/if}
				</td>
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
           	   <td width="80"  class="label"><a class="hhref" href="{$href_sort_code}&scode={$q_scode}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">MSISDN Fr</a></td>
           	   <td width="80"  class="label"><a class="hhref" href="{$href_sort_desc}&scode={$q_scode}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">MSISDN To</td>
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_status}&scode={$q_scode}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Status</a></td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child" width="100">&nbsp;{$ldata.server_name} </td>
		   <td class="child" width="80" >&nbsp;{$ldata.msisdn_fr}   </td>
		   <td class="child" width="80" >&nbsp;{$ldata.msisdn_to}   </td>
		   <td class="child" width="100">&nbsp;{$ldata.status}      </td>
		   <td class="child" width="200">
			<a href="update_hlrsrvr_mapping.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_hlrsrvr_mapping.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/delete.jpg"  border="0"></a>
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
			   <a href="add_hlrsrvr_mapping.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
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
