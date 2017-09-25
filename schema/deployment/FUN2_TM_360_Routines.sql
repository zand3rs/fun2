set define ^

-- UPDATE TO GET THE PRIORITY SEQUENCE FOR TM
CREATE OR REPLACE FUNCTION "TRIGGER_OWNER"."SF_GET_PROCESS_SEQ" (p_brand in varchar2) return varchar2 is
   vSteps Varchar2(128);
begin
   for i in (select service_code, priority from services where status = 'ACTIVE' and brand = p_brand order by priority) loop
      if vSteps is null then
         vSteps := i.service_code || ':' || to_char(i.priority);
      else
         vSteps := vSteps || '^' || i.service_code || ':' || to_char(i.priority);
      end if;
   end loop;
   return nvl(vSteps,' ');
exception
   when others then return ' ';
end sf_get_process_seq;
/
show err




-- UPDATE GET THE PRIORITY SEQUENCE FOR TM
CREATE OR REPLACE FUNCTION "TRIGGER_OWNER"."SF_GET_REVERSE_PROCESS_SEQ" (p_brand in varchar2) return varchar2 is
   vSteps Varchar2(128);
begin
   for i in (select service_code, priority from services where status = 'ACTIVE' and brand=p_brand order by priority desc) loop
      if vSteps is null then
         vSteps := i.service_code || ':' || to_char(i.priority);
      else
         vSteps := vSteps || '^' || i.service_code || ':' || to_char(i.priority);
      end if;
   end loop;
   return nvl(vSteps,' ');
exception
   when others then return ' ';
end sf_get_reverse_process_seq;
/
show err




-- UPDATE TO ADD EXPIRY FOR TM
CREATE OR REPLACE PROCEDURE "TRIGGER_OWNER"."SP_PROCESS_FOR_ARDS_EXPIRY" is
   nCommitCtr Number := 0;
   nDeactCtr  Number := 0;
   vReason Varchar2(60);
   vOtherType Varchar2(30);
   vMsg   Varchar2(2200);
   vMsgId Varchar2(120);
   vBrand Varchar2(20);

   vDiffTime Number;
   nWarningDeact number;
   nArds1stNotify number;
   nArds2ndNotify number;
   nArdsDuration number;

   nTmWarningDeact number;
   nTmArds1stNotify number;
   nTmArds2ndNotify number;
   nTmArdsDuration number;
begin
   begin
      select warning_deact_days, ards_1st_notify, ards_2nd_notify, ards_tmp_duration
      into   nWarningDeact, nArds1stNotify, nArds2ndNotify, nArdsDuration
      from   customer_types
      where  customer_type = 'TRIGGER'
      and    rownum = 1;
      if (nWarningDeact is null) or (nWarningDeact<=0) then
         nWarningDeact := 2;
      end if;
      if (nArds1stNotify is null) or (nArds1stNotify<=0) then
         nArds1stNotify := .5;
      end if;
      if (nArds2ndNotify is null) or (nArds2ndNotify<=0) then
         nArds2ndNotify := .33333333333;
      end if;
      if (nArdsDuration is null) or (nArdsDuration<=0) then
         nArdsDuration := 1;
      end if;
   exception
      when no_data_found then
         nWarningDeact  := 2;
         nArds1stNotify := .5;
         nArds2ndNotify := .33333333333;
         nArdsDuration  := 1;
   end;

   begin
      select warning_deact_days, ards_1st_notify, ards_2nd_notify, ards_tmp_duration
      into   nTmWarningDeact, nTmArds1stNotify, nTmArds2ndNotify, nTmArdsDuration
      from   customer_types
      where  customer_type = 'TM'
      and    rownum = 1;
      if (nTmWarningDeact is null) or (nTmWarningDeact<=0) then
         nTmWarningDeact := 2;
      end if;
      if (nTmArds1stNotify is null) or (nTmArds1stNotify<=0) then
         nTmArds1stNotify := .5;
      end if;
      if (nTmArds2ndNotify is null) or (nTmArds2ndNotify<=0) then
         nTmArds2ndNotify := .33333333333;
      end if;
      if (nTmArdsDuration is null) or (nTmArdsDuration<=0) then
         nTmArdsDuration := 1;
      end if;
   exception
      when no_data_found then
         nTmWarningDeact  := 2;
         nTmArds1stNotify := .5;
         nTmArds2ndNotify := .33333333333;
         nTmArdsDuration  := 1;
   end;

   for i in (select msisdn, activation_dt, deactivation_dt, deactivation_tm, duration, hot_country_cnt,
                    customer_type, deactivation_reason, request_origin, imsi, deactivation_tm-sysdate diff_time
             from   sim_activation
             where  status ='ACTIVE'
             and    request_origin = 'ARDS'
             and    deactivation_dt >= trunc(sysdate)-1
             and    deactivation_tm <= sysdate + 1)
   loop
      vMsgId := null;
      vDiffTime := (i.deactivation_tm - sysdate );
      sp_logger('SP_PROCESS_FOR_ARDS_EXPIRY' , 'ARDS EXPIRY for '  || to_char(i.msisdn) ||
                                                 ' difftime=' || to_char(vDiffTime) ||
                                                 ' nArds1stNotify=' || to_char(nArds1stNotify) ||
                                                 ' nArds2ndNotify=' || to_char(nArds2ndNotify) ||
                                                 ' nArdsDuration=' || to_char(nArdsDuration) ||
                                                 ' reason=' || i.deactivation_reason );

      vBrand := SF_GET_CUSTOMER_TYPE(i.msisdn);
      if vBrand = 'TM' then
         if vDiffTime between nTmArds2ndNotify and nTmArds1stNotify and
            (nvl(i.deactivation_reason,' ') <> '1st Notification')
         then
            vMsgId := 'ARDS_PROVISIONAL_PROV_1ST_EXPIRY';
            update sim_activation
            set    deactivation_reason = '1st Notification'
            where  msisdn = i.msisdn;
         
         elsif vDiffTime between 0 and nTmArds2ndNotify and
            (nvl(i.deactivation_reason,' ') <> '2nd Notification')
         then
            vMsgId := 'ARDS_PROVISIONAL_PROV_2ND_EXPIRY';
            update sim_activation
            set    deactivation_reason = '2nd Notification'
            where  msisdn = i.msisdn;
         end if;
      else
         vBrand := 'PREPAID';
         if vDiffTime between nArds2ndNotify and nArds1stNotify and
            (nvl(i.deactivation_reason,' ') <> '1st Notification')
         then
            vMsgId := 'ARDS_PROVISIONAL_PROV_1ST_EXPIRY';
            update sim_activation
            set    deactivation_reason = '1st Notification'
            where  msisdn = i.msisdn;
         
         elsif vDiffTime between 0 and nArds2ndNotify and
            (nvl(i.deactivation_reason,' ') <> '2nd Notification')
         then
            vMsgId := 'ARDS_PROVISIONAL_PROV_2ND_EXPIRY';
            update sim_activation
            set    deactivation_reason = '2nd Notification'
            where  msisdn = i.msisdn;
         end if;
      end if;

      if (vDiffTime <= 0) then
         vMsgId := 'ARDS_PROVISIONAL_PROV_LAST_EXPIRY';
         INSERT INTO MANUAL_ENROLLMENT_LOG (
                 ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED,
                 subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin, other_type, imsi )
         VALUES (manual_enrollment_log_seq.nextval, i.msisdn, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, user, sysdate,
                 NULL, vBrand, NULL, vReason, NULL, 0, 0, i.request_origin, 'Due to ARDS expiry', i.imsi);
         UPDATE sim_activation SET status='PENDING' WHERE MSISDN=i.msisdn;
         nCommitCtr := nCommitCtr + 1;
         nDeactCtr  := nDeactCtr + 1;
      end if;

      if (vMsgId is not null) then
         begin
            select msg_1
            into   vMsg
            from   keyword_msg
            where  message_id = vMsgId
            and    brand = vBrand;
         exception
            when no_data_found then null;
         end;
      end if;
      if (vMsg is not null) then
         begin
            INSERT INTO BROADCAST_LOG ( ID, MSISDN, MESSAGE_ID, MSG, TX_DATE, STATUS, CLUSTER_NODE )
            VALUES (broadcast_log_seq.nextval, i.msisdn, vMsgId, vMsg, trunc(sysdate), 0, 1);
            nCommitCtr := nCommitCtr + 1;
         exception
            when dup_val_on_index then
               sp_logger('SP_PROCESS_FOR_ARDS_EXPIRY' , 'ARDS BROADCAST_LOG Error: vMsgId='  || vMsgId || ' ORA:' || SQLCODE );
            when others then
               sp_logger('SP_PROCESS_FOR_ARDS_EXPIRY' , 'ARDS BROADCAST_LOG Error: vMsgId='  || vMsgId || ' ORA:' || SQLCODE );
         end;
      end if;
         
      if (mod(nCommitCtr,1500) = 0) then
         commit;
      end if;
   end loop;

   commit;
   sp_logger('SP_PROCESS_FOR_ARDS_EXPIRY', 'END WARNING => nCommitCtr:' || to_char(nCommitCtr) || ' nDeactCtr=>' || to_char(nDeactCtr));

end SP_PROCESS_FOR_ARDS_EXPIRY;
/
show err



set define &