set feedback off
set pages 0
set head off
select sql_out from (
select expiry_tm, msisdn || ',' || mnc || ',' || mcc || ',' || to_char(original_dt, 'MM/DD/YYYY HH24:MI:SS') sql_out
from   default_unli_exp
where  tran_dt =  trunc(sysdate)-1
union
select expiry_tm, msisdn || ',' || mnc || ',' || mcc || ',' || to_char(original_dt, 'MM/DD/YYYY HH24:MI:SS') sql_out
from   default_unli_hist
where  tran_dt =  trunc(sysdate)-1
) order by expiry_tm;
exit

