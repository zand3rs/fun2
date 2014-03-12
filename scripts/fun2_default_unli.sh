#!/bin/bash

FTP_HOST="localhost"
FTP_USER="zander"
FTP_PASS="zander"

REMOTE_DIR="ftp"
LOCAL_DIR="local"

BASE_DIR="${0%/*}"
CURRENT_FILE="$(date '+%Y_%m_%d_%H')*Gi*"

/usr/bin/expect -f - <<EOFEXPECT
    set timeout 60
    spawn sftp $FTP_USER@$FTP_HOST
    expect "password:"
    send "$FTP_PASS\r";
    expect "sftp>"
    send "lcd $BASE_DIR\r";
    expect "sftp>"
    send "lcd $LOCAL_DIR\r";
    expect "sftp>"
    send "cd $REMOTE_DIR\r";
    expect "sftp>"
    send "mget $CURRENT_FILE\r";
    expect "sftp>"
    send "bye\r";
    expect eof
EOFEXPECT
