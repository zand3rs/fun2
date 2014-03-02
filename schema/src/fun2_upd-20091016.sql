alter table manual_enrollment_log add silent number(1) default 0 not null;
alter table request_log add silent number(1) default 0 not null;

create table broadcast_log (
 id number not null,
 msisdn number(12) not null,
 message_id varchar2(60) not null,
 tx_date date not null,
 status number(1) default 0 not null,
 constraint broadcast_log_pk primary key (id),
 constraint broadcast_log_uk unique (msisdn, tx_date, message_id)
);

alter table broadcast_log add cluster_node number(3) default 1 not null;
alter table geo_probe_log add cluster_node number(3) default 1 not null;

