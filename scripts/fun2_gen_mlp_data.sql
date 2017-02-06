select substr(a.msisdn,3) || chr(9) || '"' || b.service_name ||  '"' || chr(9) || a.remaining_days || chr(9) || to_char(a.last_usage_tm, 'MM/DD/YYYY HH24:MI:SS') sql_out
from   mlp_log a, mlp_services b
where  a.soc_id = b.soc_id
and    a.last_usage_tm >= trunc(sysdate) -1
and    a.last_usage_tm is not null
union
select substr(a.msisdn,3) || chr(9) || '"' || b.service_name ||  '"' || chr(9) || a.remaining_days || chr(9) || to_char(a.last_usage_tm, 'MM/DD/YYYY HH24:MI:SS') sql_out
from   mlp_log a, mlp_services b
where  a.soc_id = b.soc_id
and    a.dt_created >= trunc(sysdate) -1
and    a.last_usage_tm is null
;
exit

