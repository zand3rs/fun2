<?php
$menu_spacer = "&nbsp;&nbsp;&nbsp;";
$menu_uid    = 'muid='.get_uid();
$ctr         = 0;
$can_view    = 1;
?>
//user mgmt & web-roles
var menu1=new Array();
<?
if($can_view)
{?>

menu1[<?=++$ctr?>]='<a href="#">Web User Management</a>';
menu1[<?=++$ctr?>]='<a href="list_user_master.php?<?=$menu_uid?>"><?=$menu_spacer?>View Web User</a>';
menu1[<?=++$ctr?>]='<a href="add_user_master.php?<?=$menu_uid?>"><?=$menu_spacer?>Add Web User</a>';
menu1[<?=++$ctr?>]='<a href="reset_user_pass.php?uid=<?php echo get_uid();?>" title="Admin Page" rel="gb_page_center[{$popup_win_width}, {$popup_win_height}]"><?=$menu_spacer?>Reset User Password</a>';

menu1[<?=++$ctr?>]='<a href="#">Web Role Management</a>';
menu1[<?=++$ctr?>]='<a href="list_role_master.php?<?=$menu_uid?>"><?=$menu_spacer?>View Web Role</a>';
menu1[<?=++$ctr?>]='<a href="add_role_master.php?<?=$menu_uid?>"><?=$menu_spacer?>Add Web Role</a>';

menu1[<?=++$ctr?>]='<a href="#">ACL Management</a>';
menu1[<?=++$ctr?>]='<a href="list_acl_master.php?<?=$menu_uid?>"><?=$menu_spacer?>View ACL</a>';
menu1[<?=++$ctr?>]='<a href="add_acl_master.php?<?=$menu_uid?>"><?=$menu_spacer?>Add ACL</a>';

<?}?>

//user mgmt & web-roles
var menu2=new Array();
<?
if($can_view)
{?>
menu2[<?=++$ctr?>]='<a href="list_services.php?<?=$menu_uid?>"><?=$menu_spacer?>Services</a>';
menu2[<?=++$ctr?>]='<a href="list_customers.php?<?=$menu_uid?>"><?=$menu_spacer?>Customer Types</a>';
menu2[<?=++$ctr?>]='<a href="list_custtype_mapping.php?<?=$menu_uid?>"><?=$menu_spacer?>Customer Type Mapping</a>';
menu2[<?=++$ctr?>]='<a href="list_keywords.php?<?=$menu_uid?>"><?=$menu_spacer?>Keywords</a>';
menu2[<?=++$ctr?>]='<a href="list_keyword_msgs.php?<?=$menu_uid?>"><?=$menu_spacer?>Messages</a>';
menu2[<?=++$ctr?>]='<a href="list_zones.php?<?=$menu_uid?>"><?=$menu_spacer?>Zones</a>';
menu2[<?=++$ctr?>]='<a href="list_countries.php?<?=$menu_uid?>"><?=$menu_spacer?>Countries</a>';
menu2[<?=++$ctr?>]='<a href="#">Servers</a>';
menu2[<?=++$ctr?>]='<a href="list_hlrsrvr.php?<?=$menu_uid?>"><?=$menu_spacer?>HLRs</a>';
menu2[<?=++$ctr?>]='<a href="list_hlrsrvr_mapping.php?<?=$menu_uid?>"><?=$menu_spacer?>HLRs Mapping</a>';
menu2[<?=++$ctr?>]='<a href="list_insrvr.php?<?=$menu_uid?>"><?=$menu_spacer?>INs</a>';
menu2[<?=++$ctr?>]='<a href="list_insrvr_mapping.php?<?=$menu_uid?>"><?=$menu_spacer?>INs Mapping</a>';
menu2[<?=++$ctr?>]='<a href="list_sset_mapping.php?<?=$menu_uid?>"><?=$menu_spacer?>SSET Mapping</a>';
menu2[<?=++$ctr?>]='<a href="list_plmn_mapping.php?<?=$menu_uid?>"><?=$menu_spacer?>PLMN Mapping</a>';
menu2[<?=++$ctr?>]='<a href="list_hlrimsi_mapping.php?<?=$menu_uid?>"><?=$menu_spacer?>IMSI Mapping</a>';
<?}?>

//user mgmt & web-roles
var menu3=new Array();
<?
if($can_view)
{?>
menu3[<?=++$ctr?>]='<a href="#">Manual</a>';
menu3[<?=++$ctr?>]='<a href="manual_activation.php?<?=$menu_uid?>"><?=$menu_spacer?>Activation</a>';
menu3[<?=++$ctr?>]='<a href="manual_deactivation.php?<?=$menu_uid?>"><?=$menu_spacer?>Deactivation</a>';
menu3[<?=++$ctr?>]='<a href="manual_extension.php?<?=$menu_uid?>"><?=$menu_spacer?>Extension</a>';
menu3[<?=++$ctr?>]='<a href="manual_reactivation.php?<?=$menu_uid?>"><?=$menu_spacer?>Re-Activation</a>';
menu3[<?=++$ctr?>]='<a href="manual_blacklist.php?<?=$menu_uid?>"><?=$menu_spacer?>Blacklist</a>';
menu3[<?=++$ctr?>]='<a href="manual_whitelist.php?<?=$menu_uid?>"><?=$menu_spacer?>Whitelist</a>';
menu3[<?=++$ctr?>]='<a href="manual_fun_activation.php?<?=$menu_uid?>"><?=$menu_spacer?>Fun Activation</a>';
menu3[<?=++$ctr?>]='<a href="manual_cancel.php?<?=$menu_uid?>"><?=$menu_spacer?>Fun Deactivation</a>';
menu3[<?=++$ctr?>]='<a href="#">Bulk</a>';
menu3[<?=++$ctr?>]='<a href="bulk_activation.php?<?=$menu_uid?>"><?=$menu_spacer?>Bulk Activation</a>';
menu3[<?=++$ctr?>]='<a href="bulk_deactivation.php?<?=$menu_uid?>"><?=$menu_spacer?>Bulk Deactivation</a>';
menu3[<?=++$ctr?>]='<a href="bulk_fun_activation.php?<?=$menu_uid?>"><?=$menu_spacer?>Bulk Fun Activation</a>';
menu3[<?=++$ctr?>]='<a href="bulk_fun_deactivation.php?<?=$menu_uid?>"><?=$menu_spacer?>Bulk Fun Deactivation</a>';
menu3[<?=++$ctr?>]='<a href="bulk_blacklisting.php?<?=$menu_uid?>"><?=$menu_spacer?>Bulk Blacklisting</a>';
menu3[<?=++$ctr?>]='<a href="bulk_whitelisting.php?<?=$menu_uid?>"><?=$menu_spacer?>Bulk Whitelisting</a>';
<?}?>

//user mgmt & web-roles
var menu4=new Array();
<?
if($can_view)
{?>
menu4[<?=++$ctr?>]='<a href="#">Trigger</a>';
menu4[<?=++$ctr?>]='<a href="list_trigger_subs.php?<?=$menu_uid?>"><?=$menu_spacer?>Subscribers</a>';
menu4[<?=++$ctr?>]='<a href="list_trigger_tran.php?<?=$menu_uid?>"><?=$menu_spacer?>Transaction Log</a>';
menu4[<?=++$ctr?>]='<a href="list_trigger_subs_hist.php?<?=$menu_uid?>"><?=$menu_spacer?>Subscribers History</a>';
menu4[<?=++$ctr?>]='<a href="#">FUN</a>';
menu4[<?=++$ctr?>]='<a href="list_fun_subs.php?<?=$menu_uid?>"><?=$menu_spacer?>Subscribers</a>';
menu4[<?=++$ctr?>]='<a href="list_fun_tran.php?<?=$menu_uid?>"><?=$menu_spacer?>Transaction Log</a>';
menu4[<?=++$ctr?>]='<a href="#">Others</a>';
menu4[<?=++$ctr?>]='<a href="list_blacklisted.php?<?=$menu_uid?>"><?=$menu_spacer?>Blacklisted</a>';
menu4[<?=++$ctr?>]='<a href="list_whitelisted.php?<?=$menu_uid?>"><?=$menu_spacer?>Whitelisted</a>';
menu4[<?=++$ctr?>]='<a href="list_blackwhite.php?<?=$menu_uid?>"><?=$menu_spacer?>Blacklist/Whitelist History</a>';
<?}?>


var menuwidth='180px' //default menu width
var menubgcolor='#333333'  //menu bgcolor
var disappeardelay=250  //menu disappear speed onMouseout (in miliseconds)
var hidemenu_onclick="yes" //hide menu when user clicks within menu?

/////No further editting needed

var ie4=document.all
var ns6=document.getElementById&&!document.all

if (ie4||ns6)
document.write('<div id="dropmenudiv" style="visibility:hidden;width:'+menuwidth+';background-color:'+menubgcolor+'" onMouseover="clearhidemenu()" onMouseout="dynamichide(event)"></div>')

function getposOffset(what, offsettype){
var totaloffset=(offsettype=="left")? what.offsetLeft : what.offsetTop;
var parentEl=what.offsetParent;
while (parentEl!=null){
totaloffset=(offsettype=="left")? totaloffset+parentEl.offsetLeft : totaloffset+parentEl.offsetTop;
parentEl=parentEl.offsetParent;
}
return totaloffset;
}


function showhide(obj, e, visible, hidden, menuwidth){
if (ie4||ns6)
dropmenuobj.style.left=dropmenuobj.style.top="-500px"
if (menuwidth!=""){
dropmenuobj.widthobj=dropmenuobj.style
dropmenuobj.widthobj.width=menuwidth
}
if (e.type=="click" && obj.visibility==hidden || e.type=="mouseover")
obj.visibility=visible
else if (e.type=="click")
obj.visibility=hidden
}

function iecompattest(){
return (document.compatMode && document.compatMode!="BackCompat")? document.documentElement : document.body
}

function clearbrowseredge(obj, whichedge){
var edgeoffset=0
if (whichedge=="rightedge"){
var windowedge=ie4 && !window.opera? iecompattest().scrollLeft+iecompattest().clientWidth-15 : window.pageXOffset+window.innerWidth-15
dropmenuobj.contentmeasure=dropmenuobj.offsetWidth
if (windowedge-dropmenuobj.x < dropmenuobj.contentmeasure)
edgeoffset=dropmenuobj.contentmeasure-obj.offsetWidth
}
else{
var topedge=ie4 && !window.opera? iecompattest().scrollTop : window.pageYOffset
var windowedge=ie4 && !window.opera? iecompattest().scrollTop+iecompattest().clientHeight-15 : window.pageYOffset+window.innerHeight-18
dropmenuobj.contentmeasure=dropmenuobj.offsetHeight
if (windowedge-dropmenuobj.y < dropmenuobj.contentmeasure){ //move up?
edgeoffset=dropmenuobj.contentmeasure+obj.offsetHeight
if ((dropmenuobj.y-topedge)<dropmenuobj.contentmeasure) //up no good either?
edgeoffset=dropmenuobj.y+obj.offsetHeight-topedge
}
}
return edgeoffset
}

function populatemenu(what){
if (ie4||ns6)
dropmenuobj.innerHTML=what.join("")
}


function dropdownmenu(obj, e, menucontents, menuwidth){
if (window.event) event.cancelBubble=true
else if (e.stopPropagation) e.stopPropagation()
clearhidemenu()
dropmenuobj=document.getElementById? document.getElementById("dropmenudiv") : dropmenudiv
populatemenu(menucontents)

if (ie4||ns6){
showhide(dropmenuobj.style, e, "visible", "hidden", menuwidth)
dropmenuobj.x=getposOffset(obj, "left")
dropmenuobj.y=getposOffset(obj, "top")
dropmenuobj.style.left=dropmenuobj.x-clearbrowseredge(obj, "rightedge")+"px"
dropmenuobj.style.top=dropmenuobj.y-clearbrowseredge(obj, "bottomedge")+obj.offsetHeight+"px"
}

return clickreturnvalue()
}

function clickreturnvalue(){
if (ie4||ns6) return false
else return true
}

function contains_ns6(a, b) {
while (b.parentNode)
if ((b = b.parentNode) == a)
return true;
return false;
}

function dynamichide(e){
if (ie4&&!dropmenuobj.contains(e.toElement))
delayhidemenu()
else if (ns6&&e.currentTarget!= e.relatedTarget&& !contains_ns6(e.currentTarget, e.relatedTarget))
delayhidemenu()
}

function hidemenu(e){
if (typeof dropmenuobj!="undefined"){
if (ie4||ns6)
dropmenuobj.style.visibility="hidden"
}
}

function delayhidemenu(){
if (ie4||ns6)
delayhide=setTimeout("hidemenu()",disappeardelay)
}

function clearhidemenu(){
if (typeof delayhide!="undefined")
clearTimeout(delayhide)
}

if (hidemenu_onclick=="yes")
document.onclick=hidemenu

function clearOnAddForm(oForm) {
  var elements = oForm.elements; 
  var myErrorBox=document.getElementById("errorbox")
  myErrorBox.style.display="none"
  oForm.reset();
  for(i=0; i<elements.length; i++) {
	field_type = elements[i].type.toLowerCase();
	switch(field_type) {
	
		case "text": 
		case "password": 
		case "textarea":
	        case "hidden":	
			
			elements[i].value = ""; 
			break;
        
	        case "select":	
	        case "select-one":	
	        case "select-multi":	
			
			elements[i].selectedIndex = '%'; 
			break;

		default: 
			break;
	}
    }
}

function clearOnUpdateForm() {
  var myErrorBox=document.getElementById("errorbox")
  myErrorBox.style.display="none"
}

function clearOnListErrForm() {
   var myErrorBox=document.getElementById("errorbox")
   myErrorBox.style.display="none"
   var obj1 = document.mainform.elements['q_sdate'];
   if(obj1)
      document.mainform.q_sdate.value='';

   var obj3 = document.mainform.elements['q_edate'];
   if(obj3)
      document.mainform.q_edate.value='';

   var obj3 = document.mainform.elements['q_msisdn'];
   if(obj3)
      document.mainform.q_msisdn.value='';

   var obj4 = document.mainform.elements['q_user_code'];
   if(obj4)
      document.mainform.q_user_code.value='';

   var obj5 = document.mainform.elements['q_status'];
   if(obj5)
      document.mainform.q_status.value='ALL';

   var obj6 = document.mainform.elements['scode'];
   if(obj6)
      document.mainform.scode.value='';

   var obj7 = document.mainform.elements['sname'];
   if(obj7)
      document.mainform.sname.value='';

   var obj8 = document.mainform.elements['sstat'];
   if(obj8)
      document.mainform.sstat.value='ALL';

   var obj9 = document.mainform.elements['sname_l'];
   if(obj9)
      document.mainform.sname_l.value='%';

   var obj10 = document.mainform.elements['scode_l'];
   if(obj10)
      document.mainform.scode_l.value='%';

   var obj11 = document.mainform.elements['ssubcode'];
   if(obj11)
      document.mainform.ssubcode.value='%';

   var obj12 = document.mainform.elements['scust'];
   if(obj12)
      document.mainform.scust.value='%';

   var obj13 = document.mainform.elements['smesg'];
   if(obj13)
      document.mainform.smesg.value='';

   var obj14 = document.mainform.elements['szone'];
   if(obj14)
      document.mainform.szone.value='0';

   var obj15 = document.mainform.elements['sname1'];
   if(obj15)
      document.mainform.sname1.value='';

   var obj16 = document.mainform.elements['sname2'];
   if(obj16)
      document.mainform.sname2.value='';

   var obj17 = document.mainform.elements['shot'];
   if(obj17)
      document.mainform.shot.value='ALL';


}

function clearOnSearchForm() {
   var obj1 = document.mainform.elements['q_sdate'];
   if(obj1)
      document.mainform.q_sdate.value='';

   var obj2 = document.mainform.elements['q_edate'];
   if(obj2)
      document.mainform.q_edate.value='';

   var obj3 = document.mainform.elements['q_msisdn'];
   if(obj3)
      document.mainform.q_msisdn.value='';

   var obj4 = document.mainform.elements['q_user_code'];
   if(obj4)
      document.mainform.q_user_code.value='';

   var obj5 = document.mainform.elements['q_status'];
   if(obj5)
      document.mainform.q_status.value='ALL';

   var obj6 = document.mainform.elements['scode'];
   if(obj6)
      document.mainform.scode.value='';

   var obj7 = document.mainform.elements['sname'];
   if(obj7)
      document.mainform.sname.value='';

   var obj8 = document.mainform.elements['sstat'];
   if(obj8)
      document.mainform.sstat.value='ALL';

   var obj9 = document.mainform.elements['sname_l'];
   if(obj9)
      document.mainform.sname_l.value='%';

   var obj10 = document.mainform.elements['scode_l'];
   if(obj10)
      document.mainform.scode_l.value='%';

   var obj11 = document.mainform.elements['ssubcode'];
   if(obj11)
      document.mainform.ssubcode.value='%';

   var obj12 = document.mainform.elements['scust'];
   if(obj12)
      document.mainform.scust.value='%';

   var obj13 = document.mainform.elements['smesg'];
   if(obj13)
      document.mainform.smesg.value='';

   var obj14 = document.mainform.elements['szone'];
   if(obj14)
      document.mainform.szone.value='0';

   var obj15 = document.mainform.elements['sname1'];
   if(obj15)
      document.mainform.sname1.value='';

   var obj16 = document.mainform.elements['sname2'];
   if(obj16)
      document.mainform.sname2.value='';

   var obj17 = document.mainform.elements['shot'];
   if(obj17)
      document.mainform.shot.value='ALL';

}

function confirmDelete(delUrl) {
  if (confirm("Are you sure you want to delete?")) {
    document.location = delUrl;
  }
}

function confirmDeleteCustomer(delUrl) {
   if (confirm("Are you sure you want to delete?")) {
      document.mainform.submit()
   }
}
function checkAllBox(chk) {
   if(document.mainform.Check_Rec.checked==true){
      for (i = 0; i < chk.length; i++)
         chk[i].checked = true ;
   }
   else {
      for (i = 0; i < chk.length; i++)
         chk[i].checked = false ;
   }
}

function selectAllCheckBoxes(FormName, FieldName)
{
	if(!document.forms[FormName])
		return;
	var objCheckBoxes = document.forms[FormName].elements[FieldName];
	if(!objCheckBoxes)
		return;
	var countCheckBoxes = objCheckBoxes.length;
	if(!countCheckBoxes)
		objCheckBoxes.checked = CheckValue;
	else {
		if(document.mainform.Check_Rec.checked==true){
			for(var i = 0; i < countCheckBoxes; i++)
				objCheckBoxes[i].checked = true;

   		}
   		else {
			for(var i = 0; i < countCheckBoxes; i++)
				objCheckBoxes[i].checked = false;

   		}
	}
}

/* DYNAMIC MENU HERE */
