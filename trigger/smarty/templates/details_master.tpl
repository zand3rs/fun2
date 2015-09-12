{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="pop_menu.tpl"}

         
    <!-- form -->
    <form name="mainform" method="post" >

<input type="hidden" name="id"      value="{$id}"/>
<input type="hidden" name="uid"     value="{php}echo get_uid();{/php}"/>


	   <!--Pagetitle -->
	   <div id="title">{$form_title}</div>
	   
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

 		{foreach from=$master_fields item=master_field}
           	<tr>               
           	   <td class="label" style="width:250px">{$master_field.title}</td>
           	   <td class="child" style="width:350px">{$master_field.value|escape:"html"}</td>           
           	</tr>              
           	{/foreach}
           	
           </table>
	</div>
    </form>
    <!-- form -->

    
  </div>
  <!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}