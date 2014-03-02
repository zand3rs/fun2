#!/bin/sh

pid=trigger_owner
pidpw=trgown123
ORACLE_SID=trgprd
ORACLE_HOME=/u01/app/oracle/product/10.2.0
export ORACLE_SID ORACLE_HOME pid pidpw

PATH=$PATH:$ORACLE_HOME/bin
export PATH 

cd /home/isoladm/app/log
echo >> fun2_expire.log
echo "START OF EXPIRING PENDING FUN REQUEST @ `date`" >> fun2_expire.log

sqlplus -s ${pid}/${pidpw}@${ORACLE_SID} << EOF >> fun2_expire.log
exec SP_PROCESS_FOR_EXPIRE_PENDING;
exit
EOF

echo >> fun2_cancel.log
echo "END OF EXPIRING PENDING FUN REQUEST @ `date`" >> fun2_expire.log
echo >> fun2_expire.log
echo >> fun2_expire.log



