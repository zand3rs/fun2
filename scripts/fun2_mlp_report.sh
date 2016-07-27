#!/bin/sh

pid=trigger_owner
pidpw=triggerowner123
ORACLE_SID=trgdev
ORACLE_HOME=/u01/app/oracle/product/10.2.0
FUN2_SCRIPT=/appl/fun2/scripts
FILEDIR='/appl/mlp'

export ORACLE_SID ORACLE_HOME FUN2_SCRIPT FILEDIR pid pidpw

PATH=$PATH:$ORACLE_HOME/bin
export PATH

FILENM_LIST=$( sqlplus ${pid}/${pidpw}@${ORACLE_SID} @$FUN2_SCRIPT/fun2_gen_mlp_file.sql | grep '^FUN2.' | tr -d '\t' | tr -d ' ' )

#echo $FILENM_DATA" : "$FILENM_LIST

cd /appl/log/fun2
echo >> fun2_feed_for_mlp.log
echo "START OF MLP REPORT @ `date`" >> fun2_feed_for_mlp.log

sqlplus -s ${pid}/${pidpw}@${ORACLE_SID} << EOF >> fun2_feed_for_mlp.log
set feedback off
set pages 0
set head off
spool ${FILEDIR}/${FILENM_LIST}
@${FUN2_SCRIPT}/fun2_gen_mlp_data.sql
spoo off
exit
EOF

echo >> fun2_feed_for_mlp.log
echo "END OF MLP REPORT @ `date`" >> fun2_feed_for_mlp.log
echo >> fun2_feed_for_mlp.log
echo >> fun2_feed_for_mlp.log

chmod 775 ${FILEDIR}/${FILENM_DATA}
chmod 775 ${FILEDIR}/${FILENM_LIST}
