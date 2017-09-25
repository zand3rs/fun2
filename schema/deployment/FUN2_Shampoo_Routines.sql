CREATE OR REPLACE PROCEDURE trigger_owner.sp_process_shampoo (
  p_retr out number,
  p_type in varchar2,
  p_msisdn in varchar2,
  p_plan in varchar2,
  p_start in varchar2,
  p_end in varchar2 ) is
  -- Possible Retr 
  -- 1 - successfull
  -- 100 - invalid msisdn
  -- 101 - invalid plan
  -- 102 - invalid start date
  -- 103 - invalid end date
  -- 104 - already subscribe
  -- 110 - no subscription
  nRetr Number;
  dStart  Date;
  dEnd    Date;

begin

   sp_logger('PROCESS_SHAMPOO' , 'START =>' ||
                             ' p_type:'     || p_type ||
                             ' p_msisdn:'   || p_msisdn ||
                             ' p_plan:'     || p_plan ||
                             ' p_start:'    || p_start ||
                             ' p_end:'      || p_end );

   nRetr := 1;

   if p_type = 'SUBSCRIBE' then
      if (p_msisdn is null) then
         nRetr := 100;
      elsif (p_msisdn not like '63%') then
         nRetr := 100;
      elsif (length(p_msisdn) <> 12) then
         nRetr := 100;
      end if;

      if (nRetr=1) and (p_plan is null) then
         nRetr := 101;
      end if;

      if (nRetr=1) and (p_start is not null) then
         begin
            select to_date(p_start,'YYYYMMDDHH24MISS')
            into   dStart
            from   dual;
         exception 
            when others then 
               nRetr := 102;
         end;
      end if;

      if (nRetr=1) and (p_end is not null) then
         begin
            select to_date(p_end,'YYYYMMDDHH24MISS')
            into   dEnd
            from   dual;
         exception 
            when others then 
               nRetr := 103;
         end;
      end if;

      if (nRetr=1) then
         begin
            insert into shampoo_log (
                    id, msisdn, plan, start_date, end_date, dt_created, created_by)
            values (shampoo_seq.nextval, p_msisdn, p_plan, dStart, dEnd, sysdate, user );
            commit;
         exception 
            when dup_val_on_index then nRetr := 104;
         end;
      end if;
   elsif p_type = 'UNSUBSCRIBE' then 
      delete from shampoo_log where msisdn = p_msisdn and plan = p_plan;
      if sql%notfound then
         nRetr := 110;
         rollback;
      else
         commit;
      end if;
   end if;
   p_retr := nRetr;
   sp_logger('PROCESS_SHAMPOO' , 'END => nRetr:' || nRetr || ' p_retr:' || p_retr);
end sp_process_shampoo;
/
show err




CREATE OR REPLACE FUNCTION trigger_owner.is_shampoo_filtered (p_msisdn in varchar2) return number is
  -- returns 0 no shampoo supression
  -- returns 113 with shampoo supression
  nShampoo Number;
  nRetr Number;
begin
   nRetr := 113;

   select 1
   into   nShampoo
   from   shampoo_log
   where  msisdn = p_msisdn
   and    rownum <= 1;

   return nRetr;
exception
   when no_data_found then return 0;
   when others then return 0;
end is_shampoo_filtered;
/
show err




