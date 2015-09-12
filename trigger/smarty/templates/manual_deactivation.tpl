{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

         
    
    <!-- form -->
    <form name="mainform" method="post" action="manual_deactivation.php">

    
	<!--Pagetitle -->
	<div id="title">Manual Deactivation</div>
		{if ($is_searched == "1") && ($error_msg == "SAVING_DEACTIVATION_SUCCESS")}
			<div id="errorbox">
			<br/>
			DEACTIVATION successfully entered...
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
				<td width="150" class="label">Representative's Name</td>
				<td class="child"><input type="text" name="q_name" maxlength="60" style="width:400px" /></td>
				</tr>	
				<tr>	
				<td width="150" class="label">Contact Number</td>
				<td class="child"><input type="text" name="q_contact" maxlength="60"  /></td>
				</tr>	
                                <tr>
                                <td width="150" class="label">Remarks</td>
				<td class="child"><textarea rows="2" cols="60" name="q_remarks"></textarea></td>
                                <!-- td class="child"><input type="text" name="q_remarks" maxlength="220" style="width:400px" /></td -->
                                </tr>
				<tr>	
				<td width="100" class="label">&nbsp;</td>
				<td colspan="3" class="child">
					<input type="submit" name="btnSubmit" value="Deactivate" />
					&nbsp;
					<!-- input type="reset" name="btnReset" value="Clear" /></td -->
		           		<input type="reset"  name="btnCancel" value="Clear" onclick="clearOnUpdateForm();"/>
				</tr>
			</table>
			</div>
		
			<!--Tablebody-->
		{/if}
   
    </form>
    <!-- form -->

    
  <!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}
