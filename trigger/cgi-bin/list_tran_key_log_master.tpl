{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

         
    
    <!-- form -->
    <form name="mainform" method="post" action="list_tran_log_master.php">

    
	   <!--Pagetitle -->
	   <div id="title">Transaction Key Log List</div>
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
			<td width="200" class="label"><a class="hhref" href="{$href_sort_tran_id}">Tran ID</a></td>
			<td width="200" class="label"><a class="hhref" href="{$href_sort_key_code}">Key Code</a></td>
			<td width="200" class="label"><a class="hhref" href="{$href_sort_key_value}">Key Value</a></td>
	   	</tr>
           	{foreach from=$list_data item=ldata}
			<tr >
			<td class="child" width="200">&nbsp;{$ldata.tran_id}</td>
			<td class="child" width="200">&nbsp;{$ldata.key_code}</td>
			<td class="child" width="200">&nbsp;{$ldata.key_value}</td>
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
