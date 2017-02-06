#!/bin/bash

echo `date +'%c'`
echo "Start Cipher upload [fun2_cipher.sh] ..."

count=`ps -eaf | grep cipher | grep -v grep | grep -v tail | grep -v bin | wc -l`
session_text=`ps -eaf | grep cipher | grep -v grep | grep -v tail | grep -v bin`

echo "[`date '+%Y-%m-%d %H:%M:%S'`] Sessions:"
echo ${session_text}
echo ${count}

echo "[`date '+%Y-%m-%d %H:%M:%S'`] Removing stale lockfile :"
/usr/bin/find /tmp -maxdepth 1 -name "fun2_ciher.sh.lockfile" -mmin +30 -exec rm -f {} +

echo "[`date '+%Y-%m-%d %H:%M:%S'`] Checking for existing lockfile :"
lockfile=`/usr/bin/find /tmp -maxdepth 1 -name "fun2_ciher.sh.lockfile"| wc -l`
if [ ${lockfile} -gt 0 ]
then
        echo "Previous fun2_cipher is still running!"
        exit 0
else

        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Creating lockfile"
        touch /tmp/fun2_ciher.sh.lockfile
        chmod 600 /tmp/fun2_ciher.sh.lockfile

        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Setting variables"

        #PROD
        FTP_HOST="10.237.181.204"

        #DEV
        #FTP_HOST="10.237.197.137"
        FTP_USER="ereadm"
        FTP_PASS="ere123$$$$"

        #PROD
        REMOTE_DIR="/data/FUN2"
        #DEV
        #REMOTE_DIR="/data/FUN2/test"

        LOCAL_DIR="/appl/fun2_default_unli/downloaded_files/completed"

        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Running getops"

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

        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Done running getops"

        #       Cleanup temp
        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Initial Cleanup transfer_temp/"
        #/usr/bin/find /appl/fun2_default_unli/downloaded_files/transfer_temp -maxdepth 1 -name "*Gi*.csv" -mmin +4 -exec rm -f {} +
        rm -f /appl/fun2_default_unli/downloaded_files/transfer_temp/*Gi*.csv

        #       Transfer to temp
        #echo "[`date '+%Y-%m-%d %H:%M:%S'`] Copying to transfer_temp/" >> /tmp/fun2_cipher_custom.log
        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Copying to transfer_temp/"

        #               -- Actual timestamp

        #/usr/bin/find /appl/fun2_default_unli/downloaded_files/completed/ -maxdepth 1 -name "*Gi*.csv" -mmin -2 | head -400 | awk '{printf("cp %s -u -t /appl          /fun2_default_unli/downloaded_files/transfer_temp/\n", $1);}' | sh

        FOR_CTR=0
        IS_TRANSFERRED=0
        TRANSFER_CTR=0
        DUPLICATE_CTR=0

        for i in $(find /appl/fun2_default_unli/downloaded_files/completed/ -maxdepth 1 -name "*Gi*.csv" -mmin -20);
        do

                IS_TRANSFERRED=`grep $i /tmp/fun2_cipher_transfer_temp.log | wc -l`
                #echo "[`date '+%Y-%m-%d %H:%M:%S'`] IS_TRANSFERRED : "
                #echo $IS_TRANSFERRED

                if [ $IS_TRANSFERRED -le 0 ]
                then
                        cp $i -u -t /appl/fun2_default_unli/downloaded_files/transfer_temp/
                        echo "$i" >> /tmp/fun2_cipher_transfer_temp.log
                        TRANSFER_CTR=`expr $TRANSFER_CTR + 1`
                        FOR_CTR=`expr $FOR_CTR + 1`
                else
                        DUPLICATE_CTR=`expr $DUPLICATE_CTR + 1`
                fi

                if [ $FOR_CTR -ge 300 ]
                then
                        echo "[`date '+%Y-%m-%d %H:%M:%S'`] FOR_CTR Limit reached : $FOR_CTR"
                        break
                fi
        done

        echo "[`date '+%Y-%m-%d %H:%M:%S'`] TRANSFER_CTR : $TRANSFER_CTR"
        echo "[`date '+%Y-%m-%d %H:%M:%S'`] DUPLICATE_CTR : $DUPLICATE_CTR"

        #echo "[`date '+%Y-%m-%d %H:%M:%S'`] Finished copying to transfer_temp/" >> /tmp/fun2_cipher_custom.log
        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Finished copying to transfer_temp/"

        count=`/usr/bin/find /appl/fun2_default_unli/downloaded_files/transfer_temp/ -maxdepth 1 -name "*Gi*.csv"| wc -l`
        if [ ${count} -lt 1 ]
        then
                echo "[`date '+%Y-%m-%d %H:%M:%S'`] No files to transfer"

        else

                echo "[`date '+%Y-%m-%d %H:%M:%S'`] Start transferring files"
/usr/bin/expect -f - <<EOFEXPECT
set timeout 450
spawn sftp $FTP_USER@$FTP_HOST
expect "sftp>"
send "lcd $LOCAL_DIR\r";
expect "sftp>"
send "cd $REMOTE_DIR\r";
expect "sftp>"
send "mput /appl/fun2_default_unli/downloaded_files/transfer_temp/*Gi*.csv\r"
expect "sftp>"

send "bye\r";
expect eof
EOFEXPECT

                echo "[`date '+%Y-%m-%d %H:%M:%S'`] Finished transferring files"

        fi

        #       Cleanup temp
        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Final Cleanup transfer_temp/"
        #/usr/bin/find /appl/fun2_default_unli/downloaded_files/transfer_temp -maxdepth 1 -name "*Gi*.csv" -mmin +4 -exec rm -f {} +
        rm -f /appl/fun2_default_unli/downloaded_files/transfer_temp/*Gi*.csv

        #               Cleanup transfer_temp.log
        PREVIOUS_5_HOUR=$(date '+%Y_%m_%d_%H' -d '13 hours ago')
        PREVIOUS_4_HOUR=$(date '+%Y_%m_%d_%H' -d '12 hours ago')
        PREVIOUS_3_HOUR=$(date '+%Y_%m_%d_%H' -d '11 hours ago')
        PREVIOUS_2_HOUR=$(date '+%Y_%m_%d_%H' -d '10 hours ago')
        #PREVIOUS_1_HOUR=$(date '+%Y_%m_%d_%H' -d '9 hours ago')
        FILE_PATTERN_5="$PREVIOUS_5_HOUR"
        FILE_PATTERN_4="$PREVIOUS_4_HOUR"
        FILE_PATTERN_3="$PREVIOUS_3_HOUR"
        FILE_PATTERN_2="$PREVIOUS_2_HOUR"
        #FILE_PATTERN_1="$PREVIOUS_1_HOUR"

        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Start cleanup of /tmp/fun2_cipher_transfer_temp.log"

        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Clearing $FILE_PATTERN_5"
        sed -i.tmp -e "/$FILE_PATTERN_5/ d" /tmp/fun2_cipher_transfer_temp.log
        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Clearing $FILE_PATTERN_4"
        sed -i.tmp -e "/$FILE_PATTERN_4/ d" /tmp/fun2_cipher_transfer_temp.log
        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Clearing $FILE_PATTERN_3"
        sed -i.tmp -e "/$FILE_PATTERN_3/ d" /tmp/fun2_cipher_transfer_temp.log
        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Clearing $FILE_PATTERN_2"
        sed -i.tmp -e "/$FILE_PATTERN_2/ d" /tmp/fun2_cipher_transfer_temp.log
        #echo "[`date '+%Y-%m-%d %H:%M:%S'`] Clearing $FILE_PATTERN_1"
        #sed -i.tmp -e "/$FILE_PATTERN_1/ d" /tmp/fun2_cipher_transfer_temp.log



        echo "[`date '+%Y-%m-%d %H:%M:%S'`] End cleanup of /tmp/fun2_cipher_transfer_temp.log"

        echo "[`date '+%Y-%m-%d %H:%M:%S'`] Removing lockfile"
        rm -f /tmp/fun2_ciher.sh.lockfile
        echo "[`date '+%Y-%m-%d %H:%M:%S'`] End sftp upload..."
        echo "----------------------------------------------------------"
        exit 0

fi

