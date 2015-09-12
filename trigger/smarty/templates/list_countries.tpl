{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

      
    
    <!-- form -->
    <form name="mainform" method="get" action="list_countries.php">


	   <!--Pagetitle -->
	   <div id="title">Countries List</div>

	   <!-- status msg -->
	   {if $error_msg != ""}
	    	<div id="errorbox">
	    	<br/>{$error_msg}<br/>
	    	</div>
	   {else}
           	<div id="desc">
	   	{if $error_msg != "DELETE_COUNTRY_SUCCESS"}
			{if $is_searched=="1"}
	   	            {$list_total|default:"0"} Result(s) found.&nbsp;
			{/if}
	   	{/if}
	   	</div>
	   {/if}

	         <div id="wrapper">
				<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
				<tr>	
				<td width="150" class="label">Country Code</td>
				<td width="150" class="child"><input type="text" name="scode" maxlength="10" value="{$q_scode}"/></td>
				<td width="150" class="label">Country Name</td>
				<td width="150" class="child"><input type="text" name="sname" maxlength="30" value="{$q_sname}"/></td>
				</tr>
				<tr>
				<td width="150" class="label">Zone</td>
                      		<td width="150" class='child'>
					<select name="szone" id="szone">
						{html_options options=$soptzones selected=$q_szone}
					</select>  
				</td>
				<td width="150" class="label">Status</td>
                      		<td width="150" class='child'>
					<select name="sstat" id="sstat">
						{html_options options=$soptstatus selected=$q_sstat}
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
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_code}&scode={$q_scode}&sname={$q_sname}&sstat={$q_sstat}&szone={$q_szone}&wsort={$q_ssort}">Country Code</a></td>
           	   <td width="200" class="label"><a class="hhref" href="{$href_sort_name}&scode={$q_scode}&sname={$q_sname}&sstat={$q_sstat}&szone={$q_szone}&wsort={$q_ssort}">Country Name</a></td>
           	   <td width="70"  class="label"><a class="hhref" href="{$href_sort_zone}&scode={$q_scode}&sname={$q_sname}&sstat={$q_sstat}&szone={$q_szone}&wsort={$q_ssort}">Zone ID</a></td>
           	   <td width="300" class="label">Charge</td>
           	   <td width="50"  class="label"><a class="hhref" href="{$href_sort_status}&scode={$q_scode}&sname={$q_sname}&sstat={$q_sstat}&szone={$q_szone}&wsort={$q_ssort}">Status</a></td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child" width="100">&nbsp;{$ldata.country_code}</td>
		   <td class="child" width="200">&nbsp;{$ldata.country_name}</td>
		   <td class="child" width="70" >&nbsp;{$ldata.zone_id}</td>
		   <td class="child" width="300">&nbsp;{$ldata.charge_desc}</td>
		   <td class="child" width="50" >&nbsp;{$ldata.status}</td>
		   <td class="child" width="200">
			<a href="update_country.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_country.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/delete.jpg"  border="0"></a>
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
			   <a href="add_country.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
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
