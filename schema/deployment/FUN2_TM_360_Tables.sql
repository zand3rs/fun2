alter table trigger_owner.services add BRAND VARCHAR2(16) default 'PREPAID';alter table trigger_owner.keyword_msg add BRAND VARCHAR2(16) default 'PREPAID';alter table trigger_owner.request_log add BRAND VARCHAR2(16) default 'PREPAID';alter table trigger_owner.response_log add BRAND VARCHAR2(16) default 'PREPAID';alter table trigger_owner.broadcast_log add BRAND VARCHAR2(16) default 'PREPAID';update trigger_owner.services set BRAND = 'PREPAID';commit;update trigger_owner.keyword_msg set BRAND = 'PREPAID';commit;insert into trigger_owner.services select trigger_owner.services_seq.nextval, 'TM_' || service_code, service_desc, priority+10, status, created_by, dt_created, updated_by, dt_updated,        svc_url, svc_host, svc_port, svc_user, svc_pass,        db_host, db_user, db_pass, timeout_sec, thread_count, max_retry, app_lib, svc_name, expiry, 'TM'from   trigger_owner.serviceswhere  service_code <> 'SSET';commit;insert into trigger_owner.customer_types select trigger_owner.customer_types_seq.nextval, 'TM', daily_balance, max_duration, min_voice, sms_threshold, 'ACTIVE', 'admin', sysdate, null, null, min_balance, ext_min_balance, 	     breathing_period, pre_act_day, num_link, max_inactivity_days, warning_deact_days, ards_1st_notify, ards_2nd_notify, ards_tmp_durationfrom   trigger_owner.customer_typeswhere  customer_type='TRIGGER';commit;insert into trigger_owner.keyword_msgselect trigger_owner.keyword_msg_seq.nextval, keyword, sub_keyword, description, customer_type, message_type, msg_1, msg_2, msg_3, status, 'admin', sysdate, null, null, message_id, 'TM'from keyword_msg;commit;insert into trigger_owner.conditioner_keywords (id, keyword, sub_keyword, status, dt_created, created_by) values (conditioner_keywords_seq.nextval, 'SURF', 'SSAVER', 'ACTIVE', sysdate, user);commit;update trigger_owner.keyword_msg set customer_type='TM' where brand='TM';commit;update trigger_owner.services set svc_url='http://10.170.58.9:28', svc_host=null, svc_user='FUNTWO', svc_pass='TM360fun2!',app_lib='libhlr2.so' where service_code = 'TM_HLR';commit;delete trigger_owner.keyword_msg where message_id like '%SURF%' and brand = 'TM';delete trigger_owner.keyword_msg where message_id like '%FACEBOOK%' and brand = 'TM';delete trigger_owner.keyword_msg where message_id like '%VIBER%' and brand = 'TM';delete trigger_owner.keyword_msg where message_id like 'CANCEL_%' and brand = 'TM';delete trigger_owner.keyword_msg where message_id like 'CHECKLINK_%' and brand = 'TM';delete trigger_owner.keyword_msg where message_id like 'LINK_%' and brand = 'TM';delete trigger_owner.keyword_msg where message_id like 'GEO_PROBE_%' and brand = 'TM';delete trigger_owner.keyword_msg where message_id like 'REG_%' and brand = 'TM';delete trigger_owner.keyword_msg where message_id like 'UNREG_%' and brand = 'TM';set define ^update trigger_owner.keyword_msg set msg_1 = 'Hi, Ka-TM! Tuloy-tuloy pa rin ang calls and texts mo abroad with TM roaming! Para i-ON ang roaming, i-dial ang *143# at piliin ang Roaming & Int''l at least 24 hours bago ka umalis ng bansa. Paalala lang na hindi pwedeng gamitin abroad ang subscription mo sa local promos at ringback tones, at Share-a-Load. Enjoy your trip!' where message_id = 'GROAM_HELP' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Naku, Ka-TM! Naka-OFF na ang roaming mo. Para i-ON ito ulit, i-dial lang ang *143# at piliin ang Roaming & Int''l. Salamat!' where message_id = 'GROAM_NO_PRE_DEACT' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Naku, Ka-TM! Naka-OFF na ang roaming mo. Para i-ON ito ulit, Tumawag sa roaming hotline +6327301212 gamit ang ibang TM/Globe number. Salamat!' where message_id = 'GROAM_OFF_SUCCESSFUL' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Ka-TM, hindi ma-process ang request mo dahil hindi pa active ang iyong roaming.  For assistance, tumawag sa roaming hotline +6327301212 for free.' where message_id = 'GROAM_OFF_NO_ACTIVE_ROAM' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Hi, Ka-TM! Nakuha na namin ang request para i-OFF ang roaming mo. Kung gusto mong magkaron ng signal pag dating abroad, i-ON ang roaming by dialing *143# at piliin ang Roaming & Int''l.' where message_id = 'GROAM_OFF_PRE_DEACT' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Hi, Ka-TM! Natanggap namin ang request na i-cancel ang roaming registration mo. Para makatawag at makapag-text abroad, i-ON ulit ang roaming. Dial *143# at piliin ang Roaming & Int''l. Salamat!' where message_id = 'GROAM_OFF_PRE_ACT_SUCCESSFUL' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Naku, Ka-TM! Naka-OFF na ang iyong roaming. Para makatawag at makapag-text abroad, i-ON ulit ang roaming. Tumawag sa roaming hotline +6327301212 for free. Salamat!' where message_id = 'GROAM_OFF_DEACT_DUE_TO_OVER_STAY' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Hi, Ka-TM! Mawawala na ang roaming signal mo sa loob ng ilang minuto. Para makatawag at makapag-text abroad, i-ON ulit ang roaming. Dial *143# at piliin ang Roaming & Int''l o tumawag sa roaming hotline +6327301212 for free gamit ang ibang TM/Globe number. Salamat!' where message_id = 'ARDS_PROVISIONAL_PROV_LAST_EXPIRY' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Hi, Ka-TM! Naka-OFF na ang iyong roaming. Para makatawag at makapag-text abroad, i-ON ulit ang roaming. Tumawag sa roaming hotline +6327301212 for free gamit ang ibang TM/Globe number. Salamat!' where message_id = 'ARDS_DEACTIVATION_NO_REPLY' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Ka-TM, pwede mo nang i-enjoy ang international roaming service! Para makapag-text at makatawag habang nasa labas ng bansa, siguruhing naka-ON ang roaming mo. Para mag-activate, mag-reply lang ng GROAM YES sa loob ng 12 hours. Para i-check ang roaming rates, dial *143# at piliin ang Roaming & Int''l. Paalala lang na hindi pwedeng gamitin abroad ang subscription mo sa local promos at ringback tones, at Share-a-Load. Salamat!' where message_id = 'ARDS_PROVISIONAL_PROV_1ST_EXPIRY' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Kamusta ang trip mo abroad, Ka-TM? Para makapag-text at makatawag habang nasa labas ng bansa, siguruhing naka-ON ang roaming mo. Para mag-activate, mag-reply lang ng GROAM YES sa loob ng 12 hours. Para i-check ang roaming rates, dial *143# at piliin ang Roaming & Int''l. Paalala lang na hindi pwedeng gamitin abroad ang subscription mo sa local promos at ringback tones, at Share-a-Load. Salamat!' where message_id = 'ARDS_PROVISIONAL_PROV_2ND_EXPIRY' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Hi, Ka-TM! Naka-OFF na ang iyong roaming. Para makatawag at makapag-text abroad, i-ON ulit ang roaming. Tumawag sa roaming hotline +6327301212 for free gamit ang ibang TM/Globe number. Salamat!' where message_id = 'ARDS_ROAM_DEACTIVATED' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Hi, Ka-TM! Nakuha namin ang request para i-ON ang roaming mo. Kapag nasa abroad ka na, may matatanggap kang confirmation message na active na ang roaming mo. Paalala lang na hindi pwedeng gamitin abroad ang subscription mo sa local promos at ringback tones, at Share-a-Load. Para i-check ang roaming rates for calls and texts, dial *143# at piliin ang Roaming & Int''l.' where message_id = 'GROAM_ON_SUCCESSFUL' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Hi, Ka-TM! Ang number na ito ay may pending request pa. Kapag nasa abroad ka na, may matatanggap kang confirmation message na active na ang roaming mo. Paalala lang na hindi pwedeng gamitin abroad ang subscription mo sa local promos at ringback tones, at Share-a-Load. Para i-check ang roaming rates for calls and texts, dial *143# at piliin ang Roaming & Int''l.' where message_id = 'GROAM_ON_INVALID_DUP_PRE_ACT' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Sorry, Ka-TM. Hindi ma-process ang request mo sa ngayon. Subukan ulit maya-maya o tumawag sa roaming hotline +6327301212 for free.' where message_id = 'BLACKLISTED' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Hi, Ka-TM! Nakuha namin ang request para i-ON ang roaming mo. Kapag nasa abroad ka na, may matatanggap kang confirmation message na active na ang roaming mo. Paalala lang na hindi pwedeng gamitin abroad ang subscription mo sa local promos at ringback tones, at Share-a-Load. Para i-check ang roaming rates for calls and texts, dial *143# at piliin ang Roaming & Int''l.' where message_id = 'GROAM_ON_PRE_ACT_SUCCESSFUL' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Sorry, Ka-TM. Hindi ma-process ang request mo sa ngayon. Subukan ulit maya-maya or tumawag sa roaming hotline +6327301212 for free.' where message_id = 'GROAM_ON_UNSUCCESSFUL' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Ka-TM, pwede mo nang i-enjoy ang international roaming service! Para makapag-text at makatawag habang nasa labas ng bansa, siguruhing naka-ON ang roaming mo. Para mag-activate, mag-reply lang ng GROAM YES sa loob ng 12 hours. Para i-check ang roaming rates, dial *143# at piliin ang Roaming & Int''l. Paalala lang na hindi pwedeng gamitin abroad ang subscription mo sa local promos at ringback tones, at Share-a-Load. Salamat!' where message_id = 'ARDS_ROAM_ACTIVATED' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Yehey! Active na ang roaming mo, Ka-TM! Paalala lang na hindi pwedeng gamitin abroad ang subscription mo sa local promos at ringback tones, at Share-a-Load. Para i-check ang roaming rates for calls and texts, dial *143# at piliin ang Roaming & Int''l. Pwedeng ihinto ng TM ang iyong roaming service kung may ma-detect na anumang uri ng abuso sa paggamit nito. Pagbalik sa Pilipinas, i-text ang GROAM OFF sa 2884 para i-deactivate ang roaming. Salamat!' where message_id = 'GROAM_ON_ACTIVATED' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Active na ang roaming mo, Ka-TM! To know more, i-dial ang *143# at piliin ang Roaming & Int''l.' where message_id = 'GROAM_STATUS' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Naku, Ka-TM! Hindi active ang roaming service mo. Kung gusto mong magkaron ng signal pagdating abroad, i-ON ang roaming by dialing *143# at piliin ang Roaming & Int''l.' where message_id = 'GROAM_STATUS_NO_ACTIVE_ROAM' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Active na ang iyong roaming, Ka-TM! To know more, dial *143# at piliin ang Roaming & Int''l.' where message_id = 'GROAM_STATUS_UNLIMITED' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Yehey! Active na ang roaming mo, Ka-TM! Paalala lang na hindi pwedeng gamitin abroad ang subscription mo sa local promos at ringback tones, at Share-a-Load. Para i-check ang roaming rates for calls and texts, dial *143# at piliin ang Roaming & Int''l. Pwedeng ihinto ng TM ang iyong roaming service kung may ma-detect na anumang uri ng abuso sa paggamit nito. Pagbalik sa Pilipinas, i-text ang GROAM OFF sa 2884 para i-deactivate ang roaming. Salamat!' where message_id = 'GROAM_YES_SUCCESSFUL' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = 'Active na ang roaming mo, Ka-TM! To know more, i-dial ang *143# at piliin ang Roaming & Int''l.' where message_id = 'GROAM_ON_ALREADY_ROAMER' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = ' ' where message_id = 'SYSTEM_ERROR' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = ' ' where message_id = 'GROAM_ON_BREATHING_PERIOD' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = ' ' where message_id = 'GROAM_ON_INVALID_PRE_ACT_DATE' and brand = 'TM';update trigger_owner.keyword_msg set msg_1 = ' ' where message_id = 'GROAM_OFF_PRE_DEACT_WARNING' and brand = 'TM';set define & 