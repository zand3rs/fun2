{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

<!-- form -->
<form name="mainform" method="post" action="list_audit_log.php">

	<!--Pagetitle -->
	<div id="title">Audit Log</div>
	   
	<!-- status msg -->
	{if $error_msg != ""}
		<div id="errorbox">
		<br/>
		{$error_msg}
		<br/>
		</div>
	{else}
		<div id="desc">
		{if $is_searched == "1" }
			{$list_total|default:"0"} Result(s) found.&nbsp;
		{/if}
		</div>
	{/if}
	    
	<!--Tablebody-->
	<div id="wrapper">
	<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		<tr>	
		<td width="150" class="label">Start Date<br>(YYYY-MM-DD)</td>
		<td width="150" class="child"><input type="text" name="q_sdate" maxlength="10" value="{$q_sdate}"/></td>
		<td width="150" class="label">End Date<br>(YYYY-MM-DD)</td>
		<td width="150" class="child"><input type="text" name="q_edate" maxlength="10" value="{$q_edate}"/></td>
		</tr>
		<tr>
		<td width="150" class="label">User Code</td>
		<td width="150" class="child"><input type="text" name="q_user_code" maxlength="20" value="{$q_user_code}"/></td>
		<td width="150" class="label">Role Code</td>
		<td width="150" class="child">
			<select name="{$master_fields[1].name}" {$master_fields[1].js} id="{$master_fields[1].name}" >
				{html_options options=$master_fields[1].select_options selected=$master_fields[1].select_selected}
			</select>  
		</td>
		</tr>
		<tr>	
		<td width="150" class="label">Module Name</td>
		<td width="150" class="child">
			<select name="{$master_fields[0].name}" {$master_fields[0].js} id="{$master_fields[0].name}" >
			{html_options options=$master_fields[0].select_options selected=$master_fields[0].select_selected}
			</select>  
		</td>
		<td width="150" class="label">&nbsp;</td>
		<td width="150" class="child"><input type="checkbox" value="1" name="sfile"/>Generate CSV FILE</td>
		</tr>
		<tr>	
		<td width="150" class="label">&nbsp;</td>
		<td colspan=3 class="child">
                        <input type="submit" name="btnSubmit" value="Search" />
                        &nbsp;
                        {if $error_msg != ""}
                                <input type="button" name="btnReset" value="Clear" onClick="clearOnListErrForm();"/></td>
                        {elseif $is_searched == "1"}
                                <input type="button" name="btnReset" value="Clear" onClick="clearOnSearchForm();"/></td>
                        {else}
                                <input type="reset" name="btnReset" value="Clear"/></td>
                        {/if}
		</td>
		</td>
		</tr>
	</table>
	</div>
	<!--Tablebody-->

	<div id="wrapper">
	{if $is_searched == "1" }
	<table border="0" cellspacing="0" cellpadding="0" id="tablestyle" width="100%">
	{if $q_csv <> "" }
		<tr>
		<td colspan=20 align="left" class="child">
		<a href="dl.php?muid={php}echo get_uid();{/php}&csv={$q_csv}">Download CSV</a>
		</td>
		</tr>
	{else}
		<tr>	
			<td width="100" class="label">Tx Date</td>
			<td width="50"  class="label">User</td>
			<td width="50"  class="label">Module</td>
			<td width="50"  class="label">IP Address</td>
			<td width="50"  class="label">MSISDN</td>
			<td width="300" class="label">Message</td>
	   	</tr>
           	{foreach from=$list_data item=ldata}
			<tr>
			<td class="child" width="100">&nbsp;{$ldata.start_tm  }</td>
			<td class="child" width="50" >&nbsp;{$ldata.user_code }</td>
			<td class="child" width="50" >&nbsp;{$ldata.mod_name  }</td>
			<td class="child" width="50" >&nbsp;{$ldata.ip_addr   }</td>
			<td class="child" width="50" >&nbsp;{$ldata.msisdn    }</td>
			<td class="child" width="300">&nbsp;{$ldata.message   }</td>
			</tr>                                       
           	{/foreach}                            
		<tr>                                  
		<td colspan=20 align="left" class="child">
		{$list_nav|default:""}&nbsp;
		</td>
           	</tr>

	{/if}
	</table>
	{/if}
	</div>
</form>
<!-- form -->

    
<!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}
