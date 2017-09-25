drop table trigger_owner.conditioner_log;
drop table trigger_owner.conditioner_promos;
drop table trigger_owner.conditioner_keywords;
drop table trigger_owner.conditioner_coex;

create sequence trigger_owner.conditioner_log_seq start with 1 increment by 1;

create table trigger_owner.conditioner_log (
  id          number not null, 
  tran_type   number not null,
  cluster_node number default 1 not null, 
  msisdn      varchar2(12) not null, 
  promo_code  varchar2(30) not null, 
  promo_value varchar2(30) not null, 
  promo_name  varchar2(30) not null,
  service_id  varchar2(30) not null,
  status      number(1) default 3 not null,
  request_id  number, 
  dt_created  date  not null,
  created_by  varchar2(30) not null,
  dt_modified  date,
  constraint conditioner_log_pk primary key (id) using index tablespace trigger_indx,
  constraint conditioner_log_uk unique (msisdn,request_id) using index tablespace trigger_indx
 ) tablespace trigger_data;

COMMENT ON COLUMN trigger_owner.conditioner_log.id  IS 'use conditioner_log_seq.nextval when doing an insert stmt';
COMMENT ON COLUMN trigger_owner.conditioner_log.status IS '3 - queued/waiting for ARDS activation, 0 - for sending to NF, 1 - sent to NF, 2 - successfully sent to NF, -1 - Failed transmission to NF';
COMMENT ON COLUMN trigger_owner.conditioner_log.tran_type IS '20 on,21 off ,22 status';

create index trigger_owner.conditioner_log_status_idx on trigger_owner.conditioner_log(status) tablespace trigger_indx;

create sequence trigger_owner.conditioner_promos_seq start with 1 increment by 1;

create table trigger_owner.conditioner_promos(
  id          number not null, 
  promo_code  varchar2(30) not null, 
  promo_value varchar2(30) not null, 
  promo_name  varchar2(60) not null, 
  service_id  varchar2(30) not null, 
  dt_created  date  not null, 
  created_by  varchar2(30) not null,
  constraint conditioner_promos_pk primary key (id) using index tablespace trigger_indx,
  constraint conditioner_promos_uk unique(promo_code,promo_value) using index tablespace trigger_indx
 ) tablespace trigger_data;

COMMENT ON COLUMN trigger_owner.conditioner_promos.id  IS 'use conditioner_promos_seq.nextval when doing an insert stmt';

create sequence trigger_owner.conditioner_keywords_seq start with 1 increment by 1;

create table trigger_owner.conditioner_keywords(
  id          number not null, 
  keyword     varchar2(30) not null, 
  sub_keyword varchar2(30) not null, 
  status      varchar2(12) default 'ACTIVE' not null,
  dt_created  date  not null, 
  created_by  varchar2(30) not null,
  constraint conditioner_keywords_pk primary key (id) using index tablespace trigger_indx,
  constraint conditioner_keywords_uk unique(keyword, sub_keyword) using index tablespace trigger_indx
 ) tablespace trigger_data;

COMMENT ON COLUMN trigger_owner.conditioner_keywords.id  IS 'use conditioner_promos_seq.nextval when doing an insert stmt';

create sequence trigger_owner.conditioner_coex_seq start with 1 increment by 1;

create table trigger_owner.conditioner_coex(
  id          number not null, 
  promo_code  varchar2(30) not null, 
  promo_value varchar2(30) not null, 
  allowed_promo_code  varchar2(30) not null, 
  allowed_promo_value varchar2(30) not null, 
  dt_created  date  not null, 
  created_by  varchar2(30) not null,
  constraint conditioner_coex_pk primary key (id) using index tablespace trigger_indx,
  constraint conditioner_coex_uk unique(promo_code, promo_value,allowed_promo_code,allowed_promo_value) using index tablespace trigger_indx
 ) tablespace trigger_data;

COMMENT ON COLUMN trigger_owner.conditioner_coex.id  IS 'use conditioner_matrix_seq.nextval when doing an insert stmt';

-- Enable SQLBLANKLINES to allow blank lines in SQL statements
SET SQLBLANKLINES ON;


-- New keywords and messages
insert into keywords (keyword, sub_keyword, status, created_by, dt_created, keyword_desc) values ('ROAM USURF', 'OFF'   , 'ACTIVE', user, sysdate, 'ROAM SURF de-activation');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, message_id, description) values ('ROAM USURF', 'ON'    , 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, 'ROAM_USURF_ON_INVALID_COEX', 'Notification for invalid co-existence of promos while subscriber is in the country');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, message_id, description) values ('ROAM USURF', 'OFF'   , 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, 'ROAM_USURF_OFF_PRE_ACT_SUCCESSFUL', 'Notification for opt out while subscriber is in the country');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, message_id, description) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, 'ROAM_USURF_STATUS_PRE_ACT_SUCCESSFUL', 'Notification for check status while subscriber is in the country');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, message_id, description) values ('ROAM USURF', 'OFF'   , 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, 'ROAM_USURF_OFF_WO_ACTIVE_ROAM', 'Notification for an opt-out but the subscriber is not yet an active roamer');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF1D_ON_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF3D_ON_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF5D_ON_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF299_ON_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF399_ON_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF499_ON_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_VIBER_ON_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_FACEBOOK_ON_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF1D_OFF_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF3D_OFF_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF5D_OFF_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF299_OFF_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF399_OFF_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF499_OFF_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_VIBER_OFF_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_FACEBOOK_OFF_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF1D_STATUS_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF3D_STATUS_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF5D_STATUS_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF299_STATUS_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF399_STATUS_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF499_STATUS_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_VIBER_STATUS_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_FACEBOOK_STATUS_SUCCESSFUL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF1D_ON_INVALID_COEX');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF3D_ON_INVALID_COEX');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF5D_ON_INVALID_COEX');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF299_ON_INVALID_COEX');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF399_ON_INVALID_COEX');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF499_ON_INVALID_COEX');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_VIBER_ON_INVALID_COEX');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_FACEBOOK_ON_INVALID_COEX');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF1D_OFF_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF3D_OFF_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF5D_OFF_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF299_OFF_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF399_OFF_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF499_OFF_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_VIBER_OFF_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'OFF', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_FACEBOOK_OFF_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF1D_STATUS_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF3D_STATUS_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF5D_STATUS_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF299_STATUS_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF399_STATUS_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_SURF499_STATUS_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_VIBER_STATUS_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_FACEBOOK_STATUS_INVALID');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ROAM_USURF_ON_INSUFF_BAL');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, '.', 'ARDS_USURF_ACTIVATED');
commit;

-- Existing keywords and messages
insert into keywords (keyword, sub_keyword, status, created_by, dt_created, keyword_desc) values ('ROAM USURF', 'ON'    , 'ACTIVE', user, sysdate, 'ROAM SURF activation');
insert into keywords (keyword, sub_keyword, status, created_by, dt_created, keyword_desc) values ('ROAM USURF', 'STATUS', 'ACTIVE', user, sysdate, 'ROAM SURF check status');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, message_id, description) values ('ROAM USURF', 'ON'    , 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, 'ROAM_USURF_ON_PRE_ACT_SUCCESSFUL', 'Notification for successful subscription queuing while in the country');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, message_id, description) values ('ROAM USURF', 'STATUS', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, 'ROAM_USURF_STATUS_WO_ACTIVE_ROAM', 'Notification for an check status but the subscriber is not yet an active roamer');
insert into keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, message_id, description) values ('ROAM USURF', 'ON'    , 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, 'ROAM_USURF_INVALID_KEYWORD', 'Notification for an invalid keyword');
commit;



UPDATE keyword_msg set msg_1='Thank you for registering to GLOBE PREPAID %s %s with unlimited data roaming, valid for 1 day. Your promo will be automatically activated once you arrive in your country of destination. Remember that you have to turn on your data roaming.
You will be charged when you''re already abroad. Make sure you have enough load to activate your data roaming promo subscription. 
If you''re an Android user, you can download GoRoam app on Google Play for free to monitor your usage while on roaming. Track and get real-time notifications on calls, texts and data consumption. That''s it. You''re all set to travel. Enjoy your stay abroad!' where message_id='ROAM_USURF_ON_PRE_ACT_SUCCESSFUL';

UPDATE keyword_msg set msg_1='Sorry, you have an active subscription to a promo that can''t be used with this promo. You can try to register again after your promo expires, or you may unsubscribe from your existing offer. To check your promo status, dial *143# for free. Thank you.' where message_id='ROAM_USURF_ON_INVALID_COEX';

UPDATE keyword_msg set msg_1='Your request to unsubscribe from GLOBE PREPAID %s %s has been successfully processed. There are no load deductions for registrations cancelled before data roaming is activated.
To register again, dial *143# for free, go to ROAMING & INT''L and choose from the data roaming offers. Make sure you have enough load for your chosen promo. If the registration is successful, your promo will be automatically activated once you arrive in your country of destination. Remember that you have to turn on your data roaming. Thank you!' where message_id='ROAM_USURF_OFF_PRE_ACT_SUCCESSFUL';

UPDATE keyword_msg set msg_1='Your promo will activate once you arrive in your country of destination so you can enjoy  GLOBE PREPAID %s %s for 36 hours.
You will be charged once you arrive abroad. Make sure you have enough load to activate and maintain roamingIf you''re on Android, download the GoRoam app on the Google Play Store so you can track and get real-time notifications on your calls, SMS and data usage for a worry-free travel!
That''s it! You''re all set for travel! Enjoy your stay abroad!' where message_id='ROAM_USURF_STATUS_PRE_ACT_SUCCESSFUL';

UPDATE keyword_msg set msg_1='You''re not subscribed to Roam UnliSurf. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_USURF_OFF_WO_ACTIVE_ROAM';
UPDATE keyword_msg set msg_1='You''re not subscribed to Roam UnliSurf. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_USURF_STATUS_WO_ACTIVE_ROAM';
UPDATE keyword_msg set msg_1='Sorry, the keyword you''ve entered is invalid keyword.Please make sure your keyword is correct with extra characters and spaces.For more info on promos, dial *143#,FREE from your Globe/TM mobile phone.' where message_id='ROAM_USURF_INVALID_KEYWORD';



UPDATE keyword_msg set msg_1='Stay connected while abroad with Globe Prepaid roaming! To activate the service, just reply GROAM YES within 12 hours. Within this period, all roaming charges will apply. Once roaming is active, mobile data (unless subscribed to Roam Surf), Duo/SuperDuo, promos, Share-a-Load, and ringback tone will be deactivated. Thank you.' where message_id='ARDS_ROAM_ACTIVATED';
UPDATE keyword_msg set msg_1='Your roaming is now active! While roaming is active, mobile data (unless subscribed to Roam Surf), Duo/SuperDuo, promos, Share-a-Load and ringback tone will be deactivated. For more info, visit www.globe.com.ph/roaming. Globe reserves the right to terminate your roaming without liability should it detect usage in excess of remaining load, fraud, or any kind of abuse. Once you''re back in the Philippines, you can deactivate your roaming by texting GROAM OFF to 2884. SuperDuo/Duo subscriptions can be reactivated by filling out a request form via www.globe.com.ph/talk2globenow. Thank you!' where message_id='GROAM_YES_SUCCESSFUL';

UPDATE keyword_msg set msg_1='Sorry, you don''t have enough load to register to this promo. You can reload by using a Globe prepaid call card or request for Share-a-Load from your loved ones.' where message_id='ROAM_USURF_ON_INSUFF_BAL';

UPDATE keyword_msg set msg_1='Your roaming is now active! While roaming, your local call, text and data promos (except Roam Surf) will be deactivated. For more info, visit www.globe.com.ph/roaming. Globe reserves the right to terminate your roaming without liability should it detect usage in excess of remaining load, fraud, or any kind of abuse. Once you''re back in the Philippines, stop your roaming by texting GROAM OFF to 2884. Thank you!' where message_id='ARDS_USURF_ACTIVATED';

commit;


update keyword_msg set msg_1 = 'Thank you for registering to Globe Prepaid Roam Surf 1 Day (P599). Your promo will apply and will be charged to your load once your number is detected to be outside the Philippines. 

Make sure you have enough load and your promo is available at your country of destination. Upon arrival, remember to turn on your mobile data to start surfing.

If you''re an Android user, you can download GoRoam from Google Play for worry-free usage while roaming. Track and get real-time notifications on calls, texts and data consumption. Enjoy your stay abroad!' where message_id='ROAM_SURF1D_ON_SUCCESSFUL';         

update keyword_msg set msg_1 = 'Thank you for registering to Globe Prepaid Roam Surf 3 Days (P1499). Your promo will apply and will be charged to your load once your number is detected to be outside the Philippines. 

Make sure you have enough load and your promo is available at your country of destination. Upon arrival, remember to turn on your mobile data to start surfing.

If you''re an Android user, you can download GoRoam from Google Play for worry-free usage while roaming. Track and get real-time notifications on calls, texts and data consumption. Enjoy your stay abroad!' where message_id='ROAM_SURF3D_ON_SUCCESSFUL';

update keyword_msg set msg_1 = 'Thank you for registering to Globe Prepaid Roam Surf 5 Days (P2299). Your promo will apply and will be charged to your load once your number is detected to be outside the Philippines. 

Make sure you have enough load and your promo is available at your country of destination. Upon arrival, remember to turn on your mobile data to start surfing.

If you''re an Android user, you can download GoRoam from Google Play for worry-free usage while roaming. Track and get real-time notifications on calls, texts and data consumption. Enjoy your stay abroad!' where message_id='ROAM_SURF5D_ON_SUCCESSFUL';

update keyword_msg set msg_1 = 'Thank you for registering to Globe Prepaid Roam Surf 299 (50MB), valid for 1 day. Your promo will apply and will be charged to your load once your number is detected to be outside the Philippines. 

Make sure you have enough load and your promo is available at your country of destination. Upon arrival, remember to turn on your mobile data to start surfing.

If you''re an Android user, you can download GoRoam from Google Play for worry-free usage while roaming. Track and get real-time notifications on calls, texts and data consumption. Enjoy your stay abroad!' where message_id='ROAM_SURF299_ON_SUCCESSFUL';

update keyword_msg set msg_1 = 'Thank you for registering to Globe Prepaid Roam Surf 399 (80MB), valid for 1 day. Your promo will apply and will be charged to your load once your number is detected to be outside the Philippines. 

Make sure you have enough load and your promo is available at your country of destination. Upon arrival, remember to turn on your mobile data to start surfing.

If you''re an Android user, you can download GoRoam from Google Play for worry-free usage while roaming. Track and get real-time notifications on calls, texts and data consumption. Enjoy your stay abroad!' where message_id='ROAM_SURF399_ON_SUCCESSFUL';

update keyword_msg set msg_1 = 'Thank you for registering to Globe Prepaid Roam Surf 499 (100MB), valid for 1 day. Your promo will apply and will be charged to your load once your number is detected to be outside the Philippines. 

Make sure you have enough load and your promo is available at your country of destination. Upon arrival, remember to turn on your mobile data to start surfing.

If you''re an Android user, you can download GoRoam from Google Play for worry-free usage while roaming. Track and get real-time notifications on calls, texts and data consumption. Enjoy your stay abroad!' where message_id='ROAM_SURF499_ON_SUCCESSFUL';

update keyword_msg set msg_1 = 'Thank you for registering to Globe Prepaid Roam Viber 199 with all-day access to Viber, valid for 1 day. Your promo will apply and will be charged to your load once your number is detected to be outside the Philippines. 

Make sure you have enough load and your promo is available at your country of destination. Upon arrival, remember to turn on your mobile data to start surfing.

If you''re an Android user, you can download GoRoam from Google Play for worry-free usage while roaming. Track and get real-time notifications on calls, texts and data consumption. Enjoy your stay abroad!' where message_id='ROAM_VIBER_ON_SUCCESSFUL';

update keyword_msg set msg_1 = 'Thank you for registering to Globe Prepaid Roam Facebook 299 with all-day access to Facebook, valid for 1 day. Your promo will apply and will be charged to your load once your number is detected to be outside the Philippines. 

Make sure you have enough load and your promo is available at your country of destination. Upon arrival, remember to turn on your mobile data to start surfing.

If you''re an Android user, you can download GoRoam from Google Play for worry-free usage while roaming. Track and get real-time notifications on calls, texts and data consumption. Enjoy your stay abroad!' where message_id='ROAM_FACEBOOK_ON_SUCCESSFUL';

update keyword_msg set msg_1 = 'You''ve unsubscribed from Globe Prepaid Roam Surf 1 Day (P599). There are no load deductions for registrations cancelled before the promo has been activated.

Just a friendly reminder, prepaid data roaming is only available if registered to a data roaming promo. To register again, dial *143# > Roaming & Int''l and choose from the available data roaming offers.

Make sure you have enough load for your subscription. Upon successful registration, turn on your mobile data to start surfing and activate your promo.' where message_id='ROAM_SURF1D_OFF_SUCCESSFUL';

update keyword_msg set msg_1 = 'You''ve unsubscribed from Globe Prepaid Roam Surf 3 Days (P1499). There are no load deductions for registrations cancelled before the promo has been activated.

Just a friendly reminder, prepaid data roaming is only available if registered to a data roaming promo. To register again, dial *143# > Roaming & Int''l and choose from the available data roaming offers.

Make sure you have enough load for your subscription. Upon successful registration, turn on your mobile data to start surfing and activate your promo.' where message_id='ROAM_SURF3D_OFF_SUCCESSFUL';

update keyword_msg set msg_1 = 'You''ve unsubscribed from Globe Prepaid Roam Surf 5 Days (P2299). There are no load deductions for registrations cancelled before the promo has been activated.

Just a friendly reminder, prepaid data roaming is only available if registered to a data roaming promo. To register again, dial *143# > Roaming & Int''l and choose from the available data roaming offers.

Make sure you have enough load for your subscription. Upon successful registration, turn on your mobile data to start surfing and activate your promo.' where message_id='ROAM_SURF5D_OFF_SUCCESSFUL';

update keyword_msg set msg_1 = 'You''ve unsubscribed from Globe Prepaid Roam Surf 299 (50MB). There are no load deductions for registrations cancelled before the promo has been activated.

Just a friendly reminder, prepaid data roaming is only available if registered to a data roaming promo. To register again, dial *143# > Roaming & Int''l and choose from the available data roaming offers.

Make sure you have enough load for your subscription. Upon successful registration, turn on your mobile data to start surfing and activate your promo.' where message_id='ROAM_SURF299_OFF_SUCCESSFUL';

update keyword_msg set msg_1 = 'You''ve unsubscribed from Globe Prepaid Roam Surf 399 (80MB). There are no load deductions for registrations cancelled before the promo has been activated.

Just a friendly reminder, prepaid data roaming is only available if registered to a data roaming promo. To register again, dial *143# > Roaming & Int''l and choose from the available data roaming offers.

Make sure you have enough load for your subscription. Upon successful registration, turn on your mobile data to start surfing and activate your promo.' where message_id='ROAM_SURF399_OFF_SUCCESSFUL';

update keyword_msg set msg_1 = 'You''ve unsubscribed from Globe Prepaid Roam Surf 499 (100MB). There are no load deductions for registrations cancelled before the promo has been activated.

Just a friendly reminder, prepaid data roaming is only available if registered to a data roaming promo. To register again, dial *143# > Roaming & Int''l and choose from the available data roaming offers.

Make sure you have enough load for your subscription. Upon successful registration, turn on your mobile data to start surfing and activate your promo.' where message_id='ROAM_SURF499_OFF_SUCCESSFUL';

update keyword_msg set msg_1 = 'You''ve unsubscribed from Globe Prepaid Roam Viber 199. There are no load deductions for registrations cancelled before the promo has been activated.

Just a friendly reminder, prepaid data roaming is only available if registered to a data roaming promo. To register again, dial *143# > Roaming & Int''l and choose from the available data roaming offers.

Make sure you have enough load for your subscription. Upon successful registration, turn on your mobile data to start surfing and activate your promo.' where message_id='ROAM_VIBER_OFF_SUCCESSFUL';

update keyword_msg set msg_1 = 'You''ve unsubscribed from Globe Prepaid Roam Facebook 299. There are no load deductions for registrations cancelled before the promo has been activated.

Just a friendly reminder, prepaid data roaming is only available if registered to a data roaming promo. To register again, dial *143# > Roaming & Int''l and choose from the available data roaming offers.

Make sure you have enough load for your subscription. Upon successful registration, turn on your mobile data to start surfing and activate your promo.' where message_id='ROAM_FACEBOOK_OFF_SUCCESSFUL';

update keyword_msg set msg_1 = 'Your promo will activate once you arrive in your country of destination so you can enjoy  GLOBE PREPAID ROAM SURF 1D for 24 hours.
You will be charged once you arrive abroad. Make sure you have enough load to activate and maintain roamingIf you''re on Android, download the GoRoam app on the Google Play Store so you can track and get real-time notifications on your calls, SMS and data usage for a worry-free travel!
That''s it! You''re all set for travel! Enjoy your stay abroad!' where message_id='ROAM_SURF1D_STATUS_SUCCESSFUL';

update keyword_msg set msg_1 = 'Your promo will activate once you arrive in your country of destination so you can enjoy  GLOBE PREPAID ROAM SURF 3D for 36 hours.
You will be charged once you arrive abroad. Make sure you have enough load to activate and maintain roamingIf you''re on Android, download the GoRoam app on the Google Play Store so you can track and get real-time notifications on your calls, SMS and data usage for a worry-free travel!
That''s it! You''re all set for travel! Enjoy your stay abroad!' where message_id='ROAM_SURF3D_STATUS_SUCCESSFUL';

update keyword_msg set msg_1 = 'Your promo will activate once you arrive in your country of destination so you can enjoy  GLOBE PREPAID ROAM SURF 5D for 120 hours.
You will be charged once you arrive abroad. Make sure you have enough load to activate and maintain roamingIf you''re on Android, download the GoRoam app on the Google Play Store so you can track and get real-time notifications on your calls, SMS and data usage for a worry-free travel!
That''s it! You''re all set for travel! Enjoy your stay abroad!' where message_id='ROAM_SURF5D_STATUS_SUCCESSFUL';

update keyword_msg set msg_1 = 'Your promo will activate once you arrive in your country of destination so you can enjoy  GLOBE PREPAID ROAM SURF 299 (50 MB) for 24 hours.
You will be charged once you arrive abroad. Make sure you have enough load to activate and maintain roamingIf you''re on Android, download the GoRoam app on the Google Play Store so you can track and get real-time notifications on your calls, SMS and data usage for a worry-free travel!
That''s it! You''re all set for travel! Enjoy your stay abroad!' where message_id='ROAM_SURF299_STATUS_SUCCESSFUL';

update keyword_msg set msg_1 = 'Your promo will activate once you arrive in your country of destination so you can enjoy  GLOBE PREPAID ROAM SURF 399 (80 MB) for 24 hours.
You will be charged once you arrive abroad. Make sure you have enough load to activate and maintain roamingIf you''re on Android, download the GoRoam app on the Google Play Store so you can track and get real-time notifications on your calls, SMS and data usage for a worry-free travel!
That''s it! You''re all set for travel! Enjoy your stay abroad!' where message_id='ROAM_SURF399_STATUS_SUCCESSFUL';

update keyword_msg set msg_1 = 'Your promo will activate once you arrive in your country of destination so you can enjoy  GLOBE PREPAID ROAM SURF 499 (100 MB) for 24 hours.
You will be charged once you arrive abroad. Make sure you have enough load to activate and maintain roamingIf you''re on Android, download the GoRoam app on the Google Play Store so you can track and get real-time notifications on your calls, SMS and data usage for a worry-free travel!
That''s it! You''re all set for travel! Enjoy your stay abroad!' where message_id='ROAM_SURF499_STATUS_SUCCESSFUL';

update keyword_msg set msg_1 = 'Your promo will activate once you arrive in your country of destination so you can enjoy  GLOBE PREPAID ROAM VIBER 199 for 24 hours.
You will be charged once you arrive abroad. Make sure you have enough load to activate and maintain roamingIf you''re on Android, download the GoRoam app on the Google Play Store so you can track and get real-time notifications on your calls, SMS and data usage for a worry-free travel!
That''s it! You''re all set for travel! Enjoy your stay abroad!' where message_id='ROAM_VIBER_STATUS_SUCCESSFUL';

update keyword_msg set msg_1 = 'Your promo will activate once you arrive in your country of destination so you can enjoy  GLOBE PREPAID ROAM FACEBOOK 299 for 24 hours.
You will be charged once you arrive abroad. Make sure you have enough load to activate and maintain roamingIf you''re on Android, download the GoRoam app on the Google Play Store so you can track and get real-time notifications on your calls, SMS and data usage for a worry-free travel!
That''s it! You''re all set for travel! Enjoy your stay abroad!' where message_id='ROAM_FACEBOOK_STATUS_SUCCESSFUL';

update keyword_msg set msg_1 = 'Sorry, you have an active promo subscription that can''t be used with this promo. You can try to register again after your promo expires, or you may unsubscribe from your existing offer. To check your promo status, dial *143# for free. Thank you.' where message_id='ROAM_SURF1D_ON_INVALID_COEX';

update keyword_msg set msg_1 = 'Sorry, you have an active promo subscription that can''t be used with this promo. You can try to register again after your promo expires, or you may unsubscribe from your existing offer. To check your promo status, dial *143# for free. Thank you.' where message_id='ROAM_SURF3D_ON_INVALID_COEX';

update keyword_msg set msg_1 = 'Sorry, you have an active promo subscription that can''t be used with this promo. You can try to register again after your promo expires, or you may unsubscribe from your existing offer. To check your promo status, dial *143# for free. Thank you.' where message_id='ROAM_SURF5D_ON_INVALID_COEX';

update keyword_msg set msg_1 = 'Sorry, you have an active promo subscription that can''t be used with this promo. You can try to register again after your promo expires, or you may unsubscribe from your existing offer. To check your promo status, dial *143# for free. Thank you.' where message_id='ROAM_SURF299_ON_INVALID_COEX';

update keyword_msg set msg_1 = 'Sorry, you have an active promo subscription that can''t be used with this promo. You can try to register again after your promo expires, or you may unsubscribe from your existing offer. To check your promo status, dial *143# for free. Thank you.' where message_id='ROAM_SURF399_ON_INVALID_COEX';

update keyword_msg set msg_1 = 'Sorry, you have an active promo subscription that can''t be used with this promo. You can try to register again after your promo expires, or you may unsubscribe from your existing offer. To check your promo status, dial *143# for free. Thank you.' where message_id='ROAM_SURF499_ON_INVALID_COEX';

update keyword_msg set msg_1 = 'Sorry, you have an active promo subscription that can''t be used with this promo. You can try to register again after your promo expires, or you may unsubscribe from your existing offer. To check your promo status, dial *143# for free. Thank you.' where message_id='ROAM_VIBER_ON_INVALID_COEX';

update keyword_msg set msg_1 = 'Sorry, you have an active promo subscription that can''t be used with this promo. You can try to register again after your promo expires, or you may unsubscribe from your existing offer. To check your promo status, dial *143# for free. Thank you.' where message_id='ROAM_FACEBOOK_ON_INVALID_COEX';

update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 1 Day (P599). To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF1D_OFF_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 3 Days (P1499). To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF3D_OFF_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 5 Days (P2299). To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF5D_OFF_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 299. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF299_OFF_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 399. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF399_OFF_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 499. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF499_OFF_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM VIBER 199. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_VIBER_OFF_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM FACEBOOK 299. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_FACEBOOK_OFF_INVALID';

update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 1 Day (P599). To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF1D_STATUS_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 3 Days (P1499). To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF3D_STATUS_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 5 Days (P2299). To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF5D_STATUS_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 299. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF299_STATUS_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 399. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF399_STATUS_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM SURF 499. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_SURF499_STATUS_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM VIBER 199. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_VIBER_STATUS_INVALID';
update keyword_msg set msg_1 = 'You''re not subscribed to GLOBE PREPAID ROAM FACEBOOK 299. To register, dial *143# toll-free even when roaming.' where message_id='ROAM_FACEBOOK_STATUS_INVALID';





-- KEYWORDS
insert into conditioner_keywords (id, keyword, sub_keyword, created_by, dt_created) values (conditioner_keywords_seq.nextval, 'SURF', '1D', user, sysdate);
insert into conditioner_keywords (id, keyword, sub_keyword, created_by, dt_created) values (conditioner_keywords_seq.nextval, 'SURF', '3D', user, sysdate);
insert into conditioner_keywords (id, keyword, sub_keyword, created_by, dt_created) values (conditioner_keywords_seq.nextval, 'SURF', '5D', user, sysdate);
insert into conditioner_keywords (id, keyword, sub_keyword, created_by, dt_created) values (conditioner_keywords_seq.nextval, 'SURF', '299', user, sysdate);
insert into conditioner_keywords (id, keyword, sub_keyword, created_by, dt_created) values (conditioner_keywords_seq.nextval, 'SURF', '399', user, sysdate);
insert into conditioner_keywords (id, keyword, sub_keyword, created_by, dt_created) values (conditioner_keywords_seq.nextval, 'SURF', '499', user, sysdate);
insert into conditioner_keywords (id, keyword, sub_keyword, created_by, dt_created) values (conditioner_keywords_seq.nextval, 'FACEBOOK', '299', user, sysdate);
insert into conditioner_keywords (id, keyword, sub_keyword, created_by, dt_created) values (conditioner_keywords_seq.nextval, 'VIBER', '199', user, sysdate);
commit;

-- PROMOS
insert into conditioner_promos (id, promo_code, promo_value, promo_name, service_id, created_by, dt_created) values (conditioner_promos_seq.nextval, 'SURF', '1D',  'SURF', '1333', user, sysdate);
insert into conditioner_promos (id, promo_code, promo_value, promo_name, service_id, created_by, dt_created) values (conditioner_promos_seq.nextval, 'SURF', '3D',  'SURF', '1334', user, sysdate);
insert into conditioner_promos (id, promo_code, promo_value, promo_name, service_id, created_by, dt_created) values (conditioner_promos_seq.nextval, 'SURF', '5D',  'SURF', '1335', user, sysdate);
insert into conditioner_promos (id, promo_code, promo_value, promo_name, service_id, created_by, dt_created) values (conditioner_promos_seq.nextval, 'SURF', '299', 'SURF', '1336', user, sysdate);
insert into conditioner_promos (id, promo_code, promo_value, promo_name, service_id, created_by, dt_created) values (conditioner_promos_seq.nextval, 'SURF', '399', 'SURF', '1337', user, sysdate);
insert into conditioner_promos (id, promo_code, promo_value, promo_name, service_id, created_by, dt_created) values (conditioner_promos_seq.nextval, 'SURF', '499', 'SURF', '1338', user, sysdate);
insert into conditioner_promos (id, promo_code, promo_value, promo_name, service_id, created_by, dt_created) values (conditioner_promos_seq.nextval, 'FACEBOOK', '299', 'FACEBOOK', '1339', user, sysdate);
insert into conditioner_promos (id, promo_code, promo_value, promo_name, service_id, created_by, dt_created) values (conditioner_promos_seq.nextval, 'VIBER', '199', 'VIBER', '1340', user, sysdate);
commit;

-- COEX
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '1D',  'SURF', '3D', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '1D',  'SURF', '5D', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '3D',  'SURF', '1D', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '3D',  'SURF', '5D', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '5D',  'SURF', '1D', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '5D',  'SURF', '3D', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '299', 'SURF', '399', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '299', 'SURF', '499', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '299', 'FACEBOOK', '299', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '299', 'VIBER', '199', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '399', 'SURF', '299', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '399', 'SURF', '499', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '399', 'FACEBOOK', '299', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '399', 'VIBER', '199', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '499', 'SURF', '299', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '499', 'SURF', '399', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '499', 'FACEBOOK', '299', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'SURF', '499', 'VIBER', '199', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'FACEBOOK', '299', 'SURF', '299', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'FACEBOOK', '299', 'SURF', '399', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'FACEBOOK', '299', 'SURF', '499', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'FACEBOOK', '299', 'VIBER', '199', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'VIBER', '199', 'SURF', '299', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'VIBER', '199', 'SURF', '399', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'VIBER', '199', 'SURF', '499', user, sysdate);
insert into conditioner_coex (id, promo_code, promo_value, allowed_promo_code, allowed_promo_value, created_by, dt_created) values (conditioner_coex_seq.nextval, 'VIBER', '199', 'FACEBOOK', '299', user, sysdate);
commit;

-- Disable SQLBLANKLINES to allow blank lines in SQL statements
SET SQLBLANKLINES OFF;