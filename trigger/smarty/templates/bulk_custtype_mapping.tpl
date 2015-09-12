{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

	<form name='mainform' action="bulk_custtype_processing.php" enctype="multipart/form-data" method="post" target="hiddenframe">

	   <!--Pagetitle -->
	   <div id="title">Upload Customer Mapping</div>
           <!-- div id="desc">
                {$list_total|default:"0"} Result(s) found.&nbsp;
	   </divi -->
	   
	      <!-- status msg -->
	    {if $error_msg != ""}
	    	    <div id="errorbox">
	    	    <br/>
	    	        {$error_msg}
	    	        <br/>
	    	    </div>
	    {/if}
	    
	         <div id="wrapper">
	         <table border="0" cellspacing="0" cellpadding="0" id="tablestyle">
		  <tr id='upload_tr'>	
           	   <td width="200" class="label"><input type='file' size="50" name='uploaded_file' /></td>
           	   <td width="100" class="label"><input type='button' value='Upload' onclick="lockMenu();document.mainform.submit();"/></td>
           	</tr>
                <tr>
                <td colspan="3" class="child">Format: PROFILE,639xxxxxxxxx,639xxxxxxxxx</td>
                </tr>
                </table>
		</div>
	</form>
	<iframe name='hiddenframe' id = 'hiddenframe' style='width:100%;height:500px;border:none' frameborder=0 marginWidht=0 marginHeight=0></iframe>
  <!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}
