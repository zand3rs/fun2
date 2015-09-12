<?php
//require_once('Mail.php'); //--- @/usr/share/pear directory
//require_once('PEAR/Mail/mime.php'); //--- use the local copy, wala kasi sa standard installation ng PHP-pear
 /*
 #----------------------------------------------------------------------- 	
 #	@Filename	    :	myUtils.php
 #	@Description	:	web utils
 #	@Date		      :	2009-04-30
 #	@Ver		      :	ver1.1
 #	@Author		    :	yiucon
 #-----------------------------------------------------------------------
*/


function format_string($line)
{
	$line = str_replace('"','&quot;',$line);
	$line = str_replace('\'','&#39;',$line);
	return $line;
}

function get_milli_secs() 
{
	list($usec,$sec) = explode(" ", microtime());
	return sprintf("%03d",((double)$usec * 1000));
}

function check_email($email) 
{
	list($local, $domain) = explode("@", $email);
	$pattern_local = '^([0-9a-z]*([-|_]?[0-9a-z]+)*)(([-|_]?)\.([-|_]?)[0-9a-z]*([-|_]?[0-9a-z]+)+)*([-|_]?)$';
	$pattern_domain = '^([0-9a-z]+([-]?[0-9a-z]+)*)(([-]?)\.([-]?)[0-9a-z]*([-]?[0-9a-z]+)+)*\.[a-z]{2,4}$';
	$match_local = eregi($pattern_local, $local);
	$match_domain = eregi($pattern_domain, $domain);

	if ($match_local && $match_domain) {
		return 1;
	} else {
		return 0;
	}
} 

function redir_page($url='index.php', $msg='')
{
	$ok = (strlen(trim($msg))>0) ? (1) : (0);
	echo "<script language='Javascript'> var ok=\"$ok\"; if(ok==1){alert(\"$msg\");}\nwindow.location='$url';\n</script>";
}

function get_uid($id='')
{
	global $_SERVER;
	$time    = time();
	$ip      = $_SERVER["REMOTE_ADDR"];
	$agent   = $_SERVER["HTTP_USER_AGENT"];
	$md5     = md5($time.$ip.$agent.$id);
	return $md5;
}
 

function debug($msg='')
{
	global $gLoggerConf,$gDebug,$gLogDebug;

	$tm   = date("Y-m-d H:i:s");
	$buff = sprintf("DEBUG: [%15s] : %s : %s", "---MAIN---" , $tm, $msg); 

	if($gDebug)
	{
		echo "$buff<hr>\n";	

	}
	if($gLogDebug)
	{
		$logger = &Log::singleton('file', WEBLOG, "---MAIN---", $gLoggerConf);
		$logger->log($msg);
	}
}

 

function file_save($filename='',$contents='',$mode='A')
{
	    //open
	    $mode   = (!strcasecmp($mode,'a') or !strcasecmp($mode,'append')) ? ('a') : ('w');
      $handle = @fopen($filename,$mode);	
      //write
      @fwrite($handle,"$contents\n");
      //close
      @fclose($handle);
      
      debug("file_save() :: $filename ");
}

function is_num($val='')
{
	
	$val=str_replace(" ","",trim($val));
	//return eregi("^([0-9])+([\.|,]([0-9])*)?$",$val);	
	return eregi("^([0-9]*\.[0-9]+|[0-9]+)$",$val);	
	
}



function encrypt($str, $key) 
{
	
	  //fmt
    $buffer = '';
    
    for($i=0; $i < strlen($str); $i++) 
    {
      $bfr    = substr($str, $i, 1);
      $kbfr   = substr($key, ($i % strlen($key))-1, 1);
      $bfr    = chr(ord($bfr)+ord($kbfr));
      
      $buffer.=$bfr;
      
    }
    
    $buffer = base64_encode($buffer);
    
    debug("encrypt() : [$str/$key/$buffer]");
    
    return $buffer;
}

function decrypt($str, $key)
{
	  //fmt
    $buffer = '';
    $str    = base64_decode($str);
    for($i=0; $i < strlen($str); $i++) 
    {
    	
    	
      $bfr    = substr($str, $i, 1);
      $kbfr   = substr($key, ($i % strlen($key))-1, 1);
      $bfr    = chr(ord($bfr)-ord($kbfr));
      $buffer.=$bfr;
      
    }
    
    debug("decrypt() : [$str/$key/$buffer]");
    
    return $buffer;
}
 

function save_uploaded_pic($fileparam='',$uploaddir='/tmp/',$newsavefile='',$thumb_size=96)
{
	//be sure
  global $_FILES;
  
  //fmt
  $invalid_chars     = array("'", '"', "[", "]", "/", "\\", "{", "}", "\?", "(",")");
  $newsavefile       = str_replace($invalid_chars, '_', $newsavefile);
  
  
	$upload_buff_name  = $_FILES[$fileparam]['name'];
	$upload_buff_size  = $_FILES[$fileparam]['size'];
	$upload_buff_type  = $_FILES[$fileparam]['type'];
	$upload_buff_tmp   = $_FILES[$fileparam]['tmp_name'];
	$pos               = strpos($upload_buff_type, 'image');
	
	$upload_buff_name  = str_replace($invalid_chars, '_', $upload_buff_name);
	
	$upload_buff_file  = $uploaddir .$upload_buff_name;
	
  $newsavefile       =  strtolower($newsavefile);
  
  
  
  
	$retval                = null;
	$retval['saved']       = false; 


	if($pos === false)
	{
		$retval['err_str'] = "Invalid picture upload (not an image).";
		return $retval;	
	}
	if( $upload_buff_size > UPLOAD_FILE_DEFAULT_MAXSIZE )
	{
		$retval['err_str'] = "Each photo that you upload must be less than 1MB in size.";
		return $retval;		
	}
	if($upload_buff_size <= 0)
	{
		$retval['err_str'] = "Invalid picture upload (zero file size).";
		return $retval;	
	}  
	if(!@preg_match("/(.)+(\.)(gif|jpg|jpeg)$/i", basename($upload_buff_name )))
	{
		$retval['err_str'] = "Invalid picture upload (not an image)!";
		return $retval;	
	}
	
	if (move_uploaded_file($upload_buff_tmp,$uploaddir.$newsavefile )) 
	{
		
		$dstw                  = 96;
		$dsth                  = 96;
		
		$retval['savedfile']   = $uploaddir.$newsavefile ; 
		
		$dstpix                = $uploaddir."resized.$newsavefile";
	  $srcpix                = $uploaddir.$newsavefile;
	  
    $mysize                = $dstw . 'x' . $dsth;
    
    $respix                = "resized.$newsavefile";
    $retval['resized']     = $respix;
    
    //new-style square-thumb-nail
    set_square_thumbnail($srcpix,$dstpix);
    
    //---------------------
    // @SYSTEM
    //
    //$cmd      = "/usr/local/bin/convert \"$srcpix\" -thumbnail $mysize \"$dstpix\"" ;
    //$imgret   = @system($cmd); 
    //
    //---------------------
    
    
    //pls have it world-readable    
    @system("/bin/chmod 666 " . $uploaddir. $respix );
    @system("/bin/chmod 666 " . $uploaddir. $newsavefile );
    
		debug("resized pix= $respix :: $imgret=$cmd");
		
    $retval['saved']       = true;
    
	}
	else
	{
		$retval['saved']   = false; 
		$retval['err_str'] = "Upload picture failed! ";
		
		
		
	}
	
	debug("save_uploaded_pic($respix) : upload file failed(($upload_buff_file ---> $uploaddir$newsavefile))");
	
	
	return $retval;
}


 function set_square_thumbnail($source,$dest)
 {
     //constants
     $DEF_W  = 96;
     $DEF_H  = 96;
     
     //get size
     $size   = getimagesize($source);
     $width  = $size[0];
     $height = $size[1];

     //chk width & height
     if($width > $height) 
     {
     	 $thumb_size_w = $DEF_W;
     	 $thumb_size_h = ceil( ($thumb_size_w / $width)  * $height );
     }
     else if($height > $width) 
     {
     	 $thumb_size_h = $DEF_H;
     	 $thumb_size_w = ceil( ($thumb_size_h / $height)  * $width );
     }
     else
     {
     	 $thumb_size_w = $DEF_W;
     	 $thumb_size_h = $DEF_H;
     }

     
      
     if($width> $height) {

         $x = ceil(($width - $height) / 2 );

         $width = $height;

     } elseif($height> $width) {

         $y = ceil(($height - $width) / 2);

         $height = $width;

     }
      
     
      //ext
     	$allowedExtensions = 'jpg jpeg gif png';
			$name              = explode(".", $source);
			$currentExtensions = $name[count($name)-1];
			$extensions        = explode(" ", $allowedExtensions);
			
			for($i=0; count($extensions)>$i; $i=$i+1)
			{
					if(!strcasecmp($extensions[$i],$currentExtensions))
					{ 
					        $extensionOK=1;
					        $fileExtension=$extensions[$i];
					        break; 
					}
			}
			
     
  

     $new_im = ImageCreatetruecolor($thumb_size_w,$thumb_size_h);




     //$im = imagecreatefromjpeg($source);

     if( !strcasecmp($fileExtension,"jpg")  OR !strcasecmp($fileExtension,'jpeg')  )
			{
				$im = ImageCreateFromJpeg($source);
			}
			elseif (!strcasecmp($fileExtension , "gif") )
			{
				$im = ImageCreateFromGIF($source);
			}
			elseif (!strcasecmp($fileExtension , "png") )
			{
				$im = imageCreateFromPNG($source);
			}
			elseif (!strcasecmp($fileExtension , "bmp") )
			{
				$im = imageCreateFromWBmp($source);
			}


     imagecopyresampled($new_im,$im,0,0,$x,$y,$thumb_size_w,$thumb_size_h,$width,$height);

       //imagejpeg($new_im,$dest,100);


    if( !strcasecmp($fileExtension,"jpg")  OR !strcasecmp($fileExtension,'jpeg')  )
			{
				imagejpeg($new_im,$dest,100);
			}
			elseif (!strcasecmp($fileExtension , "gif") )
			{
				imagegif($new_im, $dest);
			}
			elseif (!strcasecmp($fileExtension , "png") )
			{
				imagepng($new_im, $dest);
			}
			elseif (!strcasecmp($fileExtension , "bmp") )
			{
				imagewbmp($new_im, $dest);
			}  

 }



function base64_to_file( $data, $outputfile )
{
	
	      debug("base64_to_file($outputfile) : $data");
	      
        //encode & write data (binary)
        $ifp          = @fopen( $outputfile, "wb" );
        
        //chk
        if(! $ifp)
        {
        	
           $saveret['saved']   = false;
           $saveret['err_str'] = "Decoding raw data failed!";
           return $saveret;
           
        }
        
        $decoded_data = urlsafe_b64decode($data);
        
        @fwrite( $ifp, $decoded_data );
        @system("/bin/chmod 666 " . $outputfile );
        @fclose( $ifp );
        
        $saveret['saved'] = true;
        
        return $saveret;
        
}

function urlsafe_b64decode($string) 
{
	  //safe replace
    $data = str_replace(array('-','_'),array('+','/'),$string);
    $mod4 = strlen($data) % 4;
    if ($mod4) {
        $data .= substr('====', $mod4);
    }
    return base64_decode($data);
}

function ora_addslashes($str) 
{
	  //safe replace
    $data = str_replace("'","''",$str);
	return $data;
}


?>
