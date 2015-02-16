PROMPT create FUNCTION "SF_CHECK_ROAMER_STATUS"...
PROMPT create FUNCTION "SF_CHECK_ROAMER_STATUS_IMSI"...
PROMPT create FUNCTION "SF_CHECK_USURF_STATUS"...
PROMPT create FUNCTION "SF_DISPLAY_PROCESS_STATUS"...
PROMPT create FUNCTION "SF_DISPLAY_PROCESS_STATUS_2"...
PROMPT create FUNCTION "SF_DISPLAY_TRAN_TYPE"...
PROMPT create FUNCTION "SF_GET_CUSTOMER_TYPE"...
PROMPT create FUNCTION "SF_GET_FUN_LINKS"...
PROMPT create FUNCTION "SF_GET_FUN_LINKS_LOCAL"...
PROMPT create FUNCTION "SF_GET_MBAL_BP_DTLS"...
PROMPT create FUNCTION "SF_GET_MIN_BAL"...
PROMPT create FUNCTION "SF_GET_PROCESS_SEQ" return varchar2 is
PROMPT create FUNCTION "SF_GET_REVERSE_PROCESS_SEQ" return varchar2 is
PROMPT create FUNCTION "SF_GET_TRAN_TYPE"...
PROMPT create FUNCTION "SF_GET_UNLI_TZ"...
PROMPT create FUNCTION "SF_IS_ACTIVE_FUN"...
PROMPT create FUNCTION "SF_IS_ACTIVE_ROAMER"...
PROMPT create FUNCTION "SF_IS_ACTIVE_ROAMER_O"...
PROMPT create FUNCTION "SF_IS_BLACKLISTED"...
PROMPT create FUNCTION "IS_CHIA_FILTERED"...
PROMPT create FUNCTION "SF_IS_IN_CUSTOMER_TYPE_RANGE"...
PROMPT create FUNCTION "SF_IS_IN_HLR_RANGE"...
PROMPT create FUNCTION "SF_IS_LINK_ACTIVE"...
PROMPT create FUNCTION "SF_IS_LINK_PENDING"...
PROMPT create FUNCTION "SF_IS_LOCAL_SIM_ACTIVE_LINK"...
PROMPT create FUNCTION "SF_IS_MAX_LINK_REACHED"...
PROMPT create FUNCTION "SF_IS_ROAMER_INFO"...
PROMPT create FUNCTION "SF_IS_ROAMER_INFO_IMSI"...
PROMPT create FUNCTION "SF_IS_VALID_ACTIVATION_DT"...
PROMPT create FUNCTION "SF_IS_VALID_GLOBE_NUMBER"...
PROMPT create FUNCTION "SF_IS_VALID_USURF"...
PROMPT create FUNCTION "SF_TRIGGER_HOUSEKEEPING"...
PROMPT create FUNCTION "SF_VALIDATE_EXT_DURATION"...
PROMPT create PROCEDURE "SP_GENERATE_UNLI_NOTIFICATIONS"...
PROMPT create PROCEDURE "SP_GET_RADCOM_FILE_FORMAT"...
PROMPT create PROCEDURE "SP_GET_USURF_STATUS"...
PROMPT create PROCEDURE "SP_INIT_TRAN"...
PROMPT create PROCEDURE "SP_LOGGER"...
PROMPT create PROCEDURE "SP_PROCESS_DAILY_BALANCE"...
PROMPT create PROCEDURE "SP_PROCESS_DEFAULT_UNLI"...
PROMPT create PROCEDURE "SP_PROCESS_FOR_ARDS_EXPIRY"...
PROMPT create PROCEDURE "SP_PROCESS_FOR_CANCEL"...
PROMPT create PROCEDURE "SP_PROCESS_FOR_DAILY_BAL_CHK"...
PROMPT create PROCEDURE "SP_PROCESS_FOR_DEACTIVATION"...
PROMPT create PROCEDURE "SP_PROCESS_FOR_DEACTIVATION_M"...
PROMPT create PROCEDURE "SP_PROCESS_FOR_EXPIRE_PENDING"...
PROMPT create PROCEDURE "SP_PROCESS_GEO_PROBE"...
PROMPT create PROCEDURE "SP_PROCESS_GEO_PROBE2"...
PROMPT create PROCEDURE "SP_PROCESS_ODS"...
PROMPT create PROCEDURE "SP_PROCESS_TRAN"...
PROMPT create PROCEDURE "SP_USURF_ACTIVATION"...
PROMPT create PROCEDURE "SP_USURF_DEACTIVATION"...
PROMPT create PROCEDURE "SP_USURF_DEACTIVATE_ALL"...


CREATE OR REPLACE FUNCTION "SF_CHECK_ROAMER_STATUS" (
   p_msisdn in number
   ) return number is
   nRoamer Number(1);
   nActivationPending  Number;
begin
   nRoamer := 0;
   for i in (select activation_dt, nvl(deactivation_dt, trunc(sysdate+1)) deactivation_dt, status
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
      exit;
   end loop;
   return nRoamer;
end sf_check_roamer_status;
/
show err

CREATE OR REPLACE FUNCTION "SF_CHECK_ROAMER_STATUS_IMSI" (
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
show err



CREATE OR REPLACE FUNCTION "SF_CHECK_USURF_STATUS" (
   p_msisdn in number,
   p_country in varchar2
   ) return number is
   nUsurfer Number(1);
   nActivationPending  Number;
   vStatus usurf_activation.status%type;
begin
   nUsurfer := 0;
   select status
   into   vStatus
   from   usurf_activation
   where  msisdn = p_msisdn
   and    country =  p_country;
   if vStatus = 'ACTIVE' then
      nUsurfer := 1;
   elsif vStatus = 'PENDING' then
      nUsurfer := 2;
   end if;
   return nUsurfer;
exception
   when no_data_found then return 0;
end sf_check_usurf_status;
/
show err



CREATE OR REPLACE FUNCTION "SF_DISPLAY_PROCESS_STATUS" (
   p_process    in varchar2,
   p_step_no    in number,
   p_last_no    in number,
   p_status     in number,
   p_err_code   in number,
   p_delimiter  in varchar2) return varchar2 is
   vSteps   Varchar2(256);
   vError   Varchar2(256);
   nProcess Number;
   bFailed  Boolean;
   bFailedStep  Boolean;
   bReversed Boolean;
begin
   -- LAST STEP STATUS
   --    0    0      x
   --    0   -1   -1 or 2
   if (p_last_no = 0 and p_step_no = 0 and p_status =  3) then
      return 'Pending';
   end if;

   vSteps := p_process;
   nProcess := to_number(substr(p_process,length(p_process)));
   bReversed := FALSE;
   if (nProcess = 1) then
      nProcess := to_number(substr(p_process,instr(p_process, ':')+1,1));
      bReversed := TRUE;
   end if;

   if (p_last_no = 0 and p_step_no = 1 and p_status = -1) or
      (p_last_no = 0 and p_step_no = -1) or
      (p_last_no = 1 and p_step_no = -1 and bReversed) or
      (p_last_no = 4 and p_step_no = -1 and (not bReversed))
   then
      if p_status = 2 then
         return 'Success';
      else
         --  For All tran type
         --    100 - Blacklisted
         --    101 - Invalid Duration
         --    102 - no active roaming
         --    103 - no active fun profile
         --    104 - Invalid Activation Date Format
         --    105 - Invalid Pre-Activation past 7 days
         --    106 - Duplicate Pre-Activation
         --    107 - Reached Max Num Link
         --    108 - Insuff balance
         --    109 - already active roamer
         --    110 - Reg and Link Error - Local SIM is already used as OFW SIM
         --    111 - Invalid Duration for GROAM EXTEND
         --    112 - Reg and Link Error - Link Already exists
         --    113 - for GROAM ON - Not in HLR Range
         --    114 - Reg and Link - Local SIM already link
         if p_err_code = 100 then
            vError := 'Blacklisted';
         elsif p_err_code = 101 then
            vError := 'Invalid Duration for GROAM ON';
         elsif p_err_code = 102 then
            vError := 'No Active Roaming';
         elsif p_err_code = 103 then
            vError := 'No Active FUN Profile';
         elsif p_err_code = 104 then
            vError := 'Invalid Activation Date Format';
         elsif p_err_code = 105 then
            vError := 'Invalid Pre-Activation Days';
         elsif p_err_code = 106 then
            vError := 'Duplicate Pre-Activation';
         elsif p_err_code = 107 then
            vError := 'Exceeds Maximum Number of Link';
         elsif p_err_code = 108 then
            vError := 'Insufficient Balance';
         elsif p_err_code = 109 then
            vError := 'Already Active Roamer';
         elsif p_err_code = 110 then
            vError := 'Local SIM is already used as OFW SIM';
         elsif p_err_code = 111 then
            vError := 'Invalid Duration for GROAM EXTEND';
         elsif p_err_code = 112 then
            vError := 'Local SIM is already used as OFW SIM';
         elsif p_err_code = 113 then
            vError := 'Not in HLR Range';
         elsif p_err_code = 114 then
            vError := 'Local SIM already link';
         elsif p_err_code = 119 then
            vError := 'Within Breathing Period';
         elsif p_err_code = 140 then
            vError := 'With open ended roaming';
         elsif p_err_code = 141 then
            vError := 'No pending link';
         elsif p_err_code = 142 then
            vError := 'Invalid Syntax or Invalid Match but with existing FUN subscription';
         elsif p_err_code = 143 then
            vError := 'REG - OFW SIM already link as Local SIM';
         elsif p_err_code = 144 then
            vError := 'Invalid keyword';
         end if;

         return 'Failed: ' || vError;
      end if;
   else
      if nProcess <> p_last_no then
         bFailed := TRUE;
      else
         nProcess := p_last_no;
         bFailed := FALSE;
      end if;

      bFailedStep := FALSE;
      if bReversed then
         for i in reverse 1..nProcess loop
            if p_last_no = i then
              if p_status = -1 then
                bFailedStep := TRUE;
                select replace(vSteps, to_char(i), 'Failed') into vSteps from dual;
              elsif p_status = 0 then
                select replace(vSteps, to_char(i), 'Pending') into vSteps from dual;
              elsif p_status = 1 then
                select replace(vSteps, to_char(i), 'For Processing') into vSteps from dual;
              elsif p_status = 2 then
                select replace(vSteps, to_char(i), 'Success') into vSteps from dual;
              end if;
            elsif p_last_no < i then
               select replace(vSteps, to_char(i), 'Success') into vSteps from dual;
            else
                if bFailedStep then
                   select replace(vSteps, to_char(i), 'Stop') into vSteps from dual;
                else
                   select replace(vSteps, to_char(i), 'Pending') into vSteps from dual;
                end if;
            end if;
         end loop;
      else
         for i in 1..nProcess loop
            if p_last_no = i then
              if p_status = -1 then
                bFailedStep := TRUE;
                select replace(vSteps, to_char(i), 'Failed') into vSteps from dual;
              elsif p_status = 0 then
                select replace(vSteps, to_char(i), 'Pending') into vSteps from dual;
              elsif p_status = 1 then
                select replace(vSteps, to_char(i), 'For Processing') into vSteps from dual;
              elsif p_status = 2 then
                select replace(vSteps, to_char(i), 'Success') into vSteps from dual;
              end if;
            elsif p_last_no > i then
               select replace(vSteps, to_char(i), 'Success') into vSteps from dual;
            else
                if bFailedStep then
                   select replace(vSteps, to_char(i), 'Stop') into vSteps from dual;
                else
                   select replace(vSteps, to_char(i), 'Pending') into vSteps from dual;
                end if;
            end if;
         end loop;
      end if;

      return replace(vSteps,'^', p_delimiter);

   end if;

exception
   when others then return p_process;
end SF_DISPLAY_PROCESS_STATUS;
/
show err



CREATE OR REPLACE FUNCTION "SF_DISPLAY_PROCESS_STATUS_2" (
   p_process    in varchar2,
   p_step_no    in number,
   p_last_no    in number,
   p_status     in number,
   p_err_code   in number,
   p_delimiter  in varchar2) return varchar2 is
   vSteps   Varchar2(256);
   vError   Varchar2(256);
   nProcess Number;
   bFailed  Boolean;
   bFailedStep  Boolean;
   bReversed Boolean;
begin
   -- LAST STEP STATUS
   --    0    0      x
   --    0   -1   -1 or 2
   if (p_last_no = 0 and p_step_no = 0 and p_status =  3) then
      return 'Pending';
   end if;

   vSteps := p_process;
   nProcess := to_number(substr(p_process,length(p_process)));
   bReversed := FALSE;
   if (nProcess = 1) then
      nProcess := to_number(substr(p_process,instr(p_process, ':')+1,1));
      bReversed := TRUE;
   end if;

   if (p_last_no = 0 and p_step_no = 1 and p_status = -1) or
      (p_last_no = 0 and p_step_no = -1) or
      (p_last_no = 1 and p_step_no = -1 and bReversed) or
      (p_last_no = 4 and p_step_no = -1 and (not bReversed))
   then
      if p_status = 2 then
         return 'Success';
      else
         --  For All tran type
         --    100 - Blacklisted
         --    101 - Invalid Duration
         --    102 - no active roaming
         --    103 - no active fun profile
         --    104 - Invalid Activation Date Format
         --    105 - Invalid Pre-Activation past 7 days
         --    106 - Duplicate Pre-Activation
         --    107 - Reached Max Num Link
         --    108 - Insuff balance
         --    109 - already active roamer
         --    110 - Reg and Link Error - Local SIM is already used as OFW SIM
         --    111 - Invalid Duration for GROAM EXTEND
         --    112 - Reg and Link Error - Link Already exists
         --    113 - for GROAM ON - Not in HLR Range
         --    114 - Reg and Link - Local SIM already link
         if p_err_code = 100 then
            vError := 'Blacklisted';
         elsif p_err_code = 101 then
            vError := 'Invalid Duration for GROAM ON';
         elsif p_err_code = 102 then
            vError := 'No Active Roaming';
         elsif p_err_code = 103 then
            vError := 'No Active FUN Profile';
         elsif p_err_code = 104 then
            vError := 'Invalid Activation Date Format';
         elsif p_err_code = 105 then
            vError := 'Invalid Pre-Activation Days';
         elsif p_err_code = 106 then
            vError := 'Duplicate Pre-Activation';
         elsif p_err_code = 107 then
            vError := 'Exceeds Maximum Number of Link';
         elsif p_err_code = 108 then
            vError := 'Insufficient Balance';
         elsif p_err_code = 109 then
            vError := 'Already Active Roamer';
         elsif p_err_code = 110 then
            vError := 'Local SIM is already used as OFW SIM';
         elsif p_err_code = 111 then
            vError := 'Invalid Duration for GROAM EXTEND';
         elsif p_err_code = 112 then
            vError := 'Local SIM is already used as OFW SIM';
         elsif p_err_code = 113 then
            vError := 'Not in HLR Range';
         elsif p_err_code = 114 then
            vError := 'Local SIM already link';
         elsif p_err_code = 119 then
            vError := 'Within Breathing Period';
         elsif p_err_code = 140 then
            vError := 'With open ended roaming';
         elsif p_err_code = 141 then
            vError := 'No pending link';
         elsif p_err_code = 142 then
            vError := 'Invalid Syntax or Invalid Match but with existing FUN subscription';
         elsif p_err_code = 143 then
            vError := 'REG - OFW SIM already link as Local SIM';
         elsif p_err_code = 144 then
            vError := 'Invalid keyword';
         end if;

         return 'Failed: ' || vError;
      end if;
   else
      if nProcess <> p_last_no then
         bFailed := TRUE;
      else
         nProcess := p_last_no;
         bFailed := FALSE;
      end if;

      bFailedStep := FALSE;
      if bReversed then
         for i in reverse 1..nProcess loop
            if p_last_no = i then
              if p_status = -1 then
                bFailedStep := TRUE;
                select replace(vSteps, to_char(i), 'Failed') into vSteps from dual;
              elsif p_status = 0 then
                select replace(vSteps, to_char(i), 'Pending') into vSteps from dual;
              elsif p_status = 1 then
                select replace(vSteps, to_char(i), 'For Processing') into vSteps from dual;
              elsif p_status = 2 then
                select replace(vSteps, to_char(i), 'Success') into vSteps from dual;
              end if;
            elsif p_last_no < i then
               select replace(vSteps, to_char(i), 'Success') into vSteps from dual;
            else
                if bFailedStep then
                   select replace(vSteps, to_char(i), 'Stop') into vSteps from dual;
                else
                   select replace(vSteps, to_char(i), 'Pending') into vSteps from dual;
                end if;
            end if;
         end loop;
      else
         for i in 1..nProcess loop
            if p_last_no = i then
              if p_status = -1 then
                bFailedStep := TRUE;
                select replace(vSteps, to_char(i), 'Failed') into vSteps from dual;
              elsif p_status = 0 then
                select replace(vSteps, to_char(i), 'Pending') into vSteps from dual;
              elsif p_status = 1 then
                select replace(vSteps, to_char(i), 'For Processing') into vSteps from dual;
              elsif p_status = 2 then
                select replace(vSteps, to_char(i), 'Success') into vSteps from dual;
              end if;
            elsif p_last_no > i then
               select replace(vSteps, to_char(i), 'Success') into vSteps from dual;
            else
                if bFailedStep then
                   select replace(vSteps, to_char(i), 'Stop') into vSteps from dual;
                else
                   select replace(vSteps, to_char(i), 'Pending') into vSteps from dual;
                end if;
            end if;
         end loop;
      end if;

      return replace(vSteps,'^', p_delimiter);

   end if;

exception
   when others then return p_process;
end SF_DISPLAY_PROCESS_STATUS_2;
/
show err



CREATE OR REPLACE FUNCTION "SF_DISPLAY_TRAN_TYPE" (
   p_tran_type in  number ) return varchar2 is
begin
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
   -- 12    TRAN_TYPE_GLIST_ZONE

   if p_tran_type = 7 then
      return 'GROAM HELP';
   elsif p_tran_type = 8 then
      return 'GROAM ON';
   elsif p_tran_type = 9  then
      return 'GROAM OFF';
   elsif p_tran_type = 10 then
      return 'GROAM EXTEND';
   elsif p_tran_type = 11 then
      return 'GROAM STATUS';
   elsif p_tran_type = 12 then
      return 'GLIST';
   elsif p_tran_type = 13 then
      return 'GLIST';
   elsif p_tran_type = 17 then
      return 'YES';
   elsif p_tran_type = 1 then
      return 'HELP';
   elsif p_tran_type = 2 then
      return 'FUN Registration';
   elsif p_tran_type = 3 then
      return 'FUN UNREG';
   elsif p_tran_type = 4 then
      return 'FUN LINK';
   elsif p_tran_type = 5 then
      return 'FUN CHECKLINK';
   elsif p_tran_type = 6 then
      return 'FUN CANCEL';
   else
      return 'UNKNOWN';
   end if;

end sf_display_tran_type;
/
show err



CREATE OR REPLACE FUNCTION "SF_GET_CUSTOMER_TYPE" (
   p_msisdn in number
   ) return varchar2 is
   vCustomerType Varchar2(16);
begin
   vCustomerType := 'TRIGGER';
   if sf_is_active_fun (p_msisdn) > 0 then
      vCustomerType := 'FUN';
   else
      for i in (select customer_type
                from   customer_types_mapping
                where  p_msisdn between msisdn_fr and msisdn_to
                and    status = 'ACTIVE'
                order  by rowid)
      loop
          vCustomerType:= i.customer_type;
          exit;
      end loop;
   end if;
   return vCustomerType;
end sf_get_customer_type;
/
show err



CREATE OR REPLACE FUNCTION "SF_GET_FUN_LINKS" (
   p_msisdn in number
   ) return varchar2 is
   vLinks Varchar2(128);
begin
   for i in (select link_to from sim_activation_links where msisdn = p_msisdn and status = 'ACTIVE') loop
     if vLinks is null then
        vLinks := to_char(i.link_to);
     else
        vLinks := vLinks || ',' || to_char(i.link_to);
     end if;
   end loop;
   return vLinks;
end SF_GET_FUN_LINKS;
/
show err



CREATE OR REPLACE FUNCTION "SF_GET_FUN_LINKS_LOCAL" (
   p_msisdn in number
   ) return varchar2 is
   vLinks Varchar2(128);
begin
   for i in (select msisdn from sim_activation_links where link_to = p_msisdn and status = 'ACTIVE') loop
     if vLinks is null then
        vLinks := to_char(i.msisdn);
     else
        vLinks := vLinks || ',' || to_char(i.msisdn);
     end if;
   end loop;
   return vLinks;
end SF_GET_FUN_LINKS_LOCAL;
/
show err



CREATE OR REPLACE FUNCTION "SF_GET_MBAL_BP_DTLS" (p_ref_id in number, p_mb_bp in number, p_tran_type in number, p_out_type in number) return number is
   nBreath    Number(4);
   nMinBal    Number(4);
begin
   if p_tran_type in (8,10) then
      return p_mb_bp;
   else
      if p_out_type = 0 then
         if p_ref_id > 0 then
            return p_mb_bp;
         else
           return 1;
         end if;
      else
         return 0;
      end if;
   end if;
   --if p_tran_type in (8,10) and p_ref_id > 0 then
   --   select min_bal, breathing_period
   --   into   nMinBal, nBreath
   --   from   manual_enrollment_log
   --   where  id = p_ref_id;
   --   if p_out_type = 0 then
   --      return nMinBal;
   --   else
   --      return nBreath;
   --   end if;
   --else
   --   if p_out_type = 0 then
   --      return 1;
   --   else
   --      return 0;
   --   end if;
   --end if;
exception
   when no_data_found then
      if p_out_type = 0 then
         return 1;
      else
         return 0;
      end if;
end sf_get_mbal_bp_dtls;
/
show err



CREATE OR REPLACE FUNCTION "SF_GET_MIN_BAL" (p_custtype in varchar2, p_tran_type in number, p_override in number) return number is
   nExtMinBal Number(4);
   nMinBal    Number(4);
begin
   if p_override = 1 then
      return 0;
   else
      select min_balance, ext_min_balance
      into   nMinBal, nExtMinBal
      from   customer_types
      where customer_type = p_custtype;
      if p_tran_type = 0 then
         return nMinBal;
      else
         return nExtMinBal;
      end if;
   end if;
exception
   when no_data_found then return 0;
end sf_get_min_bal;
/
show err



CREATE OR REPLACE FUNCTION "SF_GET_PROCESS_SEQ" return varchar2 is
   vSteps Varchar2(128);
begin
   for i in (select service_code, priority from services where status = 'ACTIVE' order by priority) loop
      if vSteps is null then
         vSteps := i.service_code || ':' || to_char(i.priority);
      else
         vSteps := vSteps || '^' || i.service_code || ':' || to_char(i.priority);
      end if;
   end loop;
   return nvl(vSteps,' ');
exception
   when others then return ' ';
end sf_get_process_seq;
/
show err



CREATE OR REPLACE FUNCTION "SF_GET_REVERSE_PROCESS_SEQ" return varchar2 is
   vSteps Varchar2(128);
begin
   for i in (select service_code, priority from services where status = 'ACTIVE' order by priority desc) loop
      if vSteps is null then
         vSteps := i.service_code || ':' || to_char(i.priority);
      else
         vSteps := vSteps || '^' || i.service_code || ':' || to_char(i.priority);
      end if;
   end loop;
   return nvl(vSteps,' ');
exception
   when others then return ' ';
end sf_get_reverse_process_seq;
/
show err



CREATE OR REPLACE FUNCTION "SF_GET_TRAN_TYPE" (
   p_enrollment_type in  varchar2 ) return number is
begin
   if p_enrollment_type = 'ACTIVATION' then
      return 8;
   elsif p_enrollment_type = 'DEACTIVATION' then
      return 9;
   elsif p_enrollment_type = 'EXTENSION' then
      return 10;
   elsif p_enrollment_type = 'LINK' then
      return 4;
   elsif p_enrollment_type = 'CANCEL' then
      return 6;
   else
      return 0;
   end if;
end sf_get_tran_type;
/
show err



CREATE OR REPLACE FUNCTION "SF_GET_UNLI_TZ" (p_tz_1 in number, p_tz_2 in number) return number as
  nHours Number;
begin
   if p_tz_2 > p_tz_1 then
      nHours := (p_tz_2 - p_tz_1) * -1;
   else
      if p_tz_2 < 0 then
         nHours := (abs(p_tz_2) + p_tz_1);
      else
         nHours := (p_tz_1 - p_tz_2);
      end if;
   end if;
   return (nHours/24);
end sf_get_unli_tz;
/
show err




CREATE OR REPLACE FUNCTION "SF_IS_ACTIVE_FUN" (
   p_msisdn in number
   ) return number is
   nRoamer Number(2);
begin
   nRoamer := 0;
   select count(1)
   into   nRoamer
   from   sim_activation_links
   where  msisdn = p_msisdn
   and    status = 'ACTIVE';
   return nRoamer;
end sf_is_active_fun;
/
show err



CREATE OR REPLACE FUNCTION "SF_IS_ACTIVE_ROAMER" (
   p_msisdn in number
   ) return number is
   nRoamer Number(1);
begin
   nRoamer := 0;
   for i in (select activation_dt, deactivation_dt
             from   sim_activation
             where  msisdn = p_msisdn
             and    status='ACTIVE')
   loop
      nRoamer := 1;
      exit;
   end loop;
   return nRoamer;
end sf_is_active_roamer;
/
show err



CREATE OR REPLACE FUNCTION "SF_IS_ACTIVE_ROAMER_O" (
   p_msisdn in number,
   p_activation out date,
   p_dectivation out date,
   p_duration out number,
   p_hot_chk out number,
   p_hot_dt out date
   ) return number is
   nRoamer Number(1);
   dAct Date;
   dDea Date;
   nDur Number;
   nHot Number;
   dHot Date;
begin
   nRoamer := 0;
   for i in (select deact_basis_dt, deactivation_dt, hot_country_cnt, nvl(hot_country_max_duration,0) hot_country_max_duration, hot_country_dt
             from   sim_activation
             where  msisdn = p_msisdn)
   loop
      dAct := i.deact_basis_dt;
      dDea := i.deactivation_dt;
      nDur := i.hot_country_max_duration;
      nHot := i.hot_country_cnt;
      dHot := i.hot_country_dt;
      nRoamer := 1;
      exit;
   end loop;
   p_activation  := dAct;
   p_dectivation := dDea;
   p_duration    := nDur;
   p_hot_chk     := nHot;
   p_hot_dt      := dHot;
   return nRoamer;
end sf_is_active_roamer_o;
/
show err




CREATE OR REPLACE FUNCTION "SF_IS_BLACKLISTED" (p_msisdn in number) return number is
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
show err



CREATE OR REPLACE FUNCTION "IS_CHIA_FILTERED" (p_msisdn in number) return number is
  vChk Number;
  -- returns 0 not filtered
  -- returns 1 filtered
begin
   select 1 into vChk
   from  chia_filtered_mins
   where p_msisdn between msisdn_fr and msisdn_to
   and   status = 'ACTIVE';
   return 1;
exception
   when no_data_found then return 0;
   when too_many_rows then return 1;
   when others then return 0;
end is_chia_filtered;
/
show err



CREATE OR REPLACE FUNCTION "SF_IS_IN_CUSTOMER_TYPE_RANGE" (
   p_cust   in varchar2,
   p_msisdn in number
   ) return number is
   nCnt Number(1);
begin
   nCnt  := 0;
   select count(1)
   into   nCnt
   from   customer_types_mapping
   where  p_msisdn between msisdn_fr and msisdn_to
   and    customer_type = p_cust
   and    status = 'ACTIVE';
   return nCnt ;
end sf_is_in_customer_type_range;
/
show err



CREATE OR REPLACE FUNCTION "SF_IS_IN_HLR_RANGE" (
   p_msisdn in number
   ) return number is
   nCnt Number(1);
begin
   nCnt  := 0;
   select count(1)
   into   nCnt
   from   hlr_server_mapping
   where  p_msisdn between msisdn_fr and msisdn_to
   and    status = 'ACTIVE';
   return nCnt ;
end sf_is_in_hlr_range;
/
show err



CREATE OR REPLACE FUNCTION "SF_IS_LINK_ACTIVE" (
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
show err



CREATE OR REPLACE FUNCTION "SF_IS_LINK_PENDING" (
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
show err



CREATE OR REPLACE FUNCTION "SF_IS_LOCAL_SIM_ACTIVE_LINK" (
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
show err



CREATE OR REPLACE FUNCTION "SF_IS_MAX_LINK_REACHED" (
   p_msisdn in number
   ) return number is
   nRoamer  Number(2);
   nMaxLink Number(4);
begin
   nRoamer := 0;

   select num_link into nMaxLink
   from   customer_types
   where  customer_type = 'FUN';

   select count(1)
   into   nRoamer
   from   sim_activation_links
   where  msisdn = p_msisdn
   and    status <> 'INACTIVE';

   if nMaxLink <= nRoamer then
      return 1;
   else
      return 0;
   end if;
exception
   when others then return 0;
end sf_is_max_link_reached;
/
show err



CREATE OR REPLACE FUNCTION "SF_IS_ROAMER_INFO" (
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
   for i in (select deact_basis_dt activation_dt, deactivation_dt, hot_country_cnt, nvl(hot_country_max_duration,0) hot_country_max_duration, hot_country_dt, status
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
show err



CREATE OR REPLACE FUNCTION "SF_IS_ROAMER_INFO_IMSI" (
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
show err



CREATE OR REPLACE FUNCTION "SF_IS_VALID_ACTIVATION_DT" (p_date in varchar2, p_profile in varchar2) return number is
   dDate Date;
   nCustPre_act_day Number;
   nRetr Number := 0;
begin
   begin
      dDate := to_date(p_date, 'YYYY-MM-DD');
   exception
      when others then
         nRetr := 0;
         return nRetr;
   end;

   if p_profile is not null then
      begin
         select nvl(pre_act_day,0)
         into   nCustPre_act_day
         from   customer_types
         where  customer_type = p_profile;
      exception
         when others then
            nCustPre_act_day := 0;
      end;
   else
      nCustPre_act_day := 0;
   end if;

   if (dDate > (trunc(sysdate)+nCustPre_act_day)) or (dDate < trunc(sysdate)) then
      nRetr := 0;
   else
      nRetr := 1;
   end if;
   return nRetr;
end sf_is_valid_activation_dt;
/
show err



CREATE OR REPLACE FUNCTION "SF_IS_VALID_GLOBE_NUMBER" (p_msisdn in number) return number is
   nValid Number := 0;
begin
   if p_msisdn between 639150000000 and 639179999999 then
      nValid := 1;
   end if;
   if p_msisdn between 639250000000 and 639279999999 then
      nValid := 1;
   end if;
   if p_msisdn between 639050000000 and 639079999999 then
      nValid := 1;
   end if;
   return nValid;
end sf_is_valid_globe_number;
/
show err



CREATE OR REPLACE FUNCTION "SF_IS_VALID_USURF" (p_country in varchar2, p_duration in number) return number is
   nRetr Number;
begin
   -- 1 - OK
   -- 2 - Invalid Country
   -- 3 - Invalid Duration
   nRetr := 2;
   begin
      select 1
      into   nRetr
      from   usurf_countries
      where  country = upper(p_country)
      and    status = 'ACTIVE';
      if p_duration not in (1,3,5) then
         nRetr := 3;
      end if; 
   exception
      when no_data_found then nRetr := 2;
      when others then nRetr := 2;
   end;
   return nRetr;
end sf_is_valid_usurf;
/
show err



CREATE OR REPLACE FUNCTION "SF_TRIGGER_HOUSEKEEPING" return varchar2 as
   vOpMsg       Varchar2(1024);
   vCheckHist   Number;
   vCheckTran   Number;
   dMaxTranDate Date := add_months(trunc(sysdate), -3);
   dMinTranDate Date;
begin

   -- REQUEST_LOG maintenance
   begin
      vCheckHist := 0;
      vCheckTran := 0;

      select min(TRAN_DT)
      into   dMinTranDate
      from   REQUEST_LOG
      where  TRAN_DT <= dMaxTranDate
      and    status > 0;

      if dMinTranDate is not null then
         insert into REQUEST_LOG_HISTORY
               (  ID
                , TRAN_DT
                , A_NO
                , B_NO
                , MSG
                , STEP_NO
                , LAST_STEP_NO
                , STATUS
                , DT_CREATED
                , DT_MODIFIED
                , TXID
                , TRAN_TYPE
                , CLUSTER_NODE
                , CUSTOMER_TYPE
                , REQUEST_ORIGIN
                , REF_ID
                , MIN_BAL
                , STEP_SEQ
                , ERROR_CODE
                , IMSI
                , DURATION
                , ACTIVATION_DATE
                , DEACTIVATION_DATE
                , GSM_NUM
                , RESULT_CODE
                , SILENT
               )
         select  ID
                , TRAN_DT
                , A_NO
                , B_NO
                , MSG
                , STEP_NO
                , LAST_STEP_NO
                , STATUS
                , DT_CREATED
                , DT_MODIFIED
                , TXID
                , TRAN_TYPE
                , CLUSTER_NODE
                , CUSTOMER_TYPE
                , REQUEST_ORIGIN
                , REF_ID
                , MIN_BAL
                , STEP_SEQ
                , ERROR_CODE
                , IMSI
                , DURATION
                , ACTIVATION_DATE
                , DEACTIVATION_DATE
                , GSM_NUM
                , RESULT_CODE
                , SILENT
         from   REQUEST_LOG
         where  TRAN_DT <= dMinTranDate
         and    status > 0;
         vCheckHist := sql%rowcount;
         delete from REQUEST_LOG where TRAN_DT <= dMinTranDate and status > 0;
         vCheckTran := sql%rowcount;
         if  vCheckTran = vCheckHist then
             commit;
         else
            rollback;
            vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on REQUEST_LOG maintenance - Transfer unsuccessful.';
         end if;
      end if;
   exception
      when others then
         vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on REQUEST_LOG maintenance. ORA' || to_char(SQLCODE) || '.';
   end;


   -- RESPONSE_LOG maintenance
   begin
      vCheckHist := 0;
      vCheckTran := 0;
      dMinTranDate := null;

      select min(TRAN_DT)
      into   dMinTranDate
      from   RESPONSE_LOG
      where  TRAN_DT <= dMaxTranDate
      and    status > 0;

      if dMinTranDate is not null then
         insert into RESPONSE_LOG_HISTORY
               (  ID
                , REF_ID
                , TRAN_DT
                , A_NO
                , B_NO
                , STATUS
                , DT_CREATED
                , DT_MODIFIED
                , TXID
                , MSG
                , CLUSTER_NODE
                , CUSTOMER_TYPE
                , MSG_ID
                , TRAN_TYPE
               )
         select   ID
                , REF_ID
                , TRAN_DT
                , A_NO
                , B_NO
                , STATUS
                , DT_CREATED
                , DT_MODIFIED
                , TXID
                , MSG
                , CLUSTER_NODE
                , CUSTOMER_TYPE
                , MSG_ID
                , TRAN_TYPE
         from   RESPONSE_LOG
         where  TRAN_DT <= dMinTranDate
         and    status > 0;
         vCheckHist := sql%rowcount;
         delete from RESPONSE_LOG where TRAN_DT <= dMinTranDate and status > 0;
         vCheckTran := sql%rowcount;
         if  vCheckTran = vCheckHist then
             commit;
         else
            rollback;
            vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on RESPONSE_LOG maintenance - Transfer unsuccessful.';
         end if;
      end if;
   exception
      when others then
         vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on RESPONSE_LOG maintenance. ORA' || to_char(SQLCODE) || '.';

   end;

    -- MANUAL_ENROLLMENT_LOG maintenance
   begin
      vCheckHist := 0;
      vCheckTran := 0;
      dMinTranDate := null;

      select min(TX_DATE)
      into   dMinTranDate
      from   MANUAL_ENROLLMENT_LOG
      where  TX_DATE <= dMaxTranDate
      and    status > 0;

      if dMinTranDate is not null then
         insert into MANUAL_ENROLLMENT_LOG_HISTORY
               (   ID
                 , MSISDN
                 , ENROLLMENT_TYPE
                 , BULK_UPLOAD
                 , TX_DATE
                 , STATUS
                 , CS_USERNAME
                 , CS_REASON
                 , DT_CREATED
                 , DT_MODIFIED
                 , SUBS_NAME
                 , SUBS_GROUP
                 , SUBS_CONTACT_NO
                 , REMARKS
                 , BULK_GRP_ID
                 , DURATION
                 , CLUSTER_NODE
                 , CUSTOMER_TYPE
                 , MIN_BAL
                 , BREATHING_PERIOD
                 , LINK_TO
                 , REQUEST_ORIGIN
                 , OTHER_TYPE
                 , SILENT
                 , IMSI
               )
         select    ID
                 , MSISDN
                 , ENROLLMENT_TYPE
                 , BULK_UPLOAD
                 , TX_DATE
                 , STATUS
                 , CS_USERNAME
                 , CS_REASON
                 , DT_CREATED
                 , DT_MODIFIED
                 , SUBS_NAME
                 , SUBS_GROUP
                 , SUBS_CONTACT_NO
                 , REMARKS
                 , BULK_GRP_ID
                 , DURATION
                 , CLUSTER_NODE
                 , CUSTOMER_TYPE
                 , MIN_BAL
                 , BREATHING_PERIOD
                 , LINK_TO
                 , REQUEST_ORIGIN
                 , OTHER_TYPE
                 , SILENT
                 , IMSI
         from   MANUAL_ENROLLMENT_LOG
         where  TX_DATE <= dMinTranDate
         and    status > 0;
         vCheckHist := sql%rowcount;
         delete from MANUAL_ENROLLMENT_LOG where TX_DATE <= dMinTranDate and status > 0;
         vCheckTran := sql%rowcount;
         if  vCheckTran = vCheckHist then
             commit;
         else
            rollback;
            vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on RESPONSE_LOG maintenance - Transfer unsuccessful.';
         end if;
      end if;
   exception
      when others then
         vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on RESPONSE_LOG maintenance. ORA' || to_char(SQLCODE) || '.';

   end;

    -- KEYWORDREQUEST maintenance
   begin
      vCheckHist := 0;
      vCheckTran := 0;
      dMinTranDate := null;

      select min(START_TM)
      into   dMinTranDate
      from   KEYWORDREQUEST
      where  START_TM <= dMaxTranDate;

      if dMinTranDate is not null then
         insert into KEYWORDREQUEST_HISTORY
               (   ID
                 , MSISDN
                 , IMSI
                 , ENROLLMENT_TYPE
                 , TX_DATE
                 , STATUS
                 , START_TM
                 , END_TM
                 , REF_ID
               )
         select    ID
                 , MSISDN
                 , IMSI
                 , ENROLLMENT_TYPE
                 , trunc(START_TM)
                 , STATUS
                 , START_TM
                 , END_TM
                 , REF_ID
         from   KEYWORDREQUEST
         where  START_TM <= trunc(dMinTranDate) + 1;
         vCheckHist := sql%rowcount;
         delete from KEYWORDREQUEST where START_TM <= trunc(dMinTranDate) + 1;
         vCheckTran := sql%rowcount;
         if  vCheckTran = vCheckHist then
             commit;
         else
            rollback;
            vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on KEYWORDREQUEST maintenance - Transfer unsuccessful.';
         end if;
      end if;
   exception
      when others then
         vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on KEYWORDREQUEST maintenance. ORA' || to_char(SQLCODE) || '.';
   end;

   begin
      execute immediate ('alter index TXRES_LOG_PK rebuild tablespace TRIGGER_INDX');
      execute immediate ('alter index RES_LOG_NODE_IDX rebuild tablespace TRIGGER_INDX');
   exception
      when others then
         vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on INDEX Rebuild of RESPONSE_LOG. ORA' || to_char(SQLCODE) || '.';
   end;

   begin
      execute immediate ('alter index MANUAL_ENROLLMENT_LOG_PK rebuild tablespace TRIGGER_INDX');
   exception
      when others then
         vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on INDEX Rebuild of MANUAL_ENROLLMENT_LOG. ORA' || to_char(SQLCODE) || '.';
   end;

   begin
      execute immediate ('alter index MANUAL_ENROLLMENT_LOG_PK   rebuild tablespace TRIGGER_INDX');
   exception
      when others then
         vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on INDEX Rebuild of MANUAL_ENROLLMENT_LOG. ORA' || to_char(SQLCODE) || '.';
   end;

   begin
      execute immediate ('alter index KEYWORDREQUEST_PK rebuild tablespace TRIGGER_INDX');
      execute immediate ('alter index KEYWORDREQUEST_END_TM_IDX rebuild tablespace TRIGGER_INDX');
      execute immediate ('alter index KEYWORDREQUEST_START_TM_IDX rebuild tablespace TRIGGER_INDX');
   exception
      when others then
         vOpMsg   := nvl(vOpMsg,'') || chr(10) || 'ERROR on INDEX Rebuild of KEYWORDREQUEST. ORA' || to_char(SQLCODE) || '.';
   end;
   commit;

   delete from ARDSREQUEST where START_TM < trunc(sysdate) and status > 1;
   commit;
   delete from ARDSRESPONSE where START_TM < trunc(sysdate) and status > 1;
   commit;
   delete from ARDSREQUEST where START_TM < trunc(sysdate)-3;
   commit;
   delete from ARDSRESPONSE where START_TM < trunc(sysdate)-3;
   commit;
   delete from CUSTOMER_TYPES_MAPPING_ERR where dt_created < trunc(sysdate);
   commit;
   delete from BLACKLISTED_ERR_LOG where tran_dt < trunc(sysdate);
   commit;
   delete from WHITELISTED_ERR_LOG where tran_dt < trunc(sysdate);
   commit;
   delete from BULK_ACTIVATION_ERR_LOG where tx_date < trunc(sysdate);
   commit;
   delete from BULK_DEACTIVATION_ERR_LOG where tx_date < trunc(sysdate);
   commit;

   dMinTranDate := null;
   select min(dt_created)
   into   dMinTranDate
   from   SYSTEM_LOG
   where  dt_created <= trunc(sysdate);

   if (trunc(dMinTranDate)+3) < trunc(sysdate)-3 then
      delete from SYSTEM_LOG where dt_created < (trunc(dMinTranDate)+3);
      commit;
   else
      delete from SYSTEM_LOG where dt_created < trunc(sysdate)-3;
      commit;
   end if;

   if vOpMsg is null then
      vOpMsg   := 'Housekeep Successfull - for the day of ' || to_char(dMaxTranDate,'YYYYMMDD');
   else
      vOpMsg   := 'Housekeep with errors.' || chr(10) || vOpMsg;
   end if;

   return vOpMsg;

end sf_trigger_housekeeping;
/
show err



CREATE OR REPLACE FUNCTION "SF_VALIDATE_EXT_DURATION" (
   p_msisdn in number,
   p_duration in varchar2
   ) return number is
   nValid Number(1);
   nDur Number;
   pDur Number;
begin
   nValid := 0;
   if p_duration is not null then
      begin
         pDur := to_number(p_duration);
     exception
         when others then
            nValid := 0;
            return nValid;
     end;
   else
      pDur := 0;
   end if;

   for i in (select activation_dt, deactivation_dt, nvl(hot_country_cnt,0) hot_country_cnt, customer_type
             from   sim_activation
             where  msisdn = p_msisdn
             and    status = 'ACTIVE')
   loop
      if i.hot_country_cnt > 0 then
         select max_duration
         into   nDur
         from   customer_types
         where  customer_type = i.customer_type;
         if (nDur >= pDur) and (pDur>0) then
            nValid := 1;
         elsif (pDur=0) then
            nValid := 0;
         end if;
      else
         nValid := 1;
      end if;
      exit;
   end loop;
   return nValid;
end SF_VALIDATE_EXT_DURATION;
/
show err



CREATE OR REPLACE PROCEDURE "SP_GENERATE_UNLI_NOTIFICATIONS" is
  vMsg_Pre Varchar2(2000);
  vMsg_Exp Varchar2(2000);
  vPreCounter   Number := 0;
  vNoPreCounter Number := 0;
  vExpCounter   Number := 0;
  vNoExpCounter Number := 0;
begin
   -- generate pre expiry notifications
   for i in (select e.msisdn, (e.original_dt-sf_get_unli_tz(0, p.tz))+(e.expiry_tm-e.tran_tm) expiry_tm, e.mcc, p.tz, e.original_dt
             from   default_unli_exp e, roaming_partners p 
             where  e.mnc = p.mnc
             and    e.mcc = p.mcc
             and    e.status <= 1
             and    e.pre_expiry_dt >= trunc(sysdate)-1  
             and    e.pre_expiry_tm < sysdate  
             and    e.expiry_tm > sysdate)
   loop
      begin
         select msg
         into   vMsg_Pre
         from   mcc_messages
         where  mcc = i.mcc
         and    message_type = '3';
         begin
            insert into broadcast_log ( id, msisdn, message_id, tx_date, status, cluster_node, msg )
            values (broadcast_log_seq.nextval, i.msisdn, '3', trunc(sysdate), 0, 1, replace(vMsg_Pre, '<EXPIRY TIME>', to_char(i.expiry_tm, 'HH24:MI:SS')) );
            commit;
            vPreCounter := vPreCounter + 1;
         exception
            when dup_val_on_index then null;
         end;
      exception 
         when no_data_found then 
            vMsg_Pre := null;
            vNoPreCounter := vNoPreCounter + 1;
      end;
      update default_unli_exp set status=2 where msisdn=i.msisdn and mcc=i.mcc;
   end loop;
   commit;

   -- generate expiry notifications
   for i in (select e.msisdn, (e.original_dt-sf_get_unli_tz(0, p.tz))+(e.expiry_tm-e.tran_tm) expiry_tm, e.mcc, p.tz, e.original_dt
             from   default_unli_exp e, roaming_partners p 
             -- where  status > 0
             where  e.mnc = p.mnc
             and    e.mcc = p.mcc
             and    e.expiry_dt >= trunc(sysdate)-1 
             and    e.expiry_tm < sysdate)
   loop
      begin
         select msg
         into   vMsg_Exp
         from   mcc_messages
         where  mcc = i.mcc
         and    message_type = '4';
         begin
            -- create notifications
            insert into broadcast_log ( id, msisdn, message_id, tx_date, status, cluster_node, msg )
            values (broadcast_log_seq.nextval, i.msisdn, '4', trunc(sysdate), 0, 1, replace(vMsg_Exp, '<EXPIRY DATE AND TIME>', to_char(i.expiry_tm, 'DD-MON-YYYY HH24:MI:SS')) );
            vExpCounter := vExpCounter + 1;
         exception
            when dup_val_on_index then 
               vMsg_Pre := null;
         end;
      exception 
         when no_data_found then 
            vMsg_Exp := null;
            vNoExpCounter := vNoExpCounter + 1;
      end;
      -- create history log
      begin
         insert into default_unli_hist 
               (id, msisdn, tran_dt, tran_tm, mnc, mcc, sgsn_ip, status, pre_expiry_dt, pre_expiry_tm, 
                expiry_dt, expiry_tm, original_dt, file_name, dt_created, dt_deleted)
         select a.id, a.msisdn, a.tran_dt, a.tran_tm, b.mnc, b.mcc, a.sgsn_ip, a.status, b.pre_expiry_dt, b.pre_expiry_tm, 
                b.expiry_dt, b.expiry_tm, b.original_dt, a.file_name, a.dt_created, sysdate 
         from   default_unli_log  a, default_unli_exp b
         where a.msisdn=i.msisdn
         and   a.mcc = i.mcc
         and   a.msisdn=b.msisdn
         and   a.mcc=b.mcc;
      exception 
         when dup_val_on_index then null;
      end;
      -- clear transaction log
      delete from default_unli_log where msisdn=i.msisdn and mcc = i.mcc;
      delete from default_unli_exp where msisdn=i.msisdn and mcc = i.mcc;
      commit;
   end loop;

   -- delete from default_unli_log where status = 0 and expiry_date = trunc(sysdate)-1;
   -- delete from default_unli_exp where status = 0 and expiry_dt = trunc(sysdate)-1;
   delete from default_unli_log where expiry_date < trunc(sysdate)-2;
   commit;
   delete from default_unli_exp where expiry_dt < trunc(sysdate)-2;
   commit;
   dbms_output.put_line (chr(10));
   dbms_output.put_line (to_char(sysdate, 'YYYY-MM-DD HH24:MI:SS'));
   dbms_output.put_line ('No. of Pre Expiry Message Sent:' || to_char(vPreCounter));
   dbms_output.put_line ('No. of Pre Expiry with No Message to Sent:' || to_char(vNoPreCounter));
   dbms_output.put_line ('No. of Expiry Message Sent:' || to_char(vExpCounter));
   dbms_output.put_line ('No. of Expiry with No Message to Sent:' || to_char(vNoExpCounter));
end sp_generate_unli_notifications;
/
show err





CREATE OR REPLACE PROCEDURE "SP_GET_RADCOM_FILE_FORMAT" (
  p_retr out number,
  p_filename in varchar2 ) is
  --   1 - Success - Not yet processed
  --   110 - Error - Already processed
  nRetr    Number;
  nCheck   Number;
  dFormat  Date;
begin
  nRetr := 1;

  if substr (p_filename, 18, 2) <> 'Gi' then 
     nRetr := 110;
  else
     begin
        select to_date(substr(p_filename,1,16),'YYYY_MM_DD_HH24_MI')
        into   dFormat
        from   dual;

        if dFormat > sysdate then
           nRetr := 113;
        end if;
     exception 
        when others then 
           nRetr := 111;
     end;
  end if;

  if nRetr = 1 then
     begin
        select 1
        into   nCheck
        from   radcom_downloaded_files
        where  file_name = p_filename;
        nRetr := 112;
     exception 
        when no_data_found then null;
     end;
  end if;
  -- set return parameters
  p_retr     := nRetr;
  --p_filename := to_char(trunc(sysdate), 'YYYY_MM_DD_HH24_MI"_Gi*"');
end sp_get_radcom_file_format;
/
show err




CREATE OR REPLACE PROCEDURE "SP_GET_USURF_STATUS"(
   p_retr    out number, 
   p_partner out varchar2,
   p_exptime out varchar2,
   p_expdate out varchar2,
   p_msisdn in  varchar2) as
   nRetr Number;
   -- possible out p_retr
   --    0 - No Subscription
   --    1 - With active Subscription
   --    2 - With Pending activation   
   vPartner Varchar2(30);
   vCountry Varchar2(30);
   vTz      Varchar2(10);
   vStatus  Varchar2(30);
   dActivation Date;
   nDuration Number;
begin
   -- nRetr := sf_check_usurf_status(p_msisdn);
   begin
      select  a.activation_dt, a.country, b.roaming_partner, b.tz, a.status, a.denom
      into    dActivation, vCountry, vPartner, vTz, vStatus, nDuration
      from    usurf_activation a, usurf_countries b
      where   a.country = b.country
      and     a.status = 'ACTIVE'
      and     a.msisdn = p_msisdn
      and     rownum = 1;
      if vStatus = 'ACTIVE' then
         nRetr := 1;
      elsif vStatus = 'PENDING' then
         nRetr := 2;
      else
         nRetr := 0;
      end if;
   exception
      when no_data_found then
         nRetr := 0;
   end;
   p_partner := '';
   p_exptime := nvl(to_char(dActivation+nDuration, 'HH24:MI') || ' ' || vTz, '');
   p_expdate := nvl(to_char(trunc(dActivation+nDuration), 'MM-DD-YYYY'), '');
   p_retr := nvl(nRetr,0);
end sp_get_usurf_status;
/
show err




CREATE OR REPLACE PROCEDURE "SP_INIT_TRAN" (
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
    p_extra_i_3 in  varchar2,
    p_extra_i_4 in  varchar2
   ) is
   nRetr                Number;
   bRoamer              Boolean;
   nMinBal              Number;
   nMinBal_Web          Number;
   nDuration            Number;
   nCustMinBal          Number;
   nCustDuration        Number;
   nCustBreathingPeriod Number;
   nCustPre_Act_Day     Number;
   nCustNum_Link        Number;
   dActivation          Date;
   dDeactivation        Date;
   vProfile             Varchar2(16);
   nPreActPending       Number;
   nDefaultDuration     Number := 180;
   nDefaultPreAct       Number := 7;
   nMinBalExt           Number;
   dSubsActDt           Date;
   dSubsDeactDt         Date;
   nSubsInHot           Number;
   dSubsHotDt           Date;
   nSubsHotDur          Number;
   nBreathing_period    Number;
   nDefaultMaxDuration  Number := 9999;
   nRoamerStatus        Number;
   vStatus              Varchar2(12);
   vBlackReason         Varchar2(128);
   vActiveLinks         Varchar2(128);
   nLinkTo              Number;
   vOtherType           Varchar2(30);
   vIMSI                Varchar2(30);
   -- Bluemoon
   nUsurfStatus         Varchar2(30);
   vPartner             Varchar2(30);
   vExptime             Varchar2(30);
   vExpdate             Varchar2(30);
begin
   --  For All tran type
   --    100 - Blacklisted
   --    101 - Invalid Duration
   --    102 - no active roaming
   --    103 - no active fun profile
   --    104 - Invalid Activation Date Format
   --    105 - Invalid Pre-Activation past 7 days
   --    106 - Duplicate Pre-Activation
   --    107 - Reached Max Num Link
   --    108 - Insuff balance
   --    109 - already active roamer
   --    110 - Reg and Link Error - Local SIM is already used as OFW SIM
   --    111 - Invalid Duration for GROAM EXTEND
   --    112 - Reg and Link Error - Link Already exists
   --    113 - for GROAM ON - Not in HLR Range
   --    114 - Reg and Link - Local SIM already link
   --    115 - REG - Not in customer type mapping or not in range
   --    116 - REG - LOCAL SIM same with OFW SIM
   --    117 - REG - only PREPAID are allowed
   --    118 - REG - link to POSTPAID are not allowed
   --    119 - GROAM ON - on breathing period
   --    140 - GROAM EXTEND - with open ended roaming
   --    141 - YES - no pending link
   --    142 - Invalid Syntax or Invalid Match but existing FUN subs
   --    143 - Reg and Link - OFW SIM already link as Local SIM
   --    144 - Cancel Error - Sender is local SIM
   --    145 - USURF ON Error - Already subscribe to USURF
   --    146 - USURF ON Error - invalid country
   --    147 - USURF ON Error - invalid duration
   --    148 - USURF STATUS for no subscription
   --    149 - USURF STATUS with pending subscription
   --    152 - USURF ON Error - Already subscribe to USURF

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
   -- 17    TRAN_TYPE_YES
   -- 18    TRAN_TYPE_YES_ARDS
   -- 19    TRAN_TYPE_NO_ARDS
   -- 20    USURF_ON
   -- 21    USURF_OFF
   -- 22    USURF_STATUS

   -- retr successful returns
   --      1 - successful except pre-reg
   --      2 - successful for pre-reg

   sp_logger('INIT' , 'START =>'      ||
                      ' p_trantype :' || to_char(p_trantype) ||
                      ' p_msisdn:'    || p_msisdn            ||
                      ' p_req_id:'    || to_char(p_req_id)   ||
                      ' p_ref_id:'    || to_char(p_ref_id)   ||
                      ' p_extra_i_1:' || p_extra_i_1         ||
                      ' p_extra_i_2:' || p_extra_i_2         ||
                      ' p_extra_i_3:' || p_extra_i_3         ||
                      ' p_extra_i_4:' || p_extra_i_4);

   nRetr     := 1; -- successful except prereg
   bRoamer   := FALSE;
   nMinBal   := -1;

   if sf_is_blacklisted(p_msisdn) > 0 then

      if p_trantype = 2 then
         vBlackReason := 'Error on REG due to blacklisting.';
      elsif p_trantype = 3 then
         vBlackReason := 'Error on UNREG due to blacklisting.';
      elsif p_trantype = 4 then
         vBlackReason := 'Error on LINK due to blacklisting.';
      elsif p_trantype = 6 then
         vBlackReason := 'Error on CANCEL due to blacklisting.';
      elsif p_trantype = 8 then
         vBlackReason := 'Error on GROAM ON due to blacklisting.';
      elsif p_trantype = 9 then
         vBlackReason := 'Error on GROAM OFF due to blacklisting.';
      else
         vBlackReason := 'Error accessing application due to blacklisting.';
      end if;

      insert into BLACKLISTED_WHITELISTED_LOG
      values (p_msisdn, 'BLACKLISTED', sysdate, vBlackReason, trunc(sysdate), user, 0 );
      commit;

      nRetr  := 100;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;

   end if;

   -- GROAM ON - validate msisdn if its in range
   if (p_trantype in (8,9)) and (sf_is_in_hlr_range(p_msisdn) = 0) then
      nRetr  := 113;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- REG - validate msisdn if its in range
   --if (p_trantype = 2) and (sf_is_in_customer_type_range('FUN', p_msisdn) = 0) then
   --   nRetr  := 115;
   --   p_retr := nRetr;
   --   return;
   --end if;

   -- REG - LOCAL SIM same with OFW SIM
   if (p_trantype in (2,3,4,5,6)) and ( p_msisdn = p_extra_i_3) then
      if (p_trantype = 5) then
          vActiveLinks := sf_get_fun_links(p_msisdn);
          if vActiveLinks is not null then
             nRetr  := 142;
             p_retr := nRetr;
             p_extra_o_2 := vActiveLinks;
             sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
             return;
          else
             vActiveLinks := sf_get_fun_links_local(p_msisdn);
             if vActiveLinks is not null then
                nRetr  := 142;
                p_retr := nRetr;
                p_extra_o_2 := vActiveLinks;
                sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
                return;
             end if;
          end if;
      end if;
      nRetr  := 116;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- REG - only PREPAID are allowed
   if (p_trantype in (2,3)) and (sf_is_in_hlr_range(p_msisdn) = 0) then
      nRetr  := 117;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- REG - only PREPAID are allowed
   if (p_trantype in (2,3)) and (sf_is_in_hlr_range(p_extra_i_3) = 0) then
      nRetr  := 118;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- LINK - only PREPAID are allowed
   if (p_trantype in (4,6)) and (sf_is_in_hlr_range(p_msisdn) = 0) then
      nRetr  := 118;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- LINK - only PREPAID are allowed
   if (p_trantype in (4,6)) and (sf_is_in_hlr_range(p_extra_i_3) = 0) then
      nRetr  := 117;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   -- GROAM ON - validate start date
   if (p_trantype = 8) and (p_extra_i_1 is not null) then
      begin
         if length(p_extra_i_1) < 10 then
            nRetr  := 104;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         end if;

         dActivation := to_date(p_extra_i_1, 'MM/DD/YYYY');
         if dActivation < trunc(sysdate) then
            nRetr  := 105;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         end if;
      exception
         when others then
            nRetr  := 104;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
      end;
   end if;

   -- GROAM ON and GROAM EXTEND - validate duration
   if (p_trantype in (8,10)) and (p_extra_i_2 is not null) and (nvl(p_ref_id, 0) = 0) then
      if (p_extra_i_2 is not null) then
         begin
            nDuration := to_number(p_extra_i_2);
         exception
            when others then
               if (p_trantype = 8) then
                  nRetr := 101;
               --else
               --   nRetr := 111;
               end if;
               p_retr := nRetr;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
         end;
         -- GROAM EXTEND - validate duration
         -- allow open duration for subs in non-hot country
         --if (p_trantype = 10) and (nDuration=0) then
         --   nRetr  := 111;
         --   p_retr := nRetr;
         --   sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         --   return;
         --end if;
      end if;

   end if;

   -- 2 - TRAN_TYPE_REG
   -- 4 - TRAN_TYPE_LINK
   -- 8 - TRAN_TYPE_GROAM_ON
   -- 10 - TRAN_TYPE_GROAM_EXTEND
   if (p_trantype in (2,4,8,10)) then

      -- check if process comes from web or sms
      nBreathing_period := 0;
      if nvl(p_ref_id, 0) > 0 then
         begin
            select nvl(min_bal,0), nvl(duration,0), tx_date, nvl(breathing_period,0), other_type, nvl(min_bal,1)
            into   nMinbal, nDuration, dActivation, nBreathing_period, vOtherType, nMinBal_Web
            from   manual_enrollment_log
            where  id = p_ref_id;
            -- check if exempt from bal check
            if nMinbal <> 0 then
               nMinbal := -1;
            end if;
            if nDuration = 0 then
               nDuration := null;
            end if;
         exception
            when NO_DATA_FOUND then nMinbal := -1;
         end;
      end if;

      -- GROAM ON - validate activation date
      if (p_trantype = 8) and (dActivation is null) then
         nRetr  := 104;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- get customer type info
      begin
         if p_trantype in (2,4) then
            vProfile := 'FUN';
         else
            if (sf_is_active_fun(p_msisdn) > 0) then
               vProfile := 'FUN';
            else
               vProfile := sf_get_customer_type(to_number(p_msisdn));
            end if;
         end if;
         select nvl(min_balance,0) min_balance, nvl(max_duration,nDefaultDuration) max_duration, breathing_period, nvl(pre_act_day,nDefaultPreAct), nvl(num_link,2), ext_min_balance
         into   nCustMinbal, nCustDuration, nCustBreathingPeriod, nCustPre_act_day, nCustNum_Link, nMinBalExt
         from   customer_types
         where  customer_type = vProfile;
      exception
         when no_data_found then
            raise_application_error(-200002, 'Customer type: ' || vProfile || ' not defined. Please contact your application provider.');
      end;

      -- GROAM ON - validate max duration
      if (p_trantype = 8) then
         nCustDuration := nDefaultMaxDuration;
         if (nDuration is not null) and (nDuration > 0) and (nCustDuration < nDuration) then
            nRetr := 101;
            p_retr := nRetr;
            return;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         end if;
      end if;

      -- GROAM ON - validate past pre act days
      if (p_trantype = 8) and (dActivation > (trunc(sysdate)+nCustPre_act_day)) then
         nRetr  := 105;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- GROAM ON
      if (p_trantype = 8) and (sf_is_roamer_info_imsi(p_msisdn, dSubsActDt, dSubsDeactDt, nSubsHotDur, nSubsInHot, dSubsHotDt, vStatus, vIMSI ) > 0) then

         -- validate if already roamer
         if (vStatus='ACTIVE') then

            begin
               insert into ARDSResponse (id, imsi, enrollment_type, status, cause, start_tm, end_tm)
               values (ARDSResponse_id_seq.nextval, vIMSI, 'ACTIVATION', 0, 2, sysdate, null);
            exception
               when others then null;
            end;

            nRetr := 109;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         end if;

         -- validate breathing period
         if (nBreathing_period=0) and (nSubsHotDur>0) and (nSubsInHot > 0) and
            ((nvl(dSubsDeactDt,sysdate)-dSubsActDt) >= nSubsHotDur) and
            ((trunc(sysdate)-dSubsDeactDt) < nCustBreathingPeriod )
         then
            nRetr := 119;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         end if;

      end if;

      -- GROAM EXTEND - validate max duration
      if (p_trantype = 10) then

         -- GROAM EXTEND - validate if not an existing roamer
         begin
            select hot_country_cnt
            into   nSubsInHot
            from   sim_activation
            where  msisdn = p_msisdn
            and    status = 'ACTIVE';
         exception
            when no_data_found then
               nRetr := 102;
               p_retr := nRetr;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
         end;

         dbms_output.put_line('nDuration:'|| to_char(nDuration) || ' nCustDuration:'|| to_char(nCustDuration)  || ' nSubsInHot:'|| to_char(nSubsInHot));
         if (nSubsInHot > 0) then
            -- open duration is not allowed for subs in hot country, allowed is up to max duration.
            if (nDuration is null) or (nDuration=0) or (nCustDuration < nDuration) then
               nRetr := 111;
               p_retr := nRetr;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' spcl nCustDuration: ' || to_char(nCustDuration) || ', nDuration:' || to_char(nDuration) );
               return;
            end if;
         else
            -- allow open duration for subs in non-hot country
            nCustDuration := nDefaultMaxDuration;
            if (nDuration is not null) and (nDuration > 0 ) and (nCustDuration < nDuration) then
               nRetr := 111;
               p_retr := nRetr;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' rgrl nCustDuration: ' || to_char(nCustDuration) || ', nDuration:' || to_char(nDuration) );
               return;
            end if;
         end if;
      end if;

      -- GGROAM EXTEND - validate current duration check if open duration
      --if (p_trantype = 10) and (dSubsDeactDt is null) then
      --   nRetr := 140;
      --   p_retr := nRetr;
      --   sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:'|| p_extra_o_3);
      --   return;
      --end if;

      -- REG - check if LOCAL SIM was already used as OFW SIM
      if (p_trantype = 2) and (sf_is_active_fun(p_msisdn) > 0) then
         nRetr := 110;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- REG - if link already exists
      if (p_trantype = 2) and (sf_is_link_active(p_extra_i_3, p_msisdn) > 0) then
         nRetr := 112;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- REG - local sim already link to OFW SIM
      if (p_trantype = 2) and (sf_is_local_sim_active_link(p_msisdn) > 0) then
         nRetr := 114;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- REG - OFW sim already link as LOCAL SIM
      if (p_trantype = 2) and (sf_is_local_sim_active_link(p_extra_i_3) > 0) then
         nRetr := 143;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- REG - exceeds max number of link for OFW SIM
      if (p_trantype = 2) and (sf_is_active_fun(p_extra_i_3) >= nCustNum_Link) then
         nRetr := 107;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- LINK - sim already link as LOCAL SIM
      if (p_trantype = 4) and (sf_is_local_sim_active_link(p_msisdn) > 0) then
         nRetr := 144;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- LINK - sim already link as LOCAL SIM
      if (p_trantype = 4) and (sf_is_local_sim_active_link(p_extra_i_3) > 0) then
         nRetr := 114;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- LINK - check if LOCAL SIM was already used as OFW SIM
      if (p_trantype = 4) and (sf_is_active_fun(p_extra_i_3) > 0) then
         nRetr := 110;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- LINK - if link already exists
      if (p_trantype = 4) and (sf_is_link_active(p_msisdn, p_extra_i_3) > 0) then
         nRetr := 112;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- LINK - exceeds max number of link for OFW SIM
      if (p_trantype = 4) and (sf_is_active_fun(p_msisdn) >= nCustNum_Link) then
         nRetr := 107;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      -- get default mimimun balance
      if (nMinbal = 0) and (nvl(p_ref_id, 0) > 0) then
         nMinBal := 0;
      else
         if p_trantype = 10 then
            nMinBal := nMinBalExt;
         else
            nMinBal := nCustMinbal;
         end if;
      end if;

      if (dActivation > trunc(sysdate)) and (p_trantype = 8) then
         begin
            -- check if there's already pending pre-act transaction
            SELECT count(1)
            INTO   nPreActPending
            FROM   MANUAL_ENROLLMENT_LOG
            WHERE  ENROLLMENT_TYPE = 'ACTIVATION'
            AND    MSISDN = p_msisdn
            AND    STATUS = 0;

            nRetr := 2;
            if nPreActPending = 0 then
               sp_logger('INIT' , 'MID => p_trantype :' || to_char(p_trantype) || ' nMinBal:' || to_char(nMinBal) || ', nCustMinbal:' || to_char(nCustMinbal));
               INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin , customer_type)
               VALUES (manual_enrollment_log_seq.nextval, p_msisdn, 'ACTIVATION', 0, dActivation, nvl(nDuration,0), 0, user, sysdate, NULL, vProfile, NULL, 'Pre-activation', NULL, decode(nMinBal, 0, 0, 1), 0, 'SMS', vProfile);
            else
               nRetr := 106;
               p_retr := nRetr;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
            end if;
         exception
            when dup_val_on_index then
              nRetr := 106;
              p_retr := nRetr;
              sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
              return;
            when others then
              nRetr := SQLCODE;
              p_retr := nRetr;
              sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' ORAERR p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
              return;
         end;
      elsif (p_trantype = 8) then
         update request_log
         set    duration = nDuration,
                min_bal  = nMinBal,
                breathing_period = nBreathing_period,
                msg = nvl(vOtherType, 'ACTIVATION')
         where  id = p_req_id;
      elsif (p_trantype = 10) then
         update request_log
         set    duration = nDuration,
                min_bal  = nMinBal,
                breathing_period = nBreathing_period
         where  id = p_req_id;
      end if;

   --  9 - TRAN_TYPE_GROAM_OFF
   elsif (p_trantype = 9) then
      nRoamerStatus := SF_CHECK_ROAMER_STATUS_IMSI(p_msisdn, vIMSI);
      if nRoamerStatus = 0 then

         begin
            insert into ARDSResponse (id, imsi, enrollment_type, status, cause, start_tm, end_tm)
            values (ARDSResponse_id_seq.nextval, vIMSI, 'DEACTIVATION', 0, 2, sysdate, null);
         exception
            when others then null;
         end;

         nRetr := 102;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      elsif nRoamerStatus = 2 then
         nRetr := 2;
      end if;

      if nvl(p_ref_id, 0) > 0 then
         --begin
         --   select other_type
         --   into   vOtherType
         --   from   manual_enrollment_log
         --   where  id = p_ref_id;
            if vOtherType = 'INSUFF_BAL' then
               nRetr := 3;
            --elsif vOtherType = 'OVERSTAY_HOT_COUNTRY' then
            --   nRetr := 4;
            end if;
         --exception
         --   when NO_DATA_FOUND then nMinbal := -1;
         --end;
      end if;


   --  11 - TRAN_TYPE_GROAM_STATUS
   elsif (p_trantype = 11) then
      if sf_is_active_roamer(p_msisdn) = 0 then
         nRetr := 102;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

   -- 3 - TRAN_TYPE_UNREG
   elsif (p_trantype = 3) and (sf_is_link_active(p_extra_i_3, p_msisdn) = 0) then
      if sf_is_active_fun(p_msisdn) > 0  then
         nRetr  := 144;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;
      nRetr := 103;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;

   -- 5 - TRAN_TYPE_CHECKLINK
   elsif (p_trantype = 5) then
      if (p_extra_i_3 is not null) then
         begin
            nLinkTo := to_number(p_extra_i_3);
            if nLinkTo < 639000000000 then
               nLinkTo := null;
            end if;
         exception
            when others then nLinkTo := null;
         end;
      else
         nLinkTo := null;
      end if;

      if (nLinkTo is null) then
         vActiveLinks := sf_get_fun_links(p_msisdn);
         if vActiveLinks is not null then
            nRetr  := 142;
            p_retr := nRetr;
            p_extra_o_2 := vActiveLinks;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         else
            vActiveLinks := sf_get_fun_links_local(p_msisdn);
            if vActiveLinks is not null then
               nRetr  := 142;
               p_retr := nRetr;
               p_extra_o_2 := vActiveLinks;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
            else
               nRetr  := 141;
               p_retr := nRetr;
               p_extra_o_2 := '';
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
            end if;
         end if;
      end if;

      if (sf_is_link_active(p_msisdn, p_extra_i_3) = 0) then
         if (sf_is_link_active(p_extra_i_3, p_msisdn) = 0) then
            vActiveLinks := sf_get_fun_links(p_msisdn);
            if vActiveLinks is not null then
               nRetr  := 142;
               p_retr := nRetr;
               p_extra_o_2 := vActiveLinks;
               sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
               return;
            else
               vActiveLinks := sf_get_fun_links_local(p_msisdn);
               if vActiveLinks is not null then
                  nRetr  := 142;
                  p_retr := nRetr;
                  p_extra_o_2 := vActiveLinks;
                  sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
                  return;
               end if;
            end if;
            nRetr := 103;
            p_retr := nRetr;
            sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
            return;
         end if;
      end if;

   -- 6 - TRAN_TYPE_CANCEL
   elsif (p_trantype = 6) and (sf_is_link_active(p_msisdn, p_extra_i_3) = 0) then
      vActiveLinks := sf_get_fun_links_local(p_msisdn);
      if vActiveLinks is not null then
         nRetr  := 144;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;
      nRetr := 103;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;

   -- 14 - TRAN_TYPE_YES
   elsif (p_trantype = 17) and (sf_is_link_pending(p_msisdn) = 0) then
      nRetr := 141;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;

   -- 18    TRAN_TYPE_YES_ARDS
   elsif (p_trantype = 18) then
      nRoamerStatus := sf_check_roamer_status(p_msisdn);
      if nRoamerStatus = 0 then
         nRetr := 102;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      nRetr := 1;
      p_retr := nRetr;
      return;
   -- 20    TRAN_TYPE_USURF_ON
   elsif (p_trantype = 20) then
      nUsurfStatus := sf_check_usurf_status(p_msisdn, p_extra_i_4);
      if nUsurfStatus = 1 then
         nRetr := 145;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      nRoamerStatus := sf_is_valid_usurf(p_extra_i_4, p_extra_i_2);
      if nRoamerStatus=2 then
         nRetr := 146;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      elsif nRoamerStatus=3 then
         nRetr := 147;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      if nUsurfStatus = 0 then
         begin
            insert into usurf_activation (id, msisdn, country, denom, activation_dt, status, dt_created, created_by)
            values (usurf_activation_seq.nextval, p_msisdn, p_extra_i_4, p_extra_i_2, sysdate, 'PENDING', sysdate, user);
         exception
            when dup_val_on_index then null;
            when others then null;
         end;
      end if;
      nRoamerStatus := sf_check_roamer_status(p_msisdn);
      if nRoamerStatus = 0 then
         if nUsurfStatus = 2 then
            nRetr := 149;
         else
            nRetr := 151;
         end if;
      else
         nRetr := 150;
      end if;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;

   -- 22    TRAN_TYPE_USURF_STATUS
   elsif (p_trantype = 22) then
      sp_get_usurf_status(nRoamerStatus, vPartner, vExptime, vExpdate, p_msisdn);
      if nRoamerStatus = 1 then
         nRetr := 145;
      elsif nRoamerStatus = 0 then
         nRetr := 148;
      elsif nRoamerStatus = 2 then
         nRetr := 149;
      end if;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      p_retr := nRetr;
      p_extra_o_1 := '';
      p_extra_o_2 := nvl(vExptime,'');
      p_extra_o_3 := nvl(vExpdate,'');
      return;
   end if;


   if (p_trantype = 9) and (nRetr = 1) then
      begin
         update sim_activation
         set    status='PENDING'
         where  msisdn  = p_extra_i_1;
      exception
         when others then null;
      end;
   end if;

   -- set return - minimun balance
   if nMinBal is not null then
      p_extra_o_1 := to_char(nMinBal);
   else
      p_extra_o_1 := '';
   end if;

   --if nDuration is not null then
   --   p_extra_o_2 := to_char(nDuration);
   --else
      p_extra_o_2 := '';
   --end if;
   p_extra_o_3 := '';

   p_retr      := nRetr;
   sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) ||
                      ' p_msisdn:'          || p_msisdn            ||
                      ' p_retr:'            || to_char(p_retr)     ||
                      ' p_extra_o_1: '      || p_extra_o_1         ||
                      ' p_extra_o_2:'       || p_extra_o_2         ||
                      ' p_extra_o_3:'       || p_extra_o_3);

end sp_init_tran;
/
show err



CREATE OR REPLACE PROCEDURE "SP_LOGGER" (p_source in varchar2, p_info in varchar2) is
begin
   insert into system_log (source, info, dt_created, id )
   values (p_source, p_info, sysdate, system_log_seq.nextval);
   commit;
end;
/
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_DAILY_BALANCE" (
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
      SET    daily_bal = p_daily_bal_m,
             last_daily_bal = p_daily_bal_a,
             --last_daily_bal_chk = trunc(sysdate),
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
               INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin, other_type )
               VALUES (manual_enrollment_log_seq.nextval, p_msisdn, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, user, sysdate, NULL, 'TRIGGER', NULL, 'Due to Insuffbal', NULL, 0, 0, 'WEB', 'INSUFF_BAL');
            else
               UPDATE MANUAL_ENROLLMENT_LOG
               SET    TX_DATE = trunc(sysdate),
                      REMARKS = 'Due to Insuffbal',
                      other_type = 'INSUFF_BAL'
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
         dDeact := trunc(sysdate)+greatest(3-nDaily_Bal_Ctr, 1);
      end if;

   else
      UPDATE sim_activation
      SET    daily_bal = p_daily_bal_m,
             last_daily_bal = p_daily_bal_a,
             --last_daily_bal_chk = trunc(sysdate),
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
   sp_logger('PROCESS_DAILY_BALANCE', 'END => p_msisdn:' || p_msisdn || ' p_retr:' || to_char(p_retr) || ' p_deact_dt:' || to_char(p_deact_dt) || ' nDaily_Bal_Ctr:' || to_char(nDaily_Bal_Ctr) );

exception
   WHEN OTHERS THEN
        raise_application_error(-200002, SQLERRM);
end sp_process_daily_balance;
/
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_DEFAULT_UNLI" (
  p_retr out number,
  p_msisdn in varchar2,
  p_mnc in varchar2,
  p_mcc in varchar2,
  p_sgsn_ip in varchar2,
  p_date in varchar2,
  p_filename in varchar2 ) is
  
  -- 1   - Valid Transaction 
  -- 101 - Invalid MSISDN
  -- 102 - Invalid MNC
  -- 103 - Invalid MCC
  -- 104 - Invalid SGSN_IP
  -- 105 - invalid DATE
  -- 106 - invalid FILENAME
  -- 107 - Duplicate / With Active Default Unli
  -- 108 - Unable to insert notification message
  -- 109 - Invalid TZ
  
  vMsg_1st    Varchar2(2000);
  vMsg_Swi    Varchar2(2000);
  vMcc        Varchar2(30);
  nRetr       Number;
  vPartner    Number; 
  vTranDate   Date;
  vTranDateTZ Date;
  nPreExpiry  Number;
  nExpiry     Number;
  dExpiry     Date;
  vUnliFlag   Number;
  vTZ         Number;
  nNextId     Number; 
begin
  
  nRetr := 1;

  if  p_msisdn is null then nRetr := 101;
  elsif  substr(p_msisdn,1,2) <> '63' then nRetr := 101;
  elsif  length(p_msisdn) <> 12 then nRetr := 101;
  elsif p_mnc is null then nRetr := 102;
  elsif p_mcc is null then nRetr := 103;
  elsif p_sgsn_ip is null then nRetr := 104;
  elsif p_date is null then nRetr := 105;
  elsif p_filename is null then nRetr := 106;
  end if;

  begin
     select to_date(substr(p_date, 1, 10) || ' ' || substr(p_date, 12, 8), 'YYYY/MM/DD HH24:MI:SS') into vTranDate from dual;
  exception 
     when others then 
        nRetr := 105;
  end;

  -- check mnc
  begin
     select 1, to_number(tz) into vPartner, vTZ
     from roaming_partners
     where mcc = p_mcc
     and   mnc = p_mnc;
  exception 
     when no_data_found then 
        vPartner := 0;
     when invalid_number then 
        nRetr := 109;
  end;

  -- check mcc
  if vPartner = 1 then 
    begin
       select msg
       into vMsg_1st
       from mcc_messages
       where mcc = p_mcc
       and   message_type = '1';
    exception 
       when no_data_found then 
          nRetr := 103;
    end;
  else
    begin
       select msg
       into vMsg_Swi
       from mcc_messages
       where mcc = p_mcc
       and   message_type = '2';
    exception 
       when no_data_found then 
          nRetr := 103;
    end;
  end if;

  if nRetr = 1 then
     if vMsg_Swi is not null then
        UPDATE default_unli_log SET status=0 WHERE msisdn = p_msisdn AND mcc = p_mcc AND status >= 1;
        -- UPDATE default_unli_exp SET status=0 WHERE msisdn = p_msisdn AND mcc = p_mcc AND status >= 1;
        begin
           insert into broadcast_log (id, msisdn, message_id, tx_date, status, cluster_node, msg)
           values (broadcast_log_seq.nextval, p_msisdn, '2', vTranDate, 0, 1, vMsg_Swi);
        exception 
           when dup_val_on_index then 
              nRetr := 108;
        end;
     else
        begin
           select param_num1_value, param_num2_value 
           into   nPreExpiry, nExpiry
           from   default_unli_params
           where  param_id = 'UNLI_PRE_EXP';
        exception 
           when others then 
              nPreExpiry := (11.5/24);
              nExpiry := (12/24);
        end;
        begin
           -- Radcom is UTC+0
           --vTranDateTZ :=  vTranDate + sf_get_unli_tz(8,vTz);
           vTranDateTZ :=  vTranDate + sf_get_unli_tz(8,0);
           select default_unli_seq.nextval into nNextId from dual;
           insert into default_unli_log (
                   id, msisdn, mnc, mcc, sgsn_ip, tran_dt, tran_tm, dt_created, 
                   status, file_name, expiry_date)
           values (nNextId, p_msisdn, p_mnc, p_mcc, p_sgsn_ip, 
                   trunc(vTranDateTZ), vTranDateTZ, sysdate, 
                   1, p_filename, vTranDateTZ+nExpiry );

           insert into default_unli_exp ( 
                   id, msisdn, tran_dt, tran_tm, mcc, mnc, status, original_dt, 
                   pre_expiry_dt, pre_expiry_tm, expiry_dt, expiry_tm )
           values (nNextId, p_msisdn, trunc(vTranDateTZ), vTranDateTZ, p_mcc, p_mnc, 1, vTranDate, 
                   trunc(vTranDateTZ+nPreExpiry), vTranDateTZ+nPreExpiry, 
                   trunc(vTranDateTZ+nExpiry), vTranDateTZ+nExpiry);
        exception 
           when dup_val_on_index then 
              SELECT expiry_date, status, id
              INTO   dExpiry, vUnliFlag, nNextId
              FROM   default_unli_log
              WHERE  msisdn = p_msisdn
              AND    mcc = p_mcc;
              if (dExpiry < vTranDateTZ) then
                 UPDATE default_unli_log
                 SET    mnc = p_mnc,
                        sgsn_ip = p_sgsn_ip,
                        tran_dt = trunc(vTranDateTZ),
                        tran_tm = vTranDateTZ, 
                        expiry_date = vTranDateTZ+nExpiry, 
                        status=1,
                        file_name = p_filename
                 WHERE  msisdn = p_msisdn
                 AND    mcc = p_mcc;

                 UPDATE default_unli_exp
                 SET    mnc = p_mnc, 
                        tran_dt = trunc(vTranDateTZ),
                        tran_tm = vTranDateTZ,
                        pre_expiry_dt = trunc(vTranDateTZ+nPreExpiry), 
                        pre_expiry_tm = vTranDateTZ+nPreExpiry, 
                        expiry_dt = trunc(vTranDateTZ+nExpiry), 
                        expiry_tm = vTranDateTZ+nExpiry, 
                        original_dt = vTranDate, 
                        status=1
                 WHERE  msisdn = p_msisdn
                 AND    mcc = p_mcc;
                 nRetr := 1;
              elsif (vUnliFlag=0) then
                 UPDATE default_unli_log SET status=1 WHERE msisdn = p_msisdn AND mcc = p_mcc;
                 -- UPDATE default_unli_exp SET status=1 WHERE msisdn = p_msisdn AND mcc = p_mcc;
                 -- no notification should be received as per email dated Apr 25 with subject Notification Issue 
                 -- nRetr := 1;
                 nRetr := 107;
              else
                 nRetr := 107;
              end if;
        end;
        if (vMsg_1st is not null) and (nRetr = 1) then
           begin
              insert into broadcast_log (id, msisdn, message_id, tx_date, status, cluster_node, msg)
              values (broadcast_log_seq.nextval, p_msisdn, '1', sysdate, 0, 1, vMsg_1st);
           exception 
              when dup_val_on_index then 
                 nRetr := 108;
           end;
        end if;
     end if;
     commit;
  end if;

  -- track files uploaded 
  if nRetr = 1 then
     update radcom_downloaded_files
     set    no_valid = no_valid + 1
     where  file_name = p_filename;
     if sql%notfound then
        begin
           insert into radcom_downloaded_files (id, file_name, no_valid, no_errors, dt_uploaded )
           values (radcom_downloaded_files_seq.nextval, p_filename, 1, 0, sysdate);
        exception
           when dup_val_on_index then null;
        end;
     end if;
     commit;
  else
     update radcom_downloaded_files
     set    no_errors = no_errors + 1
     where  file_name = p_filename;
     if sql%notfound then
        begin
           insert into radcom_downloaded_files (id, file_name, no_valid, no_errors, dt_uploaded )
           values (radcom_downloaded_files_seq.nextval, p_filename, 0, 1, sysdate);
        exception
           when dup_val_on_index then null;
        end;
     end if;
     commit;
  end if;

  -- set return parameters
  p_retr := nvl(nRetr,0);

end sp_process_default_unli;
/
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_FOR_ARDS_EXPIRY" is
   nCommitCtr Number := 0;
   nDeActPending Number;
   nCustBreathingPeriod Number;
   vReason Varchar2(60);
   vOtherType Varchar2(30);
   nWarningDeact number;
   vMsg Varchar2(120);
   vDiffTime Number;
   nArds1stNotify number;
   nArds2ndNotify number;
   nArdsDuration number;
begin
   begin
      select warning_deact_days, ards_1st_notify, ards_2nd_notify, ards_tmp_duration
      into   nWarningDeact, nArds1stNotify, nArds2ndNotify, nArdsDuration
      from   customer_types
      where  customer_type = 'TRIGGER'
      and    rownum = 1;
      if (nWarningDeact is null) or (nWarningDeact<=0) then
         nWarningDeact := 2;
      end if;
      if (nArds1stNotify is null) or (nArds1stNotify<=0) then
         nArds1stNotify := .5;
      end if;
      if (nArds2ndNotify is null) or (nArds2ndNotify<=0) then
         nArds2ndNotify := .33333333333;
      end if;
      if (nArdsDuration is null) or (nArdsDuration<=0) then
         nArdsDuration := 1;
      end if;
   exception
      when no_data_found then
         nWarningDeact  := 2;
         nArds1stNotify := .5;
         nArds2ndNotify := .33333333333;
         nArdsDuration  := 1;
   end;

   for i in (select msisdn, activation_dt, deactivation_dt, deactivation_tm, duration, hot_country_cnt,
                    customer_type, deactivation_reason, request_origin, imsi, deactivation_tm-sysdate diff_time
             from   sim_activation
             where  status ='ACTIVE'
             and    request_origin = 'ARDS'
             and    deactivation_dt >= trunc(sysdate)-1
             and    deactivation_tm <= sysdate + nArdsDuration)
   loop
      vMsg := null;
      vDiffTime := (i.deactivation_tm - sysdate );
      sp_logger('SP_PROCESS_FOR_ARDS_EXPIRY' , 'ARDS EXPIRY for '  || to_char(i.msisdn) ||
                                                 ' difftime=' || to_char(vDiffTime) ||
                                                 ' nArds1stNotify=' || to_char(nArds1stNotify) ||
                                                 ' nArds2ndNotify=' || to_char(nArds2ndNotify) ||
                                                 ' nArdsDuration=' || to_char(nArdsDuration) ||
                                                 ' reason=' || i.deactivation_reason );
      if vDiffTime between nArds2ndNotify and nArds1stNotify and
         (nvl(i.deactivation_reason,' ') <> '1st Notification')
      then
         vMsg := 'ARDS_PROVISIONAL_PROV_1ST_EXPIRY';
         update sim_activation
         set    deactivation_reason = '1st Notification'
         where  msisdn = i.msisdn;

      elsif vDiffTime between 0 and nArds2ndNotify and
         (nvl(i.deactivation_reason,' ') <> '2nd Notification')
      then
         vMsg := 'ARDS_PROVISIONAL_PROV_2ND_EXPIRY';
         update sim_activation
         set    deactivation_reason = '2nd Notification'
         where  msisdn = i.msisdn;

      elsif (vDiffTime <= 0) then

         vMsg := 'ARDS_PROVISIONAL_PROV_LAST_EXPIRY';

         INSERT INTO MANUAL_ENROLLMENT_LOG (
                 ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED,
                 subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin, other_type, imsi )
         VALUES (manual_enrollment_log_seq.nextval, i.msisdn, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, user, sysdate,
                 NULL, 'TRIGGER', NULL, vReason, NULL, 0, 0, i.request_origin, 'Due to ARDS expiry', i.imsi);

         update sim_activation set status='PENDING' where MSISDN=i.msisdn;

         nCommitCtr := nCommitCtr + 1;
      end if;

      if (vMsg is not null) then
         begin
            INSERT INTO BROADCAST_LOG ( ID, MSISDN, MESSAGE_ID, TX_DATE, STATUS, CLUSTER_NODE )
            VALUES (broadcast_log_seq.nextval, i.msisdn, vMsg, trunc(sysdate), 0, 1);
            nCommitCtr := nCommitCtr + 1;
         exception
            when dup_val_on_index then
               sp_logger('SP_PROCESS_FOR_ARDS_EXPIRY' , 'ARDS BROADCAST_LOG Error: vMsg='  || vMsg || ' ORA:' || SQLCODE );
            when others then
               sp_logger('SP_PROCESS_FOR_ARDS_EXPIRY' , 'ARDS BROADCAST_LOG Error: vMsg='  || vMsg || ' ORA:' || SQLCODE );
         end;
      end if;

      if (mod(nCommitCtr,1500) = 0) then
         commit;
      end if;

   end loop;

   commit;
   sp_logger('SP_PROCESS_FOR_ARDS_EXPIRY', 'END WARNING => nCommitCtr:' || to_char(nCommitCtr));

end SP_PROCESS_FOR_ARDS_EXPIRY;
/
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_FOR_CANCEL" is
   nCommitCtr Number := 0;
   nDeActPending Number;
   vReason Varchar2(60);
   nMaxInactivity number;
   nWarningDeact  number;
begin
   begin
      select max_inactivity_days, warning_deact_days
      into   nMaxInactivity, nWarningDeact
      from   customer_types
      where  customer_type = 'FUN'
      and    rownum = 1;
      if (nMaxInactivity is null) or (nMaxInactivity<=0) then
         nMaxInactivity := 30;
      end if;
      if (nWarningDeact is null) or (nWarningDeact<=0) then
         nWarningDeact := 5;
      end if;
   exception
      when no_data_found then
         nMaxInactivity := 30;
         nWarningDeact := 5;
   end;

   sp_logger('SP_PROCESS_FOR_CANCEL', 'START => nMaxInactivity:' || to_char(nMaxInactivity) || ' nWarningDeact:' || to_char(nWarningDeact));
   for i in (select msisdn, link_to, cust_activation_dt, cust_deactivation_dt
             from   sim_activation_links
             where  status = 'ACTIVE'
             and    ods_last_chk_dt <= (trunc(sysdate)-nMaxInactivity))
   loop
      begin
         -- check if there's already pending pre-act transaction
         SELECT nvl(max(id),0)
         INTO   nDeActPending
         FROM   MANUAL_ENROLLMENT_LOG
         WHERE  ENROLLMENT_TYPE = 'CANCEL'
         AND    MSISDN  = i.msisdn
         AND    LINK_TO = i.link_to
         AND    STATUS = 0;
      exception
         when no_data_found then nDeActPending := 0;
      end;

      vReason := 'Due to Inactivity';
      if nDeActPending = 0 then
         INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, LINK_TO, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin, other_type )
         VALUES (manual_enrollment_log_seq.nextval, i.msisdn, i.link_to, 'CANCEL', 0, trunc(sysdate), 0, 0, user, sysdate, NULL, 'FUN', NULL, vReason, NULL, 0, 0, 'WEB', 'ODS_INACTIVITY');
      else
         UPDATE MANUAL_ENROLLMENT_LOG
         SET    TX_DATE = trunc(sysdate),
                REMARKS = vReason,
                other_type = 'ODS_INACTIVITY'
         where  ID = nDeActPending;
      end if;
      update sim_activation_links set status='PENDING' where MSISDN=i.msisdn and LINK_TO=i.link_to;
      nCommitCtr := nCommitCtr + 1;
      if (mod(nCommitCtr,1500) = 0) then
         commit;
      end if;
   end loop;
   commit;
   sp_logger('SP_PROCESS_FOR_CANCEL', 'END FOR DEACT => nCommitCtr:' || to_char(nCommitCtr));

   nCommitCtr := 0;
   --sp_logger('SP_PROCESS_FOR_CANCEL', 'START alert');
   -- SMS Alert for subscribers who will deactivated in n days...
   for i in (select msisdn, link_to, cust_activation_dt, cust_deactivation_dt
             from   sim_activation_links
             where  status ='ACTIVE'
             and    ods_last_chk_dt = (trunc(sysdate)-nWarningDeact) )
   loop
      begin
         INSERT INTO BROADCAST_LOG ( ID, MSISDN, MESSAGE_ID, TX_DATE, STATUS, CLUSTER_NODE )
         VALUES (broadcast_log_seq.nextval, i.msisdn, 'CANCEL_PRE_DEACT_WARNING', trunc(sysdate), 0, 1);

         INSERT INTO BROADCAST_LOG ( ID, MSISDN, MESSAGE_ID, TX_DATE, STATUS, CLUSTER_NODE )
         VALUES (broadcast_log_seq.nextval, i.link_to, 'CANCEL_PRE_DEACT_WARNING_LOCAL', trunc(sysdate), 0, 1);
         nCommitCtr := nCommitCtr + 1;
      exception
         when dup_val_on_index then null;
         when others then null;
      end;
      if (mod(nCommitCtr,1500) = 0) then
         commit;
      end if;
   end loop;
   commit;
   sp_logger('SP_PROCESS_FOR_CANCEL', 'END WARNING => nCommitCtr:' || to_char(nCommitCtr));

end sp_process_for_cancel;
/
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_FOR_DAILY_BAL_CHK" is
   nCommitCtr Number := 0;
   nDeActPending Number;
   nCustBreathingPeriod Number;
   vReason Varchar2(60);
begin
   sp_logger('SP_PROCESS_FOR_DAILY_BAL_CHK', 'START');
   for i in (SELECT a.msisdn, b.daily_balance daily_bal, a.last_daily_bal_ctr
             FROM  sim_activation a, customer_types b
             WHERE a.status = 'ACTIVE'
             AND   a.last_daily_bal_chk < trunc(sysdate)
             AND   b.customer_type = sf_get_customer_type(a.msisdn)
             AND   b.daily_balance > 0)

   loop
      begin
         INSERT INTO DAILYBAL_LOG ( ID, MSISDN, TX_DATE, STATUS, CLUSTER_NODE, daily_balance, last_daily_bal_ctr )
         VALUES (dailybal_log_seq.nextval, i.msisdn, trunc(sysdate), 0, 1, i.daily_bal, i.last_daily_bal_ctr);
         nCommitCtr := nCommitCtr + 1;
      exception
         when dup_val_on_index then null;
         when others then null;
      end;
      if (mod(nCommitCtr,1500) = 0) then
         commit;
      end if;
   end loop;
   commit;
   sp_logger('SP_PROCESS_FOR_DAILY_BAL_CHK', 'END => nCommitCtr:' || to_char(nCommitCtr));

end sp_process_for_daily_bal_chk;
/
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_FOR_DEACTIVATION" is
   nCommitCtr Number := 0;
   nDeActPending Number;
   nCustBreathingPeriod Number;
   vReason Varchar2(60);
   vOtherType Varchar2(30);
   nWarningDeact number;
   vMsg Varchar2(120);
   nArdsDuration number;
begin
   begin
      select warning_deact_days, ards_tmp_duration
      into   nWarningDeact, nArdsDuration
      from   customer_types
      where  customer_type = 'TRIGGER'
      and    rownum = 1;
      if (nWarningDeact is null) or (nWarningDeact<=0) then
         nWarningDeact := 2;
      end if;
   exception
      when no_data_found then
         nArdsDuration  := 1;
   end;

   sp_logger('SP_PROCESS_FOR_DEACTIVATION', 'START => nWarningDeact:' || to_char(nWarningDeact));
   for i in (select msisdn, activation_dt, deactivation_dt, duration, hot_country_cnt, customer_type, decode(request_origin, 'ARDS', 'ARDS', 'WEB') request_origin
             from   sim_activation
             where  status ='ACTIVE'
             and    deactivation_dt < trunc(sysdate) )
   loop
      if (i.request_origin = 'ARDS' and i.duration > nArdsDuration) or
         (i.request_origin <> 'ARDS')
      then
         begin
            -- check if there's already pending pre-act transaction
            SELECT nvl(max(id),0)
            INTO   nDeActPending
            FROM   MANUAL_ENROLLMENT_LOG
            WHERE  ENROLLMENT_TYPE = 'DEACTIVATION'
            AND    MSISDN = i.msisdn
            AND    STATUS = 0;
         exception
            when no_data_found then nDeActPending := 0;
         end;


         vReason    := 'Scheduled Deactivation';
         vOtherType := '';
         if ((i.deactivation_dt-i.activation_dt)>=i.duration) and
             (i.hot_country_cnt > 0)
         then
            begin
               select breathing_period
               into   nCustBreathingPeriod
               from   customer_types
               where  customer_type = i.customer_type;
            exception
               when no_data_found then nCustBreathingPeriod := 0;
               when others then nCustBreathingPeriod := 0;
            end;
            if nCustBreathingPeriod > trunc(sysdate-i.deactivation_dt) then
               vReason    := 'Due to Overstay on Special Country';
               vOtherType := 'OVERSTAY_HOT_COUNTRY';
            end if;
         end if;

         if nDeActPending = 0 then
            INSERT INTO MANUAL_ENROLLMENT_LOG (
                    ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED,
                    subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin, other_type )
            VALUES (manual_enrollment_log_seq.nextval, i.msisdn, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, user, sysdate,
                    NULL, 'TRIGGER', NULL, vReason, NULL, 0, 0, i.request_origin, vOtherType);
         else
            UPDATE MANUAL_ENROLLMENT_LOG
            SET    TX_DATE = trunc(sysdate),
                   REMARKS = vReason
            where  ID = nDeActPending;
         end if;
         update sim_activation set status='PENDING' where MSISDN=i.msisdn;
         nCommitCtr := nCommitCtr + 1;
         if (mod(nCommitCtr,1500) = 0) then
            commit;
         end if;
      end if;
   end loop;
   commit;
   sp_logger('SP_PROCESS_FOR_DEACTIVATION', 'END FOR DEACT => nCommitCtr:' || to_char(nCommitCtr));

   nCommitCtr := 0;
   --sp_logger('SP_PROCESS_FOR_PRE_DEACTIVATION', 'START');
   -- SMS Alert for subscribers who will deactivated in n days...
   for i in (select msisdn, activation_dt, deactivation_dt, duration, hot_country_cnt, customer_type, request_origin
             from   sim_activation
             where  status ='ACTIVE'
             and    deactivation_dt < (trunc(sysdate)+nWarningDeact) )
   loop
      if (i.request_origin = 'ARDS' and i.duration > nArdsDuration) or
         (i.request_origin <> 'ARDS')
      then
         begin
            INSERT INTO BROADCAST_LOG ( ID, MSISDN, MESSAGE_ID, TX_DATE, STATUS, CLUSTER_NODE )
            VALUES (broadcast_log_seq.nextval, i.msisdn, 'GROAM_OFF_PRE_DEACT_WARNING', trunc(sysdate), 0, 1);
            nCommitCtr := nCommitCtr + 1;
         exception
            when dup_val_on_index then null;
            when others then null;
         end;
         if (mod(nCommitCtr,1500) = 0) then
            commit;
         end if;
      end if;
   end loop;
   commit;

   sp_logger('SP_PROCESS_FOR_PRE_DEACTIVATION', 'END WARNING => nCommitCtr:' || to_char(nCommitCtr));

end sp_process_for_deactivation;
/
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_FOR_DEACTIVATION_M" (p_msisdn in number) is
   nCommitCtr Number := 0;
   nDeActPending Number;
   nCustBreathingPeriod Number;
   vReason Varchar2(60);
begin
   sp_logger('SP_PROCESS_FOR_DEACTIVATION', 'START');
   for i in (select msisdn, activation_dt, deactivation_dt, duration, hot_country_cnt, customer_type
             from   sim_activation
             where  status ='ACTIVE'
             and    deactivation_dt < trunc(sysdate)
             and    msisdn = p_msisdn)
   loop
      begin
         -- check if there's already pending pre-act transaction
         SELECT nvl(max(id),0)
         INTO   nDeActPending
         FROM   MANUAL_ENROLLMENT_LOG
         WHERE  ENROLLMENT_TYPE = 'DEACTIVATION'
         AND    MSISDN = i.msisdn
         AND    STATUS = 0;
      exception
         when no_data_found then nDeActPending := 0;
      end;


      vReason := 'Scheduled Deactivation';
      if ((i.deactivation_dt-i.activation_dt)>=i.duration) and
          (i.hot_country_cnt > 0)
      then
         begin
            select breathing_period
            into   nCustBreathingPeriod
            from   customer_types
            where  customer_type = i.customer_type;
         exception
            when no_data_found then nCustBreathingPeriod := 0;
            when others then nCustBreathingPeriod := 0;
         end;
         if nCustBreathingPeriod > trunc(sysdate-i.deactivation_dt) then
            vReason := 'Due to Overstay on Special Country';
         end if;
      end if;

      if nDeActPending = 0 then
         INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin )
         VALUES (manual_enrollment_log_seq.nextval, i.msisdn, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, user, sysdate, NULL, 'TRIGGER', NULL, vReason, NULL, 0, 0, 'WEB');
      else
         UPDATE MANUAL_ENROLLMENT_LOG
         SET    TX_DATE = trunc(sysdate),
                REMARKS = vReason
         where  ID = nDeActPending;
      end if;
      update sim_activation set status='PENDING' where MSISDN=i.msisdn;
      nCommitCtr := nCommitCtr + 1;
      if (mod(nCommitCtr,1500) = 0) then
         commit;
      end if;
   end loop;
   commit;
   sp_logger('SP_PROCESS_FOR_DEACTIVATION', 'END => nCommitCtr:' || to_char(nCommitCtr));

   nCommitCtr := 0;
   sp_logger('SP_PROCESS_FOR_PRE_DEACTIVATION', 'START');
   -- SMS Alert for subscribers who will deactivated in n days...
   for i in (select msisdn, activation_dt, deactivation_dt, duration, hot_country_cnt, customer_type
             from   sim_activation
             where  status ='ACTIVE'
             and    deactivation_dt < (trunc(sysdate)-2)
             and    msisdn = p_msisdn)
   loop
      begin
         INSERT INTO BROADCAST_LOG ( ID, MSISDN, MESSAGE_ID, TX_DATE, STATUS, CLUSTER_NODE )
         VALUES (broadcast_log_seq.nextval, i.msisdn, 'GROAM_OFF_PRE_DEACT', trunc(sysdate), 0, 1);
         nCommitCtr := nCommitCtr + 1;
      exception
         when dup_val_on_index then null;
         when others then null;
      end;
      if (mod(nCommitCtr,1500) = 0) then
         commit;
      end if;
   end loop;
   commit;
   sp_logger('SP_PROCESS_FOR_PRE_DEACTIVATION', 'END => nCommitCtr:' || to_char(nCommitCtr));

end sp_process_for_deactivation_m;
/
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_FOR_EXPIRE_PENDING" is
   dExpiredDate Date := sysdate-(5/60/24);
   vMsg Varchar2(1024);
begin
   select msg_1
   into   vMsg
   from   KEYWORD_MSG
   where  message_id = 'LINK_REQUEST_EXPIRED';

   --sp_logger('SP_PROCESS_FOR_EXPIRE_PENDING', 'START');
   for i in (select msisdn, link_to, cust_activation_dt, cust_deactivation_dt
             from   sim_activation_links
             where  status ='PENDING'
             and    expiry_date < dExpiredDate )
   loop
      begin
         INSERT INTO BROADCAST_LOG ( ID, MSISDN, MESSAGE_ID, TX_DATE, STATUS, CLUSTER_NODE, MSG )
         VALUES (broadcast_log_seq.nextval, i.msisdn, null, trunc(sysdate), 0, 1, replace(vMsg, '%s', to_char(i.link_to)));
         delete sim_activation_links
         where  msisdn = i.msisdn
         and    status = 'PENDING';
         commit;
      exception
         when dup_val_on_index then null;
         --when others then null;
      end;
   end loop;
   --sp_logger('SP_PROCESS_FOR_EXPIRE_PENDING', 'END');

end sp_process_for_expire_pending;
/
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_GEO_PROBE" (
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
   --        133 - change to non-special country with set duration
   nRetr         Number;
   nHotCtr       Number;
   nHot          Number;
   nDuration     Number;
   nHotDuration  Number;
   nSubsDuration Number;
   nBreathing    Number;
   nMsisdn       Number(12);
   vCountry      Varchar2(30);
   vCustType     Varchar2(30);
   vCurrCountry  Varchar2(60);
   vCountryName  Varchar2(60);
   nCurrHot      Number;
   nDeActPending Number;
   dDeactivationDt Date;
   dSubsActivationDt       Date;
   dSubsDeactivationDt     Date;
   nSubsBreathingPeriod    Number;
   nSubsBreathingPeriod_dt Date;
   dDeactBasisDt           Date;
begin
   nRetr := 1;
   sp_logger('PROCESS_GEO_PROBE' , 'START => p_imsi:' || p_imsi || ', p_sccp:' || to_char(p_sccp));

   begin
      SELECT hot, country_code
      INTO   nHot, vCountry
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
      SELECT nvl(hot_country_cnt,0), country, msisdn, duration, activation_dt, deactivation_dt,
             breathing_period, breathing_period_dt, customer_type, nvl(deact_basis_dt, trunc(sysdate)), hot_country_max_duration
      INTO   nHotCtr, vCurrCountry, nMsisdn, nSubsDuration, dSubsActivationDt, dSubsDeactivationDt,
             nSubsBreathingPeriod, nSubsBreathingPeriod_dt, vCustType, dDeactBasisDt, nHotDuration
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

      begin
         SELECT breathing_period, nvl(max_duration,9999)
         INTO   nBreathing, nDuration
         FROM   customer_types
         WHERE  customer_type = vCustType
         AND    rownum = 1;
      exception
        when no_data_found then
           nRetr := SQLCODE;
           p_retr := nRetr*-1;
           sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi  || ' vCustType:' || vCustType || ' on select to CUSTOMER_TYPES 1 nRetr:' || to_char(p_retr));
           return;
        when others then
           nRetr := SQLCODE;
           p_retr := nRetr;
           sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' vCustType:' || vCustType || ' on select to CUSTOMER_TYPES 2 nRetr:' || to_char(p_retr));
           return;
      end;

      sp_logger('PROCESS_GEO_PROBE' , 'CHECK => nHot:' || to_char(nHot) || ' nCurrHot:' || to_char(nCurrHot) || ' dDeactBasisDt:' || to_char(dDeactBasisDt) || ' nDuration:' || to_char(nDuration));
      if (nHot > 0) then

         sp_logger('PROCESS_GEO_PROBE' , 'CHK => nSubsDuration:' || to_char(nSubsDuration) || ' vCustType:' || vCustType || ' nDuration:' || to_char(nDuration) || ' nCurrHot:' || to_char(nCurrHot) || ' nHot:' || to_char(nHot));
         --if ((nSubsDuration is null) or (nSubsDuration=0)) and ((nDuration is null) or (nDuration=0))then
         --      dDeactivationDt := null;
         --else
            if dSubsDeactivationDt is not null then
               dDeactivationDt := least(dSubsDeactivationDt, (trunc(sysdate)+nDuration));
               --dDeactivationDt := dSubsDeactivationDt;
               dDeactBasisDt   := null;
            else
               dDeactivationDt := trunc(sysdate)+nvl(nDuration,180);
               dDeactBasisDt   := trunc(sysdate);
            end if;
         --end if;
         if (nCurrHot <> nHot) then
            nRetr := 131;
            UPDATE sim_activation
            SET    deactivation_dt = dDeactivationDt,
                   deact_basis_dt  = nvl(dDeactBasisDt, deact_basis_dt),
                   hot_country_cnt = 1,
                   hot_country_dt  = trunc(sysdate),
                   hot_country_max_duration = (dDeactivationDt-trunc(sysdate)),
                   hot_country_duration = (dDeactivationDt-trunc(sysdate)),
                   country = vCountry,
                   country_name = vCountryName,
                   last_country_chk = trunc(sysdate)
            WHERE  msisdn = nMsisdn;
         else
            if nvl(vCurrCountry,'xxx') <> nvl(vCountry, 'zzz') then
               nRetr := 131;
            else
               nRetr := 1;
            end if;
            UPDATE sim_activation
            SET    hot_country_cnt = nvl(hot_country_cnt,0) + 1,
                   country = vCountry,
                   country_name = vCountryName,
                   last_country_chk = trunc(sysdate)
            WHERE  msisdn = nMsisdn;
         end if;
         --if (nSubsBreathingPeriod > 0) and (nSubsBreathingPeriod_dt is not null) and
         --   (trunc(sysdate) < nSubsBreathingPeriod_dt)
         --then
         --   INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin )
         --   VALUES (manual_enrollment_log_seq.nextval, nMsisdn, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, user, sysdate, NULL, 'TRIGGER', NULL, 'Due to Breathing period', NULL, 0, 0, 'WEB');
         --   nRetr := 1;
         --end if;
      else
         --if (nSubsDuration is null) or (nSubsDuration=0)then
         --   dDeactivationDt := null;
         --else
         --   dDeactivationDt := dDeactBasisDt+nSubsDuration;
         --end if;
         if (trunc(sysdate)-dDeactBasisDt) < nHotDuration then
            dDeactivationDt := null;
         else
            dDeactivationDt := dSubsDeactivationDt;
         end if;
         if (nCurrHot > 0) and (nHot = 0) then
            if dDeactivationDt is null then
               nRetr := 130;
            else
               nRetr := 133;
            end if;
            UPDATE sim_activation
            SET    deactivation_dt = dDeactivationDt,
                   hot_country_cnt = 0,
                   hot_country_dt  = null,
                   hot_country_max_duration = null,
                   hot_country_duration = null,
                   country = vCountry,
                   country_name = vCountryName,
                   last_country_chk = trunc(sysdate)
            WHERE  msisdn = nMsisdn;
         else
            if nvl(vCurrCountry,'xxx') <> nvl(vCountry, 'zzz') then
               if dDeactivationDt is null then
                  nRetr := 130;
               else
                  nRetr := 133;
               end if;
            else
               nRetr := 1;
            end if;
            UPDATE sim_activation
            SET    hot_country_cnt = 0,
                   hot_country_dt  = null,
                   hot_country_max_duration = null,
                   hot_country_duration = null,
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
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_GEO_PROBE2" (
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
   --        133 - change to non-special country with set duration
   nRetr         Number;
   nHotCtr       Number;
   nHot          Number;
   nDuration     Number;
   nSubsDuration Number;
   nBreathing    Number;
   nMsisdn       Number(12);
   vCountry      Varchar2(30);
   vCustType     Varchar2(30);
   vCurrCountry  Varchar2(60);
   vCountryName  Varchar2(60);
   nCurrHot      Number;
   nDeActPending Number;
   dDeactivationDt Date;
   dSubsActivationDt       Date;
   dSubsDeactivationDt     Date;
   nSubsBreathingPeriod    Number;
   nSubsBreathingPeriod_dt Date;
   dDeactBasisDt           Date;
begin
   nRetr := 1;
   dbms_output.put_line('START => p_imsi:' || p_imsi || ', p_sccp:' || to_char(p_sccp));
   --sp_logger('PROCESS_GEO_PROBE' , 'START => p_imsi:' || p_imsi || ', p_sccp:' || to_char(p_sccp));

   begin
      SELECT hot, country_code
      INTO   nHot, vCountry
      FROM   plmn_mapping
      WHERE  addr = SUBSTR(p_sccp, 1, LENGTH(addr))
      AND    rownum = 1;
   exception
     when no_data_found then
        nRetr := SQLCODE;
        p_retr := nRetr*-1;
        dbms_output.put_line('END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to PLMN_MAPPING 1 nRetr:' || to_char(p_retr));
        --sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to PLMN_MAPPING 1 nRetr:' || to_char(p_retr));
        return;
     when others then
        nRetr := SQLCODE;
        p_retr := nRetr;
        dbms_output.put_line('END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to PLMN_MAPPING 2 nRetr:' || to_char(p_retr));
        --sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to PLMN_MAPPING 2 nRetr:' || to_char(p_retr));
        return;
   end;

   begin
      SELECT nvl(hot_country_cnt,0), country, msisdn, duration, activation_dt, deactivation_dt,
             breathing_period, breathing_period_dt, customer_type, nvl(deact_basis_dt, trunc(sysdate))
      INTO   nHotCtr, vCurrCountry, nMsisdn, nSubsDuration, dSubsActivationDt, dSubsDeactivationDt,
             nSubsBreathingPeriod, nSubsBreathingPeriod_dt, vCustType, dDeactBasisDt
      FROM   sim_activation
      WHERE  imsi = p_imsi;
   exception
     when no_data_found then
        nRetr := SQLCODE;
        p_retr := nRetr*-1;
        dbms_output.put_line('END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to SIM_ACTIVATION 1 nRetr:' || to_char(p_retr));
        --sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to SIM_ACTIVATION 1 nRetr:' || to_char(p_retr));
        return;
     when others then
        nRetr := SQLCODE;
        p_retr := nRetr;
        dbms_output.put_line('END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to SIM_ACTIVATION 2 nRetr:' || to_char(p_retr));
        --sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' on select to SIM_ACTIVATION 2 nRetr:' || to_char(p_retr));
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

      begin
         SELECT breathing_period, nvl(max_duration,9999)
         INTO   nBreathing, nDuration
         FROM   customer_types
         WHERE  customer_type = vCustType
         AND    rownum = 1;
      exception
        when no_data_found then
           nRetr := SQLCODE;
           p_retr := nRetr*-1;
           dbms_output.put_line('END =>  ' || SQLERRM || ' p_imsi:' || p_imsi  || ' vCustType:' || vCustType || ' on select to CUSTOMER_TYPES 1 nRetr:' || to_char(p_retr));
           --sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi  || ' vCustType:' || vCustType || ' on select to CUSTOMER_TYPES 1 nRetr:' || to_char(p_retr));
           return;
        when others then
           nRetr := SQLCODE;
           p_retr := nRetr;
           dbms_output.put_line('END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' vCustType:' || vCustType || ' on select to CUSTOMER_TYPES 2 nRetr:' || to_char(p_retr));
           --sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' vCustType:' || vCustType || ' on select to CUSTOMER_TYPES 2 nRetr:' || to_char(p_retr));
           return;
      end;

      dbms_output.put_line('CHECK => nHot:' || to_char(nHot) || ' nCurrHot:' || to_char(nCurrHot) || ' dDeactBasisDt:' || to_char(dDeactBasisDt) || ' nDuration:' || to_char(nDuration));
      --sp_logger('PROCESS_GEO_PROBE' , 'CHECK => nHot:' || to_char(nHot) || ' nCurrHot:' || to_char(nCurrHot) || ' dDeactBasisDt:' || to_char(dDeactBasisDt) || ' nDuration:' || to_char(nDuration));
      if (nHot > 0) then

         dbms_output.put_line('CHK => nSubsDuration:' || to_char(nSubsDuration) || ' vCustType:' || vCustType || ' nDuration:' || to_char(nDuration) || ' nCurrHot:' || to_char(nCurrHot) || ' nHot:' || to_char(nHot));
         --sp_logger('PROCESS_GEO_PROBE' , 'CHK => nSubsDuration:' || to_char(nSubsDuration) || ' vCustType:' || vCustType || ' nDuration:' || to_char(nDuration) || ' nCurrHot:' || to_char(nCurrHot) || ' nHot:' || to_char(nHot));
         --if ((nSubsDuration is null) or (nSubsDuration=0)) and ((nDuration is null) or (nDuration=0))then
         --      dDeactivationDt := null;
         --else
            if dSubsDeactivationDt is not null then
               dDeactivationDt := least(dSubsDeactivationDt, (trunc(sysdate)+nDuration));
               --dDeactivationDt := dSubsDeactivationDt;
               dDeactBasisDt   := null;
            else
               dDeactivationDt := trunc(sysdate)+nvl(nDuration,180);
               dDeactBasisDt   := trunc(sysdate);
            end if;
         --end if;
         if (nCurrHot <> nHot) then
            nRetr := 131;
            UPDATE sim_activation
            SET    deactivation_dt = dDeactivationDt,
                   deact_basis_dt  = nvl(dDeactBasisDt, deact_basis_dt),
                   hot_country_cnt = 1,
                   hot_country_dt  = trunc(sysdate),
                   hot_country_max_duration = (dDeactivationDt-trunc(sysdate)),
                   hot_country_duration = (dDeactivationDt-trunc(sysdate)),
                   country = vCountry,
                   country_name = vCountryName,
                   last_country_chk = trunc(sysdate)
            WHERE  msisdn = nMsisdn;
         else
            if nvl(vCurrCountry,'xxx') <> nvl(vCountry, 'zzz') then
               nRetr := 131;
            else
               nRetr := 1;
            end if;
            UPDATE sim_activation
            SET    hot_country_cnt = nvl(hot_country_cnt,0) + 1,
                   country = vCountry,
                   country_name = vCountryName,
                   last_country_chk = trunc(sysdate)
            WHERE  msisdn = nMsisdn;
         end if;
         if (nSubsBreathingPeriod > 0) and (nSubsBreathingPeriod_dt is not null) and
            (trunc(sysdate) < nSubsBreathingPeriod_dt)
         then
            INSERT INTO MANUAL_ENROLLMENT_LOG ( ID, MSISDN, ENROLLMENT_TYPE, BULK_UPLOAD, TX_DATE, DURATION, STATUS, CS_USERNAME, DT_CREATED, subs_name, subs_group, subs_contact_no, remarks, bulk_grp_id, min_bal, breathing_period, request_origin )
            VALUES (manual_enrollment_log_seq.nextval, nMsisdn, 'DEACTIVATION', 0, trunc(sysdate), 0, 0, user, sysdate, NULL, 'TRIGGER', NULL, 'Due to Breathing period', NULL, 0, 0, 'WEB');
            nRetr := 1;
         end if;
      else
         --if (nSubsDuration is null) or (nSubsDuration=0)then
         --   dDeactivationDt := null;
         --else
         --   dDeactivationDt := dDeactBasisDt+nSubsDuration;
         --end if;
         if (trunc(sysdate)-dDeactBasisDt) < nDuration then
            dDeactivationDt := null;
         else
            dDeactivationDt := dSubsDeactivationDt;
         end if;
         if (nCurrHot > 0) and (nHot = 0) then
            if dDeactivationDt is null then
               nRetr := 130;
            else
               nRetr := 133;
            end if;
            UPDATE sim_activation
            SET    deactivation_dt = dDeactivationDt,
                   hot_country_cnt = 0,
                   hot_country_dt  = null,
                   hot_country_max_duration = null,
                   hot_country_duration = null,
                   country = vCountry,
                   country_name = vCountryName,
                   last_country_chk = trunc(sysdate)
            WHERE  msisdn = nMsisdn;
         else
            if nvl(vCurrCountry,'xxx') <> nvl(vCountry, 'zzz') then
               if dDeactivationDt is null then
                  nRetr := 130;
               else
                  nRetr := 133;
               end if;
            else
               nRetr := 1;
            end if;
            UPDATE sim_activation
            SET    hot_country_cnt = 0,
                   hot_country_dt  = null,
                   hot_country_max_duration = null,
                   hot_country_duration = null,
                   country = vCountry,
                   country_name = vCountryName,
                   last_country_chk = trunc(sysdate)
            WHERE  msisdn = nMsisdn;
         end if;
      end if;
      --commit;

      dbms_output.put_line('END => p_imsi:' || p_imsi || ' nRetr:' || to_char(nRetr) || ' nMsisdn:' || to_char(nMsisdn) || ' nSubsDuration:' || to_char(nSubsDuration) || ' nDuration:' || to_char(nDuration));
      --sp_logger('PROCESS_GEO_PROBE' , 'END => p_imsi:' || p_imsi || ' nRetr:' || to_char(nRetr) || ' nMsisdn:' || to_char(nMsisdn) || ' nSubsDuration:' || to_char(nSubsDuration) || ' nDuration:' || to_char(nDuration));
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
        dbms_output.put_line('END => p_imsi:' || p_imsi || ' nRetr:' || to_char(nRetr) || ' nMsisdn:' || to_char(nMsisdn) || ' nSubsDuration:' || to_char(nSubsDuration) || ' nDuration:' || to_char(nDuration));
        --sp_logger('PROCESS_GEO_PROBE' , 'END =>  ' || SQLERRM || ' p_imsi:' || p_imsi || ' OERR_N nRetr:' || to_char(p_retr));
     when others then
        nRetr := SQLCODE;
        p_retr := nRetr;
        dbms_output.put_line('END => ' || SQLERRM || ' p_imsi:' || p_imsi || ' OERR_O nRetr:' || to_char(p_retr) || ' nSubsDuration:' || to_char(nSubsDuration) || ' nDuration:' || to_char(nDuration));
        --sp_logger('PROCESS_GEO_PROBE' , 'END => ' || SQLERRM || ' p_imsi:' || p_imsi || ' OERR_O nRetr:' || to_char(p_retr) || ' nSubsDuration:' || to_char(nSubsDuration) || ' nDuration:' || to_char(nDuration) );
   end;

end sp_process_geo_probe2;
/
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_ODS" (
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
show err



CREATE OR REPLACE PROCEDURE "SP_PROCESS_TRAN" (
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
   nARDS_Duration          Number;
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
   nRoamerStatus           Number;
   vServiceId              Varchar2(30);
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

   -- 20    TRAN_TYPE_USURF_ON
   -- 21    TRAN_TYPE_USURF_OFF
   -- 22    TRAN_TYPE_USURF_STATUS

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

            select nvl(max_duration,nDefaultDuration) max_duration, breathing_period, min_balance, daily_balance, ards_tmp_duration
            into   nCustDuration, nCustBreathingPeriod, nCustMinBal, nDaily_bal, nARDS_Duration
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
            if nARDS_Duration is null or nARDS_Duration <= 0 then
               nCustDuration := 2;
            else
               nCustDuration := nvl(nARDS_Duration,2);
            end if;
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
            values (ARDSResponse_id_seq.nextval, vIMSI, 'ACTIVATION', 0, 1, sysdate, null);
         exception
            when others then
                 sp_logger('PROCESS' , 'ARDS ACTIVATION Error: IMSI='  || vIMSI || ' ORA:' || SQLCODE );
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
                request_origin, cs_username, created_by, dt_created, updated_by, dt_updated, notify_date, deactivation_reason )
         SELECT sim_activation_history_seq.nextval, msisdn, country, subscriber_name, vSubsName, activation_dt, reactivation_dt,
                'INACTIVE', trunc(sysdate), hot_country_cnt, hot_country_dt, hot_country_duration, hot_country_max_duration,
                vRemarks, daily_bal, min_bal, duration, extended, breathing_period, breathing_period_dt,
                last_daily_bal, last_daily_bal_chk, last_daily_bal_ctr, last_country_chk,
                imsi, customer_type, country_name, id sim_activation_id, activation_tm, sysdate,
                vSource, nvl(vCS_Username,cs_username), created_by, sysdate, null, null, notify_date,  nvl(vOtherType, 'GROAM OFF')
         FROM   sim_activation
         WHERE  msisdn = p_msisdn;
      exception
         when dup_val_on_index then null;
      end;
      UPDATE sim_activation
      SET    deactivation_dt = trunc(sysdate),
             deactivation_reason = nvl(vOtherType, 'GROAM OFF'),
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
         begin
            insert into ARDSResponse (id, imsi, enrollment_type, status, cause, start_tm, end_tm)
            values (ARDSResponse_id_seq.nextval, vIMSI, 'DEACTIVATION', 0, 3, sysdate, null);
         exception
            when others then
                 sp_logger('PROCESS' , 'ARDS DEACTIVATION Error: IMSI='  || vIMSI || ' ORA:' || SQLCODE);
         end;
      else
         begin
            insert into ARDSResponse (id, imsi, enrollment_type, status, cause, start_tm, end_tm)
            values (ARDSResponse_id_seq.nextval, vIMSI, 'DEACTIVATION', 0, 1, sysdate, null);
         exception
            when others then
                 sp_logger('PROCESS' , 'ARDS DEACTIVATION Error: IMSI='  || vIMSI || ' ORA:' || SQLCODE);
         end;
      end if;

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

   --  19    TRAN_TYPE_NO_ARDS
   elsif (p_trantype = 19) then

      begin
         INSERT INTO sim_activation_history
              ( id, msisdn, country, subscriber_name, name_of_req_party, activation_dt, reactivation_dt,
                status, deactivation_dt, hot_country_cnt, hot_country_dt, hot_country_duration, hot_country_max_duration,
                remarks, daily_bal, min_bal, duration, extended, breathing_period, breathing_period_dt,
                last_daily_bal, last_daily_bal_chk, last_daily_bal_ctr, last_country_chk,
                imsi, customer_type, country_name, sim_activation_id, activation_tm, deactivation_tm,
                request_origin, cs_username, created_by, dt_created, updated_by, dt_updated, notify_date, deactivation_reason  )
         SELECT sim_activation_history_seq.nextval, msisdn, country, subscriber_name, null, activation_dt, reactivation_dt,
                'INACTIVE', trunc(sysdate), hot_country_cnt, hot_country_dt, hot_country_duration, hot_country_max_duration,
                null, daily_bal, min_bal, duration, extended, breathing_period, breathing_period_dt,
                last_daily_bal, last_daily_bal_chk, last_daily_bal_ctr, last_country_chk,
                imsi, customer_type, country_name, id sim_activation_id, activation_tm, sysdate,
                'ARDS', null, created_by, sysdate, null, null, notify_date, 'GROAM NO'
         FROM   sim_activation
         WHERE  msisdn = p_msisdn;
      exception
         when dup_val_on_index then null;
      end;
      UPDATE sim_activation
      SET    deactivation_dt = trunc(sysdate),
             deactivation_reason = 'GROAM NO',
             deactivation_tm = sysdate,
             remarks = vRemarks,
             status = 'INACTIVE',
             request_origin = 'ARDS'
      WHERE  msisdn = p_msisdn
      RETURNING IMSI INTO vIMSI;

      begin
         insert into ARDSResponse (id, imsi, enrollment_type, status, cause, start_tm, end_tm)
         values (ARDSResponse_id_seq.nextval, vIMSI, 'DEACTIVATION', 0, 1, sysdate, null);
      exception
         when others then
              sp_logger('PROCESS' , 'ARDS DEACTIVATION Error: IMSI='  || vIMSI || ' ORA:' || SQLCODE);
      end;

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

   --  20    TRAN_TYPE_USURF_ON
   elsif (p_trantype = 20) then
      nRoamerStatus := sf_check_roamer_status(p_msisdn);
      if nRoamerStatus = 1 then
         nRetr := 150;
         begin
            select service_id
            into   vServiceId
            from   usurf_countries
            where  country = upper(p_extra_i_1);
         exception
            when no_data_found then 
               vServiceId := 1333;
          end;
      end if;
   --  22    TRAN_TYPE_USURF_STATUS
   elsif (p_trantype = 22) then
      null;
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
   elsif (p_trantype = 8) and (not bPreAct) then
      --p_extra_o_2 := to_char(nActivationDt, 'MM/DD/YYYY');
      p_extra_o_1 := to_char(sysdate, 'MM/DD/YYYY');
      -- bluemoon update
      if sf_check_usurf_status(p_msisdn,p_extra_i_1)=2 then
         update request_log 
         set    status=0, step_no=0
         where  a_no =p_msisdn
         and    status=2
         and    step_no = -1
         and    tran_type = 20
         and    tran_dt >= trunc(sysdate)-7
         and    rownum = 1;
      end if;
   elsif (p_trantype = 17) then
      p_extra_o_1 := to_char(nMsisdn);
   elsif (p_trantype = 18) then
      p_extra_o_1 := to_char(sysdate, 'MM/DD/YYYY');
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

   if (p_trantype = 20) then
      p_extra_o_1 := p_extra_i_1;
      p_extra_o_2 := p_extra_i_2;
      p_extra_o_3 := vServiceId;
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
show err



CREATE OR REPLACE PROCEDURE "SP_USURF_ACTIVATION" (
    p_retr      out number,
    p_partner   out varchar2,
    p_exptime   out varchar2,
    p_expdate   out varchar2,
    p_msisdn    in  varchar2,
    p_country   in  varchar2,
    p_duration  in  number,
    p_nf_status in  number
   ) is
   vPartner Varchar2(30);
   vTz Varchar2(10);
begin
   sp_logger('USURF_ACTIVATION' , 'START =>'      ||
                                   ' p_nf_status :' || to_char(p_nf_status) ||
                                   ' p_msisdn:' || p_msisdn ||
                                   ' p_country:' || p_country ||
                                   ' p_duration:' || to_char(p_duration));

   if p_nf_status < 0 then
      delete from usurf_activation
      where  msisdn = p_msisdn
      and    country = upper(p_country);
      commit;
      p_partner := '';
      p_exptime := '';
      p_expdate := '';
      p_retr := 1;
   else
      update usurf_activation
      set    status = 'ACTIVE',
             denom = p_duration,
             activation_dt = sysdate
      where  msisdn = p_msisdn
      and    country = upper(p_country);
      if sql%notfound then
         begin
            insert into USURF_ACTIVATION (id, msisdn, country, denom, activation_dt, status, dt_created, created_by)
            values (usurf_activation_seq.nextval, p_msisdn, upper(p_country), p_duration, sysdate, 'ACTIVE', sysdate, user);
         exception
            when dup_val_on_index then null;
            when others then null;
         end;
      end if;
      commit;
      
      begin
         select roaming_partner, tz
         into   vPartner, vTz
         from   usurf_countries
         where  country = upper(p_country);
      exception
         when others then
            vPartner := 'Globe';
            vTz := 'UTC';
      end;
      p_partner := vPartner;
      p_exptime := to_char(sysdate+p_duration, 'HH24:MI') || ' ' || vTz;
      p_expdate := to_char(trunc(sysdate+p_duration), 'MM-DD-YYYY');
      p_retr := 1;
   end if;
   sp_logger('USURF_ACTIVATION' , 'END =>'      ||
                                  ' p_retr :  ' || to_char(p_retr) ||
                                  ' p_msisdn: ' || p_msisdn ||
                                  ' p_country:' || p_country ||
                                  ' p_partner:' || p_partner ||
                                  ' p_exptime:' || p_exptime ||
                                  ' p_expdate:' || p_expdate);
end sp_usurf_activation;
/
show err



CREATE OR REPLACE PROCEDURE "SP_USURF_DEACTIVATION" (
    p_retr       out number,
    p_msisdn     in  varchar2,
    p_service_id in  varchar2
   ) is
   vCountry Varchar2(30);
begin
   select country
   into   vCountry
   from   usurf_countries
   where  service_id = p_service_id;
   update usurf_activation
   set    status = 'INACTIVE',
          deactivation_dt = sysdate,
          deactivation_reason = 'NF'
   where  msisdn = p_msisdn
   and    country = vCountry;
   commit;
   p_retr := 1;
exception 
   when others then 
      p_retr := 1;
end sp_usurf_deactivation;
/
show err



CREATE OR REPLACE PROCEDURE "SP_USURF_DEACTIVATE_ALL" (
    p_retr       out number,
    p_service_id out varchar2,
    p_duration   out varchar2,
    p_msisdn     in  varchar2
   ) is
   vServiceID Varchar2(130);
   vDuration  Varchar2(130);
begin
   for i in (select service_id, duration from usurf_activation where msisdn = p_msisdn and status = 'ACTIVE') loop
      if vServiceId is null then
         vServiceID := to_char(i.service_id);
      else
         vServiceID := vServiceID || ',' || to_char(i.service_id);
      end if;
      if vServiceId is null then
         vDuration := to_char(i.duration);
      else
         vDuration := vDuration || ',' || to_char(i.duration);
      end if;
   end loop;

   if vServiceId is not null then
      update usurf_activation
      set    status = 'INACTIVE',
             deactivation_dt = sysdate,
             deactivation_reason = 'GROAM OFF'
      where  msisdn = p_msisdn
      and    status = 'ACTIVE';
      commit;
      p_retr := 1;
      p_service_id := vServiceID;
      p_duration   := vDuration;
   else
      p_retr := 0;
   end if;
exception 
   when others then 
      p_retr := 0;
end sp_usurf_deactivate_all;
/
show err









PROMPT creating FUN2 TRIGGERs
PROMPT creating TRIGGER "ACL_INSERT_TRIGGER"
PROMPT creating TRIGGER "BLACKLISTED_LOG_INSERT_TRIGGER"
PROMPT creating TRIGGER "COUNTRIES_INSERT_TRIGGER"
PROMPT creating TRIGGER "COUNTRIES_UPDATE_TRIGGER"
PROMPT creating TRIGGER "CUST_TYPES_INSERT_TRIGGER"
PROMPT creating TRIGGER "CUST_TYPES_MAP_INSERT_TRIGGER"
PROMPT creating TRIGGER "GEO_PROBE_LOG_TRIGGER"
PROMPT creating TRIGGER "HLR_SERVERS_INSERT_TRIGGER"
PROMPT creating TRIGGER "HLR_SVR_IMSI_INSERT_TRIGGER"
PROMPT creating TRIGGER "HLR_SVR_IMSI_UPDATE_TRIGGER"
PROMPT creating TRIGGER "HLR_SVR_MAPPING_INSERT_TRIGGER"
PROMPT creating TRIGGER "IN_SERVERS_INSERT_TRIGGER"
PROMPT creating TRIGGER "IN_SVR_MAPPING_INSERT_TRIGGER"
PROMPT creating TRIGGER "KEYWORDREQUEST_INSERT_TRIGGER"
PROMPT creating TRIGGER "KEYWORDS_INSERT_TRIGGER"
PROMPT creating TRIGGER "KEYWORD_MSG_INSERT_TRIGGER"
PROMPT creating TRIGGER "MODULES_INSERT_TRIGGER"
PROMPT creating TRIGGER "PASS_LOG_INSERT_TRIGGER"
PROMPT creating TRIGGER "PLMN_MAPPING_INSERT_TRIGGER"
PROMPT creating TRIGGER "PLMN_MAPPING_UPDATE_TRIGGER"
PROMPT creating TRIGGER "PREACTIVATION_TRIGGER"
PROMPT creating TRIGGER "REQUEST_LOG_INSERT_TRIGGER"
PROMPT creating TRIGGER "RESPONSE_LOG_INSERT_TRIGGER"
PROMPT creating TRIGGER "SERVICES_INSERT_TRIGGER"
PROMPT creating TRIGGER "SIM_ACT_NOTIFY_DT_TRIGGER"
PROMPT creating TRIGGER "SSET_MAPPING_INSERT_TRIGGER"
PROMPT creating TRIGGER "TRANSACTION_LOG_INSERT_TRIGGER"
PROMPT creating TRIGGER "USER_LOG_INSERT_TRIGGER"
PROMPT creating TRIGGER "USER_MASTER_INSERT_TRIGGER"
PROMPT creating TRIGGER "USER_ROLES_INSERT_TRIGGER"
PROMPT creating TRIGGER "WHITELISTED_LOG_INSERT_TRIGGER"
PROMPT creating TRIGGER "ZONES_INSERT_TRIGGER"
PROMPT creating TRIGGER "ZONES_UPDATE_TRIGGER"



CREATE OR REPLACE TRIGGER "ACL_INSERT_TRIGGER"
before insert on acl
for each row
begin
   if :new.id is null or :new.id = 0
   then
 select acl_seq.nextval into :new.id from dual;
   end if;
end acl_insert_trigger;
/
show err



CREATE OR REPLACE TRIGGER "BLACKLISTED_LOG_INSERT_TRIGGER"
before insert on blacklisted_log
for each row
begin
   :new.tran_dt := trunc(nvl(:new.dt_created, sysdate));
   insert into BLACKLISTED_WHITELISTED_LOG
   values (:new.msisdn, 'BLACKLISTED', :new.dt_created, :new.remarks, :new.tran_dt, :new.created_by, :new.bulk_grp_id );
end BLACKLISTED_LOG_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "COUNTRIES_INSERT_TRIGGER"
before insert on countries
for each row
begin
   if :new.id is null or :new.id = 0
   then
      select countries_seq.nextval into :new.id from dual;
   end if;
   :NEW.country_name_upper := upper(nvl(:NEW.country_name, :NEW.country_code));
end COUNTRIES_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "COUNTRIES_UPDATE_TRIGGER"
before update of zone_id on countries
for each row
declare
   nBreathing   Number;
   nMaxDuration Number;
begin
   -- get
   begin
      select breathing_period, max_duration
      into   nBreathing, nMaxDuration
      from   zones
      where  zone_id = :new.zone_id;
   exception
      when no_data_found then null;
   end;

   -- update
   update plmn_mapping
   set    breathing_period = nBreathing,
          max_duration = nMaxDuration
   where  country_code = :new.country_code;

end COUNTRIES_UPDATE_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "CUST_TYPES_INSERT_TRIGGER"
before insert on customer_types
for each row
begin
   if :new.id is null or :new.id = 0
   then
 select customer_types_seq.nextval into :new.id from dual;
   end if;
end CUST_TYPES_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "CUST_TYPES_MAP_INSERT_TRIGGER"
before insert on customer_types_mapping
for each row
begin
   if :new.id is null or :new.id = 0
   then
 select customer_types_mapping_seq.nextval into :new.id from dual;
   end if;
end CUST_TYPES_MAP_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "GEO_PROBE_LOG_TRIGGER"
before insert on geo_probe_log
for each row
begin
   :NEW.tx_date    := trunc(sysdate);
   :NEW.dt_created := sysdate;
end GEO_PROBE_LOG_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "HLR_SERVERS_INSERT_TRIGGER"
before insert on hlr_servers
for each row
begin
   if :new.id is null or :new.id = 0
   then
      select hlr_servers_seq.nextval into :new.id from dual;
   end if;
end HLR_SERVERS_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "HLR_SVR_IMSI_INSERT_TRIGGER"
before insert on HLR_IMSI_MAPPING
for each row
begin
   if :new.id is null or :new.id = 0
   then
      select hlr_imsi_mapping_seq.nextval into :new.id from dual;
   end if;
   begin
     select server_ip
     into   :new.server_ip
     from   hlr_servers
     where  server_name = :new.server_name;
   end;
end HLR_SVR_IMSI_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "HLR_SVR_IMSI_UPDATE_TRIGGER"
before update on HLR_IMSI_MAPPING
for each row
begin
   if nvl(:new.server_name,'-x') <> nvl(:old.server_name,'-x')
   then
     select server_ip
     into   :new.server_ip
     from   hlr_servers
     where  server_name = :new.server_name;
   end if;
end HLR_SVR_IMSI_UPDATE_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "HLR_SVR_MAPPING_INSERT_TRIGGER"
before insert on hlr_server_mapping
for each row
begin
   if :new.id is null or :new.id = 0
   then
      select hlr_server_mapping_seq.nextval into :new.id from dual;
   end if;
end HLR_SVR_MAPPING_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "IN_SERVERS_INSERT_TRIGGER"
before insert on in_servers
for each row
begin
   if :new.id is null or :new.id = 0
   then
      select in_servers_seq.nextval into :new.id from dual;
   end if;
end IN_SERVERS_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "IN_SVR_MAPPING_INSERT_TRIGGER"
before insert on in_server_mapping
for each row
begin
   if :new.id is null or :new.id = 0
   then
      select in_server_mapping_seq.nextval into :new.id from dual;
   end if;
end IN_SVR_MAPPING_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "KEYWORDREQUEST_INSERT_TRIGGER"
before insert on keywordRequest
for each row
begin
   --if (:new.tran_type in (8,9)) and (:new.imsi is null or :new.imsi= '') then
   if (:new.imsi is null or :new.imsi = '') then
      begin
         select imsi
         into  :new.imsi
         from  sim_activation
         where msisdn = :new.msisdn;
      exception
         when others then null;
      end;
   end if;

end keywordRequest_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "KEYWORDS_INSERT_TRIGGER"
before insert on keywords
for each row
begin
   if :new.id is null or :new.id = 0
   then
      select keywords_seq.nextval into :new.id from dual;
   end if;
end KEYWORDS_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "KEYWORD_MSG_INSERT_TRIGGER"
before insert on keyword_msg
for each row
begin
   if :new.id is null or :new.id = 0
   then
 select keyword_msg_seq.nextval into :new.id from dual;
   end if;
end KEYWORD_MSG_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "MODULES_INSERT_TRIGGER"
before insert on modules
for each row
 begin
   if :new.id is null or :new.id = 0
   then
 select modules_seq.nextval into :new.id from dual;
   end if;
end modules_insert_trigger;
/
show err



CREATE OR REPLACE TRIGGER "PASS_LOG_INSERT_TRIGGER"
before insert on pass_log
for each row
begin
   if :new.id is null or :new.id = 0
   then
 select pass_log_seq.nextval into :new.id from dual;
   end if;
end pass_log_insert_trigger;
/
show err



CREATE OR REPLACE TRIGGER "PLMN_MAPPING_INSERT_TRIGGER"
before insert on plmn_mapping
for each row
begin
   if :new.id is null or :new.id = 0
   then
      select plmn_mapping_seq.nextval into :new.id from dual;
   end if;

   -- get
   begin
      select breathing_period, max_duration
      into   :new.breathing_period, :new.max_duration
      from   zones z
      where  exists ( select 1
      from   countries c
      where  c.country_code = :new.country_code
      and    c.zone_id = z.zone_id );
   exception
      when no_data_found then null;
   end;

end PLMN_MAPPING_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "PLMN_MAPPING_UPDATE_TRIGGER"
before update of country_code on plmn_mapping
for each row
begin
   -- get
   begin
      select breathing_period, max_duration
      into   :new.breathing_period, :new.max_duration
      from   zones z
      where  exists ( select 1
      from   countries c
      where  c.country_code = :new.country_code
      and    c.zone_id = z.zone_id );
   exception
      when no_data_found then null;
   end;

end PLMN_MAPPING_UPDATE_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "PREACTIVATION_TRIGGER"
before insert on manual_enrollment_log
for each row
begin
   :new.tx_date := trunc(:new.tx_date);
   if :new.enrollment_type = 'ACTIVATION' and :new.tx_date > trunc(sysdate)then
         begin
            insert into sim_activation (id, msisdn, activation_dt, status, remarks, request_origin, duration, min_bal, daily_bal)
            values (sim_activation_seq.nextval, :new.msisdn, trunc(:new.tx_date), 'PENDING', :new.remarks, :new.request_origin,
                    :new.duration, :new.min_bal, 0 );
         exception
            when dup_val_on_index then
                UPDATE sim_activation
                SET    activation_dt = trunc(:new.tx_date),
                       deactivation_dt = null,
                       status = 'PENDING',
                       remarks = :new.remarks
                WHERE  msisdn = :new.msisdn;
            when others then null;
         end;
   end if;
   if :new.msisdn <= 0 then
      declare
         nMSISDN Number;
      begin
         select msisdn
         into   nMSISDN
         from   sim_activation
         where  imsi = :new.imsi;
         if nMSISDN is not null then
            :new.msisdn := nMSISDN;
         end if;
      exception
         when no_data_found then null;
      end;
   end if;
end PREACTIVATION_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "REQUEST_LOG_INSERT_TRIGGER"
before insert on request_log
for each row
begin
   if :new.TRAN_DT is null then
      :new.TRAN_DT := trunc(sysdate);
   end if;
   :new.STEP_SEQ := sf_get_process_seq;

   --if (:new.tran_type in (8,9)) and (:new.imsi is null or :new.imsi= '') then
   if (:new.imsi is null or :new.imsi='') and
      (:new.a_no is not null)
   then
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
show err



CREATE OR REPLACE TRIGGER "RESPONSE_LOG_INSERT_TRIGGER"
before insert on response_log
for each row
begin
   if :new.TRAN_DT is null then
      :new.TRAN_DT := trunc(sysdate);
   end if;
end RESPONSE_LOG_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "SERVICES_INSERT_TRIGGER"
before insert on services
for each row
begin
   if :new.id is null or :new.id = 0
   then
 select services_seq.nextval into :new.id from dual;
   end if;
end SERVICES_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "SIM_ACT_NOTIFY_DT_TRIGGER"
before update of deactivation_dt on sim_activation
for each row
begin
   if :new.deactivation_dt is not null then
      :new.notify_date := :new.deactivation_dt-2;
   else
      :new.notify_date := null;
   end if;
end SIM_ACT_NOTIFY_DT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "SSET_MAPPING_INSERT_TRIGGER"
before insert on sset_mapping
for each row
begin
   if :new.id is null or :new.id = 0
   then
      select sset_mapping_seq.nextval into :new.id from dual;
   end if;
   select sset_mapping_sset_id_seq.nextval into :new.sset_id from dual;
end SSET_MAPPING_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "TRANSACTION_LOG_INSERT_TRIGGER"
before insert on transaction_log
for each row
begin
   :new.tran_dt := trunc(nvl(:new.tx_start, sysdate));
end TRANSACTION_LOG_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "USER_LOG_INSERT_TRIGGER"
before insert on user_log
for each row
begin
   if :new.id is null or :new.id = 0
   then
      select user_log_seq.nextval into :new.id from dual;
   end if;
   if :new.user_code is null then
      if :new.logs like 'LOGIN: User=%' then
         :new.user_code := substr(:new.logs,13,instr(:new.logs,'Status',1,1)-13);
      elsif :new.logs like 'LOGIN ERROR: User=%' then
         :new.user_code := substr(:new.logs,19);
      elsif :new.logs like 'LOCK ACCOUNT: User=%' then
         :new.user_code := substr(:new.logs,19);
      elsif :new.logs like 'LOCK ACCOUNT: User=%' then
         :new.user_code := substr(:new.logs,19);
      elsif :new.logs like 'LOGOUT: User=%' then
         :new.user_code := substr(:new.logs,13);
      end if;
   end if;
   if (:new.mod_name = 'Enrollment') or (:new.mod_name like 'Manual%') then
      if :new.logs like '%MSISDN=%' then
         :new.msisdn := substr(:new.logs,instr(:new.logs, 'MSISDN=', 1)+7,12);
      end if;
   end if;

end user_log_insert_trigger;
/
show err



CREATE OR REPLACE TRIGGER "USER_MASTER_INSERT_TRIGGER"
before insert on user_master
for each row
begin
   if :new.user_id is null or :new.user_id = 0
   then
 select user_master_seq.nextval into :new.user_id from dual;
   end if;
end user_master_insert_trigger;
/
show err



CREATE OR REPLACE TRIGGER "USER_ROLES_INSERT_TRIGGER"
before insert on user_roles
for each row
begin
   if :new.id is null or :new.id = 0
   then
 select user_roles_seq.nextval into :new.id from dual;
   end if;
end user_roles_insert_trigger;
/
show err



CREATE OR REPLACE TRIGGER "WHITELISTED_LOG_INSERT_TRIGGER"
before insert on whitelisted_log
for each row
begin
   :new.tran_dt := trunc(nvl(:new.dt_created, sysdate));
   insert into BLACKLISTED_WHITELISTED_LOG
   values (:new.msisdn, 'WHITELISTED', :new.dt_created, :new.remarks, :new.tran_dt, :new.created_by, :new.bulk_grp_id );
end WHITELISTED_LOG_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "ZONES_INSERT_TRIGGER"
before insert on zones
for each row
begin
   if :new.id is null or :new.id = 0
   then
      select zones_seq.nextval into :new.id from dual;
   end if;
end ZONES_INSERT_TRIGGER;
/
show err



CREATE OR REPLACE TRIGGER "ZONES_UPDATE_TRIGGER"
before update of breathing_period,max_duration on zones
for each row
begin
   -- update zones
   update plmn_mapping p
   set    breathing_period = :new.breathing_period,
          max_duration = :new.max_duration
   where  exists ( select 1
   from   countries c
   where  c.country_code = p.country_code
   and    zone_id = :new.zone_id );

end ZONES_UPDATE_TRIGGER;
/
show err
