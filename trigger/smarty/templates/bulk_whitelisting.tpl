{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

	<form action="bulk_whitelisting.php" enctype="multipart/form-data" method="post">

	   <!--Pagetitle -->
	   <div id="title">Bulk Whitelisting</div>
           <div id="desc">
		{if $list_total==0}
	                No records were uploaded.&nbsp;
		{else}
			{if $list_total > 0}
	                	{$list_total} of {$list_all} record(s) were uploaded.&nbsp;
			{/if}
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
           	   <td width="200" class="label"><input type='file' size="50" name='uploaded_file' /></td>
           	   <td width="100" class="label"><input type='submit' value='Upload' /></td>
           	</tr>
                <tr>
                <td colspan="3" class="child">Format: 639xxxxxxxxx,Remarks</td>
                </tr>
		</table>
		</div>
	</form>

	{if $list_total>=0}
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		  <tr>	
           	   <td width="100" class="label">MSISDN</td>
           	   <td width="300" class="label">Remarks</td>
           	   <td width="300" class="label">Reason</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child" width="100">{$ldata.msisdn}</td>
		   <td class="child" width="300">&nbsp;{$ldata.remarks}</td>
		   <td class="child" width="300">&nbsp;{$ldata.reason}</td>
	   	</tr>
           	{/foreach}

           	<tr>
		   <td colspan=20 align="left" class="child">
		   {$list_nav|default:""}&nbsp;
		   </td>
           	</tr>

		</table>
	    {/if}
  <!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}
