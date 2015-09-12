{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

         
    
    <!-- form -->
    <form name="mainform" method="post" action="manual_retry.php">

	<!--Pagetitle -->
	<div id="title">Manual Retry of failed Transaction</div>
		{if ($is_searched == "1") && ($error_msg == "SAVING_RETRY_SUCCESS")}
			<div id="errorbox">
			<br/>
			RETRY of failed transaction successfully entered...
			<br/>
			</div>
		{else}
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
				<td width="150" class="label">MSISDN <br>(Ex. 63917xxxxxxx)</td>
				<td class="child"><input type="text" name="q_msisdn" maxlength="20"  /></td>
				</tr>
				<tr>
				<td width="150" class="label">Profile</td>
			                   		<td width="150" class='label'>
					<select name="q_service" id="q_profile">
						{html_options options=$soptcustomer selected='%'}
					</select>  
				</td>
				</tr>	
                                <tr>
                                <td width="150" class="label">Remarks</td>
                                <td class="child"><input type="text" name="q_remarks" maxlength="220" style="width:400px" /></td>
                                </tr>
				<tr>	
				<td width="100" class="label">&nbsp;</td>
				<td colspan="3" class="child"><input type="submit" name="btnSubmit" value="Save" />
					&nbsp;
				<input type="reset" name="btnReset" value="Clear" /></td>
				</tr>
			</table>
			</div>
        		
			<!--Tablebody-->

		{/if}
	</div>
   
    </form>
    <!-- form -->

    
  <!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}
