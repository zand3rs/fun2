alter table broadcast_log add msg varchar2(2000);

create table ccb_log (
  id number not null,
  msisdn varchar2(16) not null,
  ccb_action varchar2(8) not null,
  ccb_status varchar2(8),
  status number default 0 not null,
  cluster_node number(3) default 1 not null,
  tx_date date,
  dt_created date,
  CONSTRAINT "ccb_log_PK" PRIMARY KEY ("ID")
);
create sequence ccb_log_seq start with 1 increment by 1 nocache order;

