<?php
//ora-conn
putenv('ORACLE_HOME=C:\orant');
putenv('TNS_ADMIN=C:\orant\net80\ADMIN');

echo "BEGIN Test.<br>\n";
if ($conn=OCILogon("gold_owner", "goldowner123", "gvm")) {
echo "Successfully connected to Oracle using OCI extension.\n";
OCILogoff($conn);
} else {
$err = OCIError();
echo "Error in connecting to the Oracle." . $err[text];
}
?>
