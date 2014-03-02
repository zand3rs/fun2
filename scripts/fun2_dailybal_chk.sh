#!/bin/sh

pid=trigger_owner
pidpw=trgown123
ORACLE_SID=trgprd
ORACLE_HOME=/u01/app/oracle/product/10.2.0
export ORACLE_SID ORACLE_HOME pid pidpw

PATH=$PATH:$ORACLE_HOME/bin
export PATH 

cd /home/isoladm/app/log
echo >> fun2_dailybal.log
echo "START OF DAILY BALANCE CHECK @ `date`" >> fun2_dailybal.log

sqlplus -s ${pid}/${pidpw}@${ORACLE_SID} << EOF >> fun2_dailybal.log
exec SP_PROCESS_FOR_DAILY_BAL_CHK;
exit
EOF

echo >> fun2_dailybal.log
echo "END OF DAILY BALANCE CHECK @ `date`" >> fun2_dailybal.log
echo >> fun2_dailybal.log
echo >> fun2_dailybal.log


