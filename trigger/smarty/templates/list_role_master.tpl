{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

      
    
    <!-- form -->
    <form name="mainform" method="post" action="list_role_master.php">

    
	   <!--Pagetitle -->
	   <div id="title">Web Role List</div>
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
           	   <td width="200" class="label"><a class="hhref" href="{$href_sort_name}">Name</a></td>
           	   <td width="200" class="label"><a class="hhref" href="{$href_sort_desc}">Description</a></td>
           	   
           	   <td width="200" class="label">Action</td>
           	</tr>
           	{foreach from=$list_data item=ldata}
           	<tr >
		   <td class="child" width="200">{$ldata.role_code}</td>
		   <td class="child" width="200">&nbsp;{$ldata.role_desc}</td>
		   <td class="child"  width="200">
			{if $ldata.role_code!="admin"}
			<a href="update_role_master.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/update.jpg"  border="0"></a>
			&nbsp;
			<a href="delete_role_master.php?uid={php}echo get_uid();{/php}&id={$ldata.id}"><img src="images/delete.jpg"  border="0"></a>
			{else}
			&nbsp;
			{/if}
		   </td>
	   	</tr>
           	{/foreach}
           	<tr>
		   <td colspan=20 align="left" class="child">
		   {$list_nav|default:""}&nbsp;
		   </td>
           	</tr>

           </table>
	</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			   <a href="add_role_master.php?uid={php}echo get_uid();{/php}"><img src="images/add.jpg"  border="0"></a>
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
