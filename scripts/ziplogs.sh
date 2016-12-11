#!/bin/sh

curr_date=`date '+%Y%m%d' --date='yesterday'`
curr_time=`date '+%H%M%S'`
#echo "curr_time: $curr_time"
if [ ${curr_time} -lt '000003' ]
then
   mv /appl/log/fun2/ziplogs.log /appl/log/fun2/ziplogs.log.$curr_date
   echo "ziplogs.log.$date .......... Done."
   echo "Successfully Processed."
   > /appl/log/fun2/ziplogs.log
   chmod 666 /appl/log/fun2/ziplogs.log
fi

count=`ps -ef | grep ziplogs.sh | wc -l`
echo `date +'%c'`
echo "No. Runnning Processes: ${count}"
if [ ${count} -gt 5 ]
then
  echo  "Still running.."
  echo ${count}
  exit
else
  echo "Compressing completed Radcom files..."
  /usr/bin/find /appl/fun2_default_unli/downloaded_files/completed/ -name "*.csv" -mmin +2 | head -800 | awk '{printf("ls -l %s\n", $1);}' | sh | wc -l
  /usr/bin/find /appl/fun2_default_unli/downloaded_files/completed/ -name "*.csv" -mmin +2 | head -800 | awk '{printf("/usr/bin/gzip -f %s\n", $1);}' | sh
  echo "done.."
fi
echo `date +'%c'`

exit


