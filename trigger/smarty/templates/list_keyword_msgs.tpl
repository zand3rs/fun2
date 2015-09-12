{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

      
    
    <!-- form -->
    <form name="mainform" method="get" action="list_keyword_msgs.php">

    
	   <!--Pagetitle -->
	   <div id="title">Message List</div>

	   <!-- status msg -->
	   {if $error_msg != ""}
	    	<div id="errorbox">
	    	<br/>
	    	    {$error_msg}
	    	    <br/>
	    	</div>
	   {else}
            	<div id="desc">
	    	{if $error_msg != "DELETE_MESSAGE_SUCCESS"}
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
				<!-- td width="150" class="child"><input type="text" name="scode_l" maxlength="10"/></td -->
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
				<td width="150" class="label">Customer Type</td>
                      		<td width="150" class='child'>
					<select name="scust" id="scust">
						{html_options options=$soptcustomer selected=$q_scust}
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
                                <td width="150" class="label">Message ID</td>
                                <td colspan="3" class="child"><input type="text" name="smesg" maxlength="60"  value="{$q_smesg}" style="width:500px"/></td>
				</tr>

				<tr>	
				<td width="150" class="label">Message</td>
				<!-- td colspan="3" class="child"><input type="text" name="sname" maxlength="60" style="width:500px"/></td -->
				<td colspan="3" class="child"><textarea rows = "4" cols="60" name="sname">{$q_sname}</textarea></td>
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
           	   <td width="70"  class="label"><a class="hhref" href="{$href_sort_code}&scode_l={$q_scode}&ssubcode={$q_ssubcode}&scust={$q_scust}&smesg={$q_smesg}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Keyword</a></td>
           	   <td width="70"  class="label"><a class="hhref" href="{$href_sort_name}&scode_l={$q_scode}&ssubcode={$q_ssubcode}&scust={$q_scust}&smesg={$q_smesg}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Sub-Keyword</a></td>
           	   <td width="100"  class="label"><a class="hhref" href="{$href_sort_mesg}&scode_l={$q_scode}&ssubcode={$q_ssubcode}&scust={$q_scust}&smesg={$q_smesg}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Message ID</td>
           	   <!-- td width="70"  class="label"><a class="hhref" href="{$href_sort_cust}">Customer Type</a></td -->
           	   <!-- td width="30"  class="label"><a class="hhref" href="{$href_sort_mesg}">Message Type</a></td -->
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_desc}&scode_l={$q_scode}&ssubcode={$q_ssubcode}&scust={$q_scust}&smesg={$q_smesg}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Description</a></td>
           	   <td width="400" class="label">Message</td>
           	   <td width="50"  class="label"><a class="hhref" href="{$href_sort_status}&scode_l={$q_scode}&ssubcode={$q_ssubcode}&scust={$q_scust}&smesg={$q_smesg}&sname={$q_sname}&sstat={$q_sstat}&wsort={$q_ssort}">Status</a></td>
           	   <td width="70" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child" width="70">&nbsp;{$ldata.keyword}       </td>
		   <td class="child" width="70">&nbsp;{$ldata.sub_keyword}   </td>
		   <td class="child" width="100">&nbsp;{$ldata.message_id_disp}</td>
		   <!-- td class="child" width="70">&nbsp;{$ldata.customer_type} </td -->
		   <!-- td class="child" width="30" >&nbsp;{$ldata.message_type} </td -->
		   <td class="child" width="100">&nbsp;{$ldata.description}  </td>
		   <td class="child" width="400">&nbsp;{$ldata.msg}          </td>
		   <td class="child" width="50">&nbsp;{$ldata.status}        </td>
		   <td class="child" width="70">
			<a href="update_keyword_msg.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/update.jpg"  border="0"></a>
<!--
			&nbsp;
			<a href="delete_keyword_msg.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/delete.jpg"  border="0"></a>
-->
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
			   <a href="add_keyword_msg.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
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
