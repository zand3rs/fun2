#!/bin/bash

ALERT_CONFIG=/appl/yiucon/alerts/cfg/alerts.cfg 
echo "`date` PING CHECK"
for XIP in `cat ${ALERT_CONFIG} | grep ^IP_ADDRESS | awk {'print $3'}`
do

xout=`ping -c 1 $XIP  | grep transmitted | awk {'print $4'}`

if [ ${xout} -eq 0 ]
then
echo "$XIP not reachable..."
#BROADCAST
else
echo "`date` $XIP is alive"
fi
done

echo

echo "`date` NETSTAT CHECK"
for XIP in `cat ${ALERT_CONFIG} | grep ^IP_ADDRESS | awk {'print $3'}`
do

xout=`netstat -an | grep ${XIP} | grep ESTABLISHED | sed -e 's/   / /g'`
xcnt=`echo $xout | grep ESTABLISHED | wc -l | awk {'print $1'}`

if [ ${xcnt} -eq 0 ]
then
echo "$XIP is DOWN!!!"
#BROADCAST
else
echo "`date` $xout"
fi

done
echo "`date`:`whoami` health checked milestone " >> /appl/yiucon/menu/log/menu.log
echo " Done"

