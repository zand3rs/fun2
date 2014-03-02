alter table sim_activation_links add ods_last_chk_dt date;

create or replace procedure sp_process_ods (
   p_retr   out number,
   p_msisdn in varchar2 ) is

   -- possible retr
   --          1 - successful
   nRetr   Number;
   nMsisdn Number;
begin
   nRetr := 1;
   begin
      nMsisdn := to_char(p_msisdn);
      update sim_activation_links
      set    ods_last_chk_dt = trunc(sysdate)
      where  msisdn = nMsisdn;
      commit;
   exception
      when others then
        nRetr := SQLCODE;
        p_retr := nRetr;
        return;
   end;
   p_retr := nRetr;
end sp_process_ods;
/
