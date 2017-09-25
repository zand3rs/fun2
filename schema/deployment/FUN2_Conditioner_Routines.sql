set define ^
CREATE OR REPLACE FUNCTION TRIGGER_OWNER.SF_CHECK_USURF_STATUS (
   p_msisdn in number,
   p_keyword in varchar2,
   p_sub_keyword in varchar2,
   p_coexist in number
   ) return number is
   nUsurfer Number(1);
   nActivationPending  Number;
   vStatus conditioner_log.status%type;
begin
   nUsurfer := 0;
   select status
   into   vStatus
   from   conditioner_log
   where  msisdn = p_msisdn
   and    promo_code = p_keyword
   and    promo_value = p_sub_keyword
   and    tran_type = 20
   and    status = 3;
   if vStatus = 3 then
      nUsurfer := 1;
   end if;
   return nUsurfer;
exception
   when no_data_found then 
      if p_coexist = 1 then
         begin
            select count(1)
            into   nUsurfer
            from   conditioner_log 
            where  msisdn = p_msisdn
            and    status = 3;
            if nUsurfer = 0 then
               nUsurfer := 0;
            else
               select 0
               into   nUsurfer
               from   conditioner_log a, conditioner_coex b
               where  a.msisdn = p_msisdn
               and    a.promo_code = b.promo_code
               and    a.promo_value = b.promo_value
               and    a.status = 3
			         and    a.tran_type = 20
               and    b.allowed_promo_code = p_keyword
               and    b.allowed_promo_value = p_sub_keyword
               and    rownum = 1;
            end if;
         exception
            when no_data_found then nUsurfer := 1;
         end;
      end if;
      return nUsurfer;
end sf_check_usurf_status;
/
show err




CREATE OR REPLACE FUNCTION TRIGGER_OWNER.SF_IS_VALID_USURF (p_keyword in varchar2, p_sub_keyword in varchar) return number is
   nRetr Number;
begin
   -- 1 - OK
   -- 2 - Invalid Keyword
   nRetr := 2;
   begin
      select 1
      into   nRetr
      from   conditioner_keywords
      where  keyword= upper(p_keyword)
      and    sub_keyword= upper(p_sub_keyword)
      and    status = 'ACTIVE';
   exception
      when no_data_found then nRetr := 2;
      when others then nRetr := 2;
   end;
   return nRetr;
end sf_is_valid_usurf;
/
show err




CREATE OR REPLACE PROCEDURE TRIGGER_OWNER.SP_GET_USURF_SID (
   p_promo_name out varchar, 
   p_service_id out varchar,
   p_keyword in varchar2, 
   p_sub_keyword in varchar) is
   vName conditioner_promos.promo_name%type;
   vID   conditioner_promos.service_id%type;
begin
   vName := '<PROMO NAME>';
   vID   := '1333';
   begin
      select promo_name, service_id
      into   vName, vID
      from   conditioner_promos
      where  promo_code = upper(p_keyword)
      and    promo_value = upper(p_sub_keyword);
   exception
      when no_data_found then null;
      when others then null;
   end;
   p_promo_name := vName;
   p_service_id := vID;
end sp_get_usurf_sid;
/
show err




CREATE OR REPLACE PROCEDURE TRIGGER_OWNER.SP_INIT_TRAN (
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
   vPromoName           conditioner_promos.promo_name%type;
   vNF_ServiceID        conditioner_promos.service_id%type;
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
   --    150 - USURF ok
   --    151 - USURF pre-activation
   --    152 - USURF invalid keyword
   --    153 - USURF coex

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
      if sf_is_valid_usurf(p_extra_i_1, p_extra_i_2)=2 then
         nRetr := 152;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      if sf_check_roamer_status(p_msisdn) = 1 then
         nRetr := 150;
         nRoamerStatus := 0;
      else
         nRetr := 151;
         nRoamerStatus := 3;
      end if;

      -- pre-activation 
      if nRetr = 151 then
         if sf_check_usurf_status(p_msisdn, p_extra_i_1, p_extra_i_2, 1) = 1 then
            nRetr := 153;
         end if;
      end if;

      if nRetr in (150,151) then
         begin
            sp_get_usurf_sid (vPromoName, vNF_ServiceID, p_extra_i_1, p_extra_i_2);
            insert into conditioner_log (id, tran_type, cluster_node, msisdn, promo_code, promo_value, promo_name, service_id, status, request_id, dt_created, created_by)
            values (conditioner_log_seq.nextval, p_trantype, 1, p_msisdn, p_extra_i_1, p_extra_i_2, vPromoName, vNF_ServiceID, nRoamerStatus, p_req_id, sysdate, user);
            commit;
         exception
            when dup_val_on_index then 
               if nRetr = 150 then
                  update conditioner_log
                  set    status = 0, request_id = p_req_id
                  where  tran_type = p_trantype
                  and    msisdn = p_msisdn
                  and    request_id = p_req_id;
               end if;
         end;
      end if;

      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;

   -- 21    TRAN_TYPE_USURF_OFF
   -- 22    TRAN_TYPE_USURF_STATUS
   elsif (p_trantype IN (21,22)) then
      if sf_is_valid_usurf(p_extra_i_1, p_extra_i_2)=2 then
         nRetr := 152;
         p_retr := nRetr;
         sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
         return;
      end if;

      if sf_check_roamer_status(p_msisdn) = 1 then
         nRetr := 150;
         nRoamerStatus := 0;
      else
         nRetr := 151;
         nRoamerStatus := 3;
      end if;

      -- pre-activation
      if nRetr = 151 then
         if sf_check_usurf_status(p_msisdn, p_extra_i_1, p_extra_i_2, 0) = 0 then
            nRetr := 148;
         else
            if (p_trantype = 21) then
               delete from conditioner_log where msisdn = p_msisdn and promo_code = p_extra_i_1 and promo_value = p_extra_i_2;
               commit;
            end if;
         end if;
      elsif nRetr = 150  then
         begin
            sp_get_usurf_sid (vPromoName, vNF_ServiceID, p_extra_i_1, p_extra_i_2);
            insert into conditioner_log (id, tran_type, cluster_node, msisdn, promo_code, promo_value, promo_name, service_id, status, request_id, dt_created, created_by)
            values (conditioner_log_seq.nextval, p_trantype, 1, p_msisdn, p_extra_i_1, p_extra_i_2, vPromoName, vNF_ServiceID, nRoamerStatus, p_req_id, sysdate, user);
         exception
            when dup_val_on_index then 
               update conditioner_log
               set    status = 0, request_id = p_req_id
               where  tran_type = p_trantype
               and    msisdn = p_msisdn
               and    promo_code = p_extra_i_1
               and    promo_value = p_extra_i_2;
         end;
         commit;
      end if;
      p_retr := nRetr;
      sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
      return;
   end if;

   --  9    TRAN_TYPE_GROAM_OFF,
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

   -- 2  - TRAN_TYPE_REG
   -- 4  - TRAN_TYPE_LINK
   -- 8  - TRAN_TYPE_GROAM_ON
   -- 9  - TRAN_TYPE_GROAM_OFF
   -- 10 - TRAN_TYPE_GROAM_EXTEND
   if (p_trantype in (2,4,8,9,10)) then
      p_extra_o_3 := sf_is_sweden_nsn_range(p_msisdn);
   end if;

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





set define ^
CREATE OR REPLACE PROCEDURE TRIGGER_OWNER.SP_PROCESS_TRAN(
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
   vUsurfCountry           Varchar2(30);
   nUsurfDenom             Number;
   nConditionerCount       Number := 0;
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

   -- 17    TRAN_TYPE_YES
   -- 18    TRAN_TYPE_YES_ARDS
   -- 19    TRAN_TYPE_NO_ARDS
   
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

         -- conditioner changes
         select count(1) 
         into   nConditionerCount
         from   conditioner_log
         where  msisdn = p_msisdn
         and    status = 3;
 
         if vSource = 'ARDS' then
            if nConditionerCount = 0  then
               if nARDS_Duration is null or nARDS_Duration <= 0 then
                  nCustDuration := 2;
               else
                  nCustDuration := nvl(nARDS_Duration,2);
               end if;
               dDeactivateDt := trunc(sysdate)+nCustDuration;
               dDeactivateTm := sysdate+nCustDuration;
            end if;
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

   -- 17    TRAN_TYPE_YES
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

   -- 18    TRAN_TYPE_YES_ARDS
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

   -- 19    TRAN_TYPE_NO_ARDS
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

   -- 20    TRAN_TYPE_USURF_ON
   elsif (p_trantype = 20) then
      null;
   -- 21    TRAN_TYPE_USURF_OFF
   elsif (p_trantype = 21) then
      null;
   -- 22    TRAN_TYPE_USURF_STATUS
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
      -- conditioner/bluemoon update
      if nConditionerCount >= 1 then
         update conditioner_log
         set    status = 0
         where  msisdn = p_msisdn AND status = 3;
         commit;
         p_extra_o_2 := 'USURF';
      end if;
   elsif (p_trantype = 9) then
      p_extra_o_1 := vServiceId;
      p_extra_o_2 := '';
      p_extra_o_3 := '';
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
      if nConditionerCount = 0 then
         p_extra_o_2 := '';
      end if;
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
                         ' p_extra_o_3:'       || nvl(p_extra_o_3, 'null=' || to_char(nCustDuration)) ||
                         ' nConditionerCount:' || to_char(nConditionerCount) );

end sp_process_tran;
/
show err

set define &


