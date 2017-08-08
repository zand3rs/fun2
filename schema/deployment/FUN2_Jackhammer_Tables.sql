create sequence trigger_owner.jackhammer_msisdn_mapping_seq start with 1 increment by 1;
create sequence trigger_owner.jackhammer_mnc_mapping_seq start with 1 increment by 1;

create table trigger_owner.jackhammer_msisdn_mapping (
  id          number not null, 
  msisdn_fr   number not null, 
  msisdn_to   number not null, 
  status      varchar2(12) default 'ACTIVE' not null,
  dt_created  date  not null,
  created_by  varchar2(30) not null,
  dt_modified  date,
  modified_by  varchar2(30),
  constraint jk_msisdn_mapping_pk primary key (id) using index tablespace trigger_indx,
  constraint jk_msisdn_mapping_uk unique (msisdn_fr,msisdn_to) using index tablespace trigger_indx
 ) tablespace trigger_data;

create table trigger_owner.jackhammer_mnc_mapping (
  id          number not null, 
  mcc         varchar2(30) not null,
  mnc         varchar2(30) not null,
  status      varchar2(12) default 'ACTIVE' not null,
  dt_created  date  not null,
  created_by  varchar2(30) not null,
  dt_modified  date,
  modified_by  varchar2(30),
  constraint jk_msisdn_mnc_pk primary key (id) using index tablespace trigger_indx,
  constraint jk_msisdn_mnc_uk unique (mcc,mnc) using index tablespace trigger_indx
 ) tablespace trigger_data;

-- New keywords and messages
insert into trigger_owner.keyword_msg (keyword, sub_keyword, customer_type, message_type, msg_1, status, created_by, dt_created, description, message_id) values ('ROAM USURF', 'ON', 'TRIGGER', 'ALERT', '.', 'ACTIVE', user, sysdate, 'ROAM FREE CT Broadcast for Jackhammer', 'BROADCAST_ROAM_FREE_CT');
update trigger_owner.keyword_msg set msg_1='Hi. Your mobile internet usage is now covered by Roam Surf. Enjoy 24 hours of data usage for only P599. And, to make your trip even more worry-free, you are now entitled to avail of free 30 minute voice calls and unlimited SMS by simply dialing *143# and selecting Roam Free Plus! Enjoy this offer for 24 hours.  Make sure to stay connected to our roaming partner.  Enjoy your trip!' where message_id='BROADCAST_ROAM_FREE_CT';


-- MCC - MNC for Jackhammer Roaming Partners
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '505', '02', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '454', '00', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '510', '10', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '404', '10', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '450', '05', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '502', '12', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '427', '01', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '420', '01', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '525', '01', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '525', '02', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '466', '97', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '520', '01', 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_mnc_mapping (id, mcc, mnc, status, dt_created, created_by) values (trigger_owner.jackhammer_mnc_mapping_seq.nextval, '424', '03', 'ACTIVE', sysdate, user);
commit;


-- MSISDN Whitelisting for Jackhammer
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639178945371, 639178945371, 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639178945330, 639178945330, 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639171492289, 639171492289, 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639171492352, 639171492352, 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639171490592, 639171490592, 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639178945342, 639178945342, 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639178945344, 639178945344, 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639178945347, 639178945347, 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639178942480, 639178942480, 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639178943276, 639178943276, 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639178943919, 639178943919, 'ACTIVE', sysdate, user);
insert into trigger_owner.jackhammer_msisdn_mapping (id, msisdn_fr, msisdn_to, status, dt_created, created_by) values (trigger_owner.jackhammer_msisdn_mapping_seq.nextval, 639178943981, 639178943981, 'ACTIVE', sysdate, user);
