CREATE OR REPLACE TRIGGER REQUEST_LOG_INSERT_TRIGGER
before insert on request_log
for each row
begin
   if :new.TRAN_DT is null then
      :new.TRAN_DT := trunc(sysdate);
   end if;
   :new.STEP_SEQ := sf_get_process_seq;

   --if (:new.tran_type in (8,9)) and (:new.imsi is null or :new.imsi= '') then
   if (:new.imsi is null or :new.imsi= '') then
      begin
         select imsi 
         into  :new.imsi
         from  sim_activation
         where msisdn = :new.a_no;
      exception
         when others then null;
      end;
   end if;
   
end REQUEST_LOG_INSERT_TRIGGER;
/
