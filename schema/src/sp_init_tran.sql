create or replace procedure sp_init_tran (
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
                      ' p_extra_i_3:' || p_extra_i_3);

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
               values (ARDSResponse_id_seq.nextval, vIMSI, 'ACTIVATION', 0, null, sysdate, null);
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
      --   sp_logger('INIT' , 'END => p_trantype :' || to_char(p_trantype) || ' p_msisdn:' || p_msisdn || ' p_req_id:' || to_char(p_req_id) || ' p_ref_id:' || to_char(p_ref_id) || ' p_retr:' || to_char(p_retr) || ' p_extra_o_1: ' || p_extra_o_1 || ', p_extra_o_2:' || p_extra_o_2 || ', p_extra_o_3:' || p_extra_o_3);
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
            values (ARDSResponse_id_seq.nextval, vIMSI, 'DEACTIVATION', 0, null, sysdate, null);
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
