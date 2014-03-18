drop table default_unli_log;
create table default_unli_log (
  id number not null,
  msisdn varchar(12) not null,
  tran_dt date not null,
  tran_tm date not null,
  mnc varchar2(30) not null,
  mcc varchar2(30) not null,
  sgsn_ip varchar2(30) not null,
  unli_flag number default 0 not null,
  expiry_date date not null,
  dt_created date not null,
  file_name varchar2(100) not null,
  CONSTRAINT default_unli_log_pk primary key (id),
  CONSTRAINT default_unli_log_uk unique (msisdn)
);

create table roaming_partners (    
  id number not null enable,
  mnc varchar2(30) not null,
  mcc varchar2(30) not null,
  tz  varchar2(30) not null,
  constraint roaming_partners_pk primary key (id),
  constraint roaming_partners_uk unique (mnc, mcc)
);

create table mcc_messages (    
  id number not null enable,
  mcc varchar2(30) not null,
  message_type varchar2(30),
  msg varchar2(2000),
  dt_created date not null,
  constraint mcc_messages_pk primary key (id),
  constraint mcc_messages_uk unique (mcc, message_type)
);

create table radcom_downloaded_files (    
  id number not null enable,
  file_name varchar2(100) not null,
  no_valid number default 0 not null,
  no_errors number default 0 not null,
  dt_uploaded date not null,
  constraint radcom_downloaded_files_pk primary key (id),
  constraint radcom_downloaded_files_uk unique (file_name)
);

create sequence  default_unli_seq  minvalue 1 maxvalue 999999999999999999999999999 increment by 1 start with 222 nocache  order  nocycle;
create sequence  radcom_downloaded_files_seq  minvalue 1 maxvalue 999999999999999999999999999 increment by 1 start with 222 nocache  order  nocycle;
create sequence  broadcast_log_seq  minvalue 1 maxvalue 999999999999999999999999999 increment by 1 start with 222 nocache  order  nocycle;

create or replace procedure sp_process_default_unli (
  p_retr out number,
  p_msisdn in varchar2,
  p_mnc in varchar2,
  p_mcc in varchar2,
  p_sgsn_ip in varchar2,
  p_date in varchar2,
  p_filename in varchar2 ) is
  
  -- 1   - Valid Transaction 
  -- 101 - Invalid MSISDN
  -- 102 - Invalid MNC
  -- 103 - Invalid MCC
  -- 104 - Invalid SGSN_IP
  -- 105 - invalid DATE
  -- 106 - invalid FILENAME
  -- 107 - Duplicate entry
  
  vMsg_1st Varchar2(2000);
  vMsg_Swi Varchar2(2000);
  nRetr    Number;
  vPartner Number; 
  vTranDate Date;

begin

  nRetr := 1;

  if  p_msisdn is null  then nRetr := 101;
  elsif p_mnc is null then nRetr := 102;
  elsif p_mcc is null then nRetr := 103;
  elsif p_sgsn_ip is null then nRetr := 104;
  elsif p_date is null then nRetr := 105;
  elsif p_filename is null then nRetr := 106;
  end if;

  begin
     select to_date(substr(p_date, 1, 19), 'YYYY/MM/DD HH24:MI:SS') into vTranDate from dual;
  exception 
     when others then 
        nRetr := 105;
  end;

  -- check mnc
  begin
     select 1 into vPartner
     from roaming_partners
     where mnc = p_mnc;
  exception 
     when no_data_found then 
        vPartner := 0;
  end;

  -- check mcc
  if vPartner = 1 then 
    begin
       select msg
       into vMsg_1st
       from mcc_messages
       where mcc = p_mcc
       and   message_type = 'UNLI_1ST_USAGE';
    exception 
       when no_data_found then 
          nRetr := 103;
    end;
  else
    begin
       select msg
       into vMsg_Swi
       from mcc_messages
       where mcc = p_mcc
       and   message_type = 'UNLI_SWITCH';
    exception 
       when no_data_found then 
          nRetr := 103;
    end;
  end if;

  if nRetr = 1 then
     if vMsg_Swi is not null then
        UPDATE default_unli_log
        SET    tran_dt = trunc(vTranDate), 
               tran_tm = vTranDate, 
               expiry_date = vTranDate+(12/24),
               unli_flag=0
        WHERE  msisdn = p_msisdn
        AND    expiry_date <= vTranDate;
        begin
           insert into broadcast_log (id, msisdn, message_id, tx_date, status, cluster_node, msg)
           values (broadcast_log_seq.nextval, p_msisdn, 'UNLI_SWITCH', vTranDate, 0, 1, vMsg_Swi);
        exception 
           when dup_val_on_index then 
              nRetr := 107;
        end;
     else
        begin
           insert into default_unli_log (id, msisdn, mnc, mcc, sgsn_ip, tran_dt, tran_tm, expiry_date, dt_created, unli_flag, file_name)
           values (default_unli_seq.nextval, p_msisdn, p_mnc, p_mcc, p_sgsn_ip, trunc(vTranDate), vTranDate, vTranDate+(12/24), sysdate, 1, p_filename);
        exception 
           when dup_val_on_index then 
              UPDATE default_unli_log
              SET    tran_dt = trunc(vTranDate), 
                     tran_tm = vTranDate, 
                     expiry_date = vTranDate+(12/24),
                     unli_flag=1
              WHERE  msisdn = p_msisdn
              AND    expiry_date <= vTranDate;
              if sql%notfound then
                 nRetr := 107;
              end if;
        end;
        if vMsg_1st is not null then
           begin
              insert into broadcast_log (id, msisdn, message_id, tx_date, status, cluster_node, msg)
              values (broadcast_log_seq.nextval, p_msisdn, 'UNLI_1ST_USAGE', vTranDate, 0, 1, vMsg_1st);
           exception 
              when dup_val_on_index then 
                 nRetr := 107;
           end;
        end if;
     end if;
     commit;
  end if;

  -- track files uploaded 
  if nRetr = 1 then
     update radcom_downloaded_files
     set    no_valid = no_valid + 1
     where  file_name = p_filename;
     if sql%notfound then
        begin
           insert into radcom_downloaded_files (id, file_name, no_valid, no_errors, dt_uploaded )
           values (radcom_downloaded_files_seq.nextval, p_filename, 1, 0, sysdate);
        exception
           when dup_val_on_index then null;
        end;
     end if;
     commit;
  else
     update radcom_downloaded_files
     set    no_errors = no_errors + 1
     where  file_name = p_filename;
     if sql%notfound then
        begin
           insert into radcom_downloaded_files (id, file_name, no_valid, no_errors, dt_uploaded )
           values (radcom_downloaded_files_seq.nextval, p_filename, 0, 1, sysdate);
        exception
           when dup_val_on_index then null;
        end;
     end if;
     commit;
  end if;

  -- set return parameters
  p_retr := nvl(nRetr,0);

end sp_process_default_unli;
/
show err

create or replace procedure sp_get_radcom_file_format (
  p_retr out number,
  p_filename in varchar2 ) is
  --   1 - Success - Not yet processed
  --   110 - Error - Already processed
  nRetr    Number;
  nCheck   Number;
begin
  nRetr := 1;

  begin
     select 1
     into   nCheck
     from   radcom_downloaded_files
     where  file_name = p_filename;
     nRetr := 110;
  exception 
     when no_data_found then null;
  end;

  -- set return parameters
  p_retr     := nRetr;
  --p_filename := to_char(trunc(sysdate), 'YYYY_MM_DD_HH24_MI"_Gi*"');
end sp_get_radcom_file_format;
/



---------------------------------------------------------------------------------------------------------
-- #### Sample SQLPlus stored procedure call
set serveroutput on
declare
  nRetr Number;
  vFileName Varchar2(160);
begin
  sp_get_radcom_file_format(nRetr, vFileName);
  dbms_output.put_line('nRetr:' || to_char(nRetr) || ' Filname:' || vFileName);
end;
/


delete from default_unli_log;
delete from broadcast_log;
commit;

set serveroutput on
declare
  nRetr Number;
begin
  sp_process_default_unli(nRetr, '639178250091', '3', '515', '192.168.116.101', '2014/02/03 07:48:25.178', '2014_02_03_07_49_Gi_pid21942_ins1_thread_1_358.csv');
  dbms_output.put_line('nRetr:' || to_char(nRetr));
end;
/
select * from broadcast_log where id > 200;


col mcc form a6
col mnc form a6
col file_name form a60
col sgsn_ip form a16
select * from default_unli_log;
select * from radcom_downloaded_files;

-- Error Codes
--   -- 1   - Valid Transaction 
--   -- 101 - Invalid MSISDN
--   -- 102 - Invalid MNC
--   -- 103 - Invalid MCC
--   -- 104 - Invalid SGSN_IP
--   -- 105 - invalid DATE
--   -- 106 - invalid FILENAME
--   -- 107 - Duplicate entry
-- 
-- To start application
-- 
-- $ cd /home/oracle/fun2/app
-- $ app/fun2_default_unli -c cfg/fun2_default_unli.cfg
-- $ app/fun2_broadcast -c cfg/fun2_broadcast.cfg
-- $ app/fun2_response -c cfg/fun2_response.cfg
-- 
-- To stop
-- $ ps -eaf | grep fun2
-- $ kill ?
-- 
-- 
-- To view log files
-- 
-- $ tail -f log/fun2_response.log log/fun2_broadcast.log log/fun2_default_unli.log
-- 
-- To check downloaded files:
-- /home/oracle/fun2/app/local
-- /home/oracle/fun2/app/local/ignored
-- /home/oracle/fun2/app/local/completed
