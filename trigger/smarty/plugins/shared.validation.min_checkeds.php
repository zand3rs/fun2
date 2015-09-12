<?PHP /* $Id: shared.validation.min_checkeds.php,v 1.4 2002/09/14 05:03:30 darkelder Exp $ */

/*
 * Form Plugin - Validation - min_checkeds
 * -------------------------------------------------------------
 * Author:          Roberto Bertó <darkelder@users.sourceforge.net>
 * License:         LGPL
 * Documentation:   http://ourcms.sourceforge.net/form/
 * Latest Version:  http://ourcms.sourceforge.net/form/
 * Purpose:         Check if a CheckBox/Radio haven't less than $length checked fields
 * JavaScript:      Got from: http://www.codeave.com/javascript/code.asp?u_log=7062
 * Current Version: 1.0
 * Modified:        Apr, 07 2002
 * -------------------------------------------------------------
 */

function smarty_validation_min_checkeds($field,$length,$type)
{
    if ($type == SMARTY_FORM_SERVER)
    {
        if (sizeof($_REQUEST["$field"]) < $length) {
            return FALSE;
        }
        return TRUE;
    }
    elseif ($type == SMARTY_FORM_JSFUNCTION)
    {
        return 'function smarty_form_min_checkeds(field,option,message,fieldname) {
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
                
                if (x < option)
                {
                        if (message == "" || message == null)
                        {
                                var message = fieldname + " value have less than " + option + " checkeds options";
                        }
                        alert(message);
                        return false;
                }
            return true;
            

        }';
    }
}
# vim: set expandtab:
?>
