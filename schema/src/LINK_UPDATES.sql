NEW MESSAGE ID's
CANCEL_PRE_DEACT_WARNING
LINK_REQUEST_EXPIRED
YES_SUCCESSFUL_A
YES_SUCCESSFUL_B
YES_NO_PENDING_LINK


alter table sim_activation_links add expiry_date date;
create table sim_activation_link_expired as select * from sim_activation_links where rownum < 1;

create or replace function sf_is_link_pending (
   p_msisdn in number 
   ) return number is
   nRoamer Number(2);
begin
   nRoamer := 0;
   select count(1)
   into   nRoamer
   from   sim_activation_links
   where  link_to = p_msisdn
   and    status = 'PENDING';
   return nRoamer;
end sf_is_link_pending;
/

create or replace function sf_is_local_sim_active_link (
   p_msisdn in number 
   ) return number is
   nRoamer Number(2);
begin
   nRoamer := 0;
   select count(1)
   into   nRoamer
   from   sim_activation_links
   where  link_to = p_msisdn
   and    status <> 'INACTIVE';
   return nRoamer;
end sf_is_local_sim_active_link;
/

create or replace function sf_is_roamer_info (
   p_msisdn in number,
   p_activation out date,
   p_dectivation out date,
   p_duration out number,
   p_hot_chk out number,
   p_hot_dt out date,
   p_status out varchar2
   ) return number is
   nRoamer Number(1);
   dAct Date;
   dDea Date;
   nDur Number;
   nHot Number;
   dHot Date;
   vSta Varchar2(12);
begin
   nRoamer := 0;
   for i in (select activation_dt, deactivation_dt, hot_country_cnt, nvl(hot_country_max_duration,0) hot_country_max_duration, hot_country_dt, status
             from   sim_activation
             where  msisdn = p_msisdn)
   loop
      dAct := i.activation_dt;
      dDea := i.deactivation_dt;
      nDur := i.hot_country_max_duration;
      nHot := i.hot_country_cnt;
      dHot := i.hot_country_dt;
      vSta := i.status;
      nRoamer := 1;
      exit;
   end loop;
   p_activation  := dAct; 
   p_dectivation := dDea;
   p_duration    := nDur;
   p_hot_chk     := nHot;
   p_hot_dt      := dHot;
   p_status      := vSta;
   return nRoamer;   
end sf_is_roamer_info;
/


create or replace function sf_is_link_active (
   p_msisdn in number, 
   p_linkto in number 
   ) return number is
   nRoamer Number(2);
begin
   nRoamer := 0;
   select count(1)
   into   nRoamer
   from   sim_activation_links
   where  msisdn = p_msisdn
   and    link_to = p_linkto
   and    status <> 'INACTIVE';
   return nRoamer;
end sf_is_link_active;
/


create or replace procedure sp_process_ods (
   p_retr   out number, 
   p_msisdn in varchar2 ) is
   nMsisdn Number;
begin
   begin
      nMsisdn := to_number(p_msisdn);
      update sim_activation_links
      set    ods_last_chk_dt = trunc(sysdate)
      where  msisdn = nMsisdn;
      commit;
   exception
      when others then null;
   end;
   p_retr := 1;
end sp_process_ods;
/


create or replace procedure sp_process_for_expire_pending is
   dExpiredDate Date := sysdate; 
begin
   sp_logger('SP_PROCESS_FOR_EXPIRE_PENDING', 'START');
   for i in (select msisdn, cust_activation_dt, cust_deactivation_dt
             from   sim_activation_links 
             where  status ='PENDING' 
             and    expiry_date < dExpiredDate )
   loop
      begin 
         INSERT INTO BROADCAST_LOG ( ID, MSISDN, MESSAGE_ID, TX_DATE, STATUS, CLUSTER_NODE )
         VALUES (broadcast_log_seq.nextval, i.msisdn, 'LINK_REQUEST_EXPIRED', trunc(sysdate), 0, 1);
         delete sim_activation_links
         where  msisdn = i.msisdn 
         and    status = 'PENDING';
         commit;
      exception 
         when dup_val_on_index then null;
         when others then null;
      end;
   end loop;
   sp_logger('SP_PROCESS_FOR_EXPIRE_PENDING', 'END');

end sp_process_for_expire_pending;
/
