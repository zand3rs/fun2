drop table dailybal_log cascade constraints;

create table dailybal_log (
   id number not null ,
   msisdn varchar2(16) not null,
   tx_date date not null,
   status number default 0 not null,
   cluster_node number(3) default 1 not null,
   daily_balance number(4) not null,
   last_daily_bal_ctr number(2) not null,
   CONSTRAINT "dailybal_log_PK" PRIMARY KEY ("ID"),
   CONSTRAINT "dailybal_log_UK" UNIQUE ("MSISDN", "TX_DATE")
);
create sequence dailybal_log_seq start with 1 increment by 1 nocache order;


drop table ods_log cascade constraints;

create table ods_log (
  id number not null,
  msisdn varchar2(16) not null,
  status number default 0 not null,
  cluster_node number(3) default 1 not null,
  tx_date date,
  dt_created date,
  CONSTRAINT "ods_log_PK" PRIMARY KEY ("ID")
);
create sequence ods_log_seq start with 1 increment by 1 nocache order;

