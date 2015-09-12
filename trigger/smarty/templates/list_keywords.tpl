{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

      
    
    <!-- form -->
    <form name="mainform" method="get" action="list_keywords.php">

    
	   <!--Pagetitle -->
	   <div id="title">Keyword List</div>

           <!-- status msg -->
	   {if $error_msg != ""}
	    	<div id="errorbox">
	    	<br/>
	    	    {$error_msg}
	    	    <br/>
	    	</div>
	   {else}
           	<div id="desc">
	   	{if $error_msg != "DELETE_KEYWORD_SUCCESS"}
			{if $is_searched=="1"}
	   	             {$list_total|default:"0"} Result(s) found.&nbsp;
			{/if}
	   	{/if}
	   	</div>
	   {/if}

	         <div id="wrapper">
				<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
				<tr>	
				<td width="150" class="label">Keyword</td>
				<!-- td width="150" class="child"><input type="text" name="scode" maxlength="10"/></td -->
                      		<td width="150" class='child'>
					<select name="scode_l" id="scode_l">
						{html_options options=$soptkeyword selected=$q_scode_l}
					</select>  
				</td>
				<td width="150" class="label">Sub-Keyword</td>
				<!-- td width="150" class="child"><input type="text" name="ssubcode" maxlength="10"/></td -->
                      		<td width="150" class='child'>
					<select name="ssubcode" id="ssubcode">
						{html_options options=$soptsubkeyword selected=$q_ssubcode}
					</select>  
				</td>
				</tr>
				<tr>
				<td width="150" class="label">Description</td>
				<td width="150" class="child"><input type="text" name="sname" maxlength="60" style="width:200px" value="{$q_sname}"/></td>
				<td width="150" class="label">Status</td>
                      		<td width="150" class="child">
					<select name="sstat" id="sstat">
						{html_options options=$soptstatus selected=$q_sstat}
					</select>  
				</td>
				</tr>
				<tr>	
				<td width="100" class="label">&nbsp;</td>
				<td width="100" class="child"><input type="checkbox" value="1" name="sfile"/>Generate CSV FILE</td>
				<td width="100" class="label">&nbsp;</td>
				<td width="100" class="child">
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
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_code}&scode_l={$q_scode_l}&ssubcode={$q_ssubcode}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Keyword</a></td>
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_name}&scode_l={$q_scode_l}&ssubcode={$q_ssubcode}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Sub-Keyword</a></td>
           	   <td width="200" class="label"><a class="hhref" href="{$href_sort_desc}&scode_l={$q_scode_l}&ssubcode={$q_ssubcode}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Description</a></td>
           	   <td width="50"  class="label"><a class="hhref" href="{$href_sort_status}&scode_l={$q_scode_l}&ssubcode={$q_ssubcode}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Status</a></td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child" width="100">&nbsp;{$ldata.keyword}</td>
		   <td class="child" width="100">&nbsp;{$ldata.sub_keyword}</td>
		   <td class="child" width="200">&nbsp;{$ldata.keyword_desc}</td>
		   <td class="child" width="50" >&nbsp;{$ldata.status}</td>
		   <td class="child" width="200">
			<a href="update_keyword.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_keyword.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/delete.jpg"  border="0"></a>
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
			   <a href="add_keyword.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
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
