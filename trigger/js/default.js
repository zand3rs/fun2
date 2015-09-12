
/*
*	Filename	:	default.js
*	Description	:	This is a set of javascript functions that
*				are very useful on some common client
*				validation of data forms.
*	Version		:	ver1.1
*	Date		:	12-11-2001
*	Author		:	bayugyug
*/


function userCheckCompanyAgencyCode(x)
{
        
        
	if(x == 2)
	{
		document.mainform.agency_code.options[0].selected = true;     
	}
	if(x == 1)
	{
		document.mainform.company_code.options[0].selected = true;     
	}
}


function CheckAll(fmobj) {
  for (var i=0;i<fmobj.elements.length;i++) {
    var e = fmobj.elements[i];
    if ((e.type=='checkbox')) {
       e.checked = true;
      }
  }
}


function UnCheckAll(fmobj) {
  for (var i=0;i<fmobj.elements.length;i++) {
    var e = fmobj.elements[i];
    if ((e.type=='checkbox')) {
       e.checked = false;
      }
  }
}

function SetToggleLayer( whichLayer , what)
{
var elem, vis;
if( document.getElementById ) // this is the way the standards work
elem = document.getElementById( whichLayer );
else if( document.all ) // this is the way old msie versions work
elem = document.all[whichLayer];
else if( document.layers ) // this is the way nn4 works
elem = document.layers[whichLayer];
vis = elem.style;

/**
// if the style.display value is blank we try to figure it out here
if(vis.display==''&&elem.offsetWidth!=undefined&&elem.offsetHeight!=undefined)
vis.display = (elem.offsetWidth!=0&&elem.offsetHeight!=0)?'block':'none';
vis.display = (vis.display==''||vis.display=='block')?'none':'block';
**/
vis.display = what;
}


function GetToggleLayer( whichLayer )
{
var elem, vis;
if( document.getElementById ) // this is the way the standards work
elem = document.getElementById( whichLayer );
else if( document.all ) // this is the way old msie versions work
elem = document.all[whichLayer];
else if( document.layers ) // this is the way nn4 works
elem = document.layers[whichLayer];
vis = elem.style;

/**
// if the style.display value is blank we try to figure it out here
if(vis.display==''&&elem.offsetWidth!=undefined&&elem.offsetHeight!=undefined)
vis.display = (elem.offsetWidth!=0&&elem.offsetHeight!=0)?'block':'none';
vis.display = (vis.display==''||vis.display=='block')?'none':'block';
**/
return vis.display;
}


function getSelectedRadio(buttonGroup) 
{
   // returns the array number of the selected radio button or -1 if no button is selected
   if (buttonGroup[0]) { // if the button group is an array (one button is not an array)
      for (var i=0; i<buttonGroup.length; i++) {
         if (buttonGroup[i].checked) {
            return i;
         }
      }
   } else {
      if (buttonGroup.checked) { return 0; } // if the one button is checked, return zero
   }
   // if we get to this point, no radio button is selected
   return -1;
} // Ends the "getSelectedRadio" function

//------------------------------------------------------------------

function isAlphabetic(pInputVal) 
{
	var _inputStr = pInputVal.toString();
	var _char;
	for(var i = 0; i < _inputStr.length; i++) {
		_char = _inputStr.charAt(i);
		if(!((_char >= 'A' && _char <= 'Z') || (_char >= 'a' && _char <= 'z'))) {	
			return false;
		}
	}
	return true;
}


function isAlphanumeric(pInputVal) 
{
	var _inputStr = pInputVal.toString();
	var _char;
	for(var i = 0; i < _inputStr.length; i++) {
		_char = _inputStr.charAt(i);
		if(!((_char >= 'A' && _char <= 'Z') || 
		     (_char >= 'a' && _char <= 'z') ||
		     (_char >= '0' && _char <= '9'))) {	
			return false;
		}
	}
	return true;
}

function isInteger(pInputVal) 
{
	
	var _inputStr = pInputVal.toString();
	var _char;
	for(var i = 0; i < _inputStr.length; i++) {
		_char = _inputStr.charAt(i);
		if(i == 0 && _char == "-") {
			continue;
		}
		if (_char < "0" || _char > "9") {
			return false;
		}
	}
	return true;
}


function isNegativeInteger(pInputVal) {
	if (isNaN(pInputVal)) {
		return false;
	}
	if (Number(pInputVal) >= 0) {
		return false;
	}
	return isInteger(pInputVal);
}


function isNonnegativeInteger(pInputVal) 
{
	var _inputStr = pInputVal.toString();
	var _char;
	for (var i = 0; i < _inputStr.length; i++) {
		_char = _inputStr.charAt(i);
		if (_char < "0" || _char > "9") {
			return false;
		}		
	}	
	return true;	
}
function isDigit(_chars)
{
	
	
	for(var i=0; i<_chars.length;i++)
	{
		if (! ((_chars.charCodeAt(i) >= 48) && (_chars.charCodeAt(i) <= 57)))
			return false;
		
	}
	if(_chars.length == 0) return false;
	return true;
}


function checkAlphaNumeric(_value)	
{			
	
	if(_value.length == 0) return false;
	for(var i=0;i<_value.length;i++)
	{
		if (!( (_value.charCodeAt(i) >=48 && 
		       _value.charCodeAt(i) <=57 ) ||  
		      (_value.charCodeAt(i) >= 97 && 
		       _value.charCodeAt(i) <= 122) ||
		      (_value.charCodeAt(i) >= 65 && 
		       _value.charCodeAt(i) <= 90) ||
		      (_value.charCodeAt(i) == 44) || 
		      (_value.charCodeAt(i) == 45) ||
		      (_value.charCodeAt(i) == 46) ||
		      (_value.charCodeAt(i) == 32) 
		      ))
		{
			return false;
			break;
		}
	}
	return true;
}


function isEmpty(pInputVal) 
{
	return (pInputVal == null || pInputVal == '');
}




function isBlank(_arg)
{
	if(_arg == null || _arg == "undefined" || _arg.length == 0)
	{
		return true;
	}
	else
	{
		var cnt = 0;
		var _str = "";
		for(var i=0;i<_arg.length;i++)
		{
			if( ! (_arg.charCodeAt(i) == 13 || _arg.charCodeAt(i) == 32))		
			{
				_str += _arg.charAt(i);
			}

		}
		return (_str.length ==  0 || _str == "") ? (true) : (false);

	}
}

var _NewWindow = null;
function showNewWin(_location,_width,_height) 
{
	
	var _num = Math.round(Math.random() * 100000);
	var _name = "window" + _num;
	if (! (_width) )
	      _width  = 320;
	if (! (_height) )
	      _height = 320;
	      
	if (_NewWindow && !_NewWindow.closed) 
	{
		_NewWindow.location.href=_location;
		_NewWindow.focus();
	}
	else 
	{
		if (screen)
		{
			leftpos = parseInt((screen.width - _width) / 2);
			toppos = parseInt((screen.height - _height) / 2);
		}
		else
		{
			leftpos = 200;
			toppos = 100;
		}
		_NewWindow = window.open(_location,_name,"left="+leftpos+",top="+toppos+",height="+_height+",width="+_width+",menubar=no,resizable=yes,scrollbars=yes,status=yes,toolbar=no");
		
	}
}

