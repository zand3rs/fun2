create or replace procedure sp_process_for_cancel is
   nCommitCtr Number := 0; 
   nDeActPending Number;
   vReason Varchar2(60);
begin
   sp_logger('SP_PROCESS_FOR_CANCEL', 'START');
   for i in (select msisdn, cust_activation_dt, cust_deactivation_dt
             from   sim_activation_links 
             where  status = 'ACTIVE' 
             and    ods_last_chk_dt <= (trunc(sysdate)-30))
   loop
      begin 
         -- check if there's already pending pre-act transaction
         SELECT nvl(max(id),0)
         INTO   nDeActPending
         FROM   MANUAL_ENROLLMENT_LOG
         WHERE  ENROLLMENT_TYPE = 'CANCEL'
         AND    MSISDN = i.msisdn
         AND    STATUS = 0;
      exception 
         when no_data_found then nDeActPending := 0;
      end;
      
      vReason := 'Due to ';
      if nDeActPending = 0 then
         INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin )
         VALUES (manual_enrollment_log_seq.nextval, i.msisdn, 'CANCEL', 0, trunc(sysdate), 0, 0, user, sysdate, NULL, 'FUN', NULL, vReason, NULL, 0, 0, 'WEB');
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
   end loop;
   commit;
   sp_logger('SP_PROCESS_FOR_CANCEL', 'END => nCommitCtr:' || to_char(nCommitCtr));

   nCommitCtr := 0;
   sp_logger('SP_PROCESS_FOR_CANCEL', 'START alert');
   -- SMS Alert for subscribers who will deactivated in n days...
   for i in (select msisdn, cust_activation_dt, cust_deactivation_dt
             from   sim_activation_links 
             where  status ='ACTIVE' 
             and    ods_last_chk_dt = (trunc(sysdate)-28) )
   loop
      begin 
         INSERT INTO BROADCAST_LOG ( ID, MSISDN, MESSAGE_ID, TX_DATE, STATUS, CLUSTER_NODE )
         VALUES (broadcast_log_seq.nextval, i.msisdn, 'CANCEL_PRE_DEACT_WARNING', trunc(sysdate), 0, 1);
         nCommitCtr := nCommitCtr + 1;
      exception 
         when dup_val_on_index then null;
         when others then null;
      end;
      if (mod(nCommitCtr,1500) = 0) then
         commit;
      end if;
   end loop;
   commit;
   sp_logger('SP_PROCESS_FOR_CANCEL', 'END alert => nCommitCtr:' || to_char(nCommitCtr));

end sp_process_for_cancel;
/
