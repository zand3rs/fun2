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

create table broadcast_log (    
  id number not null enable,
  msisdn number(12,0) not null enable,
  message_id varchar2(60),
  tx_date date not null enable,
  status number(1,0) default 0 not null enable,
  cluster_node number(3,0) default 1 not null enable,
  msg varchar2(2000),
  constraint broadcast_log_pk primary key (id)
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

CREATE SEQUENCE  DEFAULT_UNLI_SEQ  MINVALUE 1 MAXVALUE 999999999999999999999999999 INCREMENT BY 1 START WITH 222 NOCACHE  ORDER  NOCYCLE;
CREATE SEQUENCE  RADCOM_DOWNLOADED_FILES_SEQ  MINVALUE 1 MAXVALUE 999999999999999999999999999 INCREMENT BY 1 START WITH 222 NOCACHE  ORDER  NOCYCLE;

CREATE SEQUENCE  BROADCAST_LOG_SEQ  MINVALUE 1 MAXVALUE 999999999999999999999999999 INCREMENT BY 1 START WITH 222 NOCACHE  ORDER  NOCYCLE;

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
     select to_date(p_date, 'MM/DD/YYYY HH24:MI:SS') into vTranDate from dual;
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
       select msg into vMsg
       from mcc_messages
       where mcc = p_mcc
       and   message_type = 1;
    exception 
       when no_data_found then 
          nRetr := 103;
    end;
  else
    begin
       select msg into vMsg
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
        insert into broadcast_log (id, msisdn, tx_date, status, cluster_node, msg)
        values (broadcast_log_seq.nextval, p_msisdn, vTranDate, 0, 1, vMsg);
     exception 
        when dup_val_on_index then 
           nRetr := 107;
     end;
     commit;
  end if;

  p_retr := nRetr;

end sp_process_default_unli;
/
show err


create or replace procedure sp_process_default_unli_files (
  p_retr out number,
  p_no_valid in number,
  p_no_error in number,
  p_filename in varchar2 ) is
  --   1 - Success
  -- 101 - Duplicate file uploaded
  nRetr    Number;
begin
  nRetr := 1;
  begin
     insert into radcom_downloaded_files (id, file_name, no_valid, no_errors, dt_uploaded)
     values (radcom_downloaded_files_seq.nextval, p_filename, p_no_valid, p_no_error, sysdate);
  exception 
     when dup_val_on_index then 
        nRetr := 101;
  end;
  p_retr := nRetr;
end sp_process_default_unli_files;
/
