set define ^

CREATE OR REPLACE PROCEDURE "TRIGGER_OWNER"."SP_LOGGER" (p_source in varchar2, p_info in varchar2) is
   nCtr Number := 0;
begin
   if p_source in ('PROCESS_SHAMPOO','PROCESS_MLP') then
      if p_source = 'PROCESS_MLP' and p_info like '%RemovedServices%' then
         nCtr := 1;
      else
         insert into system_log (source, info, dt_created, id )
         values (p_source, p_info, sysdate, system_log_seq.nextval);
         commit;
      end if;
   end if;
end;
/

CREATE OR REPLACE FUNCTION trigger_owner.is_default_unli_filtered (p_msisdn in varchar2) return number is
  -- returns 0 no default unli
  -- returns 114 with default unli
  nDefaultUnli Number;
  nRetr Number;
begin
   nRetr := 114;

   select 1
   into   nDefaultUnli
   from   default_unli_log
   where  msisdn = p_msisdn
   and    rownum <= 1;

   return nRetr;
exception
   when no_data_found then return 0;
   when others then return 0;
end is_default_unli_filtered;
/
show err

CREATE OR REPLACE FUNCTION trigger_owner.is_greencard_filtered (p_msisdn in varchar2) return number is
  -- returns 0 no greencard
  -- returns 115 with greencard
  nGreencard Number;
  nRetr Number;
begin
   nRetr := 114;

   select 1
   into   nGreencard
   from   conditioner_log
   where  msisdn = p_msisdn
   and    brand = 'POSTPAID'
   and    tran_type = 20
   and    status = 2
   and    rownum <= 1;

   return nRetr;
exception
   when no_data_found then return 0;
   when others then return 0;
end is_greencard_filtered;
/
show err


CREATE OR REPLACE FUNCTION trigger_owner.is_mlp_active (p_msisdn in number) return number is
  -- returns 0 no mlp subscription
  -- returns 111 with mlp subscription
  nMLP Number;
  nRetr Number;
begin
   nRetr := 111;
   select 1
   into   nMLP
   from   mlp_log a, mlp_services b
   where  a.msisdn = p_msisdn
   and    a.start_date < sysdate
   and    rownum < 2;
   return nRetr;
exception
   when no_data_found then return 0;
   when others then return 0;
end is_mlp_active;
/
show err

CREATE OR REPLACE FUNCTION trigger_owner.sf_greencard_check_coex (p_msisdn in varchar2) return number is
  -- returns 1 successful
  -- retr: 111 with mlp subscription
  --       113 with shampoo supression
  --       114 with default unli
  --       115 with greencard
  nRetr Number;
begin
   nRetr  := is_default_unli_filtered(p_msisdn);
   if nRetr = 0 then
      nRetr := is_shampoo_filtered(p_msisdn);
      if nRetr = 0 then
         nRetr := is_mlp_active(p_msisdn);
         if nRetr = 0 then
            nRetr := is_greencard_filtered(p_msisdn);
            if nRetr = 0 then
               nRetr := 1;
            end if;
         end if;
      end if;
   end if;
   return nRetr;
end sf_greencard_check_coex;
/
show err


CREATE OR REPLACE FUNCTION trigger_owner.sf_mlp_check_coex (p_msisdn in varchar2) return number is
  -- returns 1 successful
  -- retr: 113 with shampoo supression
  --       114 with default unli
  --       115 with greencard
  nRetr Number;
begin
   nRetr  := is_default_unli_filtered(p_msisdn);
   if nRetr = 0 then
      nRetr := is_shampoo_filtered(p_msisdn);
      if nRetr = 0 then
         nRetr := is_greencard_filtered(p_msisdn);
         if nRetr = 0 then
            nRetr := 1;
         end if;
      end if;
   end if;
   return nRetr;
end sf_mlp_check_coex;
/
show err

CREATE OR REPLACE FUNCTION trigger_owner.sf_shampoo_check_coex (p_msisdn in varchar2) return number is
  -- returns 1 successful
  -- retr: 111 with mlp subscription
  --       113 with shampoo supression
  --       114 with default unli
  --       115 with greencard
  nRetr Number;
begin
   nRetr  := is_default_unli_filtered(p_msisdn);
   if nRetr = 0 then
      nRetr := is_mlp_active(p_msisdn);
      if nRetr = 0 then
         nRetr := is_greencard_filtered(p_msisdn);
         if nRetr = 0 then
            nRetr := 1;
         end if;
      end if;
   end if;
   return nRetr;
end sf_shampoo_check_coex;
/
show err


CREATE OR REPLACE PROCEDURE trigger_owner.sp_process_greencard (
    p_trantype      in  number,
    p_msisdn        in  varchar2,
    p_req_id        in  number,
    p_promo_code    in  varchar2,
    p_promo_value   in  varchar2,
    p_roamer_status in  number,
    p_brand         in  varchar2
   ) is
   -- Bluemoon
   vPromoName           conditioner_promos.promo_name%type;
   vNF_ServiceID        conditioner_promos.service_id%type;
begin
   begin
      sp_get_usurf_sid (vPromoName, vNF_ServiceID, p_promo_code, p_promo_value, p_brand);
      insert into conditioner_log (id, tran_type, cluster_node, msisdn, promo_code, promo_value, promo_name, service_id, status, brand, request_id, dt_created, created_by)
      values (conditioner_log_seq.nextval, p_trantype, 1, p_msisdn, p_promo_code, p_promo_value, vPromoName, vNF_ServiceID,  p_roamer_status, p_brand, p_req_id, sysdate, user);
   exception
      when dup_val_on_index then
         if p_trantype in (21,22) then
            update conditioner_log
            set    status = 0, request_id = p_req_id
            where  tran_type = p_trantype
            and    msisdn = p_msisdn
            and    promo_code = p_promo_code
            and    promo_value = p_promo_value;
         else
            update conditioner_log
            set    status = 0, request_id = p_req_id
            where  tran_type = p_trantype
            and    msisdn = p_msisdn
            and    request_id = p_req_id;
         end if;
   end;
   -- tag subscription as expired
   if p_trantype = 21 then
      update conditioner_log set status=4, remarks='voyager' where msisdn = p_msisdn and promo_code = p_promo_code and promo_value = p_promo_value and tran_type = 20;
   end if;
   commit;
end sp_process_greencard;
/

CREATE OR REPLACE PROCEDURE trigger_owner.sp_greencard_coex_notif (p_msisdn in varchar2, p_keyword in varchar2, p_sub_keyword in varchar2) is
   vMessageId keyword_msg.message_id%type;
   vMessage   keyword_msg.msg_1%type;
BEGIN
   if p_sub_keyword = '3D' then
      vMessageId := 'VROAM_SURF3D_ON_INVALID_COEX';
   elsif p_sub_keyword = '5D' then
      vMessageId := 'VROAM_SURF5D_ON_INVALID_COEX';
   elsif p_sub_keyword = '7D' then
      vMessageId := 'VROAM_SURF7D_ON_INVALID_COEX';
   elsif p_sub_keyword = '15D' then
      vMessageId := 'VROAM_SURF15D_ON_INVALID_COEX';
   elsif p_sub_keyword = '30D' then
      vMessageId := 'VROAM_SURF30D_ON_INVALID_COEX';
   end if;

   begin
      select msg_1 
      into   vMessage 
      from   keyword_msg 
      where  message_id=vMessageId;
   exception
      when no_data_found then null;
   end;

   if vMessage is not null then
        begin
           insert into broadcast_log (id, msisdn, message_id, tx_date, status, cluster_node, msg, brand, dt_created)
           values (broadcast_log_seq.nextval, p_msisdn, vMessageId, trunc(sysdate), 0, 1, vMessage, 'PREPAID', sysdate);
        exception 
           when dup_val_on_index then null;
        end;
   end if;
END sp_greencard_coex_notif;
/
show err


CREATE OR REPLACE PROCEDURE TRIGGER_OWNER.SP_PROCESS_MLP (
  p_retr out number,
  p_msisdn in varchar2,
  p_transaction_code in varchar2,
  p_transaction_id in varchar2,
  p_bill_cycle in varchar2,
  p_type in varchar2,
  p_soc in varchar2,
  p_effdate in varchar2 ) is

  nRetr         number;
  nUdrDays      mlp_services.num_days%type;
  vServiceCyle  mlp_services.service_cycle%type;
  vMsg_Start    mlp_messages.message%type;
  vPromoName    conditioner_promos.promo_name%type;
  vPromoCode    conditioner_promos.promo_code%type;
  vPromoValue   conditioner_promos.promo_value%type;
  nNextId       number;
  dEffDate      date;
  dExpiryNotif  date;
  nGreencard    number := 0;
begin

   sp_logger('PROCESS_MLP' , 'START =>'              ||
                             ' p_msisdn:'            || p_msisdn ||
                             ' p_transaction_code:'  || p_transaction_code ||
                             ' p_transaction_id:'    || p_transaction_id ||
                             ' p_bill_cycle:'        || p_bill_cycle ||
                             ' p_type:'              || p_type ||
                             ' p_soc:'               || p_soc ||
                             ' p_effdate:'           || p_effdate);

   nRetr := 1;

   begin
      select to_date(replace(p_effdate,'T',' '), 'YYYY-MM-DD HH24:MI:SS') into dEffDate from dual;
   exception
     when others then
         nRetr := 104;
   end;

   if p_type IN ('AddedServices', 'Services') then
      if nRetr = 1 then
         -- get udr days, service cycle
         begin
            select num_days, service_cycle
            into   nUdrDays, vServiceCyle
            from   mlp_services
            where  soc_id = p_soc;
         exception
            when no_data_found then
               nRetr := 102;
         end;
      end if;

      if nRetr = 1 then
         nRetr := sf_mlp_check_coex (p_msisdn); 
         if nRetr = 1 then
            begin
               select mlp_seq.nextval into nNextId from dual;
               insert into mlp_log (
                       id, msisdn, soc_id, service_cycle, start_date, expiry_date, num_days, last_usage_dt, last_usage_tm,
                       usage_expiry, cycle_no, transaction_id, transaction_code, remaining_days, dt_created, created_by)
               values (nNextId, p_msisdn, p_soc, vServiceCyle, dEffDate, null, nUdrDays, null, null,
                       null, p_bill_cycle, p_transaction_id, p_transaction_code, nUdrDays, sysdate, user );
               nRetr := 1;
            exception
               when dup_val_on_index then
                  nRetr := 101;
            end;
         end if;
      else
         nRetr := sf_greencard_check_coex(p_msisdn);
         if nRetr = 1 then 
            begin
               sp_get_usurf_promo_kw (vPromoName, vPromoCode, vPromoValue, p_soc, 'POSTPAID');
               if vPromoCode is not null then
                  insert into conditioner_log (id, tran_type, cluster_node, msisdn, promo_code, promo_value, promo_name, service_id, status, brand, request_id, dt_activated, dt_notif, src, dt_created, created_by)
                  values (conditioner_log_seq.nextval, '20', 1, p_msisdn, vPromoCode, vPromoValue, vPromoName, p_soc, 2, 'POSTPAID', 0, dEffDate, to_date(to_char(sysdate, 'YYYYMMDD') || ' '|| to_char(dEffDate,'HH24MISS'), 'YYYYMMDD HH24MISS')+1, 'CRM', sysdate, user);
                  if sf_greencard_crm_reg_notif (p_msisdn, vPromoCode, vPromoValue, dEffDate) = 1 then
                     nRetr := 1;
                  else
                     rollback;
                  end if;
              end if;
            exception
               when dup_val_on_index then 
                  nRetr := 101;
            end;
         else
            sp_greencard_coex_notif(p_msisdn, vPromoCode, vPromoValue);
            nRetr := 163;
         end if;
      end if;
   elsif p_type = 'RemovedServices' then
      if nRetr = 1 then
         sp_greencard_expire_plan(nGreencard, p_msisdn,p_soc);
         update mlp_log set expiry_date = dEffDate where msisdn=p_msisdn and soc_id = p_soc returning expiry_notif into dExpiryNotif;
         if sql%notfound then
            if nGreencard = 0 then
               nRetr := 109;
            end if;
         else
            if (dExpiryNotif is not null) and (dExpiryNotif > dEffDate) then
               insert into mlp_log_history
                      ( id, msisdn, soc_id, service_cycle, start_date, expiry_date, num_days, cycle_no, transaction_id, transaction_code, mnc, mcc, usage_expiry,
                       last_usage_dt, last_usage_tm, remaining_days, pre_expiry_notif, expiry_notif, status, dt_created, created_by, dt_deleted, deleted_by )
               select id, msisdn, soc_id, service_cycle, start_date, expiry_date, num_days, cycle_no, transaction_id, transaction_code, mnc, mcc, usage_expiry,
                      last_usage_dt, last_usage_tm, remaining_days, pre_expiry_notif, expiry_notif, status, dt_created, created_by, sysdate, user
               from   mlp_log
               where  msisdn=p_msisdn and soc_id=p_soc;
               delete from  mlp_log where msisdn=p_msisdn and soc_id=p_soc;
            end if;
         end if;
      end if;
   else
      nRetr := 110;
   end if;
   sp_logger('PROCESS_MLP' , 'END => nRetr:' || nRetr ||
                             ' p_msisdn:'    || p_msisdn ||
                             ' p_type:'      || p_type );
   p_retr := nRetr;
   commit;
end sp_process_mlp;
/
show err


CREATE OR REPLACE PROCEDURE TRIGGER_OWNER.SP_PROCESS_SHAMPOO (
  p_retr out number,
  p_type in varchar2,
  p_msisdn in varchar2,
  p_plan in varchar2,
  p_start in varchar2,
  p_end in varchar2 ) is
  -- Possible Retr
  -- 1 - successfull
  -- 100 - invalid msisdn
  -- 101 - invalid plan
  -- 102 - invalid start date
  -- 103 - invalid end date
  -- 104 - already subscribe
  -- 110 - no subscription
  -- 111 - coexists
  nRetr Number;
  dStart  Date;
  dEnd    Date;

begin

   sp_logger('PROCESS_SHAMPOO' , 'START =>' ||
                             ' p_type:'     || p_type ||
                             ' p_msisdn:'   || p_msisdn ||
                             ' p_plan:'     || p_plan ||
                             ' p_start:'    || p_start ||
                             ' p_end:'      || p_end );

   nRetr := 1;

   if p_type = 'SUBSCRIBE' then
      if (p_msisdn is null) then
         nRetr := 100;
      elsif (p_msisdn not like '63%') then
         nRetr := 100;
      elsif (length(p_msisdn) <> 12) then
         nRetr := 100;
      end if;

      if (nRetr=1) and (p_plan is null) then
         nRetr := 101;
      end if;

      if (nRetr=1) and (p_start is not null) then
         begin
            select to_date(p_start,'YYYYMMDDHH24MISS')
            into   dStart
            from   dual;
         exception
            when others then
               nRetr := 102;
         end;
      end if;

      if (nRetr=1) and (p_end is not null) then
         begin
            select to_date(p_end,'YYYYMMDDHH24MISS')
            into   dEnd
            from   dual;
         exception
            when others then
               nRetr := 103;
         end;
      end if;

      if (nRetr=1) then
         nRetr := sf_shampoo_check_coex(p_msisdn); 
         if nRetr = 1 then
            begin
               insert into shampoo_log (
                       id, msisdn, plan, start_date, end_date, dt_created, created_by)
               values (shampoo_seq.nextval, p_msisdn, p_plan, dStart, dEnd, sysdate, user );
               commit;
               nRetr := 1;
            exception
               when dup_val_on_index then nRetr := 104;
            end;
         else
            nRetr := 111;
         end if;
      end if;
   elsif p_type = 'UNSUBSCRIBE' then
      delete from shampoo_log where msisdn = p_msisdn and plan = p_plan;
      if sql%notfound then
         nRetr := 110;
      end if;
      commit;
   end if;
   p_retr := nRetr;
   sp_logger('PROCESS_SHAMPOO' , 'END => nRetr:' || nRetr || ' p_retr:' || p_retr);
end sp_process_shampoo;
/
show err


CREATE OR REPLACE PROCEDURE TRIGGER_OWNER.SP_PROCESS_DEFAULT_UNLI (
  p_retr     out number,
  p_sta_dt   out varchar2,
  p_end_dt   out varchar2,
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
  -- 115 - Greencard suppression

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
  nGreencard  Number;
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
     nGreencard := is_greencard_filtered(p_msisdn);
     if nGreencard <> 0 then
        nRetr := nGreencard;
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

  sp_logger('PROCESS_DEFAULT_UNLI' , 'END =>' ||
                                     ' p_msisdn    :' || p_msisdn    ||
                                     ' p_mnc       :' || p_mnc       ||
                                     ' p_mcc       :' || p_mcc       ||
                                     ' nRetr       :' || nRetr );

  -- set return parameters
  p_sta_dt := to_char(vTranDate, 'YYYY-MM-DD"T"HH24:MI:SS".000+0800"');
  p_end_dt := to_char(vTranDate+nExpiry, 'YYYY-MM-DD"T"HH24:MI:SS".000+0800"');
  p_retr := nvl(nRetr,0);

end sp_process_default_unli;
/
show err


CREATE OR REPLACE PROCEDURE TRIGGER_OWNER.SP_INIT_TRAN (
    p_retr      out number,
    p_extra_o_1 out varchar2,
    p_extra_o_2 out varchar2,
    p_extra_o_3 out varchar2,
    p_trantype  in  number,
    p_msisdn    in  varchar2,
    p_req_id    in  number,
    p_ref_id    in  number,
    p_extra_i_1 in  varchar2,
    p_extra_i_2 in  varchar2,
    p_extra_i_3 in  varchar2,
    p_extra_i_4 in  varchar2
   ) is
   nRetr                Number;
   bRoamer              Boolean;
   nMinBal              Number;
   nMinBal_Web          Number;
   nDuration            Number;
   nCustMinBal          Number;
   nCustDuration        Number;
   nCustBreathingPeriod Number;
   nCustPre_Act_Day     Number;
   nCustNum_Link        Number;
   dActivation          Date;
   dDeactivation        Date;
   vProfile             Varchar2(16);
   nPreActPending       Number;
   nDefaultDuration     Number := 180;
   nDefaultPreAct       Number := 7;
   nMinBalExt           Number;
   dSubsActDt           Date;
   dSubsDeactDt         Date;
   nSubsInHot           Number;
   dSubsHotDt           Date;
   nSubsHotDur          Number;
   nBreathing_period    Number;
   nDefaultMaxDuration  Number := 9999;
   nRoamerStatus        Number;
   vStatus              Varchar2(12);
   vBlackReason         Varchar2(128);
   vActiveLinks         Varchar2(128);
   nLinkTo              Number;
   vOtherType           Varchar2(30);
   vIMSI                Varchar2(30);
   -- Bluemoon
   vPromoName           conditioner_promos.promo_name%type;
   vNF_ServiceID        conditioner_promos.service_id%type;
   -- Voyager
   isVYGR               Boolean;
   vVYGR_exp            Varchar2(30);
begin
   --  For All tran type
   --    100 - Blacklisted
   --    101 - Invalid Duration
   --    102 - no active roaming
   --    103 - no active fun profile
   --    104 - Invalid Activation Date Format
   --    105 - Invalid Pre-Activation past 7 days
   --    106 - Duplicate Pre-Activation
   --    107 - Reached Max Num Link
   --    108 - Insuff balance
   --    109 - already active roamer
   --    110 - Reg and Link Error - Local SIM is already used as OFW SIM
   --    111 - Invalid Duration for GROAM EXTEND
   --    112 - Reg and Link Error - Link Already exists
   --    113 - for GROAM ON - Not in HLR Range
   --    114 - Reg and Link - Local SIM already link
   --    115 - REG - Not in customer type mapping or not in range
   --    116 - REG - LOCAL SIM same with OFW SIM
   --    117 - REG - only PREPAID are allowed
   --    118 - REG - link to POSTPAID are not allowed
   --    119 - GROAM ON - on breathing period
   --    140 - GROAM EXTEND - with open ended roaming
   --    141 - YES - no pending link
   --    142 - Invalid Syntax or Invalid Match but existing FUN subs
   --    143 - Reg and Link - OFW SIM already link as Local SIM
   --    144 - Cancel Error - Sender is local SIM
   --    145 - USURF ON Error - Already subscribe to USURF (na)
   --    146 - USURF ON Error - invalid country (na)
   --    147 - USURF ON Error - invalid duration (na)
   --    148 - USURF STATUS for no subscription
   --    149 - USURF STATUS with pending subscription (na)
   --    150 - USURF ok
   --    151 - USURF pre-activation
   --    152 - USURF invalid keyword
   --    153 - USURF coex
   --    155 - VYGR ON Error - Already subscribe to USURF (na)
   --    156 - VYGR ON Error - invalid country (na)
   --    157 - VYGR ON Error - invalid duration (na)
   --    158 - VYGR STATUS for no subscription
   --    159 - VYGR STATUS with active subscription (na)
   --    160 - VYGR ok
   --    161 - VYGR pre-activation (na)
   --    162 - VYGR invalid keyword
   --    163 - VYGR coex (na)

   --  0    TRAN_TYPE_UNKNOWN,
   --  1    TRAN_TYPE_HELP,
   --  2    TRAN_TYPE_REG,
   --  3    TRAN_TYPE_UNREG,
   --  4    TRAN_TYPE_LINK,
   --  5    TRAN_TYPE_CHECKLINK,
   --  6    TRAN_TYPE_CANCEL,
   --  7    TRAN_TYPE_GROAM_HELP,
   --  8    TRAN_TYPE_GROAM_ON,
   --  9    TRAN_TYPE_GROAM_OFF,
   -- 10    TRAN_TYPE_GROAM_EXTEND,
   -- 11    TRAN_TYPE_GROAM_STATUS,
   -- 12    TRAN_TYPE_GLIST
   -- 13    TRAN_TYPE_GLIST_ZONE
   -- 17    TRAN_TYPE_YES
   -- 18    TRAN_TYPE_YES_ARDS
   -- 19    TRAN_TYPE_NO_ARDS
   -- 20    USURF_ON
   -- 21    USURF_OFF
   -- 22    USURF_STATUS

   -- retr successful returns
   --      1 - successful except pre-reg
   --      2 - successful for pre-reg

   sp_logger('INIT' , 'START =>'      ||
                      ' p_trantype :' || to_char(p_trantype) ||
                      ' p_msisdn:'    || p_msisdn            ||
                      ' p_req_id:'    || to_char(p_req_id)   ||
                      ' p_ref_id:'    || to_char(p_ref_id)   ||
                      ' p_extra_i_1:' || p_extra_i_1         ||
                      ' p_extra_i_2:' || p_extra_i_2         ||
                      ' p_extra_i_3:' || p_extra_i_3         ||
                      ' p_extra_i_4:' || p_extra_i_4);

   nRetr     := 1; -- successful except prereg
   bRoamer   := FALSE;
   nMinBal   := -1;

   if sf_is_blacklisted(p_msisdn) > 0 then

      if p_trantype = 2 then
         vBlackReason := 'Error on REG due to blacklisting.';
      elsif p_trantype = 3 then
         vBlackReason := 'Error on UNREG due to blacklisting.';
      elsif p_trantype = 4 then
         vBlackReason := 'Error on LINK due to blacklisting.';
      elsif p_trantype = 6 then
         vBlackReason := 'Error on CANCEL due to blacklisting.';
      elsif p_trantype = 8 then
         vBlackReason := 'Error on GROAM ON due to blacklisting.';
      elsif p_trantype = 9 then
         vBlackReason := 'Error on GROAM OFF due to blacklisting.';
      else
         vBlackReason := 'Error accessing application due to blacklisting.';
      end if;

      insert into BLACKLISTED_WHITELISTED_LOG
      values (p_msisdn, 'BLACKLISTED', sysdate, vBlackReason, trunc(sysdate), user, 0 );
      commit;

      nRetr  := 100;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;

   end if;

   -- GROAM ON - validate msisdn if its in range
   if (p_trantype in (8,9)) and (sf_is_in_hlr_range(p_msisdn) = 0) then
      nRetr  := 113;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- REG - validate msisdn if its in range
   --if (p_trantype = 2) and (sf_is_in_customer_type_range('FUN', p_msisdn) = 0) then
   --   nRetr  := 115;
   --   p_retr := nRetr;
   --   return;
   --end if;

   -- REG - LOCAL SIM same with OFW SIM
   if (p_trantype in (2,3,4,5,6)) and ( p_msisdn = p_extra_i_3) then
      if (p_trantype = 5) then
          vActiveLinks := sf_get_fun_links(p_msisdn);
          if vActiveLinks is not null then
             nRetr  := 142;
             p_retr := nRetr;
             p_extra_o_2 := vActiveLinks;
             sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
             return;
          else
             vActiveLinks := sf_get_fun_links_local(p_msisdn);
             if vActiveLinks is not null then
                nRetr  := 142;
                p_retr := nRetr;
                p_extra_o_2 := vActiveLinks;
                sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
                return;
             end if;
          end if;
      end if;
      nRetr  := 116;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- REG - only PREPAID are allowed
   if (p_trantype in (2,3)) and (sf_is_in_hlr_range(p_msisdn) = 0) then
      nRetr  := 117;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- REG - only PREPAID are allowed
   if (p_trantype in (2,3)) and (sf_is_in_hlr_range(p_extra_i_3) = 0) then
      nRetr  := 118;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- LINK - only PREPAID are allowed
   if (p_trantype in (4,6)) and (sf_is_in_hlr_range(p_msisdn) = 0) then
      nRetr  := 118;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- LINK - only PREPAID are allowed
   if (p_trantype in (4,6)) and (sf_is_in_hlr_range(p_extra_i_3) = 0) then
      nRetr  := 117;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- GROAM ON - validate start date
   if (p_trantype = 8) and (p_extra_i_1 is not null) then
      begin
         if length(p_extra_i_1) < 10 then
            nRetr  := 104;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         end if;

         dActivation := to_date(p_extra_i_1, 'MM/DD/YYYY');
         if dActivation < trunc(sysdate) then
            nRetr  := 105;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         end if;
      exception
         when others then
            nRetr  := 104;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
      end;
   end if;

   -- GROAM ON and GROAM EXTEND - validate duration
   if (p_trantype in (8,10)) and (p_extra_i_2 is not null) and (nvl(p_ref_id, 0) = 0) then
      if (p_extra_i_2 is not null) then
         begin
            nDuration := to_number(p_extra_i_2);
         exception
            when others then
               if (p_trantype = 8) then
                  nRetr := 101;
               --else
               --   nRetr := 111;
               end if;
               p_retr := nRetr;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
         end;
         -- GROAM EXTEND - validate duration
         -- allow open duration for subs in non-hot country
         --if (p_trantype = 10) and (nDuration=0) then
         --   nRetr  := 111;
         --   p_retr := nRetr;
         --   sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         --   return;
         --end if;
      end if;

   end if;

   -- 2 - TRAN_TYPE_REG
   -- 4 - TRAN_TYPE_LINK
   -- 8 - TRAN_TYPE_GROAM_ON
   -- 10 - TRAN_TYPE_GROAM_EXTEND
   if (p_trantype in (2,4,8,10)) then

      -- check if process comes from web or sms
      nBreathing_period := 0;
      if nvl(p_ref_id, 0) > 0 then
         begin
            select nvl(min_bal,0), nvl(duration,0), tx_date, nvl(breathing_period,0), other_type, nvl(min_bal,1)
            into   nMinbal, nDuration, dActivation, nBreathing_period, vOtherType, nMinBal_Web
            from   manual_enrollment_log
            where  id = p_ref_id;
            -- check if exempt from bal check
            if nMinbal <> 0 then
               nMinbal := -1;
            end if;
            if nDuration = 0 then
               nDuration := null;
            end if;
         exception
            when NO_DATA_FOUND then nMinbal := -1;
         end;
      end if;

      -- GROAM ON - validate activation date
      if (p_trantype = 8) and (dActivation is null) then
         nRetr  := 104;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- get customer type info
      begin
         if p_trantype in (2,4) then
            vProfile := 'FUN';
         else
            if (sf_is_active_fun(p_msisdn) > 0) then
               vProfile := 'FUN';
            else
               vProfile := sf_get_customer_type(to_number(p_msisdn));
            end if;
         end if;
         select nvl(min_balance,0) min_balance, nvl(max_duration,nDefaultDuration) max_duration, breathing_period, nvl(pre_act_day,nDefaultPreAct), nvl(num_link,2), ext_min_balance
         into   nCustMinbal, nCustDuration, nCustBreathingPeriod, nCustPre_act_day, nCustNum_Link, nMinBalExt
         from   customer_types
         where  customer_type = vProfile;
      exception
         when no_data_found then
            raise_application_error(-200002, 'Customer type: ' || vProfile || ' not defined. Please contact your application provider.');
      end;

      -- GROAM ON - validate max duration
      if (p_trantype = 8) then
         nCustDuration := nDefaultMaxDuration;
         if (nDuration is not null) and (nDuration > 0) and (nCustDuration < nDuration) then
            nRetr := 101;
            p_retr := nRetr;
            return;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         end if;
      end if;

      -- GROAM ON - validate past pre act days
      if (p_trantype = 8) and (dActivation > (trunc(sysdate)+nCustPre_act_day)) then
         nRetr  := 105;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- GROAM ON
      if (p_trantype = 8) and (sf_is_roamer_info_imsi(p_msisdn, dSubsActDt, dSubsDeactDt, nSubsHotDur, nSubsInHot, dSubsHotDt, vStatus, vIMSI ) > 0) then

         -- validate if already roamer
         if (vStatus='ACTIVE') then

            begin
               insert into ARDSResponse (id, imsi, enrollment_type, status, cause, start_tm, end_tm)
               values (ARDSResponse_id_seq.nextval, vIMSI, 'ACTIVATION', 0, 2, sysdate, null);
            exception
               when others then null;
            end;

            nRetr := 109;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         end if;

         -- validate breathing period
         if (nBreathing_period=0) and (nSubsHotDur>0) and (nSubsInHot > 0) and
            ((nvl(dSubsDeactDt,sysdate)-dSubsActDt) >= nSubsHotDur) and
            ((trunc(sysdate)-dSubsDeactDt) < nCustBreathingPeriod )
         then
            nRetr := 119;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         end if;

      end if;

      -- GROAM EXTEND - validate max duration
      if (p_trantype = 10) then

         -- GROAM EXTEND - validate if not an existing roamer
         begin
            select hot_country_cnt
            into   nSubsInHot
            from   sim_activation
            where  msisdn = p_msisdn
            and    status = 'ACTIVE';
         exception
            when no_data_found then
               nRetr := 102;
               p_retr := nRetr;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
         end;

         dbms_output.put_line('nDuration:'|| to_char(nDuration) || ' nCustDuration:'|| to_char(nCustDuration)  || ' nSubsInHot:'|| to_char(nSubsInHot));
         if (nSubsInHot > 0) then
            -- open duration is not allowed for subs in hot country, allowed is up to max duration.
            if (nDuration is null) or (nDuration=0) or (nCustDuration < nDuration) then
               nRetr := 111;
               p_retr := nRetr;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' spcl nCustDuration: ' || to_char(nCustDuration) || ', nDuration:' || to_char(nDuration) );
               return;
            end if;
         else
            -- allow open duration for subs in non-hot country
            nCustDuration := nDefaultMaxDuration;
            if (nDuration is not null) and (nDuration > 0 ) and (nCustDuration < nDuration) then
               nRetr := 111;
               p_retr := nRetr;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' rgrl nCustDuration: ' || to_char(nCustDuration) || ', nDuration:' || to_char(nDuration) );
               return;
            end if;
         end if;
      end if;

      -- GGROAM EXTEND - validate current duration check if open duration
      --if (p_trantype = 10) and (dSubsDeactDt is null) then
      --   nRetr := 140;
      --   p_retr := nRetr;
      --   sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:'|| p_extra_o_3);
      --   return;
      --end if;

      -- REG - check if LOCAL SIM was already used as OFW SIM
      if (p_trantype = 2) and (sf_is_active_fun(p_msisdn) > 0) then
         nRetr := 110;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- REG - if link already exists
      if (p_trantype = 2) and (sf_is_link_active(p_extra_i_3, p_msisdn) > 0) then
         nRetr := 112;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- REG - local sim already link to OFW SIM
      if (p_trantype = 2) and (sf_is_local_sim_active_link(p_msisdn) > 0) then
         nRetr := 114;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- REG - OFW sim already link as LOCAL SIM
      if (p_trantype = 2) and (sf_is_local_sim_active_link(p_extra_i_3) > 0) then
         nRetr := 143;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- REG - exceeds max number of link for OFW SIM
      if (p_trantype = 2) and (sf_is_active_fun(p_extra_i_3) >= nCustNum_Link) then
         nRetr := 107;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- LINK - sim already link as LOCAL SIM
      if (p_trantype = 4) and (sf_is_local_sim_active_link(p_msisdn) > 0) then
         nRetr := 144;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- LINK - sim already link as LOCAL SIM
      if (p_trantype = 4) and (sf_is_local_sim_active_link(p_extra_i_3) > 0) then
         nRetr := 114;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- LINK - check if LOCAL SIM was already used as OFW SIM
      if (p_trantype = 4) and (sf_is_active_fun(p_extra_i_3) > 0) then
         nRetr := 110;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- LINK - if link already exists
      if (p_trantype = 4) and (sf_is_link_active(p_msisdn, p_extra_i_3) > 0) then
         nRetr := 112;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- LINK - exceeds max number of link for OFW SIM
      if (p_trantype = 4) and (sf_is_active_fun(p_msisdn) >= nCustNum_Link) then
         nRetr := 107;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- get default mimimun balance
      if (nMinbal = 0) and (nvl(p_ref_id, 0) > 0) then
         nMinBal := 0;
      else
         if p_trantype = 10 then
            nMinBal := nMinBalExt;
         else
            nMinBal := nCustMinbal;
         end if;
      end if;

      if (dActivation > trunc(sysdate)) and (p_trantype = 8) then
         begin
            -- check if there's already pending pre-act transaction
            SELECT count(1)
            INTO   nPreActPending
            FROM   MANUAL_ENROLLMENT_LOG
            WHERE  ENROLLMENT_TYPE = 'ACTIVATION'
            AND    MSISDN = p_msisdn
            AND    STATUS = 0;

            nRetr := 2;
            if nPreActPending = 0 then
               sp_logger('INIT' , 'MID => p_trantype :' || to_char(p_trantype) || ' nMinBal:' || to_char(nMinBal) || ', nCustMinbal:' || to_char(nCustMinbal));
               INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin , customer_type)
               VALUES (manual_enrollment_log_seq.nextval, p_msisdn, 'ACTIVATION', 0, dActivation, nvl(nDuration,0), 0, user, sysdate, NULL, vProfile, NULL, 'Pre-activation', NULL, decode(nMinBal, 0, 0, 1), 0, 'SMS', vProfile);
            else
               nRetr := 106;
               p_retr := nRetr;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
            end if;
         exception
            when dup_val_on_index then
              nRetr := 106;
              p_retr := nRetr;
              sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
              return;
            when others then
              nRetr := SQLCODE;
              p_retr := nRetr;
              sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' ORAERR p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
              return;
         end;
      elsif (p_trantype = 8) then
         update request_log
         set    duration = nDuration,
                min_bal  = nMinBal,
                breathing_period = nBreathing_period,
                msg = nvl(vOtherType, 'ACTIVATION')
         where  id = p_req_id;
      elsif (p_trantype = 10) then
         update request_log
         set    duration = nDuration,
                min_bal  = nMinBal,
                breathing_period = nBreathing_period
         where  id = p_req_id;
      end if;

   --  9 - TRAN_TYPE_GROAM_OFF
   elsif (p_trantype = 9) then
      nRoamerStatus := SF_CHECK_ROAMER_STATUS_IMSI(p_msisdn, vIMSI);
      if nRoamerStatus = 0 then

         begin
            insert into ARDSResponse (id, imsi, enrollment_type, status, cause, start_tm, end_tm)
            values (ARDSResponse_id_seq.nextval, vIMSI, 'DEACTIVATION', 0, 2, sysdate, null);
         exception
            when others then null;
         end;

         nRetr := 102;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      elsif nRoamerStatus = 2 then
         nRetr := 2;
      end if;

      if nvl(p_ref_id, 0) > 0 then
         --begin
         --   select other_type
         --   into   vOtherType
         --   from   manual_enrollment_log
         --   where  id = p_ref_id;
            if vOtherType = 'INSUFF_BAL' then
               nRetr := 3;
            --elsif vOtherType = 'OVERSTAY_HOT_COUNTRY' then
            --   nRetr := 4;
            end if;
         --exception
         --   when NO_DATA_FOUND then nMinbal := -1;
         --end;
      end if;


   --  11 - TRAN_TYPE_GROAM_STATUS
   elsif (p_trantype = 11) then
      if sf_is_active_roamer(p_msisdn) = 0 then
         nRetr := 102;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

   -- 3 - TRAN_TYPE_UNREG
   elsif (p_trantype = 3) and (sf_is_link_active(p_extra_i_3, p_msisdn) = 0) then
      if sf_is_active_fun(p_msisdn) > 0  then
         nRetr  := 144;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;
      nRetr := 103;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;

   -- 5 - TRAN_TYPE_CHECKLINK
   elsif (p_trantype = 5) then
      if (p_extra_i_3 is not null) then
         begin
            nLinkTo := to_number(p_extra_i_3);
            if nLinkTo < 639000000000 then
               nLinkTo := null;
            end if;
         exception
            when others then nLinkTo := null;
         end;
      else
         nLinkTo := null;
      end if;

      if (nLinkTo is null) then
         vActiveLinks := sf_get_fun_links(p_msisdn);
         if vActiveLinks is not null then
            nRetr  := 142;
            p_retr := nRetr;
            p_extra_o_2 := vActiveLinks;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         else
            vActiveLinks := sf_get_fun_links_local(p_msisdn);
            if vActiveLinks is not null then
               nRetr  := 142;
               p_retr := nRetr;
               p_extra_o_2 := vActiveLinks;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
            else
               nRetr  := 141;
               p_retr := nRetr;
               p_extra_o_2 := '';
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
            end if;
         end if;
      end if;

      if (sf_is_link_active(p_msisdn, p_extra_i_3) = 0) then
         if (sf_is_link_active(p_extra_i_3, p_msisdn) = 0) then
            vActiveLinks := sf_get_fun_links(p_msisdn);
            if vActiveLinks is not null then
               nRetr  := 142;
               p_retr := nRetr;
               p_extra_o_2 := vActiveLinks;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
            else
               vActiveLinks := sf_get_fun_links_local(p_msisdn);
               if vActiveLinks is not null then
                  nRetr  := 142;
                  p_retr := nRetr;
                  p_extra_o_2 := vActiveLinks;
                  sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
                  return;
               end if;
            end if;
            nRetr := 103;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         end if;
      end if;

   -- 6 - TRAN_TYPE_CANCEL
   elsif (p_trantype = 6) and (sf_is_link_active(p_msisdn, p_extra_i_3) = 0) then
      vActiveLinks := sf_get_fun_links_local(p_msisdn);
      if vActiveLinks is not null then
         nRetr  := 144;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;
      nRetr := 103;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;

   -- 14 - TRAN_TYPE_YES
   elsif (p_trantype = 17) and (sf_is_link_pending(p_msisdn) = 0) then
      nRetr := 141;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;

   -- 18    TRAN_TYPE_YES_ARDS
   elsif (p_trantype = 18) then
      nRoamerStatus := sf_check_roamer_status(p_msisdn);
      if nRoamerStatus = 0 then
         nRetr := 102;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      nRetr := 1;
      p_retr := nRetr;
      return;

   -- 20    TRAN_TYPE_USURF_ON
   elsif (p_trantype = 20) then
      if sf_is_in_hlr_range(p_msisdn) = 0 then
         isVYGR := TRUE;
      else
         isVYGR := FALSE;
      end if;

      if sf_is_valid_usurf(p_extra_i_1, p_extra_i_2)=2 then
         if isVYGR then
            nRetr := 162;
         else
            nRetr := 152;
         end if;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      if isVYGR then
         nRetr := 160;
         nRoamerStatus := 0;

         -- if sf_check_usurf_status(p_msisdn, p_extra_i_1, p_extra_i_2, 0, 'POSTPAID', 2) = 1 then
         --    nRetr := 159;
         -- else
         if sf_greencard_check_coex(p_msisdn) = 1 then
            sp_process_greencard (p_trantype, p_msisdn, p_req_id, p_extra_i_1, p_extra_i_2, nRoamerStatus, 'POSTPAID');
         else
            nRetr := 163;
         end if;
         -- end if;
      else
         if sf_check_roamer_status(p_msisdn) = 1 then
            nRetr := 150;
            nRoamerStatus := 0;
         else
            nRetr := 151;
            nRoamerStatus := 3;
         end if;

         -- pre-activation
         if nRetr = 151 then
            if sf_check_usurf_status(p_msisdn, p_extra_i_1, p_extra_i_2, 1, 'PREPAID', 3) = 1 then
               nRetr := 153;
            end if;
         end if;
         
         if nRetr in (150,151) then
            sp_process_greencard (p_trantype, p_msisdn, p_req_id, p_extra_i_1, p_extra_i_2, nRoamerStatus, 'PREPAID');
         end if;

      end if;

      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;

   -- 21    TRAN_TYPE_USURF_OFF
   -- 22    TRAN_TYPE_USURF_STATUS
   elsif (p_trantype IN (21,22)) then
      if sf_is_in_hlr_range(p_msisdn) = 0 then
         isVYGR := TRUE;
      else
         isVYGR := FALSE;
      end if;
      if sf_is_valid_usurf(p_extra_i_1, p_extra_i_2)=2 then
         if isVYGR then
            nRetr := 162;
         else
            nRetr := 152;
         end if;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      if isVYGR then
         nRetr := 161;
         nRoamerStatus := 0;

         if sf_check_usurf_status(p_msisdn, p_extra_i_1, p_extra_i_2, 0, 'POSTPAID', 2) = 0 then
            nRetr := 158;
         else
            if (p_trantype = 21) then
               sp_process_greencard (p_trantype, p_msisdn, p_req_id, p_extra_i_1, p_extra_i_2, nRoamerStatus, 'POSTPAID');
            elsif (p_trantype = 22) then
               begin
                  select to_char(dt_activated + to_number(replace(promo_value,'D','')),'YYYY-MM-DD HH24:MI:SS')
                  into   vVYGR_exp
                  from   conditioner_log
                  where  msisdn = p_msisdn
                  and    promo_code = p_extra_i_1
                  and    promo_value = p_extra_i_2
                  and    tran_type = 20
                  and    status = 2
                  and    brand = 'POSTPAID';
                  p_extra_o_1 := vVYGR_exp;
               exception
                 when others then
                    p_retr := -9999;
                    sp_logger('INIT' , 'ERROR => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ' ERR:' || SQLCODE);
                    return;
               end;
            end if;
         end if;
      else
         if sf_check_roamer_status(p_msisdn) = 1 then
            nRetr := 150;
            nRoamerStatus := 0;
         else
            nRetr := 151;
            nRoamerStatus := 3;
         end if;

         -- pre-activation
         if nRetr = 151 then
            if sf_check_usurf_status(p_msisdn, p_extra_i_1, p_extra_i_2, 0, 'PREPAID', 3) = 0 then
               nRetr := 148;
            else
               if (p_trantype = 21) then
                  delete from conditioner_log where msisdn = p_msisdn and promo_code = p_extra_i_1 and promo_value = p_extra_i_2 and tran_type = 20;
                  commit;
               end if;
            end if;
         elsif nRetr = 150  then
            sp_process_greencard (p_trantype, p_msisdn, p_req_id, p_extra_i_1, p_extra_i_2, nRoamerStatus, 'PREPAID');
         end if;
      end if;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   --  9    TRAN_TYPE_GROAM_OFF,
   if (p_trantype = 9) and (nRetr = 1) then
      begin
         update sim_activation
         set    status='PENDING'
         where  msisdn  = p_extra_i_1;
      exception
         when others then null;
      end;
   end if;

   -- set return - minimun balance
   if nMinBal is not null then
      p_extra_o_1 := to_char(nMinBal);
   else
      p_extra_o_1 := '';
   end if;

   --if nDuration is not null then
   --   p_extra_o_2 := to_char(nDuration);
   --else
      p_extra_o_2 := '';
   --end if;
   p_extra_o_3 := '';

   -- 2  - TRAN_TYPE_REG
   -- 4  - TRAN_TYPE_LINK
   -- 8  - TRAN_TYPE_GROAM_ON
   -- 9  - TRAN_TYPE_GROAM_OFF
   -- 10 - TRAN_TYPE_GROAM_EXTEND
   if (p_trantype in (2,4,8,9,10)) then
      p_extra_o_3 := sf_is_sweden_nsn_range(p_msisdn);
   end if;

   p_retr      := nRetr;
   sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) ||
                      ' p_msisdn:'          || p_msisdn            ||
                      ' p_retr:'            || to_char(p_retr)     ||
                      ' p_extra_o_1: '      || p_extra_o_1         ||
                      ' p_extra_o_2:'       || p_extra_o_2         ||
                      ' p_extra_o_3:'       || p_extra_o_3);

end sp_init_tran;
/
show err

set define &

