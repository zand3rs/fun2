set define ^

create or replace procedure trigger_owner.sp_is_jackhammer_whitelisted (
   p_retr     out number,
   p_msisdn   in varchar2,
   p_mnc      in varchar2,
   p_mcc      in varchar2) is
   nRetr Number;
   nCnt  Number;
begin
   nRetr := 164;
   -- check if mnc is whitelisted
   begin
      select 1
      into  nCnt 
      from  jackhammer_mnc_mapping
      where mnc = p_mnc
      and   mcc = p_mcc
      and   rownum < 2;
   exception
      when no_data_found then
         select count(1)
         into   nCnt 
         from   jackhammer_mnc_mapping;
         -- if no records at all then all MNC's are whitelisted
         if (nCnt = 0) then nCnt := 1; else nCnt := 0; end if;
   end;

   -- check if msisdn is whitelisted
   if nCnt = 1 then
      begin
         select 1
         into  nCnt 
         from  jackhammer_msisdn_mapping
         where p_msisdn between msisdn_fr and msisdn_to
         and   rownum < 2;
      exception
         when no_data_found then
            select count(1)
            into   nCnt 
            from   jackhammer_msisdn_mapping;
            -- if no records at all then all MSISDN's are whitelisted
            if (nCnt = 0) then nCnt := 1; else nCnt := 0; end if;
      end;
   end if;

   if nCnt = 0 then
      nRetr := 1;
   end if;
   p_retr := nRetr;
end sp_is_jackhammer_whitelisted;
/


CREATE OR REPLACE PROCEDURE TRIGGER_OWNER.SP_PROCESS_DEFAULT_UNLI (
  p_retr     out number,
  p_sta_dt   out varchar2,
  p_end_dt   out varchar2,
  p_tran_id  out number,
  p_msisdn   in varchar2,
  p_mnc      in varchar2,
  p_mcc      in varchar2,
  p_sgsn_ip  in varchar2,
  p_date     in varchar2,
  p_filename in varchar2 ) is

  -- 1   - Valid Transaction
  -- 2   - Invalid partner/switch notification
  -- 101 - Invalid MSISDN
  -- 102 - Invalid MNC -- superseeded on new patch, blank mnc are allowed
  -- 103 - Invalid MCC
  -- 104 - Invalid SGSN_IP
  -- 105 - invalid DATE
  -- 106 - invalid FILENAME
  -- 107 - Duplicate / With Active Default Unli
  -- 108 - Unable to insert notification message
  -- 109 - Postpaid filtered - chia filtered
  -- 110 - Invalid TZ
  -- 111 - MLP subscriber
  -- 112 - MLP subscriber no activation message
  -- 113 - Shampoo suppression
  -- 164 - DB_RETR_NF_BUS

  vMsg_1st    Varchar2(2000);
  vMsg_Swi    Varchar2(2000);
  vMcc        Varchar2(30);
  vMnc        Varchar2(30);
  nRetr       Number;
  vPartner    Number;
  vTranDate   Date;
  vTranDateTZ Date;
  nPreExpiry  Number;
  nExpiry     Number;
  dExpiry     Date;
  vUnliFlag   Number;
  nNextId     Number;
  bCatchAll   Boolean := FALSE;
  nMLP        Number;
  nShampoo    Number;
  vNfBus_Msg  Varchar2(2000);
  nNfBus      Number;
begin

  sp_logger('PROCESS_DEFAULT_UNLI' , 'START =>' ||
                                     ' p_msisdn    :' || p_msisdn    ||
                                     ' p_mnc       :' || p_mnc       ||
                                     ' p_mcc       :' || p_mcc       ||
                                     ' p_sgsn_ip   :' || p_sgsn_ip   ||
                                     ' p_date      :' || p_date      ||
                                     ' p_filename  :' || p_filename );


  nRetr := 1;

  if  p_msisdn is null then nRetr := 101;
  elsif substr(p_msisdn,1,2) <> '63' then nRetr := 101;
  elsif length(p_msisdn) <> 12 then nRetr := 101;
  -- elsif p_mnc is null then nRetr := 102;
  -- elsif p_mcc is null then nRetr := 103;
  elsif (p_sgsn_ip is null) or (p_sgsn_ip = '') then nRetr := 104;
  elsif p_date is null then nRetr := 105;
  elsif p_filename is null then nRetr := 106;
  end if;

  if nRetr <> 1 then
     sp_logger('PROCESS_DEFAULT_UNLI' , 'END =>' ||
                                        ' p_msisdn    :' || p_msisdn    ||
                                        ' p_mnc       :' || p_mnc       ||
                                        ' p_mcc       :' || p_mcc       ||
                                        ' nRetr       :' || nRetr );
     return;
  end if;

  begin
     select to_date(substr(p_date, 1, 10) || ' ' || substr(p_date, 12, 8), 'YYYY/MM/DD HH24:MI:SS') into vTranDate from dual;
  exception
     when others then
        nRetr := 105;
  end;

  -- check if chia filtered
  if nRetr = 1 then
     if is_chia_filtered(to_number(p_msisdn)) =  1 then
        nRetr := 109;
     end if;
  end if;

  -- check if Shampoo subscriber
  if nRetr = 1 then
     nShampoo := is_shampoo_filtered(p_msisdn);
     if nShampoo <> 0 then
        nRetr := nShampoo;
     end if;
  end if;

  -- check mnc
  if (p_mcc is null or p_mnc is null) then
     -- catch-all
     bCatchAll := TRUE;
     vMnc := '0';
     vMcc := '0';
     vPartner := 1;
  else
     vMnc := p_mnc;
     vMcc := p_mcc;
  end if;

  -- check if MLP subscriber
  if nRetr = 1 then
     nMLP := is_mlp_filtered(p_msisdn, vTranDate, vMcc, vMnc);
     if nMLP <> 0 then
        nRetr := nMLP;
     end if;
  end if;

  if nRetr = 1 then
     if (vMcc = '0' or vMnc = '0') and (not bCatchAll) then
        vPartner := 0;
        nRetr    := 103;
     else
        begin
           select 1 into vPartner
           from roaming_partners
           where mcc = vMcc
           and   mnc = vMnc;
        exception
           when no_data_found then
              vPartner := 0;
           when invalid_number then
              nRetr := 110;
        end;
     end if;

     -- check mcc
     if vPartner = 1 then
       begin
          select msg
          into vMsg_1st
          from mcc_messages
          where mcc = vMcc
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
          where mcc = vMcc
          and   message_type = '2';
       exception
          when no_data_found then
             nRetr := 103;
       end;
     end if;
  end if;

  if nRetr = 1 then
     if vMsg_Swi is not null then
        UPDATE default_unli_log SET status=0 WHERE msisdn = p_msisdn AND mcc = vMcc AND status >= 1;
        -- UPDATE default_unli_exp SET status=0 WHERE msisdn = p_msisdn AND mcc = vMcc AND status >= 1;
        nRetr := 2;
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
           -- Radcom is UTC+0
           --vTranDateTZ :=  vTranDate + sf_get_unli_tz(8,vTz);
           vTranDateTZ :=  vTranDate + sf_get_unli_tz(8,0);
           select default_unli_seq.nextval into nNextId from dual;
           insert into default_unli_log (
                   id, msisdn, mnc, mcc, sgsn_ip, tran_dt, tran_tm, dt_created,
                   status, file_name, expiry_date)
           values (nNextId, p_msisdn, vMnc, vMcc, p_sgsn_ip,
                   trunc(vTranDateTZ), vTranDateTZ, sysdate,
                   1, p_filename, vTranDateTZ+nExpiry );

           insert into default_unli_exp (
                   id, msisdn, tran_dt, tran_tm, mcc, mnc, status, original_dt,
                   pre_expiry_dt, pre_expiry_tm, expiry_dt, expiry_tm )
           values (nNextId, p_msisdn, trunc(vTranDateTZ), vTranDateTZ, vMcc, vMnc, 1, vTranDate,
                   trunc(vTranDateTZ+nPreExpiry), vTranDateTZ+nPreExpiry,
                   trunc(vTranDateTZ+nExpiry), vTranDateTZ+nExpiry);
        exception
           when dup_val_on_index then
              SELECT expiry_date, status, id
              INTO   dExpiry, vUnliFlag, nNextId
              FROM   default_unli_log
              WHERE  msisdn = p_msisdn
              AND    mcc = vMcc;
              if (dExpiry < vTranDateTZ) then
                 UPDATE default_unli_log
                 SET    mnc = vMnc,
                        sgsn_ip = p_sgsn_ip,
                        tran_dt = trunc(vTranDateTZ),
                        tran_tm = vTranDateTZ,
                        expiry_date = vTranDateTZ+nExpiry,
                        status=1,
                        file_name = p_filename
                 WHERE  msisdn = p_msisdn
                 AND    mcc = vMcc;

                 UPDATE default_unli_exp
                 SET    mnc = vMnc,
                        tran_dt = trunc(vTranDateTZ),
                        tran_tm = vTranDateTZ,
                        pre_expiry_dt = trunc(vTranDateTZ+nPreExpiry),
                        pre_expiry_tm = vTranDateTZ+nPreExpiry,
                        expiry_dt = trunc(vTranDateTZ+nExpiry),
                        expiry_tm = vTranDateTZ+nExpiry,
                        original_dt = vTranDate,
                        status=1
                 WHERE  msisdn = p_msisdn
                 AND    mcc = vMcc;
                 nRetr := 1;
              elsif (vUnliFlag=0) then
                 UPDATE default_unli_log SET status=1 WHERE msisdn = p_msisdn AND mcc = vMcc;
                 -- UPDATE default_unli_exp SET status=1 WHERE msisdn = p_msisdn AND mcc = vMcc;
                 -- no notification should be received as per email dated Apr 25 with subject Notification Issue
                 -- nRetr := 1;
                 nRetr := 107;
              else
                 nRetr := 107;
              end if;
        end;
        if (vMsg_1st is not null) and (nRetr = 1) then
           sp_is_jackhammer_whitelisted(nNfBus, p_msisdn, p_mnc, p_mcc);
           if (nvl(nNfBus,0) = 164) then
              begin
                 select msg_1
                 into   vNfBus_Msg
                 from   keyword_msg
                 where  message_id='BROADCAST_ROAM_FREE_CT';
              exception
                 when no_data_found then vNfBus_Msg := null;
              end;
           end if;
           -- send SMS notification
           if vNfBus_Msg is not null then
              begin
                 insert into broadcast_log (id, msisdn, message_id, tx_date, status, cluster_node, msg)
                 values (broadcast_log_seq.nextval, p_msisdn, '1', sysdate, 0, 1, vNfBus_Msg);
              exception
                 when dup_val_on_index then nRetr := 108;
              end;
           else
              begin
                 insert into broadcast_log (id, msisdn, message_id, tx_date, status, cluster_node, msg)
                 values (broadcast_log_seq.nextval, p_msisdn, '1', sysdate, 0, 1, vMsg_1st);
              exception
                 when dup_val_on_index then nRetr := 108;
              end;
           end if;
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

  if (nvl(nRetr,0) = 1) and (nvl(nNfBus,0) = 164) then
     nRetr := nNfBus;
  end if;
  sp_logger('PROCESS_DEFAULT_UNLI' , 'END =>' ||
                                     ' p_msisdn    :' || p_msisdn    ||
                                     ' p_mnc       :' || p_mnc       ||
                                     ' p_mcc       :' || p_mcc       ||
                                     ' nRetr       :' || nRetr );

  -- set return parameters
  p_sta_dt := to_char(vTranDate, 'YYYY-MM-DD"T"HH24:MI:SS".000+0800"');
  p_end_dt := to_char(vTranDate+nExpiry, 'YYYY-MM-DD"T"HH24:MI:SS".000+0800"');
  p_tran_id := nvl(nNextId,0);
  p_retr := nvl(nRetr,0);

end sp_process_default_unli;
/

set define &
