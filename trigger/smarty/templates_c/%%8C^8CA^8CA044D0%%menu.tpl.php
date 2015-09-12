<?php /* Smarty version 2.6.20, created on 2010-12-04 18:53:03
         compiled from menu.tpl */ ?>
<!--Topnavigation -->
<div id="nav">
<table border="0" cellspacing="0" cellpadding="0">
<tr>
<?php if ($this->_tpl_vars['is_log_in'] == 1): ?>
<td>
<div>
	<a class="bhref" href="#" onClick="window.location='index.php'">Home</a> 
</div>
</td>  

<td>
<div>
	<a class="bhref"  href="#" onClick="return clickreturnvalue()" onMouseover="dropdownmenu(this,event, menu1, '180px')" onMouseout="delayhidemenu()">User Administration</a>
</div>
</td>  

<td>
<div>
	<a class="bhref" href="#"  onClick="return clickreturnvalue()" onMouseover="dropdownmenu(this,event, menu2, '230px')" onMouseout="delayhidemenu()">Application Management</a>
</div>
</td>  

<td>
<div>
	<a class="bhref" href="#"  onClick="return clickreturnvalue()" onMouseover="dropdownmenu(this,event, menu3, '230px')" onMouseout="delayhidemenu()">IR Enrollments</a>
</div>
</td>  

<td>
<div>
	<a class="bhref" href="#"  onClick="return clickreturnvalue()" onMouseover="dropdownmenu(this,event, menu4, '230px')" onMouseout="delayhidemenu()">Transactions</a>
</div>
</td>  

<td>
<div>
	<a class="bhref" href="#"  onClick="window.location='list_audit_log.php'">Audit Log</a> 
</div>
</td>  


<td>
<div>
	<a class="bhref" href="#"  onClick="window.location='logout.php'">Logout</a> 
</div>
</td>  
<?php else: ?>

<?php endif; ?>
</tr>
</table>
</div>
<!--Topnavigation -->