{* header *}
{include file="header.tpl"}
{* menu *}
{include file="menu.tpl"}

         
    
    <!-- form -->
    <form name="mainform" method="post" action="list_fun_tran.php">

    
	<!--Pagetitle -->
	<div id="title">Fun Transactions</div>

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
		<td width="150" class="label">Start Date <br>(YYYY-MM-DD)</td>
		<td width="100" class="child"><input type="text" name="q_sdate" maxlength="10" value="{$q_sdate}"/></td>
		<td width="150" class="label">End Date <br>(YYYY-MM-DD)</td>
		<td width="100" class="child"><input type="text" name="q_edate" maxlength="10" value="{$q_edate}"/></td>
		</tr>
		<tr>
		<!--
		<td width="150" class="label">Customer Type</td>
		<td width="100" class="child">
			<select name="{$master_fields[0].name}" {$master_fields[0].js} id="{$master_fields[0].name}" >
			{html_options options=$master_fields[0].select_options selected=$master_fields[0].select_selected}
			</select>  
		</td>
		<td width="150" class="label">Status</td>
		<td width="150" class='label'>
			<select name="q_status" id="q_status">
				{html_options options=$soptstatus selected='ALL'}
			</select>  
		</td>
		-->
		<td width="150" class="label">MSISDN <br>(Ex. 63917xxxxxxx)</td>
		<td class="child"><input type="text" name="q_msisdn" maxlength="20" value="{$q_msisdn}"/></td>
                <td width="150" class="label">&nbsp</td>
                <td class="child">&nbsp</td>
                </tr>
                <tr>
		<td width="100" class="label">&nbsp;</td>
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
		</td>
		</tr>
	</table>
	</div>

	<!--Tablebody-->

	<div id="wrapper">

		{if $is_searched == "1" }

		<table border="0" cellspacing="0" cellpadding="0" id="tablestyle" width="100%">
			<tr>	
                        <td width="30"  class="label">ID           </td>
                        <td width="100" class="label">Tran Date    </td>
                        <td width="50"  class="label">Tran ID.     </td>
                        <td width="50"  class="label">A_NO         </td>
                        <td width="50"  class="label">B_NO         </td>
                        <td width="100" class="label">Message      </td>
                        <td width="30"  class="label">Status       </td>
                        <td width="30"  class="label">Origin       </td>
			</tr>                         
           		{foreach from=$list_data item=ldata}
				<tr >                 
                                {if $ldata.stat < 2}
                                <td class="child" width="30"><a href="details_trigger_tran.php?uid={php}echo get_uid();{/php}&tran_id={$ldata.tran_id}" title="Retry Failed Transaction" rel="gb_page_center[{$popup_win_width}, {$popup_win_height}]">{$ldata.tran_id}</a></td>
                                {else}
                                <td class="child" width="30" >{$ldata.tran_id       }</td>
                                {/if}

                                <td class="child" width="100">{$ldata.dt_created    }</td>
                                <td class="child" width="50" >{$ldata.txid          }</td>
                                <td class="child" width="50" >{$ldata.a_no          }</td>
                                <td class="child" width="50" >{$ldata.b_no          }</td>
                                <td class="child" width="100">{$ldata.msg           }</td>
                                <td class="child" width="30" >{$ldata.status        }</td>
                                <td class="child" width="30" >{$ldata.request_origin}</td>
				</tr>
           		{/foreach}
                	
			<tr>
			<td colspan=20 align="left" class="child">
			{$list_nav|default:""}&nbsp;
			</td>
           		</tr>
                	
			{if $list_total == ".1" }
			<tr>
			<td colspan=20 align="left" class="child">
			<a href="dl.php?muid={php}echo get_uid();{/php}&csv={$q_csv}">Download CSV</a>
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
