select 'FUN2_EXT.' || to_char(sysdate,'YYYYMMDD"."HH24"0002."') || count(1) || '.txt'  from (
select msisdn || ',' || mnc || ',' || mcc || ',' || to_char(tran_tm, 'MM/DD/YYYY HH24:MI:SS') sql_out
from   default_unli_exp
where  tran_dt =  trunc(sysdate)-1
union
select msisdn || ',' || mnc || ',' || mcc || ',' || to_char(tran_tm, 'MM/DD/YYYY HH24:MI:SS') sql_out
from   default_unli_hist
where  tran_dt =  trunc(sysdate)-1);
exit
