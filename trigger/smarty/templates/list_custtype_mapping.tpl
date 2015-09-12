{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

      
    
    <!-- form -->
    <form name="mainform" method="get" action="list_custtype_mapping.php">

    
	<!--Pagetitle -->
	<div id="title">Customer Type Mapping List</div>

	<!-- status msg -->
	{if $error_msg != ""}
		<div id="errorbox">
		<br/>{$error_msg}<br/>
		</div>
	{else}
		<div id="desc">
		{if $error_msg != "DELETE_CUSTOMER_TYPE_MAPPING_SUCCESS"}
			{if ($is_searched=="1") and ($list_all>0)}
				{if $list_total=="0"}
					No records were uploaded out of {$list_all} record(s)&nbsp;
				{else}
					Uploaded {$list_total} of {$list_all} record(s)&nbsp;
				{/if}
			{elseif ($is_searched=="1")}
		        	{$list_total|default:"0"} Result(s) found.&nbsp;
			{/if}
		{/if}
		</div>
	{/if}

	<div id="wrapper">
		<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		<tr>
		<td width="150" class="label">Customer Type</td>
              	<td width="150" class='child'>
			<select name="sname_l" id="sname_l">
				{html_options options=$soptcustyp selected=$q_sname_l}
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

	<!--Tablebody-->
	{if $is_searched=="1"}
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
           	   <!-- td width="20"  class="label"><input type="checkbox" name="Check_Rec" value="yes" onclick="checkAllBox(document.mainform.check_rec)"/></td -->
           	   <td width="20"  class="label"><input type="checkbox" name="Check_Rec" value="yes" onclick="selectAllCheckBoxes('mainform', 'check_rec[]')"/></td>
           	   <td width="150" class="label"><a class="hhref" href="{$href_sort_name}&scode={$q_scode}&sname_l={$q_sname_l}&sstat={$q_sstat}&wsort={$q_ssort}">Customer Type</a></td>
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_code}&scode={$q_scode}&sname_l={$q_sname_l}&sstat={$q_sstat}&wsort={$q_ssort}">MSISDN Fr</a></td>
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_desc}&scode={$q_scode}&sname_l={$q_sname_l}&sstat={$q_sstat}&wsort={$q_ssort}">MSISDN To</td>
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_status}&scode={$q_scode}&sname_l={$q_sname_l}&sstat={$q_sstat}&wsort={$q_ssort}">Status</a></td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr>
		   <td class="child" width="20" ><input type="checkbox" name="check_rec[]" value="{$ldata.id}"/></td>
		   <td class="child" width="150">&nbsp;{$ldata.customer_type} </td>
		   <td class="child" width="100">&nbsp;{$ldata.msisdn_fr}   </td>
		   <td class="child" width="100">&nbsp;{$ldata.msisdn_to}   </td>
		   <td class="child" width="100">&nbsp;{$ldata.status}      </td>
		   <td class="child" width="200">
			<a href="update_custtype_mapping.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/update.jpg" border="0"></a>
			&nbsp;
			<a href="delete_custtype_mapping.php?uid={php}echo get_uid();{/php}&id={$ldata.id}" onclick="return confirm('Are you sure you want to delete?')"><img src="images/delete.jpg" border="0"></a>
		   </td>
	   	</tr>
           	{/foreach}


		{if $q_sdall=="1"}
           	<tr>
		<td colspan=20 align="left" class="child">
	        <a href="delete_all_custtype_mapping.php?uid={php}echo get_uid();{/php}&scode={$q_scode}&sname_l={$q_sname_l}&sstat={$q_sstat}" onclick="return confirm('Are you sure you want to delete?')">Delete All Searched Items</a>
		</td>
           	</tr>
		{/if}

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
			   <a href="add_custtype_mapping.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
			   </td>
                           <td colspan=20 align="left">
                           <a href="bulk_custtype_mapping.php?uid={php}echo get_uid();{/php}"><img src="images/bulk.jpg"  border="0"></a>
                           </td>
                           <td colspan=20 align="left">
                           <input type="image" name="btnSubmit" value="Delete Multiple" onclick="return confirm('Are you sure you want to delete?')" src="images/delete_all.jpg"/>
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
