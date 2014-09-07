#!/bin/sh

pid=trigger_owner
pidpw=trgown123
ORACLE_SID=trgprd
ORACLE_HOME=/u01/app/oracle/product/10.2.0
export ORACLE_SID ORACLE_HOME pid pidpw

PATH=$PATH:$ORACLE_HOME/bin
export PATH

cd /appl/fun2_default_unli/log 
echo >> fun2_unli_notifications.log
echo "START OF UNLI NOTIFICATIONS @ `date`" >> fun2_unli_notifications.log

sqlplus -s ${pid}/${pidpw}@${ORACLE_SID} << EOF >> fun2_unli_notifications.log
set feedback off
set serveroutput on
exec sp_generate_unli_notifications;
exit
EOF

echo >> fun2_unli_notifications.log
echo "END OF UNLI NOTIFICATIONS @ `date`" >> fun2_unli_notifications.log
echo >> fun2_unli_notifications.log
echo >> fun2_unli_notifications.log


