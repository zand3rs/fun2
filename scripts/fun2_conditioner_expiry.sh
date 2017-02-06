#!/bin/sh

pid=trigger_owner
pidpw=trgown123
ORACLE_SID=trgprd
ORACLE_HOME=/u01/app/oracle/product/10.2.0
export ORACLE_SID ORACLE_HOME pid pidpw

PATH=$PATH:$ORACLE_HOME/bin
export PATH

cd /appl/log/fun2
echo >> fun2_conditioner_expiry.log
echo "START OF CONDITIONER Expiry @ `date`" >> fun2_conditioner_expiry.log

sqlplus -s ${pid}/${pidpw}@${ORACLE_SID} << EOF >> fun2_conditioner_expiry.log
set feedback off
set serveroutput on
exec sp_generate_conditioner_expiry;
exit
EOF

echo >> fun2_conditioner_expiry.log
echo "END OF CONDITIONER Expiry @ `date`" >> fun2_conditioner_expiry.log
echo >> fun2_conditioner_expiry.log
echo >> fun2_conditioner_expiry.log

