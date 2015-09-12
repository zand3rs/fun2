{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

      
    
    <!-- form -->
    <form name="mainform" method="get" action="list_zones.php">

    
	   <!--Pagetitle -->
	   <div id="title">Zone List</div>

	   <!-- status msg -->
	   {if $error_msg != ""}
	    	<div id="errorbox">
	    	<br/>{$error_msg}<br/>
	    	</div>
	   {else}
           	<div id="desc">
           	     {$list_total|default:"0"} Result(s) found.&nbsp;
	   	</div>
	   {/if}

            <!--Tablebody-->
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		  <tr>	
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_name}">Zone</a></td>
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_desc}">Breathing Period</a></td>
           	   <td width="100" class="label">Max Duration</td>
           	   <td width="100" class="label"><a class="hhref" href="{$href_sort_status}">Status</a></td>
           	   <td width="200" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child" width="100">{$ldata.zone_id}</td>
		   <td class="child" width="100">&nbsp;{$ldata.breathing_period}</td>
		   <td class="child" width="100">&nbsp;{$ldata.max_duration}</td>
		   <td class="child" width="100">&nbsp;{$ldata.status}</td>
		   <td class="child"  width="200">
			<a href="update_zone.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_zone.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/delete.jpg"  border="0"></a>
		   </td>
	   	</tr>
           	{/foreach}
           	<tr>
		   <td colspan=20 align="left" class="child">
		   {$list_nav|default:""}&nbsp;
		   </td>
           	</tr>

		{if $q_csv <> "" }
		<tr>
		<td colspan=20 align="left" class="child">
		<a href="dl.php?muid={php}echo get_uid();{/php}&csv={$q_csv}">Download CSV</a>
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
			   <a href="add_zone.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
			   </td>
		          </tr>
		        </table>
		      </div>
		<!--buttons-->

    
    </form>
    <!-- form -->

    
  </div>
  <!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}
