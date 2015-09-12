<?PHP /* $Id: shared.form.php,v 1.3 2002/09/14 05:11:16 darkelder Exp $ */


// This class is autoloaded when this file is opened 
global $smarty_form;
$smarty_form = new smarty_form();

/**
  * Validation type is Server-side (do not change it)
  */
define("SMARTY_FORM_SERVER",0);
/**
  * Validation is client-side (do not change)
  */
define("SMARTY_FORM_JSFUNCTION",1);

/**
  * Smarty Form Plugin - main Class
  *
  * A LGPL Smarty Plugin to process forms with server-side and client-side validations
  * and session variables save to {@link http://smarty.php.net Smarty template engine} 
  *
  * Currently key-features are:
  * - customized error messages;
  * - easy-to-expand validations rules;
  * - smarty function to load session-saved variables into form steps
  * - in one template you put all form steps (eg: step 1 - register your email;  step 2 - choice your password; step 3 - confirm you submission; step 4 - results)
  * - all form fields attributes are saved. So you can add class, onclick, onfocus, and whatever attributes inside. 
  * - auto complete: smarty-form-plugin autocomplete form fields with user sent values
  * - smarty-form-plugin use the same Smarty tech to open files only when they're need. So, the shared.form.php will be loaded only there are {formprocess} tags inside template.
  *
  * @access             public
  * @author             Roberto Bertó - darkelder (inside) users (dot) sourceforge (dot) net
  * @version            1.0rc1
  */
class smarty_form
{
// the below variables can be changed at your like
        /**
          * smarty-form-plugin version
          *
          * @access             public
          * @since              smarty-form-plugin 1.0rc1
          * @var                string
          */
        var $version                                     = "0.9.1";
        /**
          * enable client-side validation. The default is TRUE to save server processor
          * 
          * @access             public
          * @since              smarty-form-plugin 1.0rc1
          * @var                bool            TRUE enable client-side validation
          */
        var $javascript					= TRUE;
        /**
          * keep TRUE will enable Session fields saving. The default is FALSE for security reasons, and can be changed here.
          *
          * @access             public
          * @since              smarty-form-plugin 1.0rc1
          * @var                bool            TRUE enable Session
          */
	var $keep                      			= FALSE;

// the below variables is used internally by smarty-form-plugin
        /**
          * step start from 0
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                int             Current Form Step
          */
        var $step                                        = 0;
        /** 
          * form name attribute default is simply form
          * 
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                string          <form NAME="">
          */
        var $name                                       = 'form';
        /**
          * session name, used by php
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                string
        */
	var $session			                = '';
        /** 
          * list current form fields types. 
          *
          * Each element should be an array(field name,need close),
          * where field name is the field name <fieldname> and if need
          * close is true, so {/fildname} will be replace to </fieldname>
          *
          * Should exists one smarty function to each form field.
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                array
          */
        var $fields_map = array(
                array("input",          FALSE),
                array("select",         TRUE),
                array("textarea",       TRUE),
                array("form",           TRUE)
            );
        /**
          * internal table of validation
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                array
          */
        var $validation_table           = array();
        /**
          * internal table of validation fields
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                array
          */
        var $validation_field           = array();
        /**
          * internal table of validation messages
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                array
          */
        var $validation_messages        = array();
        /**
          * internal table of validation clent fields
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                array
          */
        var $validation_client          = array();
        /**
          * reference variable to smarty
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                object
          */
        var $smarty;
        /**
          * internal table of javascript callers
          *
          * have fields rules to call javascript validation functions
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                array
          */
        var $javascript_callers         = array();
        /**
          * internal table of fields types
          *
          * tell what the type of each field (input)
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                array
        */
        var $fields_types         = array();
        /**
          * javascript validation functions
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                array
          */
        var $javascript_functions       = array();
        /**
          * internal variable to handle steps 
          *
          * @access             private
          * @since              smarty-form-plugin 1.0rc1
          * @var                int
          */
        var $step_compiling             = 0;

        /**
          * constructor
          *
          * Get actual smarty_form_step and the smarty_form_session
          *
          * @access             public
          * @since              smarty-form-plugin 1.0rc1
          */
        function smarty_form()
        {
                // getting the real form step
                if ($_REQUEST["_smarty_form_step"] !== NULL)
                {
                    $this->step = $_REQUEST["_smarty_form_step"];
                    $this->nofirst = TRUE;
                }
                if ($_REQUEST["_smarty_form_session"] !== NULL)
                {
                    $this->session = $_REQUEST["_smarty_form_session"];
                }
            }
        /**
          * Called by the prefilter_form
          *
          * 1) Get parameters from {formprocessor} and setup-it
          * 2) Start Session if keep=1
          * 3) Put right name on $this->name
          * 4) Turn JavaScript handle on/off
	  * 5) Find the Steps (form MUST have steps) and call compiler_steps
	  *
          * @access             public
          * @since              smarty-form-plugin 1.0rc1
          * @return             string                  Compiled Steps
          */
        function compiler_formprocessor($changing)
        {
                /* $changing = array(
                *       0 => all {formprocessor} block
                *       1 => formprocessor attributes
                *       2 => formprocessor content
                ) */

                // get name of formprocessor, if exists
                if (preg_match('|name="(.*?)"|is',$changing[1],$matches))
                {
                    $this->name = $matches[1];
                }

                // get keep of formprocessor, if exists
                if (preg_match('|keep="(.*?)"|is',$changing[1],$matches) || preg_match('/keep=(TRUE|FALSE|1|0)/is',$changing[1],$matches))
                {
                        if (strtolower($matches[1]) == "true" || $matches[1] == 1)
                        {       
                                $this->keep = TRUE;
                        }
                        else
			{
				$this->keep = FALSE;
			}
			
                        if ($this->keep == TRUE)
                        {
                    	        session_name("smarty_form");
                    	        session_start();
                    	        if ($this->session == NULL) {
                    		        session_unset();
        	        	        $this->session = session_id();
        	       	        }
                                else
                                {
        	       		        session_id($this->session);
        	       	        }
                        }
                }

                // get javascript of form, if exists
        	if (preg_match('|javascript="(.*?)"|is',$changing[1],$matches) || preg_match('/javascript=(TRUE|FALSE|1|0)/is',$changing[1],$matches))
	        {
                        if (strtolower($matches[1]) == "true" || $matches[1] == 1)
                        {
                        	$this->javascript = TRUE;
                }
			else 
			{
				$this->javascript = FALSE;
			}
		}

	        // find steps
        	if (preg_match_all("/{step}(.*?){\/step}/is",$changing[2],$matches))
        	{
          		return $this->compiler_step($matches[1]);
        	} else {
            		// error, dont find steps
           		$this->smarty->trigger_error("form plugin: you must set {step} tags inside {formprocessor}");
        	}
	}
        
        /**
          * Compile the Right STEP Content
          *
          * @access             public
          * @since              smarty-form-plugin 1.0rc1
          * @return             string                  Compiled Step Content
          */
        function compiler_step($steps)
        {
                /*  $steps = array(
                *       0 => fist one
                *       1 => second
                *       2 => third
                *       ...
                )*/
        
                $step_current   = $this->step;
                $step_lastone   = $this->step - 1;

                /* ROUTINE
                   get validations of step_current and process it,
                    - TRUE
                        if there are $this->stepNext
                                set _smarty_form_step to $this->step+1 and add an input hidden below {form}
                                assign $formsucess to TRUE
                    - if fail
                        if there are $this->stepNext
                                set _smarty_form_step to $this->step and add an input hidden below {form}
                                assign $formsucess to FALSE
                                assign $formerrors
                */

                // PROCESS CURRENT STEP
                // get validations parms, setting javascript to fields, set _smarty_form_step and _smarty_form_name
                
                // STEP = 0
                if ($this->nofirst != TRUE)
                {
                        $this->step_compiling = 0;
                        $this->smarty->assign("smarty_form_sucess",NULL);


                        $form = $this->compiler_form($steps[0]);
                        $this->compiler_validation(TRUE);
                        return $form;
                }
                else
                {
                        $this->step_compiling = $this->step;
                        $form = $this->compiler_form($steps[$this->step]);

                        $this->validation_errors = $this->compiler_validation();
                        if (sizeof($this->validation_errors) > 0)
                        {
                                $this->smarty->assign("smarty_form_errors",$this->validation_errors);
                                $this->smarty->assign("smarty_form_sucess",FALSE);
                                // LAST - because we have error
                                return $form;
                        }

                        // CURRENT - because no error
                	#$this->compiler_stepCleaner();
                        $this->step_compiling++;
                        $this->smarty->assign("smarty_form_sucess",TRUE);
                        $GLOBALS["_smarty_form_sucess"] = 1;
                        $form = $this->compiler_form($steps[$this->step_compiling]);
                        $this->session_write();
                        return $form;
                }
        }

        /* why use it?
	function compiler_stepCleaner()
	{
		$this->validation_table = array();
		$this->validation_field = array();
		$this->validation_messages = array();
		$this->validation_client = array();
		$this->javascript_callers = array();
		$this->javascript_functions = array();
		$this->validation_errors = array();
	}
        */

        /** 
          * Compile the Form
          *
          * Get validation schema and put into the validation table.
          * Also, change the {/select}, {/form}, etc.. to </select>, </form>
          *
          * @since              smarty-form-plugin 1.0rc1
          * @access             public
          * @return             string                  Close Tags without {}
          */
        function compiler_form($changing)
        {
                $this->compiler_getValidationTable($changing);
                return $this->compiler_stripOutTwoWayTags($changing);
        }

        /**
          * Get Validation Table
          *
          * Get Validation Table From Form Fields 
          *
          * @access             public
          * @since              smarty-form-plugin 1.0rc1
          */
        function compiler_getValidationTable($changing)
        {
                foreach ($this->fields_map as $field)
                {
                        $field_name = $field[0];
                        if (preg_match_all("/\{({$field_name})[^}]*?(name|validation|messages)=\"([^}]+?)\"[^}]*?(name|validation|messages)=\"([^}]+?)\"[^}]*?((name|validation|messages)=\"([^}]+?)\"[^}]*?){0,1}}/is",$changing,$matches,PREG_SET_ORDER))
                        {
                                for ($i = 0; $i < count($matches); $i++)
                                {
                                        // dont allow loop var bug
                                        unset($messages);
                                        unset($name);
                                        unset($validation);

                                        // discover the input type
                                        if ($field_name == "input")
                                        {
                                                if (preg_match("/type=\"(text|password|checkbox|radio|submit|reset|file|hidden|image|button)\"/is",$matches[$i][0],$tmatch))
                                                {
                                                        $type = $tmatch[1];
                                                }
                                        }
                                        
                                        $matches[$i][2] = strtolower($matches[$i][2]);
                                        $matches[$i][4] = strtolower($matches[$i][4]);
                                        $matches[$i][7] = strtolower($matches[$i][7]);
                
                                        $$matches[$i][2] = $matches[$i][3];
                                        $$matches[$i][4] = $matches[$i][5];
                                        $$matches[$i][7] = $matches[$i][8];

                                        $this->fields_types[$name] = $type;

                                        // array form format to string
                                        $name = preg_replace("/\[\]/","",$name);

                                        foreach (explode("&",$validation,2) as $arg)
                                        {
                                                list($key,$val) = explode("=",$arg,2);
                                                $val = urldecode($val);
                                                $this->validation_table["$name"]["$key"] = $val;
                                                
                                                // not used now
                                                #$this->validation_field["$name"] = $matches[$i][1];
                                        }
                        
                                        foreach (explode("&",$messages,2) as $arg)
                                        {
                    	                        unset($key);
                                            	unset($val);
                                                list($key,$val) = explode("=",$arg,2);
                                            	if ($key != NULL)
                                                {
                                                	$val = urldecode($val);
                                                       	$this->validation_messages["$name"]["$key"] = $val;
                                             	}
                                        }
                                }
                        }
                }
        }
        
        /**
          * Change {/..} to </..> according fields_map
          *
          * @access             public
          * @since              smarty-form-plugin 1.0rc1
          * @return             string          striped
          */
        function compiler_stripOutTwoWayTags($changing)
        {

                foreach ($this->fields_map as $tag)
                {
                        if ($tag[1] == TRUE)
                        {
                                $changing = preg_replace("/\{\/{$tag[0]}\}/is","</{$tag[0]}>",$changing);
                        }
                }
                return $changing;
        }


        function compiler_validation($onlyjavascript = FALSE)
        {
                $errors = array();
                foreach ($this->validation_table as $field => $args)
                {
                        foreach ($args as $key => $val)
                        {
                                // security: user cant load a file that is out of plugins dir
                                $file = realpath(SMARTY_DIR . $this->smarty->plugins_dir[0] . '/shared.validation.' . $key . '.php');
                                if (strpos($key,".") !== FALSE)
                                {
                                        $this->smarty->trigger_error("you cannot use '.' in validations rules because of javascript rules");
                                }
                                elseif (!file_exists($file))
                                {
                                        $this->smarty->trigger_error("validation rule $key doesnt exists");
                                }
                                else {
                                        include_once $file;
                                        $jsfunction = NULL;
                                        eval('$jsfunction   = smarty_validation_' . $key . '($field,$val,SMARTY_FORM_JSFUNCTION);');
                                        if ($jsfunction != NULL)
                                        {
                                                $this->validation_client[] = array($field,$key,$val,$jsfunction);
                    }

                    if ($onlyjavascript == FALSE)
                    {
				eval('$serverError   = smarty_validation_' . $key . '($field,$val,SMARTY_FORM_SERVER);');
				if ($serverError == FALSE)
				{
			        	$errors[] = array("field" => $field,"key" => $key, "value" => $val,"message" => $this->validation_messages["$field"]["$key"]);
                        	}
			}
                }
            }
        }
        return $errors;
    }



// FIELD FUNCTIONS - functions called by fields smarty functions
    function field_create($field,$parms)
    {
        // not needed
        unset($parms['validation']);
        unset($parms['messages']);

        // assign or not?
        $assign = $parms['assign'];
        unset($parms['assign']);

        $fieldTag = array();
        $tag = '';

        if ($field == "form")
        {
            $js = $this->field_validation($parms['name']);
            if ($js != NULL)
            {
            	$tag .= $js;
            	if ($parms["onsubmit"] != NULL)
            	{
            		$parms["onsubmit"] = $parms["onsubmit"] . '; ';
				}
				$parms["onsubmit"] = $parms["onsubmit"] . 'return caller_smarty_form(this);';
            }
        }

        $fieldTag[] = "<$field";

		// checkboxes and radios need a checked value
        if ($field == "input" && ($parms["type"] == "radio" || $parms["type"] == "checkbox"))
        {
			$_name = $this->_noArrayName($parms["name"]);
			settype($_REQUEST["$_name"],"array");
			if (in_array($parms["value"],$_REQUEST["$_name"]))
			{
				$fieldTag[] = "checked";
			}
			elseif ($parms["checked"] == TRUE && sizeof($this->validation_errors) < 1)
			{
				$fieldTag[] = "checked";
			}
			unset($parms["checked"]);
        }


        foreach ($parms as $key => $val)
        {
            $fieldTag[] = "$key=\"$val\"";
        }

        $tag .= implode(' ',$fieldTag);

        if ($field == "input") 
        {
                $tag .= " /";
        }

        $tag .= ">";

        if ($field == "form")
        {
            // adding hidden
                $tag .= $this->field_create("input",array("type"=>"hidden","name"=>"_smarty_form_step","value"=>$this->step_compiling,"assign"=>TRUE));
                $tag .= $this->field_create("input",array("type"=>"hidden","name"=>"_smarty_form_name","value"=>$this->name,"assign"=>TRUE));
	        $tag .= $this->field_create("input",array("type"=>"hidden","name"=>"_smarty_form_session","value"=>$this->session,"assign"=>TRUE));
        }

        // output
        if ($assign != NULL)
        {
            return $tag;
        }
        print $tag;
    }

    function field_validation($formname)
    {
    	if ($this->javascript != TRUE)
    	{
    		return '';
    	}
    	$jfunc = array();
    	$jcall = array();


        foreach ($this->validation_client as $args)
        {
            list($field,$key,$value,$js) = $args;
            $jfunc[$key] = $js;

            // radio and checkbox cant get focus()
            if ($this->fields_types[$field] != "radio" && $this->fields_types[$field] != "checkbox")
            {
                $focus = sprintf('form.%s.focus(); ', $field);
            }
            else {
                $focus = " ";
            }
            $jcall[] .= sprintf("\tif (smarty_form_%s(form.%s,'%s','%s','%s') == false) { %s return false; }\n",$key,$field,addslashes($value),addslashes(preg_replace("/[\r\n]/","",$this->validation_messages["$field"]["$key"])),$field,$focus);
        }
        if (sizeof($jfunc) > 0)
        {
        	$callers = "function caller_smarty_form(form) {\n";
        	foreach ($jcall as $caller)
        	{
        		$callers .= $caller; 
        	}
        	$callers .= "\treturn true;\n}\n";
        	$jfunc[] = $callers;
            $js = "<script language=\"JavaScript\"><!--\n" . join("\n",$jfunc) . "\n//--></script>";

        }
        return $js;
    }



	function session_write()
	{
                if ($this->keep == TRUE)
                {
        		$GLOBALS["_smarty_form_session"] = $_SESSION["_smarty_form_session"];
	        	$GLOBALS["_smarty_form_session"]["{$this->step}"] = $_REQUEST;
		        unset($GLOBALS["_smarty_form_session"]["{$this->step}"]["_smarty_form_step"]);
        		unset($GLOBALS["_smarty_form_session"]["{$this->step}"]["_smarty_form_session"]);
	        	unset($GLOBALS["_smarty_form_session"]["{$this->step}"]["_smarty_form_name"]);
		        unset($GLOBALS["_smarty_form_session"]["{$this->step}"]["PHPSESSID"]);
        		unset($GLOBALS["_smarty_form_session"]["{$this->step}"]["smarty_form"]);
                        session_register("_smarty_form_session");
                }
	}

	function session_get($step = 0, $data)
	{
		return $_SESSION["_smarty_form_session"][$step][$data];
	}



	function _noArrayName($s)
	{
		return preg_replace("/\[.*?\]/","",$s);
	}
}

/* vim: set expandtab: */
?>
