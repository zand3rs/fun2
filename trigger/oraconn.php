<?php
//ora-conn
echo "BEGIN Test.<br>\n";
if ($conn=OCILogon("triger_web", "trgweb123", "trgprd")) {
echo "Successfully connected to Oracle using OCI extension.\n";
OCILogoff($conn);
} else {
$err = OCIError();
echo "Error in connecting to the Oracle." . $err[text];
}
?>

