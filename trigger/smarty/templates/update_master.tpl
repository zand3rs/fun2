{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

           
    
    <!-- form -->
    <form name="mainform" method="post" action="{$form_action}">




<input type="hidden" name="{$form_primary}"      value="{$id}"/>
<input type="hidden" name="uid"                  value="{php}echo get_uid();{/php}"/>


    
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
	<td class="label">{$master_field.title}</td>
	<td class="child">
		{if $master_field.type == "text"}
			<input type="{$master_field.type}" name="{$master_field.name}" value="{$master_field.value|escape:"html"}" style="width:{$master_field.width}px" maxlength="{$master_field.max}" "{$master_field.read_only}"/>
		{elseif $master_field.type == "password"}
			<input type="{$master_field.type}" name="{$master_field.name}" value="{$master_field.value|escape:"html"}" maxlength="{$master_field.max}"/>
		{elseif $master_field.type == "select"}
			<select name="{$master_field.name}" {$master_field.js} id="{$master_field.name}" >
				{html_options options=$master_field.select_options selected=$master_field.select_selected}
			</select>  
		{elseif $master_field.type == "check"}
			<input type="checkbox" name="{$master_field.name}" value="1" {if $master_field.value == "1"}checked=true{/if}/>
		{elseif $master_field.type == "textarea"}
			<textarea rows = "{$master_field.rows}" cols="{$master_field.cols}" name="{$master_field.name}">{$master_field.value}</textarea>
		{/if}
	</td>
	</tr>
	{/foreach}

	</table>
	</div>
    		 <!--buttons-->
		      <div id="buttonh">
		        <table border="0" cellspacing="0" cellpadding="0">
		          <tr>
			   <td colspan=20 align="left">
			   <input type="submit" name="btnSubmit" value="{$form_submit}"/>
		           <input type="reset"  name="btnCancel" value="Reset" onclick="clearOnUpdateForm();"/>
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
