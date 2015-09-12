<?PHP /* $Id: shared.validation.max_length.php,v 1.2 2002/09/14 04:44:49 darkelder Exp $ */

/*
 * Form Plugin - Validation - max_length
 * -------------------------------------------------------------
 * Author:          Roberto Bertó <darkelder@users.sourceforge.net>
 * License:         LGPL
 * Documentation:   http://ourcms.sourceforge.net/form/
 * Latest Version:  http://ourcms.sourceforge.net/form/
 * Purpose:         Check if the Field isnt greater then $length chars
 * Current Version: 1.0
 * Modified:        Apr, 07 2002
 * -------------------------------------------------------------
 */

function smarty_validation_max_length($field,$option,$type,$message = NULL)
{
    if ($type == SMARTY_FORM_SERVER)
    {
        if (strlen(trim($_REQUEST["$field"])) > $option) {
            return FALSE;
        }
        return TRUE;
    }
    elseif ($type == SMARTY_FORM_JSFUNCTION)
    {
        return 'function smarty_form_max_length(field,option,message,fieldname) {
            if (!message)
            {
                message = fieldname + " value have more than " + option + " chars";
            }

            if (field.value.length > option)
            {
                alert(message);
                return false;
            }
            return true;
        }';
    }
}

?>
