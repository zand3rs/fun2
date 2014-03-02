create or replace function sf_is_blacklisted (p_msisdn in number) return number is
   nBlacklisted Number := 0;
begin
   select 1
   into   nBlacklisted
   from   BLACKLISTED_LOG
   where  msisdn = p_msisdn;
   return nBlacklisted;
exception
   when no_data_found then return 0;
   when too_many_rows then return 0;
   when others then return 0;
end sf_is_blacklisted;
/
