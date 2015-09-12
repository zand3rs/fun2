<link   href="css/styles.css"  rel="stylesheet" type="text/css" />
<div style='padding-left:30px'>
   
	<!-- status msg -->
	{if $error_msg != ""}
		<div id="errorbox">
		<br/>{$error_msg}<br/>
		</div>
	{else}
		<div id="desc">
		{if ($is_searched=="1") and ($list_all>0)}
			{if $list_total=="0"}
				No records were uploaded out of {$list_all} record(s)&nbsp;
			{else}
				Uploaded {$list_total} of {$list_all} record(s)&nbsp;
			{/if}
		{elseif ($is_searched=="1")}
	        	{$list_total|default:"0"} Result(s) found.&nbsp;
		{/if}
		</div>
	{/if}

	<!--Tablebody-->
	{if $is_searched=="1"}
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		  <tr>	
           	   <td width="150" class="label">Customer Type</td>
           	   <td width="100" class="label">MSISDN Fr</td>
           	   <td width="100" class="label">MSISDN To</td>
           	   <td width="300" class="label">Status</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr>
		   <td class="child" width="150">&nbsp;{$ldata.customer_type} </td>
		   <td class="child" width="100">&nbsp;{$ldata.msisdn_fr}   </td>
		   <td class="child" width="100">&nbsp;{$ldata.msisdn_to}   </td>
		   <td class="child" width="300">&nbsp;{$ldata.status}      </td>
	   	</tr>
           	{/foreach}

           	<tr>
		   <td colspan=20 align="left" class="child">
		   {$list_nav|default:""}&nbsp;
		   </td>
           	</tr>

           	</table>
		</div>

	    {/if}
</div>