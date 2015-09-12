<?PHP /* $Id: shared.validation.min_length.php,v 1.2 2002/09/14 04:44:48 darkelder Exp $ */

/*
 * Form Plugin - Validation - min_option
 * -------------------------------------------------------------
 * Author:          Roberto Bertó <darkelder@users.sourceforge.net>
 * License:         LGPL
 * Documentation:   http://ourcms.sourceforge.net/form/
 * Latest Version:  http://ourcms.sourceforge.net/form/
 * Purpose:         Check if the Field isnt lower then $option chars
 * Current Version: 1.0
 * Modified:        Apr, 07 2002
 * -------------------------------------------------------------
 */

function smarty_validation_min_length($field,$option,$type,$message = NULL)
{
    if ($type == SMARTY_FORM_SERVER)
    {
        if (strlen(trim($_REQUEST["$field"])) < $option) {
            return FALSE;
        }
        return TRUE;
    }
    elseif ($type == SMARTY_FORM_JSFUNCTION)
    {
        return 'function smarty_form_min_length(field,option,message,fieldname) {
            if (field.value.length < option)
            {
				if (message == "" || message == null)
				{
					var message = fieldname + " value have less than " + option + " chars";
				}
                alert(message);
                return false;
            }
            return true;
        }';
    }
}

?>
