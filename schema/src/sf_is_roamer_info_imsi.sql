create or replace function sf_is_roamer_info_imsi (
   p_msisdn in number,
   p_activation out date,
   p_dectivation out date,
   p_duration out number,
   p_hot_chk out number,
   p_hot_dt out date,
   p_status out varchar2,
   p_imsi out varchar2
   ) return number is
   nRoamer Number(1);
   dAct Date;
   dDea Date;
   nDur Number;
   nHot Number;
   dHot Date;
   vSta Varchar2(12);
   vIMSI Varchar2(30);
begin
   nRoamer := 0;
   for i in (select deact_basis_dt activation_dt, deactivation_dt, hot_country_cnt, nvl(hot_country_max_duration,0) hot_country_max_duration, hot_country_dt, status, imsi
             from   sim_activation
             where  msisdn = p_msisdn)
   loop
      dAct := i.activation_dt;
      dDea := i.deactivation_dt;
      nDur := i.hot_country_max_duration;
      nHot := i.hot_country_cnt;
      dHot := i.hot_country_dt;
      vSta := i.status;
      vIMSI := i.imsi;
      nRoamer := 1;
      exit;
   end loop;
   p_activation  := dAct;
   p_dectivation := dDea;
   p_duration    := nDur;
   p_hot_chk     := nHot;
   p_hot_dt      := dHot;
   p_status      := vSta;
   p_imsi        := vIMSI;
   return nRoamer;
end sf_is_roamer_info_imsi;
/
