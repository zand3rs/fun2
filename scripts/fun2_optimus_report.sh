#!/bin/sh

pid=trigger_owner
pidpw=trgown123
ORACLE_SID=trgprd
ORACLE_HOME=/u01/app/oracle/product/10.2.0
FUN2_SCRIPT=/home/isoladm/app/scripts
FILEDIR='/home/isoladm/app/local'
export ORACLE_SID ORACLE_HOME FUN2_SCRIPT FILEDIR pid pidpw

PATH=$PATH:$ORACLE_HOME/bin
export PATH

FILENM_DATA=$( sqlplus ${pid}/${pidpw}@${ORACLE_SID} @$FUN2_SCRIPT/fun2_gen_optimus_fnamedata.sql | grep '^FUN2_EXT.' | tr -d '\t' | tr -d ' ' )
FILENM_LIST=$( sqlplus ${pid}/${pidpw}@${ORACLE_SID} @$FUN2_SCRIPT/fun2_gen_optimus_fnamelist.sql | grep '^FUN2_EXT.' | tr -d '\t' | tr -d ' ' )

echo $FILENM_DATA" : "$FILENM_LIST

cd /home/isoladm/app/log
echo >> fun2_feed_for_optimus.log
echo "START OF OPTIMUS REPORT @ `date`" >> fun2_feed_for_optimus.log

sqlplus -s ${pid}/${pidpw}@${ORACLE_SID} << EOF >> fun2_feed_for_optimus.log
spool ${FILEDIR}/${FILENM_LIST}
@${FUN2_SCRIPT}/fun2_gen_optimus_filelist.sql
spoo off
exit
EOF

sqlplus -s ${pid}/${pidpw}@${ORACLE_SID} << EOF >> fun2_feed_for_optimus.log
spool ${FILEDIR}/${FILENM_DATA}
@${FUN2_SCRIPT}/fun2_gen_optimus_filedata.sql
spoo off
exit
EOF

echo >> fun2_unli_notifications.log
echo "END OF OPTIMUS REPORT @ `date`" >> fun2_feed_for_optimus.log
echo >> fun2_feed_for_optimus.log
echo >> fun2_feed_for_optimus.log

