create or replace procedure sp_process_geo_probe (
   p_retr       out number,
   p_msisdn     out varchar2,
   p_country    out varchar2,
   p_deact_dt   out varchar2,
   p_imsi       in  varchar2,
   p_sccp       in  varchar2 ) is
 
   -- possible retr
   --          1 - successful
   --        130 - change to non-special country
   --        131 - change to hot/special country
   --        132 - excess max limit for consecutive stay on hot/special country
   nRetr         Number;
   nHotCtr       Number;
   nHot          Number;
   nDuration     Number;
   nSubsDuration Number;
   nBreathing    Number;
   nMsisdn       Number(12);
   vCountry      Varchar2(30);
   vCurrCountry  Varchar2(60);
   vCountryName  Varchar2(60);
   nCurrHot      Number;
   nDeActPending Number;
   dDeactivationDt Date;
   dActivationDt   Date;
begin
   nRetr := 1;
   sp_logger('PROCESS_GEO_PROBE' , 'START => p_imsi:' || p_imsi || ', p_sccp:' || to_char(p_sccp));

   begin
      SELECT hot, country_code, breathing_period, nvl(max_duration,99999)
      INTO   nHot, vCountry, nBreathing, nDuration
      FROM   plmn_mapping
      WHERE  addr = SUBSTR(p_sccp, 1, LENGTH(addr))
      AND    rownum = 1;
   exception
     when no_data_found then 
        nRetr := SQLCODE;
        p_retr := nRetr*-1;
        sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to PLMN_MAPPING 1 nRetr:' || to_char(p_retr));
        return;
     when others then 
        nRetr := SQLCODE;
        p_retr := nRetr;
        sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to PLMN_MAPPING 2 nRetr:' || to_char(p_retr));
        return;
   end;

   begin
      SELECT nvl(hot_country_cnt,0), country, msisdn, duration, activation_dt
      INTO   nHotCtr, vCurrCountry, nMsisdn, nSubsDuration, dActivationDt
      FROM   sim_activation
      WHERE  imsi = p_imsi;
   exception
     when no_data_found then 
        nRetr := SQLCODE;
        p_retr := nRetr*-1;
        sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to SIM_ACTIVATION 1 nRetr:' || to_char(p_retr));
        return;
     when others then 
        nRetr := SQLCODE;
        p_retr := nRetr;
        sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to SIM_ACTIVATION 2 nRetr:' || to_char(p_retr));
        return;
   end;

   if nHotCtr > 0 then
      nCurrHot := 1;
   else
      nCurrHot := 0;
   end if;

   begin
      -- get country name
      begin
         SELECT country_name
         INTO   vCountryName
         FROM   countries
         WHERE  country_code = vCountry
         AND    rownum = 1;
      exception
        when no_data_found then 
           vCountryName:= vCountry;
        when others then 
           vCountryName:= vCountry;
      end;

      if (nHot > 0) then
         dbms_output.put_line('CHECK => nSubsDuration:' || to_char(nSubsDuration) || ' nDuration:' || to_char(nDuration));
         if nSubsDuration is null then
            if nDuration is null then
               dDeactivationDt := null;
            else
               dDeactivationDt := dActivationDt+nDuration;
            end if;
         else
            dDeactivationDt := dActivationDt+least(nSubsDuration, nvl(nDuration,99999));
         end if;
         if (nCurrHot <> nHot) then
            nRetr := 131;
            UPDATE sim_activation
            SET    deactivation_dt = dDeactivationDt,
                   hot_country_cnt = nvl(hot_country_cnt,0) + 1,
                   country = vCountry,
                   country_name = vCountryName,
                   last_country_chk = trunc(sysdate)
            WHERE  msisdn = nMsisdn;
         else
            nRetr := 1;
            UPDATE sim_activation
            SET    hot_country_cnt = nvl(hot_country_cnt,0) + 1,
                   country = vCountry,
                   country_name = vCountryName,
                   last_country_chk = trunc(sysdate)
            WHERE  msisdn = nMsisdn;
         end if;
      else
         if nCurrHot <> nHot then
            nRetr := 130;
            if nSubsDuration is null then
               dDeactivationDt := null;
            else
               dDeactivationDt := dActivationDt+nSubsDuration;
            end if;
            UPDATE sim_activation
            SET    deactivation_dt = dDeactivationDt,
                   hot_country_cnt = 0,
                   country = vCountry,
                   country_name = vCountryName,
                   last_country_chk = trunc(sysdate)
            WHERE  msisdn = nMsisdn;
         else
            nRetr := 1;
            UPDATE sim_activation
            SET    hot_country_cnt = 0,
                   country = vCountry,
                   country_name = vCountryName,
                   last_country_chk = trunc(sysdate)
            WHERE  msisdn = nMsisdn;
         end if;
      end if;
      commit;

      sp_logger('PROCESS_GEO_PROBE' , 'END => p_imsi:' || p_imsi || ' nRetr:' || to_char(nRetr) || ' nMsisdn:' || to_char(nMsisdn) || ' nSubsDuration:' || to_char(nSubsDuration) || ' nDuration:' || to_char(nDuration));
      p_retr := nRetr;
      if nMsisdn is not null then
         p_msisdn := to_char(nMsisdn);
      else
         p_msisdn := '';
      end if;
      if vCountryName is not null then
         p_country := vCountryName;
      else
         p_country := '';
      end if;
      if dDeactivationDt is not null then
         p_deact_dt := to_char(dDeactivationDt, 'DD-MON-YYYY');
      else
         p_deact_dt := '';
      end if;

   exception
     when no_data_found then 
        nRetr := SQLCODE;
        p_retr := nRetr;
        sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' OERR_N nRetr:' || to_char(p_retr));
     when others then 
        nRetr := SQLCODE;
        p_retr := nRetr;
        sp_logger('PROCESS_GEO_PROBE' , 'END => ' || SQLERRM || ' p_imsi:' || p_imsi || ' OERR_O nRetr:' || to_char(p_retr) || ' nSubsDuration:' || to_char(nSubsDuration) || ' nDuration:' || to_char(nDuration) );
   end;

end sp_process_geo_probe;
/
