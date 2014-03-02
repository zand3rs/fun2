alter table sim_activation add imsi varchar2(30);
alter table sim_activation add last_daily_bal number default 0;
alter table sim_activation add last_daily_bal_ctr number default 0;


-- to get ALL Active Roamer for daily balance check 
SELECT msisdn, daily_bal, last_daily_bal_ctr
FROM   sim_activation
WHERE  deactivation_dt is null
AND    daily_bal > 0
AND    last_daily_bal_chk < trunc(sysdate)
UNION
SELECT msisdn, daily_bal, last_daily_bal_ctr
FROM   sim_activation
WHERE  deactivation_dt > trunc(sysdate)
AND    daily_bal > 0
AND    last_daily_bal_chk < trunc(sysdate)


-- stored proc to call
create or replace procedure sp_process_daily_balance (
   p_retr        out number,
   p_deact_dt    out varchar2,
   p_msisdn      in  varchar2,
   p_daily_ctr_m in  number,
   p_daily_bal_m in  number,
   p_daily_bal_a in  number ) is
   -- possible retr
   --          1 - successful
   --        120 - insuff bal
   --        121 - insuff bal for deactivation
   nDaily_Bal_Ctr  Number;
   nRetr  Number;
   dDeact Date;
   nDeActPending Number;
begin
   nRetr := 1;
   sp_logger('PROCESS_DAILY_BALANCE', 'START => p_msisdn:' || p_msisdn || ' p_daily_bal_m:' || to_char(p_daily_bal_m) || ' p_daily_bal_a:' || to_char(p_daily_bal_a) || ' p_retr:' || to_char(nRetr) );
   if p_daily_bal_m > p_daily_bal_a then
      UPDATE sim_activation
      SET    last_daily_bal = p_daily_bal_a, 
             last_daily_bal_chk = trunc(sysdate),
             last_daily_bal_ctr = nvl(last_daily_bal_ctr,0) + 1
      WHERE  msisdn = to_number(p_msisdn)
      RETURNING last_daily_bal_ctr
      INTO      nDaily_Bal_Ctr;
      commit;

      if nDaily_Bal_Ctr >= 3 then
         begin 
            nDeActPending := 0;
            begin 
               -- check if there's already pending pre-act transaction
               SELECT nvl(max(id),0)
               INTO   nDeActPending
               FROM   MANUAL_ENROLLMENT_LOG
               WHERE  ENROLLMENT_TYPE = 'DEACTIVATION'
               AND    MSISDN = p_msisdn
               AND    STATUS = 0;
            exception 
               when no_data_found then nDeActPending := 0;
            end;

            if nDeActPending = 0 then
               INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin )
               VALUES (manual_enrollment_log_seq.nextval, p_msisdn, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, user, sysdate, NULL, 'TRIGGER', NULL, 'Due to Insuffbal', NULL, 0, 0, 'WEB');
            else
               UPDATE MANUAL_ENROLLMENT_LOG
               SET    TX_DATE = trunc(sysdate),
                      REMARKS = 'Due to Insuffbal'
               where  ID = nDeActPending;
            end if;
         exception 
            when others then 
              nRetr := SQLCODE;
              p_retr := nRetr;
              sp_logger('PROCESS_DAILY_BALANCE' , 'END => ORAERR p_retr:' || to_char(p_retr));
              return;
         end;

         nRetr := 121;
      else
         nRetr  := 120;
         dDeact := trunc(sysdate)+greatest(3-(nDaily_Bal_Ctr-1), 1);
      end if;

   else
      UPDATE sim_activation
      SET    last_daily_bal = p_daily_bal_a, 
             last_daily_bal_chk = trunc(sysdate),
             last_daily_bal_ctr = 0 
      WHERE  msisdn = to_number(p_msisdn);
      commit;
   end if;
   p_retr := nRetr;
   if dDeact is not null then
      p_deact_dt := to_char(dDeact,'MM/DD/YYYY');
   else 
      p_deact_dt := '';
   end if;  
   sp_logger('PROCESS_DAILY_BALANCE', 'END => p_msisdn:' || p_msisdn || ' p_retr:' || to_char(p_retr) || ' p_deact_dt:' || to_char(p_deact_dt) );

exception
   WHEN OTHERS THEN
   	raise_application_error(-200002, SQLERRM);
end sp_process_daily_balance;
/






alter table plmn_mapping add breathing_period number;
alter table plmn_mapping add max_duration number;
alter table plmn_mapping add constraint plmn_country_fk foreign key (country_code) references countries(country_code);
alter table plmn_mapping add constraint plmn_country_fk foreign key (country_code) references countries(country_code);
alter table COUNTRIES drop constraint COUNTRIES_FK;
alter table countries add constraint country_zone_fk foreign key (zone_id) references zones(zone_id);

create table geo_probe_log (
  imsi varchar2(30),
  addr varchar2(30),
  stat number default 0 not null,
  tx_date date,
  dt_created date
);

create index imsi_geo_idx on geo_probe_log(imsi);

CREATE OR REPLACE TRIGGER "GEO_PROBE_LOG_TRIGGER"
before insert on geo_probe_log 
for each row 
begin 
   :NEW.tx_date    := trunc(sysdate);
   :NEW.dt_created := sysdate;
end GEO_PROBE_LOG_TRIGGER;
/



-- stored proc to call
create or replace procedure sp_process_geo_probe (
   p_retr       out number,
   p_imsi       in  varchar2,
   p_addr       in  number ) is
 
   -- possible retr
   --          1 - successful
   --        130 - change to non-special country
   --        131 - change to hot/special country
   --        132 - excess max limit for consecutive stay on hot/special country
   nRetr         Number;
   nHotCtr       Number;
   nHot          Number;
   nDuration     Number;
   nBreathing    Number;
   nMsisdn       Number(12);
   vCountry      Varchar2(30);
   vCurrCountry  Varchar2(60);
   nCurrHot      Number;
   nDeActPending Number;
begin
   nRetr := 1;
   sp_logger('PROCESS_GEO_PROBE' , 'START => p_imsi:' || p_imsi || ', p_addr:' || to_char(p_addr));

   begin
      SELECT hot, country_code, breathing_period, max_duration 
      INTO   nHot, vCountry, nBreathing, nDuration
      FROM   plmn_mapping
      WHERE  addr = p_addr;

      SELECT hot_country_cnt, country, msisdn
      INTO   nHotCtr, vCurrCountry, nMsisdn
      FROM   sim_activation
      WHERE  imsi = p_imsi;

      SELECT hot
      INTO   nCurrHot
      FROM   plmn_mapping
      WHERE  country_code = vCurrCountry;

      if nHot > 0 then
         if nHotCtr > nBreathing then
            nRetr := 132;
            begin 
               nDeActPending := 0;
               begin 
                  -- check if there's already pending pre-act transaction
                  SELECT nvl(max(id),0)
                  INTO   nDeActPending
                  FROM   MANUAL_ENROLLMENT_LOG
                  WHERE  ENROLLMENT_TYPE = 'DEACTIVATION'
                  AND    MSISDN = nMsisdn
                  AND    STATUS = 0;
               exception 
                  when no_data_found then nDeActPending := 0;
               end;
            
               if nDeActPending = 0 then
                  INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin )
                  VALUES (manual_enrollment_log_seq.nextval, nMsisdn, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, user, sysdate, NULL, 'TRIGGER', NULL, 'Due to Overstay on Special Country', NULL, 0, 0, 'WEB');
               else
                  UPDATE MANUAL_ENROLLMENT_LOG
                  SET    TX_DATE = trunc(sysdate),
                         REMARKS = 'Due to Overstay on Special Country'
                  where  ID = nDeActPending;
               end if;
            exception 
               when others then 
                 nRetr := SQLCODE;
                 p_retr := nRetr;
                 sp_logger('PROCESS_GEO_PROBE' , 'END => ORAERR p_retr:' || to_char(p_retr));
                 return;
            end;
         else
            if nCurrHot <> nHot then
               nRetr := 131;
            end if;
         end if;
         UPDATE sim_activation
         SET    deactivation_dt = trunc(sysdate),
                hot_country_cnt = nvl(hot_country_cnt,0) + 1,
                country = vCountry,
                last_country_chk = trunc(sysdate)
         WHERE  msisdn = nMsisdn;
      else
         UPDATE sim_activation
         SET    hot_country_cnt = 0,
                country = vCountry,
                last_country_chk = trunc(sysdate)
         WHERE  msisdn = nMsisdn;
         if nCurrHot <> nHot then
            nRetr := 130;
         end if;
      end if;
      commit;

      sp_logger('PROCESS_GEO_PROBE' , 'END => nRetr:' || to_char(nRetr));
      p_retr := nRetr;

   exception
     when no_data_found then 
        p_retr := nRetr;
   end;

end sp_process_geo_probe;
/


-- #####################################################################################################
-- 
-- Sir ito yung sample data for Geoprobe:
-- 
-- ,0x00000002,0x0014009c,0x00000000,1,2,0x515022401818493f,1,2,0x1927830102110f,2,1,0x1927830102110f,3,1,0x19639160000001,4,1,0x19639052618134
-- 3,3:1219:571214215,02/10/09 12:26:47:124,02/10/09 12:26:52:798,8522,10502,255,12,966560132007,15,639170705490852,7,6,0005,GsmMap,0xa1,0x00000002,0xa2,0x00000002,0x0014009c,0x00000000,1,2,0x515020705490852f,1,2,0x19966560132007,2,1,0x19966560132007,3,1,0x19639170000026,4,1,0x19639174960965
-- 3,3:1267:1389255548,02/10/09 12:26:50:686,02/10/09 12:26:52:715,8689,10521,255,12,971504042991,15,639170203547645,7,6,0005,GsmMap,0xa1,0x00000002,0xa2,0x00000002,0x0014009c,0x00000000,1,2,0x515020203547645f,1,2,0x19971504042991,2,1,0x19971504042991,3,1,0x19639170000033,4,1,0x19639162923539
-- 3,3:1234:1046029859,02/10/09 12:26:50:938,02/10/09 12:26:52:876,10521,6185,255,12,639170000721,15,131231234579268,7,6,0005,GsmMap,0xa1,0x00000002,0xa3,0x00000008,0x0111009c,0x00000000,1,2,0x310410234579268f,1,2,0x19639170000721,2,1,0x19639170000721
-- 3,3:1219:571214526,02/10/09 12:26:51:398,02/10/09 12:26:52:846,10643,10521,255,10,6593340012,15,639170101824308,7,6,0005,GsmMap,0xa1,0x00000002,0xa2,0x00000002,0x0014009c,0x00000000,1,2,0x515020101824308f,1,2,0x196593340012,2,1,0x196593340012,3,1,0x19639170000020,4,1,0x19639175374264
-- 3,3:1234:1046029903,02/10/09 12:26:51:363,02/10/09 12:26:52:909,10643,10521,255,10,6596197740,15,639170111455504,7,6,0005,GsmMap,0xa1,0x00000002,0xa2,0x00000002,0x0014009c,0x00000000,1,2,0x515020111455504f,1,2,0x196596197740,2,1,0x196596197740,3,1,0x19639170000020,4,1,0x19639052408249
-- 
--  
-- 
-- Ito po yung column details:
-- FILENAME "TRIM('CDR.20090210.1227.ttp.txt')",BATCH_CODE "TRIM('20090210132716')",START_TRAN_DATE TIMESTAMP "MM/DD/YY HH24:MI:SS:FF3",END_TRAN_DATE TIMESTAMP "MM/DD/YY HH24:MI:SS:FF3",FILLER3 FILLER,FILLER4 FILLER,FILLER5FILLER,FILLER6 FILLER,SCCP,FILLER8 FILLER,FILLER9 FILLER,FILLER10 FILLER,FILLER11 FILLER,FILLER12 FILLER,FILLER13 FILLER,FILLER14 FILLER,FILLER15 FILLER,FILLER16 FILLER,FILLER17 FILLER,FILLER18 FILLER,FILLER19 FILLER,FILLER20 FILLER,FILLER21 FILLER,IMSI

 
