{* header *}
{include file="header.tpl"}
{* menu *}
{include file="menu.tpl"}



<!-- form -->
<form name="mainform" method="post" action="login.php">


<!--Pagetitle -->

{if $is_log_in == ""}
<div id="title">Login</div>

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
<td class="label">Username</td>
<td class="child"><input type="text" name="user" value=""/></td>
</tr>
<tr>
<td class="label">Password</td>
<td class="child"><input type="password" name="pass" value=""/></td>
</tr>
<tr>
<td class="label">&nbsp;</td>
<td class="child"><input type="submit" name="btnSubmit" value="Login"/></td>
</tr>

{if ($error_msg == "Sorry, your password already expired.") || ($error_msg == "Sorry, you need to change your password. Click Change Password link below.")}
<tr>
<td class="label">&nbsp;</td>
<td class="child">
		<a href="change_user_pass.php?uid={php}echo get_uid();{/php}" title="Admin Page" rel="gb_page_center[{$popup_win_width}, {$popup_win_height}]">Change Password</a>

</td>
</tr>
{/if}



</table>


{else}
<div id="title">Home</div>
<!-- status msg -->
{if $error_msg != ""}
<div id="errorbox">
<br/>
{$error_msg}
<br/>
</div>
{/if}
<div style="height:420px">
<br/><br/><br/><br/>
Welcome, {php}echo $_SESSION["session.smartygold.profile.user"];{/php}, you are now logged in!
		<br/><br/><br/><br/>
		<a class="hhref" href="change_user_pass.php?uid={php}echo get_uid();{/php}" title="Admin Page" rel="gb_page_center[{$popup_win_width}, {$popup_win_height}]">Change Password</a>
		</div>


{/if}


</div>


</form>
<!-- form -->


</div>
<!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}
