alter table request_log add imsi varchar2(32);
alter table request_log add activation_date varchar2(32);
alter table request_log add deactivation_date varchar2(32);
alter table request_log add duration number(3);
alter table request_log add gsm_num varchar2(20);

drop table geo_probe_log cascade constraints;
create table geo_probe_log (
  id number not null,
  imsi varchar2(30),
  sccp varchar2(30),
  status number default 0 not null,
  tx_date date,
  dt_created date,
  CONSTRAINT "GEO_PROBE_LOG_PK" PRIMARY KEY ("ID")
);

CREATE SEQUENCE  "GEO_PROBE_LOG_SEQ"  MINVALUE 1 MAXVALUE 999999999999999999999999999 INCREMENT BY 1 START WITH 1 NOCACHE  ORDER  NOCYCLE;
