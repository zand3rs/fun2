#!/bin/ksh

pending=`/u01/app/oracle/product/10.2.0/bin/sqlplus -s mls_owner/mls_owner123@vm2dbprd1 << EOF
set head off;
select count(*) 
from mobiletoyahoo 
where status=0 
and node=1;
exit
EOF`

echo $pending

#update mobiletoyahoo set node=2 where status=0 and node=1;
