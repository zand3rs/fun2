#!/bin/sh

pid=trigger_owner
pidpw=trgown123
ORACLE_SID=trgprd
ORACLE_HOME=/u01/app/oracle/product/10.2.0
export ORACLE_SID ORACLE_HOME pid pidpw

PATH=$PATH:$ORACLE_HOME/bin
export PATH 

cd /home/isoladm/app/log
echo >> fun2_cancel.log
echo "START OF DAILY DEACTIVATION PROCESS @ `date`" >> fun2_cancel.log

sqlplus -s ${pid}/${pidpw}@${ORACLE_SID} << EOF >> fun2_cancel.log
exec SP_PROCESS_FOR_CANCEL;
exit
EOF

echo >> fun2_cancel.log
echo "END OF DAILY DEACTIVATION PROCESS @ `date`" >> fun2_cancel.log
echo >> fun2_cancel.log
echo >> fun2_cancel.log



