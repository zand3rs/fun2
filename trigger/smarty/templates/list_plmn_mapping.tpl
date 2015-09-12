{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

      
    
    <!-- form -->
    <form name="mainform" method="get" action="list_plmn_mapping.php">


	   <!--Pagetitle -->
	   <div id="title">PLMN Mapping List</div>

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
				<td width="150" class="label">Country Code</td>
				<!-- td width="150" class="child"><input type="text" name="scode_l" maxlength="60" style="width:200px" value="{$q_scode_l}"/></td -->
                                <td width="150" class='child'>
                                        <select name="scode_l" id="scode_l">
                                                {html_options options=$soptcountry selected=$q_scode_l}
                                        </select>
                                </td>
				<td width="150" class="label">Carrier</td>
				<td width="150" class="child"><input type="text" name="sname" maxlength="60" style="width:200px" value="{$q_sname}"/></td>
				</tr>
				<tr>
				<td width="150" class="label">Hot</td>
                      		<td width="150" class='child'>
					<select name="shot" id="shot">
						{html_options options=$sopthot selected=$q_shot}
					</select>  
				</td>
				<td width="150" class="label">Status</td>
                      		<td width="150" class='child'>
					<select name="sstat" id="sstat">
						{html_options options=$soptstatus selected=$q_stat}
					</select>  
				</td>
				</tr>
				<tr>	
				<td width="150" class="label">&nbsp;</td>
				<td width="150" class="child"><input type="checkbox" value="1" name="sfile"/>Generate CSV FILE</td>
				<td width="150" class="label">&nbsp;</td>
				<td width="150" class="child">
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
           	   <td width="150" class="label"><a class="hhref" href="{$href_sort_code}&scode={$q_scode}&sname={$q_sname}&shot={$q_shot}&sstat={$q_sstat}&wsort={$q_ssort}">Country Code</a></td>
           	   <td width="200" class="label"><a class="hhref" href="{$href_sort_name}&scode={$q_scode}&sname={$q_sname}&shot={$q_shot}&sstat={$q_sstat}&wsort={$q_ssort}">Carrier</a></td>
           	   <td width="150" class="label"><a class="hhref" href="{$href_sort_addr}&scode={$q_scode}&sname={$q_sname}&shot={$q_shot}&sstat={$q_sstat}&wsort={$q_ssort}">Address</a></td>
           	   <td width="50"  class="label"><a class="hhref" href="{$href_sort_hot}&scode={$q_scode}&sname={$q_sname}&shot={$q_shot}&sstat={$q_sstat}&wsort={$q_ssort}">Hot</a></td>
           	   <td width="75"  class="label"><a class="hhref" href="{$href_sort_status}&scode={$q_scode}&sname={$q_sname}&shot={$q_shot}&sstat={$q_sstat}&wsort={$q_ssort}">Status</a></td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child" width="150">&nbsp;{$ldata.country_code}</td>
		   <td class="child" width="200">&nbsp;{$ldata.carrier}</td>
		   <td class="child" width="150">&nbsp;{$ldata.addr}</td>
		   <td class="child" width="50" >&nbsp;{$ldata.hot}</td>
		   <td class="child" width="75" >&nbsp;{$ldata.status}</td>
		   <td class="child" width="200">
			<a href="update_plmn_mapping.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_plmn_mapping.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/delete.jpg"  border="0"></a>
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
			   <a href="add_plmn_mapping.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
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
