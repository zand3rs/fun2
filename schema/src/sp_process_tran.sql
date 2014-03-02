create or replace procedure sp_process_tran (
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
    p_extra_i_3 in  varchar2
   ) is
   vSubsName               manual_enrollment_log.subs_name%type;
   vSubsGroup              manual_enrollment_log.subs_group%type;
   vSubsContactNo          manual_enrollment_log.subs_contact_no%type;
   vRemarks                manual_enrollment_log.remarks%type;
   nDuration               manual_enrollment_log.duration%type;
   nMinBal                 manual_enrollment_log.min_bal%type;
   nCustomerType           manual_enrollment_log.customer_type%type;
   nDaily_bal              Number;
   nCustDuration           Number;
   nCustMinBal             Number;
   nCustBreathingPeriod    Number;
   nActivationDt           Date;
   nDeactivationDt         Date;
   dDeactivateTm           Date;
   vSource                 Varchar2(8) := 'WEB';
   nRetr                   Number;
   vProfile                Varchar2(16);
   dDeactivateDt           Date;
   dPreAct                 Date;
   bPreAct                 Boolean;
   vCountries              Varchar2(1024);
   vCharge                 Varchar2(255);
   nDefaultDuration        Number := 180;
   nDefaultPreAct          Number := 7;
   vOtherType              Varchar2(30);
   nSubsBreathing          Number;
   nSubsBreathingPeriod    Number;
   nSubsBreathingPeriod_dt Date;
   nLinkType               Number;
   nMsisdn                 Number;
   nLinkCnt                Number;
   vCS_Username            Varchar2(30);
   vIMSI                   Varchar2(30);
begin
   --  1 - ACTIVATION
   --  2 - DEACTIVATION
   --  3 - EXTEND
   -- 17 - LINK
   -- 20 - CANCEL

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

   sp_logger('PROCESS' , 'START => ' ||
                      ' p_trantype:'  || to_char(p_trantype) ||
                      ' p_msisdn:'    || p_msisdn            ||
                      ' p_req_id:'    || to_char(p_req_id)   ||
                      ' p_ref_id:'    || to_char(p_ref_id)   ||
                      ' p_extra_i_1:' || p_extra_i_1         ||
                      ' p_extra_i_2:' || p_extra_i_2         ||
                      ' p_extra_i_3:' || p_extra_i_3);

   nRetr := 1;
   vSource := 'SMS';
   if nvl(p_ref_id, 0) > 0 then
      begin
         select subs_name, subs_group, subs_contact_no, remarks, duration, customer_type, min_bal, remarks, request_origin, other_type, breathing_period, cs_username, imsi
         into   vSubsName, vSubsGroup, vSubsContactNo, vRemarks, nDuration, nCustomerType, nMinBal, vRemarks, vSource, vOtherType, nSubsBreathingPeriod, vCS_Username, vIMSI
         from   manual_enrollment_log
         where  id = p_ref_id;
      exception
         when NO_DATA_FOUND then null;
      end;
   end if;

   --  8    TRAN_TYPE_GROAM_ON
   if (p_trantype = 8) then

      -- check if process is pre activation
      bPreAct := FALSE;
      if p_extra_i_1 is not null then
         begin
            dPreAct := to_date(p_extra_i_1, 'MM/DD/YYYY');
            if dPreAct > trunc(sysdate) then
               bPreAct := TRUE;
            end if;
         exception
            when others then null;
         end;
      end if;

      if not bPreAct then
         -- get customer type info
         if nvl(p_ref_id, 0) = 0 then
            begin
               select duration
               into   nDuration
               from   request_log
               where  id = p_req_id;
            exception
               when no_data_found then nDuration := 0;
            end;
         end if;

         -- get customer type info
         begin
            --vProfile := sf_get_customer_type(to_number(p_msisdn));
            if (sf_is_active_fun(p_msisdn) > 0) then
               vProfile := 'FUN';
            else
               vProfile := sf_get_customer_type(to_number(p_msisdn));
            end if;

            select nvl(max_duration,nDefaultDuration) max_duration, breathing_period, min_balance, daily_balance
            into   nCustDuration, nCustBreathingPeriod, nCustMinBal, nDaily_bal
            from   customer_types
            where  customer_type = vProfile;
         exception
            when no_data_found then
               raise_application_error(-200002, 'Customer type: ' || vProfile || ' not found. Please contact your application provider.');
         end;

         if nMinBal = 0 then
            nCustMinBal := 0;
         end if;

         if (nDuration is null) or (nDuration = 0) then
            nCustDuration := null;
            dDeactivateDt := null;
         else
            nCustDuration := nDuration;
            dDeactivateDt := trunc(sysdate)+nCustDuration;
         end if;

         if vSource = 'ARDS' then
            nCustDuration := 2;
            dDeactivateDt := trunc(sysdate)+nCustDuration;
            dDeactivateTm := sysdate+nCustDuration;
         end if;


         begin
            if p_extra_i_3 is null and vIMSI is null then
               begin
                  select imsi 
                  into   vIMSI
                  from   sim_activation
                  where  msisdn = p_msisdn;
               exception
                  when others then null;
               end;
            else
               if vIMSI is null then
                  vIMSI := p_extra_i_3;
               end if;
            end if;
            insert into ARDSResponse (id, imsi, enrollment_type, status, cause, start_tm, end_tm)
            values (ARDSResponse_id_seq.nextval, vIMSI, 'ACTIVATION', 0, null, sysdate, null);
         exception
            when others then null;
         end;

         begin
            insert into sim_activation (id, msisdn, activation_dt, deact_basis_dt, deactivation_dt, deactivation_tm, subscriber_name, status, duration, min_bal, remarks, request_origin, daily_bal, last_daily_bal_chk , imsi, customer_type, breathing_period, activation_tm, cs_username)
            values (sim_activation_seq.nextval, p_msisdn, trunc(sysdate), trunc(sysdate), dDeactivateDt, dDeactivateTm, vSubsName, 'ACTIVE', nCustDuration, nCustMinBal, vRemarks, vSource, nDaily_bal, trunc(sysdate), p_extra_i_3, vProfile, nCustBreathingPeriod, sysdate, vCS_Username);
         exception
            when dup_val_on_index then
                if vOtherType = 'REACTIVATION' then
                   UPDATE sim_activation
                   SET    reactivation_dt = trunc(sysdate),
                          deactivation_dt = decode(hot_country_cnt,0, (deact_basis_dt+duration), (deact_basis_dt+hot_country_duration)),
                          remarks = vRemarks,
                          status = 'ACTIVE',
                          last_daily_bal_chk = trunc(sysdate),
                          last_daily_bal_ctr = 0,
                          daily_bal = 0,
                          hot_country_dt = null,
                          cs_username = vCS_Username,
                          breathing_period_dt=null
                   WHERE  msisdn = p_msisdn;
               else
                   if nvl(nSubsBreathingPeriod,0) = 1 then
                      nCustBreathingPeriod := 0;
                      nSubsBreathingPeriod_dt := null;
                   else
                      nSubsBreathingPeriod_dt := trunc(sysdate)+nCustBreathingPeriod;
                   end if;

                   UPDATE sim_activation
                   SET    activation_dt = trunc(sysdate),
                          deactivation_dt = dDeactivateDt,
                          deact_basis_dt = trunc(sysdate),
                          reactivation_dt = null,
                          extended = 0,
                          remarks = vRemarks,
                          status = 'ACTIVE',
                          subscriber_name = vSubsName,
                          min_bal = nCustMinBal,
                          duration = nCustDuration,
                          last_daily_bal_chk = trunc(sysdate),
                          last_daily_bal_ctr = 0,
                          daily_bal = 0,
                          request_origin = vSource,
                          imsi = p_extra_i_3,
                          breathing_period = nCustBreathingPeriod,
                          --breathing_period_dt = decode(nvl(hot_country_cnt,0), 0, null, nSubsBreathingPeriod_dt),
                          breathing_period_dt = null,
                          notify_date = null,
                          country = null,
                          country_name = null,
                          last_country_chk = null,
                          hot_country_dt = null,
                          hot_country_cnt = null,
                          hot_country_duration = null,
                          hot_country_max_duration = null,
                          customer_type = vProfile,
                          activation_tm = sysdate,
                          deactivation_tm = dDeactivateTm,
                          cs_username = vCS_Username
                   WHERE  msisdn = p_msisdn;
               end if;
         end;
      else
         nActivationDt := dPreAct;
      end if;

   --  9    TRAN_TYPE_GROAM_OFF
   elsif (p_trantype = 9) then
      delete from manual_enrollment_log
      where  enrollment_type = 'ACTIVATION'
      and    msisdn = p_msisdn
      and    status = 0;
      begin
         INSERT INTO sim_activation_history
              ( id, msisdn, country, subscriber_name, name_of_req_party, activation_dt, reactivation_dt,
                status, deactivation_dt, hot_country_cnt, hot_country_dt, hot_country_duration, hot_country_max_duration,
                remarks, daily_bal, min_bal, duration, extended, breathing_period, breathing_period_dt,
                last_daily_bal, last_daily_bal_chk, last_daily_bal_ctr, last_country_chk,
                imsi, customer_type, country_name, sim_activation_id, activation_tm, deactivation_tm,
                request_origin, cs_username, created_by, dt_created, updated_by, dt_updated, notify_date )
         SELECT sim_activation_history_seq.nextval, msisdn, country, subscriber_name, vSubsName, activation_dt, reactivation_dt,
                'INACTIVE', trunc(sysdate), hot_country_cnt, hot_country_dt, hot_country_duration, hot_country_max_duration,
                vRemarks, daily_bal, min_bal, duration, extended, breathing_period, breathing_period_dt,
                last_daily_bal, last_daily_bal_chk, last_daily_bal_ctr, last_country_chk,
                imsi, customer_type, country_name, id sim_activation_id, activation_tm, sysdate,
                vSource, nvl(vCS_Username,cs_username), created_by, sysdate, null, null, notify_date
         FROM   sim_activation
         WHERE  msisdn = p_msisdn;
      exception
         when dup_val_on_index then null;
      end;
      UPDATE sim_activation
      SET    deactivation_dt = trunc(sysdate),
             remarks = vRemarks,
             status = 'INACTIVE',
             name_of_req_party = vSubsName,
             request_origin = vSource,
             deactivation_tm = sysdate,
             cs_username = nvl(vCS_Username,cs_username)
      WHERE  msisdn = p_msisdn
      RETURNING IMSI INTO vIMSI;

      if vOtherType = 'INSUFF_BAL' then
         nRetr := 3;
      --elsif vOtherType = 'OVERSTAY_HOT_COUNTRY' then
      --   nRetr := 4;
      end if;


      begin
         insert into ARDSResponse (id, imsi, enrollment_type, status, cause, start_tm, end_tm)
         values (ARDSResponse_id_seq.nextval, vIMSI, 'DEACTIVATION', 0, null, sysdate, null);
      exception
         when others then null;
      end;

   -- 10    TRAN_TYPE_GROAM_EXTEND
   elsif (p_trantype = 10) then

      if nvl(p_ref_id, 0) = 0 then
         if (p_extra_i_1 is not null) then
            begin
               nDuration := to_number(p_extra_i_1);
            exception
               when others then nDuration := 0;
            end;
         else
            nDuration := 0;
         end if;
      end if;

      -- get customer type info
      --if nvl(p_ref_id, 0) = 0 then
      --   begin
      --      select nvl(duration,nDefaultDuration)
      --      into   nDuration
      --      from   request_log
      --      where  id = p_req_id;
      --   exception
      --      when no_data_found then nDuration := 0;
      --   end;
      --end if;

      -- get customer type info
      -- open duration
      if nDuration = 0 then
         --begin
         --   vProfile := sf_get_customer_type(to_number(p_msisdn));
         --   select nvl(max_duration,nDefaultDuration) max_duration, breathing_period, min_balance, daily_balance
         --   into   nCustDuration, nCustBreathingPeriod, nCustMinBal, nDaily_bal
         --   from   customer_types
         --   where  customer_type = vProfile;
         --exception
         --   when no_data_found then
         --      raise_application_error(-200002, 'Customer type: ' || vProfile || ' not found. Please contact your application provider.');
         --end;
         nCustDuration := 0;
         dDeactivateDt := null;
      else
         nCustDuration := nDuration;
         dDeactivateDt := trunc(sysdate)+nCustDuration;
      end if;

      UPDATE sim_activation
      SET    deactivation_dt = decode(nCustDuration, 0, null, (nvl(deactivation_dt,trunc(sysdate)) + nCustDuration)),
             duration = decode(hot_country_cnt, 0, (nvl(duration,0)+nCustDuration), duration),
             hot_country_duration = decode(hot_country_cnt, 0, 0, (nvl(hot_country_duration,0)+nCustDuration)),
             deact_basis_dt = decode(nCustDuration, 0, trunc(sysdate), nvl(deact_basis_dt,trunc(sysdate))),
             extended = extended + 1,
             remarks  = vRemarks,
             status   = 'ACTIVE',
             name_of_req_party = vSubsName,
             request_origin = vSource
      WHERE  msisdn = p_msisdn;

   -- 11    TRAN_TYPE_GROAM_STATUS
   elsif (p_trantype = 11) then
      nActivationDt   := null;
      nDeactivationDt := null;
      nCustDuration   := null;
      begin
         select activation_dt, deactivation_dt, nvl(duration,0), customer_type
         into   nActivationDt, nDeactivationDt, nCustDuration, vProfile
         from   sim_activation
         where  msisdn  = p_msisdn;
      exception
         when others then
             nRetr := SQLCODE;
      end;
      --if nDeactivationDt is null and nCustDuration > 0 then
      --   nDeactivationDt := nActivationDt+nCustDuration;
      --else
      --   -- get customer type info
      --   begin
      --      select nvl(max_duration,180) max_duration
      --      into   nCustDuration
      --      from   customer_types
      --      where  customer_type = vProfile;
      --   exception
      --      when no_data_found then
      --         raise_application_error(-200002, 'Customer type: ' || vProfile || ' not found. Please contact your application provider.');
      --   end;
      --   nDeactivationDt := nActivationDt+nCustDuration;
      --end if;


   -- 12    TRAN_TYPE_GLIST
   elsif (p_trantype = 12) then
      if p_extra_i_1 is not null then
         vCountries := null;
         for i in (select zone_id, charge_desc from countries where country_name_upper like upper(p_extra_i_1)) loop
             vCountries := TO_CHAR(i.zone_id);
             vCharge    := i.charge_desc;
             exit;
         end loop;
      end if;

   -- 13    TRAN_TYPE_GLIST_ZONE
   elsif (p_trantype = 13) then
      if p_extra_i_1 is not null and to_number(p_extra_i_1) > 0 then
         vCountries := null;
         for i in (select country_name from countries where zone_id = p_extra_i_1 and country_name is not null) loop
            if vCountries is null then
               vCountries := i.country_name;
            else
               if length(vCountries)+length(i.country_name) > 319 then
                  vCountries := vCountries || '...';
                  exit;
               end if;
               vCountries := vCountries || ', ' || i.country_name;
            end if;
         end loop;
      end if;

   --  2    TRAN_TYPE_REG
   elsif (p_trantype = 2) then
      begin
         insert into sim_activation_links (id, customer_type, msisdn, link_to, link_to_type, status, cust_activation_dt, dt_created, link_cmd, ods_last_chk_dt, cust_activation_tm, cs_username)
         values (sim_activation_links_seq.nextval, 'FUN', p_extra_i_1, p_msisdn, 'OFW', 'ACTIVE', trunc(sysdate), sysdate, 'REG', trunc(sysdate), sysdate, vCS_Username);
      exception
         when dup_val_on_index then
             update sim_activation_links
             set    customer_type = 'FUN',
                    cust_activation_dt = trunc(sysdate),
                    cust_activation_tm = sysdate,
                    cust_deactivation_dt = null,
                    cust_deactivation_tm = null,
                    status =  'ACTIVE',
                    dt_modified =  sysdate,
                    link_cmd = 'REG',
                    ods_last_chk_dt = trunc(sysdate),
                    notify_date = null,
                    cs_username = vCS_Username
             where  msisdn = p_extra_i_1
             and    link_to = p_msisdn;
         when others then
             nRetr := SQLCODE;
      end;

      begin
         update sim_activation
         set    customer_type = 'FUN',
                last_daily_bal_chk = trunc(sysdate),
                last_daily_bal_ctr = 0,
                daily_bal = 0
         where msisdn  = p_extra_i_1;
      exception
         when others then null;
      end;

   --  3    TRAN_TYPE_UNREG
   elsif (p_trantype = 3) then
      begin
         update sim_activation_links
         set    status = 'INACTIVE',
                cust_deactivation_dt = trunc(sysdate),
                cust_deactivation_tm = sysdate,
                cs_username = nvl(vCS_Username,cs_username)
         where msisdn  = p_extra_i_1
         and   link_to = p_msisdn;
      exception
         when others then
             nRetr := SQLCODE;
      end;

      begin
         select count(1) into nLinkCnt
         from   sim_activation_links
         where  msisdn  = p_extra_i_1
         and    status <> 'INACTIVE';
         if nLinkCnt = 0 then
            update sim_activation
            set    customer_type = 'TRIGGER'
            where msisdn  = p_extra_i_1;
         end if;
      exception
         when others then null;
      end;

      if nvl(nLinkCnt,0) > 0 then
         nRetr := 7;
      end if;

   --  4    TRAN_TYPE_LINK
   elsif (p_trantype = 4) then
      begin
         insert into sim_activation_links (id, customer_type, msisdn, link_to, link_to_type, status, cust_activation_dt, dt_created, link_cmd, expiry_date, ods_last_chk_dt, cust_activation_tm, cs_username)
         values (sim_activation_links_seq.nextval, 'FUN', p_msisdn, p_extra_i_1, 'OFW', 'PENDING', trunc(sysdate), sysdate, 'LINK', sysdate, trunc(sysdate), sysdate, vCS_Username );
      exception
         when dup_val_on_index then
             update sim_activation_links
             set    cust_activation_dt = trunc(sysdate),
                    cust_activation_tm = sysdate,
                    cust_deactivation_dt = null,
                    cust_deactivation_tm = null,
                    status = 'PENDING',
                    dt_modified = sysdate,
                    expiry_date = sysdate,
                    link_cmd = 'LINK',
                    ods_last_chk_dt = trunc(sysdate),
                    notify_date = null,
                    cs_username = vCS_Username
             where  msisdn = p_msisdn
             and    link_to = p_extra_i_1;
         when others then
             nRetr := SQLCODE;
      end;

   --  17    TRAN_TYPE_YES
   elsif (p_trantype = 17) then
      update sim_activation_links
      set    cust_activation_dt = trunc(sysdate),
             cust_deactivation_dt = null,
             status =  'ACTIVE',
             dt_modified =  sysdate,
             link_cmd = 'LINK',
             expiry_date = null,
             notify_date = null
      where  status = 'PENDING'
      and    link_to = p_msisdn
      returning msisdn into nMsisdn;

      begin
         update sim_activation
         set    customer_type = 'FUN',
                last_daily_bal_chk = trunc(sysdate),
                last_daily_bal_ctr = 0,
                daily_bal = 0
         where msisdn  = nMsisdn;
      exception
         when others then null;
      end;

   --  18    TRAN_TYPE_YES_ARDS
   elsif (p_trantype = 18) then

      -- get customer type info
      begin
         --vProfile := sf_get_customer_type(to_number(p_msisdn));
         if (sf_is_active_fun(p_msisdn) > 0) then
            vProfile := 'FUN';
         else
            vProfile := sf_get_customer_type(to_number(p_msisdn));
         end if;
 
         select nvl(max_duration,nDefaultDuration) max_duration, breathing_period, min_balance, daily_balance
         into   nCustDuration, nCustBreathingPeriod, nCustMinBal, nDaily_bal
         from   customer_types
         where  customer_type = vProfile;
      exception
         when no_data_found then
            raise_application_error(-200002, 'Customer type: ' || vProfile || ' not found. Please contact your application provider.');
      end;

      UPDATE sim_activation
      SET    status = 'ACTIVE',
             deactivation_dt = activation_dt + nCustDuration,
             deactivation_tm = nvl(activation_tm,activation_dt) + nCustDuration,
             duration = nCustDuration,
             last_daily_bal_chk = trunc(sysdate),
             last_daily_bal_ctr = 0,
             daily_bal = 0,
             hot_country_dt = null
      WHERE  msisdn = p_msisdn;

   --  5    TRAN_TYPE_CHECKLINK
   elsif (p_trantype = 5) then
      nLinkType := 0;
      begin
         --vProfile := sf_get_customer_type(to_number(p_msisdn));
         select cust_activation_dt, cust_deactivation_dt
         into   nActivationDt, nDeactivationDt
         from   sim_activation_links
         where  msisdn  = p_msisdn
         and    link_to = p_extra_i_1
         and    STATUS = 'ACTIVE';
      exception
         when no_data_found then
            begin
               select cust_activation_dt, cust_deactivation_dt
               into   nActivationDt, nDeactivationDt
               from   sim_activation_links
               where  msisdn  = p_extra_i_1
               and    link_to = p_msisdn
               and    STATUS = 'ACTIVE';
               nLinkType := 1;
            exception
               when others then
                   nRetr := SQLCODE;
            end;
         when others then
             nRetr := SQLCODE;
      end;

   --  6    TRAN_TYPE_CANCEL
   elsif (p_trantype = 6) then
      begin
         vProfile := sf_get_customer_type(to_number(p_msisdn));
         update sim_activation_links
         set    status = 'INACTIVE',
                cust_deactivation_dt = trunc(sysdate),
                cust_deactivation_tm = sysdate
         where msisdn  = p_msisdn
         and   link_to = p_extra_i_1;
      exception
         when others then
             nRetr := SQLCODE;
      end;

      begin
         select count(1) into nLinkCnt
         from   sim_activation_links
         where  msisdn  = p_msisdn
         and    status <> 'INACTIVE';
         if nLinkCnt = 0 then
            update sim_activation
            set    customer_type = 'TRIGGER'
            where  msisdn  = p_msisdn;
         end if;
      exception
         when others then null;
      end;
      if vOtherType = 'ODS_INACTIVITY' then
         if nLinkCnt = 0 then
            nRetr := 5;
         else
            nRetr := 6;
         end if;
      else
         if nvl(nLinkCnt,0) > 0 then
            nRetr := 7;
         end if;
      end if;
   end if;
   commit;

   -- set o_1 for GROAM STATUS
   if (p_trantype = 11) and (nActivationDt is not null) then
      p_extra_o_1 := to_char(nActivationDt, 'MM/DD/YYYY');
   -- set o_1 for GLIST and GLIST_ZONE
   elsif (p_trantype in (12, 13)) and (vCountries is not null) then
      p_extra_o_1 := vCountries;
   elsif (p_trantype = 5) then
      p_extra_o_1 := to_char(nLinkType);
   -- set o_1 for GROAM ON (pre-activation)
   elsif (p_trantype = 8) and (bPreAct) and (nActivationDt is not null) then
      --p_extra_o_2 := to_char(nActivationDt, 'MM/DD/YYYY');
      p_extra_o_1 := to_char(nActivationDt, 'MM/DD/YYYY');
   elsif (p_trantype = 17) then
      p_extra_o_1 := to_char(nMsisdn);
   else
      p_extra_o_1 := '';
   end if;

   -- set o_2 for GROAM STATUS
   if (p_trantype = 11) and (nDeactivationDt is not null) then
           p_extra_o_2 := to_char(nDeactivationDt, 'MM/DD/YYYY');
   -- set o_1 for GLIST
   elsif (p_trantype = 12) and (vCountries is not null) then
      p_extra_o_2 := vCharge;
   else
      p_extra_o_2 := '';
   end if;

   -- set o_3 for GROAM EXTEND
   if (p_trantype = 10) and (nCustDuration is not null) then
      p_extra_o_3 := to_char(nCustDuration);
   else
      p_extra_o_3 := '';
   end if;

   p_retr := nRetr;
   sp_logger('PROCESS' , 'END => p_trantype:'  || to_char(p_trantype) ||
                         ' p_msisdn:'          || p_msisdn            ||
                         ' p_retr:'            || to_char(p_retr)     ||
                         ' p_extra_o_1: '      || p_extra_o_1         ||
                         ' p_extra_o_2:'       || p_extra_o_2         ||
                         ' p_extra_o_3:'       || nvl(p_extra_o_3, 'null=' || to_char(nCustDuration)));

end sp_process_tran;
/
