CREATE OR REPLACE TRIGGER PREACTIVATION_TRIGGER
before insert on manual_enrollment_log
for each row
begin
   if :new.enrollment_type = 'ACTIVATION' and :new.tx_date > trunc(sysdate)then
         begin
            insert into sim_activation (id, msisdn, activation_dt, status, remarks, request_origin, duration, min_bal, daily_bal)
            values (sim_activation_seq.nextval, :new.msisdn, trunc(:new.tx_date), 'PENDING', :new.remarks, :new.request_origin,
                    :new.duration, :new.min_bal, 0 );
         exception
            when dup_val_on_index then
                UPDATE sim_activation
                SET    activation_dt = trunc(:new.tx_date),
                       deactivation_dt = null,
                       status = 'PENDING',
                       remarks = :new.remarks
                WHERE  msisdn = :new.msisdn;
            when others then null;
         end;
   end if;
   if :new.msisdn <= 0 then 
      declare
         nMSISDN Number;
      begin
         select msisdn 
         into   nMSISDN
         from sim_activation 
         where imsi = :new.imsi;
         if nMSISDN is not null then
            :new.msisdn := nMSISDN;
         end if;
      exception
         when no_data_found then null;
      end;
   end if;
end PREACTIVATION_TRIGGER;
/
