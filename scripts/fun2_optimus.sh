#!/bin/bash

FTP_HOST="10.234.20.220"
FTP_USER="funsftp"
FTP_PASS=""
REMOTE_DIR="/archive_initload3/infa_shared_ph1/SrcFiles/Landing/NON_BSS/FUN2"
LOCAL_DIR="/appl/optimus"
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
FILE_PATTERN="FUN2*"

/usr/bin/expect -f - <<EOFEXPECT
    set timeout 60
    spawn sftp $FTP_USER@$FTP_HOST
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


