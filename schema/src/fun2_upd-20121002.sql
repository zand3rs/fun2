-- create table act_deact_log (
--    id number not null,
--    ref_id number not null,
--    msisdn number(12) not null,
--    status number(4) default 0 not null,
--    tran_type number(4) not null,
--    tran_date date not null,
--    bcast_date date,
--    cluster_node number(3) default 1 not null,
--    constraint act_deact_log_pk primary key (id)
-- );
-- 
-- create index act_deact_log_idx on act_deact_log(status);

-- select msisdn, tran_type, tran_date
-- from   act_deact_log
-- where  status = 0
-- and     bcast_date <= sysdate;

drop table notification_log cascade constraints;
create table notification_log (
   id number not null,
   ref_id number not null,
   msisdn varchar2(12) not null,
   status number(4) default 0 not null,
   tran_type number(4) not null,
   tran_date date not null,
   dt_created date,
   dt_modified date,
   cluster_node number(3) default 1 not null,
   constraint notification_log_pk primary key (id)
);

create index notification_log_idx on notification_log(status);

drop sequence notification_log_seq;
create sequence notification_log_seq start with 1 increment by 1 nocache order;

