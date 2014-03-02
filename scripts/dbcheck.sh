#!/bin/ksh

echo "Checking DB Connectivity"
echo "========================"
sleep 1

for i in `grep ^DB_HOSTS /appl/fun2/alerts/cfg/alerts.cfg | awk {'print $2'}`
do
echo -e "DB NAME: $i    \c"

sleep 1

res=`echo -e "SELECT 123 from dual\n/\n\n" | sqlplus -s trigger_owner/trgown123@TRGPRD | egrep -v '^[[:space:]]*$' | tail -1 | sed 's/\t//g' | tr -d ' '`
if [[ "a$res" == "a123" ]]
then
        echo -e "OK"
	echo -e "FUN2 is connected to DB."
else
	echo "NOK"
	echo "CC, Pls check FUN2 could not connect to DB!"
fi
done
echo "`date`:`whoami` checked FUN2 DB connection" >> /appl/fun2/menu/log/menu.log
echo " Done"

