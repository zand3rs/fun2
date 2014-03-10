create table default_unli_log (
  id number not null,
  msisdn varchar(12) not null,
  tran_dt date not null,
  tran_tm date not null,
  mnc varchar2(30) not null,
  mcc varchar2(30) not null,
  sgsn_ip varchar2(30) not null,
  status number default 0 not null,
  expiry_date date not null,
  dt_created date not null,
  file_name varchar2(100) not null,
  CONSTRAINT default_unli_log_pk primary key (id)
);

-- create table broadcast_log (    
--   id number not null enable,
--   msisdn number(12,0) not null enable,
--   message_id varchar2(60),
--   tx_date date not null enable,
--   status number(1,0) default 0 not null enable,
--   cluster_node number(3,0) default 1 not null enable,
--   msg varchar2(2000),
--   constraint broadcast_log_pk primary key (id)
-- );

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
  
  vMsg     Varchar2(2000);
  vMsgType Varchar2(30);
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
     select to_date(substr(p_date, 1, 19), 'YYYY/DD/MM HH24:MI:SS') into vTranDate from dual;
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
       select msg, message_type 
       into vMsg, vMsgType
       from mcc_messages
       where mcc = p_mcc
       and   message_type = 1;
    exception 
       when no_data_found then 
          nRetr := 103;
    end;
  else
    begin
       select msg, message_type 
       into vMsg, vMsgType
       from mcc_messages
       where mcc = p_mcc
       and   message_type = 2;
    exception 
       when no_data_found then 
          nRetr := 103;
    end;
  end if;

  if nRetr = 1 then
     begin
        insert into default_unli_log (id, msisdn, mnc, mcc, sgsn_ip, tran_dt, tran_tm, expiry_date, dt_created, file_name)
        values (default_unli_seq.nextval, p_msisdn, p_mnc, p_mcc, p_sgsn_ip, trunc(vTranDate), vTranDate, vTranDate+(12/24), sysdate, p_filename);
     exception 
        when dup_val_on_index then 
           nRetr := 107;
     end;
     begin
        insert into broadcast_log (id, msisdn, message_id, tx_date, status, cluster_node, msg)
        values (broadcast_log_seq.nextval, p_msisdn, vMsgType, vTranDate, 0, 1, vMsg);
     exception 
        when dup_val_on_index then 
           nRetr := 107;
     end;
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
  end if;
  p_retr := nRetr;

end sp_process_default_unli;
/
show err

create or replace procedure sp_get_radcom_file_format (
  p_retr out number,
  p_filename out varchar2 ) is
  --   1 - Success
  -- 101 - Duplicate file uploaded
  nRetr    Number;
  vFileFormat Varchar2(160);
begin
  nRetr := 1;

  --select to_char(nvl(to_date(max(substr(file_name,1,16)), 'YYYY_MM_DD_HH24_MI')+(1/60/24), sysdate-1), 'YYYY_MM_DD_HH24_MI"_Gi*"')
  select max(file_name)
  into   vFileFormat
  from   radcom_downloaded_files;

  if vFileFormat is not null then
     p_filename := vFileFormat;
  else
     p_filename := to_char(trunc(sysdate), 'YYYY_MM_DD_HH24_MI"_Gi*"');
  end if;

  p_retr := nRetr;
end sp_get_radcom_file_format;
/
show err

---------------------------------------------------------------------------------------------------------
-- #### Sample SQLPlus stored procedure call
set serveroutput on
declare
  nRetr Number;
  vFileName Varchar2(160);
begin
  sp_get_radcom_file_format(nRetr, vFileName);
  dbms_output.put_line('nRetr:' || to_char(nRetr) || ' Filename:' || vFileName);
end;
/


set serveroutput on
declare
  nRetr Number;
begin
  sp_process_default_unli(nRetr, '639178250091', '1', 'USA', '192.168.116.101', '2014/02/03 07:48:25.178', '2014_02_03_07_49_Gi_pid21942_ins1_thread_1_358.csv');
  dbms_output.put_line('nRetr:' || to_char(nRetr));
end;
/

set define ^
insert into mcc_messages values (4, '515', 1, 'Thank you for using Roam UnliSurf! You can now enjoy 24 hrs of unlimited surfing at only P599, with FREE TravelCare insurance for up to 30 days. Please manually select AT&T during your stay, as usage on other networks is charged at P300/MB. For TravelCare info,  dial *143# toll-free.', sysdate);
insert into mcc_messages values (5, '515', 2, 'Hi! We noticed you are incurring regular surfing charges of P300/MB on your current roaming network. You may wish to manually select AT&T instead for unlimited mobile surfing at only P599/day.',  sysdate); 
insert into mcc_messages values (6, '515', 3, 'Your Roam UnliSurf expires at <EXPIRY TIME> today, EST. To continue enjoying unlimited mobile surfing for another 24 hrs at only P599, just stay connected on AT&T. Usage on other operators will incur data roaming charges of P300/MB. Thank you!', sysdate);
set define &
insert into roaming_partners values (1, '2', '515', '+7'); 

col mcc form a6
col mnc form a6
col file_name form a60
col sgsn_ip form a16
select * from default_unli_log;
select * from radcom_downloaded_files;
