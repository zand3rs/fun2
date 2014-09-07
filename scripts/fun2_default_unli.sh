#!/bin/bash

FTP_HOST="10.8.80.116"
FTP_USER="radcom_sftp"
FTP_PASS=""

REMOTE_DIR="/radcom/omniq/csv/Export_Raven"
ARCHIVE_DIR="/appl/fun2_default_unli/downloaded_files/archive"
LOCAL_DIR="/appl/fun2_default_unli/downloaded_files"

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
PREVIOUS_5_MINUTE=$(date '+%Y_%m_%d_%H_%M' -d '8 hours ago 7 minutes ago')
PREVIOUS_4_MINUTE=$(date '+%Y_%m_%d_%H_%M' -d '8 hours ago 6 minutes ago')
PREVIOUS_3_MINUTE=$(date '+%Y_%m_%d_%H_%M' -d '8 hours ago 5 minutes ago')
PREVIOUS_2_MINUTE=$(date '+%Y_%m_%d_%H_%M' -d '8 hours ago 4 minutes ago')
PREVIOUS_1_MINUTE=$(date '+%Y_%m_%d_%H_%M' -d '8 hours ago 3 minutes ago')
FILE_PATTERN_5="$PREVIOUS_5_MINUTE*Gi*.csv.gz"
FILE_PATTERN_4="$PREVIOUS_4_MINUTE*Gi*.csv.gz"
FILE_PATTERN_3="$PREVIOUS_3_MINUTE*Gi*.csv.gz"
FILE_PATTERN_2="$PREVIOUS_2_MINUTE*Gi*.csv.gz"
FILE_PATTERN_1="$PREVIOUS_1_MINUTE*Gi*.csv.gz"

echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: Start sftp download..."
echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: $FILE_PATTERN_5"
echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: $FILE_PATTERN_4"
echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: $FILE_PATTERN_3"
echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: $FILE_PATTERN_2"
echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: $FILE_PATTERN_1"

/usr/bin/expect -f - <<EOFEXPECT
    set timeout 60
    spawn sftp $FTP_USER@$FTP_HOST
    expect "sftp>"
    send "lcd $BASE_DIR\r";
    expect "sftp>"
    send "lcd $ARCHIVE_DIR\r";
    expect "sftp>"
    send "cd $REMOTE_DIR\r";
    expect "sftp>"
    send "mget $FILE_PATTERN_5\r";
    send "mget $FILE_PATTERN_4\r";
    send "mget $FILE_PATTERN_3\r";
    send "mget $FILE_PATTERN_2\r";
    send "mget $FILE_PATTERN_1\r";
    expect "sftp>"
    send "bye\r";
    expect eof
EOFEXPECT

echo "..."
echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: uncompress files..."

find $ARCHIVE_DIR -maxdepth 1 -name $FILE_PATTERN_5 -exec gzip -d {} \;
find $ARCHIVE_DIR -maxdepth 1 -name $FILE_PATTERN_4 -exec gzip -d {} \;
find $ARCHIVE_DIR -maxdepth 1 -name $FILE_PATTERN_3 -exec gzip -d {} \;
find $ARCHIVE_DIR -maxdepth 1 -name $FILE_PATTERN_2 -exec gzip -d {} \;
find $ARCHIVE_DIR -maxdepth 1 -name $FILE_PATTERN_1 -exec gzip -d {} \;

echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: moving csv files..."

mv $ARCHIVE_DIR/*.csv $LOCAL_DIR/.

#rm -f $ARCHIVE_DIR/$FILE_PATTERN_5
#rm -f $ARCHIVE_DIR/$FILE_PATTERN_4
rm -f $ARCHIVE_DIR/$FILE_PATTERN_3
rm -f $ARCHIVE_DIR/$FILE_PATTERN_2
rm -f $ARCHIVE_DIR/$FILE_PATTERN_1

echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: End sftp download..."

exit 0
