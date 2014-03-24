drop table default_unli_log;
create table default_unli_log (
  id number(11) not null,
  msisdn varchar(30) not null,
  tran_dt date not null,
  tran_tm date not null,
  mnc varchar2(30) not null,
  mcc varchar2(60) not null,
  sgsn_ip varchar2(30) not null,
  status number(1) default 0 not null,
  expiry_date date not null,
  dt_created date not null,
  file_name varchar2(100) not null,
  CONSTRAINT default_unli_log_pk primary key (id),
  CONSTRAINT default_unli_log_uk unique (msisdn)
);

drop table default_unli_exp;
create table default_unli_exp (
  id number not null,
  msisdn varchar(12) not null,
  tran_dt date not null,
  tran_tm date not null,
  mcc varchar2(30) not null,
  mnc varchar2(30) not null,
  status number default 0 not null,
  original_dt date not null,
  pre_expiry_dt date not null,
  pre_expiry_tm date not null,
  expiry_dt date not null,
  expiry_tm date not null,
  CONSTRAINT default_unli_exp_pk primary key (id),
  CONSTRAINT default_unli_exp_uk unique (msisdn)
);
create index default_unli_pre_idx on default_unli_exp(pre_expiry_dt);
create index default_unli_exp_idx on default_unli_exp(expiry_dt);
create index default_unli_trx_idx on default_unli_exp(tran_dt);

drop table default_unli_hist;
create table default_unli_hist (
  id number not null,
  msisdn varchar(12) not null,
  tran_dt date not null,
  tran_tm date not null,
  mnc varchar2(30) not null,
  mcc varchar2(30) not null,
  sgsn_ip varchar2(30) not null,
  status number default 0 not null,
  pre_expiry_dt date not null,
  pre_expiry_tm date not null,
  expiry_dt date not null,
  expiry_tm date not null,
  original_dt date not null,
  dt_created date not null,
  dt_deleted date not null,
  file_name varchar2(100) not null,
  CONSTRAINT default_unli_hist_pk primary key (id)
);
create index default_unli_hist_msisdn_idx on default_unli_hist(msisdn);
create index default_unli_hist_trx_idx on default_unli_hist(tran_dt);

drop table default_unli_params;
create table default_unli_params (
  param_id varchar(30) not null,
  param_char_value varchar2(60),
  param_num1_value number,
  param_num2_value number,
  param_description varchar2(60),
  CONSTRAINT default_unli_params_pk primary key (param_id)
);

delete from default_unli_params;
insert into default_unli_params values ('UNLI_PRE_EXP', null, (3/60/24), (5/60/24), 'Set number of hours for pre expiry time and for expiry time.'); 
commit;

create table roaming_partners (    
  id number not null enable,
  mnc varchar2(30) not null,
  mcc varchar2(60) not null,
  tz  varchar2(30) not null,
  dt_created date not null,
  constraint roaming_partners_pk primary key (id),
  constraint roaming_partners_uk unique (mnc, mcc)
);

create table mcc_messages (    
  id number not null enable,
  mcc varchar2(60) not null,
  message_type varchar2(30) not null,
  msg varchar2(2000) not null,
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
  -- 107 - Duplicate / With Active Default Unli
  -- 108 - Unable to insert notification message
  -- 109 - Invalid TZ
  
  vMsg_1st    Varchar2(2000);
  vMsg_Swi    Varchar2(2000);
  nRetr       Number;
  vPartner    Number; 
  vTranDate   Date;
  vTranDateTZ Date;
  nPreExpiry  Number;
  nExpiry     Number;
  dExpiry     Date;
  vUnliFlag   Number;
  vTZ         Number;
  nNextId     Number; 
begin
  
  nRetr := 1;

  if  p_msisdn is null then nRetr := 101;
  elsif  substr(p_msisdn,1,2) <> '63' then nRetr := 101;
  elsif  length(p_msisdn) <> 12 then nRetr := 101;
  elsif p_mnc is null then nRetr := 102;
  elsif p_mcc is null then nRetr := 103;
  elsif p_sgsn_ip is null then nRetr := 104;
  elsif p_date is null then nRetr := 105;
  elsif p_filename is null then nRetr := 106;
  end if;

  begin
     select to_date(substr(p_date, 1, 10) || ' ' || substr(p_date, 12, 8), 'YYYY/MM/DD HH24:MI:SS') into vTranDate from dual;
  exception 
     when others then 
        nRetr := 105;
  end;

  -- check mnc
  begin
     select 1, to_number(tz) into vPartner, vTZ
     from roaming_partners
     where mcc = p_mcc
     and   mnc = p_mnc;
  exception 
     when no_data_found then 
        vPartner := 0;
     when invalid_number then 
        nRetr := 109;
  end;

  -- check mcc
  if vPartner = 1 then 
    begin
       select msg
       into vMsg_1st
       from mcc_messages
       where mcc = p_mcc
       and   message_type = '1';
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
       and   message_type = '2';
    exception 
       when no_data_found then 
          nRetr := 103;
    end;
  end if;

  if nRetr = 1 then
     if vMsg_Swi is not null then
        UPDATE default_unli_log SET status=0 WHERE msisdn = p_msisdn AND status = 1;
        UPDATE default_unli_exp SET status=0 WHERE msisdn = p_msisdn AND status = 1;
        begin
           insert into broadcast_log (id, msisdn, message_id, tx_date, status, cluster_node, msg)
           values (broadcast_log_seq.nextval, p_msisdn, '2', vTranDate, 0, 1, vMsg_Swi);
        exception 
           when dup_val_on_index then 
              nRetr := 108;
        end;
     else
        begin
           select param_num1_value, param_num2_value 
           into   nPreExpiry, nExpiry
           from   default_unli_params
           where  param_id = 'UNLI_PRE_EXP';
        exception 
           when others then 
              nPreExpiry := (11.5/24);
              nExpiry := (12/24);
        end;
        begin
           vTranDateTZ :=  vTranDate + sf_get_unli_tz(8,vTz);
           select default_unli_seq.nextval into nNextId from dual;
           insert into default_unli_log (
                   id, msisdn, mnc, mcc, sgsn_ip, tran_dt, tran_tm, dt_created, 
                   status, file_name, expiry_date)
           values (nNextId, p_msisdn, p_mnc, p_mcc, p_sgsn_ip, 
                   trunc(vTranDateTZ), vTranDateTZ, sysdate, 
                   1, p_filename, vTranDateTZ+nExpiry );

           insert into default_unli_exp ( 
                   id, msisdn, tran_dt, tran_tm, mcc, mnc, status, original_dt, 
                   pre_expiry_dt, pre_expiry_tm, expiry_dt, expiry_tm )
           values (nNextId, p_msisdn, trunc(vTranDateTZ), vTranDateTZ, p_mcc, p_mnc, 1, vTranDate, 
                   trunc(vTranDateTZ+nPreExpiry), vTranDateTZ+nPreExpiry, 
                   trunc(vTranDateTZ+nExpiry), vTranDateTZ+nExpiry);
        exception 
           when dup_val_on_index then 
              SELECT expiry_date, status, id
              INTO   dExpiry, vUnliFlag, nNextId
              FROM   default_unli_log
              WHERE  msisdn = p_msisdn;
              if (dExpiry < vTranDateTZ) then
                 UPDATE default_unli_log
                 SET    mcc = p_mcc,
                        mnc = p_mcc,
                        sgsn_ip = p_sgsn_ip,
                        tran_dt = trunc(vTranDateTZ),
                        tran_tm = vTranDateTZ, 
                        expiry_date = vTranDateTZ+nExpiry, 
                        status=1,
                        file_name = p_filename
                 WHERE  msisdn = p_msisdn;

                 UPDATE default_unli_exp
                 SET    mcc = p_mcc, 
                        mnc = p_mnc, 
                        tran_dt = trunc(vTranDateTZ),
                        tran_tm = vTranDateTZ,
                        pre_expiry_dt = trunc(vTranDateTZ+nPreExpiry), 
                        pre_expiry_tm = vTranDateTZ+nPreExpiry, 
                        expiry_dt = trunc(vTranDateTZ+nExpiry), 
                        expiry_tm = vTranDateTZ+nExpiry, 
                        original_dt = vTranDate, 
                        status=1
                 WHERE  msisdn = p_msisdn;

                 nRetr := 1;
              elsif (vUnliFlag=0) then
                 UPDATE default_unli_log SET status=1 WHERE msisdn = p_msisdn;
                 UPDATE default_unli_exp SET status=1 WHERE msisdn = p_msisdn;
                 nRetr := 1;
              else
                 nRetr := 107;
              end if;
        end;
        if (vMsg_1st is not null) and (nRetr = 1) then
           begin
              insert into broadcast_log (id, msisdn, message_id, tx_date, status, cluster_node, msg)
              values (broadcast_log_seq.nextval, p_msisdn, '1', sysdate, 0, 1, vMsg_1st);
           exception 
              when dup_val_on_index then 
                 nRetr := 108;
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
  dFormat  Date;
begin
  nRetr := 1;

  if substr (p_filename, 18, 2) <> 'Gi' then 
     nRetr := 110;
  else
     begin
        select to_date(substr(p_filename,1,16),'YYYY_MM_DD_HH24_MI')
        into   dFormat
        from   dual;

        if dFormat > sysdate then
           nRetr := 113;
        end if;
     exception 
        when others then 
           nRetr := 111;
     end;
  end if;

  if nRetr = 1 then
     begin
        select 1
        into   nCheck
        from   radcom_downloaded_files
        where  file_name = p_filename;
        nRetr := 112;
     exception 
        when no_data_found then null;
     end;
  end if;
  -- set return parameters
  p_retr     := nRetr;
  --p_filename := to_char(trunc(sysdate), 'YYYY_MM_DD_HH24_MI"_Gi*"');
end sp_get_radcom_file_format;
/

create or replace procedure sp_generate_unli_notifications is
  vMsg_Pre Varchar2(2000);
  vMsg_Exp Varchar2(2000);
  vPreCounter   Number := 0;
  vNoPreCounter Number := 0;
  vExpCounter   Number := 0;
  vNoExpCounter Number := 0;
begin
   -- generate pre expiry notifications
   for i in (select msisdn, original_dt+(expiry_tm-tran_tm) expiry_tm, mcc
             from   default_unli_exp
             where  status = 1
             and    pre_expiry_dt >= trunc(sysdate)-1  
             and    pre_expiry_tm < sysdate  
             and    expiry_tm > sysdate)
   loop
      begin
         select msg
         into   vMsg_Pre
         from   mcc_messages
         where  mcc = i.mcc
         and    message_type = '3';
         begin
            insert into broadcast_log ( id, msisdn, message_id, tx_date, status, cluster_node, msg )
            values (broadcast_log_seq.nextval, i.msisdn, '3', trunc(sysdate), 0, 1, replace(vMsg_Pre, '<EXPIRY TIME>', to_char(i.expiry_tm, 'HH24:MI:SS')) );
            update default_unli_exp set status=2 where msisdn=i.msisdn;
            commit;
            vPreCounter := vPreCounter + 1;
         exception
            when dup_val_on_index then null;
         end;
      exception 
         when no_data_found then 
            vMsg_Pre := null;
            vNoPreCounter := vNoPreCounter + 1;
      end;
   end loop;

   -- generate pre expiry notifications
   for i in (select msisdn, mcc
             from   default_unli_exp
             where  status > 0
             and    expiry_dt >= trunc(sysdate)-1 
             and    expiry_tm < sysdate)
   loop
      begin
         select msg
         into   vMsg_Exp
         from   mcc_messages
         where  mcc = i.mcc
         and    message_type = '4';
         begin
            -- create notifications
            insert into broadcast_log ( id, msisdn, message_id, tx_date, status, cluster_node, msg )
            values (broadcast_log_seq.nextval, i.msisdn, '4', trunc(sysdate), 0, 1, vMsg_Exp );
            vExpCounter := vExpCounter + 1;
         exception
            when dup_val_on_index then 
               vMsg_Pre := null;
         end;
      exception 
         when no_data_found then 
            vMsg_Exp := null;
            vNoExpCounter := vNoExpCounter + 1;
      end;
      -- create history log
      insert into default_unli_hist 
            (id, msisdn, tran_dt, tran_tm, mnc, mcc, sgsn_ip, status, pre_expiry_dt, pre_expiry_tm, 
             expiry_dt, expiry_tm, original_dt, file_name, dt_created, dt_deleted)
      select a.id, a.msisdn, a.tran_dt, a.tran_tm, a.mnc, a.mcc, a.sgsn_ip, a.status, b.pre_expiry_dt, b.pre_expiry_tm, 
             b.expiry_dt, b.expiry_tm, b.original_dt, a.file_name, a.dt_created, sysdate 
      from   default_unli_log  a, default_unli_exp b
      where a.msisdn=i.msisdn
      and   a.msisdn=b.msisdn;
      -- clear transaction log
      delete from default_unli_log where msisdn=i.msisdn;
      delete from default_unli_exp where msisdn=i.msisdn;
      commit;
   end loop;

   delete from default_unli_log where status = 0 and expiry_date = trunc(sysdate)-1;
   delete from default_unli_exp where status = 0 and expiry_dt = trunc(sysdate)-1;
   commit;
   dbms_output.put_line (chr(10));
   dbms_output.put_line (to_char(sysdate, 'YYYY-MM-DD HH24:MI:SS'));
   dbms_output.put_line ('No. of Pre Expiry Message Sent:' || to_char(vPreCounter));
   dbms_output.put_line ('No. of Pre Expiry with No Message to Sent:' || to_char(vNoPreCounter));
   dbms_output.put_line ('No. of Expiry Message Sent:' || to_char(vExpCounter));
   dbms_output.put_line ('No. of Expiry with No Message to Sent:' || to_char(vNoExpCounter));
end sp_generate_unli_notifications;
/


create or replace function sf_get_unli_tz (p_tz_1 in number, p_tz_2 in number) return number as
  nHours Number;
begin
   if p_tz_2 > p_tz_1 then
      nHours := (p_tz_2 - p_tz_1);
   else
      if p_tz_2 < 0 then
         nHours := (abs(p_tz_2) + p_tz_1) * -1;
      else
         nHours := (p_tz_1 - p_tz_2) * -1;
      end if;
   end if;
   return (nHours/24);
end sf_get_unli_tz;
/

