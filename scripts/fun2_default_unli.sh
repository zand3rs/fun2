#!/bin/bash

FTP_HOST="localhost"
FTP_USER="zander"
FTP_PASS="zander"

REMOTE_DIR="ftp"
LOCAL_DIR="local"

# do not show getopts errors...
OPTERR=0

while getopts h:u:p:l:r: FLAG; do
    case $FLAG in
        h) FTP_HOST=$OPTARG ;;
        u) FTP_USER=$OPTARG ;;
        p) FTP_PASS=$OPTARG ;;
        r) REMOTE_DIR=$OPTARG ;;
        l) LOCAL_DIR=$OPTARG ;;
        ?) echo "$0 [OPTIONS]"
           echo "OPTIONS:"
           echo "  -h <ftp host>"
           echo "  -u <ftp user>"
           echo "  -p <ftp password>"
           echo "  -r <remote directory>"
           echo "  -l <local directory>"
           exit 1
    esac
done

BASE_DIR="${0%/*}"
PREVIOUS_MINUTE=$(date '+%M' -d '2 minutes ago')
CURRENT_MINUTE=$(date '+%M')

DATE_PATTERN="$(date '+%Y_%m_%d_%H_')\[${PREVIOUS_MINUTE:0:1}${CURRENT_MINUTE:0:1}\]"
FILE_PATTERN="$DATE_PATTERN*Gi*"

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
    send "mget $FILE_PATTERN\r";
    expect "sftp>"
    send "bye\r";
    expect eof
EOFEXPECT

exit 0
