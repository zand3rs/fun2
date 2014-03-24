set feedback off
set pages 0
set head off
select sql_data from (
select 1 seq_no, '00' || chr(9) || to_char(sysdate, 'YYYYMMDDHH24MISS') sql_data from dual union
select 2 seq_no, '01' || chr(9) || file_name || chr(9) || d_bytes sql_data from (
    select 'FUN2_EXT.' || to_char(sysdate,'MM/DD/YYYY"."HH24MISS"."') || count(1) || '.txt' file_name, sum(d_bytes) d_bytes from (
    select msisdn || ',' || mnc || ',' || mcc || ',' || to_char(original_dt, 'MM/DD/YYYY HH24:MI:SS') sql_out, 81 d_bytes
    from   default_unli_exp
    where  tran_dt =  trunc(sysdate)-1
    union
    select msisdn || ',' || mnc || ',' || mcc || ',' || to_char(original_dt, 'MM/DD/YYYY HH24:MI:SS'), 81 d_bytes 
    from   default_unli_hist
    where  tran_dt =  trunc(sysdate)-1)
    ) union
select 3 seq_no, '99' || chr(9) || '1' sql_data from dual
) order by seq_no;
exit

