#!/bin/sh

pid=trigger_owner
pidpw=trgown123
ORACLE_SID=trgprd
ORACLE_HOME=/u01/app/oracle/product/10.2.0
export ORACLE_SID ORACLE_HOME pid pidpw

PATH=$PATH:$ORACLE_HOME/bin
export PATH

cd /appl/fun2/log
echo >> fun2_mlp_notifications.log
echo "START OF MLP NOTIFICATIONS @ `date`" >> fun2_mlp_notifications.log

sqlplus -s ${pid}/${pidpw}@${ORACLE_SID} << EOF >> fun2_mlp_notifications.log
set feedback off
set serveroutput on
exec sp_generate_mlp_notifications;
exit
EOF

echo >> fun2_mlp_notifications.log
echo "END OF MLP NOTIFICATIONS @ `date`" >> fun2_mlp_notifications.log
echo >> fun2_mlp_notifications.log
echo >> fun2_mlp_notifications.log
