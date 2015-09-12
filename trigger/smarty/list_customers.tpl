{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

      
    
    <!-- form -->
    <form name="mainform" method="get" action="list_customers.php">


	   <!--Pagetitle -->
	   <div id="title">Customer Types List</div>

	    <!-- status msg -->
	    {if $error_msg != ""}
	    	    <div id="errorbox">
	    	    <br/>
	    	        {$error_msg}
	    	        <br/>
	    	    </div>
	   {else}
           	<div id="desc">
	   	 {if $error_msg != "DELETE_CUSTOMER_TYPE_SUCCESS"}
			{if $is_searched=="1"}
	   	             {$list_total|default:"0"} Result(s) found.&nbsp;
			{/if}
	   	{/if}
	   	</div>
	    {/if}

	         <div id="wrapper">
				<table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
				<tr>	
				<td width="150" class="label">Customer Type</td>
				<td width="150" class="child"><input type="text" name="scode" maxlength="10" value="{$q_scode}"/></td>
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
           	   <td width="200" class="label"><a class="hhref" href="{$href_sort_name}&scode={$q_scode}&sstat={$q_sstat}&wsort={$q_ssort}">Customer Type</a></td>
           	   <td width="50"  class="label">Daily Balance</td>
           	   <td width="50"  class="label">Max Duration</td>
           	   <td width="50"  class="label">Breathing Period</td>
           	   <td width="50"  class="label">Min Balance</td>
           	   <td width="50"  class="label">Extension Min Balance</td>
           	   <td width="50"  class="label">Min Voice</td>
           	   <td width="50"  class="label">SMS Threshold</td>
           	   <td width="50"  class="label">Pre-Act Day</td>
           	   <td width="50"  class="label">Max Link</td>
           	   <td width="50"  class="label">ARDS 1st Notification</td>
           	   <td width="50"  class="label">ARDS 2nd Notification</td>
           	   <td width="50"  class="label">ARDS Expiry</td>
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_desc}&scode={$q_scode}&sstat={$q_sstat}&wsort={$q_ssort}">Status</a></td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child" width="200">{$ldata.customer_type}</td>
		   <td class="child" width="50">{$ldata.daily_balance}</td>
		   <td class="child" width="50">&nbsp;{$ldata.max_duration}</td>
		   <td class="child" width="50">&nbsp;{$ldata.breathing_period}</td>
		   <td class="child" width="50">&nbsp;{$ldata.min_balance}</td>
		   <td class="child" width="50">&nbsp;{$ldata.ext_min_balance}</td>
		   <td class="child" width="50">&nbsp;{$ldata.min_voice}</td>
		   <td class="child" width="50">&nbsp;{$ldata.sms_treshold}</td>
		   <td class="child" width="50">&nbsp;{$ldata.pre_act_day}</td>
		   <td class="child" width="50">&nbsp;{$ldata.num_link}</td>
		   <td class="child" width="50">&nbsp;{$ldata.ards_1st_notify}</td>
		   <td class="child" width="50">&nbsp;{$ldata.ards_2nd_notify}</td>
		   <td class="child" width="50">&nbsp;{$ldata.ards_tmp_duration}</td>
		   <td class="child" width="100">&nbsp;{$ldata.status}</td>
		   <td class="child"  width="200">
			<a href="update_customer.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_customer.php?uid={php}echo get_uid();{/php}&id={$ldata.id}" onclick="return confirm('Are you sure you want to delete?')"><img src="images/delete.jpg"  border="0"></a>
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
			   <a href="add_customer.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
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
