create or replace function sf_check_roamer_status_imsi (
   p_msisdn in number,
   p_imsi out varchar2
   ) return number is
   nRoamer Number(1);
   nActivationPending  Number;
   vImsi Varchar2(30);
begin
   nRoamer := 0;
   for i in (select activation_dt, nvl(deactivation_dt, trunc(sysdate+1)) deactivation_dt, status, imsi
             from   sim_activation
             where  msisdn = p_msisdn)
   loop
      nRoamer := 1;
      if i.status='ACTIVE' then
         nRoamer := 1;
      elsif (i.status='PENDING') and (i.deactivation_dt = trunc(sysdate)) then
         select count(1) into nActivationPending
         from   manual_enrollment_log
         where  enrollment_type = 'ACTIVATION'
         and    msisdn = p_msisdn
         and    status = 0;
         if nActivationPending > 0 then
           nRoamer := 2;
         else
           nRoamer := 1;
         end if;
      elsif i.status='INACTIVE' then
         nRoamer := 0;
      end if;
      vImsi := i.imsi;
      exit;
   end loop;
   p_imsi := vImsi;
   return nRoamer;
end sf_check_roamer_status_imsi;
/
