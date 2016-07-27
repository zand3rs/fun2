set feedback off
set trimspool on
set pages 0
set head off
select sql_out from (
select trunc(sysdate)-2 expiry_tm, '"MSISDN","START DATE TIME","MCC","MNC"' sql_out
from   dual
union
select expiry_tm, '"' || msisdn || '","' || to_char(tran_tm, 'MM/DD/YYYY HH24:MI:SS') || '","' || mcc || '","' || mnc || '"' sql_out
from   default_unli_exp
where  tran_dt =  trunc(sysdate)-1
union
select expiry_tm, '"' || msisdn || '","' || to_char(tran_tm, 'MM/DD/YYYY HH24:MI:SS') || '","' || mcc || '","' || mnc || '"' sql_out
from   default_unli_hist
where  tran_dt =  trunc(sysdate)-1
) order by expiry_tm;
exit
