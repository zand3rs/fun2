#!/bin/bash

count=`ps -eaf | grep cipher | grep -v grep | grep -v tail | grep -v bin | wc -l`

echo `date +'%c'`

if [ ${count} -gt 2 ]
then
  echo  "fun2_cipher still running.->"
  echo ${count}
  exit
else
   FTP_HOST="10.237.181.204"
   FTP_USER="ereadm"
   FTP_PASS="ere123$$$$"
   
   REMOTE_DIR="/data/FUN2"
   LOCAL_DIR="/appl/fun2_default_unli/downloaded_files/completed"
   
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
   FILE_PATTERN_5="$PREVIOUS_5_MINUTE*Gi*.csv"
   FILE_PATTERN_4="$PREVIOUS_4_MINUTE*Gi*.csv"
   FILE_PATTERN_3="$PREVIOUS_3_MINUTE*Gi*.csv"
   FILE_PATTERN_2="$PREVIOUS_2_MINUTE*Gi*.csv"
   FILE_PATTERN_1="$PREVIOUS_1_MINUTE*Gi*.csv"
   
   echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: Start sftp upload..."
   echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: $FILE_PATTERN_5"
   echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: $FILE_PATTERN_4"
   echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: $FILE_PATTERN_3"
   echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: $FILE_PATTERN_2"
   echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: $FILE_PATTERN_1"
   
   /usr/bin/expect -f - <<EOFEXPECT
       set timeout 60
       spawn sftp $FTP_USER@$FTP_HOST
       expect "sftp>"
       send "lcd $LOCAL_DIR\r";
       expect "sftp>"
       send "cd $REMOTE_DIR\r";
       expect "sftp>"
       send "mput $FILE_PATTERN_5\r";
       expect "sftp>"
       send "mput $FILE_PATTERN_4\r";
       expect "sftp>"
       send "mput $FILE_PATTERN_3\r";
       expect "sftp>"
       send "mput $FILE_PATTERN_2\r";
       expect "sftp>"
       send "mput $FILE_PATTERN_1\r";
       expect "sftp>"
       send "bye\r";
       expect eof
   EOFEXPECT
fi

echo "..."
echo "[`date '+%Y-%m-%d %H:%M:%S'`] INFO: End sftp upload..."

exit 0
