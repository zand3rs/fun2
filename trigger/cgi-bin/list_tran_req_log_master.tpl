{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

         
    
    <!-- form -->
    <form name="mainform" method="post" action="list_tran_log_master.php">

    
	   <!--Pagetitle -->
	   <div id="title">Transfer Request Log List</div>
           <div id="desc">
                {$list_total|default:"0"} Result(s) found.&nbsp;
	   </div>
	   
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
			<td width="200" class="label"><a class="hhref" href="{$href_sort_appref_id}">App Ref ID</a></td>
			<td width="200" class="label"><a class="hhref" href="{$href_sort_tran_id}">Tran ID</a></td>
			<td width="200" class="label"><a class="hhref" href="{$href_sort_tran_dt}">Tran Date</a></td>
			<td width="200" class="label"><a class="hhref" href="{$href_sort_tran_amt}">Tran Amt</a></td>
			<td width="200" class="label"><a class="hhref" href="{$href_sort_gsmnum}">Phone</a></td>
			<td width="200" class="label">Result Code</td>
			<td width="200" class="label">App Name</td>
			<td width="200" class="label">App Packet</td>
			<td width="200" class="label">Error Code</td>
			<td width="200" class="label">Created Date</td>
	   	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
           	   <td class="child" width="200">&nbsp;{$ldata.app_ref_id}</td>
           	   <td class="child" width="200">&nbsp;{$ldata.tran_id}</td>
		   <td class="child" width="200">&nbsp;{$ldata.tran_dt}</td>
		   <td class="child" width="200">&nbsp;{$ldata.tran_amt}</td>
		   <td class="child" width="200">&nbsp;{$ldata.gsmnum}</td>
		   <td class="child" width="200">&nbsp;{$ldata.result_code}</td>
		   <td class="child" width="200">&nbsp;{$ldata.app_name}</td>
		   <td class="child" width="200">&nbsp;{$ldata.app_packet}</td>
		   <td class="child" width="200">&nbsp;{$ldata.error_code}</td>
		   <td class="child" width="200">&nbsp;{$ldata.dt_created}</td>
	   	</tr>
           	{/foreach}
           	<tr>
		   <td colspan=20 align="left" class="child">
		   {$list_nav|default:""}&nbsp;
		   </td>
           	</tr>

           </table>
	</div>
    
    </form>
    <!-- form -->

    
  </div>
  <!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}
