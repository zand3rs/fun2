<?PHP /* $Id: function.fsession.php,v 1.1.1.1 2002/09/13 17:52:11 darkelder Exp $ */

/*
 * Form Plugin - function input
 * -------------------------------------------------------------
 * Author:          Roberto Bertó <darkelder@users.sourceforge.net>
 * License:         LGPL
 * Documentation:   http://ourcms.sourceforge.net/form/
 * Latest Version:  http://ourcms.sourceforge.net/form/
 * Purpose:         Run $smarty->form to transform to html
 * Current Version: 1.0
 * Modified:        Apr, 07 2002
 * -------------------------------------------------------------
 */

function smarty_function_fsession($parms,&$smarty)
{
    include_once SMARTY_DIR . 'plugins/shared.form.php';
    global $smarty_form;
    $step = 0;
    extract($parms);
    $value = $smarty_form->session_get($step,$field);
    if ($assign != NULL)
    {
    	$smarty->assign($assign,$value);
    } else {
    	print $value;
    }
}

?>