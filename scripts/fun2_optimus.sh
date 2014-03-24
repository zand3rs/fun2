#!/bin/bash

FTP_HOST="10.226.224.108"
FTP_USER="oracle"
FTP_PASS="oracle123"

REMOTE_DIR="/optimus/insurance_feed"
LOCAL_DIR="/home/isoladm/app/local"

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
PROCESSED_DIR="processed"
COMPLETED_DIR="completed"
FILE_PATTERN="FUN2.EXT*"

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
    send "!\\mv -f $FILE_PATTERN $PROCESSED_DIR\r";
    expect "sftp>"
    send "mput $PROCESSED_DIR/$FILE_PATTERN\r";
    expect "sftp>"
    send "!\\mv -f $PROCESSED_DIR/$FILE_PATTERN $COMPLETED_DIR\r";
    expect "sftp>"
    send "bye\r";
    expect eof
EOFEXPECT

exit 0
