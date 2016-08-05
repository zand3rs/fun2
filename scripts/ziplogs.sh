#!/bin/sh

echo `date +'%d-%b-%Y'` > date1.txt
DT=`date +'%Y%m'`
DT2=`date +'%y%m'`
HR=`date +'%H'`
cat date1.txt | awk -F '-' '{print $1}' > day.txt

DD=`cat day.txt | tr -d ' '`

NWD=`expr $DD - 1`
NWD2=`expr $DD - 2`
NWHR=`expr $HR - 1`
NWD3=`expr $DD - 0`
DT3=`date +'%y%m%d'`
DT4=`date +'%Y_%m_%d_'`
DT5=`date +'%Y_%m_'`

count=`ps -ef | grep ziplogs.sh | wc -l`

echo `date +'%c'`
echo "zipping previous dates.."


if [ ${NWD} -lt 10 ]
then
  /usr/bin/gzip -f /appl/default_unli/downloaded_files/completed/${DT5}0${NWD}*.csv
else
  /usr/bin/gzip -f /appl/default_unli/downloaded_files/completed/${DT5}${NWD}*.csv
fi
echo "done zipping previous dates..."


if [ ${count} -gt 5 ]
then
  echo  "still running.."
  echo ${count}
  exit
else

  echo "zipping current date files..."
   PREVIOUS_1_MINUTE=$(date '+%Y_%m_%d_%H_%M' -d '8 hours ago 20 minutes ago')
   PREVIOUS_2_MINUTE=$(date '+%Y_%m_%d_%H_%M' -d '8 hours ago 19 minutes ago')
   PREVIOUS_3_MINUTE=$(date '+%Y_%m_%d_%H_%M' -d '8 hours ago 18 minutes ago')
   PREVIOUS_4_MINUTE=$(date '+%Y_%m_%d_%H_%M' -d '8 hours ago 17 minutes ago')
   PREVIOUS_5_MINUTE=$(date '+%Y_%m_%d_%H_%M' -d '8 hours ago 16 minutes ago')
   FILE_PATTERN_1="$PREVIOUS_1_MINUTE*Gi*.csv"
   FILE_PATTERN_2="$PREVIOUS_2_MINUTE*Gi*.csv"
   FILE_PATTERN_3="$PREVIOUS_3_MINUTE*Gi*.csv"
   FILE_PATTERN_4="$PREVIOUS_4_MINUTE*Gi*.csv"
   FILE_PATTERN_5="$PREVIOUS_5_MINUTE*Gi*.csv"
   
   echo "File pattern $FILE_PATTERN_1 ..."
   echo "File pattern $FILE_PATTERN_2 ..."
   echo "File pattern $FILE_PATTERN_3 ..."
   echo "File pattern $FILE_PATTERN_4 ..."
   echo "File pattern $FILE_PATTERN_5 ..."
   /usr/bin/gzip -f /appl/default_unli/downloaded_files/completed/$FILE_PATTERN_1
   /usr/bin/gzip -f /appl/default_unli/downloaded_files/completed/$FILE_PATTERN_2
   /usr/bin/gzip -f /appl/default_unli/downloaded_files/completed/$FILE_PATTERN_3
   /usr/bin/gzip -f /appl/default_unli/downloaded_files/completed/$FILE_PATTERN_4
   /usr/bin/gzip -f /appl/default_unli/downloaded_files/completed/$FILE_PATTERN_5
  echo "done.."
fi

exit
/

