create or replace procedure sp_process_for_deactivation is
   nCommitCtr Number := 0;
   nDeActPending Number;
   nCustBreathingPeriod Number;
   vReason Varchar2(60);
   vOtherType Varchar2(30);
   nWarningDeact number;
   vMsg Varchar2(120);
begin
   begin
      select warning_deact_days
      into   nWarningDeact
      from   customer_types
      where  customer_type = 'TRIGGER'
      and    rownum = 1;
      if (nWarningDeact is null) or (nWarningDeact<=0) then
         nWarningDeact := 2;
      end if;
   exception
      when no_data_found then
         nWarningDeact := 2;
   end;

   sp_logger('SP_PROCESS_FOR_DEACTIVATION', 'START => nWarningDeact:' || to_char(nWarningDeact));
   for i in (select msisdn, activation_dt, deactivation_dt, duration, hot_country_cnt, customer_type, decode(request_origin, 'ARDS', 'ARDS', 'WEB') request_origin
             from   sim_activation
             where  status ='ACTIVE'
             and    deactivation_dt < trunc(sysdate) )
   loop
      if (i.request_origin = 'ARDS' and i.duration > 2) or
         (i.request_origin = 'WEB')
      then
         begin
            -- check if there's already pending pre-act transaction
            SELECT nvl(max(id),0)
            INTO   nDeActPending
            FROM   MANUAL_ENROLLMENT_LOG
            WHERE  ENROLLMENT_TYPE = 'DEACTIVATION'
            AND    MSISDN = i.msisdn
            AND    STATUS = 0;
         exception
            when no_data_found then nDeActPending := 0;
         end;
         
         
         vReason    := 'Scheduled Deactivation';
         vOtherType := '';
         if ((i.deactivation_dt-i.activation_dt)>=i.duration) and
             (i.hot_country_cnt > 0)
         then
            begin
               select breathing_period
               into   nCustBreathingPeriod
               from   customer_types
               where  customer_type = i.customer_type;
            exception
               when no_data_found then nCustBreathingPeriod := 0;
               when others then nCustBreathingPeriod := 0;
            end;
            if nCustBreathingPeriod > trunc(sysdate-i.deactivation_dt) then
               vReason    := 'Due to Overstay on Special Country';
               vOtherType := 'OVERSTAY_HOT_COUNTRY';
            end if;
         end if;
         
         if nDeActPending = 0 then
            INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin, other_type )
            VALUES (manual_enrollment_log_seq.nextval, i.msisdn, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, user, sysdate, NULL, 'TRIGGER', NULL, vReason, NULL, 0, 0, i.request_origin, vOtherType);
         else
            UPDATE MANUAL_ENROLLMENT_LOG
            SET    TX_DATE = trunc(sysdate),
                   REMARKS = vReason
            where  ID = nDeActPending;
         end if;
         update sim_activation set status='PENDING' where MSISDN=i.msisdn;
         nCommitCtr := nCommitCtr + 1;
         if (mod(nCommitCtr,1500) = 0) then
            commit;
         end if;
      end if;
   end loop;
   commit;
   sp_logger('SP_PROCESS_FOR_DEACTIVATION', 'END FOR DEACT => nCommitCtr:' || to_char(nCommitCtr));

   nCommitCtr := 0;
   --sp_logger('SP_PROCESS_FOR_PRE_DEACTIVATION', 'START');
   -- SMS Alert for subscribers who will deactivated in n days...
   for i in (select msisdn, activation_dt, deactivation_dt, duration, hot_country_cnt, customer_type, request_origin
             from   sim_activation
             where  status ='ACTIVE'
             and    deactivation_dt < (trunc(sysdate)+nWarningDeact) )
   loop
      if (i.request_origin = 'ARDS' and i.duration > 2) or
         (i.request_origin = 'WEB')
      then
         begin
            INSERT INTO BROADCAST_LOG ( ID, MSISDN, MESSAGE_ID, TX_DATE, STATUS, CLUSTER_NODE )
            VALUES (broadcast_log_seq.nextval, i.msisdn, 'GROAM_OFF_PRE_DEACT_WARNING', trunc(sysdate), 0, 1);
            nCommitCtr := nCommitCtr + 1;
         exception
            when dup_val_on_index then null;
            when others then null;
         end;
         if (mod(nCommitCtr,1500) = 0) then
            commit;
         end if;
      end if;
   end loop;
   commit;


   for i in (select msisdn, activation_dt, deactivation_dt, deactivation_tm, duration, hot_country_cnt, customer_type, deactivation_reason, request_origin
             from   sim_activation
             where  status ='ACTIVE'
             and    request_origin = 'ARDS'
             and    deactivation_dt <= trunc(sysdate)
             and    deactivation_tm <= sysdate - (8/24) )
   loop
      vMsg := null;
      if (i.deactivation_tm > (sysdate - 2)) and (i.deactivation_tm <= (sysdate - 1)) and
         (nvl(i.deactivation_reason,' ') <> '1st Notification')
      then
         vMsg := 'ARDS_PROVISIONAL_PROV_1ST_EXPIRY';
         update sim_activation 
         set    deactivation_reason = '1st Notification'
         where  msisdn = i.msisdn;

      elsif (i.deactivation_tm > (sysdate - 1)) and (i.deactivation_tm <= (sysdate - (8/24))) and
         (i.deactivation_reason = '1st Notification')
      then
         vMsg := 'ARDS_PROVISIONAL_PROV_2ND_EXPIRY';
         update sim_activation 
         set    deactivation_reason = '2nd Notification'
         where  msisdn = i.msisdn;

      elsif (i.deactivation_tm < (sysdate - 2)) and (i.deactivation_reason = '2nd Notification') then

         INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin, other_type )
         VALUES (manual_enrollment_log_seq.nextval, i.msisdn, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, user, sysdate, NULL, 'TRIGGER', NULL, vReason, NULL, 0, 0, i.request_origin, null); 

         update sim_activation set status='PENDING' where MSISDN=i.msisdn;

         nCommitCtr := nCommitCtr + 1;
      end if;

      if (vMsg is not null) then
         begin
            INSERT INTO BROADCAST_LOG ( ID, MSISDN, MESSAGE_ID, TX_DATE, STATUS, CLUSTER_NODE )
            VALUES (broadcast_log_seq.nextval, i.msisdn, vMsg, trunc(sysdate), 0, 1);
            nCommitCtr := nCommitCtr + 1;
         exception
            when dup_val_on_index then null;
            when others then null;
         end;
      end if;

      if (mod(nCommitCtr,1500) = 0) then
         commit;
      end if;

   end loop;

   commit;
   sp_logger('SP_PROCESS_FOR_PRE_DEACTIVATION', 'END WARNING => nCommitCtr:' || to_char(nCommitCtr));

end sp_process_for_deactivation;
/
