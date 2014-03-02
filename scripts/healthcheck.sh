#!/bin/ksh

get_cpu()
{
ave_cpu=`sar -u 2 1 | grep Average | awk {'print $8'}`
cpu_threshold=`grep ^HEALTHCHECK ${CONFIG}/alerts.cfg | grep CPU | awk {'print $3'}`

if [ ${cpu_threshold} -gt ${ave_cpu} ]
then
echo "CPU usage for $HOSTNAME is now ${ave_cpu}% IDLE (below threshold)" | tee -a ${MESSAGE_FILE} | tee -a ${MESSAGE_ALERT}
cp ${MESSAGE_ALERT} ${MESSAGE}/health_check_`date "+%Y%m%d%H%M%S"`.msg
else
echo "CPU usage for $HOSTNAME is normal."
fi
}

get_space()
{

check_all=`grep ^HEALTHCHECK ${CONFIG}/alerts.cfg | grep DISKSPACE | grep ALL | wc -l | awk {'print $1'}`

if [ ${check_all} -eq 1 ]
then
disk_threshold=`grep ^HEALTHCHECK ${CONFIG}/alerts.cfg | grep DISKSPACE | grep ALL | awk {'print $4'}`

for i in `grep ^HEALTHCHECK ${CONFIG}/alerts.cfg | grep DISKSPACE | grep ALL | awk {'print $5'} | sed -e 's/,/ /g'`
do
curr_space=`df -k ${i} | grep "/" | cut -c 51-56 | grep % | sed -e 's/%//g' | awk {'print $1'}`

if [ ${curr_space} -gt ${disk_threshold} ]
then
echo "Disk usage for $HOSTNAME $i is now ${curr_space}% (above threshold)" | tee -a ${MESSAGE_FILE} | tee -a ${MESSAGE_ALERT}
cp ${MESSAGE_ALERT} ${MESSAGE}/health_check_`date "+%Y%m%d%H%M%S"`.msg
else
echo "Disk usage for $HOSTNAME $i is normal."
fi
done
else
for i in `grep ^HEALTHCHECK ${CONFIG}/alerts.cfg | grep DISKSPACE | awk {'print $3'}`
do
disk_threshold=`grep ^HEALTHCHECK ${CONFIG}/alerts.cfg | grep DISKSPACE | grep " $i " | awk {'print $4'}`

if [ $i = '/root' ]
then
i="/"
fi

curr_space=`df -k ${i} | grep "/" | cut -c 51-56 | grep % | sed -e 's/%//g' | awk {'print $1'}`
#echo "$i    ${curr_space}   ${disk_threshold}"
if [ ${curr_space} -gt ${disk_threshold} ]
then
echo "Disk usage for $HOSTNAME $i is now ${curr_space}% (above threshold)" | tee -a ${MESSAGE_FILE} | tee -a ${MESSAGE_ALERT}
cp ${MESSAGE_ALERT} ${MESSAGE}/health_check_`date "+%Y%m%d%H%M%S"`.msg
else
echo "Disk usage for $HOSTNAME $i is normal."
fi
done
fi
}
echo "`date`:`whoami` healthchecked `hostname` Server" >> /appl/fun2/menu/log/menu.log
echo " Done"


#MAIN
#BASE_DIR=/home/isolusr
#BIN=${BASE_DIR}/bin
#TEMP=${BASE_DIR}/temp
#CONFIG=${BASE_DIR}/cfg
#LOG=${BASE_DIR}/log
#SCRIPTS=${BASE_DIR}/scripts
#MESSAGE=${BASE_DIR}/messages/bowl
#MESSAGE_FILE=${LOG}/health_check.log
#MESSAGE_ALERT=${LOG}/health_check.msg
#RCPT=CORE_GRP,SM_GRP      #RECIPIENT LISTS

BASE_DIR=/appl/fun2
BIN=${BASE_DIR}/bin
TEMP=${BASE_DIR}/temp
CONFIG=${BASE_DIR}/alerts/cfg
LOG=/appl/log/fun2
SCRIPTS=${BASE_DIR}/scripts
MESSAGE=${BASE_DIR}/alerts/messages/bowl
MESSAGE_FILE=${LOG}/health_check.log
MESSAGE_ALERT=${LOG}/health_check.msg
#RCPT=CORE_GRP,SM_GRP      #RECIPIENT LISTS
RCPT=ME_GRP      #RECIPIENT LISTS


echo "RCPT:${RCPT}" > ${MESSAGE_ALERT}
get_cpu #| tee -a ${MESSAGE_FILE} | tee -a ${MESSAGE_ALERT}
get_space #| tee -a ${MESSAGE_FILE} | tee -a ${MESSAGE_ALERT}
#cp ${MESSAGE_ALERT} ${MESSAGE}/health_check_`date "+%Y%m%d%H%M%S"`.msg

#END PROGRAM

                                                                              
