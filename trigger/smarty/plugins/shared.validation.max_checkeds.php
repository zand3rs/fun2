<?PHP /* $Id: shared.validation.max_checkeds.php,v 1.3 2002/09/14 04:54:40 darkelder Exp $ */

/*
 * Form Plugin - Validation - max_checkeds
 * -------------------------------------------------------------
 * Author:          Roberto Bertó <darkelder@users.sourceforge.net>
 * License:         LGPL
 * Documentation:   http://ourcms.sourceforge.net/form/
 * Latest Version:  http://ourcms.sourceforge.net/form/
 * Current Version: 1
 * Purpose:         Check if a CheckBox/Radio haven't more than $length checked fields
 * Usage:           You should use {input name="foo[]"}. You cant use name="foo"!
 * Javascript:	    Got from http://www.codeave.com/javascript/code.asp?u_log=7062
 * Current Version: 1.0
 * Modified:        Apr, 07 2002
 * -------------------------------------------------------------
 */

function smarty_validation_max_checkeds($field,$length,$type)
{
    if ($type == SMARTY_FORM_SERVER)
    {
        if (sizeof($_REQUEST["$field"]) > $length) {
            return FALSE;
        }
        return TRUE;
    }
    elseif ($type == SMARTY_FORM_JSFUNCTION)
    {
        return 'function smarty_form_max_checkeds(field,option,message,fieldname) {
                var x = 0;

                // Loop from zero to the one minus the number of checkbox button selections
                for (i = 0; i < field.length; i++)
                {

                        //If a checkbox has been selected it will return true
                        //(If not it will return false)
                        if (field[i].checked)
                        {
                               x = x + 1;
                        }
                }
                if (x > option)
                {
                        if (message == "" || message == null)
                        {
                                var message = fieldname + " have more than " + option + " checkeds options";
                        }
                        alert(message);
                        return false;
                }
            return true;


        }';
    }

}

?>
