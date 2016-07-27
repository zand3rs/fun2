set feedback off
set trimspool on
set pages 0
set head off
select sql_data from (
select 1 seq_no, '00' || chr(9) || to_char(sysdate, 'YYYYMMDDHH24MISS') sql_data from dual union
select 2 seq_no, '01' || chr(9) || file_name || chr(9) || nvl(d_bytes,'0') sql_data from (
    select 'FUN2_EXT.' || to_char(sysdate,'YYYYMMDD"."HH24"0002."') || (count(1)-1) || '.txt' file_name, sum(length(sql_out)+1) d_bytes from (
    select '"MSISDN","START DATE TIME","MCC","MNC"' sql_out
    from   dual
    union
    select '"' || msisdn || '","' || to_char(tran_tm, 'MM/DD/YYYY HH24:MI:SS') || '","' || mcc || '","' || mnc || '"' sql_out
    from   default_unli_exp
    where  tran_dt =  trunc(sysdate)-1
    union
    select '"' || msisdn || '","' || to_char(tran_tm, 'MM/DD/YYYY HH24:MI:SS') || '","' || mcc || '","' || mnc || '"' sql_out
    from   default_unli_hist
    where  tran_dt =  trunc(sysdate)-1 )
    ) union
select 3 seq_no, '99' || chr(9) || '1' sql_data from dual
) order by seq_no;
exit
