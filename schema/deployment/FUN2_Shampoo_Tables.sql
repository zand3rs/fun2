create table trigger_owner.shampoo_log (
  id              number not null, 
  msisdn          varchar2(12) not null, 
  plan            varchar2(60) not null, 
  start_date      date, 
  end_date        date,
  dt_created      date  not null, 
  created_by      varchar2(30) not null,
  constraint shampoo_log_pk primary key (id),
  constraint shampoo_log_uk unique (msisdn,plan)
 );
 
create sequence trigger_owner.shampoo_seq start with 1 increment by 1;

