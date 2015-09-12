{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

         
    
    <!-- form -->
    <form name="mainform" method="post" action="manual_activation.php">

	<!--Pagetitle -->
	<div id="title">Manual Activation</div>
		{if ($is_searched == "1") && ($error_msg == "SAVING_ACTIVATION_SUCCESS")}
			<div id="errorbox">
			<br/>
			ACTIVATION successfully entered...
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
				<td width="150" class="label">Start Date <br>(YYYY-MM-DD)</td>
				<td width="100" class="child"><input type="text" name="q_sdate" maxlength="10" /></td>
				</tr>
				<tr>	
				<td width="150" class="label">Duration</td>
				<td class="child"><input type="text" name="q_duration" maxlength="20" style='width:50px' /></td>
				</tr>	
				<tr>
                                <td width="150" class="label">Override Balance Check?</td>
                                <td class="child"><input type='checkbox' name="q_balchk" value="0" style='display:block'/>
				</td>
				</tr>
				<tr>
                                <td width="150" class="label">Override Breathing Period?</td>
                                <td class="child"><input type='checkbox' name="q_bperiod" value="0" style='display:block'/>
				</td>
				</tr>
                                <tr>
                                <td width="150" class="label">Remarks</td>
				<td class="child"><textarea rows="2" cols="60" name="q_remarks"></textarea>
                                <!-- <td class="child"><input type="text" name="q_remarks" maxlength="220" style="width:400px" /> -->
				</td>
                                </tr>
				<tr>	
				<td width="100" class="label">&nbsp;</td>
				<td colspan="3" class="child">
					<input type="submit" name="btnSubmit" value="Activate" />
					&nbsp;
					<!-- input type="reset" name="btnReset" value="Clear" /></td -->
		           		<input type="reset"  name="btnCancel" value="Clear" onclick="clearOnUpdateForm();"/>
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
